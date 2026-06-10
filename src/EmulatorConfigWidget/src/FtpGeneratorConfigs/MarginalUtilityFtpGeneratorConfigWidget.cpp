#include "MarginalUtilityFtpGeneratorConfigWidget.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QSpinBox>
#include <QStackedWidget>
#include <QVBoxLayout>

using namespace starTopologyEmulator;

namespace
{

void setupDouble(QDoubleSpinBox* sb, double min, double max, double step, int decimals)
{
	sb->setRange(min, max);
	sb->setSingleStep(step);
	sb->setDecimals(decimals);
}

QSpinBox* makeUint8Spin()
{
	auto* sb = new QSpinBox();
	sb->setRange(0, 255);
	return sb;
}

void addRaCommonRows(QFormLayout* form, QSpinBox* raMin, QSpinBox* raMax, QSpinBox* yellowSlots)
{
	form->addRow(QString::fromLocal8Bit("Минимум RA-слотов в кадре: "), raMin);
	form->addRow(QString::fromLocal8Bit("Максимум RA-слотов в кадре: "), raMax);
	form->addRow(QString::fromLocal8Bit("Жёлтых слотов в кадре: "), yellowSlots);
}

} // namespace

// =================== F1 ===================
F1LinearUtilityConfigWidget::F1LinearUtilityConfigWidget(QWidget* parent)
	: QWidget(parent) { setupUi(); }

F1LinearUtilityConfig F1LinearUtilityConfigWidget::getConfig() const
{
	F1LinearUtilityConfig cfg;
	cfg.wAcq = _wAcq->value();
	cfg.wAuth = _wAuth->value();
	cfg.wColl = _wColl->value();
	cfg.wB = _wB->value();
	cfg.wD = _wD->value();
	cfg.raMin = static_cast<std::uint8_t>(_raMin->value());
	cfg.raMax = static_cast<std::uint8_t>(_raMax->value());
	cfg.yellowSlots = static_cast<std::uint8_t>(_yellowSlots->value());
	return cfg;
}

void F1LinearUtilityConfigWidget::setupUi()
{
	auto* form = new QFormLayout(this);
	_wAcq = new QDoubleSpinBox; _wAuth = new QDoubleSpinBox;
	_wColl = new QDoubleSpinBox; _wB = new QDoubleSpinBox; _wD = new QDoubleSpinBox;
	for (auto* s : { _wAcq, _wAuth, _wColl, _wB, _wD }) setupDouble(s, 0.0, 1000.0, 0.1, 4);
	_raMin = makeUint8Spin(); _raMax = makeUint8Spin(); _yellowSlots = makeUint8Spin();
	F1LinearUtilityConfig d;
	_wAcq->setValue(d.wAcq); _wAuth->setValue(d.wAuth);
	_wColl->setValue(d.wColl); _wB->setValue(d.wB); _wD->setValue(d.wD);
	_raMin->setValue(d.raMin); _raMax->setValue(d.raMax); _yellowSlots->setValue(d.yellowSlots);

	form->addRow(QString::fromLocal8Bit("Вес acquisition (wAcq): "), _wAcq);
	form->addRow(QString::fromLocal8Bit("Вес авторизации (wAuth): "), _wAuth);
	form->addRow(QString::fromLocal8Bit("Вес коллизий (wColl): "), _wColl);
	form->addRow(QString::fromLocal8Bit("Вес скорости передачи DA (wB): "), _wB);
	form->addRow(QString::fromLocal8Bit("Вес задержки DA (wD): "), _wD);
	addRaCommonRows(form, _raMin, _raMax, _yellowSlots);
}

// =================== F2 ===================
F2LogarithmicUtilityConfigWidget::F2LogarithmicUtilityConfigWidget(QWidget* parent)
	: QWidget(parent) { setupUi(); }

F2LogarithmicUtilityConfig F2LogarithmicUtilityConfigWidget::getConfig() const
{
	F2LogarithmicUtilityConfig cfg;
	cfg.wAcq = _wAcq->value();
	cfg.wAuth = _wAuth->value();
	cfg.wColl = _wColl->value();
	cfg.wB = _wB->value();
	cfg.wD = _wD->value();
	cfg.raMin = static_cast<std::uint8_t>(_raMin->value());
	cfg.raMax = static_cast<std::uint8_t>(_raMax->value());
	cfg.yellowSlots = static_cast<std::uint8_t>(_yellowSlots->value());
	return cfg;
}

void F2LogarithmicUtilityConfigWidget::setupUi()
{
	auto* form = new QFormLayout(this);
	_wAcq = new QDoubleSpinBox; _wAuth = new QDoubleSpinBox;
	_wColl = new QDoubleSpinBox; _wB = new QDoubleSpinBox; _wD = new QDoubleSpinBox;
	for (auto* s : { _wAcq, _wAuth, _wColl, _wB, _wD }) setupDouble(s, 0.0, 1000.0, 0.1, 4);
	_raMin = makeUint8Spin(); _raMax = makeUint8Spin(); _yellowSlots = makeUint8Spin();
	F2LogarithmicUtilityConfig d;
	_wAcq->setValue(d.wAcq); _wAuth->setValue(d.wAuth);
	_wColl->setValue(d.wColl); _wB->setValue(d.wB); _wD->setValue(d.wD);
	_raMin->setValue(d.raMin); _raMax->setValue(d.raMax); _yellowSlots->setValue(d.yellowSlots);

	form->addRow(QString::fromLocal8Bit("Вес acquisition (wAcq): "), _wAcq);
	form->addRow(QString::fromLocal8Bit("Вес авторизации (wAuth): "), _wAuth);
	form->addRow(QString::fromLocal8Bit("Вес коллизий (wColl): "), _wColl);
	form->addRow(QString::fromLocal8Bit("Вес скорости передачи DA (wB): "), _wB);
	form->addRow(QString::fromLocal8Bit("Вес задержки DA (wD): "), _wD);
	addRaCommonRows(form, _raMin, _raMax, _yellowSlots);
}

// =================== F3 ===================
F3AlphaFairUtilityConfigWidget::F3AlphaFairUtilityConfigWidget(QWidget* parent)
	: QWidget(parent) { setupUi(); }

F3AlphaFairUtilityConfig F3AlphaFairUtilityConfigWidget::getConfig() const
{
	F3AlphaFairUtilityConfig cfg;
	cfg.wAcq = _wAcq->value();
	cfg.wAuth = _wAuth->value();
	cfg.wColl = _wColl->value();
	cfg.wB = _wB->value();
	cfg.wD = _wD->value();
	cfg.alphaFair = _alphaFair->value();
	cfg.epsilon = _epsilon->value();
	cfg.raMin = static_cast<std::uint8_t>(_raMin->value());
	cfg.raMax = static_cast<std::uint8_t>(_raMax->value());
	cfg.yellowSlots = static_cast<std::uint8_t>(_yellowSlots->value());
	return cfg;
}

void F3AlphaFairUtilityConfigWidget::setupUi()
{
	auto* form = new QFormLayout(this);
	_wAcq = new QDoubleSpinBox; _wAuth = new QDoubleSpinBox;
	_wColl = new QDoubleSpinBox; _wB = new QDoubleSpinBox; _wD = new QDoubleSpinBox;
	_alphaFair = new QDoubleSpinBox; _epsilon = new QDoubleSpinBox;
	for (auto* s : { _wAcq, _wAuth, _wColl, _wB, _wD }) setupDouble(s, 0.0, 1000.0, 0.1, 4);
	setupDouble(_alphaFair, 0.0, 1000.0, 0.1, 4);
	setupDouble(_epsilon, 0.0, 1.0, 1e-6, 9);
	_raMin = makeUint8Spin(); _raMax = makeUint8Spin(); _yellowSlots = makeUint8Spin();
	F3AlphaFairUtilityConfig d;
	_wAcq->setValue(d.wAcq); _wAuth->setValue(d.wAuth);
	_wColl->setValue(d.wColl); _wB->setValue(d.wB); _wD->setValue(d.wD);
	_alphaFair->setValue(d.alphaFair); _epsilon->setValue(d.epsilon);
	_raMin->setValue(d.raMin); _raMax->setValue(d.raMax); _yellowSlots->setValue(d.yellowSlots);

	form->addRow(QString::fromLocal8Bit("Вес acquisition (wAcq): "), _wAcq);
	form->addRow(QString::fromLocal8Bit("Вес авторизации (wAuth): "), _wAuth);
	form->addRow(QString::fromLocal8Bit("Вес коллизий (wColl): "), _wColl);
	form->addRow(QString::fromLocal8Bit("Вес скорости передачи DA (wB): "), _wB);
	form->addRow(QString::fromLocal8Bit("Вес задержки DA (wD): "), _wD);
	form->addRow(QString::fromLocal8Bit("Параметр alpha (alphaFair): "), _alphaFair);
	form->addRow(QString::fromLocal8Bit("Регуляризатор (epsilon): "), _epsilon);
	addRaCommonRows(form, _raMin, _raMax, _yellowSlots);
}

// =================== F4 ===================
F4SigmoidalUtilityConfigWidget::F4SigmoidalUtilityConfigWidget(QWidget* parent)
	: QWidget(parent) { setupUi(); }

F4SigmoidalUtilityConfig F4SigmoidalUtilityConfigWidget::getConfig() const
{
	F4SigmoidalUtilityConfig cfg;
	cfg.wAcq = _wAcq->value();
	cfg.wAuth = _wAuth->value();
	cfg.wColl = _wColl->value();
	cfg.wB = _wB->value();
	cfg.wD = _wD->value();
	cfg.k = _k->value();
	cfg.x0 = _x0->value();
	cfg.raMin = static_cast<std::uint8_t>(_raMin->value());
	cfg.raMax = static_cast<std::uint8_t>(_raMax->value());
	cfg.yellowSlots = static_cast<std::uint8_t>(_yellowSlots->value());
	return cfg;
}

void F4SigmoidalUtilityConfigWidget::setupUi()
{
	auto* form = new QFormLayout(this);
	_wAcq = new QDoubleSpinBox; _wAuth = new QDoubleSpinBox;
	_wColl = new QDoubleSpinBox; _wB = new QDoubleSpinBox; _wD = new QDoubleSpinBox;
	_k = new QDoubleSpinBox; _x0 = new QDoubleSpinBox;
	for (auto* s : { _wAcq, _wAuth, _wColl, _wB, _wD, _k, _x0 }) setupDouble(s, -1000.0, 1000.0, 0.1, 4);
	_raMin = makeUint8Spin(); _raMax = makeUint8Spin(); _yellowSlots = makeUint8Spin();
	F4SigmoidalUtilityConfig d;
	_wAcq->setValue(d.wAcq); _wAuth->setValue(d.wAuth);
	_wColl->setValue(d.wColl); _wB->setValue(d.wB); _wD->setValue(d.wD);
	_k->setValue(d.k); _x0->setValue(d.x0);
	_raMin->setValue(d.raMin); _raMax->setValue(d.raMax); _yellowSlots->setValue(d.yellowSlots);

	form->addRow(QString::fromLocal8Bit("Вес acquisition (wAcq): "), _wAcq);
	form->addRow(QString::fromLocal8Bit("Вес авторизации (wAuth): "), _wAuth);
	form->addRow(QString::fromLocal8Bit("Вес коллизий (wColl): "), _wColl);
	form->addRow(QString::fromLocal8Bit("Вес скорости передачи DA (wB): "), _wB);
	form->addRow(QString::fromLocal8Bit("Вес задержки DA (wD): "), _wD);
	form->addRow(QString::fromLocal8Bit("Крутизна сигмоиды (k): "), _k);
	form->addRow(QString::fromLocal8Bit("Точка перегиба (x0): "), _x0);
	addRaCommonRows(form, _raMin, _raMax, _yellowSlots);
}

// =================== F5 ===================
F5HardDeadlineUtilityConfigWidget::F5HardDeadlineUtilityConfigWidget(QWidget* parent)
	: QWidget(parent) { setupUi(); }

F5HardDeadlineUtilityConfig F5HardDeadlineUtilityConfigWidget::getConfig() const
{
	F5HardDeadlineUtilityConfig cfg;
	cfg.wAcq = _wAcq->value();
	cfg.wColl = _wColl->value();
	cfg.wB = _wB->value();
	cfg.B = _B->value();
	cfg.Dtar = _Dtar->value();
	cfg.raMin = static_cast<std::uint8_t>(_raMin->value());
	cfg.raMax = static_cast<std::uint8_t>(_raMax->value());
	cfg.yellowSlots = static_cast<std::uint8_t>(_yellowSlots->value());
	return cfg;
}

void F5HardDeadlineUtilityConfigWidget::setupUi()
{
	auto* form = new QFormLayout(this);
	_wAcq = new QDoubleSpinBox; _wColl = new QDoubleSpinBox;
	_wB = new QDoubleSpinBox; _B = new QDoubleSpinBox; _Dtar = new QDoubleSpinBox;
	for (auto* s : { _wAcq, _wColl, _wB, _B, _Dtar }) setupDouble(s, 0.0, 10000.0, 0.1, 4);
	_raMin = makeUint8Spin(); _raMax = makeUint8Spin(); _yellowSlots = makeUint8Spin();
	F5HardDeadlineUtilityConfig d;
	_wAcq->setValue(d.wAcq); _wColl->setValue(d.wColl);
	_wB->setValue(d.wB); _B->setValue(d.B); _Dtar->setValue(d.Dtar);
	_raMin->setValue(d.raMin); _raMax->setValue(d.raMax); _yellowSlots->setValue(d.yellowSlots);

	form->addRow(QString::fromLocal8Bit("Вес acquisition (wAcq): "), _wAcq);
	form->addRow(QString::fromLocal8Bit("Вес коллизий (wColl): "), _wColl);
	form->addRow(QString::fromLocal8Bit("Вес ln(d) (wB): "), _wB);
	form->addRow(QString::fromLocal8Bit("Штраф за дедлайн (B): "), _B);
	form->addRow(QString::fromLocal8Bit("Целевая задержка (Dtar): "), _Dtar);
	addRaCommonRows(form, _raMin, _raMax, _yellowSlots);
}

// =================== F6 ===================
F6CostOfDelayUtilityConfigWidget::F6CostOfDelayUtilityConfigWidget(QWidget* parent)
	: QWidget(parent) { setupUi(); }

F6CostOfDelayUtilityConfig F6CostOfDelayUtilityConfigWidget::getConfig() const
{
	F6CostOfDelayUtilityConfig cfg;
	cfg.cD = _cD->value();
	cfg.cJ = _cJ->value();
	cfg.d0 = _d0->value();
	cfg.raMin = static_cast<std::uint8_t>(_raMin->value());
	cfg.raMax = static_cast<std::uint8_t>(_raMax->value());
	cfg.yellowSlots = static_cast<std::uint8_t>(_yellowSlots->value());
	return cfg;
}

void F6CostOfDelayUtilityConfigWidget::setupUi()
{
	auto* form = new QFormLayout(this);
	_cD = new QDoubleSpinBox; _cJ = new QDoubleSpinBox; _d0 = new QDoubleSpinBox;
	for (auto* s : { _cD, _cJ, _d0 }) setupDouble(s, 0.0, 10000.0, 0.1, 4);
	_raMin = makeUint8Spin(); _raMax = makeUint8Spin(); _yellowSlots = makeUint8Spin();
	F6CostOfDelayUtilityConfig d;
	_cD->setValue(d.cD); _cJ->setValue(d.cJ); _d0->setValue(d.d0);
	_raMin->setValue(d.raMin); _raMax->setValue(d.raMax); _yellowSlots->setValue(d.yellowSlots);

	form->addRow(QString::fromLocal8Bit("Удельная стоимость задержки DA (cD): "), _cD);
	form->addRow(QString::fromLocal8Bit("Удельная стоимость задержки RA (cJ): "), _cJ);
	form->addRow(QString::fromLocal8Bit("RTT-смещение (d0): "), _d0);
	addRaCommonRows(form, _raMin, _raMax, _yellowSlots);
}

// =================== F7 ===================
F7QuadraticBacklogUtilityConfigWidget::F7QuadraticBacklogUtilityConfigWidget(QWidget* parent)
	: QWidget(parent) { setupUi(); }

F7QuadraticBacklogUtilityConfig F7QuadraticBacklogUtilityConfigWidget::getConfig() const
{
	F7QuadraticBacklogUtilityConfig cfg;
	cfg.wAcq = _wAcq->value();
	cfg.wColl = _wColl->value();
	cfg.wB = _wB->value();
	cfg.wS = _wS->value();
	cfg.raMin = static_cast<std::uint8_t>(_raMin->value());
	cfg.raMax = static_cast<std::uint8_t>(_raMax->value());
	cfg.yellowSlots = static_cast<std::uint8_t>(_yellowSlots->value());
	return cfg;
}

void F7QuadraticBacklogUtilityConfigWidget::setupUi()
{
	auto* form = new QFormLayout(this);
	_wAcq = new QDoubleSpinBox; _wColl = new QDoubleSpinBox;
	_wB = new QDoubleSpinBox; _wS = new QDoubleSpinBox;
	for (auto* s : { _wAcq, _wColl, _wB, _wS }) setupDouble(s, 0.0, 10000.0, 0.1, 4);
	_raMin = makeUint8Spin(); _raMax = makeUint8Spin(); _yellowSlots = makeUint8Spin();
	F7QuadraticBacklogUtilityConfig d;
	_wAcq->setValue(d.wAcq); _wColl->setValue(d.wColl);
	_wB->setValue(d.wB); _wS->setValue(d.wS);
	_raMin->setValue(d.raMin); _raMax->setValue(d.raMax); _yellowSlots->setValue(d.yellowSlots);

	form->addRow(QString::fromLocal8Bit("Вес acquisition (wAcq): "), _wAcq);
	form->addRow(QString::fromLocal8Bit("Вес коллизий (wColl): "), _wColl);
	form->addRow(QString::fromLocal8Bit("Вес ln(1+d) (wB): "), _wB);
	form->addRow(QString::fromLocal8Bit("Вес штрафа за бэклог (wS): "), _wS);
	addRaCommonRows(form, _raMin, _raMax, _yellowSlots);
}

// =================== F8 ===================
F8CesUtilityConfigWidget::F8CesUtilityConfigWidget(QWidget* parent)
	: QWidget(parent) { setupUi(); }

F8CesUtilityConfig F8CesUtilityConfigWidget::getConfig() const
{
	F8CesUtilityConfig cfg;
	cfg.rho = _rho->value();
	cfg.wRa = _wRa->value();
	cfg.wDa = _wDa->value();
	cfg.raMin = static_cast<std::uint8_t>(_raMin->value());
	cfg.raMax = static_cast<std::uint8_t>(_raMax->value());
	cfg.yellowSlots = static_cast<std::uint8_t>(_yellowSlots->value());
	return cfg;
}

void F8CesUtilityConfigWidget::setupUi()
{
	auto* form = new QFormLayout(this);
	_rho = new QDoubleSpinBox; _wRa = new QDoubleSpinBox; _wDa = new QDoubleSpinBox;
	for (auto* s : { _rho, _wRa, _wDa }) setupDouble(s, -100.0, 100.0, 0.1, 4);
	_raMin = makeUint8Spin(); _raMax = makeUint8Spin(); _yellowSlots = makeUint8Spin();
	F8CesUtilityConfig d;
	_rho->setValue(d.rho); _wRa->setValue(d.wRa); _wDa->setValue(d.wDa);
	_raMin->setValue(d.raMin); _raMax->setValue(d.raMax); _yellowSlots->setValue(d.yellowSlots);

	form->addRow(QString::fromLocal8Bit("Параметр эластичности (rho): "), _rho);
	form->addRow(QString::fromLocal8Bit("Вес RA (wRa): "), _wRa);
	form->addRow(QString::fromLocal8Bit("Вес DA (wDa): "), _wDa);
	addRaCommonRows(form, _raMin, _raMax, _yellowSlots);
}

// =================== MarginalUtility ===================
MarginalUtilityFtpGeneratorConfigWidget::MarginalUtilityFtpGeneratorConfigWidget(QWidget* parent)
	: QWidget(parent)
{
	setupUi();
}

MarginalUtilityFtpGeneratorConfig MarginalUtilityFtpGeneratorConfigWidget::getConfig() const
{
	switch (_stack->currentIndex())
	{
	case 0: return _f1->getConfig();
	case 1: return _f2->getConfig();
	case 2: return _f3->getConfig();
	case 3: return _f4->getConfig();
	case 4: return _f5->getConfig();
	case 5: return _f6->getConfig();
	case 6: return _f7->getConfig();
	case 7: return _f8->getConfig();
	default: return F1LinearUtilityConfig{};
	}
}

void MarginalUtilityFtpGeneratorConfigWidget::setupUi()
{
	auto* mainLayout = new QVBoxLayout(this);

	_familySelector = new QComboBox(this);
	_familySelector->addItem(QString::fromLocal8Bit("F1: линейная"));
	_familySelector->addItem(QString::fromLocal8Bit("F2: логарифмическая (Kelly)"));
	_familySelector->addItem(QString::fromLocal8Bit("F3: alpha-fair (Mo-Walrand)"));
	_familySelector->addItem(QString::fromLocal8Bit("F4: сигмоидальная"));
	_familySelector->addItem(QString::fromLocal8Bit("F5: жёсткий дедлайн"));
	_familySelector->addItem(QString::fromLocal8Bit("F6: cost-of-delay (c?-rule)"));
	_familySelector->addItem(QString::fromLocal8Bit("F7: квадратичный штраф за бэклог"));
	_familySelector->addItem(QString::fromLocal8Bit("F8: CES"));

	_stack = new QStackedWidget(this);
	_f1 = new F1LinearUtilityConfigWidget;
	_f2 = new F2LogarithmicUtilityConfigWidget;
	_f3 = new F3AlphaFairUtilityConfigWidget;
	_f4 = new F4SigmoidalUtilityConfigWidget;
	_f5 = new F5HardDeadlineUtilityConfigWidget;
	_f6 = new F6CostOfDelayUtilityConfigWidget;
	_f7 = new F7QuadraticBacklogUtilityConfigWidget;
	_f8 = new F8CesUtilityConfigWidget;
	_stack->addWidget(_f1);
	_stack->addWidget(_f2);
	_stack->addWidget(_f3);
	_stack->addWidget(_f4);
	_stack->addWidget(_f5);
	_stack->addWidget(_f6);
	_stack->addWidget(_f7);
	_stack->addWidget(_f8);

	connect(_familySelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
		_stack, &QStackedWidget::setCurrentIndex);

	mainLayout->addWidget(_familySelector);
	mainLayout->addWidget(_stack);
	mainLayout->addStretch();
}