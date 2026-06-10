#pragma once

#include <QJsonDocument>
#include <QString>

#include "BenchTypes.h"

class BenchJson
{
public:
	static QJsonDocument toJson(const BenchConfig&);
	static BenchConfig   fromJson(const QJsonDocument&, QString* error = nullptr);
};
