#pragma once

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QWidget>

#include <StarTopologyEmulator/StarHubStrategy/FtpGenerator/BacklogFeedbackFtpGeneratorConfig.h>

class BacklogFeedbackFtpGeneratorConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit BacklogFeedbackFtpGeneratorConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::BacklogFeedbackFtpGeneratorConfig getConfig() const;
	void setConfig(const starTopologyEmulator::BacklogFeedbackFtpGeneratorConfig&);

private:
	void setupUi();

	QDoubleSpinBox* _rhoAuthSpin;
	QDoubleSpinBox* _r0Spin;
	QDoubleSpinBox* _kJSpin;
	QDoubleSpinBox* _kQSpin;
	QDoubleSpinBox* _jStarSpin;
	QDoubleSpinBox* _qStarSpin;
	QDoubleSpinBox* _deltaRSpin;
	QSpinBox*       _raMinSpin;
	QSpinBox*       _raMaxSpin;
	QSpinBox*       _yellowSlotsSpin;
};
