#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QWidget>

#include <StarTopologyEmulator/StarHubStrategy/IncomeLoadController/AlphaFairLoadControllerConfig.h>

class AlphaFairIncomeLoadConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit AlphaFairIncomeLoadConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::AlphaFairLoadControllerConfig getConfig() const;
	void setConfig(const starTopologyEmulator::AlphaFairLoadControllerConfig&);

private:
	void setupUi();

	QDoubleSpinBox* _alphaSpin;
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
