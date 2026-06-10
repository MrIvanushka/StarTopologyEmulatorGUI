#pragma once

#include <QLabel>
#include <QSpinBox>
#include <QWidget>

class EmulatorParamsWidget : public QWidget {
	Q_OBJECT
public:
	struct Data {
		int numStations = 10;
		int messagesNeeded = 1;
		int hubTts = 10;
		int stationTts = 100;
		int duration = 1000;
		int seed = 42;
	};

	explicit EmulatorParamsWidget(QWidget* parent = nullptr);
	
	void setData(const Data& d);
	
	Data data() const;
signals:
	void changed();

private:
	QSpinBox* _numStations;
	QSpinBox* _messagesNeeded;
	QSpinBox* _hubTts;
	QSpinBox* _stationTts;
	QSpinBox* _duration;
	QSpinBox* _seed;
};
