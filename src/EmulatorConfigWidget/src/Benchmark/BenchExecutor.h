#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <vector>

#include <QObject>
#include <QThreadPool>

#include "BenchPlan.h"

class BenchExecutor : public QObject
{
	Q_OBJECT
public:
	explicit BenchExecutor(QObject* parent = nullptr);
	~BenchExecutor();

	void start(const BenchConfig&, std::vector<BenchRun> plan);
	void cancel();
	bool running() const;

	struct State
	{
		BenchConfig             config;
		std::vector<BenchRun>   plan;
		std::atomic<int>        completed{ 0 };
		std::atomic<int>        cancelled{ 0 };
		std::atomic<int>        active{ 0 };
		std::mutex              indexMutex;
		QString                 indexPath;
	};

signals:
	void runStarted(int runIndex);
	void runFinished(int runIndex, bool ok, const QString& csvPath, const QString& error);
	void progress(int completed, int total);
	void finished();

private:
	void writeIndexHeader(const std::vector<BenchRun>&);

	std::shared_ptr<State> _state;
	QThreadPool*           _pool = nullptr;
};
