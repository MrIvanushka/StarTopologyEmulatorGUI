#pragma once

#include <QString>

namespace editor
{

class Info
{
public:
	explicit Info(QString name, QString description = "");

	const QString& name() const;

	const QString& description() const;
private:
	QString _name;

	QString _description;
};

} // namespace editor
