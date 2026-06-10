#pragma once

#include <QHash>
#include <QPushButton>
#include <QSplitter>
#include <QStandardItemModel>
#include <QStringList>
#include <QTimer>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidget>

#include <memory>

#pragma push_macro("emit")
#undef emit
#include <StarTopologyEmulator/Metrics/MetricSink.h>
#pragma pop_macro("emit")

#include "CustomPlot.h"

struct MetricData {
        QVector<double> times;
        QVector<double> values;
        QCPGraph* graph = nullptr;
        QColor color;
};

class MetricPlotter : public QWidget {
        Q_OBJECT

public:
        explicit MetricPlotter(QWidget* parent = nullptr);
        ~MetricPlotter() override;

        // Subscribes to live samples from the given sink. Resets internal state.
        void attach(std::shared_ptr<starTopologyEmulator::IMetricSink> sink);

        // Stops the subscription and clears the plot/tree.
        void detach();

public slots:
        void exportEnabledMetrics();

private slots:
        void handleTreeClick(QStandardItem* item);
        void onReplotTick();

private:
        struct PendingSample
        {
                starTopologyEmulator::MetricHandle handle;
                starTopologyEmulator::MetricSample sample;
        };

        void setupUi();
        void clearAll();

        QStandardItem* ensurePathItem(const QString& path);

        void enableGraph(const QString& path, const QString& name);
        void disableGraph(const QString& path);

        // Сдвигаем правую границу X к последнему фрейму, сохраняя фиксированную
        // ширину окна. Не делает ничего, если пользователь сам подвинул ось.
        void maybeFollowXAxis();

        // Подгоняем диапазон Y, чтобы все значения включённых серий помещались.
        void rescaleYAxis();

        QStringList enabledMetricPaths() const;
        bool saveEnabledMetricsToDelimitedText(const QString& fileName,
                QString* errorMessage = nullptr) const;
        static QString escapeDelimited(const QString& value, QChar separator);

        QCustomPlot* customPlot = nullptr;
        QTreeView* treeView = nullptr;
        QStandardItemModel* treeModel = nullptr;
        QPushButton* exportButton = nullptr;
        QTimer* _replotTimer = nullptr;

        QHash<QString, MetricData> allData;
        QHash<QString, QStandardItem*> _itemByPath;

        std::shared_ptr<starTopologyEmulator::IMetricSink> _sink;
        starTopologyEmulator::IMetricSink::SubscriptionId _subscriptionId = 0;
        bool _hasSubscription = false;

        std::vector<PendingSample> _pending;
        double _maxFrame = 0.0;
        bool _dirty = false;

        // Правый край видимой области, который мы выставили автоматически
        // в прошлый replot. Используется для определения, не подвинул ли
        // пользователь ось сам.
        double _lastAutoSetUpper = 0.0;
};