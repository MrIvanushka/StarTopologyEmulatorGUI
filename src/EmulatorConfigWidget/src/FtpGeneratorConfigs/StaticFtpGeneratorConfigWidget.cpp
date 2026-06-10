#include "StaticFtpGeneratorConfigWidget.h"

using namespace starTopologyEmulator;

StaticFtpGeneratorConfigWidget::StaticFtpGeneratorConfigWidget(QWidget* parent)
	: QWidget(parent)
{
	setupUi();
}

StarHubPlanMessage::FtpConfig StaticFtpGeneratorConfigWidget::getConfig() const
{
	StarHubPlanMessage::FtpConfig cfg;
	cfg.randomAccessSlotsCountInFrame = _raSlotsSpin->value();
	cfg.yellowSlotsCountInFrame = _yellowSlotsSpin->value();
	cfg.onlineSlotsCountInFrame = _onlineSlotsSpin->value();
	return cfg;
}

void StaticFtpGeneratorConfigWidget::setConfig(const StarHubPlanMessage::FtpConfig& cfg)
{
	_raSlotsSpin->setValue(cfg.randomAccessSlotsCountInFrame);
	_yellowSlotsSpin->setValue(cfg.yellowSlotsCountInFrame);
	_onlineSlotsSpin->setValue(cfg.onlineSlotsCountInFrame);
}

void StaticFtpGeneratorConfigWidget::setupUi()
{
	auto* mainLayout = new QFormLayout(this);

	// Окно регрессии (uint32)
	_raSlotsSpin = new QSpinBox();
	_raSlotsSpin->setRange(0, 100);

	_yellowSlotsSpin = new QSpinBox();
	_yellowSlotsSpin->setRange(0, 100);

	_onlineSlotsSpin = new QSpinBox();
	_onlineSlotsSpin->setRange(0, 100);

	// Добавляем строки в форму
	mainLayout->addRow(QString::fromLocal8Bit("Слотов случайного доступа:"), _raSlotsSpin);
	mainLayout->addRow(QString::fromLocal8Bit("Слотов для аутентификации:"), _yellowSlotsSpin);
	mainLayout->addRow(QString::fromLocal8Bit("Слотов для вошедших станций:"), _onlineSlotsSpin);

	// Установка значений по умолчанию из структуры
	StarHubPlanMessage::FtpConfig defaultConfig;
	setConfig(defaultConfig);
}