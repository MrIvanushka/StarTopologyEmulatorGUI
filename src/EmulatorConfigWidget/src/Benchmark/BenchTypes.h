#pragma once

#include <cstdint>
#include <variant>
#include <vector>

#include <QHash>
#include <QString>
#include <QVariant>

#pragma push_macro("emit")
#undef emit
#include <StarTopologyEmulator/IncomeLoadEstimator/EmaIncomeLoadEstimatorConfig.h>
#include <StarTopologyEmulator/IncomeLoadEstimator/KalmanIncomeLoadEstimatorConfig.h>
#include <StarTopologyEmulator/StarHubStrategy/IncomeStationsPredictor/GreyModelIncomeStationsPredictorConfig.h>
#include <StarTopologyEmulator/StarHubStrategy/IncomeStationsPredictor/LinearRegressionIncomeStationsPredictorConfig.h>
#pragma pop_macro("emit")

#include "../Scenario/ScenarioEventData.h"

struct SweepRange
{
	double min = 0.0;
	double max = 0.0;
	double step = 1.0;

	bool isValid() const { return step > 0.0 && min <= max; }
	int  pointCount() const;
};

using ParamValue = std::variant<double, int, bool, SweepRange>;

struct ImplConfig
{
	QString kind;
	QHash<QString, ParamValue> params;
};

enum class BenchEstimatorKind { Ema, Kalman };
enum class BenchPredictorKind { LinearRegression, Cogorthy, BackoffAwareCogorthy, GreyModel };

struct BenchBaseConfig
{
	int      numStations      = 50;
	int      duration         = 10000;
	int      messagesNeeded   = 1;
	qint64   slotDuration     = 100;
	int      slotCountInFrame = 10;
	int      bitsPerSlot      = 1024;
	qint64   hubTts           = 100;
	qint64   stationTts       = 100;
	quint64  seed             = 42;

	BenchEstimatorKind estimatorKind = BenchEstimatorKind::Ema;
	starTopologyEmulator::EmaIncomeLoadEstimatorConfig    emaConfig{};
	starTopologyEmulator::KalmanIncomeLoadEstimatorConfig kalmanConfig{};

	BenchPredictorKind predictorKind = BenchPredictorKind::LinearRegression;
	starTopologyEmulator::LinearRegressionIncomeStationsPredictorConfig linearRegressionConfig{};
	starTopologyEmulator::GreyModelIncomeStationsPredictorConfig         greyModelConfig{};

	std::vector<ScenarioEventData> scenario;
};

struct NamedScenario
{
	QString                        name;
	std::vector<ScenarioEventData> events;
};

struct BenchConfig
{
	BenchBaseConfig            base;
	std::vector<ImplConfig>    ftpGenerators;
	std::vector<ImplConfig>    controllers;
	std::vector<NamedScenario> scenarios;
	QString                    outputDir;
	int                        threadCount = 0;
};
