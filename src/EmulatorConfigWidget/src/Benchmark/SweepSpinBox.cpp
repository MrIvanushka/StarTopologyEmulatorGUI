#include "SweepSpinBox.h"

#include <QHBoxLayout>
#include <QLabel>

namespace
{

ParamValue scalarFromDouble(const ParamSpec& spec, double v)
{
	switch (spec.type)
	{
	case ParamSpec::Type::Int:  return static_cast<int>(std::round(v));
	case ParamSpec::Type::Bool: return v != 0.0;
	case ParamSpec::Type::Double:
	default: return v;
	}
}

double doubleFromParam(const ParamValue& v, double def)
{
	return std::visit([def](const auto& x) -> double {
		using T = std::decay_t<decltype(x)>;
		if constexpr (std::is_same_v<T, double>) return x;
		else if constexpr (std::is_same_v<T, int>) return static_cast<double>(x);
		else if constexpr (std::is_same_v<T, bool>) return x ? 1.0 : 0.0;
		else return def;
	}, v);
}

}

SweepSpinBox::SweepSpinBox(const ParamSpec& spec, QWidget* parent)
	: QWidget(parent), _spec(spec)
{
	auto* layout = new QHBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);

	_vary  = new QCheckBox(QString::fromLocal8Bit("варьировать"), this);
	_vary->setEnabled(spec.sweepable);
	_stack = new QStackedWidget(this);

	_scalar = new QDoubleSpinBox;
	applySpec(_scalar);
	_scalar->setValue(spec.defaultValue.toDouble());

	auto* rangeWrap = new QWidget;
	auto* rangeLayout = new QHBoxLayout(rangeWrap);
	rangeLayout->setContentsMargins(0, 0, 0, 0);
	_min  = new QDoubleSpinBox;
	_max  = new QDoubleSpinBox;
	_step = new QDoubleSpinBox;
	applySpec(_min);
	applySpec(_max);
	applySpec(_step);
	_min->setValue(spec.defaultValue.toDouble());
	_max->setValue(spec.defaultValue.toDouble());
	_step->setValue(spec.step);
	rangeLayout->addWidget(new QLabel(QString::fromLocal8Bit("min")));
	rangeLayout->addWidget(_min);
	rangeLayout->addWidget(new QLabel(QString::fromLocal8Bit("max")));
	rangeLayout->addWidget(_max);
	rangeLayout->addWidget(new QLabel(QString::fromLocal8Bit("шаг")));
	rangeLayout->addWidget(_step);

	_stack->addWidget(_scalar);
	_stack->addWidget(rangeWrap);

	layout->addWidget(_stack, 1);
	layout->addWidget(_vary);

	connect(_vary, &QCheckBox::toggled, this, &SweepSpinBox::onVaryToggled);
	onVaryToggled(false);
}

void SweepSpinBox::applySpec(QDoubleSpinBox* sb) const
{
	sb->setRange(_spec.minBound, _spec.maxBound);
	sb->setSingleStep(_spec.step);
	sb->setDecimals(_spec.type == ParamSpec::Type::Int ? 0 : _spec.decimals);
}

void SweepSpinBox::onVaryToggled(bool on)
{
	_stack->setCurrentIndex(on ? 1 : 0);
}

ParamValue SweepSpinBox::value() const
{
	if (_vary->isChecked() && _spec.sweepable)
	{
		SweepRange r;
		r.min  = _min->value();
		r.max  = _max->value();
		r.step = _step->value();
		return r;
	}
	return scalarFromDouble(_spec, _scalar->value());
}

void SweepSpinBox::setValue(const ParamValue& v)
{
	if (std::holds_alternative<SweepRange>(v))
	{
		const auto& r = std::get<SweepRange>(v);
		_vary->setChecked(true);
		_min->setValue(r.min);
		_max->setValue(r.max);
		_step->setValue(r.step);
	}
	else
	{
		_vary->setChecked(false);
		_scalar->setValue(doubleFromParam(v, _spec.defaultValue.toDouble()));
	}
}