#pragma once

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QWidget>

#include <StarTopologyEmulator/StarHubStrategy/IncomeLoadController/PiLoadControllerConfig.h>

#include "PiAntiWindupConfigWidget.h"

class PiIncomeLoadConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit PiIncomeLoadConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::PiLoadControllerConfig getConfig() const;
	void setConfig(const starTopologyEmulator::PiLoadControllerConfig&);

private:
	void setupUi();

	QDoubleSpinBox* _gTargetSpin;
	QDoubleSpinBox* _kPSpin;
	QDoubleSpinBox* _kISpin;
	QSpinBox*       _integralWindowSpin;
	QDoubleSpinBox* _alphaSpin;
	QDoubleSpinBox* _minProbabilitySpin;
	QDoubleSpinBox* _maxProbabilitySpin;
	QDoubleSpinBox* _maxProbabilityStepSpin;
	QDoubleSpinBox* _epsilonSpin;
	QCheckBox*      _allowFeedForwardBox;

	QDoubleSpinBox* _backoffPTxSpin;
	QSpinBox*       _backoffBaseWindowSpin;
	QSpinBox*       _backoffMaxWindowSpin;
	QDoubleSpinBox* _backoffExponentBaseSpin;
	QComboBox*      _backoffTypeCombo;
	QSpinBox*       _backoffAdditiveStepSpin;

	PiAntiWindupConfigWidget* _antiWindupWidget;
};
