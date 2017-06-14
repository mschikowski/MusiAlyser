#include "Painter.h"
//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------
#include "Views.h"
#include "Fft.h"
#include "WaveIn.h"
#include "ImagePlayer.h"
#include "FilePlayer.h"
#include "WinMaker.h"
#include "Canvas.h"
#include "WinEx.h"
#include <sstream>
#include <cassert>

Painter::Painter (HWND hwndOwner)
:   _hwnd (hwndOwner), 
    _viewWave (hwndOwner),
    _viewFreq (hwndOwner),
	_source (16), // num of buffers
    _fftPoints (0),
	_sourceName ("test.wav"),
	_changed (false)
{
	_event.Block ();
	_thread.Resume ();
}

void Painter::SetFormat (
    unsigned samplesPerBuf, 
    unsigned samplesPerSec, 
    unsigned fftPoints,
    unsigned bitsPerSample) throw ()
{
	Win::Lock lock (_critSect);
	if (bitsPerSample == _format.BitsPerSample () &&
		samplesPerSec == _format.SamplesPerSec () &&
		fftPoints == _fftPoints &&
		samplesPerBuf == _samplesPerBuf)
	{
		return;
	}
	_changed = true;
	_samplesPerBuf = samplesPerBuf;
	_format.nSamplesPerSec = samplesPerSec;
	_fftPoints = fftPoints;
	_format.wBitsPerSample = bitsPerSample;
	_format.nChannels = 1;
	_format.nBlockAlign = _format.nChannels * bitsPerSample/8;
}

void Painter::ReInit () throw ()
{
    Win::Lock lock (_critSect);
	if (!_changed)
		return;
	_changed = false;

    bool wasStarted = true;
	if (_device.get () != 0)
		wasStarted = _device->IsStarted ();

	try
    {
		switch (_sourceType)
		{
		case SRC_MIKE:
			_device = std::auto_ptr<Wave::AsyncDevice> (new Wave::InDevice (_format));
			break;
		case SRC_WAVE:
			_device = std::auto_ptr<Wave::AsyncDevice> (new FilePlayer (_sourceName.c_str ()));
			break;
		case SRC_IMAGE:
			_device = std::auto_ptr<Wave::AsyncDevice> (new ImagePlayer (_viewFreq.Hwnd (), _sourceName.c_str (), _format.SamplesPerSec ()));
			break;
		};
		_format = _device->GetFormat ();

		_fftTransformer = std::auto_ptr<Fft> (new Fft (_fftPoints, _format.SamplesPerSec ()));
		_device->Open (_event);
		_source.PlugInDevice (_device.get (), _samplesPerBuf);
		if (wasStarted)
			_device->Start ();
		else 
			_device->Stop ();
		_viewFreq.Init (_fftTransformer->Points (), _fftTransformer->HzToPoint (1000));
		_viewWave.Init (_source.GetDataSize ());
    }
    catch (Win::Exception e)
    {
		std::ostringstream buf;
		buf << e.GetMessage () << ", Error " << std::hex << e.GetError ();
		::MessageBox (0, buf.str ().c_str (), "Exception", MB_ICONEXCLAMATION | MB_OK);
    }
    catch (...)
    {
        ::MessageBox (0, "Unknown", "Exception", MB_ICONEXCLAMATION | MB_OK);
    }
}

Painter::~Painter () 
{}

void Painter::Run ()
{
	try
	{
		for (;;)
		{
			_event.Wait ();
			if (_isDying)
				return;

			Win::Lock lock (_critSect);
			if (_source.IsDataReady ())
				LokWaveInData ();
		}
	}
	catch (Win::Exception & e)
	{
		std::ostringstream buf;
		buf << e.GetMessage () << ", Error " << std::hex << e.GetError ();
		::MessageBox (0, buf.str ().c_str (), "Exception", MB_ICONEXCLAMATION | MB_OK);
	}
	catch (...)
	{
		::MessageBox (0, "Unknown", "Exception", MB_ICONEXCLAMATION | MB_OK);
	}
}

void Painter::FlushThread ()
{
    _event.Release ();
}

bool Painter::Start () throw ()
{
	bool success = false;
	try
	{
		Win::Lock lock (_critSect);
		_device->Start ();
		success = true;
	}
	catch (Win::Exception e)
	{
        char buf [50];
		// revisit: use strstream
        wsprintf ( buf, "%s, Error %d", e.GetMessage(), e.GetError() );
        ::MessageBox (0, buf, "Exception", MB_ICONEXCLAMATION | MB_OK);
    }
    catch (...)
    {
        ::MessageBox (0, "Unknown", "Exception", MB_ICONEXCLAMATION | MB_OK);
    }
	return success;
}

void Painter::Stop ()
{
    Win::Lock lock (_critSect);
    _device->Stop ();
}

void Painter::LokWaveInData ()
{
	_source.GetData (_outBuf);
    _fftTransformer->Transform (_outBuf);
    _viewFreq.Update (*_fftTransformer);
    _viewWave.Update (_outBuf);
}

