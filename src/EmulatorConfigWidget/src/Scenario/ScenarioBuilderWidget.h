#pragma once

#include <vector>

#include <QPushButton>
#include <QTableWidget>
#include <QWidget>

#include "ScenarioEventData.h"

class ScenarioBuilderWidget : public QWidget
{
	Q_OBJECT
public:
	explicit ScenarioBuilderWidget(QWidget* parent = nullptr);

	const std::vector<ScenarioEventData>& events() const;
	void setEvents(std::vector<ScenarioEventData>);

private slots:
	void onAddClicked();
	void onEditClicked();
	void onRemoveClicked();
	void onClearClicked();
	void onSaveClicked();
	void onLoadClicked();

private:
	void setupUi();
	void rebuildTable();
	void sortByTime();
	QString summarizeProfile(const ScenarioProfileConfig&) const;
	QString eventLabel(const ScenarioEventData&) const;

	std::vector<ScenarioEventData> _events;

	QTableWidget* _table;
	QPushButton* _btnAdd;
	QPushButton* _btnEdit;
	QPushButton* _btnRemove;
	QPushButton* _btnClear;
	QPushButton* _btnSave;
	QPushButton* _btnLoad;
};
