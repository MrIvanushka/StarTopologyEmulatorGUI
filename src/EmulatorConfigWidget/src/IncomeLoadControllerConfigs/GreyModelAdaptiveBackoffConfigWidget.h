#pragma once

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QWidget>

#include <StarTopologyEmulator/StarHubStrategy/IncomeLoadController/GreyModelAdaptiveBackoffControllerConfig.h>

class GreyModelAdaptiveBackoffConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit GreyModelAdaptiveBackoffConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::GreyModelAdaptiveBackoffControllerConfig getConfig() const;
	void setConfig(const starTopologyEmulator::GreyModelAdaptiveBackoffControllerConfig&);

private:
	void setupUi();

	QDoubleSpinBox* _gTargetSpin;
	QSpinBox*       _historySizeSpin;
	QSpinBox*       _minHistoryForPredictionSpin;
	QSpinBox*       _minBackoffWindowSpin;
	QSpinBox*       _maxBackoffWindowSpin;
	QDoubleSpinBox* _epsilonSpin;

	QDoubleSpinBox* _backoffPTxSpin;
	QSpinBox*       _backoffBaseWindowSpin;
	QSpinBox*       _backoffMaxWindowSpin;
	QDoubleSpinBox* _backoffExponentBaseSpin;
	QComboBox*      _backoffTypeCombo;
	QSpinBox*       _backoffAdditiveStepSpin;
};
