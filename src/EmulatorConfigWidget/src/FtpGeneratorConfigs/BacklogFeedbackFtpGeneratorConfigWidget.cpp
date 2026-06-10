#include "BacklogFeedbackFtpGeneratorConfigWidget.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QVBoxLayout>

using namespace starTopologyEmulator;

BacklogFeedbackFtpGeneratorConfigWidget::BacklogFeedbackFtpGeneratorConfigWidget(QWidget* parent)
	: QWidget(parent)
{
	setupUi();
}

BacklogFeedbackFtpGeneratorConfig BacklogFeedbackFtpGeneratorConfigWidget::getConfig() const
{
	BacklogFeedbackFtpGeneratorConfig cfg;
	cfg.rhoAuth = _rhoAuthSpin->value();
	cfg.R0 = _r0Spin->value();
	cfg.kJ = _kJSpin->value();
	cfg.kQ = _kQSpin->value();
	cfg.jStar = _jStarSpin->value();
	cfg.qStar = _qStarSpin->value();
	cfg.deltaR = _deltaRSpin->value();
	cfg.raMin = static_cast<std::uint8_t>(_raMinSpin->value());
	cfg.raMax = static_cast<std::uint8_t>(_raMaxSpin->value());
	cfg.yellowSlots = static_cast<std::uint8_t>(_yellowSlotsSpin->value());
	return cfg;
}

void BacklogFeedbackFtpGeneratorConfigWidget::setConfig(const BacklogFeedbackFtpGeneratorConfig& cfg)
{
	_rhoAuthSpin->setValue(cfg.rhoAuth);
	_r0Spin->setValue(cfg.R0);
	_kJSpin->setValue(cfg.kJ);
	_kQSpin->setValue(cfg.kQ);
	_jStarSpin->setValue(cfg.jStar);
	_qStarSpin->setValue(cfg.qStar);
	_deltaRSpin->setValue(cfg.deltaR);
	_raMinSpin->setValue(cfg.raMin);
	_raMaxSpin->setValue(cfg.raMax);
	_yellowSlotsSpin->setValue(cfg.yellowSlots);
}

void BacklogFeedbackFtpGeneratorConfigWidget::setupUi()
{
	auto setupDouble = [](QDoubleSpinBox* sb, double min, double max, double step, int decimals) {
		sb->setRange(min, max);
		sb->setSingleStep(step);
		sb->setDecimals(decimals);
	};

	auto* mainLayout = new QVBoxLayout(this);

	auto* group = new QGroupBox(QString::fromLocal8Bit("Параметры обратной связи по бэклогу"), this);
	auto* form = new QFormLayout(group);

	_rhoAuthSpin = new QDoubleSpinBox();
	_r0Spin = new QDoubleSpinBox();
	_kJSpin = new QDoubleSpinBox();
	_kQSpin = new QDoubleSpinBox();
	_jStarSpin = new QDoubleSpinBox();
	_qStarSpin = new QDoubleSpinBox();
	_deltaRSpin = new QDoubleSpinBox();
	_raMinSpin = new QSpinBox();
	_raMaxSpin = new QSpinBox();
	_yellowSlotsSpin = new QSpinBox();

	setupDouble(_rhoAuthSpin, 0.0, 1000.0, 0.1, 3);
	setupDouble(_r0Spin, 0.0, 10000.0, 1.0, 3);
	setupDouble(_kJSpin, 0.0, 1000.0, 0.1, 4);
	setupDouble(_kQSpin, 0.0, 1000.0, 0.01, 4);
	setupDouble(_jStarSpin, 0.0, 100000.0, 1.0, 3);
	setupDouble(_qStarSpin, 0.0, 100000.0, 1.0, 3);
	setupDouble(_deltaRSpin, 0.0, 1000.0, 1.0, 3);
	_raMinSpin->setRange(0, 255);
	_raMaxSpin->setRange(0, 255);
	_yellowSlotsSpin->setRange(0, 255);

	form->addRow(QString::fromLocal8Bit("Удельная нагрузка авторизации (rhoAuth): "), _rhoAuthSpin);
	form->addRow(QString::fromLocal8Bit("Базовый размер окна RA (R0): "), _r0Spin);
	form->addRow(QString::fromLocal8Bit("Коэффициент по входному потоку (kJ): "), _kJSpin);
	form->addRow(QString::fromLocal8Bit("Коэффициент по очереди (kQ): "), _kQSpin);
	form->addRow(QString::fromLocal8Bit("Целевая нагрузка авторизации (jStar): "), _jStarSpin);
	form->addRow(QString::fromLocal8Bit("Целевая длина очереди (qStar): "), _qStarSpin);
	form->addRow(QString::fromLocal8Bit("Максимальное изменение RA-окна (deltaR): "), _deltaRSpin);
	form->addRow(QString::fromLocal8Bit("Минимум RA-слотов в кадре: "), _raMinSpin);
	form->addRow(QString::fromLocal8Bit("Максимум RA-слотов в кадре: "), _raMaxSpin);
	form->addRow(QString::fromLocal8Bit("Жёлтых слотов в кадре: "), _yellowSlotsSpin);

	mainLayout->addWidget(group);
	mainLayout->addStretch();

	setConfig(BacklogFeedbackFtpGeneratorConfig());
}