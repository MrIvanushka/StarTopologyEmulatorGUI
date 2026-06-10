#pragma once

#include <QString>
#include <QSortFilterProxyModel>
#include <QCheckbox>
#include <QMenu>
#include "ModuleWidget.h"
#include "CustomPlot.h"

class GraphWidget : public editor::ModuleWidget
{
	Q_OBJECT
public:
	GraphWidget();

	void init(std::shared_ptr<editor::IContext> context) override;

	const editor::Info& info() const override;
private slots:
	void clearGraphs();
private:
	QMenu* _graphFilterMenu;

	std::shared_ptr<editor::IContext> _context;
	editor::Info _info = editor::Info(QString::fromLocal8Bit("График приёма миллисекундных отсчётов"));

	CustomPlot* _plot;

	int _currentSignalIndex;
	std::unordered_map<int, int> _prnToGraphIndex;
};
