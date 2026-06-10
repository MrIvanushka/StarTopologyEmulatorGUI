#include "CollisionBudgetIncomeLoadConfigWidget.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QVBoxLayout>

using namespace starTopologyEmulator;

CollisionBudgetIncomeLoadConfigWidget::CollisionBudgetIncomeLoadConfigWidget(QWidget* parent)
	: QWidget(parent)
{
	setupUi();
}

CollisionBudgetLoadControllerConfig CollisionBudgetIncomeLoadConfigWidget::getConfig() const
{
	CollisionBudgetLoadControllerConfig cfg;
	cfg.collisionBudget = _collisionBudgetSpin->value();
	cfg.gradientStep = _gradientStepSpin->value();
	cfg.lagrangianStep = _lagrangianStepSpin->value();
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

void CollisionBudgetIncomeLoadConfigWidget::setConfig(const CollisionBudgetLoadControllerConfig& cfg)
{
	_collisionBudgetSpin->setValue(cfg.collisionBudget);
	_gradientStepSpin->setValue(cfg.gradientStep);
	_lagrangianStepSpin->setValue(cfg.lagrangianStep);
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

void CollisionBudgetIncomeLoadConfigWidget::setupUi()
{
	auto setupDouble = [](QDoubleSpinBox* sb, double min, double max, double step, int decimals) {
		sb->setRange(min, max);
		sb->setSingleStep(step);
		sb->setDecimals(decimals);
	};

	auto* mainLayout = new QVBoxLayout(this);

	auto* utilGroup = new QGroupBox(QString::fromLocal8Bit("Параметры бюджета коллизий"), this);
	auto* utilLayout = new QFormLayout(utilGroup);

	_collisionBudgetSpin = new QDoubleSpinBox();
	_gradientStepSpin = new QDoubleSpinBox();
	_lagrangianStepSpin = new QDoubleSpinBox();
	setupDouble(_collisionBudgetSpin, 0.0, 10.0, 0.01, 4);
	setupDouble(_gradientStepSpin, 0.0, 10.0, 0.01, 4);
	setupDouble(_lagrangianStepSpin, 0.0, 10.0, 0.01, 4);
	utilLayout->addRow(QString::fromLocal8Bit("Бюджет коллизий (collisionBudget): "), _collisionBudgetSpin);
	utilLayout->addRow(QString::fromLocal8Bit("Шаг градиента (eta): "), _gradientStepSpin);
	utilLayout->addRow(QString::fromLocal8Bit("Шаг лагранжиана: "), _lagrangianStepSpin);

	auto* probGroup = new QGroupBox(QString::fromLocal8Bit("Ограничения вероятности"), this);
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
	backoffLayout->addRow(QString::fromLocal8Bit("Основание экспоненты: "), _backoffExponentBaseSpin);
	backoffLayout->addRow(QString::fromLocal8Bit("Backoff type: "), _backoffTypeCombo);
	backoffLayout->addRow(QString::fromLocal8Bit("LMILD additive step: "), _backoffAdditiveStepSpin);

	mainLayout->addWidget(utilGroup);
	mainLayout->addWidget(probGroup);
	mainLayout->addWidget(backoffGroup);
	mainLayout->addStretch();

	setConfig(CollisionBudgetLoadControllerConfig());
}