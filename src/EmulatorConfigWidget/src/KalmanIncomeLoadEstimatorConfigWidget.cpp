#include "KalmanIncomeLoadEstimatorConfigWidget.h"

#include <QFormLayout>

using namespace starTopologyEmulator;

KalmanIncomeLoadEstimatorConfigWidget::KalmanIncomeLoadEstimatorConfigWidget(QWidget* parent)
	: QWidget(parent)
{
	setupUi();
}

KalmanIncomeLoadEstimatorConfig KalmanIncomeLoadEstimatorConfigWidget::getConfig() const
{
	return KalmanIncomeLoadEstimatorConfig({
			.qG = _qG->value(),
			.qPlr = _qPlr->value(),
			.rBase = _rBase->value(),
			.collisionWeight = _collisionWeight->value(),
		});
}

void KalmanIncomeLoadEstimatorConfigWidget::setConfig(const KalmanIncomeLoadEstimatorConfig& cfg)
{
	_qG->setValue(cfg.qG);
	_qPlr->setValue(cfg.qPlr);
	_rBase->setValue(cfg.rBase);
	_collisionWeight->setValue(cfg.collisionWeight);
}

void KalmanIncomeLoadEstimatorConfigWidget::setupUi()
{
	QFormLayout* layout = new QFormLayout(this);
	layout->setSizeConstraint(QLayout::SetMinimumSize);

	_qG = new QDoubleSpinBox(this);
	_qG->setRange(0.001, 1.0);
	_qG->setSingleStep(0.001);

	_qPlr = new QDoubleSpinBox(this);
	_qPlr->setRange(0.001, 1.0);
	_qPlr->setSingleStep(0.001);

	_rBase = new QDoubleSpinBox(this);
	_rBase->setRange(0.001, 1.0);
	_rBase->setSingleStep(0.001);

	_collisionWeight = new QDoubleSpinBox(this);
	_collisionWeight->setRange(1, 100);
	_collisionWeight->setSingleStep(0.01);

	layout->addRow(QString::fromLocal8Bit("Шум входной нагрузки: "), _qG);
	layout->addRow(QString::fromLocal8Bit("Шум PLR: "), _qPlr);
	layout->addRow(QString::fromLocal8Bit("Базовый шум измерения: "), _rBase);
	layout->addRow(QString::fromLocal8Bit("Сколько станций \"в среднем\" стоит за одной коллизией: "), _collisionWeight);

	setConfig(KalmanIncomeLoadEstimatorConfig());
}
