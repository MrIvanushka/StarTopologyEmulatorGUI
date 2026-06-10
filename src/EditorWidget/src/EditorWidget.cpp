#include "EditorWidget.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QFileDialog>
#include <QMenuBar>
#include <QAction>

#include "EmulatorConfigWidget.h"
#include "Benchmark/BenchmarkWidget.h"

#include "Ads/SectionTitleWidget.h"

using namespace editor;

EditorWidget::EditorWidget()
{
	_context = std::make_shared<Context>();

	resize(1920, 1080);
	QMenu* windowMenu = menuBar()->addMenu(QString::fromLocal8Bit("Окно"));

	_container = new ContainerWidget();
	this->setCentralWidget(_container);

	auto emulatorConfigEditorModule = new EmulatorConfigWidget;
	auto benchmarkModule = new BenchmarkWidget;
	_widgets = { emulatorConfigEditorModule, benchmarkModule };

	auto center = makeSection(emulatorConfigEditorModule, CenterDropArea);
	makeSection(benchmarkModule, CenterDropArea, center);

	auto madeMenu = _container->createContextMenu();

	for(auto action : madeMenu->actions())
		windowMenu->addAction(action);
}

SectionWidget* EditorWidget::makeSection(ModuleWidget* widget, DropArea dropArea, SectionWidget* parent)
{
	widget->init(_context);
	auto section = SectionContent::newSectionContent(widget->info().name(), _container, new QLabel(widget->info().name()), widget);
	if (dropArea != InvalidDropArea)
	{
		return _container->addSectionContent(section, parent, dropArea);
	}
	else
	{
		_container->addSectionContent(section, NULL, CenterDropArea);
		_container->hideSectionContent(section);
		return nullptr;
	}
}