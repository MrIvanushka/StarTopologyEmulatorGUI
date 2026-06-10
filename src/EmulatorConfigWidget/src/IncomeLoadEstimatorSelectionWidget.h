#pragma once

#include <QComboBox>
#include <QStackedWidget>
#include <QWidget>

#pragma push_macro("emit")
#undef emit
#include <StarTopologyEmulator/IFaces/IIncomeLoadEstimator.h>
#include <StarTopologyEmulator/Metrics/MetricSink.h>
#pragma pop_macro("emit")

#include "EmaIncomeLoadEstimatorConfigWidget.h"
#include "KalmanIncomeLoadEstimatorConfigWidget.h"

class IncomeLoadEstimatorSelectionWidget : public QWidget {
	Q_OBJECT
private:
	enum IncomeLoadEstimatorType : int
	{
		Ema = 0,
		Kalman = 1
	};
public:
	explicit IncomeLoadEstimatorSelectionWidget(QWidget* parent = nullptr);

	std::unique_ptr<starTopologyEmulator::IIncomeLoadEstimator> incomeLoadEstimator(
		starTopologyEmulator::MetricScope scope = {}) const;

signals:
	void changed();
private slots:
	void onEstimatorIndexChanged(int index);
private:
	QComboBox* _comboBox;
	QStackedWidget* _settingsStack;

	EmaIncomeLoadEstimatorConfigWidget* _emaPage;
	KalmanIncomeLoadEstimatorConfigWidget* _kalmanPage;
};