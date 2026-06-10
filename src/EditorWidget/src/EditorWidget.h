#pragma once

#include <QWidget>
#include <QMainWindow>
#include <QFileInfo>


#include "Context.h"
#include "ModuleWidget.h"

#include "ads/ContainerWidget.h"
#include "ads/SectionContent.h"

namespace editor
{

class EditorWidget : public QMainWindow
{
	Q_OBJECT
public:
	EditorWidget();
private:
	SectionWidget* makeSection(ModuleWidget* widget, DropArea dropArea = DropArea::InvalidDropArea, SectionWidget* parent = nullptr);
private:
	std::shared_ptr<Context> _context;

	ContainerWidget* _container;
	QList<ModuleWidget*> _widgets;
};

} // namespace editor
