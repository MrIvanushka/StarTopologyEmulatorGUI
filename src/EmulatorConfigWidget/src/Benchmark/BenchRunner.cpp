#include "BenchRunner.h"

#include <random>

#include <QDir>
#include <QFile>
#include <QTextStream>

#pragma push_macro("emit")
#undef emit
#include <StarTopologyEmulator/BacklogAccumulator/BacklogAccumulatorFactory.h>
#include <StarTopologyEmulator/DynamicFrameSettingsFactory.h>
#include <StarTopologyEmulator/EmulatorFactory.h>
#include <StarTopologyEmulator/FrameCalculatorFactory.h>
#include <StarTopologyEmulator/IncomeLoadEstimator/IncomeLoadEstimatorFactory.h>
#include <StarTopologyEmulator/Metrics/MetricSink.h>
#include <StarTopologyEmulator/Metrics/MetricSinkFactory.h>
#include <StarTopologyEmulator/Scenario/ScenarioRunnerFactory.h>
#include <StarTopologyEmulator/StarHubStrategy/IncomeStationsPredictor/IncomeStationsPredictorFactory.h>
#include <StarTopologyEmulator/StarHubStrategy/StarHubStrategyFactory.h>
#include <StarTopologyEmulator/Stations/StarHubFactory.h>
#include <StarTopologyEmulator/Stations/StarStationFactory.h>
#pragma pop_macro("emit")

#include "../Scenario/ScenarioJson.h"
#include "ParamRegistry.h"

using namespace starTopologyEmulator;

namespace
{

std::shared_ptr<IIncomeLoadEstimator> makeEstimator(const BenchBaseConfig& cfg, MetricScope scope)
{
	switch (cfg.estimatorKind)
	{
	case BenchEstimatorKind::Kalman:
		return IncomeLoadEstimatorFactory::make(cfg.kalmanConfig, std::move(scope));
	case BenchEstimatorKind::Ema:
	default:
		return IncomeLoadEstimatorFactory::make(cfg.emaConfig, std::move(scope));
	}
}

std::shared_ptr<IIncomeStationsPredictor> makePredictor(
	const BenchBaseConfig& cfg,
	std::shared_ptr<IIncomeLoadEstimator> estimator,
	std::shared_ptr<IDynamicFrameSettings> dfs,
	std::shared_ptr<IFrameCalculator> frameCalc,
	MetricScope scope)
{
	switch (cfg.predictorKind)
	{
	case BenchPredictorKind::Cogorthy:
		return IncomeStationsPredictorFactory::make(estimator, dfs, std::move(scope));
	case BenchPredictorKind::BackoffAwareCogorthy:
		return IncomeStationsPredictorFactory::make(
			estimator, dfs,
			BackoffAwareCogorthyIncomeStationsPredictorConfig{},
			std::move(scope));
	case BenchPredictorKind::GreyModel:
		return IncomeStationsPredictorFactory::make(
			estimator, dfs,
			GreyModelIncomeStationsPredictorConfig(cfg.greyModelConfig),
			std::move(scope));
	case BenchPredictorKind::LinearRegression:
	default:
		return IncomeStationsPredictorFactory::make(
			estimator, dfs, frameCalc,
			LinearRegressionIncomeStationsPredictorConfig(cfg.linearRegressionConfig),
			std::move(scope));
	}
}

void writeCsv(const QString& path, IMetricSink& sink, std::uint64_t lastFrame)
{
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
		return;

	QTextStream out(&file);

	const auto paths = sink.paths();
	std::vector<MetricHandle> handles;
	handles.reserve(paths.size());
	for (const auto& p : paths)
		handles.push_back(sink.find(p));

	out << "frame";
	for (const auto& p : paths)
	{
		out << ",\"";
		const auto qp = QString::fromStdString(p);
		QString escaped = qp;
		escaped.replace('"', QStringLiteral("\"\""));
		out << escaped << "\"";
	}
	out << "\n";

	std::vector<std::vector<MetricSample>> series;
	series.reserve(handles.size());
	for (auto h : handles)
		series.push_back(sink.series(h));

	std::vector<std::size_t> cursor(handles.size(), 0);

	for (std::uint64_t f = 0; f <= lastFrame; ++f)
	{
		bool anyHasFrame = false;
		for (std::size_t i = 0; i < series.size(); ++i)
		{
			if (cursor[i] < series[i].size() && series[i][cursor[i]].frame == f)
			{
				anyHasFrame = true;
				break;
			}
		}
		if (!anyHasFrame) continue;

		out << f;
		for (std::size_t i = 0; i < series.size(); ++i)
		{
			out << ",";
			if (cursor[i] < series[i].size() && series[i][cursor[i]].frame == f)
			{
				out << series[i][cursor[i]].value;
				++cursor[i];
			}
		}
		out << "\n";
	}
}

}

BenchRunner::RunResult BenchRunner::run(
	const BenchRun& run,
	const BenchBaseConfig& base,
	const QString& outputDir)
{
	RunResult res;
	const QString fileName = QStringLiteral("run_%1.csv").arg(run.index, 5, 10, QLatin1Char('0'));
	res.csvPath = QDir(outputDir).absoluteFilePath(fileName);

	try
	{
		FrameConfig frameConfig;
		frameConfig.slotDuration     = base.slotDuration;
		frameConfig.slotCountInFrame = static_cast<std::uint64_t>(base.slotCountInFrame);
		frameConfig.epoch            = 0;
		frameConfig.bitsPerSlot      = static_cast<std::uint64_t>(base.bitsPerSlot);

		auto metricSink = MetricSinkFactory::make();

		std::mt19937 rng(static_cast<std::mt19937::result_type>(base.seed));

		const std::string hubScopeName       = QString::fromLocal8Bit("Хаб").toUtf8().toStdString();
		const std::string strategyScopeName  = QString::fromLocal8Bit("Стратегия").toUtf8().toStdString();
		const std::string estimatorScopeName = QString::fromLocal8Bit("Оценка входной нагрузки").toUtf8().toStdString();
		const std::string predictorScopeName = QString::fromLocal8Bit("Оценка числа абонентов").toUtf8().toStdString();
		const std::string controllerScopeName= QString::fromLocal8Bit("Контроль нагрузки").toUtf8().toStdString();

		const BenchBaseConfig baseCopy = base;
		const BenchRun runCopy = run;

		EmulatorInitData initData;
		initData.metricSink     = metricSink;
		initData.stationCount   = baseCopy.numStations;
		initData.abonentFrameCalculator = FrameCalculatorFactory::hubCalculator(frameConfig, baseCopy.stationTts);
		initData.hubFrameCalculator = FrameCalculatorFactory::hubCalculator(frameConfig, baseCopy.hubTts);

		initData.stationFactory = [&](auto sendFunc, auto id, std::shared_ptr<IStationStatsCollector> stats) {
			return StarStationFactory::make(StarStationInitData({
				.sendFunc = sendFunc,
				.frameCalculator = FrameCalculatorFactory::abonentCalculator(frameConfig, baseCopy.stationTts),
				.dynamicFrameSettings = DynamicFrameSettingsFactory::make(),
				.id = id,
				.messagesNeeded = baseCopy.messagesNeeded,
				.tts = baseCopy.stationTts,
				.rng = rng,
				.statsCollector = std::move(stats),
			}));
		};

		initData.hubFactory = [&, hubScopeName, strategyScopeName, estimatorScopeName, predictorScopeName, controllerScopeName](auto sendFunc) {
			MetricScope hubScope(metricSink, hubScopeName);
			MetricScope strategyScope  = hubScope.child(strategyScopeName);
			MetricScope estimatorScope = hubScope.child(estimatorScopeName);
			MetricScope predictorScope = strategyScope.child(predictorScopeName);
			MetricScope controllerScope= strategyScope.child(controllerScopeName);

			auto incomeLoadEstimator = std::shared_ptr<IIncomeLoadEstimator>(
				makeEstimator(baseCopy, estimatorScope));

			auto hubFrameCalculator = std::shared_ptr<IFrameCalculator>(
				FrameCalculatorFactory::hubCalculator(frameConfig, baseCopy.hubTts));

			auto hubDfs = std::shared_ptr<IDynamicFrameSettings>(
				DynamicFrameSettingsFactory::make());

			auto predictor = makePredictor(
				baseCopy, incomeLoadEstimator, hubDfs, hubFrameCalculator, predictorScope);

			auto backlogAccumulator = std::shared_ptr<IBacklogAccumulator>(
				BacklogAccumulatorFactory::make(frameConfig.bitsPerSlot));

			auto ftpGen = ParamRegistry::makeFtpGenerator(
				runCopy.ftpKind, runCopy.ftpParams,
				hubDfs, predictor, backlogAccumulator, hubFrameCalculator);

			auto controller = ParamRegistry::makeController(
				runCopy.controllerKind, runCopy.controllerParams,
				hubDfs, predictor, controllerScope);

			auto strategy = StarHubStrategyFactory::make(
				std::move(ftpGen), std::move(controller), strategyScope);

			return StarHubFactory::make(StarHubInitData({
				.sendFunc = sendFunc,
				.incomeLoadEstimator = incomeLoadEstimator,
				.frameCalculator = hubFrameCalculator,
				.dynamicFrameSettings = hubDfs,
				.strategy = std::move(strategy),
				.backlogAccumulator = std::move(backlogAccumulator),
				.tts = baseCopy.hubTts,
				.metricScope = hubScope,
			}));
		};

		auto emu = std::shared_ptr<IEmulator>(EmulatorFactory::make(std::move(initData)));
		auto hub = emu->hub();
		auto stations = emu->stations();

		std::unique_ptr<IScenarioRunner> scenarioRunner;
		if (!run.scenario.empty())
		{
			scenarioRunner = ScenarioRunnerFactory::make(emu);
			scenarioRunner->load(ScenarioJson::buildEvents(run.scenario));
			scenarioRunner->prepare();
		}

		for (Timestamp t = 0; t < base.duration; ++t)
		{
			emu->update(t);
			if (scenarioRunner)
				scenarioRunner->update(t);
			hub->update(t);
			for (auto& s : stations)
				s->update(t);
		}

		const std::uint64_t lastFrame =
			static_cast<std::uint64_t>(base.duration / base.slotCountInFrame) + 1;

		QDir().mkpath(outputDir);
		writeCsv(res.csvPath, *metricSink, lastFrame);

		res.ok = true;
	}
	catch (const std::exception& ex)
	{
		res.error = QString::fromUtf8(ex.what());
	}

	return res;
}
