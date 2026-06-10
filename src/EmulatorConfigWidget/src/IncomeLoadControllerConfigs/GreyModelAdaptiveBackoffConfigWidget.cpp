#include "GreyModelAdaptiveBackoffConfigWidget.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QVBoxLayout>

using namespace starTopologyEmulator;

GreyModelAdaptiveBackoffConfigWidget::GreyModelAdaptiveBackoffConfigWidget(QWidget* parent)
	: QWidget(parent)
{
	setupUi();
}

GreyModelAdaptiveBackoffControllerConfig GreyModelAdaptiveBackoffConfigWidget::getConfig() const
{
	GreyModelAdaptiveBackoffControllerConfig cfg;
	cfg.gTarget = _gTargetSpin->value();
	cfg.historySize = static_cast<std::uint32_t>(_historySizeSpin->value());
	cfg.minHistoryForPrediction = static_cast<std::uint32_t>(_minHistoryForPredictionSpin->value());
	cfg.minBackoffWindow = static_cast<std::uint8_t>(_minBackoffWindowSpin->value());
	cfg.maxBackoffWindow = static_cast<std::uint8_t>(_maxBackoffWindowSpin->value());
	cfg.epsilon = _epsilonSpin->value();

	cfg.backoffTemplate.pTx = _backoffPTxSpin->value();
	cfg.backoffTemplate.baseWindow = static_cast<std::uint8_t>(_backoffBaseWindowSpin->value());
	cfg.backoffTemplate.maxWindow = static_cast<std::uint8_t>(_backoffMaxWindowSpin->value());
	cfg.backoffTemplate.exponentBase = _backoffExponentBaseSpin->value();
	cfg.backoffTemplate.backoffType = static_cast<starTopologyEmulator::StarHubPlanMessage::BackoffType>(_backoffTypeCombo->currentIndex());
	cfg.backoffTemplate.additiveStep = static_cast<std::uint8_t>(_backoffAdditiveStepSpin->value());
	return cfg;
}

void GreyModelAdaptiveBackoffConfigWidget::setConfig(const GreyModelAdaptiveBackoffControllerConfig& cfg)
{
	_gTargetSpin->setValue(cfg.gTarget);
	_historySizeSpin->setValue(cfg.historySize);
	_minHistoryForPredictionSpin->setValue(cfg.minHistoryForPrediction);
	_minBackoffWindowSpin->setValue(cfg.minBackoffWindow);
	_maxBackoffWindowSpin->setValue(cfg.maxBackoffWindow);
	_epsilonSpin->setValue(cfg.epsilon);

	_backoffPTxSpin->setValue(cfg.backoffTemplate.pTx);
	_backoffBaseWindowSpin->setValue(cfg.backoffTemplate.baseWindow);
	_backoffMaxWindowSpin->setValue(cfg.backoffTemplate.maxWindow);
	_backoffExponentBaseSpin->setValue(cfg.backoffTemplate.exponentBase);
	_backoffTypeCombo->setCurrentIndex(static_cast<int>(cfg.backoffTemplate.backoffType));
	_backoffAdditiveStepSpin->setValue(cfg.backoffTemplate.additiveStep);
}

void GreyModelAdaptiveBackoffConfigWidget::setupUi()
{
	auto setupDouble = [](QDoubleSpinBox* sb, double min, double max, double step, int decimals) {
		sb->setRange(min, max);
		sb->setSingleStep(step);
		sb->setDecimals(decimals);
	};

	auto* mainLayout = new QVBoxLayout(this);

	auto* greyGroup = new QGroupBox(QString::fromLocal8Bit("Параметры серой модели GM(1,1)"), this);
	auto* greyLayout = new QFormLayout(greyGroup);
	_gTargetSpin = new QDoubleSpinBox();
	_historySizeSpin = new QSpinBox();
	_minHistoryForPredictionSpin = new QSpinBox();
	_minBackoffWindowSpin = new QSpinBox();
	_maxBackoffWindowSpin = new QSpinBox();
	_epsilonSpin = new QDoubleSpinBox();
	setupDouble(_gTargetSpin, 0.0, 10.0, 0.01, 4);
	_historySizeSpin->setRange(1, 1000);
	_minHistoryForPredictionSpin->setRange(1, 1000);
	_minBackoffWindowSpin->setRange(1, 255);
	_maxBackoffWindowSpin->setRange(1, 255);
	setupDouble(_epsilonSpin, 0.0, 1.0, 1e-6, 9);
	greyLayout->addRow(QString::fromLocal8Bit("Целевая нагрузка (gTarget): "), _gTargetSpin);
	greyLayout->addRow(QString::fromLocal8Bit("Размер истории: "), _historySizeSpin);
	greyLayout->addRow(QString::fromLocal8Bit("Минимум выборок для предсказания: "), _minHistoryForPredictionSpin);
	greyLayout->addRow(QString::fromLocal8Bit("Минимальное окно backoff: "), _minBackoffWindowSpin);
	greyLayout->addRow(QString::fromLocal8Bit("Максимальное окно backoff: "), _maxBackoffWindowSpin);
	greyLayout->addRow(QString::fromLocal8Bit("Регуляризатор (epsilon): "), _epsilonSpin);

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

	mainLayout->addWidget(greyGroup);
	mainLayout->addWidget(backoffGroup);
	mainLayout->addStretch();

	setConfig(GreyModelAdaptiveBackoffControllerConfig());
}