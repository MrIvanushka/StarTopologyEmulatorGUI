#include "EmulatorConfigWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#pragma push_macro("emit")
#undef emit
#include <StarTopologyEmulator/BacklogAccumulator/BacklogAccumulatorFactory.h>
#include <StarTopologyEmulator/EmulatorFactory.h>
#include <StarTopologyEmulator/FrameCalculatorFactory.h>
#include <StarTopologyEmulator/Metrics/MetricSink.h>
#include <StarTopologyEmulator/Metrics/MetricSinkFactory.h>
#include <StarTopologyEmulator/Stations/StarHubFactory.h>
#include <StarTopologyEmulator/Stations/StarStationFactory.h>

#include "StarTopologyEmulator/DynamicFrameSettingsFactory.h"
#pragma pop_macro("emit")

#include "CollapsibleSection.h"

using namespace editor;
using namespace starTopologyEmulator;

namespace ctx_keys
{
        static const QString SlotMs = QStringLiteral("emulator/slot_duration_ms");
        static const QString RttMs = QStringLiteral("emulator/rtt_ms");
        static const QString RttSlots = QStringLiteral("emulator/rtt_slots");
        static const QString NumStations = QStringLiteral("emulator/subscriber_count");
        static const QString MessagesNeeded = QStringLiteral("emulator/messages_needed");
        static const QString MaxSlots = QStringLiteral("emulator/max_slots");
        static const QString Seed = QStringLiteral("emulator/rng_seed");
        static const QString HubStrategy = QStringLiteral("emulator/hub_strategy");

        static const QString Created = QStringLiteral("emulator/runtime/created");
        static const QString IsRunning = QStringLiteral("emulator/runtime/is_running");
        static const QString StartEpochMs = QStringLiteral("emulator/runtime/start_epoch_ms");
        static const QString CurrentSlot = QStringLiteral("emulator/runtime/current_slot");
        static const QString ElapsedMs = QStringLiteral("emulator/runtime/elapsed_ms");

        static const QString TerminalsJoined = QStringLiteral("emulator/stats/terminals_joined");

        static const QString TotalSuccess = QStringLiteral("emulator/results/total_success");
        static const QString TotalCollisions = QStringLiteral("emulator/results/total_collisions");
        static const QString IncomeLoadG = QStringLiteral("emulator/results/income_load_g");
        static const QString IncomePlr = QStringLiteral("emulator/results/income_plr");
        static const QString StationStatsJson = QStringLiteral("emulator/results/station_stats_json");
} // namespace ctx_keys

const Info EmulatorConfigWidget::_info{
        QString::fromLocal8Bit("Эмулятор протокола"),
        QString::fromLocal8Bit("Создание и запуск симуляции StarHub/StarStation + статистика")
};

static void ctxSet(std::shared_ptr<editor::IContext>& ctx, const QString& k, const QVariant& v)
{
        if (ctx) ctx->setValue(k, v);
}

EmulatorConfigWidget::EmulatorConfigWidget(QWidget* parent)
        : ModuleWidget()
{
        auto* root = new QVBoxLayout(this);
        root->setAlignment(Qt::AlignTop);

        _logWidget = new EmulatorLogWidget(this);

        auto* buttons = new QHBoxLayout();
        _btnCreate = new QPushButton(QString::fromLocal8Bit("Создать эмулятор"), this);
        _btnRun = new QPushButton(QString::fromLocal8Bit("Запустить симуляцию"), this);
        _btnStop = new QPushButton(QString::fromLocal8Bit("Стоп"), this);
        _btnStop->setEnabled(false);
        buttons->addWidget(_btnCreate);
        buttons->addWidget(_btnRun);
        buttons->addWidget(_btnStop);
        buttons->addStretch(1);

        auto* secParams = new CollapsibleSection(QString::fromLocal8Bit("Параметры эмулятора"), this);
        auto* layoutParams = new QVBoxLayout();
        _paramsWidget = new EmulatorParamsWidget(this);
        layoutParams->addWidget(_paramsWidget);
        secParams->setContentLayout(layoutParams);

        auto* secFrame = new CollapsibleSection(QString::fromLocal8Bit("Параметры кадра"), this);
        auto* layoutFrame = new QVBoxLayout();
        _frameConfigWidget = new FrameConfigWidget(this);
        layoutFrame->addWidget(_frameConfigWidget);
        secFrame->setContentLayout(layoutFrame);

        auto* secIncome = new CollapsibleSection(QString::fromLocal8Bit("Источник данных абонентов"), this);
        auto* layoutIncome = new QVBoxLayout();
        _incomeLoadEstimatorWidget = new IncomeLoadEstimatorSelectionWidget(this);
        layoutIncome->addWidget(_incomeLoadEstimatorWidget);
        secIncome->setContentLayout(layoutIncome);

        auto* secStrategy = new CollapsibleSection(QString::fromLocal8Bit("Стратегия адаптивного доступа"), this);
        auto* layoutStrategy = new QVBoxLayout();
        _strategyWidget = new HubStrategySelectionWidget(this);
        layoutStrategy->addWidget(_strategyWidget);
        secStrategy->setContentLayout(layoutStrategy);

        root->addWidget(secParams);
        root->addWidget(secFrame);
        root->addWidget(secIncome);
        root->addWidget(secStrategy);
        root->addLayout(buttons);
        root->addWidget(_logWidget);

        _plotter = new MetricPlotter;
        root->addWidget(_plotter);

        // Таймер симуляции.
        _simTimer = new QTimer(this);
        _simTimer->setInterval(0);
        connect(_simTimer, &QTimer::timeout, this, &EmulatorConfigWidget::onSimTick);

        connect(_btnCreate, &QPushButton::clicked, this, &EmulatorConfigWidget::onCreateEmulatorClicked);
        connect(_btnRun, &QPushButton::clicked, this, &EmulatorConfigWidget::onStartSimulationClicked);
        connect(_btnStop, &QPushButton::clicked, this, &EmulatorConfigWidget::onStopSimulationClicked);

        _logWidget->setStatus(QString::fromLocal8Bit("Готов: создайте эмулятор и запустите симуляцию..."));
}

const Info& EmulatorConfigWidget::info() const
{
        return _info;
}

void EmulatorConfigWidget::init(std::shared_ptr<editor::IContext> context)
{
        _context = std::move(context);

        ctxSet(_context, ctx_keys::TerminalsJoined, 0);

        logLine(QStringLiteral("[init] context attached, params loaded/saved."));
}

int EmulatorConfigWidget::computeJoinedCount() const
{
        int joined = 0;
        for (const auto& st : _stations)
        {
                if (st && st->currentState() == TerminalState::OPERATION)
                        ++joined;
        }
        return joined;
}

QString EmulatorConfigWidget::terminalStateToString(TerminalState st) const
{
        switch (st)
        {
        case TerminalState::OFF:        return QStringLiteral("OFF");
        case TerminalState::ACQUISITION:  return QStringLiteral("ACQUISITION");
        case TerminalState::AUTH:       return QStringLiteral("AUTH");
        case TerminalState::OPERATION:     return QStringLiteral("OPERATION");
        }
        return QStringLiteral("UNKNOWN");
}

void EmulatorConfigWidget::logLine(const QString& line)
{
        _logWidget->log(line);
}

void EmulatorConfigWidget::setUiEnabled(bool enabled)
{
        _paramsWidget->setEnabled(enabled);
        _strategyWidget->setEnabled(enabled);
        _logWidget->setEnabled(enabled);

        _btnCreate->setEnabled(enabled);
        _btnRun->setEnabled(enabled);
        _btnStop->setEnabled(!enabled);
}

void EmulatorConfigWidget::onCreateEmulatorClicked()
{
        if (_running)
        {
                logLine(QString::fromLocal8Bit("[create] ошибка: симуляция уже идёт"));
                return;
        }

        // Сбрасываем предыдущее состояние.
        _plotter->detach();
        _emu.reset();
        _hub.reset();
        _stations.clear();
        _metricSink.reset();

        auto generalParams = _paramsWidget->data();
        auto frameConfig = _frameConfigWidget->getConfig();

        _duration = generalParams.duration;

        _rng.seed(static_cast<std::mt19937::result_type>(generalParams.seed));

        _metricSink = MetricSinkFactory::make();

        // Имена scope передаются внутрь IMetricSink в UTF-8.
        const std::string hubScopeName =
                QString::fromLocal8Bit("Хаб").toUtf8().toStdString();
        const std::string strategyScopeName =
                QString::fromLocal8Bit("Стратегия").toUtf8().toStdString();
        const std::string estimatorScopeName =
                QString::fromLocal8Bit("Оценка входной нагрузки").toUtf8().toStdString();

        EmulatorInitData initData;
        initData.metricSink = _metricSink;
        initData.stationFactory = [&](auto sendFunc, auto id, std::shared_ptr<starTopologyEmulator::IStationStatsCollector> stats)
                {
                        return StarStationFactory::make(
                                StarStationInitData({
                                        .sendFunc = sendFunc,
                                        .frameCalculator = FrameCalculatorFactory::abonentCalculator(frameConfig, generalParams.stationTts),
                                        .dynamicFrameSettings = DynamicFrameSettingsFactory::make(),
                                        .id = id,
                                        .messagesNeeded = generalParams.messagesNeeded,
                                        .tts = generalParams.stationTts,
                                        .rng = _rng,
                                        .statsCollector = std::move(stats),
                                }));
                };
        initData.hubFactory = [&, hubScopeName, strategyScopeName, estimatorScopeName](auto sendFunc)
                {
                        MetricScope hubScope(_metricSink, hubScopeName);
                        MetricScope estimatorScope = hubScope.child(estimatorScopeName);
                        MetricScope strategyScope = hubScope.child(strategyScopeName);

                        std::shared_ptr<IIncomeLoadEstimator> incomeLoadEstimator =
                                _incomeLoadEstimatorWidget->incomeLoadEstimator(estimatorScope);
                        std::shared_ptr<IFrameCalculator> hubFrameCalculator =
                                FrameCalculatorFactory::hubCalculator(frameConfig, generalParams.hubTts);
                        std::shared_ptr<IDynamicFrameSettings> hubDynamicFrameSettings =
                                DynamicFrameSettingsFactory::make();
                        std::shared_ptr<IBacklogAccumulator> backlogAccumulator =
                                BacklogAccumulatorFactory::make(frameConfig.bitsPerSlot);
                        auto strategy = _strategyWidget->strategy(
                                incomeLoadEstimator,
                                hubDynamicFrameSettings,
                                hubFrameCalculator,
                                backlogAccumulator,
                                frameConfig.slotCountInFrame,
                                strategyScope);
                        return StarHubFactory::make(
                                StarHubInitData({
                                        .sendFunc = sendFunc,
                                        .incomeLoadEstimator = incomeLoadEstimator,
                                        .frameCalculator = hubFrameCalculator,
                                        .dynamicFrameSettings = hubDynamicFrameSettings,
                                        .strategy = std::move(strategy),
                                        .backlogAccumulator = std::move(backlogAccumulator),
                                        .tts = generalParams.hubTts,
                                        .metricScope = hubScope,
                                }));
                };
        initData.abonentFrameCalculator = FrameCalculatorFactory::abonentCalculator(frameConfig, generalParams.stationTts);
        initData.hubFrameCalculator = FrameCalculatorFactory::hubCalculator(frameConfig, generalParams.hubTts);
        initData.stationCount = generalParams.numStations;

        _emu = EmulatorFactory::make(std::move(initData));

        _hub = _emu->hub();
        _stations = _emu->stations();

        _created = true;
        _currentSlot = 0;

        _plotter->attach(_metricSink);

        _logWidget->setProgress(0, _duration);
        _logWidget->setStatus(QString::fromLocal8Bit("Эмулятор создан. Можно запускать симуляцию."));
}

void EmulatorConfigWidget::onStartSimulationClicked()
{
        if (_running)
                return;

        _logWidget->setProgress(0, _duration);

        setUiEnabled(false);
        _logWidget->setStatus(QString::fromLocal8Bit("Симуляция запущена"));
        logLine(QStringLiteral("[run] started. maxSlots=%1, stepsPerTick=%2").arg(_duration).arg(_stepsPerTick));

        _running = true;
        _simTimer->start();
}

void EmulatorConfigWidget::onStopSimulationClicked()
{
        if (!_running)
                return;

        _simTimer->stop();
        _running = false;

        _logWidget->setStatus(QString::fromLocal8Bit("Симуляция остановлена."));
        logLine(QStringLiteral("[run] stopped by user"));

        setUiEnabled(true);
}

void EmulatorConfigWidget::onSimTick()
{
        if (!_running || !_emu || !_hub)
                return;

        const int maxSlots = _duration;

        const Timestamp t = _currentSlot;
        _emu->update(t);
        _hub->update(t);
        for (auto& st : _stations)
                st->update(t);

        ++_currentSlot;

        const int joined = computeJoinedCount();

        _logWidget->setProgress(static_cast<int>(std::min<Timestamp>(_currentSlot, maxSlots)), maxSlots);

        _logWidget->setStatus(QString::fromLocal8Bit("Текущий слот=%1/%2, joined=%3")
                .arg(static_cast<qint64>(_currentSlot))
                .arg(maxSlots)
                .arg(joined));

        if (_currentSlot >= maxSlots)
                finishSimulationAndShowResults();
}

void EmulatorConfigWidget::finishSimulationAndShowResults()
{
        _simTimer->stop();
        _running = false;

        setUiEnabled(true);
}