#include "BenchExecutor.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QRunnable>
#include <QTextStream>

#include "BenchJson.h"
#include "BenchRunner.h"

namespace
{

class RunTask : public QRunnable
{
public:
	RunTask(std::weak_ptr<BenchExecutor::State> state, BenchExecutor* executor, int planIndex)
		: _state(std::move(state)), _executor(executor), _planIndex(planIndex)
	{
		setAutoDelete(true);
	}

	void run() override
	{
		auto state = _state.lock();
		if (!state)
			return;

		if (state->cancelled.load())
		{
			state->completed.fetch_add(1);
			QMetaObject::invokeMethod(_executor, [exec = _executor, total = static_cast<int>(state->plan.size()),
				done = state->completed.load()] {
					emit exec->progress(done, total);
				}, Qt::QueuedConnection);
			return;
		}

		const BenchRun& runDescriptor = state->plan[_planIndex];

		QMetaObject::invokeMethod(_executor, [exec = _executor, idx = runDescriptor.index] {
			emit exec->runStarted(idx);
		}, Qt::QueuedConnection);

		auto result = BenchRunner::run(runDescriptor, state->config.base, state->config.outputDir);

		{
			std::lock_guard<std::mutex> lock(state->indexMutex);
			QFile f(state->indexPath);
			if (f.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
			{
				QTextStream out(&f);
				out << runDescriptor.index << ","
					<< "\"" << runDescriptor.ftpKind << "\","
					<< "\"" << runDescriptor.controllerKind << "\","
					<< (result.ok ? "ok" : "fail") << ","
					<< "\"" << QFileInfo(result.csvPath).fileName() << "\"";
				for (auto it = runDescriptor.sweptParams.constBegin();
					it != runDescriptor.sweptParams.constEnd(); ++it)
				{
					out << ",\"" << it.key() << "=" << it.value().toString() << "\"";
				}
				if (!result.ok)
					out << ",\"" << result.error.toUtf8().replace('"', "\"\"") << "\"";
				out << "\n";
			}
		}

		const int total = static_cast<int>(state->plan.size());
		const int done  = state->completed.fetch_add(1) + 1;
		const int activeAfter = state->active.fetch_sub(1) - 1;

		QMetaObject::invokeMethod(_executor,
			[exec = _executor, idx = runDescriptor.index, ok = result.ok,
			 csv = result.csvPath, err = result.error, done, total, activeAfter] {
				emit exec->runFinished(idx, ok, csv, err);
				emit exec->progress(done, total);
				if (done == total && activeAfter == 0)
					emit exec->finished();
			}, Qt::QueuedConnection);
	}

private:
	std::weak_ptr<BenchExecutor::State> _state;
	BenchExecutor*                      _executor;
	int                                 _planIndex;
};

}

BenchExecutor::BenchExecutor(QObject* parent)
	: QObject(parent), _pool(new QThreadPool(this))
{
}

BenchExecutor::~BenchExecutor()
{
	cancel();
	if (_pool)
		_pool->waitForDone();
}

bool BenchExecutor::running() const
{
	return _state && _state->completed.load() < static_cast<int>(_state->plan.size());
}

void BenchExecutor::cancel()
{
	if (_state)
		_state->cancelled.store(1);
}

void BenchExecutor::writeIndexHeader(const std::vector<BenchRun>& plan)
{
	QFile f(_state->indexPath);
	if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
		return;
	QTextStream out(&f);
	out << "run_id,ftp_kind,controller_kind,status,csv_file";
	if (!plan.empty())
	{
		for (auto it = plan.front().sweptParams.constBegin();
			it != plan.front().sweptParams.constEnd(); ++it)
		{
			out << ",\"" << it.key() << "\"";
		}
	}
	out << ",error\n";
}

void BenchExecutor::start(const BenchConfig& config, std::vector<BenchRun> plan)
{
	if (running())
		return;

	_state = std::make_shared<State>();
	_state->config = config;
	_state->plan   = std::move(plan);

	QDir().mkpath(config.outputDir);

	_state->indexPath = QDir(config.outputDir).absoluteFilePath("runs.csv");
	writeIndexHeader(_state->plan);

	{
		QFile cfgFile(QDir(config.outputDir).absoluteFilePath("config.json"));
		if (cfgFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
			cfgFile.write(BenchJson::toJson(config).toJson(QJsonDocument::Indented));
	}

	const int threadCount = config.threadCount > 0
		? config.threadCount
		: QThread::idealThreadCount();
	_pool->setMaxThreadCount(threadCount);

	const int total = static_cast<int>(_state->plan.size());

	if (total == 0)
	{
		QMetaObject::invokeMethod(this, [this] {
			emit progress(0, 0);
			emit finished();
		}, Qt::QueuedConnection);
		return;
	}

	for (int i = 0; i < total; ++i)
	{
		_state->active.fetch_add(1);
		_pool->start(new RunTask(_state, this, i));
	}
}
