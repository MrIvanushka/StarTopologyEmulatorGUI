#include "ScenarioJson.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#pragma push_macro("emit")
#undef emit
#include <StarTopologyEmulator/Scenario/ScenarioEventFactory.h>
#include <StarTopologyEmulator/TrafficProfile/TrafficProfileFactory.h>
#pragma pop_macro("emit")

using namespace starTopologyEmulator;

namespace
{

constexpr auto kKindEnable      = "STATION_ENABLE";
constexpr auto kKindDisable     = "STATION_DISABLE";
constexpr auto kKindSetProfile  = "STATION_SET_PROFILE";

constexpr auto kProfileCbr         = "cbr";
constexpr auto kProfilePoisson     = "poisson";
constexpr auto kProfileBurst       = "burst";
constexpr auto kProfileParetoBurst = "pareto_burst";

QString kindToString(ScenarioEventKind k)
{
	switch (k)
	{
	case ScenarioEventKind::STATION_ENABLE:      return kKindEnable;
	case ScenarioEventKind::STATION_DISABLE:     return kKindDisable;
	case ScenarioEventKind::STATION_SET_PROFILE: return kKindSetProfile;
	default: return QString();
	}
}

bool kindFromString(const QString& s, ScenarioEventKind& out)
{
	if (s == kKindEnable)     { out = ScenarioEventKind::STATION_ENABLE;      return true; }
	if (s == kKindDisable)    { out = ScenarioEventKind::STATION_DISABLE;     return true; }
	if (s == kKindSetProfile) { out = ScenarioEventKind::STATION_SET_PROFILE; return true; }
	return false;
}

QJsonObject profileToJson(const ScenarioProfileConfig& p)
{
	QJsonObject obj;
	std::visit([&](const auto& cfg) {
		using T = std::decay_t<decltype(cfg)>;
		if constexpr (std::is_same_v<T, CbrTrafficProfileConfig>)
		{
			obj["type"] = kProfileCbr;
			obj["bitsPerTimestamp"] = cfg.bitsPerTimestamp;
		}
		else if constexpr (std::is_same_v<T, PoissonTrafficProfileConfig>)
		{
			obj["type"] = kProfilePoisson;
			obj["packetsPerTimestamp"] = cfg.packetsPerTimestamp;
			obj["bitsPerPacket"]       = static_cast<qint64>(cfg.bitsPerPacket);
			obj["seed"]                = static_cast<qint64>(cfg.seed);
		}
		else if constexpr (std::is_same_v<T, BurstTrafficProfileConfig>)
		{
			obj["type"] = kProfileBurst;
			obj["packetsPerTimestampOn"] = cfg.packetsPerTimestampOn;
			obj["bitsPerPacket"]         = static_cast<qint64>(cfg.bitsPerPacket);
			obj["meanOnDuration"]        = cfg.meanOnDuration;
			obj["meanOffDuration"]       = cfg.meanOffDuration;
			obj["seed"]                  = static_cast<qint64>(cfg.seed);
		}
		else if constexpr (std::is_same_v<T, ParetoBurstTrafficProfileConfig>)
		{
			obj["type"] = kProfileParetoBurst;
			obj["packetsPerTimestampOn"] = cfg.packetsPerTimestampOn;
			obj["bitsPerPacket"]         = static_cast<qint64>(cfg.bitsPerPacket);
			obj["minOnDuration"]         = cfg.minOnDuration;
			obj["minOffDuration"]        = cfg.minOffDuration;
			obj["alpha"]                 = cfg.alpha;
			obj["seed"]                  = static_cast<qint64>(cfg.seed);
		}
	}, p);
	return obj;
}

bool profileFromJson(const QJsonObject& obj, ScenarioProfileConfig& out, QString& err)
{
	const QString type = obj.value("type").toString();
	if (type == kProfileCbr)
	{
		CbrTrafficProfileConfig c;
		c.bitsPerTimestamp = obj.value("bitsPerTimestamp").toDouble();
		out = c;
		return true;
	}
	if (type == kProfilePoisson)
	{
		PoissonTrafficProfileConfig c;
		c.packetsPerTimestamp = obj.value("packetsPerTimestamp").toDouble();
		c.bitsPerPacket       = static_cast<std::uint64_t>(obj.value("bitsPerPacket").toVariant().toLongLong());
		c.seed                = static_cast<std::uint32_t>(obj.value("seed").toVariant().toUInt());
		out = c;
		return true;
	}
	if (type == kProfileBurst)
	{
		BurstTrafficProfileConfig c;
		c.packetsPerTimestampOn = obj.value("packetsPerTimestampOn").toDouble();
		c.bitsPerPacket         = static_cast<std::uint64_t>(obj.value("bitsPerPacket").toVariant().toLongLong());
		c.meanOnDuration        = obj.value("meanOnDuration").toDouble();
		c.meanOffDuration       = obj.value("meanOffDuration").toDouble();
		c.seed                  = static_cast<std::uint32_t>(obj.value("seed").toVariant().toUInt());
		out = c;
		return true;
	}
	if (type == kProfileParetoBurst)
	{
		ParetoBurstTrafficProfileConfig c;
		c.packetsPerTimestampOn = obj.value("packetsPerTimestampOn").toDouble();
		c.bitsPerPacket         = static_cast<std::uint64_t>(obj.value("bitsPerPacket").toVariant().toLongLong());
		c.minOnDuration         = obj.value("minOnDuration").toDouble();
		c.minOffDuration        = obj.value("minOffDuration").toDouble();
		c.alpha                 = obj.value("alpha").toDouble();
		c.seed                  = static_cast<std::uint32_t>(obj.value("seed").toVariant().toUInt());
		out = c;
		return true;
	}
	err = QStringLiteral("unknown profile type: ") + type;
	return false;
}

std::unique_ptr<ITrafficProfile> makeProfile(const ScenarioProfileConfig& p)
{
	return std::visit([](const auto& cfg) -> std::unique_ptr<ITrafficProfile> {
		return TrafficProfileFactory::make(cfg);
	}, p);
}

} // namespace

QJsonDocument ScenarioJson::toJson(const std::vector<ScenarioEventData>& events)
{
	QJsonArray arr;
	for (const auto& e : events)
	{
		QJsonObject obj;
		obj["kind"]  = kindToString(e.kind);
		obj["at"]    = static_cast<qint64>(e.at);
		obj["minId"] = static_cast<qint64>(e.minId);
		obj["maxId"] = static_cast<qint64>(e.maxId);
		if (e.kind == ScenarioEventKind::STATION_SET_PROFILE)
			obj["profile"] = profileToJson(e.profile);
		arr.append(obj);
	}
	QJsonObject root;
	root["events"] = arr;
	return QJsonDocument(root);
}

std::vector<ScenarioEventData> ScenarioJson::fromJson(const QJsonDocument& doc, QString* error)
{
	auto setErr = [error](const QString& msg) { if (error) *error = msg; };

	if (!doc.isObject())
	{
		setErr("root is not an object");
		return {};
	}
	const QJsonArray arr = doc.object().value("events").toArray();

	std::vector<ScenarioEventData> result;
	result.reserve(arr.size());

	for (int i = 0; i < arr.size(); ++i)
	{
		const QJsonObject obj = arr.at(i).toObject();
		ScenarioEventData ev;

		const QString kindStr = obj.value("kind").toString();
		if (!kindFromString(kindStr, ev.kind))
		{
			setErr(QStringLiteral("event[%1]: unknown kind '%2'").arg(i).arg(kindStr));
			return {};
		}

		ev.at    = static_cast<Timestamp>(obj.value("at").toVariant().toLongLong());
		ev.minId = static_cast<StationID>(obj.value("minId").toVariant().toUInt());
		ev.maxId = static_cast<StationID>(obj.value("maxId").toVariant().toUInt());

		if (ev.kind == ScenarioEventKind::STATION_SET_PROFILE)
		{
			QString profileErr;
			if (!profileFromJson(obj.value("profile").toObject(), ev.profile, profileErr))
			{
				setErr(QStringLiteral("event[%1]: %2").arg(i).arg(profileErr));
				return {};
			}
		}

		result.push_back(std::move(ev));
	}

	return result;
}

std::vector<std::unique_ptr<IScenarioEvent>>
	ScenarioJson::buildEvents(const std::vector<ScenarioEventData>& events)
{
	std::vector<std::unique_ptr<IScenarioEvent>> result;
	result.reserve(events.size());
	for (const auto& e : events)
	{
		switch (e.kind)
		{
		case ScenarioEventKind::STATION_ENABLE:
			result.push_back(ScenarioEventFactory::makeStationEnable(e.at, e.minId, e.maxId));
			break;
		case ScenarioEventKind::STATION_DISABLE:
			result.push_back(ScenarioEventFactory::makeStationDisable(e.at, e.minId, e.maxId));
			break;
		case ScenarioEventKind::STATION_SET_PROFILE:
		{
			const ScenarioProfileConfig cfg = e.profile;
			result.push_back(ScenarioEventFactory::makeStationSetProfile(
				e.at, e.minId, e.maxId,
				[cfg]() { return makeProfile(cfg); }));
			break;
		}
		default:
			break;
		}
	}
	return result;
}
