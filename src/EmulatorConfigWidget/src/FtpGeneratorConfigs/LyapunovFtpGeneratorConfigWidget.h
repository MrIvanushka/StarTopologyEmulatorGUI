#pragma once

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QWidget>

#include <StarTopologyEmulator/StarHubStrategy/FtpGenerator/LyapunovFtpGeneratorConfig.h>

class LyapunovFtpGeneratorConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit LyapunovFtpGeneratorConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::LyapunovFtpGeneratorConfig getConfig() const;
	void setConfig(const starTopologyEmulator::LyapunovFtpGeneratorConfig&);

private:
	void setupUi();

	QDoubleSpinBox* _vSpin;
	QSpinBox*       _raMinSpin;
	QSpinBox*       _raMaxSpin;
	QSpinBox*       _yellowSlotsSpin;
};
