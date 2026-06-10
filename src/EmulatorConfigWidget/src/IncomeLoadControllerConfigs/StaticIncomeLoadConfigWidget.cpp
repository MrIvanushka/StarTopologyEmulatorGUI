#include "StaticIncomeLoadConfigWidget.h"

#include <QFormLayout>
#include <QGroupBox>

using namespace starTopologyEmulator;

StaticIncomeLoadConfigWidget::StaticIncomeLoadConfigWidget(QWidget* parent)
	: QWidget(parent)
{
	setupUi();
}

starTopologyEmulator::StarHubPlanMessage::BackoffConfig StaticIncomeLoadConfigWidget::getConfig() const
{
	StarHubPlanMessage::BackoffConfig cfg;
	cfg.pTx            = _pTxSpin->value();
	cfg.baseWindow     = static_cast<std::uint8_t>(_baseWindowSpin->value());
	cfg.maxWindow      = static_cast<std::uint8_t>(_maxWindowSpin->value());
	cfg.backoffType = static_cast<StarHubPlanMessage::BackoffType>(_backoffTypeCombo->currentIndex());
	cfg.additiveStep = static_cast<std::uint8_t>(_backoffAdditiveStepSpin->value());
	cfg.exponentBase   = _exponentBaseSpin->value();
	return cfg;
}

void StaticIncomeLoadConfigWidget::setConfig(const starTopologyEmulator::StarHubPlanMessage::BackoffConfig& cfg)
{
	_pTxSpin->setValue(cfg.pTx);
	_baseWindowSpin->setValue(cfg.baseWindow);
	_maxWindowSpin->setValue(cfg.maxWindow);
	_backoffTypeCombo->setCurrentIndex(static_cast<int>(cfg.backoffType));
	_backoffAdditiveStepSpin->setValue(cfg.additiveStep);
	_exponentBaseSpin->setValue(cfg.exponentBase);
	_exponentBaseSpin->setEnabled(cfg.backoffType != StarHubPlanMessage::BackoffType::NONE);
	_maxWindowSpin->setEnabled(cfg.backoffType != StarHubPlanMessage::BackoffType::NONE);
}

void StaticIncomeLoadConfigWidget::setupUi()
{
	auto* mainLayout = new QFormLayout(this);

	_pTxSpin = new QDoubleSpinBox(this);
	_pTxSpin->setRange(0.0, 1.0);
	_pTxSpin->setSingleStep(0.01);
	_pTxSpin->setDecimals(3);

	_baseWindowSpin = new QSpinBox(this);
	_baseWindowSpin->setRange(1, 255);

	_backoffTypeCombo = new QComboBox(this);
	_backoffTypeCombo->addItems({QStringLiteral("NONE"), QStringLiteral("BEB"), QStringLiteral("MILD"), QStringLiteral("LMILD")});
	_backoffAdditiveStepSpin = new QSpinBox(this);
	_backoffAdditiveStepSpin->setRange(1, 255);

	_exponentBaseSpin = new QDoubleSpinBox(this);
	_exponentBaseSpin->setRange(1.0, 10.0);
	_exponentBaseSpin->setSingleStep(0.1);
	_exponentBaseSpin->setDecimals(2);

	_maxWindowSpin = new QSpinBox(this);
	_maxWindowSpin->setRange(1, 255);

	QObject::connect(_backoffTypeCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int idx) {
		const bool on = idx != static_cast<int>(StarHubPlanMessage::BackoffType::NONE);
		_exponentBaseSpin->setEnabled(on);
		_maxWindowSpin->setEnabled(on);
	});

	mainLayout->addRow(QString::fromLocal8Bit("Вероятность вещания АС: "),         _pTxSpin);
	mainLayout->addRow(QString::fromLocal8Bit("Базовая ширина окна backoff: "),    _baseWindowSpin);
	mainLayout->addRow(QString::fromLocal8Bit("Backoff type: "),       _backoffTypeCombo);
	mainLayout->addRow(QString::fromLocal8Bit("LMILD additive step: "),   _backoffAdditiveStepSpin);
	mainLayout->addRow(QString::fromLocal8Bit("Основание экспоненты: "),           _exponentBaseSpin);
	mainLayout->addRow(QString::fromLocal8Bit("Максимальная ширина окна: "),       _maxWindowSpin);

	setConfig(StarHubPlanMessage::BackoffConfig());
}