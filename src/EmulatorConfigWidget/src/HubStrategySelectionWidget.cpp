#include "HubStrategySelectionWidget.h"

#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

#pragma push_macro("emit")
#undef emit
#include <StarTopologyEmulator/StarHubStrategy/StarHubStrategyFactory.h>
#pragma pop_macro("emit")

using namespace starTopologyEmulator;

HubStrategySelectionWidget::HubStrategySelectionWidget(QWidget* parent) : QWidget(parent) {
	auto* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSizeConstraint(QLayout::SetMinimumSize);

	_strategyCombo = new QComboBox(this);
	_strategyCombo->addItem(QString::fromLocal8Bit("Простой адаптивный backoff"), static_cast<int>(HubStrategyType::Simple));
	_strategyCombo->addItem(QString::fromLocal8Bit("Композитная"), static_cast<int>(HubStrategyType::Common));

	_settingsStack = new QStackedWidget(this);

	_simpleStrategyPage = new SimpleStrategyConfigWidget();
	_commonStrategyPage = new CommonStrategyConfigWidget();

	_settingsStack->addWidget(_simpleStrategyPage);
	_settingsStack->addWidget(_commonStrategyPage);

	mainLayout->addWidget(_strategyCombo);
	mainLayout->addWidget(_settingsStack);

	connect(_strategyCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
		_settingsStack, &QStackedWidget::setCurrentIndex);
}

void HubStrategySelectionWidget::onStrategyIndexChanged(int index)
{
	for (int i = 0; i < _settingsStack->count(); ++i) {
		QSizePolicy::Policy policy = (i == index) ? QSizePolicy::Minimum : QSizePolicy::Ignored;
		_settingsStack->widget(i)->setSizePolicy(policy, policy);
	}
	_settingsStack->widget(index)->updateGeometry();
	_settingsStack->updateGeometry();

	_settingsStack->setCurrentIndex(index);
	emit changed();
}

std::unique_ptr<IStarHubStrategy> HubStrategySelectionWidget::strategy(
	std::shared_ptr<starTopologyEmulator::IIncomeLoadEstimator> incomeLoadEstimator,
	std::shared_ptr<starTopologyEmulator::IDynamicFrameSettings> dynamicFrameSettings,
	std::shared_ptr<starTopologyEmulator::IFrameCalculator> frameCalculator,
	std::shared_ptr<starTopologyEmulator::IBacklogAccumulator> backlogAccumulator,
	int totalSlots,
	MetricScope scope) const
{
	switch (_settingsStack->currentIndex())
	{
	case static_cast<int>(HubStrategyType::Simple):
		return StarHubStrategyFactory::make(
			incomeLoadEstimator,
			_simpleStrategyPage->getConfig(totalSlots),
			std::move(scope));
	case static_cast<int>(HubStrategyType::Common):
		return _commonStrategyPage->makeStrategy(
			incomeLoadEstimator, dynamicFrameSettings, frameCalculator,
			std::move(backlogAccumulator), std::move(scope));
	}

	return nullptr;
}