#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QWidget>

#include <StarTopologyEmulator/StarHubStrategy/IncomeLoadController/SimpleMarginalUtilityBasedLoadControllerConfig.h>

class SimpleUtilityBasedIncomeLoadConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit SimpleUtilityBasedIncomeLoadConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::SimpleMarginalUtilityBasedLoadControllerConfig getConfig() const;

	void setConfig(const starTopologyEmulator::SimpleMarginalUtilityBasedLoadControllerConfig&);
private:
	QDoubleSpinBox* _weightThroughputSpin;
	QDoubleSpinBox* _weightCollisionSpin;
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

	void setupUi();
};
