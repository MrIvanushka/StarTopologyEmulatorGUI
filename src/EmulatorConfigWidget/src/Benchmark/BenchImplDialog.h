#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QHash>
#include <QList>
#include <QString>
#include <QStackedWidget>
#include <QWidget>

#include "BenchTypes.h"
#include "ParamRegistry.h"
#include "SweepSpinBox.h"

class BenchImplDialog : public QDialog
{
	Q_OBJECT
public:
	enum class Catalog { FtpGenerator, Controller };

	BenchImplDialog(Catalog catalog, QWidget* parent = nullptr);

	void setConfig(const ImplConfig&);
	ImplConfig config() const;

private slots:
	void onKindChanged(int);

private:
	struct PageWidgets
	{
		QHash<QString, SweepSpinBox*> sweeps;
		QHash<QString, QCheckBox*>    bools;
	};

	void buildPagesFromRegistry();
	QWidget* buildPage(const ImplSpec&, PageWidgets& out);

	Catalog              _catalog;
	QList<ImplSpec>      _impls;
	QComboBox*           _kindCombo;
	QStackedWidget*      _stack;
	QList<PageWidgets>   _pages;
};
