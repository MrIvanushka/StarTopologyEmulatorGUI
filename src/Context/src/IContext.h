#pragma once

#include <QString>
#include <QVariant>

namespace editor
{

class IContext
{
public:
    virtual ~IContext() = default;

    virtual void setValue(const QString& key, const QVariant& value) = 0;
    virtual QVariant value(const QString& key, const QVariant& defaultValue = {}) const = 0;
};

}  // namespace editor
