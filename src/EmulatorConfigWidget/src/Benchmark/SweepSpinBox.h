#pragma once

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QStackedWidget>
#include <QWidget>

#include "BenchTypes.h"
#include "ParamRegistry.h"

class SweepSpinBox : public QWidget
{
	Q_OBJECT
public:
	explicit SweepSpinBox(const ParamSpec& spec, QWidget* parent = nullptr);

	ParamValue value() const;
	void setValue(const ParamValue&);

private slots:
	void onVaryToggled(bool);

private:
	void applySpec(QDoubleSpinBox*) const;

	ParamSpec       _spec;
	QCheckBox*      _vary;
	QStackedWidget* _stack;
	QDoubleSpinBox* _scalar;
	QDoubleSpinBox* _min;
	QDoubleSpinBox* _max;
	QDoubleSpinBox* _step;
};
