#pragma once

#include <cstdint>
#include <variant>

#pragma push_macro("emit")
#undef emit
#include <StarTopologyEmulator/IFaces/IScenarioEvent.h>
#include <StarTopologyEmulator/TrafficProfile/BurstTrafficProfileConfig.h>
#include <StarTopologyEmulator/TrafficProfile/CbrTrafficProfileConfig.h>
#include <StarTopologyEmulator/TrafficProfile/ParetoBurstTrafficProfileConfig.h>
#include <StarTopologyEmulator/TrafficProfile/PoissonTrafficProfileConfig.h>
#pragma pop_macro("emit")

using ScenarioProfileConfig = std::variant<
	starTopologyEmulator::CbrTrafficProfileConfig,
	starTopologyEmulator::PoissonTrafficProfileConfig,
	starTopologyEmulator::BurstTrafficProfileConfig,
	starTopologyEmulator::ParetoBurstTrafficProfileConfig>;

struct ScenarioEventData
{
	starTopologyEmulator::ScenarioEventKind kind =
		starTopologyEmulator::ScenarioEventKind::STATION_ENABLE;
	starTopologyEmulator::Timestamp at = 0;
	starTopologyEmulator::StationID minId = 0;
	starTopologyEmulator::StationID maxId = 0;
	ScenarioProfileConfig profile{};
};
