#pragma once

#include "IContext.h"
#include "Info.h"
#include <memory>
#include <QWidget>

namespace editor
{

class ModuleWidget : public QWidget
{
public:
	virtual void init(std::shared_ptr<IContext> context) = 0;

	virtual const Info& info() const = 0;
};

}  // namespace editor
