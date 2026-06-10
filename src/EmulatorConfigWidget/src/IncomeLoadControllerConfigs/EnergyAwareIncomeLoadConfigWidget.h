#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QWidget>

#include <StarTopologyEmulator/StarHubStrategy/IncomeLoadController/EnergyAwareLoadControllerConfig.h>

class EnergyAwareIncomeLoadConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit EnergyAwareIncomeLoadConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::EnergyAwareLoadControllerConfig getConfig() const;
	void setConfig(const starTopologyEmulator::EnergyAwareLoadControllerConfig&);

private:
	void setupUi();

	QDoubleSpinBox* _weightThroughputSpin;
	QDoubleSpinBox* _weightEnergySpin;
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
