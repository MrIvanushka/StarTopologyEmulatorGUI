#include "GraphWidget.h"

#include <QVBoxLayout>
#include <QToolButton>

using namespace editor;

GraphWidget::GraphWidget()
{
	auto lay = new QVBoxLayout;
	setLayout(lay);

	auto formLay = new QFormLayout;
	lay->addLayout(formLay);
	formLay->setContentsMargins(0, 0, 0, 0);

	_graphFilterMenu = new QMenu();
	auto toolButton = new QToolButton();
	toolButton->setText(QString::fromLocal8Bit("Выберите..."));
	toolButton->setMenu(_graphFilterMenu);
	toolButton->setPopupMode(QToolButton::InstantPopup);
	toolButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	formLay->addRow(QString::fromLocal8Bit("Фильтр по спутникам"), toolButton);
	QCPRange xrange = { 0, 200 };
	QCPRange yrange = { -1000, 1000 };
	QString ylabel = "Imagine value";
	QString xlabel = "Signal number";

	_plot = new CustomPlot(nullptr);
	_plot->setMinimumSize(400, 300);
	_plot->xAxis->setRange(xrange);
	_plot->xAxis->setLabel(xlabel);
	_plot->yAxis->setRange(yrange);
	_plot->yAxis->setLabel(ylabel);
	_plot->setHorizontalAxisLimit(0);
	//_plot->setVerticalAxisLimit(0);
	_plot->SetEditable(false);
	_plot->legend->setVisible(true);

	lay->addWidget(_plot);
}

void GraphWidget::init(std::shared_ptr<IContext> context)
{
	_context = context;
	//_context->addReceiver(this);
}

const Info& GraphWidget::info() const
{
	return _info;
}

void GraphWidget::clearGraphs()
{
	_plot->clearGraphs();
	_currentSignalIndex = 0;
	_prnToGraphIndex.clear();
	_graphFilterMenu->clear();
}
