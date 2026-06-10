#pragma once

#include <QWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QFormLayout>

#include <StarTopologyEmulator/Messages/StarHubPlanMessage.h>

class StaticFtpGeneratorConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit StaticFtpGeneratorConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::StarHubPlanMessage::FtpConfig getConfig() const;
	void setConfig(const starTopologyEmulator::StarHubPlanMessage::FtpConfig& cfg);

private:
	void setupUi();

	QSpinBox* _raSlotsSpin;
	QSpinBox* _yellowSlotsSpin;
	QSpinBox* _onlineSlotsSpin;
};