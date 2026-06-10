#include "ServiceDelayFtpGeneratorConfigWidget.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QVBoxLayout>

using namespace starTopologyEmulator;

ServiceDelayFtpGeneratorConfigWidget::ServiceDelayFtpGeneratorConfigWidget(QWidget* parent)
	: QWidget(parent)
{
	setupUi();
}

ServiceDelayFtpGeneratorConfig ServiceDelayFtpGeneratorConfigWidget::getConfig() const
{
	ServiceDelayFtpGeneratorConfig cfg;
	cfg.d0 = _d0Spin->value();
	cfg.lambdaRatio = _lambdaRatioSpin->value();
	cfg.raMin = static_cast<std::uint8_t>(_raMinSpin->value());
	cfg.raMax = static_cast<std::uint8_t>(_raMaxSpin->value());
	cfg.yellowSlots = static_cast<std::uint8_t>(_yellowSlotsSpin->value());
	return cfg;
}

void ServiceDelayFtpGeneratorConfigWidget::setConfig(const ServiceDelayFtpGeneratorConfig& cfg)
{
	_d0Spin->setValue(cfg.d0);
	_lambdaRatioSpin->setValue(cfg.lambdaRatio);
	_raMinSpin->setValue(cfg.raMin);
	_raMaxSpin->setValue(cfg.raMax);
	_yellowSlotsSpin->setValue(cfg.yellowSlots);
}

void ServiceDelayFtpGeneratorConfigWidget::setupUi()
{
	auto* mainLayout = new QVBoxLayout(this);

	auto* group = new QGroupBox(QString::fromLocal8Bit("Параметры выравнивания задержки сервиса"), this);
	auto* form = new QFormLayout(group);

	_d0Spin = new QDoubleSpinBox();
	_lambdaRatioSpin = new QDoubleSpinBox();
	_d0Spin->setRange(0.0, 10000.0);
	_d0Spin->setSingleStep(0.1);
	_d0Spin->setDecimals(4);
	_lambdaRatioSpin->setRange(0.0, 1000.0);
	_lambdaRatioSpin->setSingleStep(0.01);
	_lambdaRatioSpin->setDecimals(4);

	_raMinSpin = new QSpinBox();
	_raMaxSpin = new QSpinBox();
	_yellowSlotsSpin = new QSpinBox();
	_raMinSpin->setRange(0, 255);
	_raMaxSpin->setRange(0, 255);
	_yellowSlotsSpin->setRange(0, 255);

	form->addRow(QString::fromLocal8Bit("Базовое смещение задержки (d0): "), _d0Spin);
	form->addRow(QString::fromLocal8Bit("Отношение интенсивностей (lambdaRatio): "), _lambdaRatioSpin);
	form->addRow(QString::fromLocal8Bit("Минимум RA-слотов в кадре: "), _raMinSpin);
	form->addRow(QString::fromLocal8Bit("Максимум RA-слотов в кадре: "), _raMaxSpin);
	form->addRow(QString::fromLocal8Bit("Жёлтых слотов в кадре: "), _yellowSlotsSpin);

	mainLayout->addWidget(group);
	mainLayout->addStretch();

	setConfig(ServiceDelayFtpGeneratorConfig());
}