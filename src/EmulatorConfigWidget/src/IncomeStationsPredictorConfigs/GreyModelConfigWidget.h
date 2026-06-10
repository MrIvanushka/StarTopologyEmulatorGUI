#pragma once

#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QWidget>

#include <StarTopologyEmulator/StarHubStrategy/IncomeStationsPredictor/GreyModelIncomeStationsPredictorConfig.h>

class GreyModelConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit GreyModelConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::GreyModelIncomeStationsPredictorConfig getConfig() const;
	void setConfig(const starTopologyEmulator::GreyModelIncomeStationsPredictorConfig& cfg);

private:
	void setupUi();

	QSpinBox*       _windowSizeSpin;
	QSpinBox*       _minHistorySpin;
	QDoubleSpinBox* _minProbSpin;
	QDoubleSpinBox* _maxProbSpin;
	QDoubleSpinBox* _epsilonSpin;
};
