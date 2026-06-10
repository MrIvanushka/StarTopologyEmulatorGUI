#include "PiAntiWindupConfigWidget.h"

#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

using namespace starTopologyEmulator;

PiAntiWindupConfigWidget::PiAntiWindupConfigWidget(QWidget* parent)
	: QWidget(parent)
{
	setupUi();
}

PiAntiWindupConfig PiAntiWindupConfigWidget::getConfig() const
{
	switch (_stack->currentIndex())
	{
	case None:
		return NoneAntiWindupConfig{};
	case Clamping:
		return ClampingAntiWindupConfig{};
	case BackCalculation:
	{
		BackCalculationAntiWindupConfig cfg;
		cfg.kT = _kTSpin->value();
		return cfg;
	}
	case TrackingMode:
		return TrackingModeAntiWindupConfig{};
	}
	return NoneAntiWindupConfig{};
}

void PiAntiWindupConfigWidget::setConfig(const PiAntiWindupConfig& cfg)
{
	int idx = None;
	std::visit(
		[this, &idx](const auto& c) {
			using T = std::decay_t<decltype(c)>;
			if constexpr (std::is_same_v<T, NoneAntiWindupConfig>)
				idx = None;
			else if constexpr (std::is_same_v<T, ClampingAntiWindupConfig>)
				idx = Clamping;
			else if constexpr (std::is_same_v<T, BackCalculationAntiWindupConfig>) {
				idx = BackCalculation;
				_kTSpin->setValue(c.kT);
			}
			else if constexpr (std::is_same_v<T, TrackingModeAntiWindupConfig>)
				idx = TrackingMode;
		},
		cfg);
	_kindSelector->setCurrentIndex(idx);
}

void PiAntiWindupConfigWidget::setupUi()
{
	auto* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);

	_kindSelector = new QComboBox(this);
	_kindSelector->addItem(QString::fromLocal8Bit("Отключено"));
	_kindSelector->addItem(QString::fromLocal8Bit("Условное интегрирование (clamping)"));
	_kindSelector->addItem(QString::fromLocal8Bit("Back-calculation"));
	_kindSelector->addItem(QString::fromLocal8Bit("Tracking-mode"));

	_stack = new QStackedWidget(this);

	_stack->addWidget(new QWidget);

	{
		auto* page = new QWidget;
		auto* l = new QFormLayout(page);
		auto* hint = new QLabel(QString::fromLocal8Bit(
			"Интегратор замораживается, когда выход уперся в границу [pMin; pMax] "
			"и знак ошибки усугубляет насыщение."));
		hint->setWordWrap(true);
		l->addRow(hint);
		_stack->addWidget(page);
	}

	{
		auto* page = new QWidget;
		auto* l = new QFormLayout(page);
		_kTSpin = new QDoubleSpinBox(page);
		_kTSpin->setRange(0.0, 100.0);
		_kTSpin->setSingleStep(0.01);
		_kTSpin->setDecimals(4);
		_kTSpin->setValue(BackCalculationAntiWindupConfig{}.kT);
		l->addRow(QString::fromLocal8Bit("Коэффициент возврата (kT): "), _kTSpin);
		_stack->addWidget(page);
	}

	{
		auto* page = new QWidget;
		auto* l = new QFormLayout(page);
		auto* hint = new QLabel(QString::fromLocal8Bit(
			"На выходе из насыщения интегратор переинициализируется "
			"из обратной формулы (bumpless transfer)."));
		hint->setWordWrap(true);
		l->addRow(hint);
		_stack->addWidget(page);
	}

	layout->addWidget(_kindSelector);
	layout->addWidget(_stack);

	connect(_kindSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
		_stack, &QStackedWidget::setCurrentIndex);
}