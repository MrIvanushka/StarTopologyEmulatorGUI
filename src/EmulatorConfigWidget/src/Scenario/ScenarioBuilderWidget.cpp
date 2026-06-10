#include "ScenarioBuilderWidget.h"

#include <algorithm>

#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonDocument>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

#include "ScenarioEventDialog.h"
#include "ScenarioJson.h"

using namespace starTopologyEmulator;

ScenarioBuilderWidget::ScenarioBuilderWidget(QWidget* parent)
	: QWidget(parent)
{
	setupUi();
}

const std::vector<ScenarioEventData>& ScenarioBuilderWidget::events() const
{
	return _events;
}

void ScenarioBuilderWidget::setEvents(std::vector<ScenarioEventData> events)
{
	_events = std::move(events);
	sortByTime();
	rebuildTable();
}

void ScenarioBuilderWidget::setupUi()
{
	auto* mainLayout = new QVBoxLayout(this);

	_table = new QTableWidget(this);
	_table->setColumnCount(4);
	_table->setHorizontalHeaderLabels({
		QString::fromLocal8Bit("Время"),
		QString::fromLocal8Bit("Событие"),
		QString::fromLocal8Bit("Станции"),
		QString::fromLocal8Bit("Параметры")
	});
	_table->horizontalHeader()->setStretchLastSection(true);
	_table->verticalHeader()->setVisible(false);
	_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	_table->setSelectionMode(QAbstractItemView::SingleSelection);
	_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	mainLayout->addWidget(_table);

	auto* btnRow = new QHBoxLayout();
	_btnAdd    = new QPushButton(QString::fromLocal8Bit("Добавить..."), this);
	_btnEdit   = new QPushButton(QString::fromLocal8Bit("Изменить..."), this);
	_btnRemove = new QPushButton(QString::fromLocal8Bit("Удалить"), this);
	_btnClear  = new QPushButton(QString::fromLocal8Bit("Очистить"), this);
	btnRow->addWidget(_btnAdd);
	btnRow->addWidget(_btnEdit);
	btnRow->addWidget(_btnRemove);
	btnRow->addWidget(_btnClear);
	btnRow->addStretch(1);
	mainLayout->addLayout(btnRow);

	auto* ioRow = new QHBoxLayout();
	_btnSave = new QPushButton(QString::fromLocal8Bit("Сохранить в JSON..."), this);
	_btnLoad = new QPushButton(QString::fromLocal8Bit("Загрузить из JSON..."), this);
	ioRow->addWidget(_btnSave);
	ioRow->addWidget(_btnLoad);
	ioRow->addStretch(1);
	mainLayout->addLayout(ioRow);

	connect(_btnAdd,    &QPushButton::clicked, this, &ScenarioBuilderWidget::onAddClicked);
	connect(_btnEdit,   &QPushButton::clicked, this, &ScenarioBuilderWidget::onEditClicked);
	connect(_btnRemove, &QPushButton::clicked, this, &ScenarioBuilderWidget::onRemoveClicked);
	connect(_btnClear,  &QPushButton::clicked, this, &ScenarioBuilderWidget::onClearClicked);
	connect(_btnSave,   &QPushButton::clicked, this, &ScenarioBuilderWidget::onSaveClicked);
	connect(_btnLoad,   &QPushButton::clicked, this, &ScenarioBuilderWidget::onLoadClicked);
}

QString ScenarioBuilderWidget::eventLabel(const ScenarioEventData& d) const
{
	switch (d.kind)
	{
	case ScenarioEventKind::STATION_ENABLE:      return QString::fromLocal8Bit("Включить");
	case ScenarioEventKind::STATION_DISABLE:     return QString::fromLocal8Bit("Выключить");
	case ScenarioEventKind::STATION_SET_PROFILE: return QString::fromLocal8Bit("Сменить профиль");
	default: return QString::fromLocal8Bit("Неизвестно");
	}
}

QString ScenarioBuilderWidget::summarizeProfile(const ScenarioProfileConfig& p) const
{
	return std::visit([](const auto& cfg) -> QString {
		using T = std::decay_t<decltype(cfg)>;
		if constexpr (std::is_same_v<T, CbrTrafficProfileConfig>)
			return QString::fromLocal8Bit("CBR, %1 бит/ts").arg(cfg.bitsPerTimestamp);
		else if constexpr (std::is_same_v<T, PoissonTrafficProfileConfig>)
			return QString::fromLocal8Bit("Пуассон, %1 пакетов/ts, %2 бит/пакет")
				.arg(cfg.packetsPerTimestamp).arg(cfg.bitsPerPacket);
		else if constexpr (std::is_same_v<T, BurstTrafficProfileConfig>)
			return QString::fromLocal8Bit("Берст (exp), on=%1, off=%2")
				.arg(cfg.meanOnDuration).arg(cfg.meanOffDuration);
		else if constexpr (std::is_same_v<T, ParetoBurstTrafficProfileConfig>)
			return QString::fromLocal8Bit("Берст (Парето), on>=%1, off>=%2, alpha=%3")
				.arg(cfg.minOnDuration).arg(cfg.minOffDuration).arg(cfg.alpha);
		return QString();
	}, p);
}

void ScenarioBuilderWidget::rebuildTable()
{
	_table->setRowCount(static_cast<int>(_events.size()));
	for (int i = 0; i < static_cast<int>(_events.size()); ++i)
	{
		const auto& e = _events[i];
		_table->setItem(i, 0, new QTableWidgetItem(QString::number(static_cast<qint64>(e.at))));
		_table->setItem(i, 1, new QTableWidgetItem(eventLabel(e)));
		const QString stations = (e.minId == e.maxId)
			? QString::number(e.minId)
			: QString("%1–%2").arg(e.minId).arg(e.maxId);
		_table->setItem(i, 2, new QTableWidgetItem(stations));

		QString params;
		if (e.kind == ScenarioEventKind::STATION_SET_PROFILE)
			params = summarizeProfile(e.profile);
		_table->setItem(i, 3, new QTableWidgetItem(params));
	}
	_table->resizeColumnsToContents();
	_table->horizontalHeader()->setStretchLastSection(true);
}

void ScenarioBuilderWidget::sortByTime()
{
	std::stable_sort(_events.begin(), _events.end(),
		[](const ScenarioEventData& a, const ScenarioEventData& b) {
			return a.at < b.at;
		});
}

void ScenarioBuilderWidget::onAddClicked()
{
	ScenarioEventDialog dlg(this);
	if (dlg.exec() != QDialog::Accepted)
		return;
	_events.push_back(dlg.data());
	sortByTime();
	rebuildTable();
}

void ScenarioBuilderWidget::onEditClicked()
{
	const int row = _table->currentRow();
	if (row < 0 || row >= static_cast<int>(_events.size()))
		return;
	ScenarioEventDialog dlg(this);
	dlg.setData(_events[row]);
	if (dlg.exec() != QDialog::Accepted)
		return;
	_events[row] = dlg.data();
	sortByTime();
	rebuildTable();
}

void ScenarioBuilderWidget::onRemoveClicked()
{
	const int row = _table->currentRow();
	if (row < 0 || row >= static_cast<int>(_events.size()))
		return;
	_events.erase(_events.begin() + row);
	rebuildTable();
}

void ScenarioBuilderWidget::onClearClicked()
{
	if (_events.empty())
		return;
	const auto answer = QMessageBox::question(this,
		QString::fromLocal8Bit("Очистить сценарий"),
		QString::fromLocal8Bit("Удалить все события?"));
	if (answer != QMessageBox::Yes)
		return;
	_events.clear();
	rebuildTable();
}

void ScenarioBuilderWidget::onSaveClicked()
{
	const QString path = QFileDialog::getSaveFileName(this,
		QString::fromLocal8Bit("Сохранить сценарий"),
		QString(),
		QString::fromLocal8Bit("JSON (*.json)"));
	if (path.isEmpty())
		return;

	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QMessageBox::warning(this,
			QString::fromLocal8Bit("Ошибка"),
			QString::fromLocal8Bit("Не удалось открыть файл для записи."));
		return;
	}
	file.write(ScenarioJson::toJson(_events).toJson(QJsonDocument::Indented));
}

void ScenarioBuilderWidget::onLoadClicked()
{
	const QString path = QFileDialog::getOpenFileName(this,
		QString::fromLocal8Bit("Загрузить сценарий"),
		QString(),
		QString::fromLocal8Bit("JSON (*.json)"));
	if (path.isEmpty())
		return;

	QFile file(path);
	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::warning(this,
			QString::fromLocal8Bit("Ошибка"),
			QString::fromLocal8Bit("Не удалось открыть файл."));
		return;
	}

	QJsonParseError parseErr;
	const auto doc = QJsonDocument::fromJson(file.readAll(), &parseErr);
	if (parseErr.error != QJsonParseError::NoError)
	{
		QMessageBox::warning(this,
			QString::fromLocal8Bit("Ошибка разбора JSON"),
			parseErr.errorString());
		return;
	}

	QString err;
	auto loaded = ScenarioJson::fromJson(doc, &err);
	if (!err.isEmpty())
	{
		QMessageBox::warning(this,
			QString::fromLocal8Bit("Ошибка структуры сценария"),
			err);
		return;
	}

	setEvents(std::move(loaded));
}