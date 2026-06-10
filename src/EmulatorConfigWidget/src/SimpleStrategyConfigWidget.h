#pragma once

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QWidget>

#include <StarTopologyEmulator/StarHubStrategy/StarHubStrategyConfig.h>

class SimpleStrategyConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit SimpleStrategyConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::StarHubStrategyConfig getConfig(int totalSlots) const;

	void setConfig(const starTopologyEmulator::StarHubStrategyConfig& config);

private:
	QDoubleSpinBox* _sbMinRaSlots;
	QDoubleSpinBox* _sbMaxRaSlots;
	QSpinBox* _sbMinBaseWindow;
	QSpinBox* _sbMaxBaseWindow;

	QDoubleSpinBox* _dsbTargetPlr;
	QDoubleSpinBox* _dsbHeavyLoadG;
	QDoubleSpinBox* _dsbMinPTx;
	QDoubleSpinBox* _dsbMaxPTx;

	void setupUi();
};
