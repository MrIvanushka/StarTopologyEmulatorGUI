#include "SimpleStrategyConfigWidget.h"

#include <QFormLayout>

using namespace starTopologyEmulator;

SimpleStrategyConfigWidget::SimpleStrategyConfigWidget(QWidget* parent) 
	: QWidget(parent)
{
	setupUi();
}

StarHubStrategyConfig SimpleStrategyConfigWidget::getConfig(int totalSlots) const {
	StarHubStrategyConfig cfg;
	cfg.totalSlots = static_cast<std::uint8_t>(totalSlots);
	cfg.minRaSlots = static_cast<std::uint8_t>(_sbMinRaSlots->value() * totalSlots);
	cfg.maxRaSlots = static_cast<std::uint8_t>(_sbMaxRaSlots->value() * totalSlots);

	cfg.targetPlr = _dsbTargetPlr->value();
	cfg.heavyLoadG = _dsbHeavyLoadG->value();

	cfg.minBaseWindow = static_cast<std::uint8_t>(_sbMinBaseWindow->value());
	cfg.maxBaseWindow = static_cast<std::uint8_t>(_sbMaxBaseWindow->value());

	cfg.minPTx = _dsbMinPTx->value();
	cfg.maxPTx = _dsbMaxPTx->value();

	return cfg;
}

void SimpleStrategyConfigWidget::setConfig(const StarHubStrategyConfig& cfg) {
	_sbMinRaSlots->setValue(static_cast<double>(cfg.minRaSlots) / cfg.totalSlots);
	_sbMaxRaSlots->setValue(static_cast<double>(cfg.maxRaSlots) / cfg.totalSlots);

	_dsbTargetPlr->setValue(cfg.targetPlr);
	_dsbHeavyLoadG->setValue(cfg.heavyLoadG);

	_sbMinBaseWindow->setValue(cfg.minBaseWindow);
	_sbMaxBaseWindow->setValue(cfg.maxBaseWindow);

	_dsbMinPTx->setValue(cfg.minPTx);
	_dsbMaxPTx->setValue(cfg.maxPTx);
}

void SimpleStrategyConfigWidget::setupUi()
{
	QFormLayout* layout = new QFormLayout(this);

	_sbMinRaSlots = new QDoubleSpinBox(this);
	_sbMinRaSlots->setRange(0, 1);

	_sbMaxRaSlots = new QDoubleSpinBox(this);
	_sbMaxRaSlots->setRange(0, 1);

	_dsbTargetPlr = new QDoubleSpinBox(this);
	_dsbTargetPlr->setRange(0.0, 1.0);
	_dsbTargetPlr->setSingleStep(0.01);
	_dsbTargetPlr->setDecimals(3);

	_dsbHeavyLoadG = new QDoubleSpinBox(this);
	_dsbHeavyLoadG->setRange(0.0, 5.0);
	_dsbHeavyLoadG->setSingleStep(0.1);

	_sbMinBaseWindow = new QSpinBox(this);
	_sbMinBaseWindow->setRange(1, 255);

	_sbMaxBaseWindow = new QSpinBox(this);
	_sbMaxBaseWindow->setRange(1, 255);

	_dsbMinPTx = new QDoubleSpinBox(this);
	_dsbMinPTx->setRange(0.0, 1.0);
	_dsbMinPTx->setSingleStep(0.05);

	_dsbMaxPTx = new QDoubleSpinBox(this);
	_dsbMaxPTx->setRange(0.0, 1.0);
	_dsbMaxPTx->setSingleStep(0.05);

	layout->addRow(QString::fromLocal8Bit("Минимальная доля слотов для случайного доступа: "), _sbMinRaSlots);
	layout->addRow(QString::fromLocal8Bit("Максимальная доля слотов для случайного доступа: "), _sbMaxRaSlots);
	layout->addRow(QString::fromLocal8Bit("Желаемый PLR: "), _dsbTargetPlr);
	layout->addRow(QString::fromLocal8Bit("Порог \"тяжёлой\" входной нагрузки: "), _dsbHeavyLoadG);
	layout->addRow(QString::fromLocal8Bit("Минимальное окно backoff: "), _sbMinBaseWindow);
	layout->addRow(QString::fromLocal8Bit("Максимальное окно backoff: "), _sbMaxBaseWindow);
	layout->addRow(QString::fromLocal8Bit("Минимальная вероятность вещания: "), _dsbMinPTx);
	layout->addRow(QString::fromLocal8Bit("Максимальная вероятность вещания: "), _dsbMaxPTx);

	setConfig(StarHubStrategyConfig());
}
