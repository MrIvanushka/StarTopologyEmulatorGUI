#include "LyapunovFtpGeneratorConfigWidget.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QVBoxLayout>

using namespace starTopologyEmulator;

LyapunovFtpGeneratorConfigWidget::LyapunovFtpGeneratorConfigWidget(QWidget* parent)
	: QWidget(parent)
{
	setupUi();
}

LyapunovFtpGeneratorConfig LyapunovFtpGeneratorConfigWidget::getConfig() const
{
	LyapunovFtpGeneratorConfig cfg;
	cfg.V = _vSpin->value();
	cfg.raMin = static_cast<std::uint8_t>(_raMinSpin->value());
	cfg.raMax = static_cast<std::uint8_t>(_raMaxSpin->value());
	cfg.yellowSlots = static_cast<std::uint8_t>(_yellowSlotsSpin->value());
	return cfg;
}

void LyapunovFtpGeneratorConfigWidget::setConfig(const LyapunovFtpGeneratorConfig& cfg)
{
	_vSpin->setValue(cfg.V);
	_raMinSpin->setValue(cfg.raMin);
	_raMaxSpin->setValue(cfg.raMax);
	_yellowSlotsSpin->setValue(cfg.yellowSlots);
}

void LyapunovFtpGeneratorConfigWidget::setupUi()
{
	auto* mainLayout = new QVBoxLayout(this);

	auto* group = new QGroupBox(QString::fromLocal8Bit("Параметры Lyapunov-генератора"), this);
	auto* form = new QFormLayout(group);

	_vSpin = new QDoubleSpinBox();
	_vSpin->setRange(0.0, 100000.0);
	_vSpin->setSingleStep(0.1);
	_vSpin->setDecimals(4);

	_raMinSpin = new QSpinBox();
	_raMaxSpin = new QSpinBox();
	_yellowSlotsSpin = new QSpinBox();
	_raMinSpin->setRange(0, 255);
	_raMaxSpin->setRange(0, 255);
	_yellowSlotsSpin->setRange(0, 255);

	form->addRow(QString::fromLocal8Bit("Коэффициент V (Lyapunov drift-plus-penalty): "), _vSpin);
	form->addRow(QString::fromLocal8Bit("Минимум RA-слотов в кадре: "), _raMinSpin);
	form->addRow(QString::fromLocal8Bit("Максимум RA-слотов в кадре: "), _raMaxSpin);
	form->addRow(QString::fromLocal8Bit("Жёлтых слотов в кадре: "), _yellowSlotsSpin);

	mainLayout->addWidget(group);
	mainLayout->addStretch();

	setConfig(LyapunovFtpGeneratorConfig());
}