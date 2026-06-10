#pragma once

#include <memory>
#include <vector>
#include <random>

#include <QWidget>
#include <QSpinBox>
#include <QComboBox>
#include <QGroupBox>
#include <QProgressBar>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QTimer>

#include "ModuleWidget.h"
#include "IContext.h"

#pragma push_macro("emit")
#undef emit
#include <StarTopologyEmulator/CommonTypedefs.h>
#include <StarTopologyEmulator/IFaces/IEmulator.h>
#include <StarTopologyEmulator/IFaces/IStarHubStrategy.h>
#include <StarTopologyEmulator/IFaces/IStarHub.h>
#include <StarTopologyEmulator/IFaces/IStarStation.h>
#include <StarTopologyEmulator/Metrics/MetricSink.h>
#include <StarTopologyEmulator/TerminalState.h>
#pragma pop_macro("emit")

#include "EmulatorParamsWidget.h"
#include "FrameConfigWidget.h"
#include "IncomeLoadEstimatorSelectionWidget.h"
#include "HubStrategySelectionWidget.h"
#include "EmulatorLogWidget.h"

#include "../../GraphWidget/src/MetricPlotter.h"

class EmulatorConfigWidget final : public editor::ModuleWidget
{
        Q_OBJECT
public:
        explicit EmulatorConfigWidget(QWidget* parent = nullptr);

        void init(std::shared_ptr<editor::IContext> context) override;
        const editor::Info& info() const override;

private slots:
        void onCreateEmulatorClicked();
        void onStartSimulationClicked();
        void onStopSimulationClicked();
        void onSimTick();

private:
        int computeJoinedCount() const;

        QString terminalStateToString(starTopologyEmulator::TerminalState st) const;

        void logLine(const QString& line);
        void setUiEnabled(bool enabled);
        void finishSimulationAndShowResults();

private:
        static const editor::Info _info;

        std::shared_ptr<editor::IContext> _context;

        // UI
        EmulatorParamsWidget* _paramsWidget;
        FrameConfigWidget* _frameConfigWidget;
        IncomeLoadEstimatorSelectionWidget* _incomeLoadEstimatorWidget;
        HubStrategySelectionWidget* _strategyWidget;

        QPushButton* _btnCreate = nullptr;
        QPushButton* _btnRun = nullptr;
        QPushButton* _btnStop = nullptr;

        EmulatorLogWidget* _logWidget;

        MetricPlotter* _plotter;

        QTimer* _simTimer = nullptr;

        // Emulator runtime state (���� � �������)
        bool _created = false;
        bool _running = false;

        starTopologyEmulator::Timestamp _currentSlot = 0;
        int _stepsPerTick = 200;

        std::unique_ptr<starTopologyEmulator::IEmulator> _emu;
        std::shared_ptr<starTopologyEmulator::IStarHub> _hub;
        std::vector<std::shared_ptr<starTopologyEmulator::IStarStation>> _stations;
        std::shared_ptr<starTopologyEmulator::IMetricSink> _metricSink;
        std::mt19937 _rng;

        int _duration = 0;
};
