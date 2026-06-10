#pragma once

#include <QDoubleSpinBox>
#include <QWidget>

#include <StarTopologyEmulator/IncomeLoadEstimator/KalmanIncomeLoadEstimatorConfig.h>

class KalmanIncomeLoadEstimatorConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit KalmanIncomeLoadEstimatorConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::KalmanIncomeLoadEstimatorConfig getConfig() const;

	void setConfig(const starTopologyEmulator::KalmanIncomeLoadEstimatorConfig& config);

private:
	QDoubleSpinBox* _qG;
	QDoubleSpinBox* _qPlr;
	QDoubleSpinBox* _rBase;
	QDoubleSpinBox* _collisionWeight;

	void setupUi();
};
