#pragma once

#include <QWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QFormLayout>

#include <StarTopologyEmulator/StarHubStrategy/IncomeStationsPredictor/LinearRegressionIncomeStationsPredictorConfig.h>

class LinearRegressionConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit LinearRegressionConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::LinearRegressionIncomeStationsPredictorConfig getConfig() const;
	void setConfig(const starTopologyEmulator::LinearRegressionIncomeStationsPredictorConfig& cfg);

private:
	void setupUi();

	QSpinBox* _regressionWindowSpin;
	QDoubleSpinBox* _forgettingHorizonSpin;
	QDoubleSpinBox* _minProbSpin;
	QDoubleSpinBox* _maxProbSpin;
	QDoubleSpinBox* _epsilonSpin;
};