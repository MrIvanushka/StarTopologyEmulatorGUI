#pragma once

#include <QDoubleSpinBox>
#include <QWidget>

#include <StarTopologyEmulator/IncomeLoadEstimator/EmaIncomeLoadEstimatorConfig.h>

class EmaIncomeLoadEstimatorConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit EmaIncomeLoadEstimatorConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::EmaIncomeLoadEstimatorConfig getConfig() const;

	void setConfig(const starTopologyEmulator::EmaIncomeLoadEstimatorConfig& config);

private:
	QDoubleSpinBox* _alphaG;
	QDoubleSpinBox* _alphaPlr;
	QDoubleSpinBox* _collisionWeight;

	void setupUi();
};
