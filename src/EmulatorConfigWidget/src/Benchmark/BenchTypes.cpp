#include "BenchTypes.h"

#include <cmath>

int SweepRange::pointCount() const
{
	if (!isValid())
		return 1;
	const double n = (max - min) / step;
	return static_cast<int>(std::floor(n + 1e-9)) + 1;
}
