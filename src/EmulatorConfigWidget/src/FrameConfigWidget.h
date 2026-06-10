#pragma once

#include <QSpinBox>
#include <QWidget>

#include <StarTopologyEmulator/IFaces/IFrameCalculator.h>

class FrameConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit FrameConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::FrameConfig getConfig() const;

	void setConfig(const starTopologyEmulator::FrameConfig& config);

private:
	QSpinBox* _slotCountInFrame;
	QSpinBox* _slotDuration;
	QSpinBox* _epoch;
	QSpinBox* _bitsPerSlot;

	void setupUi();
};
