#pragma once

#include <vector>

#include <QHash>
#include <QString>
#include <QVariant>

#include "BenchTypes.h"

struct BenchRun
{
	int    index = 0;
	QString ftpKind;
	QString controllerKind;
	QString scenarioName;

	QHash<QString, QVariant> ftpParams;
	QHash<QString, QVariant> controllerParams;

	QHash<QString, QVariant> sweptParams;

	std::vector<ScenarioEventData> scenario;
};

class BenchPlan
{
public:
	static std::vector<BenchRun> expand(const BenchConfig&);
};
