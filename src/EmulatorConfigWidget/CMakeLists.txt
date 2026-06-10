project(EmulatorConfigWidget LANGUAGES CXX)

find_package(Qt5Widgets        REQUIRED)
find_package(Qt5Gui            REQUIRED)

set(ModuleWidget
	src/EmulatorConfigWidget.cpp
	src/EmulatorConfigWidget.h
)
source_group(ModuleWidget FILES ${ModuleWidget})

set(FactoryWidgets
	src/EmulatorLogWidget.cpp
	src/EmulatorLogWidget.h
	src/EmulatorParamsWidget.cpp
	src/EmulatorParamsWidget.h
	src/HubStrategySelectionWidget.cpp
	src/HubStrategySelectionWidget.h
	src/IncomeLoadEstimatorSelectionWidget.cpp
	src/IncomeLoadEstimatorSelectionWidget.h
	src/FrameConfigWidget.cpp
	src/FrameConfigWidget.h
)
source_group(FactoryWidgets FILES ${FactoryWidgets})

set(IncomeLoadControllerConfigs
	src/IncomeLoadControllerConfigs/AlphaFairIncomeLoadConfigWidget.cpp
	src/IncomeLoadControllerConfigs/AlphaFairIncomeLoadConfigWidget.h
	src/IncomeLoadControllerConfigs/CollisionBudgetIncomeLoadConfigWidget.cpp
	src/IncomeLoadControllerConfigs/CollisionBudgetIncomeLoadConfigWidget.h
	src/IncomeLoadControllerConfigs/EnergyAwareIncomeLoadConfigWidget.cpp
	src/IncomeLoadControllerConfigs/EnergyAwareIncomeLoadConfigWidget.h
	src/IncomeLoadControllerConfigs/GreyModelAdaptiveBackoffConfigWidget.cpp
	src/IncomeLoadControllerConfigs/GreyModelAdaptiveBackoffConfigWidget.h
	src/IncomeLoadControllerConfigs/LogBarrierIncomeLoadConfigWidget.cpp
	src/IncomeLoadControllerConfigs/LogBarrierIncomeLoadConfigWidget.h
	src/IncomeLoadControllerConfigs/PiAntiWindupConfigWidget.cpp
	src/IncomeLoadControllerConfigs/PiAntiWindupConfigWidget.h
	src/IncomeLoadControllerConfigs/PiIncomeLoadConfigWidget.cpp
	src/IncomeLoadControllerConfigs/PiIncomeLoadConfigWidget.h
	src/IncomeLoadControllerConfigs/PseudoBayesianIncomeLoadConfigWidget.cpp
	src/IncomeLoadControllerConfigs/PseudoBayesianIncomeLoadConfigWidget.h
	src/IncomeLoadControllerConfigs/RiskSensitiveIncomeLoadConfigWidget.cpp
	src/IncomeLoadControllerConfigs/RiskSensitiveIncomeLoadConfigWidget.h
	src/IncomeLoadControllerConfigs/SimpleUtilityBasedIncomeLoadConfigWidget.cpp
	src/IncomeLoadControllerConfigs/SimpleUtilityBasedIncomeLoadConfigWidget.h
	src/IncomeLoadControllerConfigs/StaticIncomeLoadConfigWidget.cpp
	src/IncomeLoadControllerConfigs/StaticIncomeLoadConfigWidget.h
)
source_group(IncomeLoadControllerConfigs FILES ${IncomeLoadControllerConfigs})

set(FtpGeneratorConfigs
	src/FtpGeneratorConfigs/BacklogFeedbackFtpGeneratorConfigWidget.cpp
	src/FtpGeneratorConfigs/BacklogFeedbackFtpGeneratorConfigWidget.h
	src/FtpGeneratorConfigs/LyapunovFtpGeneratorConfigWidget.cpp
	src/FtpGeneratorConfigs/LyapunovFtpGeneratorConfigWidget.h
	src/FtpGeneratorConfigs/MarginalUtilityFtpGeneratorConfigWidget.cpp
	src/FtpGeneratorConfigs/MarginalUtilityFtpGeneratorConfigWidget.h
	src/FtpGeneratorConfigs/ServiceDelayFtpGeneratorConfigWidget.cpp
	src/FtpGeneratorConfigs/ServiceDelayFtpGeneratorConfigWidget.h
	src/FtpGeneratorConfigs/StaticFtpGeneratorConfigWidget.cpp
	src/FtpGeneratorConfigs/StaticFtpGeneratorConfigWidget.h
)
source_group(FtpGeneratorConfigs FILES ${FtpGeneratorConfigs})

set(StrategyConfigs
	${IncomeLoadControllerConfigs}
	${FtpGeneratorConfigs}
	src/CommonStrategyConfigWidget.cpp
	src/CommonStrategyConfigWidget.h
	src/SimpleStrategyConfigWidget.cpp
	src/SimpleStrategyConfigWidget.h
	src/IncomeStationsPredictorConfigs/LinearRegressionConfigWidget.cpp
	src/IncomeStationsPredictorConfigs/LinearRegressionConfigWidget.h
	src/IncomeStationsPredictorConfigs/GreyModelConfigWidget.cpp
	src/IncomeStationsPredictorConfigs/GreyModelConfigWidget.h
)
source_group(StrategyConfigs FILES ${StrategyConfigs})

set(Common
	src/CollapsibleSection.cpp
	src/CollapsibleSection.h
)
source_group(Common FILES ${Common})

set(Scenario
	src/Scenario/ScenarioEventData.h
	src/Scenario/ScenarioJson.cpp
	src/Scenario/ScenarioJson.h
	src/Scenario/ScenarioEventDialog.cpp
	src/Scenario/ScenarioEventDialog.h
	src/Scenario/ScenarioBuilderWidget.cpp
	src/Scenario/ScenarioBuilderWidget.h
)
source_group(Scenario FILES ${Scenario})

set(Benchmark
	src/Benchmark/BenchTypes.h
	src/Benchmark/BenchTypes.cpp
	src/Benchmark/ParamRegistry.h
	src/Benchmark/ParamRegistry.cpp
	src/Benchmark/BenchJson.h
	src/Benchmark/BenchJson.cpp
	src/Benchmark/BenchPlan.h
	src/Benchmark/BenchPlan.cpp
	src/Benchmark/BenchRunner.h
	src/Benchmark/BenchRunner.cpp
	src/Benchmark/BenchExecutor.h
	src/Benchmark/BenchExecutor.cpp
	src/Benchmark/SweepSpinBox.h
	src/Benchmark/SweepSpinBox.cpp
	src/Benchmark/BenchImplDialog.h
	src/Benchmark/BenchImplDialog.cpp
	src/Benchmark/BenchmarkWidget.h
	src/Benchmark/BenchmarkWidget.cpp
)
source_group(Benchmark FILES ${Benchmark})

set(IncomeLoadEstimatorConfigs
	src/EmaIncomeLoadEstimatorConfigWidget.cpp
	src/EmaIncomeLoadEstimatorConfigWidget.h
	src/KalmanIncomeLoadEstimatorConfigWidget.cpp
	src/KalmanIncomeLoadEstimatorConfigWidget.h
)
source_group(IncomeLoadEstimatorConfigs FILES ${IncomeLoadEstimatorConfigs})

add_library(${PROJECT_NAME} STATIC
	${ModuleWidget}
	${IncomeLoadEstimatorConfigs}
	${StrategyConfigs}
	${FactoryWidgets}
	${Common}
	${Scenario}
	${Benchmark}
)

target_include_directories(${PROJECT_NAME}
	PUBLIC ../Context/src
	PUBLIC ../GraphWidget/src
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC Context
	PUBLIC GraphWidget
	PUBLIC StarTopologyEmulator
)

target_link_libraries (${PROJECT_NAME} PUBLIC Qt5::Widgets)
target_link_libraries (${PROJECT_NAME} PUBLIC Qt5::Gui)

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)