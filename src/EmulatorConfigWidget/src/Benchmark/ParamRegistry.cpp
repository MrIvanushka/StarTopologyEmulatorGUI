#include "ParamRegistry.h"

#pragma push_macro("emit")
#undef emit
#include <StarTopologyEmulator/StarHubStrategy/FtpGenerator/FtpGeneratorFactory.h>
#include <StarTopologyEmulator/StarHubStrategy/IncomeLoadController/IncomeLoadControllerFactory.h>
#include <StarTopologyEmulator/Messages/StarHubPlanMessage.h>
#pragma pop_macro("emit")

using namespace starTopologyEmulator;

namespace
{

double getD(const ParamRegistry::ParamMap& p, const QString& k, double def)
{
	auto it = p.find(k);
	return (it != p.end()) ? it.value().toDouble() : def;
}

int getI(const ParamRegistry::ParamMap& p, const QString& k, int def)
{
	auto it = p.find(k);
	return (it != p.end()) ? it.value().toInt() : def;
}

bool getB(const ParamRegistry::ParamMap& p, const QString& k, bool def)
{
	auto it = p.find(k);
	return (it != p.end()) ? it.value().toBool() : def;
}

void fillBackoff(StarHubPlanMessage::BackoffConfig& cfg, const ParamRegistry::ParamMap& p)
{
	StarHubPlanMessage::BackoffConfig def;
	cfg.pTx            = getD(p, "backoffPTx", def.pTx);
	cfg.baseWindow     = static_cast<std::uint8_t>(getI(p, "backoffBaseWindow", def.baseWindow));
	cfg.maxWindow      = static_cast<std::uint8_t>(getI(p, "backoffMaxWindow", def.maxWindow));
	cfg.exponentBase   = getD(p, "backoffExponentBase", def.exponentBase);
	cfg.additiveStep   = static_cast<std::uint8_t>(getI(p, "backoffAdditiveStep", def.additiveStep));
	if (p.contains("backoffBackoffType"))
		cfg.backoffType = static_cast<StarHubPlanMessage::BackoffType>(getI(p, "backoffBackoffType", static_cast<int>(def.backoffType)));
	else if (p.contains("backoffUseExponential"))
		cfg.backoffType = getB(p, "backoffUseExponential", true) ? StarHubPlanMessage::BackoffType::BEB : StarHubPlanMessage::BackoffType::NONE;
	else
		cfg.backoffType = def.backoffType;
}

QList<ParamSpec> backoffParams()
{
	StarHubPlanMessage::BackoffConfig d;
	return {
		ParamSpec{"backoffPTx",            QString::fromLocal8Bit("Backoff: pTx"),
			ParamSpec::Type::Double, 0.0, 1.0, 0.01, 3, d.pTx, true},
		ParamSpec{"backoffBaseWindow",     QString::fromLocal8Bit("Backoff: базовое окно"),
			ParamSpec::Type::Int, 1, 255, 1, 0, int(d.baseWindow), true},
		ParamSpec{"backoffMaxWindow",      QString::fromLocal8Bit("Backoff: максимальное окно"),
			ParamSpec::Type::Int, 1, 255, 1, 0, int(d.maxWindow), true},
		ParamSpec{"backoffExponentBase",   QString::fromLocal8Bit("Backoff: основание экспоненты"),
			ParamSpec::Type::Double, 1.0, 10.0, 0.1, 3, d.exponentBase, true},
		ParamSpec{"backoffBackoffType", QString::fromLocal8Bit("Backoff: backoff type (0=NONE,1=BEB,2=MILD,3=LMILD)"),
			ParamSpec::Type::Int, 0, 3, 1, 0, static_cast<int>(d.backoffType), true},
		ParamSpec{"backoffAdditiveStep", QString::fromLocal8Bit("Backoff: LMILD additive step"),
			ParamSpec::Type::Int, 1, 255, 1, 0, int(d.additiveStep), false},
	};
}

ParamSpec sd(const char* key, const QString& label, double dflt, double minB, double maxB, double step, int dec)
{
	return ParamSpec{ QString::fromLatin1(key), label, ParamSpec::Type::Double, minB, maxB, step, dec, dflt, true };
}

ParamSpec si(const char* key, const QString& label, int dflt, int minB, int maxB)
{
	return ParamSpec{ QString::fromLatin1(key), label, ParamSpec::Type::Int, double(minB), double(maxB), 1, 0, dflt, true };
}

QList<ImplSpec> makeFtpRegistry()
{
	QList<ImplSpec> r;

	r.append(ImplSpec{ "Static", QString::fromLocal8Bit("Статический"), {
		si("raSlotsCount",        QString::fromLocal8Bit("Слотов случайного доступа в кадре"), 10, 0, 255),
		si("yellowSlotsCount",    QString::fromLocal8Bit("Слотов для аутентификации в кадре"),  0, 0, 255),
		si("operationSlotsCount", QString::fromLocal8Bit("Слотов для вошедших станций в кадре"), 15, 0, 255),
	}});

	r.append(ImplSpec{ "BacklogFeedback", QString::fromLocal8Bit("Обратная связь по бэклогу"), {
		sd("rhoAuth",     QString::fromLocal8Bit("Удельная нагрузка авторизации (rhoAuth)"), 2.0, 0.0, 1000.0, 0.1, 3),
		sd("R0",          QString::fromLocal8Bit("Базовый размер окна RA (R0)"),             20.0, 0.0, 10000.0, 1.0, 3),
		sd("kJ",          QString::fromLocal8Bit("Коэффициент по входному потоку (kJ)"),      1.0, 0.0, 1000.0, 0.1, 4),
		sd("kQ",          QString::fromLocal8Bit("Коэффициент по очереди (kQ)"),              0.1, 0.0, 1000.0, 0.01, 4),
		sd("jStar",       QString::fromLocal8Bit("Целевая нагрузка авторизации (jStar)"),    10.0, 0.0, 100000.0, 1.0, 3),
		sd("qStar",       QString::fromLocal8Bit("Целевая длина очереди (qStar)"),          100.0, 0.0, 100000.0, 1.0, 3),
		sd("deltaR",      QString::fromLocal8Bit("Макс. изменение RA-окна (deltaR)"),         5.0, 0.0, 1000.0, 1.0, 3),
		si("raMin",       QString::fromLocal8Bit("RA min"),       5, 0, 255),
		si("raMax",       QString::fromLocal8Bit("RA max"),      50, 0, 255),
		si("yellowSlots", QString::fromLocal8Bit("Жёлтых слотов в кадре"), 0, 0, 255),
	}});

	r.append(ImplSpec{ "Lyapunov", QString::fromLocal8Bit("Lyapunov drift-plus-penalty"), {
		sd("V",           QString::fromLocal8Bit("Коэффициент V"), 10.0, 0.0, 100000.0, 0.1, 4),
		si("raMin",       QString::fromLocal8Bit("RA min"),       5, 0, 255),
		si("raMax",       QString::fromLocal8Bit("RA max"),      50, 0, 255),
		si("yellowSlots", QString::fromLocal8Bit("Жёлтых слотов в кадре"), 0, 0, 255),
	}});

	r.append(ImplSpec{ "ServiceDelay", QString::fromLocal8Bit("Выравнивание задержки"), {
		sd("d0",          QString::fromLocal8Bit("Базовое смещение задержки (d0)"), 0.0, 0.0, 10000.0, 0.1, 4),
		sd("lambdaRatio", QString::fromLocal8Bit("Отношение интенсивностей (lambdaRatio)"), 1.0, 0.0, 1000.0, 0.01, 4),
		si("raMin",       QString::fromLocal8Bit("RA min"),       5, 0, 255),
		si("raMax",       QString::fromLocal8Bit("RA max"),      50, 0, 255),
		si("yellowSlots", QString::fromLocal8Bit("Жёлтых слотов в кадре"), 0, 0, 255),
	}});

	auto utilityCommon = [](){
		QList<ParamSpec> p;
		p.append(sd("wAcq",  QString::fromLocal8Bit("Вес acquisition (wAcq)"),       1.0, 0.0, 1000.0, 0.1, 4));
		p.append(sd("wAuth", QString::fromLocal8Bit("Вес авторизации (wAuth)"),       1.0, 0.0, 1000.0, 0.1, 4));
		p.append(sd("wColl", QString::fromLocal8Bit("Вес коллизий (wColl)"),          0.5, 0.0, 1000.0, 0.1, 4));
		p.append(sd("wB",    QString::fromLocal8Bit("Вес скорости передачи DA (wB)"), 1.0, 0.0, 1000.0, 0.1, 4));
		p.append(sd("wD",    QString::fromLocal8Bit("Вес задержки DA (wD)"),          0.5, 0.0, 1000.0, 0.1, 4));
		return p;
	};

	auto raCommon = [](){
		QList<ParamSpec> p;
		p.append(si("raMin",       QString::fromLocal8Bit("RA min"),       5, 0, 255));
		p.append(si("raMax",       QString::fromLocal8Bit("RA max"),      50, 0, 255));
		p.append(si("yellowSlots", QString::fromLocal8Bit("Жёлтых слотов в кадре"), 0, 0, 255));
		return p;
	};

	auto withCommon = [&](QList<ParamSpec> own){
		QList<ParamSpec> p = utilityCommon();
		p.append(own);
		p.append(raCommon());
		return p;
	};

	r.append(ImplSpec{ "F1Linear",      QString::fromLocal8Bit("F1: линейная"),                withCommon({}) });
	r.append(ImplSpec{ "F2Logarithmic", QString::fromLocal8Bit("F2: логарифмическая (Kelly)"), withCommon({}) });

	{
		QList<ParamSpec> own;
		own.append(sd("alphaFair", QString::fromLocal8Bit("Параметр alpha (alphaFair)"), 1.0, 0.0, 1000.0, 0.1, 4));
		own.append(sd("epsilon",   QString::fromLocal8Bit("Регуляризатор (epsilon)"),    1e-6, 0.0, 1.0, 1e-6, 9));
		r.append(ImplSpec{ "F3AlphaFair", QString::fromLocal8Bit("F3: alpha-fair (Mo-Walrand)"), withCommon(own) });
	}

	{
		QList<ParamSpec> own;
		own.append(sd("k",  QString::fromLocal8Bit("Крутизна сигмоиды (k)"), 1.0, -1000.0, 1000.0, 0.1, 4));
		own.append(sd("x0", QString::fromLocal8Bit("Точка перегиба (x0)"),    0.5, -1000.0, 1000.0, 0.01, 4));
		r.append(ImplSpec{ "F4Sigmoidal", QString::fromLocal8Bit("F4: сигмоидальная"), withCommon(own) });
	}

	{
		QList<ParamSpec> p;
		p.append(sd("wAcq",  QString::fromLocal8Bit("Вес acquisition (wAcq)"),    1.0, 0.0, 1000.0, 0.1, 4));
		p.append(sd("wColl", QString::fromLocal8Bit("Вес коллизий (wColl)"),      0.5, 0.0, 1000.0, 0.1, 4));
		p.append(sd("wB",    QString::fromLocal8Bit("Вес ln(d) (wB)"),            1.0, 0.0, 1000.0, 0.1, 4));
		p.append(sd("B",     QString::fromLocal8Bit("Штраф за дедлайн (B)"),     10.0, 0.0, 10000.0, 0.1, 4));
		p.append(sd("Dtar",  QString::fromLocal8Bit("Целевая задержка (Dtar)"),  10.0, 0.0, 10000.0, 0.1, 4));
		p.append(raCommon());
		r.append(ImplSpec{ "F5HardDeadline", QString::fromLocal8Bit("F5: жёсткий дедлайн"), p });
	}

	{
		QList<ParamSpec> p;
		p.append(sd("cD", QString::fromLocal8Bit("Удельная стоимость задержки DA (cD)"), 1.0, 0.0, 10000.0, 0.1, 4));
		p.append(sd("cJ", QString::fromLocal8Bit("Удельная стоимость задержки RA (cJ)"), 1.0, 0.0, 10000.0, 0.1, 4));
		p.append(sd("d0", QString::fromLocal8Bit("RTT-смещение (d0)"),                  0.0, 0.0, 10000.0, 0.1, 4));
		p.append(raCommon());
		r.append(ImplSpec{ "F6CostOfDelay", QString::fromLocal8Bit("F6: cost-of-delay (c?-rule)"), p });
	}

	{
		QList<ParamSpec> p;
		p.append(sd("wAcq",  QString::fromLocal8Bit("Вес acquisition (wAcq)"),     1.0, 0.0, 1000.0, 0.1, 4));
		p.append(sd("wColl", QString::fromLocal8Bit("Вес коллизий (wColl)"),       0.5, 0.0, 1000.0, 0.1, 4));
		p.append(sd("wB",    QString::fromLocal8Bit("Вес ln(1+d) (wB)"),           1.0, 0.0, 1000.0, 0.1, 4));
		p.append(sd("wS",    QString::fromLocal8Bit("Вес штрафа за бэклог (wS)"),  0.1, 0.0, 1000.0, 0.01, 4));
		p.append(raCommon());
		r.append(ImplSpec{ "F7QuadraticBacklog", QString::fromLocal8Bit("F7: квадратичный штраф за бэклог"), p });
	}

	{
		QList<ParamSpec> p;
		p.append(sd("rho", QString::fromLocal8Bit("Параметр эластичности (rho)"), 0.5, -100.0, 100.0, 0.1, 4));
		p.append(sd("wRa", QString::fromLocal8Bit("Вес RA (wRa)"),                1.0, 0.0, 1000.0, 0.1, 4));
		p.append(sd("wDa", QString::fromLocal8Bit("Вес DA (wDa)"),                1.0, 0.0, 1000.0, 0.1, 4));
		p.append(raCommon());
		r.append(ImplSpec{ "F8Ces", QString::fromLocal8Bit("F8: CES"), p });
	}

	return r;
}

QList<ImplSpec> makeControllerRegistry()
{
	QList<ImplSpec> r;

	{
		StarHubPlanMessage::BackoffConfig d;
		r.append(ImplSpec{ "Static", QString::fromLocal8Bit("Статический"), {
			sd("pTx",            QString::fromLocal8Bit("pTx"),                            d.pTx, 0.0, 1.0, 0.01, 3),
			si("baseWindow",     QString::fromLocal8Bit("Базовое окно backoff"),           int(d.baseWindow), 1, 255),
			si("maxWindow",      QString::fromLocal8Bit("Максимальное окно backoff"),      int(d.maxWindow), 1, 255),
			sd("exponentBase",   QString::fromLocal8Bit("Основание экспоненты"),           d.exponentBase, 1.0, 10.0, 0.1, 3),
			ParamSpec{"backoffType", QString::fromLocal8Bit("Backoff type (0=NONE,1=BEB,2=MILD,3=LMILD)"),
				ParamSpec::Type::Int, 0, 3, 1, 0, static_cast<int>(d.backoffType), true},
			ParamSpec{"additiveStep", QString::fromLocal8Bit("LMILD additive step"),
				ParamSpec::Type::Int, 1, 255, 1, 0, int(d.additiveStep), false},
		}});
	}

	r.append(ImplSpec{ "Pi", QString::fromLocal8Bit("PI-регулятор"), QList<ParamSpec>{
		sd("gTarget",              QString::fromLocal8Bit("Целевая нагрузка (gTarget)"),       0.9, 0.0, 10.0, 0.01, 4),
		sd("kP",                   QString::fromLocal8Bit("kP"),                                0.5, 0.0, 100.0, 0.01, 4),
		sd("kI",                   QString::fromLocal8Bit("kI"),                                0.05, 0.0, 100.0, 0.001, 4),
		si("integralWindowFrames", QString::fromLocal8Bit("Окно интегрирования"),               10, 1, 10000),
		sd("alpha",                QString::fromLocal8Bit("alpha"),                             1.0, 0.0, 100.0, 0.1, 4),
		sd("minProbability",       QString::fromLocal8Bit("Минимальная вероятность (pMin)"),    1e-3, 0.0, 1.0, 0.001, 6),
		sd("maxProbability",       QString::fromLocal8Bit("Максимальная вероятность (pMax)"),   1.0, 0.0, 1.0, 0.01, 4),
		sd("maxProbabilityStep",   QString::fromLocal8Bit("Макс. шаг вероятности"),             1.0, 0.0, 1.0, 0.01, 4),
		sd("epsilon",              QString::fromLocal8Bit("epsilon"),                           1e-9, 0.0, 1.0, 1e-6, 9),
		si("antiWindupKind",       QString::fromLocal8Bit("Anti-windup (0=None, 1=Clamping, 2=BackCalc, 3=Tracking)"), 0, 0, 3),
		sd("antiWindupKT",         QString::fromLocal8Bit("Anti-windup: kT (только для BackCalc)"), 0.41, 0.0, 100.0, 0.01, 4),
		ParamSpec{"allowFeedForward", QString::fromLocal8Bit("Feedforward (pFF = R / N_hat)"),
			ParamSpec::Type::Bool, 0, 1, 1, 0, false, false},
	} + backoffParams() });

	r.append(ImplSpec{ "SimpleMarginalUtility", QString::fromLocal8Bit("По предельной полезности (NUM)"), QList<ParamSpec>{
		sd("weightThroughput",  QString::fromLocal8Bit("Вес пропускной способности (wThroughput)"), 1.0, 0.0, 1000.0, 0.1, 3),
		sd("weightCollision",   QString::fromLocal8Bit("Вес коллизий (wCollision)"),                1.0, 0.0, 1000.0, 0.1, 3),
		sd("gradientStep",      QString::fromLocal8Bit("Шаг градиента (eta)"),                      0.1, 0.0, 10.0, 0.01, 4),
		sd("maxProbabilityStep",QString::fromLocal8Bit("Макс. шаг (maxProbabilityStep)"),           0.2, 0.0, 1.0, 0.01, 4),
		sd("minProbability",    QString::fromLocal8Bit("pMin"),                                     1e-3, 0.0, 1.0, 0.001, 6),
		sd("maxProbability",    QString::fromLocal8Bit("pMax"),                                      1.0, 0.0, 1.0, 0.01, 4),
		sd("epsilon",           QString::fromLocal8Bit("epsilon"),                                  1e-9, 0.0, 1.0, 1e-6, 9),
	} + backoffParams() });

	r.append(ImplSpec{ "PseudoBayesian", QString::fromLocal8Bit("Псевдо-байесовский"), QList<ParamSpec>{
		sd("minProbability", QString::fromLocal8Bit("pMin"),                          1e-3, 0.0, 1.0, 0.001, 6),
		sd("maxProbability", QString::fromLocal8Bit("pMax"),                           1.0, 0.0, 1.0, 0.01, 4),
		sd("maxStepUp",      QString::fromLocal8Bit("Макс. шаг повышения pTx"),        1.0, 0.0, 1.0, 0.01, 4),
		sd("maxStepDown",    QString::fromLocal8Bit("Макс. шаг понижения pTx"),        1.0, 0.0, 1.0, 0.01, 4),
		sd("epsilon",        QString::fromLocal8Bit("epsilon"),                       1e-9, 0.0, 1.0, 1e-6, 9),
	} + backoffParams() });

	r.append(ImplSpec{ "CollisionBudget", QString::fromLocal8Bit("Бюджет коллизий"), QList<ParamSpec>{
		sd("collisionBudget",    QString::fromLocal8Bit("Бюджет коллизий (collisionBudget)"), 0.1, 0.0, 10.0, 0.01, 4),
		sd("gradientStep",       QString::fromLocal8Bit("Шаг градиента (eta)"),                0.1, 0.0, 10.0, 0.01, 4),
		sd("lagrangianStep",     QString::fromLocal8Bit("Шаг лагранжиана"),                    0.1, 0.0, 10.0, 0.01, 4),
		sd("maxProbabilityStep", QString::fromLocal8Bit("Макс. шаг"),                          0.5, 0.0, 1.0, 0.01, 4),
		sd("minProbability",     QString::fromLocal8Bit("pMin"),                              1e-3, 0.0, 1.0, 0.001, 6),
		sd("maxProbability",     QString::fromLocal8Bit("pMax"),                               1.0, 0.0, 1.0, 0.01, 4),
		sd("epsilon",            QString::fromLocal8Bit("epsilon"),                           1e-9, 0.0, 1.0, 1e-6, 9),
	} + backoffParams() });

	r.append(ImplSpec{ "GreyModelAdaptiveBackoff", QString::fromLocal8Bit("Серая модель GM(1,1)"), QList<ParamSpec>{
		sd("gTarget",                   QString::fromLocal8Bit("Целевая нагрузка"),                      0.9, 0.0, 10.0, 0.01, 4),
		si("historySize",               QString::fromLocal8Bit("Размер истории"),                         6, 1, 1000),
		si("minHistoryForPrediction",   QString::fromLocal8Bit("Мин. выборок для предсказания"),         4, 1, 1000),
		si("minBackoffWindow",          QString::fromLocal8Bit("Мин. окно backoff"),                      1, 1, 255),
		si("maxBackoffWindow",          QString::fromLocal8Bit("Макс. окно backoff"),                    64, 1, 255),
		sd("epsilon",                   QString::fromLocal8Bit("epsilon"),                              1e-9, 0.0, 1.0, 1e-6, 9),
	} + backoffParams() });

	r.append(ImplSpec{ "LogBarrier", QString::fromLocal8Bit("Логарифмический барьер"), QList<ParamSpec>{
		sd("weightThroughput",   QString::fromLocal8Bit("Вес пропускной способности (wThroughput)"), 1.0, 0.0, 1000.0, 0.1, 3),
		sd("weightCollision",    QString::fromLocal8Bit("Вес коллизий (wCollision)"),                0.5, 0.0, 1000.0, 0.1, 3),
		sd("gradientStep",       QString::fromLocal8Bit("Шаг градиента (eta)"),                      0.1, 0.0, 10.0, 0.01, 4),
		sd("maxProbabilityStep", QString::fromLocal8Bit("Макс. шаг"),                                 0.5, 0.0, 1.0, 0.01, 4),
		sd("minProbability",     QString::fromLocal8Bit("pMin"),                                     1e-3, 0.0, 1.0, 0.001, 6),
		sd("maxProbability",     QString::fromLocal8Bit("pMax"),                                      1.0, 0.0, 1.0, 0.01, 4),
		sd("epsilon",            QString::fromLocal8Bit("epsilon"),                                  1e-9, 0.0, 1.0, 1e-6, 9),
	} + backoffParams() });

	r.append(ImplSpec{ "EnergyAware", QString::fromLocal8Bit("Энергоэффективный"), QList<ParamSpec>{
		sd("weightThroughput",   QString::fromLocal8Bit("Вес пропускной способности (wThroughput)"), 1.0, 0.0, 1000.0, 0.1, 3),
		sd("weightEnergy",       QString::fromLocal8Bit("Вес энергопотребления (wEnergy)"),          0.1, 0.0, 1000.0, 0.1, 3),
		sd("gradientStep",       QString::fromLocal8Bit("Шаг градиента (eta)"),                      0.1, 0.0, 10.0, 0.01, 4),
		sd("maxProbabilityStep", QString::fromLocal8Bit("Макс. шаг"),                                 0.5, 0.0, 1.0, 0.01, 4),
		sd("minProbability",     QString::fromLocal8Bit("pMin"),                                     1e-3, 0.0, 1.0, 0.001, 6),
		sd("maxProbability",     QString::fromLocal8Bit("pMax"),                                      1.0, 0.0, 1.0, 0.01, 4),
		sd("epsilon",            QString::fromLocal8Bit("epsilon"),                                  1e-9, 0.0, 1.0, 1e-6, 9),
	} + backoffParams() });

	r.append(ImplSpec{ "AlphaFair", QString::fromLocal8Bit("Alpha-fair"), QList<ParamSpec>{
		sd("alpha",              QString::fromLocal8Bit("Параметр alpha"),       1.0, 0.0, 1000.0, 0.1, 3),
		sd("gradientStep",       QString::fromLocal8Bit("Шаг градиента (eta)"),  0.1, 0.0, 10.0, 0.01, 4),
		sd("maxProbabilityStep", QString::fromLocal8Bit("Макс. шаг"),             0.5, 0.0, 1.0, 0.01, 4),
		sd("minProbability",     QString::fromLocal8Bit("pMin"),                 1e-3, 0.0, 1.0, 0.001, 6),
		sd("maxProbability",     QString::fromLocal8Bit("pMax"),                  1.0, 0.0, 1.0, 0.01, 4),
		sd("epsilon",            QString::fromLocal8Bit("epsilon"),              1e-9, 0.0, 1.0, 1e-6, 9),
	} + backoffParams() });

	r.append(ImplSpec{ "RiskSensitive", QString::fromLocal8Bit("Рискочувствительный"), QList<ParamSpec>{
		sd("beta",               QString::fromLocal8Bit("Коэффициент риска (beta)"), 1.0, 0.0, 100.0, 0.1, 4),
		sd("collisionPenalty",   QString::fromLocal8Bit("Штраф за коллизию"),         1.0, 0.0, 1000.0, 0.1, 3),
		sd("gradientStep",       QString::fromLocal8Bit("Шаг градиента (eta)"),       0.1, 0.0, 10.0, 0.01, 4),
		sd("maxProbabilityStep", QString::fromLocal8Bit("Макс. шаг"),                  0.5, 0.0, 1.0, 0.01, 4),
		sd("minProbability",     QString::fromLocal8Bit("pMin"),                      1e-3, 0.0, 1.0, 0.001, 6),
		sd("maxProbability",     QString::fromLocal8Bit("pMax"),                       1.0, 0.0, 1.0, 0.01, 4),
		sd("epsilon",            QString::fromLocal8Bit("epsilon"),                   1e-9, 0.0, 1.0, 1e-6, 9),
	} + backoffParams() });

	return r;
}

}

const QList<ImplSpec>& ParamRegistry::ftpGenerators()
{
	static const QList<ImplSpec> r = makeFtpRegistry();
	return r;
}

const QList<ImplSpec>& ParamRegistry::controllers()
{
	static const QList<ImplSpec> r = makeControllerRegistry();
	return r;
}

const ImplSpec* ParamRegistry::findFtpGenerator(const QString& kind)
{
	for (const auto& s : ftpGenerators())
		if (s.kind == kind) return &s;
	return nullptr;
}

const ImplSpec* ParamRegistry::findController(const QString& kind)
{
	for (const auto& s : controllers())
		if (s.kind == kind) return &s;
	return nullptr;
}

std::unique_ptr<IFtpGenerator> ParamRegistry::makeFtpGenerator(
	const QString& kind,
	const ParamMap& p,
	std::shared_ptr<IDynamicFrameSettings> dfs,
	std::shared_ptr<IIncomeStationsPredictor> predictor,
	std::shared_ptr<IBacklogAccumulator> backlog,
	std::shared_ptr<IFrameCalculator> frameCalc)
{
	if (kind == "Static")
	{
		return FtpGeneratorFactory::make(
			static_cast<std::uint8_t>(getI(p, "raSlotsCount", 10)),
			static_cast<std::uint8_t>(getI(p, "yellowSlotsCount", 0)),
			static_cast<std::uint8_t>(getI(p, "operationSlotsCount", 15)));
	}
	if (kind == "BacklogFeedback")
	{
		BacklogFeedbackFtpGeneratorConfig c;
		c.rhoAuth     = getD(p, "rhoAuth", c.rhoAuth);
		c.R0          = getD(p, "R0", c.R0);
		c.kJ          = getD(p, "kJ", c.kJ);
		c.kQ          = getD(p, "kQ", c.kQ);
		c.jStar       = getD(p, "jStar", c.jStar);
		c.qStar       = getD(p, "qStar", c.qStar);
		c.deltaR      = getD(p, "deltaR", c.deltaR);
		c.raMin       = static_cast<std::uint8_t>(getI(p, "raMin", c.raMin));
		c.raMax       = static_cast<std::uint8_t>(getI(p, "raMax", c.raMax));
		c.yellowSlots = static_cast<std::uint8_t>(getI(p, "yellowSlots", c.yellowSlots));
		return FtpGeneratorFactory::makeBacklogFeedback(dfs, predictor, backlog, frameCalc, std::move(c));
	}
	if (kind == "Lyapunov")
	{
		LyapunovFtpGeneratorConfig c;
		c.V           = getD(p, "V", c.V);
		c.raMin       = static_cast<std::uint8_t>(getI(p, "raMin", c.raMin));
		c.raMax       = static_cast<std::uint8_t>(getI(p, "raMax", c.raMax));
		c.yellowSlots = static_cast<std::uint8_t>(getI(p, "yellowSlots", c.yellowSlots));
		return FtpGeneratorFactory::makeLyapunov(dfs, predictor, backlog, frameCalc, std::move(c));
	}
	if (kind == "ServiceDelay")
	{
		ServiceDelayFtpGeneratorConfig c;
		c.d0          = getD(p, "d0", c.d0);
		c.lambdaRatio = getD(p, "lambdaRatio", c.lambdaRatio);
		c.raMin       = static_cast<std::uint8_t>(getI(p, "raMin", c.raMin));
		c.raMax       = static_cast<std::uint8_t>(getI(p, "raMax", c.raMax));
		c.yellowSlots = static_cast<std::uint8_t>(getI(p, "yellowSlots", c.yellowSlots));
		return FtpGeneratorFactory::makeServiceDelay(dfs, predictor, backlog, frameCalc, std::move(c));
	}

	auto fillRa = [&](auto& c) {
		c.raMin       = static_cast<std::uint8_t>(getI(p, "raMin", c.raMin));
		c.raMax       = static_cast<std::uint8_t>(getI(p, "raMax", c.raMax));
		c.yellowSlots = static_cast<std::uint8_t>(getI(p, "yellowSlots", c.yellowSlots));
	};
	auto fillUtility = [&](auto& c) {
		c.wAcq  = getD(p, "wAcq",  c.wAcq);
		c.wAuth = getD(p, "wAuth", c.wAuth);
		c.wColl = getD(p, "wColl", c.wColl);
		c.wB    = getD(p, "wB",    c.wB);
		c.wD    = getD(p, "wD",    c.wD);
	};

	if (kind == "F1Linear")
	{
		F1LinearUtilityConfig c; fillUtility(c); fillRa(c);
		return FtpGeneratorFactory::makeF1Linear(dfs, predictor, backlog, frameCalc, std::move(c));
	}
	if (kind == "F2Logarithmic")
	{
		F2LogarithmicUtilityConfig c; fillUtility(c); fillRa(c);
		return FtpGeneratorFactory::makeF2Logarithmic(dfs, predictor, backlog, frameCalc, std::move(c));
	}
	if (kind == "F3AlphaFair")
	{
		F3AlphaFairUtilityConfig c;
		fillUtility(c);
		c.alphaFair = getD(p, "alphaFair", c.alphaFair);
		c.epsilon   = getD(p, "epsilon",   c.epsilon);
		fillRa(c);
		return FtpGeneratorFactory::makeF3AlphaFair(dfs, predictor, backlog, frameCalc, std::move(c));
	}
	if (kind == "F4Sigmoidal")
	{
		F4SigmoidalUtilityConfig c;
		fillUtility(c);
		c.k  = getD(p, "k",  c.k);
		c.x0 = getD(p, "x0", c.x0);
		fillRa(c);
		return FtpGeneratorFactory::makeF4Sigmoidal(dfs, predictor, backlog, frameCalc, std::move(c));
	}
	if (kind == "F5HardDeadline")
	{
		F5HardDeadlineUtilityConfig c;
		c.wAcq  = getD(p, "wAcq",  c.wAcq);
		c.wColl = getD(p, "wColl", c.wColl);
		c.wB    = getD(p, "wB",    c.wB);
		c.B     = getD(p, "B",     c.B);
		c.Dtar  = getD(p, "Dtar",  c.Dtar);
		fillRa(c);
		return FtpGeneratorFactory::makeF5HardDeadline(dfs, predictor, backlog, frameCalc, std::move(c));
	}
	if (kind == "F6CostOfDelay")
	{
		F6CostOfDelayUtilityConfig c;
		c.cD = getD(p, "cD", c.cD);
		c.cJ = getD(p, "cJ", c.cJ);
		c.d0 = getD(p, "d0", c.d0);
		fillRa(c);
		return FtpGeneratorFactory::makeF6CostOfDelay(dfs, predictor, backlog, frameCalc, std::move(c));
	}
	if (kind == "F7QuadraticBacklog")
	{
		F7QuadraticBacklogUtilityConfig c;
		c.wAcq  = getD(p, "wAcq",  c.wAcq);
		c.wColl = getD(p, "wColl", c.wColl);
		c.wB    = getD(p, "wB",    c.wB);
		c.wS    = getD(p, "wS",    c.wS);
		fillRa(c);
		return FtpGeneratorFactory::makeF7QuadraticBacklog(dfs, predictor, backlog, frameCalc, std::move(c));
	}
	if (kind == "F8Ces")
	{
		F8CesUtilityConfig c;
		c.rho = getD(p, "rho", c.rho);
		c.wRa = getD(p, "wRa", c.wRa);
		c.wDa = getD(p, "wDa", c.wDa);
		fillRa(c);
		return FtpGeneratorFactory::makeF8Ces(dfs, predictor, backlog, frameCalc, std::move(c));
	}

	return nullptr;
}

std::unique_ptr<IIncomeLoadController> ParamRegistry::makeController(
	const QString& kind,
	const ParamMap& p,
	std::shared_ptr<IDynamicFrameSettings> dfs,
	std::shared_ptr<IIncomeStationsPredictor> predictor,
	MetricScope scope)
{
	if (kind == "Static")
	{
		StarHubPlanMessage::BackoffConfig c;
		c.pTx            = getD(p, "pTx", c.pTx);
		c.baseWindow     = static_cast<std::uint8_t>(getI(p, "baseWindow", c.baseWindow));
		c.maxWindow      = static_cast<std::uint8_t>(getI(p, "maxWindow", c.maxWindow));
		c.exponentBase   = getD(p, "exponentBase", c.exponentBase);
		c.additiveStep   = static_cast<std::uint8_t>(getI(p, "additiveStep", c.additiveStep));
		if (p.contains("backoffType"))
			c.backoffType = static_cast<StarHubPlanMessage::BackoffType>(getI(p, "backoffType", static_cast<int>(c.backoffType)));
		else if (p.contains("useExponential"))
			c.backoffType = getB(p, "useExponential", true) ? StarHubPlanMessage::BackoffType::BEB : StarHubPlanMessage::BackoffType::NONE;
		return IncomeLoadControllerFactory::make(predictor, std::move(c), std::move(scope));
	}
	if (kind == "Pi")
	{
		PiLoadControllerConfig c;
		c.gTarget              = getD(p, "gTarget", c.gTarget);
		c.kP                   = getD(p, "kP", c.kP);
		c.kI                   = getD(p, "kI", c.kI);
		c.integralWindowFrames = static_cast<std::uint32_t>(getI(p, "integralWindowFrames", c.integralWindowFrames));
		c.alpha                = getD(p, "alpha", c.alpha);
		c.minProbability       = getD(p, "minProbability", c.minProbability);
		c.maxProbability       = getD(p, "maxProbability", c.maxProbability);
		c.maxProbabilityStep   = getD(p, "maxProbabilityStep", c.maxProbabilityStep);
		c.epsilon              = getD(p, "epsilon", c.epsilon);
		c.allowFeedForward     = getB(p, "allowFeedForward", c.allowFeedForward);
		switch (getI(p, "antiWindupKind", 0))
		{
		case 1: c.antiWindup = ClampingAntiWindupConfig{}; break;
		case 2: { BackCalculationAntiWindupConfig aw; aw.kT = getD(p, "antiWindupKT", aw.kT); c.antiWindup = aw; break; }
		case 3: c.antiWindup = TrackingModeAntiWindupConfig{}; break;
		default: c.antiWindup = NoneAntiWindupConfig{}; break;
		}
		fillBackoff(c.backoffTemplate, p);
		return IncomeLoadControllerFactory::make(dfs, predictor, std::move(c), std::move(scope));
	}
	if (kind == "SimpleMarginalUtility")
	{
		SimpleMarginalUtilityBasedLoadControllerConfig c;
		c.weightThroughput   = getD(p, "weightThroughput", c.weightThroughput);
		c.weightCollision    = getD(p, "weightCollision", c.weightCollision);
		c.gradientStep       = getD(p, "gradientStep", c.gradientStep);
		c.maxProbabilityStep = getD(p, "maxProbabilityStep", c.maxProbabilityStep);
		c.minProbability     = getD(p, "minProbability", c.minProbability);
		c.maxProbability     = getD(p, "maxProbability", c.maxProbability);
		c.epsilon            = getD(p, "epsilon", c.epsilon);
		fillBackoff(c.backoffTemplate, p);
		return IncomeLoadControllerFactory::make(dfs, predictor, std::move(c), std::move(scope));
	}
	if (kind == "PseudoBayesian")
	{
		PseudoBayesianLoadControllerConfig c;
		c.minProbability = getD(p, "minProbability", c.minProbability);
		c.maxProbability = getD(p, "maxProbability", c.maxProbability);
		c.maxStepUp      = getD(p, "maxStepUp", c.maxStepUp);
		c.maxStepDown    = getD(p, "maxStepDown", c.maxStepDown);
		c.epsilon        = getD(p, "epsilon", c.epsilon);
		fillBackoff(c.backoffTemplate, p);
		return IncomeLoadControllerFactory::make(predictor, std::move(c), std::move(scope));
	}
	if (kind == "CollisionBudget")
	{
		CollisionBudgetLoadControllerConfig c;
		c.collisionBudget    = getD(p, "collisionBudget", c.collisionBudget);
		c.gradientStep       = getD(p, "gradientStep", c.gradientStep);
		c.lagrangianStep     = getD(p, "lagrangianStep", c.lagrangianStep);
		c.maxProbabilityStep = getD(p, "maxProbabilityStep", c.maxProbabilityStep);
		c.minProbability     = getD(p, "minProbability", c.minProbability);
		c.maxProbability     = getD(p, "maxProbability", c.maxProbability);
		c.epsilon            = getD(p, "epsilon", c.epsilon);
		fillBackoff(c.backoffTemplate, p);
		return IncomeLoadControllerFactory::make(dfs, predictor, std::move(c), std::move(scope));
	}
	if (kind == "GreyModelAdaptiveBackoff")
	{
		GreyModelAdaptiveBackoffControllerConfig c;
		c.gTarget                 = getD(p, "gTarget", c.gTarget);
		c.historySize             = static_cast<std::uint32_t>(getI(p, "historySize", c.historySize));
		c.minHistoryForPrediction = static_cast<std::uint32_t>(getI(p, "minHistoryForPrediction", c.minHistoryForPrediction));
		c.minBackoffWindow        = static_cast<std::uint8_t>(getI(p, "minBackoffWindow", c.minBackoffWindow));
		c.maxBackoffWindow        = static_cast<std::uint8_t>(getI(p, "maxBackoffWindow", c.maxBackoffWindow));
		c.epsilon                 = getD(p, "epsilon", c.epsilon);
		fillBackoff(c.backoffTemplate, p);
		return IncomeLoadControllerFactory::make(predictor, std::move(c), std::move(scope));
	}
	if (kind == "LogBarrier")
	{
		LogBarrierLoadControllerConfig c;
		c.weightThroughput   = getD(p, "weightThroughput", c.weightThroughput);
		c.weightCollision    = getD(p, "weightCollision", c.weightCollision);
		c.gradientStep       = getD(p, "gradientStep", c.gradientStep);
		c.maxProbabilityStep = getD(p, "maxProbabilityStep", c.maxProbabilityStep);
		c.minProbability     = getD(p, "minProbability", c.minProbability);
		c.maxProbability     = getD(p, "maxProbability", c.maxProbability);
		c.epsilon            = getD(p, "epsilon", c.epsilon);
		fillBackoff(c.backoffTemplate, p);
		return IncomeLoadControllerFactory::make(dfs, predictor, std::move(c), std::move(scope));
	}
	if (kind == "EnergyAware")
	{
		EnergyAwareLoadControllerConfig c;
		c.weightThroughput   = getD(p, "weightThroughput", c.weightThroughput);
		c.weightEnergy       = getD(p, "weightEnergy", c.weightEnergy);
		c.gradientStep       = getD(p, "gradientStep", c.gradientStep);
		c.maxProbabilityStep = getD(p, "maxProbabilityStep", c.maxProbabilityStep);
		c.minProbability     = getD(p, "minProbability", c.minProbability);
		c.maxProbability     = getD(p, "maxProbability", c.maxProbability);
		c.epsilon            = getD(p, "epsilon", c.epsilon);
		fillBackoff(c.backoffTemplate, p);
		return IncomeLoadControllerFactory::make(dfs, predictor, std::move(c), std::move(scope));
	}
	if (kind == "AlphaFair")
	{
		AlphaFairLoadControllerConfig c;
		c.alpha              = getD(p, "alpha", c.alpha);
		c.gradientStep       = getD(p, "gradientStep", c.gradientStep);
		c.maxProbabilityStep = getD(p, "maxProbabilityStep", c.maxProbabilityStep);
		c.minProbability     = getD(p, "minProbability", c.minProbability);
		c.maxProbability     = getD(p, "maxProbability", c.maxProbability);
		c.epsilon            = getD(p, "epsilon", c.epsilon);
		fillBackoff(c.backoffTemplate, p);
		return IncomeLoadControllerFactory::make(dfs, predictor, std::move(c), std::move(scope));
	}
	if (kind == "RiskSensitive")
	{
		RiskSensitiveLoadControllerConfig c;
		c.beta               = getD(p, "beta", c.beta);
		c.collisionPenalty   = getD(p, "collisionPenalty", c.collisionPenalty);
		c.gradientStep       = getD(p, "gradientStep", c.gradientStep);
		c.maxProbabilityStep = getD(p, "maxProbabilityStep", c.maxProbabilityStep);
		c.minProbability     = getD(p, "minProbability", c.minProbability);
		c.maxProbability     = getD(p, "maxProbability", c.maxProbability);
		c.epsilon            = getD(p, "epsilon", c.epsilon);
		fillBackoff(c.backoffTemplate, p);
		return IncomeLoadControllerFactory::make(dfs, predictor, std::move(c), std::move(scope));
	}
	return nullptr;
}
