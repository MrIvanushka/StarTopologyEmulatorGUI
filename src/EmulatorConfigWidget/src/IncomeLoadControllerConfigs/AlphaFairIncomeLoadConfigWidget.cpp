#include "AlphaFairIncomeLoadConfigWidget.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QVBoxLayout>

using namespace starTopologyEmulator;

AlphaFairIncomeLoadConfigWidget::AlphaFairIncomeLoadConfigWidget(QWidget* parent)
	: QWidget(parent)
{
	setupUi();
}

AlphaFairLoadControllerConfig AlphaFairIncomeLoadConfigWidget::getConfig() const
{
	AlphaFairLoadControllerConfig cfg;
	cfg.alpha = _alphaSpin->value();
	cfg.gradientStep = _gradientStepSpin->value();
	cfg.maxProbabilityStep = _maxProbabilityStepSpin->value();
	cfg.minProbability = _minProbabilitySpin->value();
	cfg.maxProbability = _maxProbabilitySpin->value();
	cfg.epsilon = _epsilonSpin->value();

	cfg.backoffTemplate.pTx = _backoffPTxSpin->value();
	cfg.backoffTemplate.baseWindow = static_cast<std::uint8_t>(_backoffBaseWindowSpin->value());
	cfg.backoffTemplate.maxWindow = static_cast<std::uint8_t>(_backoffMaxWindowSpin->value());
	cfg.backoffTemplate.exponentBase = _backoffExponentBaseSpin->value();
	cfg.backoffTemplate.backoffType = static_cast<starTopologyEmulator::StarHubPlanMessage::BackoffType>(_backoffTypeCombo->currentIndex());
	cfg.backoffTemplate.additiveStep = static_cast<std::uint8_t>(_backoffAdditiveStepSpin->value());
	return cfg;
}

void AlphaFairIncomeLoadConfigWidget::setConfig(const AlphaFairLoadControllerConfig& cfg)
{
	_alphaSpin->setValue(cfg.alpha);
	_gradientStepSpin->setValue(cfg.gradientStep);
	_maxProbabilityStepSpin->setValue(cfg.maxProbabilityStep);
	_minProbabilitySpin->setValue(cfg.minProbability);
	_maxProbabilitySpin->setValue(cfg.maxProbability);
	_epsilonSpin->setValue(cfg.epsilon);

	_backoffPTxSpin->setValue(cfg.backoffTemplate.pTx);
	_backoffBaseWindowSpin->setValue(cfg.backoffTemplate.baseWindow);
	_backoffMaxWindowSpin->setValue(cfg.backoffTemplate.maxWindow);
	_backoffExponentBaseSpin->setValue(cfg.backoffTemplate.exponentBase);
	_backoffTypeCombo->setCurrentIndex(static_cast<int>(cfg.backoffTemplate.backoffType));
	_backoffAdditiveStepSpin->setValue(cfg.backoffTemplate.additiveStep);
}

void AlphaFairIncomeLoadConfigWidget::setupUi()
{
	auto setupDouble = [](QDoubleSpinBox* sb, double min, double max, double step, int decimals) {
		sb->setRange(min, max);
		sb->setSingleStep(step);
		sb->setDecimals(decimals);
	};

	auto* mainLayout = new QVBoxLayout(this);

	auto* utilityGroup = new QGroupBox(QString::fromLocal8Bit("Параметры alpha-fair полезности"), this);
	auto* utilityLayout = new QFormLayout(utilityGroup);

	_alphaSpin = new QDoubleSpinBox();
	_gradientStepSpin = new QDoubleSpinBox();
	setupDouble(_alphaSpin, 0.0, 1000.0, 0.1, 3);
	setupDouble(_gradientStepSpin, 0.0, 10.0, 0.01, 4);
	utilityLayout->addRow(QString::fromLocal8Bit("Параметр alpha: "), _alphaSpin);
	utilityLayout->addRow(QString::fromLocal8Bit("Шаг градиента (eta): "), _gradientStepSpin);

	auto* probGroup = new QGroupBox(QString::fromLocal8Bit("Вероятность передачи"), this);
	auto* probLayout = new QFormLayout(probGroup);

	_maxProbabilityStepSpin = new QDoubleSpinBox();
	_minProbabilitySpin = new QDoubleSpinBox();
	_maxProbabilitySpin = new QDoubleSpinBox();
	_epsilonSpin = new QDoubleSpinBox();
	setupDouble(_maxProbabilityStepSpin, 0.0, 1.0, 0.01, 4);
	setupDouble(_minProbabilitySpin, 0.0, 1.0, 0.001, 6);
	setupDouble(_maxProbabilitySpin, 0.0, 1.0, 0.01, 4);
	setupDouble(_epsilonSpin, 0.0, 1.0, 1e-6, 9);

	probLayout->addRow(QString::fromLocal8Bit("Максимальный шаг (maxProbabilityStep): "), _maxProbabilityStepSpin);
	probLayout->addRow(QString::fromLocal8Bit("Минимальная вероятность (pMin): "), _minProbabilitySpin);
	probLayout->addRow(QString::fromLocal8Bit("Максимальная вероятность (pMax): "), _maxProbabilitySpin);
	probLayout->addRow(QString::fromLocal8Bit("Регуляризатор (epsilon): "), _epsilonSpin);

	auto* backoffGroup = new QGroupBox(QString::fromLocal8Bit("Шаблон backoff"), this);
	auto* backoffLayout = new QFormLayout(backoffGroup);
	_backoffPTxSpin = new QDoubleSpinBox();
	_backoffBaseWindowSpin = new QSpinBox();
	_backoffMaxWindowSpin = new QSpinBox();
	_backoffExponentBaseSpin = new QDoubleSpinBox();
	_backoffTypeCombo = new QComboBox();
	_backoffTypeCombo->addItems({QStringLiteral("NONE"), QStringLiteral("BEB"), QStringLiteral("MILD"), QStringLiteral("LMILD")});
	_backoffAdditiveStepSpin = new QSpinBox();
	_backoffAdditiveStepSpin->setRange(1, 255);
	setupDouble(_backoffPTxSpin, 0.0, 1.0, 0.01, 3);
	_backoffBaseWindowSpin->setRange(1, 255);
	_backoffMaxWindowSpin->setRange(1, 255);
	setupDouble(_backoffExponentBaseSpin, 1.0, 10.0, 0.1, 3);
	backoffLayout->addRow(QString::fromLocal8Bit("Вероятность передачи (pTx): "), _backoffPTxSpin);
	backoffLayout->addRow(QString::fromLocal8Bit("Базовое окно backoff: "), _backoffBaseWindowSpin);
	backoffLayout->addRow(QString::fromLocal8Bit("Максимальное окно backoff: "), _backoffMaxWindowSpin);
	backoffLayout->addRow(QString::fromLocal8Bit("Основание показателя: "), _backoffExponentBaseSpin);
	backoffLayout->addRow(QString::fromLocal8Bit("Backoff type: "), _backoffTypeCombo);
	backoffLayout->addRow(QString::fromLocal8Bit("LMILD additive step: "), _backoffAdditiveStepSpin);

	mainLayout->addWidget(utilityGroup);
	mainLayout->addWidget(probGroup);
	mainLayout->addWidget(backoffGroup);
	mainLayout->addStretch();

	setConfig(AlphaFairLoadControllerConfig());
}