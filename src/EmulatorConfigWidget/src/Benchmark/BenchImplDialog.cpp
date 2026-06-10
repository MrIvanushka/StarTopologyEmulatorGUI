#include "BenchImplDialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QVBoxLayout>

BenchImplDialog::BenchImplDialog(Catalog catalog, QWidget* parent)
	: QDialog(parent), _catalog(catalog)
{
	setWindowTitle(catalog == Catalog::FtpGenerator
		? QString::fromLocal8Bit("FTP-генератор")
		: QString::fromLocal8Bit("Контроллер нагрузки"));

	_impls = (catalog == Catalog::FtpGenerator)
		? ParamRegistry::ftpGenerators()
		: ParamRegistry::controllers();

	auto* mainLayout = new QVBoxLayout(this);

	_kindCombo = new QComboBox(this);
	for (const auto& s : _impls)
		_kindCombo->addItem(s.label, s.kind);
	mainLayout->addWidget(_kindCombo);

	_stack = new QStackedWidget(this);
	mainLayout->addWidget(_stack, 1);

	buildPagesFromRegistry();

	auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
	mainLayout->addWidget(buttons);

	connect(_kindCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
		this, &BenchImplDialog::onKindChanged);

	if (!_impls.isEmpty())
		_stack->setCurrentIndex(0);
}

void BenchImplDialog::buildPagesFromRegistry()
{
	_pages.clear();
	for (const auto& spec : _impls)
	{
		PageWidgets pw;
		QWidget* page = buildPage(spec, pw);
		_stack->addWidget(page);
		_pages.append(pw);
	}
}

QWidget* BenchImplDialog::buildPage(const ImplSpec& spec, PageWidgets& out)
{
	auto* page = new QWidget;
	auto* form = new QFormLayout(page);

	for (const auto& p : spec.params)
	{
		if (p.type == ParamSpec::Type::Bool)
		{
			auto* box = new QCheckBox;
			box->setChecked(p.defaultValue.toBool());
			form->addRow(p.label, box);
			out.bools.insert(p.key, box);
		}
		else
		{
			auto* sw = new SweepSpinBox(p);
			form->addRow(p.label, sw);
			out.sweeps.insert(p.key, sw);
		}
	}

	return page;
}

void BenchImplDialog::onKindChanged(int index)
{
	_stack->setCurrentIndex(index);
}

void BenchImplDialog::setConfig(const ImplConfig& cfg)
{
	int idx = -1;
	for (int i = 0; i < _impls.size(); ++i)
	{
		if (_impls[i].kind == cfg.kind) { idx = i; break; }
	}
	if (idx < 0)
		return;

	_kindCombo->setCurrentIndex(idx);
	_stack->setCurrentIndex(idx);

	const auto& pw = _pages[idx];

	for (auto it = cfg.params.constBegin(); it != cfg.params.constEnd(); ++it)
	{
		const QString& key = it.key();
		if (auto sw = pw.sweeps.value(key, nullptr))
		{
			sw->setValue(it.value());
		}
		else if (auto box = pw.bools.value(key, nullptr))
		{
			std::visit([&](const auto& v) {
				using T = std::decay_t<decltype(v)>;
				if constexpr (std::is_same_v<T, bool>) box->setChecked(v);
				else if constexpr (std::is_same_v<T, int>) box->setChecked(v != 0);
				else if constexpr (std::is_same_v<T, double>) box->setChecked(v != 0.0);
			}, it.value());
		}
	}
}

ImplConfig BenchImplDialog::config() const
{
	ImplConfig cfg;
	const int idx = _kindCombo->currentIndex();
	if (idx < 0 || idx >= _impls.size())
		return cfg;

	cfg.kind = _impls[idx].kind;
	const auto& pw = _pages[idx];

	for (auto it = pw.sweeps.constBegin(); it != pw.sweeps.constEnd(); ++it)
		cfg.params.insert(it.key(), it.value()->value());

	for (auto it = pw.bools.constBegin(); it != pw.bools.constEnd(); ++it)
		cfg.params.insert(it.key(), it.value()->isChecked());

	return cfg;
}