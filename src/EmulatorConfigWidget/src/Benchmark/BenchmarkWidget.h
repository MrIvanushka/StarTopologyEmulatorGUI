#pragma once

#include <memory>

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QWidget>

#include "ModuleWidget.h"
#include "IContext.h"

#include "BenchExecutor.h"
#include "BenchTypes.h"
#include "../Scenario/ScenarioBuilderWidget.h"

class BenchmarkWidget : public editor::ModuleWidget
{
	Q_OBJECT
public:
	explicit BenchmarkWidget();

	void init(std::shared_ptr<editor::IContext> context) override;
	const editor::Info& info() const override;
private slots:
	void onAddFtp();
	void onEditFtp();
	void onRemoveFtp();
	void onAddController();
	void onEditController();
	void onRemoveController();
	void onAddScenario();
	void onLoadScenarios();
	void onRemoveScenario();
	void onScenarioSelectionChanged();
	void onPickOutputDir();
	void onSave();
	void onLoad();
	void onStart();
	void onCancel();
	void onPreviewPlan();

private:
	void setupUi();
	BenchConfig collectConfig() const;
	void applyConfig(const BenchConfig&);
	void rebuildFtpTable();
	void rebuildControllerTable();
	void rebuildScenarioList();
	void commitCurrentScenario();
	void log(const QString&);

	static const editor::Info _info;

	QSpinBox* _numStations;
	QSpinBox* _duration;
	QSpinBox* _messagesNeeded;
	QSpinBox* _slotDuration;
	QSpinBox* _slotCountInFrame;
	QSpinBox* _bitsPerSlot;
	QSpinBox* _hubTts;
	QSpinBox* _stationTts;
	QSpinBox* _seed;
	QSpinBox* _threadCount;

	QComboBox* _estimatorKind;
	QDoubleSpinBox* _emaAlphaG;
	QDoubleSpinBox* _emaAlphaPlr;
	QDoubleSpinBox* _emaCollisionWeight;
	QDoubleSpinBox* _kalmanQG;
	QDoubleSpinBox* _kalmanQPlr;
	QDoubleSpinBox* _kalmanRBase;
	QDoubleSpinBox* _kalmanCollisionWeight;

	QComboBox* _predictorKind;
	QSpinBox* _linRegRegressionWindow;
	QDoubleSpinBox* _linRegForgettingHorizon;
	QDoubleSpinBox* _linRegMinProb;
	QDoubleSpinBox* _linRegMaxProb;
	QDoubleSpinBox* _linRegEpsilon;

	QSpinBox*       _greyWindowSize;
	QSpinBox*       _greyMinHistory;
	QDoubleSpinBox* _greyMinProb;
	QDoubleSpinBox* _greyMaxProb;
	QDoubleSpinBox* _greyEpsilon;

	QTableWidget* _ftpTable;
	QTableWidget* _controllerTable;
	std::vector<ImplConfig> _ftpConfigs;
	std::vector<ImplConfig> _controllerConfigs;

	ScenarioBuilderWidget* _scenarioBuilder;
	QListWidget*           _scenarioList;
	std::vector<NamedScenario> _scenarios;
	int                    _currentScenario = -1;

	QLineEdit* _outputDir;
	QPushButton* _btnSave;
	QPushButton* _btnLoad;
	QPushButton* _btnStart;
	QPushButton* _btnCancel;
	QPushButton* _btnPreview;

	QProgressBar* _progress;
	QPlainTextEdit* _log;

	std::unique_ptr<BenchExecutor> _executor;
};
