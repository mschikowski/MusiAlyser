#if !defined (TUNER_H)
#define TUNER_H
// (c) Reliable Software 2002
#include "WaveOut.h"
#include "Controller.h"
#include <vector>


class Tuner: public Wave::OutDevice
{
public:
 	Tuner (double freq, long samplesPerSec)
		: _freq (freq), _samplesPerSec (samplesPerSec)
	{
		Wave::Format format (8, 1, _samplesPerSec);
		SetFormat (format);
	}
	void InitBuffer (char * buf, unsigned size);
private:
	double	_freq;
	long	_samplesPerSec;
};

#endif
