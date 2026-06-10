#pragma once

#include <QString>

#include "BenchPlan.h"

class BenchRunner
{
public:
	struct RunResult
	{
		bool    ok = false;
		QString csvPath;
		QString error;
	};

	static RunResult run(const BenchRun&, const BenchBaseConfig&, const QString& outputDir);
};
