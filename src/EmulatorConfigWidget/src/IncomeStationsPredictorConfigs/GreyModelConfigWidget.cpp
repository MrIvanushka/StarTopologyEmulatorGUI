#include "GreyModelConfigWidget.h"

using namespace starTopologyEmulator;

GreyModelConfigWidget::GreyModelConfigWidget(QWidget* parent)
	: QWidget(parent)
{
	setupUi();
}

GreyModelIncomeStationsPredictorConfig GreyModelConfigWidget::getConfig() const
{
	GreyModelIncomeStationsPredictorConfig cfg;
	cfg.windowSize = static_cast<std::uint32_t>(_windowSizeSpin->value());
	cfg.minHistory = static_cast<std::uint32_t>(_minHistorySpin->value());
	cfg.minProbability = _minProbSpin->value();
	cfg.maxProbability = _maxProbSpin->value();
	cfg.epsilon = _epsilonSpin->value();
	return cfg;
}

void GreyModelConfigWidget::setConfig(const GreyModelIncomeStationsPredictorConfig& cfg)
{
	_windowSizeSpin->setValue(static_cast<int>(cfg.windowSize));
	_minHistorySpin->setValue(static_cast<int>(cfg.minHistory));
	_minProbSpin->setValue(cfg.minProbability);
	_maxProbSpin->setValue(cfg.maxProbability);
	_epsilonSpin->setValue(cfg.epsilon);
}

void GreyModelConfigWidget::setupUi()
{
	auto* mainLayout = new QFormLayout(this);

	_windowSizeSpin = new QSpinBox();
	_windowSizeSpin->setRange(1, 10000);

	_minHistorySpin = new QSpinBox();
	_minHistorySpin->setRange(1, 10000);

	_minProbSpin = new QDoubleSpinBox();
	_minProbSpin->setRange(0.0, 1.0);
	_minProbSpin->setSingleStep(0.01);
	_minProbSpin->setDecimals(4);

	_maxProbSpin = new QDoubleSpinBox();
	_maxProbSpin->setRange(0.0, 1.0);
	_maxProbSpin->setSingleStep(0.01);
	_maxProbSpin->setDecimals(4);

	_epsilonSpin = new QDoubleSpinBox();
	_epsilonSpin->setRange(1e-15, 1.0);
	_epsilonSpin->setDecimals(12);
	_epsilonSpin->setSingleStep(1e-10);

	mainLayout->addRow(QString::fromLocal8Bit("Размер окна:"), _windowSizeSpin);
	mainLayout->addRow(QString::fromLocal8Bit("Мин. история:"), _minHistorySpin);
	mainLayout->addRow(QString::fromLocal8Bit("Мин. вероятность:"), _minProbSpin);
	mainLayout->addRow(QString::fromLocal8Bit("Макс. вероятность:"), _maxProbSpin);
	mainLayout->addRow(QString::fromLocal8Bit("Эпсилон (точность):"), _epsilonSpin);

	GreyModelIncomeStationsPredictorConfig defaultConfig;
	setConfig(defaultConfig);
}