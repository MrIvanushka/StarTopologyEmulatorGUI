#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QWidget>

#include <StarTopologyEmulator/StarHubStrategy/IncomeLoadController/LogBarrierLoadControllerConfig.h>

class LogBarrierIncomeLoadConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit LogBarrierIncomeLoadConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::LogBarrierLoadControllerConfig getConfig() const;
	void setConfig(const starTopologyEmulator::LogBarrierLoadControllerConfig&);

private:
	void setupUi();

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
};
