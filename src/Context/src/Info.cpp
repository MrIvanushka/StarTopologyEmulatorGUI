#include "Info.h"

using namespace editor;

Info::Info(QString name, QString description)
	: _name(name), _description(description)
{

}

const QString& Info::name() const
{
	return _name;
}

const QString& Info::description() const
{
	return _description;
}