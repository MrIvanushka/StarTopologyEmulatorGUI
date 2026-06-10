#pragma once

#include <QComboBox>
#include <QStackedWidget>
#include <QGroupBox>
#include <QVBoxLayout>

#pragma push_macro("emit")
#undef emit
#include <StarTopologyEmulator/IFaces/IBacklogAccumulator.h>
#include <StarTopologyEmulator/IFaces/IDynamicFrameSettings.h>
#include <StarTopologyEmulator/IFaces/IFrameCalculator.h>
#include <StarTopologyEmulator/IFaces/IFtpGenerator.h>
#include <StarTopologyEmulator/IFaces/IIncomeLoadController.h>
#include <StarTopologyEmulator/IFaces/IIncomeLoadEstimator.h>
#include <StarTopologyEmulator/IFaces/IIncomeStationsPredictor.h>
#include <StarTopologyEmulator/IFaces/IStarHubStrategy.h>
#include <StarTopologyEmulator/Metrics/MetricSink.h>
#pragma pop_macro("emit")

#include "IncomeLoadControllerConfigs/AlphaFairIncomeLoadConfigWidget.h"
#include "IncomeLoadControllerConfigs/CollisionBudgetIncomeLoadConfigWidget.h"
#include "IncomeLoadControllerConfigs/EnergyAwareIncomeLoadConfigWidget.h"
#include "IncomeLoadControllerConfigs/GreyModelAdaptiveBackoffConfigWidget.h"
#include "IncomeLoadControllerConfigs/LogBarrierIncomeLoadConfigWidget.h"
#include "IncomeLoadControllerConfigs/PiIncomeLoadConfigWidget.h"
#include "IncomeLoadControllerConfigs/PseudoBayesianIncomeLoadConfigWidget.h"
#include "IncomeLoadControllerConfigs/RiskSensitiveIncomeLoadConfigWidget.h"
#include "IncomeLoadControllerConfigs/SimpleUtilityBasedIncomeLoadConfigWidget.h"
#include "IncomeLoadControllerConfigs/StaticIncomeLoadConfigWidget.h"

#include "IncomeStationsPredictorConfigs/GreyModelConfigWidget.h"
#include "IncomeStationsPredictorConfigs/LinearRegressionConfigWidget.h"

#include "FtpGeneratorConfigs/BacklogFeedbackFtpGeneratorConfigWidget.h"
#include "FtpGeneratorConfigs/LyapunovFtpGeneratorConfigWidget.h"
#include "FtpGeneratorConfigs/MarginalUtilityFtpGeneratorConfigWidget.h"
#include "FtpGeneratorConfigs/ServiceDelayFtpGeneratorConfigWidget.h"
#include "FtpGeneratorConfigs/StaticFtpGeneratorConfigWidget.h"

class CommonStrategyConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit CommonStrategyConfigWidget(QWidget* parent = nullptr);

	std::unique_ptr<starTopologyEmulator::IStarHubStrategy> makeStrategy(
		std::shared_ptr<starTopologyEmulator::IIncomeLoadEstimator>,
		std::shared_ptr<starTopologyEmulator::IDynamicFrameSettings>,
		std::shared_ptr<starTopologyEmulator::IFrameCalculator>,
		std::shared_ptr<starTopologyEmulator::IBacklogAccumulator>,
		starTopologyEmulator::MetricScope scope = {}) const;

private:
	void setupUi();

	std::unique_ptr<starTopologyEmulator::IFtpGenerator> createFtpGenerator(
		std::shared_ptr<starTopologyEmulator::IDynamicFrameSettings>,
		std::shared_ptr<starTopologyEmulator::IIncomeStationsPredictor>,
		std::shared_ptr<starTopologyEmulator::IBacklogAccumulator>,
		std::shared_ptr<starTopologyEmulator::IFrameCalculator>) const;

	std::unique_ptr<starTopologyEmulator::IIncomeLoadController> createIncomeLoadController(
		std::shared_ptr<starTopologyEmulator::IDynamicFrameSettings>,
		std::shared_ptr<starTopologyEmulator::IIncomeStationsPredictor>,
		starTopologyEmulator::MetricScope scope = {}) const;

	std::shared_ptr<starTopologyEmulator::IIncomeStationsPredictor> createIncomeStationsPredictor(
		std::shared_ptr<starTopologyEmulator::IIncomeLoadEstimator>,
		std::shared_ptr<starTopologyEmulator::IDynamicFrameSettings>,
		std::shared_ptr<starTopologyEmulator::IFrameCalculator>,
		starTopologyEmulator::MetricScope scope = {}) const;

	// ��������� � �������� ��������.
	QComboBox* _ftpGeneratorSelector;
	QStackedWidget* _ftpGeneratorStack;
	StaticFtpGeneratorConfigWidget*          _staticFtpPage;
	BacklogFeedbackFtpGeneratorConfigWidget* _backlogFeedbackFtpPage;
	ServiceDelayFtpGeneratorConfigWidget*    _serviceDelayFtpPage;
	LyapunovFtpGeneratorConfigWidget*        _lyapunovFtpPage;
	MarginalUtilityFtpGeneratorConfigWidget* _marginalUtilityFtpPage;

	QComboBox* _incomeLoadControllerSelector;
	QStackedWidget* _incomeLoadControllerStack;
	StaticIncomeLoadConfigWidget*              _staticIncomePage;
	PiIncomeLoadConfigWidget*                  _piIncomePage;
	SimpleUtilityBasedIncomeLoadConfigWidget*  _simpleUtilityIncomePage;
	PseudoBayesianIncomeLoadConfigWidget*      _pseudoBayesianIncomePage;
	CollisionBudgetIncomeLoadConfigWidget*     _collisionBudgetIncomePage;
	GreyModelAdaptiveBackoffConfigWidget*      _greyModelIncomePage;
	LogBarrierIncomeLoadConfigWidget*          _logBarrierIncomePage;
	EnergyAwareIncomeLoadConfigWidget*         _energyAwareIncomePage;
	AlphaFairIncomeLoadConfigWidget*           _alphaFairIncomePage;
	RiskSensitiveIncomeLoadConfigWidget*       _riskSensitiveIncomePage;

	QComboBox* _predictorSelector;
	QStackedWidget* _predictorStack;
	LinearRegressionConfigWidget* _linearRegressionPredictorPage;
	GreyModelConfigWidget*        _greyModelPredictorPage;
};