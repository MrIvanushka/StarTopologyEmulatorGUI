#include "BenchmarkWidget.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QSignalBlocker>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonDocument>
#include <QLabel>
#include <QMessageBox>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "BenchImplDialog.h"
#include "BenchJson.h"
#include "../Scenario/ScenarioBuilderWidget.h"
#include "BenchPlan.h"
#include "ParamRegistry.h"
#include "../Scenario/ScenarioJson.h"

namespace
{

void setupDouble(QDoubleSpinBox* sb, double min, double max, double step, int decimals)
{
	sb->setRange(min, max);
	sb->setSingleStep(step);
	sb->setDecimals(decimals);
}

QString sweepLabel(const ParamValue& v)
{
	return std::visit([](const auto& x) -> QString {
		using T = std::decay_t<decltype(x)>;
		if constexpr (std::is_same_v<T, double>) return QString::number(x);
		else if constexpr (std::is_same_v<T, int>) return QString::number(x);
		else if constexpr (std::is_same_v<T, bool>) return x ? QStringLiteral("true") : QStringLiteral("false");
		else if constexpr (std::is_same_v<T, SweepRange>)
			return QStringLiteral("[%1..%2/%3]").arg(x.min).arg(x.max).arg(x.step);
		return QString();
	}, v);
}

QString summarizeImpl(const ImplConfig& cfg)
{
	QStringList parts;
	for (auto it = cfg.params.constBegin(); it != cfg.params.constEnd(); ++it)
	{
		if (std::holds_alternative<SweepRange>(it.value()))
			parts << (it.key() + "=" + sweepLabel(it.value()));
	}
	if (parts.isEmpty())
		return QString::fromLocal8Bit("(без развёрток)");
	return parts.join(", ");
}

}

const editor::Info BenchmarkWidget::_info{
	QString::fromLocal8Bit("Прогон сценариев"),
	QString::fromLocal8Bit("Создание и запуск симуляции StarHub/StarStation + статистика")
};

BenchmarkWidget::BenchmarkWidget()
{
	setupUi();
}

const editor::Info& BenchmarkWidget::info() const
{
	return _info;
}

void BenchmarkWidget::init(std::shared_ptr<editor::IContext> context)
{
	
}

void BenchmarkWidget::setupUi()
{
	auto* main = new QVBoxLayout(this);

	auto* baseBox = new QGroupBox(QString::fromLocal8Bit("Базовая конфигурация"), this);
	auto* baseRow = new QHBoxLayout(baseBox);

	auto* paramsCol = new QFormLayout;
	_numStations = new QSpinBox; _numStations->setRange(1, 100000); _numStations->setValue(50);
	_duration = new QSpinBox; _duration->setRange(1, INT_MAX); _duration->setValue(10000);
	_messagesNeeded = new QSpinBox; _messagesNeeded->setRange(1, 100000); _messagesNeeded->setValue(1);
	_slotDuration = new QSpinBox; _slotDuration->setRange(1, INT_MAX); _slotDuration->setValue(100);
	_slotCountInFrame = new QSpinBox; _slotCountInFrame->setRange(1, 1000); _slotCountInFrame->setValue(10);
	_bitsPerSlot = new QSpinBox; _bitsPerSlot->setRange(1, INT_MAX); _bitsPerSlot->setValue(1024);
	_hubTts = new QSpinBox; _hubTts->setRange(1, INT_MAX); _hubTts->setValue(100);
	_stationTts = new QSpinBox; _stationTts->setRange(1, INT_MAX); _stationTts->setValue(100);
	_seed = new QSpinBox; _seed->setRange(0, INT_MAX); _seed->setValue(42);
	_threadCount = new QSpinBox; _threadCount->setRange(0, 256); _threadCount->setValue(0);
	paramsCol->addRow(QString::fromLocal8Bit("Число абонентов:"), _numStations);
	paramsCol->addRow(QString::fromLocal8Bit("Длительность (слотов):"), _duration);
	paramsCol->addRow(QString::fromLocal8Bit("Сообщений на станцию:"), _messagesNeeded);
	paramsCol->addRow(QString::fromLocal8Bit("Длительность слота:"), _slotDuration);
	paramsCol->addRow(QString::fromLocal8Bit("Слотов в кадре:"), _slotCountInFrame);
	paramsCol->addRow(QString::fromLocal8Bit("Бит на слот:"), _bitsPerSlot);
	paramsCol->addRow(QString::fromLocal8Bit("TTS хаба:"), _hubTts);
	paramsCol->addRow(QString::fromLocal8Bit("TTS станции:"), _stationTts);
	paramsCol->addRow(QString::fromLocal8Bit("Seed:"), _seed);
	paramsCol->addRow(QString::fromLocal8Bit("Потоков (0 = авто):"), _threadCount);
	baseRow->addLayout(paramsCol);

	auto* estimatorCol = new QFormLayout;
	_estimatorKind = new QComboBox;
	_estimatorKind->addItem(QString::fromLocal8Bit("EMA"));
	_estimatorKind->addItem(QString::fromLocal8Bit("Калман"));
	_emaAlphaG = new QDoubleSpinBox; setupDouble(_emaAlphaG, 0.0, 1.0, 0.01, 4); _emaAlphaG->setValue(0.2);
	_emaAlphaPlr = new QDoubleSpinBox; setupDouble(_emaAlphaPlr, 0.0, 1.0, 0.01, 4); _emaAlphaPlr->setValue(0.15);
	_emaCollisionWeight = new QDoubleSpinBox; setupDouble(_emaCollisionWeight, 0.0, 100.0, 0.01, 4); _emaCollisionWeight->setValue(2.39);
	_kalmanQG = new QDoubleSpinBox; setupDouble(_kalmanQG, 0.0, 100.0, 0.0001, 6); _kalmanQG->setValue(0.001);
	_kalmanQPlr = new QDoubleSpinBox; setupDouble(_kalmanQPlr, 0.0, 100.0, 0.0001, 6); _kalmanQPlr->setValue(0.001);
	_kalmanRBase = new QDoubleSpinBox; setupDouble(_kalmanRBase, 0.0, 100.0, 0.01, 4); _kalmanRBase->setValue(0.5);
	_kalmanCollisionWeight = new QDoubleSpinBox; setupDouble(_kalmanCollisionWeight, 0.0, 100.0, 0.01, 4); _kalmanCollisionWeight->setValue(2.39);
	estimatorCol->addRow(QString::fromLocal8Bit("Оценщик нагрузки:"), _estimatorKind);
	estimatorCol->addRow(QString::fromLocal8Bit("EMA: alphaG"), _emaAlphaG);
	estimatorCol->addRow(QString::fromLocal8Bit("EMA: alphaPlr"), _emaAlphaPlr);
	estimatorCol->addRow(QString::fromLocal8Bit("EMA: collisionWeight"), _emaCollisionWeight);
	estimatorCol->addRow(QString::fromLocal8Bit("Калман: qG"), _kalmanQG);
	estimatorCol->addRow(QString::fromLocal8Bit("Калман: qPlr"), _kalmanQPlr);
	estimatorCol->addRow(QString::fromLocal8Bit("Калман: rBase"), _kalmanRBase);
	estimatorCol->addRow(QString::fromLocal8Bit("Калман: collisionWeight"), _kalmanCollisionWeight);
	baseRow->addLayout(estimatorCol);

	auto* predictorCol = new QFormLayout;
	_predictorKind = new QComboBox;
	_predictorKind->addItem(QString::fromLocal8Bit("Линейная регрессия"));
	_predictorKind->addItem(QString::fromLocal8Bit("Когортный"));
	_predictorKind->addItem(QString::fromLocal8Bit("Когортный с backoff"));
	_predictorKind->addItem(QString::fromLocal8Bit("Серая модель GM(1,1)"));
	_linRegRegressionWindow = new QSpinBox; _linRegRegressionWindow->setRange(1, 100000); _linRegRegressionWindow->setValue(16);
	_linRegForgettingHorizon = new QDoubleSpinBox; setupDouble(_linRegForgettingHorizon, 0.0, 1e6, 0.1, 4); _linRegForgettingHorizon->setValue(2.0);
	_linRegMinProb = new QDoubleSpinBox; setupDouble(_linRegMinProb, 0.0, 1.0, 0.001, 6); _linRegMinProb->setValue(0.01);
	_linRegMaxProb = new QDoubleSpinBox; setupDouble(_linRegMaxProb, 0.0, 1.0, 0.001, 4); _linRegMaxProb->setValue(1.0);
	_linRegEpsilon = new QDoubleSpinBox; setupDouble(_linRegEpsilon, 0.0, 1.0, 1e-6, 9); _linRegEpsilon->setValue(1e-9);
	_greyWindowSize = new QSpinBox; _greyWindowSize->setRange(1, 100000); _greyWindowSize->setValue(8);
	_greyMinHistory = new QSpinBox; _greyMinHistory->setRange(1, 100000); _greyMinHistory->setValue(4);
	_greyMinProb = new QDoubleSpinBox; setupDouble(_greyMinProb, 0.0, 1.0, 0.001, 6); _greyMinProb->setValue(0.01);
	_greyMaxProb = new QDoubleSpinBox; setupDouble(_greyMaxProb, 0.0, 1.0, 0.001, 4); _greyMaxProb->setValue(1.0);
	_greyEpsilon = new QDoubleSpinBox; setupDouble(_greyEpsilon, 0.0, 1.0, 1e-6, 9); _greyEpsilon->setValue(1e-9);
	predictorCol->addRow(QString::fromLocal8Bit("Предсказатель абонентов:"), _predictorKind);
	predictorCol->addRow(QString::fromLocal8Bit("LinReg: окно"), _linRegRegressionWindow);
	predictorCol->addRow(QString::fromLocal8Bit("LinReg: forgettingHorizonSec"), _linRegForgettingHorizon);
	predictorCol->addRow(QString::fromLocal8Bit("LinReg: pMin"), _linRegMinProb);
	predictorCol->addRow(QString::fromLocal8Bit("LinReg: pMax"), _linRegMaxProb);
	predictorCol->addRow(QString::fromLocal8Bit("LinReg: epsilon"), _linRegEpsilon);
	predictorCol->addRow(QString::fromLocal8Bit("GreyModel: windowSize"), _greyWindowSize);
	predictorCol->addRow(QString::fromLocal8Bit("GreyModel: minHistory"), _greyMinHistory);
	predictorCol->addRow(QString::fromLocal8Bit("GreyModel: pMin"), _greyMinProb);
	predictorCol->addRow(QString::fromLocal8Bit("GreyModel: pMax"), _greyMaxProb);
	predictorCol->addRow(QString::fromLocal8Bit("GreyModel: epsilon"), _greyEpsilon);
	baseRow->addLayout(predictorCol);

	main->addWidget(baseBox);

	auto buildImplTable = [](const QString& title, QTableWidget*& table, QPushButton*& add, QPushButton*& edit, QPushButton*& rem) {
		auto* box = new QGroupBox(title);
		auto* layout = new QVBoxLayout(box);
		table = new QTableWidget;
		table->setColumnCount(2);
		table->setHorizontalHeaderLabels({
			QString::fromLocal8Bit("Реализация"),
			QString::fromLocal8Bit("Развёртки")
		});
		table->horizontalHeader()->setStretchLastSection(true);
		table->verticalHeader()->setVisible(false);
		table->setSelectionBehavior(QAbstractItemView::SelectRows);
		table->setSelectionMode(QAbstractItemView::SingleSelection);
		table->setEditTriggers(QAbstractItemView::NoEditTriggers);
		layout->addWidget(table);
		auto* row = new QHBoxLayout;
		add = new QPushButton(QString::fromLocal8Bit("Добавить..."));
		edit = new QPushButton(QString::fromLocal8Bit("Изменить..."));
		rem = new QPushButton(QString::fromLocal8Bit("Удалить"));
		row->addWidget(add); row->addWidget(edit); row->addWidget(rem); row->addStretch(1);
		layout->addLayout(row);
		return box;
	};

	QPushButton* addF; QPushButton* editF; QPushButton* remF;
	QPushButton* addC; QPushButton* editC; QPushButton* remC;
	auto* ftpBox = buildImplTable(QString::fromLocal8Bit("FTP-генераторы"), _ftpTable, addF, editF, remF);
	auto* ctlBox = buildImplTable(QString::fromLocal8Bit("Контроллеры нагрузки"), _controllerTable, addC, editC, remC);

	auto* implRow = new QHBoxLayout;
	implRow->addWidget(ftpBox);
	implRow->addWidget(ctlBox);
	main->addLayout(implRow);

	auto* scenarioBox = new QGroupBox(QString::fromLocal8Bit("Сценарии (ось прогона)"));
	auto* scenarioLayout = new QHBoxLayout(scenarioBox);

	auto* scenarioListCol = new QVBoxLayout;
	_scenarioList = new QListWidget;
	_scenarioList->setSelectionMode(QAbstractItemView::SingleSelection);
	scenarioListCol->addWidget(_scenarioList);
	auto* scenarioBtnRow = new QHBoxLayout;
	auto* addScen  = new QPushButton(QString::fromLocal8Bit("Добавить"));
	auto* loadScen = new QPushButton(QString::fromLocal8Bit("Загрузить сценарии..."));
	auto* remScen  = new QPushButton(QString::fromLocal8Bit("Удалить"));
	scenarioBtnRow->addWidget(addScen);
	scenarioBtnRow->addWidget(loadScen);
	scenarioBtnRow->addWidget(remScen);
	scenarioListCol->addLayout(scenarioBtnRow);
	scenarioLayout->addLayout(scenarioListCol, 1);

	_scenarioBuilder = new ScenarioBuilderWidget;
	scenarioLayout->addWidget(_scenarioBuilder, 2);

	main->addWidget(scenarioBox);

	connect(addScen,  &QPushButton::clicked, this, &BenchmarkWidget::onAddScenario);
	connect(loadScen, &QPushButton::clicked, this, &BenchmarkWidget::onLoadScenarios);
	connect(remScen,  &QPushButton::clicked, this, &BenchmarkWidget::onRemoveScenario);
	connect(_scenarioList, &QListWidget::currentRowChanged, this,
		[this](int) { onScenarioSelectionChanged(); });

	onAddScenario();

	auto* outRow = new QHBoxLayout;
	outRow->addWidget(new QLabel(QString::fromLocal8Bit("Папка вывода:")));
	_outputDir = new QLineEdit;
	auto* pickDir = new QPushButton(QString::fromLocal8Bit("..."));
	outRow->addWidget(_outputDir, 1);
	outRow->addWidget(pickDir);
	main->addLayout(outRow);

	auto* actionRow = new QHBoxLayout;
	_btnSave    = new QPushButton(QString::fromLocal8Bit("Сохранить config..."));
	_btnLoad    = new QPushButton(QString::fromLocal8Bit("Загрузить config..."));
	_btnPreview = new QPushButton(QString::fromLocal8Bit("Показать план"));
	_btnStart   = new QPushButton(QString::fromLocal8Bit("Старт"));
	_btnCancel  = new QPushButton(QString::fromLocal8Bit("Отмена"));
	_btnCancel->setEnabled(false);
	actionRow->addWidget(_btnSave);
	actionRow->addWidget(_btnLoad);
	actionRow->addWidget(_btnPreview);
	actionRow->addStretch(1);
	actionRow->addWidget(_btnStart);
	actionRow->addWidget(_btnCancel);
	main->addLayout(actionRow);

	_progress = new QProgressBar;
	main->addWidget(_progress);

	_log = new QPlainTextEdit;
	_log->setReadOnly(true);
	_log->setMaximumBlockCount(2000);
	main->addWidget(_log, 1);

	connect(addF,  &QPushButton::clicked, this, &BenchmarkWidget::onAddFtp);
	connect(editF, &QPushButton::clicked, this, &BenchmarkWidget::onEditFtp);
	connect(remF,  &QPushButton::clicked, this, &BenchmarkWidget::onRemoveFtp);
	connect(addC,  &QPushButton::clicked, this, &BenchmarkWidget::onAddController);
	connect(editC, &QPushButton::clicked, this, &BenchmarkWidget::onEditController);
	connect(remC,  &QPushButton::clicked, this, &BenchmarkWidget::onRemoveController);
	connect(pickDir,     &QPushButton::clicked, this, &BenchmarkWidget::onPickOutputDir);
	connect(_btnSave,    &QPushButton::clicked, this, &BenchmarkWidget::onSave);
	connect(_btnLoad,    &QPushButton::clicked, this, &BenchmarkWidget::onLoad);
	connect(_btnStart,   &QPushButton::clicked, this, &BenchmarkWidget::onStart);
	connect(_btnCancel,  &QPushButton::clicked, this, &BenchmarkWidget::onCancel);
	connect(_btnPreview, &QPushButton::clicked, this, &BenchmarkWidget::onPreviewPlan);
}

BenchConfig BenchmarkWidget::collectConfig() const
{
	BenchConfig c;
	c.base.numStations      = _numStations->value();
	c.base.duration         = _duration->value();
	c.base.messagesNeeded   = _messagesNeeded->value();
	c.base.slotDuration     = _slotDuration->value();
	c.base.slotCountInFrame = _slotCountInFrame->value();
	c.base.bitsPerSlot      = _bitsPerSlot->value();
	c.base.hubTts           = _hubTts->value();
	c.base.stationTts       = _stationTts->value();
	c.base.seed             = static_cast<quint64>(_seed->value());

	c.base.estimatorKind = (_estimatorKind->currentIndex() == 1)
		? BenchEstimatorKind::Kalman
		: BenchEstimatorKind::Ema;
	c.base.emaConfig.alphaG          = _emaAlphaG->value();
	c.base.emaConfig.alphaPlr        = _emaAlphaPlr->value();
	c.base.emaConfig.collisionWeight = _emaCollisionWeight->value();
	c.base.kalmanConfig.qG              = _kalmanQG->value();
	c.base.kalmanConfig.qPlr            = _kalmanQPlr->value();
	c.base.kalmanConfig.rBase           = _kalmanRBase->value();
	c.base.kalmanConfig.collisionWeight = _kalmanCollisionWeight->value();

	switch (_predictorKind->currentIndex())
	{
	case 1: c.base.predictorKind = BenchPredictorKind::Cogorthy; break;
	case 2: c.base.predictorKind = BenchPredictorKind::BackoffAwareCogorthy; break;
	case 3: c.base.predictorKind = BenchPredictorKind::GreyModel; break;
	default: c.base.predictorKind = BenchPredictorKind::LinearRegression; break;
	}
	c.base.linearRegressionConfig.regressionWindow     = static_cast<std::uint32_t>(_linRegRegressionWindow->value());
	c.base.linearRegressionConfig.forgettingHorizonSec = _linRegForgettingHorizon->value();
	c.base.linearRegressionConfig.minProbability       = _linRegMinProb->value();
	c.base.linearRegressionConfig.maxProbability       = _linRegMaxProb->value();
	c.base.linearRegressionConfig.epsilon              = _linRegEpsilon->value();
	c.base.greyModelConfig.windowSize     = static_cast<std::uint32_t>(_greyWindowSize->value());
	c.base.greyModelConfig.minHistory     = static_cast<std::uint32_t>(_greyMinHistory->value());
	c.base.greyModelConfig.minProbability = _greyMinProb->value();
	c.base.greyModelConfig.maxProbability = _greyMaxProb->value();
	c.base.greyModelConfig.epsilon        = _greyEpsilon->value();

	c.ftpGenerators = _ftpConfigs;
	c.controllers   = _controllerConfigs;
	c.scenarios     = _scenarios;
	if (_currentScenario >= 0 && _currentScenario < static_cast<int>(c.scenarios.size()))
		c.scenarios[_currentScenario].events = _scenarioBuilder->events();
	c.outputDir     = _outputDir->text();
	c.threadCount   = _threadCount->value();
	return c;
}

void BenchmarkWidget::applyConfig(const BenchConfig& c)
{
	_numStations->setValue(c.base.numStations);
	_duration->setValue(c.base.duration);
	_messagesNeeded->setValue(c.base.messagesNeeded);
	_slotDuration->setValue(static_cast<int>(c.base.slotDuration));
	_slotCountInFrame->setValue(c.base.slotCountInFrame);
	_bitsPerSlot->setValue(c.base.bitsPerSlot);
	_hubTts->setValue(static_cast<int>(c.base.hubTts));
	_stationTts->setValue(static_cast<int>(c.base.stationTts));
	_seed->setValue(static_cast<int>(c.base.seed));

	_estimatorKind->setCurrentIndex(c.base.estimatorKind == BenchEstimatorKind::Kalman ? 1 : 0);
	_emaAlphaG->setValue(c.base.emaConfig.alphaG);
	_emaAlphaPlr->setValue(c.base.emaConfig.alphaPlr);
	_emaCollisionWeight->setValue(c.base.emaConfig.collisionWeight);
	_kalmanQG->setValue(c.base.kalmanConfig.qG);
	_kalmanQPlr->setValue(c.base.kalmanConfig.qPlr);
	_kalmanRBase->setValue(c.base.kalmanConfig.rBase);
	_kalmanCollisionWeight->setValue(c.base.kalmanConfig.collisionWeight);

	int predIdx = 0;
	switch (c.base.predictorKind)
	{
	case BenchPredictorKind::Cogorthy: predIdx = 1; break;
	case BenchPredictorKind::BackoffAwareCogorthy: predIdx = 2; break;
	case BenchPredictorKind::GreyModel: predIdx = 3; break;
	default: predIdx = 0; break;
	}
	_predictorKind->setCurrentIndex(predIdx);
	_linRegRegressionWindow->setValue(static_cast<int>(c.base.linearRegressionConfig.regressionWindow));
	_linRegForgettingHorizon->setValue(c.base.linearRegressionConfig.forgettingHorizonSec);
	_linRegMinProb->setValue(c.base.linearRegressionConfig.minProbability);
	_linRegMaxProb->setValue(c.base.linearRegressionConfig.maxProbability);
	_linRegEpsilon->setValue(c.base.linearRegressionConfig.epsilon);
	_greyWindowSize->setValue(static_cast<int>(c.base.greyModelConfig.windowSize));
	_greyMinHistory->setValue(static_cast<int>(c.base.greyModelConfig.minHistory));
	_greyMinProb->setValue(c.base.greyModelConfig.minProbability);
	_greyMaxProb->setValue(c.base.greyModelConfig.maxProbability);
	_greyEpsilon->setValue(c.base.greyModelConfig.epsilon);

	_ftpConfigs = c.ftpGenerators;
	_controllerConfigs = c.controllers;
	_scenarios = c.scenarios;
	_currentScenario = -1;
	rebuildScenarioList();
	if (_scenarios.empty())
		onAddScenario();
	else
		_scenarioList->setCurrentRow(0);
	_outputDir->setText(c.outputDir);
	_threadCount->setValue(c.threadCount);

	rebuildFtpTable();
	rebuildControllerTable();
}

void BenchmarkWidget::rebuildFtpTable()
{
	_ftpTable->setRowCount(static_cast<int>(_ftpConfigs.size()));
	for (int i = 0; i < static_cast<int>(_ftpConfigs.size()); ++i)
	{
		const auto* spec = ParamRegistry::findFtpGenerator(_ftpConfigs[i].kind);
		const QString label = spec ? spec->label : _ftpConfigs[i].kind;
		_ftpTable->setItem(i, 0, new QTableWidgetItem(label));
		_ftpTable->setItem(i, 1, new QTableWidgetItem(summarizeImpl(_ftpConfigs[i])));
	}
}

void BenchmarkWidget::rebuildControllerTable()
{
	_controllerTable->setRowCount(static_cast<int>(_controllerConfigs.size()));
	for (int i = 0; i < static_cast<int>(_controllerConfigs.size()); ++i)
	{
		const auto* spec = ParamRegistry::findController(_controllerConfigs[i].kind);
		const QString label = spec ? spec->label : _controllerConfigs[i].kind;
		_controllerTable->setItem(i, 0, new QTableWidgetItem(label));
		_controllerTable->setItem(i, 1, new QTableWidgetItem(summarizeImpl(_controllerConfigs[i])));
	}
}

void BenchmarkWidget::onAddFtp()
{
	BenchImplDialog dlg(BenchImplDialog::Catalog::FtpGenerator, this);
	if (dlg.exec() != QDialog::Accepted) return;
	_ftpConfigs.push_back(dlg.config());
	rebuildFtpTable();
}

void BenchmarkWidget::onEditFtp()
{
	const int row = _ftpTable->currentRow();
	if (row < 0 || row >= static_cast<int>(_ftpConfigs.size())) return;
	BenchImplDialog dlg(BenchImplDialog::Catalog::FtpGenerator, this);
	dlg.setConfig(_ftpConfigs[row]);
	if (dlg.exec() != QDialog::Accepted) return;
	_ftpConfigs[row] = dlg.config();
	rebuildFtpTable();
}

void BenchmarkWidget::onRemoveFtp()
{
	const int row = _ftpTable->currentRow();
	if (row < 0 || row >= static_cast<int>(_ftpConfigs.size())) return;
	_ftpConfigs.erase(_ftpConfigs.begin() + row);
	rebuildFtpTable();
}

void BenchmarkWidget::onAddController()
{
	BenchImplDialog dlg(BenchImplDialog::Catalog::Controller, this);
	if (dlg.exec() != QDialog::Accepted) return;
	_controllerConfigs.push_back(dlg.config());
	rebuildControllerTable();
}

void BenchmarkWidget::onEditController()
{
	const int row = _controllerTable->currentRow();
	if (row < 0 || row >= static_cast<int>(_controllerConfigs.size())) return;
	BenchImplDialog dlg(BenchImplDialog::Catalog::Controller, this);
	dlg.setConfig(_controllerConfigs[row]);
	if (dlg.exec() != QDialog::Accepted) return;
	_controllerConfigs[row] = dlg.config();
	rebuildControllerTable();
}

void BenchmarkWidget::onRemoveController()
{
	const int row = _controllerTable->currentRow();
	if (row < 0 || row >= static_cast<int>(_controllerConfigs.size())) return;
	_controllerConfigs.erase(_controllerConfigs.begin() + row);
	rebuildControllerTable();
}

void BenchmarkWidget::onAddScenario()
{
	commitCurrentScenario();
	NamedScenario ns;
	ns.name = QString::fromLocal8Bit("Сценарий %1").arg(_scenarios.size() + 1);
	_scenarios.push_back(std::move(ns));
	rebuildScenarioList();
	_scenarioList->setCurrentRow(static_cast<int>(_scenarios.size()) - 1);
}

void BenchmarkWidget::onLoadScenarios()
{
	const QStringList paths = QFileDialog::getOpenFileNames(this,
		QString::fromLocal8Bit("Загрузить сценарии"),
		QString(),
		QString::fromLocal8Bit("JSON (*.json)"));
	if (paths.isEmpty()) return;

	commitCurrentScenario();

	QStringList failed;
	for (const QString& path : paths)
	{
		QFile f(path);
		if (!f.open(QIODevice::ReadOnly))
		{
			failed << QFileInfo(path).fileName();
			continue;
		}
		QJsonParseError pe;
		const auto doc = QJsonDocument::fromJson(f.readAll(), &pe);
		if (pe.error != QJsonParseError::NoError)
		{
			failed << QFileInfo(path).fileName();
			continue;
		}
		QString err;
		auto events = ScenarioJson::fromJson(doc, &err);
		if (!err.isEmpty())
		{
			failed << QFileInfo(path).fileName();
			continue;
		}
		NamedScenario ns;
		ns.name   = QFileInfo(path).completeBaseName();
		ns.events = std::move(events);
		_scenarios.push_back(std::move(ns));
	}

	rebuildScenarioList();
	if (!_scenarios.empty())
		_scenarioList->setCurrentRow(static_cast<int>(_scenarios.size()) - 1);

	if (!failed.isEmpty())
		QMessageBox::warning(this,
			QString::fromLocal8Bit("Ошибка загрузки"),
			QString::fromLocal8Bit("Не удалось загрузить: ") + failed.join(", "));
}

void BenchmarkWidget::onRemoveScenario()
{
	const int row = _scenarioList->currentRow();
	if (row < 0 || row >= static_cast<int>(_scenarios.size())) return;
	_scenarios.erase(_scenarios.begin() + row);
	_currentScenario = -1;
	rebuildScenarioList();
	if (!_scenarios.empty())
		_scenarioList->setCurrentRow(std::min(row, static_cast<int>(_scenarios.size()) - 1));
	else
		_scenarioBuilder->setEvents({});
}

void BenchmarkWidget::onScenarioSelectionChanged()
{
	const int row = _scenarioList->currentRow();
	if (row == _currentScenario) return;
	commitCurrentScenario();
	_currentScenario = row;
	if (row >= 0 && row < static_cast<int>(_scenarios.size()))
		_scenarioBuilder->setEvents(_scenarios[row].events);
	else
		_scenarioBuilder->setEvents({});
}

void BenchmarkWidget::commitCurrentScenario()
{
	if (_currentScenario >= 0 && _currentScenario < static_cast<int>(_scenarios.size()))
		_scenarios[_currentScenario].events = _scenarioBuilder->events();
}

void BenchmarkWidget::rebuildScenarioList()
{
	const QSignalBlocker blocker(_scenarioList);
	_scenarioList->clear();
	for (const auto& s : _scenarios)
		_scenarioList->addItem(QString::fromLocal8Bit("%1 (%2 событий)")
			.arg(s.name).arg(static_cast<int>(s.events.size())));
}

void BenchmarkWidget::onPickOutputDir()
{
	const QString dir = QFileDialog::getExistingDirectory(this,
		QString::fromLocal8Bit("Папка для CSV"), _outputDir->text());
	if (!dir.isEmpty()) _outputDir->setText(dir);
}

void BenchmarkWidget::onSave()
{
	const QString path = QFileDialog::getSaveFileName(this,
		QString::fromLocal8Bit("Сохранить bench-конфиг"),
		QString(),
		QString::fromLocal8Bit("JSON (*.json)"));
	if (path.isEmpty()) return;
	QFile f(path);
	if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QMessageBox::warning(this,
			QString::fromLocal8Bit("Ошибка"),
			QString::fromLocal8Bit("Не удалось открыть файл для записи."));
		return;
	}
	f.write(BenchJson::toJson(collectConfig()).toJson(QJsonDocument::Indented));
}

void BenchmarkWidget::onLoad()
{
	const QString path = QFileDialog::getOpenFileName(this,
		QString::fromLocal8Bit("Загрузить bench-конфиг"),
		QString(),
		QString::fromLocal8Bit("JSON (*.json)"));
	if (path.isEmpty()) return;
	QFile f(path);
	if (!f.open(QIODevice::ReadOnly))
	{
		QMessageBox::warning(this,
			QString::fromLocal8Bit("Ошибка"),
			QString::fromLocal8Bit("Не удалось открыть файл."));
		return;
	}
	QJsonParseError pe;
	const auto doc = QJsonDocument::fromJson(f.readAll(), &pe);
	if (pe.error != QJsonParseError::NoError)
	{
		QMessageBox::warning(this,
			QString::fromLocal8Bit("Ошибка JSON"), pe.errorString());
		return;
	}
	QString err;
	auto cfg = BenchJson::fromJson(doc, &err);
	if (!err.isEmpty())
	{
		QMessageBox::warning(this,
			QString::fromLocal8Bit("Ошибка структуры"), err);
		return;
	}
	applyConfig(cfg);
}

void BenchmarkWidget::onPreviewPlan()
{
	const auto cfg = collectConfig();
	auto plan = BenchPlan::expand(cfg);
	log(QString::fromLocal8Bit("План: %1 запусков").arg(plan.size()));
	const int show = std::min<int>(5, static_cast<int>(plan.size()));
	for (int i = 0; i < show; ++i)
	{
		QString s = QString("  [%1] ftp=%2 controller=%3").arg(plan[i].index).arg(plan[i].ftpKind).arg(plan[i].controllerKind);
		for (auto it = plan[i].sweptParams.constBegin(); it != plan[i].sweptParams.constEnd(); ++it)
			s += QString(" %1=%2").arg(it.key()).arg(it.value().toString());
		log(s);
	}
}

void BenchmarkWidget::onStart()
{
	if (_executor && _executor->running()) return;

	const auto cfg = collectConfig();
	if (cfg.outputDir.isEmpty())
	{
		QMessageBox::warning(this,
			QString::fromLocal8Bit("Папка вывода"),
			QString::fromLocal8Bit("Укажите папку вывода."));
		return;
	}
	if (cfg.ftpGenerators.empty() || cfg.controllers.empty())
	{
		QMessageBox::warning(this,
			QString::fromLocal8Bit("Конфиг неполный"),
			QString::fromLocal8Bit("Нужна хотя бы одна реализация FTP и одна реализация контроллера."));
		return;
	}

	auto plan = BenchPlan::expand(cfg);
	if (plan.empty())
	{
		QMessageBox::warning(this,
			QString::fromLocal8Bit("Пустой план"),
			QString::fromLocal8Bit("План пустой."));
		return;
	}

	_progress->setRange(0, static_cast<int>(plan.size()));
	_progress->setValue(0);
	log(QString::fromLocal8Bit("Запуск: %1 прогонов").arg(plan.size()));

	_executor = std::make_unique<BenchExecutor>(this);
	connect(_executor.get(), &BenchExecutor::runStarted, this,
		[this](int idx) { log(QString::fromLocal8Bit("> старт #%1").arg(idx)); });
	connect(_executor.get(), &BenchExecutor::runFinished, this,
		[this](int idx, bool ok, const QString& csv, const QString& err) {
			if (ok) log(QString::fromLocal8Bit("? #%1 > %2").arg(idx).arg(csv));
			else    log(QString::fromLocal8Bit("? #%1: %2").arg(idx).arg(err));
		});
	connect(_executor.get(), &BenchExecutor::progress, this,
		[this](int done, int total) { _progress->setValue(done); _progress->setMaximum(total); });
	connect(_executor.get(), &BenchExecutor::finished, this, [this]() {
		log(QString::fromLocal8Bit("Готово."));
		_btnStart->setEnabled(true);
		_btnCancel->setEnabled(false);
	});

	_btnStart->setEnabled(false);
	_btnCancel->setEnabled(true);
	_executor->start(cfg, std::move(plan));
}

void BenchmarkWidget::onCancel()
{
	if (_executor) _executor->cancel();
	log(QString::fromLocal8Bit("Отмена…"));
}

void BenchmarkWidget::log(const QString& s)
{
	_log->appendPlainText(s);
}