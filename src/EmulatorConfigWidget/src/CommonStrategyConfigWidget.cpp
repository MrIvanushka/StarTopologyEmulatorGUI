#include "CommonStrategyConfigWidget.h"
#include <QFormLayout>
#include <QHBoxLayout>

#pragma push_macro("emit")
#undef emit
#include <StarTopologyEmulator/StarHubStrategy/StarHubStrategyFactory.h>
#include <StarTopologyEmulator/StarHubStrategy/IncomeLoadController/IncomeLoadControllerFactory.h>
#include <StarTopologyEmulator/StarHubStrategy/IncomeStationsPredictor/IncomeStationsPredictorFactory.h>
#include <StarTopologyEmulator/StarHubStrategy/FtpGenerator/FtpGeneratorFactory.h>
#pragma pop_macro("emit")

using namespace starTopologyEmulator;

CommonStrategyConfigWidget::CommonStrategyConfigWidget(QWidget* parent)
	: QWidget(parent)
{
	setupUi();
}

std::unique_ptr<IStarHubStrategy> CommonStrategyConfigWidget::makeStrategy(
	std::shared_ptr<IIncomeLoadEstimator> estimator,
	std::shared_ptr<IDynamicFrameSettings> dynamicFrameSettings,
	std::shared_ptr<IFrameCalculator> frameCalculator,
	std::shared_ptr<IBacklogAccumulator> backlogAccumulator,
	MetricScope scope) const
{
	// Метрики хранятся в UTF-8 (внутри MetricSink), исходник в CP-1251 —
	// конвертируем через QString.
	auto predictorScope = scope.child(
		QString::fromLocal8Bit("Оценка числа абонентов").toUtf8().toStdString());
	auto controllerScope = scope.child(
		QString::fromLocal8Bit("Контроль нагрузки").toUtf8().toStdString());

	auto predictor = createIncomeStationsPredictor(
		estimator, dynamicFrameSettings, frameCalculator, std::move(predictorScope));

	auto ftp = createFtpGenerator(
		dynamicFrameSettings, predictor, backlogAccumulator, frameCalculator);

	auto incomeLoadController = createIncomeLoadController(
		dynamicFrameSettings, predictor, std::move(controllerScope));

	return StarHubStrategyFactory::make(
		std::move(ftp), std::move(incomeLoadController), std::move(scope));
}

void CommonStrategyConfigWidget::setupUi()
{
	auto* mainLayout = new QHBoxLayout(this);

	// --- Блок FTP-генераторов ---
	auto* ftpGroup = new QGroupBox(QString::fromLocal8Bit("Генератор FTP"));
	auto* ftpLayout = new QVBoxLayout(ftpGroup);
	_ftpGeneratorSelector = new QComboBox;
	_ftpGeneratorSelector->addItem(QString::fromLocal8Bit("Статический"));
	_ftpGeneratorSelector->addItem(QString::fromLocal8Bit("Обратная связь по бэклогу"));
	_ftpGeneratorSelector->addItem(QString::fromLocal8Bit("Выравнивание задержки"));
	_ftpGeneratorSelector->addItem(QString::fromLocal8Bit("Lyapunov drift-plus-penalty"));
	_ftpGeneratorSelector->addItem(QString::fromLocal8Bit("Маржинальная полезность"));

	_ftpGeneratorStack = new QStackedWidget;
	_staticFtpPage          = new StaticFtpGeneratorConfigWidget;
	_backlogFeedbackFtpPage = new BacklogFeedbackFtpGeneratorConfigWidget;
	_serviceDelayFtpPage    = new ServiceDelayFtpGeneratorConfigWidget;
	_lyapunovFtpPage        = new LyapunovFtpGeneratorConfigWidget;
	_marginalUtilityFtpPage = new MarginalUtilityFtpGeneratorConfigWidget;
	_ftpGeneratorStack->addWidget(_staticFtpPage);
	_ftpGeneratorStack->addWidget(_backlogFeedbackFtpPage);
	_ftpGeneratorStack->addWidget(_serviceDelayFtpPage);
	_ftpGeneratorStack->addWidget(_lyapunovFtpPage);
	_ftpGeneratorStack->addWidget(_marginalUtilityFtpPage);

	ftpLayout->addWidget(_ftpGeneratorSelector);
	ftpLayout->addWidget(_ftpGeneratorStack);

	// --- Блок предсказателей абонентов ---
	auto* predictorGroup = new QGroupBox(QString::fromLocal8Bit("Предсказатель числа абонентов"));
	auto* predictorLayout = new QVBoxLayout(predictorGroup);
	_predictorSelector = new QComboBox;
	_predictorSelector->addItem(QString::fromLocal8Bit("Линейная регрессия"));
	_predictorSelector->addItem(QString::fromLocal8Bit("Когортный предсказатель"));
	_predictorSelector->addItem(QString::fromLocal8Bit("Когортный предсказатель с backoff"));
	_predictorSelector->addItem(QString::fromLocal8Bit("Серая модель GM(1,1)"));

	_predictorStack = new QStackedWidget;
	_linearRegressionPredictorPage = new LinearRegressionConfigWidget;
	_greyModelPredictorPage        = new GreyModelConfigWidget;
	_predictorStack->addWidget(_linearRegressionPredictorPage);
	_predictorStack->addWidget(new QWidget);
	_predictorStack->addWidget(new QWidget);
	_predictorStack->addWidget(_greyModelPredictorPage);

	predictorLayout->addWidget(_predictorSelector);
	predictorLayout->addWidget(_predictorStack);

	// --- Блок контроллеров нагрузки ---
	auto* controllerGroup = new QGroupBox(QString::fromLocal8Bit("Контроллер входной нагрузки"));
	auto* controllerLayout = new QVBoxLayout(controllerGroup);
	_incomeLoadControllerSelector = new QComboBox;
	_incomeLoadControllerSelector->addItem(QString::fromLocal8Bit("Статический"));
	_incomeLoadControllerSelector->addItem(QString::fromLocal8Bit("PI-регулятор"));
	_incomeLoadControllerSelector->addItem(QString::fromLocal8Bit("По предельной полезности (NUM)"));
	_incomeLoadControllerSelector->addItem(QString::fromLocal8Bit("Псевдо-байесовский"));
	_incomeLoadControllerSelector->addItem(QString::fromLocal8Bit("Бюджет коллизий"));
	_incomeLoadControllerSelector->addItem(QString::fromLocal8Bit("Серая модель GM(1,1)"));
	_incomeLoadControllerSelector->addItem(QString::fromLocal8Bit("Логарифмический барьер"));
	_incomeLoadControllerSelector->addItem(QString::fromLocal8Bit("Энергоэффективный"));
	_incomeLoadControllerSelector->addItem(QString::fromLocal8Bit("Alpha-fair"));
	_incomeLoadControllerSelector->addItem(QString::fromLocal8Bit("Рискочувствительный"));

	_incomeLoadControllerStack = new QStackedWidget;
	_staticIncomePage          = new StaticIncomeLoadConfigWidget;
	_piIncomePage              = new PiIncomeLoadConfigWidget;
	_simpleUtilityIncomePage   = new SimpleUtilityBasedIncomeLoadConfigWidget;
	_pseudoBayesianIncomePage  = new PseudoBayesianIncomeLoadConfigWidget;
	_collisionBudgetIncomePage = new CollisionBudgetIncomeLoadConfigWidget;
	_greyModelIncomePage       = new GreyModelAdaptiveBackoffConfigWidget;
	_logBarrierIncomePage      = new LogBarrierIncomeLoadConfigWidget;
	_energyAwareIncomePage     = new EnergyAwareIncomeLoadConfigWidget;
	_alphaFairIncomePage       = new AlphaFairIncomeLoadConfigWidget;
	_riskSensitiveIncomePage   = new RiskSensitiveIncomeLoadConfigWidget;

	_incomeLoadControllerStack->addWidget(_staticIncomePage);
	_incomeLoadControllerStack->addWidget(_piIncomePage);
	_incomeLoadControllerStack->addWidget(_simpleUtilityIncomePage);
	_incomeLoadControllerStack->addWidget(_pseudoBayesianIncomePage);
	_incomeLoadControllerStack->addWidget(_collisionBudgetIncomePage);
	_incomeLoadControllerStack->addWidget(_greyModelIncomePage);
	_incomeLoadControllerStack->addWidget(_logBarrierIncomePage);
	_incomeLoadControllerStack->addWidget(_energyAwareIncomePage);
	_incomeLoadControllerStack->addWidget(_alphaFairIncomePage);
	_incomeLoadControllerStack->addWidget(_riskSensitiveIncomePage);

	controllerLayout->addWidget(_incomeLoadControllerSelector);
	controllerLayout->addWidget(_incomeLoadControllerStack);

	mainLayout->addWidget(ftpGroup);
	mainLayout->addWidget(predictorGroup);
	mainLayout->addWidget(controllerGroup);
	mainLayout->addStretch();

	connect(_ftpGeneratorSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
		_ftpGeneratorStack, &QStackedWidget::setCurrentIndex);

	connect(_predictorSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
		_predictorStack, &QStackedWidget::setCurrentIndex);

	connect(_incomeLoadControllerSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
		_incomeLoadControllerStack, &QStackedWidget::setCurrentIndex);
}

std::unique_ptr<IFtpGenerator> CommonStrategyConfigWidget::createFtpGenerator(
	std::shared_ptr<IDynamicFrameSettings> dynamicFrameSettings,
	std::shared_ptr<IIncomeStationsPredictor> predictor,
	std::shared_ptr<IBacklogAccumulator> backlogAccumulator,
	std::shared_ptr<IFrameCalculator> frameCalculator) const
{
	switch (_ftpGeneratorStack->currentIndex())
	{
	case 0:
	{
		auto cfg = _staticFtpPage->getConfig();
		return FtpGeneratorFactory::make(
			cfg.randomAccessSlotsCountInFrame,
			cfg.yellowSlotsCountInFrame,
			cfg.onlineSlotsCountInFrame);
	}
	case 1:
		return FtpGeneratorFactory::makeBacklogFeedback(
			dynamicFrameSettings, predictor, backlogAccumulator, frameCalculator,
			_backlogFeedbackFtpPage->getConfig());
	case 2:
		return FtpGeneratorFactory::makeServiceDelay(
			dynamicFrameSettings, predictor, backlogAccumulator, frameCalculator,
			_serviceDelayFtpPage->getConfig());
	case 3:
		return FtpGeneratorFactory::makeLyapunov(
			dynamicFrameSettings, predictor, backlogAccumulator, frameCalculator,
			_lyapunovFtpPage->getConfig());
	case 4:
		return FtpGeneratorFactory::makeMarginalUtility(
			dynamicFrameSettings, predictor, backlogAccumulator, frameCalculator,
			_marginalUtilityFtpPage->getConfig());
	default:
		return nullptr;
	}
}

std::shared_ptr<IIncomeStationsPredictor> CommonStrategyConfigWidget::createIncomeStationsPredictor(
	std::shared_ptr<IIncomeLoadEstimator> estimator,
	std::shared_ptr<IDynamicFrameSettings> dynamicFrameSettings,
	std::shared_ptr<IFrameCalculator> frameCalculator,
	MetricScope scope) const
{
	switch (_predictorStack->currentIndex())
	{
	case 0:
		return IncomeStationsPredictorFactory::make(
			estimator, dynamicFrameSettings, frameCalculator,
			_linearRegressionPredictorPage->getConfig(),
			std::move(scope));
	case 1:
		return IncomeStationsPredictorFactory::make(
			estimator, dynamicFrameSettings, std::move(scope));
	case 2:
		return IncomeStationsPredictorFactory::make(
			estimator, dynamicFrameSettings,
			BackoffAwareCogorthyIncomeStationsPredictorConfig{},
			std::move(scope));
	case 3:
		return IncomeStationsPredictorFactory::make(
			estimator, dynamicFrameSettings,
			_greyModelPredictorPage->getConfig(),
			std::move(scope));
	default:
		return nullptr;
	}
}

std::unique_ptr<IIncomeLoadController> CommonStrategyConfigWidget::createIncomeLoadController(
	std::shared_ptr<IDynamicFrameSettings> dynamicFrameSettings,
	std::shared_ptr<IIncomeStationsPredictor> predictor,
	MetricScope scope) const
{
	switch (_incomeLoadControllerStack->currentIndex())
	{
	case 0:
		return IncomeLoadControllerFactory::make(
			predictor,
			_staticIncomePage->getConfig(),
			std::move(scope));
	case 1:
		return IncomeLoadControllerFactory::make(
			dynamicFrameSettings, predictor,
			_piIncomePage->getConfig(),
			std::move(scope));
	case 2:
		return IncomeLoadControllerFactory::make(
			dynamicFrameSettings, predictor,
			_simpleUtilityIncomePage->getConfig(),
			std::move(scope));
	case 3:
		return IncomeLoadControllerFactory::make(
			predictor,
			_pseudoBayesianIncomePage->getConfig(),
			std::move(scope));
	case 4:
		return IncomeLoadControllerFactory::make(
			dynamicFrameSettings, predictor,
			_collisionBudgetIncomePage->getConfig(),
			std::move(scope));
	case 5:
		return IncomeLoadControllerFactory::make(
			predictor,
			_greyModelIncomePage->getConfig(),
			std::move(scope));
	case 6:
		return IncomeLoadControllerFactory::make(
			dynamicFrameSettings, predictor,
			_logBarrierIncomePage->getConfig(),
			std::move(scope));
	case 7:
		return IncomeLoadControllerFactory::make(
			dynamicFrameSettings, predictor,
			_energyAwareIncomePage->getConfig(),
			std::move(scope));
	case 8:
		return IncomeLoadControllerFactory::make(
			dynamicFrameSettings, predictor,
			_alphaFairIncomePage->getConfig(),
			std::move(scope));
	case 9:
		return IncomeLoadControllerFactory::make(
			dynamicFrameSettings, predictor,
			_riskSensitiveIncomePage->getConfig(),
			std::move(scope));
	default:
		return nullptr;
	}
}