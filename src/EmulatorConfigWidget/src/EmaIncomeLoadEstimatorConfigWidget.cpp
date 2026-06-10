#include "EmaIncomeLoadEstimatorConfigWidget.h"

#include <QFormLayout>

using namespace starTopologyEmulator;

EmaIncomeLoadEstimatorConfigWidget::EmaIncomeLoadEstimatorConfigWidget(QWidget* parent) 
	: QWidget(parent)
{
	setupUi();
}

EmaIncomeLoadEstimatorConfig EmaIncomeLoadEstimatorConfigWidget::getConfig() const
{
	return EmaIncomeLoadEstimatorConfig({
			.alphaG = _alphaG->value(),
			.alphaPlr = _alphaPlr->value(),
			.collisionWeight = _collisionWeight->value(),
		});
}

void EmaIncomeLoadEstimatorConfigWidget::setConfig(const EmaIncomeLoadEstimatorConfig& cfg)
{
	_alphaG->setValue(cfg.alphaG);
	_alphaPlr->setValue(cfg.alphaPlr);
	_collisionWeight->setValue(cfg.collisionWeight);
}

void EmaIncomeLoadEstimatorConfigWidget::setupUi()
{
	QFormLayout* layout = new QFormLayout(this);
	layout->setSizeConstraint(QLayout::SetMinimumSize);

	_alphaG = new QDoubleSpinBox(this);
	_alphaG->setRange(0.05, 1.0);
	_alphaG->setSingleStep(0.05);

	_alphaPlr = new QDoubleSpinBox(this);
	_alphaPlr->setRange(0.05, 1.0);
	_alphaPlr->setSingleStep(0.05);

	_collisionWeight = new QDoubleSpinBox(this);
	_collisionWeight->setRange(1, 100);
	_collisionWeight->setSingleStep(0.01);

	layout->addRow(QString::fromLocal8Bit("Коэффициент сглаживания входной нагрузки: "), _alphaG);
	layout->addRow(QString::fromLocal8Bit("Коэффициент сглаживания PLR: "), _alphaPlr);
	layout->addRow(QString::fromLocal8Bit("Сколько станций \"в среднем\" стоит за одной коллизией: "), _collisionWeight);

	setConfig(EmaIncomeLoadEstimatorConfig());
}
