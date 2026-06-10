#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QWidget>

#include <StarTopologyEmulator/StarHubStrategy/IncomeLoadController/RiskSensitiveLoadControllerConfig.h>

class RiskSensitiveIncomeLoadConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit RiskSensitiveIncomeLoadConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::RiskSensitiveLoadControllerConfig getConfig() const;
	void setConfig(const starTopologyEmulator::RiskSensitiveLoadControllerConfig&);

private:
	void setupUi();

	QDoubleSpinBox* _betaSpin;
	QDoubleSpinBox* _collisionPenaltySpin;
	QDoubleSpinBox* _gradientStepSpin;
	QDoubleSpinBox* _maxProbabilityStepSpin;
	QDoubleSpinBox* _minProbabilitySpin;
	QDoubleSpinBox* _maxProbabilitySpin;
	QDoubleSpinBox* _epsilonSpin;

	QDoubleSpinBox* _backoffPTxSpin;
	QSpinBox*       _backoffBaseWindowSpin;
	QSpinBox*       _backoffMaxWindowSpin;
	QDoubleSpinBox* _backoffExponentBaseSpin;
	QComboBox*      _backoffTypeCombo;
	QSpinBox*       _backoffAdditiveStepSpin;
};
