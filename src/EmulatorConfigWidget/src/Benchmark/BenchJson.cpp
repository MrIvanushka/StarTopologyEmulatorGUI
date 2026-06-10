#include "BenchJson.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include "../Scenario/ScenarioJson.h"

namespace
{

QJsonValue paramValueToJson(const ParamValue& v)
{
	return std::visit([](const auto& x) -> QJsonValue {
		using T = std::decay_t<decltype(x)>;
		if constexpr (std::is_same_v<T, double>) return QJsonValue(x);
		else if constexpr (std::is_same_v<T, int>) return QJsonValue(x);
		else if constexpr (std::is_same_v<T, bool>) return QJsonValue(x);
		else if constexpr (std::is_same_v<T, SweepRange>) {
			QJsonArray a;
			a.append(x.min);
			a.append(x.max);
			a.append(x.step);
			return a;
		}
		return QJsonValue();
	}, v);
}

ParamValue paramValueFromJson(const QJsonValue& v)
{
	if (v.isArray())
	{
		const auto a = v.toArray();
		SweepRange r;
		if (a.size() >= 1) r.min  = a.at(0).toDouble();
		if (a.size() >= 2) r.max  = a.at(1).toDouble();
		if (a.size() >= 3) r.step = a.at(2).toDouble();
		return r;
	}
	if (v.isBool())
		return v.toBool();
	if (v.isDouble())
	{
		const double d = v.toDouble();
		const double r = std::round(d);
		if (std::abs(d - r) < 1e-12 && std::abs(d) < 1e15)
			return static_cast<int>(r);
		return d;
	}
	return 0.0;
}

QJsonObject implToJson(const ImplConfig& c)
{
	QJsonObject o;
	o["kind"] = c.kind;
	QJsonObject params;
	for (auto it = c.params.constBegin(); it != c.params.constEnd(); ++it)
		params[it.key()] = paramValueToJson(it.value());
	o["params"] = params;
	return o;
}

ImplConfig implFromJson(const QJsonObject& o)
{
	ImplConfig c;
	c.kind = o.value("kind").toString();
	const QJsonObject params = o.value("params").toObject();
	for (auto it = params.constBegin(); it != params.constEnd(); ++it)
		c.params.insert(it.key(), paramValueFromJson(it.value()));
	return c;
}

QJsonObject emaToJson(const starTopologyEmulator::EmaIncomeLoadEstimatorConfig& c)
{
	QJsonObject o;
	o["alphaG"]          = c.alphaG;
	o["alphaPlr"]        = c.alphaPlr;
	o["collisionWeight"] = c.collisionWeight;
	return o;
}

void emaFromJson(starTopologyEmulator::EmaIncomeLoadEstimatorConfig& c, const QJsonObject& o)
{
	c.alphaG          = o.value("alphaG").toDouble(c.alphaG);
	c.alphaPlr        = o.value("alphaPlr").toDouble(c.alphaPlr);
	c.collisionWeight = o.value("collisionWeight").toDouble(c.collisionWeight);
}

QJsonObject kalmanToJson(const starTopologyEmulator::KalmanIncomeLoadEstimatorConfig& c)
{
	QJsonObject o;
	o["qG"]              = c.qG;
	o["qPlr"]            = c.qPlr;
	o["rBase"]           = c.rBase;
	o["collisionWeight"] = c.collisionWeight;
	return o;
}

void kalmanFromJson(starTopologyEmulator::KalmanIncomeLoadEstimatorConfig& c, const QJsonObject& o)
{
	c.qG              = o.value("qG").toDouble(c.qG);
	c.qPlr            = o.value("qPlr").toDouble(c.qPlr);
	c.rBase           = o.value("rBase").toDouble(c.rBase);
	c.collisionWeight = o.value("collisionWeight").toDouble(c.collisionWeight);
}

QJsonObject linRegToJson(const starTopologyEmulator::LinearRegressionIncomeStationsPredictorConfig& c)
{
	QJsonObject o;
	o["regressionWindow"]     = static_cast<int>(c.regressionWindow);
	o["forgettingHorizonSec"] = c.forgettingHorizonSec;
	o["minProbability"]       = c.minProbability;
	o["maxProbability"]       = c.maxProbability;
	o["epsilon"]              = c.epsilon;
	return o;
}

void linRegFromJson(starTopologyEmulator::LinearRegressionIncomeStationsPredictorConfig& c, const QJsonObject& o)
{
	c.regressionWindow     = static_cast<std::uint32_t>(o.value("regressionWindow").toInt(c.regressionWindow));
	c.forgettingHorizonSec = o.value("forgettingHorizonSec").toDouble(c.forgettingHorizonSec);
	c.minProbability       = o.value("minProbability").toDouble(c.minProbability);
	c.maxProbability       = o.value("maxProbability").toDouble(c.maxProbability);
	c.epsilon              = o.value("epsilon").toDouble(c.epsilon);
}

QJsonObject greyModelToJson(const starTopologyEmulator::GreyModelIncomeStationsPredictorConfig& c)
{
	QJsonObject o;
	o["windowSize"]     = static_cast<int>(c.windowSize);
	o["minHistory"]     = static_cast<int>(c.minHistory);
	o["minProbability"] = c.minProbability;
	o["maxProbability"] = c.maxProbability;
	o["epsilon"]        = c.epsilon;
	return o;
}

void greyModelFromJson(starTopologyEmulator::GreyModelIncomeStationsPredictorConfig& c, const QJsonObject& o)
{
	c.windowSize     = static_cast<std::uint32_t>(o.value("windowSize").toInt(c.windowSize));
	c.minHistory     = static_cast<std::uint32_t>(o.value("minHistory").toInt(c.minHistory));
	c.minProbability = o.value("minProbability").toDouble(c.minProbability);
	c.maxProbability = o.value("maxProbability").toDouble(c.maxProbability);
	c.epsilon        = o.value("epsilon").toDouble(c.epsilon);
}

QString estimatorKindToStr(BenchEstimatorKind k)
{
	switch (k)
	{
	case BenchEstimatorKind::Ema:    return "ema";
	case BenchEstimatorKind::Kalman: return "kalman";
	}
	return "ema";
}

BenchEstimatorKind estimatorKindFromStr(const QString& s)
{
	if (s == "kalman") return BenchEstimatorKind::Kalman;
	return BenchEstimatorKind::Ema;
}

QString predictorKindToStr(BenchPredictorKind k)
{
	switch (k)
	{
	case BenchPredictorKind::LinearRegression:     return "linearRegression";
	case BenchPredictorKind::Cogorthy:             return "cogorthy";
	case BenchPredictorKind::BackoffAwareCogorthy: return "backoffAwareCogorthy";
	case BenchPredictorKind::GreyModel:            return "greyModel";
	}
	return "linearRegression";
}

BenchPredictorKind predictorKindFromStr(const QString& s)
{
	if (s == "cogorthy")             return BenchPredictorKind::Cogorthy;
	if (s == "backoffAwareCogorthy") return BenchPredictorKind::BackoffAwareCogorthy;
	if (s == "greyModel")            return BenchPredictorKind::GreyModel;
	return BenchPredictorKind::LinearRegression;
}

}

QJsonDocument BenchJson::toJson(const BenchConfig& c)
{
	QJsonObject root;

	QJsonObject base;
	base["numStations"]      = c.base.numStations;
	base["duration"]         = c.base.duration;
	base["messagesNeeded"]   = c.base.messagesNeeded;
	base["slotDuration"]     = static_cast<qint64>(c.base.slotDuration);
	base["slotCountInFrame"] = c.base.slotCountInFrame;
	base["bitsPerSlot"]      = c.base.bitsPerSlot;
	base["hubTts"]           = static_cast<qint64>(c.base.hubTts);
	base["stationTts"]       = static_cast<qint64>(c.base.stationTts);
	base["seed"]             = static_cast<qint64>(c.base.seed);

	QJsonObject estimator;
	estimator["kind"] = estimatorKindToStr(c.base.estimatorKind);
	estimator["ema"]    = emaToJson(c.base.emaConfig);
	estimator["kalman"] = kalmanToJson(c.base.kalmanConfig);
	base["estimator"] = estimator;

	QJsonObject predictor;
	predictor["kind"] = predictorKindToStr(c.base.predictorKind);
	predictor["linearRegression"] = linRegToJson(c.base.linearRegressionConfig);
	predictor["greyModel"]        = greyModelToJson(c.base.greyModelConfig);
	base["predictor"] = predictor;

	base["scenario"] = ScenarioJson::toJson(c.base.scenario).object();

	root["base"] = base;

	QJsonArray scenarioArr;
	for (const auto& s : c.scenarios)
	{
		QJsonObject so;
		so["name"]   = s.name;
		so["events"] = ScenarioJson::toJson(s.events).object();
		scenarioArr.append(so);
	}
	root["scenarios"] = scenarioArr;

	QJsonArray ftpArr;
	for (const auto& impl : c.ftpGenerators) ftpArr.append(implToJson(impl));
	root["ftpGenerators"] = ftpArr;

	QJsonArray ctlArr;
	for (const auto& impl : c.controllers) ctlArr.append(implToJson(impl));
	root["controllers"] = ctlArr;

	root["outputDir"]   = c.outputDir;
	root["threadCount"] = c.threadCount;

	return QJsonDocument(root);
}

BenchConfig BenchJson::fromJson(const QJsonDocument& doc, QString* error)
{
	BenchConfig c;
	if (!doc.isObject())
	{
		if (error) *error = "root is not an object";
		return c;
	}
	const QJsonObject root = doc.object();
	const QJsonObject base = root.value("base").toObject();

	c.base.numStations      = base.value("numStations").toInt(c.base.numStations);
	c.base.duration         = base.value("duration").toInt(c.base.duration);
	c.base.messagesNeeded   = base.value("messagesNeeded").toInt(c.base.messagesNeeded);
	c.base.slotDuration     = static_cast<qint64>(base.value("slotDuration").toVariant().toLongLong());
	c.base.slotCountInFrame = base.value("slotCountInFrame").toInt(c.base.slotCountInFrame);
	c.base.bitsPerSlot      = base.value("bitsPerSlot").toInt(c.base.bitsPerSlot);
	c.base.hubTts           = static_cast<qint64>(base.value("hubTts").toVariant().toLongLong());
	c.base.stationTts       = static_cast<qint64>(base.value("stationTts").toVariant().toLongLong());
	c.base.seed             = static_cast<quint64>(base.value("seed").toVariant().toULongLong());

	const QJsonObject estimator = base.value("estimator").toObject();
	c.base.estimatorKind = estimatorKindFromStr(estimator.value("kind").toString());
	emaFromJson(c.base.emaConfig, estimator.value("ema").toObject());
	kalmanFromJson(c.base.kalmanConfig, estimator.value("kalman").toObject());

	const QJsonObject predictor = base.value("predictor").toObject();
	c.base.predictorKind = predictorKindFromStr(predictor.value("kind").toString());
	linRegFromJson(c.base.linearRegressionConfig, predictor.value("linearRegression").toObject());
	greyModelFromJson(c.base.greyModelConfig, predictor.value("greyModel").toObject());

	c.base.scenario = ScenarioJson::fromJson(QJsonDocument(base.value("scenario").toObject()));

	for (const auto v : root.value("scenarios").toArray())
	{
		const QJsonObject so = v.toObject();
		NamedScenario ns;
		ns.name   = so.value("name").toString();
		ns.events = ScenarioJson::fromJson(QJsonDocument(so.value("events").toObject()));
		c.scenarios.push_back(std::move(ns));
	}
	if (c.scenarios.empty() && !c.base.scenario.empty())
		c.scenarios.push_back({ QStringLiteral("scenario"), c.base.scenario });

	for (const auto v : root.value("ftpGenerators").toArray())
		c.ftpGenerators.push_back(implFromJson(v.toObject()));
	for (const auto v : root.value("controllers").toArray())
		c.controllers.push_back(implFromJson(v.toObject()));

	c.outputDir   = root.value("outputDir").toString();
	c.threadCount = root.value("threadCount").toInt(0);

	return c;
}
