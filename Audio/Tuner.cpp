// (c) Reliable Software 2002
#include "Tuner.h"
#include <complex>

void Tuner::InitBuffer (char * buf, unsigned size)
{
	const double PI = 2.0 * asin (1.0);
	double a = 2.0 * PI * _freq / _samplesPerSec;
	double b = 2 * a;
	double c = 3 * a;
	double d = 4 * a;
	static unsigned phase = 0;
	for (unsigned i = 0; i < size; ++i)
	{
		buf [i] = 128 + static_cast<char> (
			32.0 * sin (a * (i + phase)) +
			16.0 * sin (b * (i + phase)) +
			8.0 * sin (d * (i + phase)) +
			8.0 * sin (c * (i + phase)));
	}
	phase += size;
};

