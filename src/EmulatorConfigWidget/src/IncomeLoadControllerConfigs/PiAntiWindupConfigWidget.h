#pragma once

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QStackedWidget>
#include <QWidget>

#include <StarTopologyEmulator/StarHubStrategy/IncomeLoadController/AntiWindup/PiAntiWindupConfig.h>

class PiAntiWindupConfigWidget : public QWidget {
	Q_OBJECT
private:
	enum AntiWindupKind : int {
		None = 0,
		Clamping = 1,
		BackCalculation = 2,
		TrackingMode = 3,
	};
public:
	explicit PiAntiWindupConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::PiAntiWindupConfig getConfig() const;
	void setConfig(const starTopologyEmulator::PiAntiWindupConfig&);

private:
	void setupUi();

	QComboBox* _kindSelector;
	QStackedWidget* _stack;
	QDoubleSpinBox* _kTSpin;
};