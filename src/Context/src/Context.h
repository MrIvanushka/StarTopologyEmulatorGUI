#pragma once

#include "IContext.h"

namespace editor
{

class Context : public IContext
{
	//Q_OBJECT
public:
	void setValue(const QString& key, const QVariant& value) { return; }
	QVariant value(const QString& key, const QVariant& defaultValue = {}) const { return {}; }
};

} // namespace editor
