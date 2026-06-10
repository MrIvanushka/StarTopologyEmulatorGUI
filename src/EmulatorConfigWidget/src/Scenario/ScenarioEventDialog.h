#pragma once

#include <QComboBox>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QStackedWidget>

#include "ScenarioEventData.h"

class ScenarioEventDialog : public QDialog
{
	Q_OBJECT
public:
	explicit ScenarioEventDialog(QWidget* parent = nullptr);

	void setData(const ScenarioEventData&);
	ScenarioEventData data() const;

private slots:
	void onKindChanged(int);
	void onProfileChanged(int);

private:
	void setupUi();

	QSpinBox*  _atSpin;
	QSpinBox*  _minIdSpin;
	QSpinBox*  _maxIdSpin;
	QComboBox* _kindCombo;

	QStackedWidget* _profileStack;
	QComboBox*      _profileCombo;

	QDoubleSpinBox* _cbrBitsPerTs;

	QDoubleSpinBox* _poiPacketsPerTs;
	QSpinBox*       _poiBitsPerPacket;
	QSpinBox*       _poiSeed;

	QDoubleSpinBox* _burPacketsPerTsOn;
	QSpinBox*       _burBitsPerPacket;
	QDoubleSpinBox* _burMeanOn;
	QDoubleSpinBox* _burMeanOff;
	QSpinBox*       _burSeed;

	QDoubleSpinBox* _parPacketsPerTsOn;
	QSpinBox*       _parBitsPerPacket;
	QDoubleSpinBox* _parMinOn;
	QDoubleSpinBox* _parMinOff;
	QDoubleSpinBox* _parAlpha;
	QSpinBox*       _parSeed;

	QWidget* _profileGroup;
};
