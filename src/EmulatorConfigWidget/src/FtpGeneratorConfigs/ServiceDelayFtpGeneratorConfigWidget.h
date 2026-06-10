#pragma once

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QWidget>

#include <StarTopologyEmulator/StarHubStrategy/FtpGenerator/ServiceDelayFtpGeneratorConfig.h>

class ServiceDelayFtpGeneratorConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit ServiceDelayFtpGeneratorConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::ServiceDelayFtpGeneratorConfig getConfig() const;
	void setConfig(const starTopologyEmulator::ServiceDelayFtpGeneratorConfig&);

private:
	void setupUi();

	QDoubleSpinBox* _d0Spin;
	QDoubleSpinBox* _lambdaRatioSpin;
	QSpinBox*       _raMinSpin;
	QSpinBox*       _raMaxSpin;
	QSpinBox*       _yellowSlotsSpin;
};
