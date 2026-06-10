#include "ScenarioEventDialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

using namespace starTopologyEmulator;

namespace
{

int kindToIndex(ScenarioEventKind k)
{
	switch (k)
	{
	case ScenarioEventKind::STATION_ENABLE:      return 0;
	case ScenarioEventKind::STATION_DISABLE:     return 1;
	case ScenarioEventKind::STATION_SET_PROFILE: return 2;
	default: return 0;
	}
}

ScenarioEventKind indexToKind(int idx)
{
	switch (idx)
	{
	case 0: return ScenarioEventKind::STATION_ENABLE;
	case 1: return ScenarioEventKind::STATION_DISABLE;
	case 2: return ScenarioEventKind::STATION_SET_PROFILE;
	default: return ScenarioEventKind::STATION_ENABLE;
	}
}

void setupDouble(QDoubleSpinBox* sb, double min, double max, double step, int decimals)
{
	sb->setRange(min, max);
	sb->setSingleStep(step);
	sb->setDecimals(decimals);
}

}

ScenarioEventDialog::ScenarioEventDialog(QWidget* parent)
	: QDialog(parent)
{
	setupUi();
	onKindChanged(_kindCombo->currentIndex());
}

void ScenarioEventDialog::setupUi()
{
	setWindowTitle(QString::fromLocal8Bit("Событие сценария"));

	auto* mainLayout = new QVBoxLayout(this);

	auto* commonForm = new QFormLayout();
	_atSpin = new QSpinBox(this);
	_atSpin->setRange(0, INT_MAX);
	_minIdSpin = new QSpinBox(this);
	_minIdSpin->setRange(0, INT_MAX);
	_maxIdSpin = new QSpinBox(this);
	_maxIdSpin->setRange(0, INT_MAX);

	_kindCombo = new QComboBox(this);
	_kindCombo->addItem(QString::fromLocal8Bit("Включить станции"));
	_kindCombo->addItem(QString::fromLocal8Bit("Выключить станции"));
	_kindCombo->addItem(QString::fromLocal8Bit("Сменить профиль трафика"));

	commonForm->addRow(QString::fromLocal8Bit("Тип события:"), _kindCombo);
	commonForm->addRow(QString::fromLocal8Bit("Время (Timestamp):"), _atSpin);
	commonForm->addRow(QString::fromLocal8Bit("ID станций — минимум:"), _minIdSpin);
	commonForm->addRow(QString::fromLocal8Bit("ID станций — максимум:"), _maxIdSpin);

	mainLayout->addLayout(commonForm);

	_profileGroup = new QGroupBox(QString::fromLocal8Bit("Параметры профиля трафика"), this);
	auto* profileLayout = new QVBoxLayout(_profileGroup);

	_profileCombo = new QComboBox(_profileGroup);
	_profileCombo->addItem(QString::fromLocal8Bit("CBR"));
	_profileCombo->addItem(QString::fromLocal8Bit("Пуассон"));
	_profileCombo->addItem(QString::fromLocal8Bit("Берст (экспоненциальный)"));
	_profileCombo->addItem(QString::fromLocal8Bit("Берст (Парето)"));

	_profileStack = new QStackedWidget(_profileGroup);

	{
		auto* page = new QWidget;
		auto* form = new QFormLayout(page);
		_cbrBitsPerTs = new QDoubleSpinBox;
		setupDouble(_cbrBitsPerTs, 0.0, 1e12, 1.0, 6);
		form->addRow(QString::fromLocal8Bit("Бит/ts:"), _cbrBitsPerTs);
		_profileStack->addWidget(page);
	}

	{
		auto* page = new QWidget;
		auto* form = new QFormLayout(page);
		_poiPacketsPerTs = new QDoubleSpinBox;
		setupDouble(_poiPacketsPerTs, 0.0, 1e9, 0.001, 9);
		_poiBitsPerPacket = new QSpinBox;
		_poiBitsPerPacket->setRange(0, INT_MAX);
		_poiSeed = new QSpinBox;
		_poiSeed->setRange(0, INT_MAX);
		form->addRow(QString::fromLocal8Bit("Пакетов/ts:"), _poiPacketsPerTs);
		form->addRow(QString::fromLocal8Bit("Бит на пакет:"), _poiBitsPerPacket);
		form->addRow(QString::fromLocal8Bit("Сид:"), _poiSeed);
		_profileStack->addWidget(page);
	}

	{
		auto* page = new QWidget;
		auto* form = new QFormLayout(page);
		_burPacketsPerTsOn = new QDoubleSpinBox;
		setupDouble(_burPacketsPerTsOn, 0.0, 1e9, 0.001, 9);
		_burBitsPerPacket = new QSpinBox;
		_burBitsPerPacket->setRange(0, INT_MAX);
		_burMeanOn = new QDoubleSpinBox;
		setupDouble(_burMeanOn, 0.0, 1e12, 1.0, 6);
		_burMeanOff = new QDoubleSpinBox;
		setupDouble(_burMeanOff, 0.0, 1e12, 1.0, 6);
		_burSeed = new QSpinBox;
		_burSeed->setRange(0, INT_MAX);
		form->addRow(QString::fromLocal8Bit("Пакетов/ts (ON):"), _burPacketsPerTsOn);
		form->addRow(QString::fromLocal8Bit("Бит на пакет:"), _burBitsPerPacket);
		form->addRow(QString::fromLocal8Bit("Средн. длительность ON:"), _burMeanOn);
		form->addRow(QString::fromLocal8Bit("Средн. длительность OFF:"), _burMeanOff);
		form->addRow(QString::fromLocal8Bit("Сид:"), _burSeed);
		_profileStack->addWidget(page);
	}

	{
		auto* page = new QWidget;
		auto* form = new QFormLayout(page);
		_parPacketsPerTsOn = new QDoubleSpinBox;
		setupDouble(_parPacketsPerTsOn, 0.0, 1e9, 0.001, 9);
		_parBitsPerPacket = new QSpinBox;
		_parBitsPerPacket->setRange(0, INT_MAX);
		_parMinOn = new QDoubleSpinBox;
		setupDouble(_parMinOn, 0.0, 1e12, 1.0, 6);
		_parMinOff = new QDoubleSpinBox;
		setupDouble(_parMinOff, 0.0, 1e12, 1.0, 6);
		_parAlpha = new QDoubleSpinBox;
		setupDouble(_parAlpha, 1.0, 10.0, 0.01, 4);
		_parSeed = new QSpinBox;
		_parSeed->setRange(0, INT_MAX);
		form->addRow(QString::fromLocal8Bit("Пакетов/ts (ON):"), _parPacketsPerTsOn);
		form->addRow(QString::fromLocal8Bit("Бит на пакет:"), _parBitsPerPacket);
		form->addRow(QString::fromLocal8Bit("Мин. длительность ON:"), _parMinOn);
		form->addRow(QString::fromLocal8Bit("Мин. длительность OFF:"), _parMinOff);
		form->addRow(QString::fromLocal8Bit("Парето alpha:"), _parAlpha);
		form->addRow(QString::fromLocal8Bit("Сид:"), _parSeed);
		_profileStack->addWidget(page);
	}

	profileLayout->addWidget(_profileCombo);
	profileLayout->addWidget(_profileStack);

	mainLayout->addWidget(_profileGroup);

	auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
	mainLayout->addWidget(buttons);

	connect(_kindCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
		this, &ScenarioEventDialog::onKindChanged);
	connect(_profileCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
		this, &ScenarioEventDialog::onProfileChanged);
}

void ScenarioEventDialog::onKindChanged(int index)
{
	_profileGroup->setVisible(indexToKind(index) == ScenarioEventKind::STATION_SET_PROFILE);
}

void ScenarioEventDialog::onProfileChanged(int index)
{
	_profileStack->setCurrentIndex(index);
}

void ScenarioEventDialog::setData(const ScenarioEventData& d)
{
	_kindCombo->setCurrentIndex(kindToIndex(d.kind));
	_atSpin->setValue(static_cast<int>(d.at));
	_minIdSpin->setValue(static_cast<int>(d.minId));
	_maxIdSpin->setValue(static_cast<int>(d.maxId));

	const int profileIdx = static_cast<int>(d.profile.index());
	_profileCombo->setCurrentIndex(profileIdx);
	_profileStack->setCurrentIndex(profileIdx);

	std::visit([&](const auto& cfg) {
		using T = std::decay_t<decltype(cfg)>;
		if constexpr (std::is_same_v<T, CbrTrafficProfileConfig>)
		{
			_cbrBitsPerTs->setValue(cfg.bitsPerTimestamp);
		}
		else if constexpr (std::is_same_v<T, PoissonTrafficProfileConfig>)
		{
			_poiPacketsPerTs->setValue(cfg.packetsPerTimestamp);
			_poiBitsPerPacket->setValue(static_cast<int>(cfg.bitsPerPacket));
			_poiSeed->setValue(static_cast<int>(cfg.seed));
		}
		else if constexpr (std::is_same_v<T, BurstTrafficProfileConfig>)
		{
			_burPacketsPerTsOn->setValue(cfg.packetsPerTimestampOn);
			_burBitsPerPacket->setValue(static_cast<int>(cfg.bitsPerPacket));
			_burMeanOn->setValue(cfg.meanOnDuration);
			_burMeanOff->setValue(cfg.meanOffDuration);
			_burSeed->setValue(static_cast<int>(cfg.seed));
		}
		else if constexpr (std::is_same_v<T, ParetoBurstTrafficProfileConfig>)
		{
			_parPacketsPerTsOn->setValue(cfg.packetsPerTimestampOn);
			_parBitsPerPacket->setValue(static_cast<int>(cfg.bitsPerPacket));
			_parMinOn->setValue(cfg.minOnDuration);
			_parMinOff->setValue(cfg.minOffDuration);
			_parAlpha->setValue(cfg.alpha);
			_parSeed->setValue(static_cast<int>(cfg.seed));
		}
	}, d.profile);
}

ScenarioEventData ScenarioEventDialog::data() const
{
	ScenarioEventData d;
	d.kind  = indexToKind(_kindCombo->currentIndex());
	d.at    = static_cast<Timestamp>(_atSpin->value());
	d.minId = static_cast<StationID>(_minIdSpin->value());
	d.maxId = static_cast<StationID>(_maxIdSpin->value());

	switch (_profileCombo->currentIndex())
	{
	case 0:
	{
		CbrTrafficProfileConfig c;
		c.bitsPerTimestamp = _cbrBitsPerTs->value();
		d.profile = c;
		break;
	}
	case 1:
	{
		PoissonTrafficProfileConfig c;
		c.packetsPerTimestamp = _poiPacketsPerTs->value();
		c.bitsPerPacket       = static_cast<std::uint64_t>(_poiBitsPerPacket->value());
		c.seed                = static_cast<std::uint32_t>(_poiSeed->value());
		d.profile = c;
		break;
	}
	case 2:
	{
		BurstTrafficProfileConfig c;
		c.packetsPerTimestampOn = _burPacketsPerTsOn->value();
		c.bitsPerPacket         = static_cast<std::uint64_t>(_burBitsPerPacket->value());
		c.meanOnDuration        = _burMeanOn->value();
		c.meanOffDuration       = _burMeanOff->value();
		c.seed                  = static_cast<std::uint32_t>(_burSeed->value());
		d.profile = c;
		break;
	}
	case 3:
	{
		ParetoBurstTrafficProfileConfig c;
		c.packetsPerTimestampOn = _parPacketsPerTsOn->value();
		c.bitsPerPacket         = static_cast<std::uint64_t>(_parBitsPerPacket->value());
		c.minOnDuration         = _parMinOn->value();
		c.minOffDuration        = _parMinOff->value();
		c.alpha                 = _parAlpha->value();
		c.seed                  = static_cast<std::uint32_t>(_parSeed->value());
		d.profile = c;
		break;
	}
	}
	return d;
}