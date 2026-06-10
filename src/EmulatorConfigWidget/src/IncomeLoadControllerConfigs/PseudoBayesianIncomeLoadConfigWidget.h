#pragma once

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QWidget>

#include <StarTopologyEmulator/StarHubStrategy/IncomeLoadController/PseudoBayesianLoadControllerConfig.h>

class PseudoBayesianIncomeLoadConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit PseudoBayesianIncomeLoadConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::PseudoBayesianLoadControllerConfig getConfig() const;
	void setConfig(const starTopologyEmulator::PseudoBayesianLoadControllerConfig&);

private:
	void setupUi();

	QDoubleSpinBox* _minProbabilitySpin;
	QDoubleSpinBox* _maxProbabilitySpin;
	QDoubleSpinBox* _maxStepUpSpin;
	QDoubleSpinBox* _maxStepDownSpin;
	QDoubleSpinBox* _epsilonSpin;

	QDoubleSpinBox* _backoffPTxSpin;
	QSpinBox*       _backoffBaseWindowSpin;
	QSpinBox*       _backoffMaxWindowSpin;
	QDoubleSpinBox* _backoffExponentBaseSpin;
	QComboBox*      _backoffTypeCombo;
	QSpinBox*       _backoffAdditiveStepSpin;
};
