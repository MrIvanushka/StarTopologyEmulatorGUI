#pragma once

#include <QComboBox>
#include <QStackedWidget>
#include <QWidget>

#pragma push_macro("emit")
#undef emit
#include <StarTopologyEmulator/IFaces/IBacklogAccumulator.h>
#include <StarTopologyEmulator/IFaces/IStarHubStrategy.h>
#include <StarTopologyEmulator/Metrics/MetricSink.h>
#pragma pop_macro("emit")

#include "CommonStrategyConfigWidget.h"
#include "SimpleStrategyConfigWidget.h"

class HubStrategySelectionWidget : public QWidget {
	Q_OBJECT
private:
	enum class HubStrategyType : int
	{
		Simple = 0,
		Common = 1
	};
public:
	explicit HubStrategySelectionWidget(QWidget* parent = nullptr);

	std::unique_ptr<starTopologyEmulator::IStarHubStrategy> strategy(
		std::shared_ptr<starTopologyEmulator::IIncomeLoadEstimator>,
		std::shared_ptr<starTopologyEmulator::IDynamicFrameSettings>,
		std::shared_ptr<starTopologyEmulator::IFrameCalculator>,
		std::shared_ptr<starTopologyEmulator::IBacklogAccumulator>,
		int totalSlots,
		starTopologyEmulator::MetricScope scope = {}) const;

signals:
	void changed();
private slots:
	void onStrategyIndexChanged(int index);
private:
	QComboBox* _strategyCombo;
	QStackedWidget* _settingsStack;

	SimpleStrategyConfigWidget* _simpleStrategyPage;
	CommonStrategyConfigWidget* _commonStrategyPage;
};