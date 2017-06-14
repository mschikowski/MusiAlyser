#if !defined PAINTER_H
#define PAINTER_H
//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------
#include "Fft.h"
#include "BufferedDevice.h"
#include "WaveFormat.h"
#include "Active.h"
#include "Views.h"
#include "MediaFile.h"

class ViewWave;
class ViewFreq;

class Painter: public ActiveObject
{
public:
    Painter (HWND hwndOwner);
	~Painter ();

	void SetSource (SourceType src, char const * name = 0) 
	{
		assert (src != SRC_ANY && src != SRC_NONE);
		Win::Lock lock (_critSect);
		_sourceType = src;
		if (name != 0)
			_sourceName.assign (name);
		_changed = true;
	}

	SourceType GetSourceType ()
	{
		Win::Lock lock (_critSect);
		return _sourceType;
	}

    void ReInit () throw ();

	void SetFormat (
        unsigned samplesPerBuf, 
        unsigned samplesPerSec, 
        unsigned fftPoints,
        unsigned bitsPerSample) throw ();

	void GetFormat (unsigned & samplesPerSec, unsigned & bitsPerSample)
	{
		Win::Lock lock (_critSect);
		samplesPerSec = _format.SamplesPerSec ();
		bitsPerSample = _format.BitsPerSample ();
	}

    bool Start () throw ();
    void Stop ();

    int HzToPoint (int hz)
    { 
        Win::Lock lock (_critSect);
        return _fftTransformer->HzToPoint (hz); 
    }

    unsigned Points () 
    { 
        Win::Lock lock (_critSect);
        return _fftTransformer->Points (); 
    }

	unsigned SamplesPerBuf ()
	{
        Win::Lock lock (_critSect);
		return _samplesPerBuf;
	}

private:
    void InitThread () {}
    void Run ();
    void FlushThread ();

    void LokWaveInData ();
private:
    ViewWave	_viewWave;
    ViewFreq	_viewFreq;

	bool		_changed;
	Wave::Format _format;
    unsigned	_samplesPerBuf;
    unsigned	_fftPoints;

    HWND        _hwnd;

	Win::CritSect    _critSect;
	Win::Event       _event;
	
	SourceType				_sourceType;
	std::string				_sourceName;
	Wave::BufferedDevice	_source;
	std::auto_ptr<Wave::AsyncDevice> _device;
	std::auto_ptr<Fft>		_fftTransformer;
	std::vector<int>		_outBuf;
};

#endif
