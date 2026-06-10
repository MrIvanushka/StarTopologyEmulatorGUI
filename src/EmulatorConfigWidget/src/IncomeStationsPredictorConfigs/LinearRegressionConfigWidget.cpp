#include "LinearRegressionConfigWidget.h"

using namespace starTopologyEmulator;

LinearRegressionConfigWidget::LinearRegressionConfigWidget(QWidget* parent)
	: QWidget(parent)
{
	setupUi();
}

LinearRegressionIncomeStationsPredictorConfig LinearRegressionConfigWidget::getConfig() const
{
	LinearRegressionIncomeStationsPredictorConfig cfg;
	cfg.regressionWindow = static_cast<std::uint32_t>(_regressionWindowSpin->value());
	cfg.forgettingHorizonSec = _forgettingHorizonSpin->value();
	cfg.minProbability = _minProbSpin->value();
	cfg.maxProbability = _maxProbSpin->value();
	cfg.epsilon = _epsilonSpin->value();
	return cfg;
}

void LinearRegressionConfigWidget::setConfig(const LinearRegressionIncomeStationsPredictorConfig& cfg)
{
	_regressionWindowSpin->setValue(cfg.regressionWindow);
	_forgettingHorizonSpin->setValue(cfg.forgettingHorizonSec);
	_minProbSpin->setValue(cfg.minProbability);
	_maxProbSpin->setValue(cfg.maxProbability);
	_epsilonSpin->setValue(cfg.epsilon);
}

void LinearRegressionConfigWidget::setupUi()
{
	auto* mainLayout = new QFormLayout(this);

	// Окно регрессии (uint32)
	_regressionWindowSpin = new QSpinBox();
	_regressionWindowSpin->setRange(1, 10000);

	// Горизонт забывания (double)
	_forgettingHorizonSpin = new QDoubleSpinBox();
	_forgettingHorizonSpin->setRange(0.1, 3600.0);
	_forgettingHorizonSpin->setSuffix(QString::fromLocal8Bit(" сек"));
	_forgettingHorizonSpin->setDecimals(2);

	// Минимальная вероятность (double)
	_minProbSpin = new QDoubleSpinBox();
	_minProbSpin->setRange(0.0, 1.0);
	_minProbSpin->setSingleStep(0.01);
	_minProbSpin->setDecimals(4);

	// Максимальная вероятность (double)
	_maxProbSpin = new QDoubleSpinBox();
	_maxProbSpin->setRange(0.0, 1.0);
	_maxProbSpin->setSingleStep(0.01);
	_maxProbSpin->setDecimals(4);

	// Эпсилон (double) - здесь нужна высокая точность
	_epsilonSpin = new QDoubleSpinBox();
	_epsilonSpin->setRange(1e-15, 1.0);
	_epsilonSpin->setDecimals(12); // Увеличиваем количество знаков
	_epsilonSpin->setSingleStep(1e-10);

	// Добавляем строки в форму
	mainLayout->addRow(QString::fromLocal8Bit("Окно регрессии:"), _regressionWindowSpin);
	mainLayout->addRow(QString::fromLocal8Bit("Горизонт забывания:"), _forgettingHorizonSpin);
	mainLayout->addRow(QString::fromLocal8Bit("Мин. вероятность:"), _minProbSpin);
	mainLayout->addRow(QString::fromLocal8Bit("Макс. вероятность:"), _maxProbSpin);
	mainLayout->addRow(QString::fromLocal8Bit("Эпсилон (точность):"), _epsilonSpin);

	// Установка значений по умолчанию из структуры
	LinearRegressionIncomeStationsPredictorConfig defaultConfig;
	setConfig(defaultConfig);
}