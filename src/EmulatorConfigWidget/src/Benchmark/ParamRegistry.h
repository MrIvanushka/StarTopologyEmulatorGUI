#pragma once

#include <memory>

#include <QHash>
#include <QList>
#include <QString>
#include <QVariant>

#pragma push_macro("emit")
#undef emit
#include <StarTopologyEmulator/IFaces/IBacklogAccumulator.h>
#include <StarTopologyEmulator/IFaces/IDynamicFrameSettings.h>
#include <StarTopologyEmulator/IFaces/IFrameCalculator.h>
#include <StarTopologyEmulator/IFaces/IFtpGenerator.h>
#include <StarTopologyEmulator/IFaces/IIncomeLoadController.h>
#include <StarTopologyEmulator/IFaces/IIncomeStationsPredictor.h>
#include <StarTopologyEmulator/Metrics/MetricSink.h>
#pragma pop_macro("emit")

struct ParamSpec
{
	enum class Type { Double, Int, Bool };

	QString  key;
	QString  label;
	Type     type;
	double   minBound = 0.0;
	double   maxBound = 1.0e9;
	double   step     = 0.01;
	int      decimals = 4;
	QVariant defaultValue;
	bool     sweepable = true;
};

struct ImplSpec
{
	QString          kind;
	QString          label;
	QList<ParamSpec> params;
};

class ParamRegistry
{
public:
	static const QList<ImplSpec>& ftpGenerators();
	static const QList<ImplSpec>& controllers();

	static const ImplSpec* findFtpGenerator(const QString& kind);
	static const ImplSpec* findController(const QString& kind);

	using ParamMap = QHash<QString, QVariant>;

	static std::unique_ptr<starTopologyEmulator::IFtpGenerator> makeFtpGenerator(
		const QString& kind,
		const ParamMap& params,
		std::shared_ptr<starTopologyEmulator::IDynamicFrameSettings>,
		std::shared_ptr<starTopologyEmulator::IIncomeStationsPredictor>,
		std::shared_ptr<starTopologyEmulator::IBacklogAccumulator>,
		std::shared_ptr<starTopologyEmulator::IFrameCalculator>);

	static std::unique_ptr<starTopologyEmulator::IIncomeLoadController> makeController(
		const QString& kind,
		const ParamMap& params,
		std::shared_ptr<starTopologyEmulator::IDynamicFrameSettings>,
		std::shared_ptr<starTopologyEmulator::IIncomeStationsPredictor>,
		starTopologyEmulator::MetricScope);
};
