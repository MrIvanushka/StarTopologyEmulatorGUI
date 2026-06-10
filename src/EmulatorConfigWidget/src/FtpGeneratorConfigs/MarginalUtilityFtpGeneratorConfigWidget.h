#pragma once

#include <QComboBox>
#include <QStackedWidget>
#include <QWidget>

#include <StarTopologyEmulator/StarHubStrategy/FtpGenerator/MarginalUtilityFtpGeneratorConfig.h>

class F1LinearUtilityConfigWidget;
class F2LogarithmicUtilityConfigWidget;
class F3AlphaFairUtilityConfigWidget;
class F4SigmoidalUtilityConfigWidget;
class F5HardDeadlineUtilityConfigWidget;
class F6CostOfDelayUtilityConfigWidget;
class F7QuadraticBacklogUtilityConfigWidget;
class F8CesUtilityConfigWidget;

class MarginalUtilityFtpGeneratorConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit MarginalUtilityFtpGeneratorConfigWidget(QWidget* parent = nullptr);

	starTopologyEmulator::MarginalUtilityFtpGeneratorConfig getConfig() const;

private:
	void setupUi();

	QComboBox* _familySelector;
	QStackedWidget* _stack;

	F1LinearUtilityConfigWidget*           _f1;
	F2LogarithmicUtilityConfigWidget*      _f2;
	F3AlphaFairUtilityConfigWidget*        _f3;
	F4SigmoidalUtilityConfigWidget*        _f4;
	F5HardDeadlineUtilityConfigWidget*     _f5;
	F6CostOfDelayUtilityConfigWidget*      _f6;
	F7QuadraticBacklogUtilityConfigWidget* _f7;
	F8CesUtilityConfigWidget*              _f8;
};

class F1LinearUtilityConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit F1LinearUtilityConfigWidget(QWidget* parent = nullptr);
	starTopologyEmulator::F1LinearUtilityConfig getConfig() const;
private:
	void setupUi();
	class QDoubleSpinBox* _wAcq;
	class QDoubleSpinBox* _wAuth;
	class QDoubleSpinBox* _wColl;
	class QDoubleSpinBox* _wB;
	class QDoubleSpinBox* _wD;
	class QSpinBox*       _raMin;
	class QSpinBox*       _raMax;
	class QSpinBox*       _yellowSlots;
};

class F2LogarithmicUtilityConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit F2LogarithmicUtilityConfigWidget(QWidget* parent = nullptr);
	starTopologyEmulator::F2LogarithmicUtilityConfig getConfig() const;
private:
	void setupUi();
	class QDoubleSpinBox* _wAcq;
	class QDoubleSpinBox* _wAuth;
	class QDoubleSpinBox* _wColl;
	class QDoubleSpinBox* _wB;
	class QDoubleSpinBox* _wD;
	class QSpinBox*       _raMin;
	class QSpinBox*       _raMax;
	class QSpinBox*       _yellowSlots;
};

class F3AlphaFairUtilityConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit F3AlphaFairUtilityConfigWidget(QWidget* parent = nullptr);
	starTopologyEmulator::F3AlphaFairUtilityConfig getConfig() const;
private:
	void setupUi();
	class QDoubleSpinBox* _wAcq;
	class QDoubleSpinBox* _wAuth;
	class QDoubleSpinBox* _wColl;
	class QDoubleSpinBox* _wB;
	class QDoubleSpinBox* _wD;
	class QDoubleSpinBox* _alphaFair;
	class QDoubleSpinBox* _epsilon;
	class QSpinBox*       _raMin;
	class QSpinBox*       _raMax;
	class QSpinBox*       _yellowSlots;
};

class F4SigmoidalUtilityConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit F4SigmoidalUtilityConfigWidget(QWidget* parent = nullptr);
	starTopologyEmulator::F4SigmoidalUtilityConfig getConfig() const;
private:
	void setupUi();
	class QDoubleSpinBox* _wAcq;
	class QDoubleSpinBox* _wAuth;
	class QDoubleSpinBox* _wColl;
	class QDoubleSpinBox* _wB;
	class QDoubleSpinBox* _wD;
	class QDoubleSpinBox* _k;
	class QDoubleSpinBox* _x0;
	class QSpinBox*       _raMin;
	class QSpinBox*       _raMax;
	class QSpinBox*       _yellowSlots;
};

class F5HardDeadlineUtilityConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit F5HardDeadlineUtilityConfigWidget(QWidget* parent = nullptr);
	starTopologyEmulator::F5HardDeadlineUtilityConfig getConfig() const;
private:
	void setupUi();
	class QDoubleSpinBox* _wAcq;
	class QDoubleSpinBox* _wColl;
	class QDoubleSpinBox* _wB;
	class QDoubleSpinBox* _B;
	class QDoubleSpinBox* _Dtar;
	class QSpinBox*       _raMin;
	class QSpinBox*       _raMax;
	class QSpinBox*       _yellowSlots;
};

class F6CostOfDelayUtilityConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit F6CostOfDelayUtilityConfigWidget(QWidget* parent = nullptr);
	starTopologyEmulator::F6CostOfDelayUtilityConfig getConfig() const;
private:
	void setupUi();
	class QDoubleSpinBox* _cD;
	class QDoubleSpinBox* _cJ;
	class QDoubleSpinBox* _d0;
	class QSpinBox*       _raMin;
	class QSpinBox*       _raMax;
	class QSpinBox*       _yellowSlots;
};

class F7QuadraticBacklogUtilityConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit F7QuadraticBacklogUtilityConfigWidget(QWidget* parent = nullptr);
	starTopologyEmulator::F7QuadraticBacklogUtilityConfig getConfig() const;
private:
	void setupUi();
	class QDoubleSpinBox* _wAcq;
	class QDoubleSpinBox* _wColl;
	class QDoubleSpinBox* _wB;
	class QDoubleSpinBox* _wS;
	class QSpinBox*       _raMin;
	class QSpinBox*       _raMax;
	class QSpinBox*       _yellowSlots;
};

class F8CesUtilityConfigWidget : public QWidget {
	Q_OBJECT
public:
	explicit F8CesUtilityConfigWidget(QWidget* parent = nullptr);
	starTopologyEmulator::F8CesUtilityConfig getConfig() const;
private:
	void setupUi();
	class QDoubleSpinBox* _rho;
	class QDoubleSpinBox* _wRa;
	class QDoubleSpinBox* _wDa;
	class QSpinBox*       _raMin;
	class QSpinBox*       _raMax;
	class QSpinBox*       _yellowSlots;
};
