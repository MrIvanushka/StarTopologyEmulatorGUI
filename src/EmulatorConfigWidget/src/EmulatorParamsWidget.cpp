#include "EmulatorParamsWidget.h"

#include <QFormLayout>
#include <QGroupBox>

EmulatorParamsWidget::EmulatorParamsWidget(QWidget* parent) : QWidget(parent) {
        auto* form = new QFormLayout(this);

        _numStations = new QSpinBox(this); _numStations->setRange(1, 1'000'000);
        _messagesNeeded = new QSpinBox(this); _messagesNeeded->setRange(1, 1'000);
        _hubTts = new QSpinBox(this); _hubTts->setRange(1, 3600 * 1000); _hubTts->setSuffix(" ms");
        _stationTts = new QSpinBox(this); _stationTts->setRange(1, 3600 * 1000); _stationTts->setSuffix(" ms");
        _duration = new QSpinBox(this); _duration->setRange(1, 100'000'000); _duration->setSuffix(" ms");
        _seed = new QSpinBox(this); _seed->setRange(0, std::numeric_limits<int>::max());

        form->addRow(QString::fromLocal8Bit("Количество станций:"), _numStations);
        form->addRow(QString::fromLocal8Bit("Сообщений нужно (messagesNeeded):"), _messagesNeeded);
        form->addRow(QString::fromLocal8Bit("TTS центральной станции:"), _hubTts);
        form->addRow(QString::fromLocal8Bit("TTS абонентских станций:"), _stationTts);
        form->addRow(QString::fromLocal8Bit("Длительность симуляции:"), _duration);
        form->addRow(QString::fromLocal8Bit("Seed RNG:"), _seed);

        form->setContentsMargins(0, 0, 0, 0);
}

EmulatorParamsWidget::Data EmulatorParamsWidget::data() const
{
        return { _numStations->value(), _messagesNeeded->value(), _hubTts->value(),
                _stationTts->value(), _duration->value(), _seed->value() };
}

void EmulatorParamsWidget::setData(const Data& d)
{
        _numStations->setValue(d.numStations);
        _messagesNeeded->setValue(d.messagesNeeded);
        _stationTts->setValue(d.stationTts);
        _hubTts->setValue(d.hubTts);
        _duration->setValue(d.duration);
        _seed->setValue(d.seed);
}