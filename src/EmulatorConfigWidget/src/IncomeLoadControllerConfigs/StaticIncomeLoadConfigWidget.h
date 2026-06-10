#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QWidget>

#include <StarTopologyEmulator/Messages/StarHubPlanMessage.h>

class StaticIncomeLoadConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit StaticIncomeLoadConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::StarHubPlanMessage::BackoffConfig getConfig() const;

	void setConfig(const starTopologyEmulator::StarHubPlanMessage::BackoffConfig&);
private:
	QDoubleSpinBox* _pTxSpin;
	QSpinBox*       _baseWindowSpin;
	QSpinBox*       _maxWindowSpin;
	QComboBox*      _backoffTypeCombo;
	QSpinBox*       _backoffAdditiveStepSpin;
	QDoubleSpinBox* _exponentBaseSpin;

	void setupUi();
};
