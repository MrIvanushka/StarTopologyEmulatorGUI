#include "FrameConfigWidget.h"

#include <QFormLayout>

using namespace starTopologyEmulator;

FrameConfigWidget::FrameConfigWidget(QWidget* parent)
	: QWidget(parent)
{
	setupUi();
}

FrameConfig FrameConfigWidget::getConfig() const
{
	FrameConfig ret;
	ret.slotCountInFrame = _slotCountInFrame->value();
	ret.slotDuration = _slotDuration->value();
	ret.epoch = _epoch->value();
	ret.bitsPerSlot = static_cast<std::uint64_t>(_bitsPerSlot->value());
	return ret;
}

void FrameConfigWidget::setConfig(const FrameConfig& cfg)
{
	_slotCountInFrame->setValue(cfg.slotCountInFrame);
	_slotDuration->setValue(cfg.slotDuration);
	_epoch->setValue(cfg.epoch);
	_bitsPerSlot->setValue(static_cast<int>(cfg.bitsPerSlot));
}

void FrameConfigWidget::setupUi()
{
	QFormLayout* layout = new QFormLayout(this);

	_slotCountInFrame = new QSpinBox(this);
	_slotCountInFrame->setRange(4, 252);
	_slotCountInFrame->setSingleStep(1);

	_slotDuration = new QSpinBox(this);
	_slotDuration->setRange(1, 10);
	_slotDuration->setSingleStep(1);

	_epoch = new QSpinBox(this);

	_bitsPerSlot = new QSpinBox(this);
	_bitsPerSlot->setRange(0, 1000000);
	_bitsPerSlot->setSingleStep(8);

	layout->addRow(QString::fromLocal8Bit("Количество слотов в кадре: "), _slotCountInFrame);
	layout->addRow(QString::fromLocal8Bit("Длительность слота (мс): "), _slotDuration);
	layout->addRow(QString::fromLocal8Bit("Время начала первого кадра: "), _epoch);
	layout->addRow(QString::fromLocal8Bit("Бит в слоте (для аккумулятора backlog): "), _bitsPerSlot);

	FrameConfig defaultCfg;
	defaultCfg.bitsPerSlot = 1024;
	setConfig(defaultCfg);
}