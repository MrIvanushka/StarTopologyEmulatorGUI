#include "PiIncomeLoadConfigWidget.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QVBoxLayout>

using namespace starTopologyEmulator;

PiIncomeLoadConfigWidget::PiIncomeLoadConfigWidget(QWidget* parent)
	: QWidget(parent)
{
	setupUi();
}

PiLoadControllerConfig PiIncomeLoadConfigWidget::getConfig() const
{
	PiLoadControllerConfig cfg;
	cfg.gTarget = _gTargetSpin->value();
	cfg.kP = _kPSpin->value();
	cfg.kI = _kISpin->value();
	cfg.integralWindowFrames = static_cast<std::uint32_t>(_integralWindowSpin->value());
	cfg.alpha = _alphaSpin->value();
	cfg.minProbability = _minProbabilitySpin->value();
	cfg.maxProbability = _maxProbabilitySpin->value();
	cfg.maxProbabilityStep = _maxProbabilityStepSpin->value();
	cfg.epsilon = _epsilonSpin->value();
	cfg.allowFeedForward = _allowFeedForwardBox->isChecked();

	cfg.backoffTemplate.pTx = _backoffPTxSpin->value();
	cfg.backoffTemplate.baseWindow = static_cast<std::uint8_t>(_backoffBaseWindowSpin->value());
	cfg.backoffTemplate.maxWindow = static_cast<std::uint8_t>(_backoffMaxWindowSpin->value());
	cfg.backoffTemplate.exponentBase = _backoffExponentBaseSpin->value();
	cfg.backoffTemplate.backoffType = static_cast<starTopologyEmulator::StarHubPlanMessage::BackoffType>(_backoffTypeCombo->currentIndex());
	cfg.backoffTemplate.additiveStep = static_cast<std::uint8_t>(_backoffAdditiveStepSpin->value());

	cfg.antiWindup = _antiWindupWidget->getConfig();
	return cfg;
}

void PiIncomeLoadConfigWidget::setConfig(const PiLoadControllerConfig& cfg)
{
	_gTargetSpin->setValue(cfg.gTarget);
	_kPSpin->setValue(cfg.kP);
	_kISpin->setValue(cfg.kI);
	_integralWindowSpin->setValue(cfg.integralWindowFrames);
	_alphaSpin->setValue(cfg.alpha);
	_minProbabilitySpin->setValue(cfg.minProbability);
	_maxProbabilitySpin->setValue(cfg.maxProbability);
	_maxProbabilityStepSpin->setValue(cfg.maxProbabilityStep);
	_epsilonSpin->setValue(cfg.epsilon);
	_allowFeedForwardBox->setChecked(cfg.allowFeedForward);

	_backoffPTxSpin->setValue(cfg.backoffTemplate.pTx);
	_backoffBaseWindowSpin->setValue(cfg.backoffTemplate.baseWindow);
	_backoffMaxWindowSpin->setValue(cfg.backoffTemplate.maxWindow);
	_backoffExponentBaseSpin->setValue(cfg.backoffTemplate.exponentBase);
	_backoffTypeCombo->setCurrentIndex(static_cast<int>(cfg.backoffTemplate.backoffType));
	_backoffAdditiveStepSpin->setValue(cfg.backoffTemplate.additiveStep);

	_antiWindupWidget->setConfig(cfg.antiWindup);
}

void PiIncomeLoadConfigWidget::setupUi()
{
	auto setupDouble = [](QDoubleSpinBox* sb, double min, double max, double step, int decimals) {
		sb->setRange(min, max);
		sb->setSingleStep(step);
		sb->setDecimals(decimals);
	};

	auto* mainLayout = new QVBoxLayout(this);

	auto* piGroup = new QGroupBox(QString::fromLocal8Bit("PI-регулятор входной нагрузки"), this);
	auto* piLayout = new QFormLayout(piGroup);

	_gTargetSpin = new QDoubleSpinBox();
	_kPSpin = new QDoubleSpinBox();
	_kISpin = new QDoubleSpinBox();
	_integralWindowSpin = new QSpinBox();
	_alphaSpin = new QDoubleSpinBox();
	_allowFeedForwardBox = new QCheckBox();
	setupDouble(_gTargetSpin, 0.0, 10.0, 0.01, 4);
	setupDouble(_kPSpin, 0.0, 100.0, 0.01, 4);
	setupDouble(_kISpin, 0.0, 100.0, 0.001, 4);
	_integralWindowSpin->setRange(1, 10000);
	setupDouble(_alphaSpin, 0.0, 100.0, 0.1, 4);
	piLayout->addRow(QString::fromLocal8Bit("Целевая нагрузка (gTarget): "), _gTargetSpin);
	piLayout->addRow(QString::fromLocal8Bit("Пропорциональный коэффициент (kP): "), _kPSpin);
	piLayout->addRow(QString::fromLocal8Bit("Интегральный коэффициент (kI): "), _kISpin);
	piLayout->addRow(QString::fromLocal8Bit("Окно интегрирования (фреймов): "), _integralWindowSpin);
	piLayout->addRow(QString::fromLocal8Bit("Параметр alpha: "), _alphaSpin);
	piLayout->addRow(QString::fromLocal8Bit("Feedforward по оценке N (pFF = R / N_hat): "), _allowFeedForwardBox);

	auto* probGroup = new QGroupBox(QString::fromLocal8Bit("Ограничения вероятности"), this);
	auto* probLayout = new QFormLayout(probGroup);
	_minProbabilitySpin = new QDoubleSpinBox();
	_maxProbabilitySpin = new QDoubleSpinBox();
	_maxProbabilityStepSpin = new QDoubleSpinBox();
	_epsilonSpin = new QDoubleSpinBox();
	setupDouble(_minProbabilitySpin, 0.0, 1.0, 0.001, 6);
	setupDouble(_maxProbabilitySpin, 0.0, 1.0, 0.01, 4);
	setupDouble(_maxProbabilityStepSpin, 0.0, 1.0, 0.01, 4);
	setupDouble(_epsilonSpin, 0.0, 1.0, 1e-6, 9);
	probLayout->addRow(QString::fromLocal8Bit("Минимальная вероятность (pMin): "), _minProbabilitySpin);
	probLayout->addRow(QString::fromLocal8Bit("Максимальная вероятность (pMax): "), _maxProbabilitySpin);
	probLayout->addRow(QString::fromLocal8Bit("Максимальный шаг (maxProbabilityStep): "), _maxProbabilityStepSpin);
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

	auto* antiWindupGroup = new QGroupBox(QString::fromLocal8Bit("Anti-windup интегратора"), this);
	auto* antiWindupLayout = new QVBoxLayout(antiWindupGroup);
	_antiWindupWidget = new PiAntiWindupConfigWidget(antiWindupGroup);
	antiWindupLayout->addWidget(_antiWindupWidget);

	mainLayout->addWidget(piGroup);
	mainLayout->addWidget(probGroup);
	mainLayout->addWidget(backoffGroup);
	mainLayout->addWidget(antiWindupGroup);
	mainLayout->addStretch();

	setConfig(PiLoadControllerConfig());
}
