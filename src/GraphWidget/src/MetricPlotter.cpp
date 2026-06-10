#include "MetricPlotter.h"

#include <QDateTime>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLocale>
#include <QMessageBox>
#include <QSaveFile>
#include <QTextStream>
#include <algorithm>
#include <cmath>
#include <limits>

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#include <QRandomGenerator>
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringConverter>
#endif

using starTopologyEmulator::IMetricSink;
using starTopologyEmulator::MetricHandle;
using starTopologyEmulator::MetricSample;

namespace {
// Ширина видимого окна по оси X (фиксированная, в фреймах).
// Окно сдвигается вправо вместе с последним фреймом.
constexpr double kXWindow = 100.0;

// Допуск на сравнение double, чтобы определить, не подвинул ли пользователь
// ось вручную (любое отклонение > этого значения считаем пользовательским).
constexpr double kFollowToleranceFrames = 1.0;
} // namespace

MetricPlotter::MetricPlotter(QWidget* parent)
        : QWidget(parent)
{
        setupUi();

        _replotTimer = new QTimer(this);
        _replotTimer->setInterval(100);
        connect(_replotTimer, &QTimer::timeout, this, &MetricPlotter::onReplotTick);
        _replotTimer->start();
}

MetricPlotter::~MetricPlotter()
{
        detach();
}

void MetricPlotter::attach(std::shared_ptr<IMetricSink> sink)
{
        // Запоминаем включённые серии до полной очистки, чтобы после
        // пересборки дерева восстановить выбор пользователя.
        const QStringList previouslyEnabled = enabledMetricPaths();

        detach();
        clearAll();

        _sink = std::move(sink);
        if (!_sink)
                return;

        // Подхватываем метрики, зарегистрированные/уже emit-нутые до подписки.
        for (const auto& path : _sink->paths()) {
                const MetricHandle h = _sink->find(path);
                if (h == starTopologyEmulator::kInvalidMetricHandle)
                        continue;
                ensurePathItem(QString::fromUtf8(path.c_str()));
                for (const auto& s : _sink->series(h))
                        _pending.push_back({h, s});
        }

        // Восстанавливаем галочки для путей, которые юзер уже включал.
        // Срабатывание handleTreeClick через сигнал itemChanged создаст графики.
        for (const QString& path : previouslyEnabled) {
                auto it = _itemByPath.find(path);
                if (it == _itemByPath.end() || !it.value())
                        continue;
                it.value()->setCheckState(Qt::Checked);
        }

        _subscriptionId = _sink->subscribe(
                [this](MetricHandle handle, MetricSample sample) {
                        // Sink emit'ится из той же нити, что и UI, поэтому
                        // буферизуем без синхронизации.
                        _pending.push_back({handle, sample});
                });
        _hasSubscription = true;
        _dirty = true;
}

void MetricPlotter::detach()
{
        if (_sink && _hasSubscription) {
                _sink->unsubscribe(_subscriptionId);
        }
        _hasSubscription = false;
        _subscriptionId = 0;
        _sink.reset();
        _pending.clear();
}

void MetricPlotter::clearAll()
{
        customPlot->clearGraphs();
        allData.clear();
        _itemByPath.clear();
        treeModel->clear();
        _maxFrame = 0.0;
        _dirty = false;
        customPlot->xAxis->setRange(0.0, kXWindow);
        _lastAutoSetUpper = kXWindow;
        customPlot->replot();
}

QStandardItem* MetricPlotter::ensurePathItem(const QString& path)
{
        if (path.isEmpty())
                return nullptr;

        auto it = _itemByPath.find(path);
        if (it != _itemByPath.end())
                return it.value();

        const int slash = path.lastIndexOf('/');
        QStandardItem* parent = nullptr;
        QString leaf;
        if (slash < 0) {
                parent = treeModel->invisibleRootItem();
                leaf = path;
        }
        else {
                parent = ensurePathItem(path.left(slash));
                leaf = path.mid(slash + 1);
                if (!parent)
                        parent = treeModel->invisibleRootItem();
        }

        auto* item = new QStandardItem(leaf);
        item->setEditable(false);
        item->setCheckable(true);
        item->setCheckState(Qt::Unchecked);
        item->setData(path, Qt::UserRole);
        parent->appendRow(item);
        _itemByPath.insert(path, item);
        return item;
}

void MetricPlotter::onReplotTick()
{
        if (_pending.empty() && !_dirty)
                return;

        const double prevMaxFrame = _maxFrame;

        // Сливаем накопленные сэмплы в серии.
        if (!_pending.empty()) {
                std::vector<PendingSample> drained;
                drained.swap(_pending);

                for (const auto& ps : drained) {
                        if (!_sink)
                                continue;
                        const auto pathSv = _sink->path(ps.handle);
                        if (pathSv.empty())
                                continue;
                        const QString path = QString::fromUtf8(pathSv.data(), static_cast<int>(pathSv.size()));

                        ensurePathItem(path);

                        auto& data = allData[path];
                        const double x = static_cast<double>(ps.sample.frame);
                        data.times.append(x);
                        data.values.append(ps.sample.value);
                        if (data.graph)
                                data.graph->addData(x, ps.sample.value);

                        if (x > _maxFrame)
                                _maxFrame = x;
                }
        }

        // X-ось двигаем только если 1) реально пришли новые данные
        // и 2) пользователь сам не трогал вьюпорт.
        if (_maxFrame > prevMaxFrame)
                maybeFollowXAxis();

        // Y-ось всегда подгоняем под все значения включённых серий —
        // независимо от того, что вызвало replot (новые данные или
        // вкл/выкл графика).
        rescaleYAxis();

        customPlot->replot();
        _dirty = false;
}

void MetricPlotter::maybeFollowXAxis()
{
        const QCPRange currentRange = customPlot->xAxis->range();
        // Считаем, что юзер не трогал ось, если и правая граница, и ширина
        // совпадают с тем, что мы выставили в прошлый раз. Любая прокрутка
        // или зум сдвинут эти параметры за пределы допуска и follow-mode
        // отключится; при этом scroll-to-end не реактивирует follow.
        const bool upperMatches =
                std::abs(currentRange.upper - _lastAutoSetUpper) <= kFollowToleranceFrames;
        const bool widthMatches =
                std::abs(currentRange.size() - kXWindow) <= kFollowToleranceFrames;
        if (!upperMatches || !widthMatches)
                return;

        const double right = std::max(_maxFrame, kXWindow);
        const double left = right - kXWindow;
        customPlot->xAxis->setRange(left, right);
        _lastAutoSetUpper = right;
}

void MetricPlotter::rescaleYAxis()
{
        double ymin = std::numeric_limits<double>::infinity();
        double ymax = -std::numeric_limits<double>::infinity();
        bool any = false;

        for (auto it = allData.cbegin(); it != allData.cend(); ++it) {
                const MetricData& data = it.value();
                if (!data.graph)
                        continue;
                for (double v : data.values) {
                        if (v < ymin) ymin = v;
                        if (v > ymax) ymax = v;
                        any = true;
                }
        }

        if (!any)
                return;

        double pad;
        if (std::abs(ymax - ymin) < 1e-12) {
                // Все значения одинаковые — даём константный отступ, чтобы
                // линия не прилипала к границе.
                pad = std::max(std::abs(ymin) * 0.1, 0.5);
        } else {
                pad = (ymax - ymin) * 0.05;
        }
        customPlot->yAxis->setRange(ymin - pad, ymax + pad);
}

void MetricPlotter::handleTreeClick(QStandardItem* item)
{
        if (!item || !item->isCheckable())
                return;

        const QString path = item->data(Qt::UserRole).toString();
        if (path.isEmpty())
                return;

        if (item->checkState() == Qt::Checked)
                enableGraph(path, item->text());
        else
                disableGraph(path);

        _dirty = true;
}

void MetricPlotter::setupUi()
{
        auto* layout = new QVBoxLayout(this);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        auto* topBar = new QHBoxLayout();
        exportButton = new QPushButton(QString::fromLocal8Bit("Экспорт включенных метрик"), this);
        topBar->addStretch();
        topBar->addWidget(exportButton);
        layout->addLayout(topBar);

        auto* splitter = new QSplitter(Qt::Horizontal, this);

        treeView = new QTreeView(this);
        treeModel = new QStandardItemModel(this);
        treeView->setModel(treeModel);
        treeView->setHeaderHidden(true);

        customPlot = new QCustomPlot(this);
        customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        customPlot->xAxis->setLabel("Frame");
        customPlot->yAxis->setLabel("Value");
        customPlot->xAxis->setRange(0.0, kXWindow);
        _lastAutoSetUpper = kXWindow;

        splitter->addWidget(treeView);
        splitter->addWidget(customPlot);
        layout->addWidget(splitter);

        connect(treeModel, &QStandardItemModel::itemChanged,
                this, &MetricPlotter::handleTreeClick);

        connect(exportButton, &QPushButton::clicked,
                this, &MetricPlotter::exportEnabledMetrics);
}

void MetricPlotter::enableGraph(const QString& path, const QString& name)
{
        auto& data = allData[path];
        if (data.graph)
                return;

        data.graph = customPlot->addGraph();
        data.graph->setName(name);
        data.graph->setData(data.times, data.values);

        if (!data.color.isValid()) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
                data.color = QColor::fromHsv(QRandomGenerator::global()->bounded(360), 200, 200);
#else
                data.color = QColor::fromHsv(qrand() % 360, 200, 200);
#endif
        }

        data.graph->setPen(QPen(data.color, 2));
}

void MetricPlotter::disableGraph(const QString& path)
{
        auto it = allData.find(path);
        if (it == allData.end())
                return;
        if (!it.value().graph)
                return;

        customPlot->removeGraph(it.value().graph);
        it.value().graph = nullptr;
}

QStringList MetricPlotter::enabledMetricPaths() const
{
        QStringList result;

        for (auto it = allData.cbegin(); it != allData.cend(); ++it) {
                const MetricData& data = it.value();
                if (data.graph != nullptr)
                        result << it.key();
        }

        std::sort(result.begin(), result.end());
        return result;
}

QString MetricPlotter::escapeDelimited(const QString& value, QChar separator)
{
        QString escaped = value;
        escaped.replace('"', "\"\"");

        const bool needsQuotes =
                escaped.contains(separator) ||
                escaped.contains('\n') ||
                escaped.contains('\r') ||
                escaped.contains('"');

        return needsQuotes ? "\"" + escaped + "\"" : escaped;
}

bool MetricPlotter::saveEnabledMetricsToDelimitedText(const QString& fileName,
        QString* errorMessage) const
{
        const QStringList paths = enabledMetricPaths();
        if (paths.isEmpty()) {
                if (errorMessage)
                        *errorMessage = QString::fromLocal8Bit("Нет включенных метрик.");
                return false;
        }

        const bool isTsv = fileName.endsWith(".tsv", Qt::CaseInsensitive);

        QChar separator = '\t';
        if (!isTsv) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                const bool decimalComma = QLocale::system().decimalPoint().contains(',');
#else
                const bool decimalComma = (QLocale::system().decimalPoint() == QLatin1Char(','));
#endif
                separator = decimalComma ? ';' : ',';
        }

        QSaveFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                if (errorMessage)
                        *errorMessage = file.errorString();
                return false;
        }

        QTextStream out(&file);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        out.setEncoding(QStringConverter::Utf8);
#else
        out.setCodec("UTF-8");
#endif
        out.setGenerateByteOrderMark(true);

        QLocale numberLocale = QLocale::system();
        numberLocale.setNumberOptions(numberLocale.numberOptions() | QLocale::OmitGroupSeparator);

        QVector<double> allTimestamps;
        for (const QString& path : paths) {
                const auto it = allData.constFind(path);
                if (it == allData.cend())
                        continue;

                const MetricData& data = it.value();
                const int count = qMin(data.times.size(), data.values.size());

                allTimestamps.reserve(allTimestamps.size() + count);
                for (int i = 0; i < count; ++i)
                        allTimestamps.push_back(data.times.at(i));
        }

        std::sort(allTimestamps.begin(), allTimestamps.end());
        allTimestamps.erase(std::unique(allTimestamps.begin(), allTimestamps.end()),
                allTimestamps.end());

        QVector<QMap<double, double>> seriesByMetric;
        seriesByMetric.reserve(paths.size());

        for (const QString& path : paths) {
                QMap<double, double> series;

                const auto it = allData.constFind(path);
                if (it != allData.cend()) {
                        const MetricData& data = it.value();
                        const int count = qMin(data.times.size(), data.values.size());

                        for (int i = 0; i < count; ++i)
                                series.insert(data.times.at(i), data.values.at(i));
                }

                seriesByMetric.push_back(series);
        }

        out << escapeDelimited("frame", separator);
        for (const QString& path : paths)
                out << separator << escapeDelimited(path, separator);
        out << '\n';

        for (double ts : allTimestamps) {
                out << numberLocale.toString(ts, 'g', 17);

                for (const auto& series : seriesByMetric) {
                        out << separator;

                        const auto it = series.constFind(ts);
                        if (it != series.cend())
                                out << numberLocale.toString(it.value(), 'g', 17);
                }

                out << '\n';
        }

        out.flush();
        if (out.status() != QTextStream::Ok) {
                if (errorMessage)
                        *errorMessage = QString::fromLocal8Bit("Ошибка записи итогового файла.");
                return false;
        }

        if (!file.commit()) {
                if (errorMessage)
                        *errorMessage = file.errorString();
                return false;
        }

        return true;
}

void MetricPlotter::exportEnabledMetrics()
{
        const QStringList active = enabledMetricPaths();
        if (active.isEmpty()) {
                QMessageBox::information(this,
                        QString::fromLocal8Bit("Экспорт"),
                        QString::fromLocal8Bit("Нет включенных метрик для экспорта."));
                return;
        }

        const QString defaultBaseName =
                QStringLiteral("metrics_%1")
                .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));

        QString selectedFilter;
        QString fileName = QFileDialog::getSaveFileName(
                this,
                QString::fromLocal8Bit("Сохранить включенные метрики"),
                defaultBaseName + ".csv",
                tr("CSV/Excel (*.csv);;TSV/Excel (*.tsv)"),
                &selectedFilter
        );

        if (fileName.isEmpty())
                return;

        if (QFileInfo(fileName).suffix().isEmpty())
                fileName += selectedFilter.contains("*.tsv") ? ".tsv" : ".csv";

        QString errorMessage;
        if (!saveEnabledMetricsToDelimitedText(fileName, &errorMessage)) {
                QMessageBox::critical(
                        this,
                        QString::fromLocal8Bit("Ошибка экспорта"),
                        QString::fromLocal8Bit("Не удалось сохранить файл:\n%1\n\nПричина: %2")
                        .arg(fileName,
                                errorMessage.isEmpty() ? QString::fromLocal8Bit("неизвестная ошибка") : errorMessage)
                );
                return;
        }

        QMessageBox::information(
                this,
                QString::fromLocal8Bit("Экспорт завершён"),
                QString::fromLocal8Bit("Сохранено серий: %1\nФайл: %2")
                .arg(active.size())
                .arg(fileName)
        );
}