#include "BenchPlan.h"

#include <cmath>

namespace
{

struct SweepAxis
{
	QString        key;
	std::vector<QVariant> values;
};

std::vector<QVariant> enumerateRange(const SweepRange& r)
{
	std::vector<QVariant> v;
	if (!r.isValid())
	{
		v.push_back(r.min);
		return v;
	}
	const int n = r.pointCount();
	for (int i = 0; i < n; ++i)
		v.push_back(r.min + r.step * i);
	return v;
}

struct ResolvedImpl
{
	QString                  kind;
	QHash<QString, QVariant> fixedParams;
	std::vector<SweepAxis>   sweepAxes;
};

ResolvedImpl resolveImpl(const ImplConfig& cfg)
{
	ResolvedImpl r;
	r.kind = cfg.kind;
	for (auto it = cfg.params.constBegin(); it != cfg.params.constEnd(); ++it)
	{
		std::visit([&](const auto& v) {
			using T = std::decay_t<decltype(v)>;
			if constexpr (std::is_same_v<T, double>)
				r.fixedParams.insert(it.key(), v);
			else if constexpr (std::is_same_v<T, int>)
				r.fixedParams.insert(it.key(), v);
			else if constexpr (std::is_same_v<T, bool>)
				r.fixedParams.insert(it.key(), v);
			else if constexpr (std::is_same_v<T, SweepRange>)
				r.sweepAxes.push_back({ it.key(), enumerateRange(v) });
		}, it.value());
	}
	return r;
}

}

std::vector<BenchRun> BenchPlan::expand(const BenchConfig& cfg)
{
	std::vector<BenchRun> result;

	std::vector<ResolvedImpl> ftps;
	std::vector<ResolvedImpl> ctls;
	for (const auto& f : cfg.ftpGenerators) ftps.push_back(resolveImpl(f));
	for (const auto& c : cfg.controllers)   ctls.push_back(resolveImpl(c));

	std::vector<NamedScenario> scenarios = cfg.scenarios;
	if (scenarios.empty())
		scenarios.push_back({ QString(), cfg.base.scenario });

	int index = 0;
	for (const auto& scen : scenarios)
	{
		for (const auto& ftp : ftps)
		{
			for (const auto& ctl : ctls)
			{
				std::vector<int> ftpIdx(ftp.sweepAxes.size(), 0);
				std::vector<int> ctlIdx(ctl.sweepAxes.size(), 0);

				while (true)
				{
					BenchRun r;
					r.index          = index++;
					r.ftpKind        = ftp.kind;
					r.controllerKind = ctl.kind;
					r.scenarioName   = scen.name;
					r.scenario       = scen.events;
					r.ftpParams      = ftp.fixedParams;
					r.controllerParams = ctl.fixedParams;

					if (!scen.name.isEmpty())
						r.sweptParams.insert("scenario", scen.name);

					for (size_t i = 0; i < ftp.sweepAxes.size(); ++i)
					{
						const auto& ax = ftp.sweepAxes[i];
						const QVariant val = ax.values[ftpIdx[i]];
						r.ftpParams.insert(ax.key, val);
						r.sweptParams.insert("ftp." + ax.key, val);
					}
					for (size_t i = 0; i < ctl.sweepAxes.size(); ++i)
					{
						const auto& ax = ctl.sweepAxes[i];
						const QVariant val = ax.values[ctlIdx[i]];
						r.controllerParams.insert(ax.key, val);
						r.sweptParams.insert("controller." + ax.key, val);
					}

					result.push_back(std::move(r));

					int dim = static_cast<int>(ftpIdx.size()) - 1;
					while (dim >= 0)
					{
						if (++ftpIdx[dim] < static_cast<int>(ftp.sweepAxes[dim].values.size()))
							break;
						ftpIdx[dim] = 0;
						--dim;
					}
					if (dim >= 0) continue;

					dim = static_cast<int>(ctlIdx.size()) - 1;
					while (dim >= 0)
					{
						if (++ctlIdx[dim] < static_cast<int>(ctl.sweepAxes[dim].values.size()))
							break;
						ctlIdx[dim] = 0;
						--dim;
					}
					if (dim < 0) break;
				}
			}
		}
	}

	return result;
}
