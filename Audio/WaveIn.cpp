//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------
#include "WaveIn.h"
#include "WaveHeader.h"
#include "Thread.h"
#include "WinEx.h"

using namespace Wave;

InDevice::InDevice (Wave::Format const & format, unsigned devId)
	: _devId (devId), _handle (0), _status (MMSYSERR_BADDEVICEID)
{
	_format = format;
}

void InDevice::Start ()
{
	_status = ::waveInStart(_handle);
	Started (true);
}

void InDevice::Stop ()
{
	_status = ::waveInStop(_handle);
	Started (false);
}

InDevice::~InDevice ()
{
	if (Ok ())
	{
		::waveInReset(_handle);
		::waveInClose (_handle);
	}
}

void InDevice::Open (Win::Event & event)
{
	_status = ::waveInOpen (
        &_handle, 
        _devId, 
        &_format, 
        reinterpret_cast<DWORD> (event.Get ()),
        0, // callback instance data
        CALLBACK_EVENT);
	switch (_status)
	{
	case MMSYSERR_NOERROR:
		return;
	case MMSYSERR_ALLOCATED:
		throw Win::Exception("Cannot open input device: Specified resource is already allocated.");
	case MMSYSERR_BADDEVICEID:
		throw Win::Exception("Cannot open input device: Specified device identifier is out of range.");
	case MMSYSERR_NODRIVER:
		throw Win::Exception("Cannot open input device: No device driver is present.");
	case MMSYSERR_NOMEM:
		throw Win::Exception("Cannot open input device: Unable to allocate or lock memory.");
	case WAVERR_BADFORMAT:
		throw Win::Exception("Cannot open input device: Attempted to open with an unsupported waveform-audio format.");
	case WAVERR_SYNC:
		throw Win::Exception("Cannot open input device: The device is synchronous but waveOutOpen was called without using the WAVE_ALLOWSYNC flag.");
	default:
		throw Win::Exception ("Cannot open input device: Unknown error");
	}
	if (!Ok ())
		throw Win::Exception ("Opening the input device failed.");
}

void InDevice::Prepare (Wave::Header * pHeader)
{
	::waveInPrepareHeader(_handle, pHeader, sizeof (WAVEHDR));
}

void InDevice::SendBuffer (Wave::Header * pHeader)
{
	::waveInAddBuffer (_handle, pHeader, sizeof (WAVEHDR));
}

void InDevice::Unprepare (Wave::Header * pHeader)
{
	::waveInUnprepareHeader (_handle, pHeader, sizeof (WAVEHDR));
}

void InDevice::Reset ()
{
	if (Ok())
		::waveInReset (_handle);
}

void InDevice::Close ()
{
	if (Ok ())
	{
		::waveInReset(_handle);
		if (::waveInClose (_handle) != 0)
			throw Win::Exception ("Cannot close device");
		_status = MMSYSERR_BADDEVICEID;
	}
}

// Returns position (sample count) from the beggining of recording
unsigned long InDevice::GetSamplePosition ()
{
	MMTIME mtime;
	mtime.wType = TIME_SAMPLES;
	::waveInGetPosition (_handle, &mtime, sizeof (MMTIME));
	return mtime.u.sample;
}

void InDevice::GetErrorText (char * buf, int len)
{
	::waveInGetErrorText (_status, buf, len);
}

