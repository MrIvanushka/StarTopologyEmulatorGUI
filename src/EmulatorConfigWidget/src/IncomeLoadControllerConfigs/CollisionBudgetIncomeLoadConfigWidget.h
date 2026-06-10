#pragma once

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QWidget>

#include <StarTopologyEmulator/StarHubStrategy/IncomeLoadController/CollisionBudgetLoadControllerConfig.h>

class CollisionBudgetIncomeLoadConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit CollisionBudgetIncomeLoadConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::CollisionBudgetLoadControllerConfig getConfig() const;
	void setConfig(const starTopologyEmulator::CollisionBudgetLoadControllerConfig&);

private:
	void setupUi();

	QDoubleSpinBox* _collisionBudgetSpin;
	QDoubleSpinBox* _gradientStepSpin;
	QDoubleSpinBox* _lagrangianStepSpin;
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
