#pragma once

#include <memory>
#include <vector>

#include <QJsonDocument>
#include <QString>

#pragma push_macro("emit")
#undef emit
#include <StarTopologyEmulator/IFaces/IScenarioEvent.h>
#pragma pop_macro("emit")

#include "ScenarioEventData.h"

class ScenarioJson
{
public:
	static QJsonDocument toJson(const std::vector<ScenarioEventData>&);
	static std::vector<ScenarioEventData> fromJson(const QJsonDocument&, QString* error = nullptr);

	static std::vector<std::unique_ptr<starTopologyEmulator::IScenarioEvent>>
		buildEvents(const std::vector<ScenarioEventData>&);
};
