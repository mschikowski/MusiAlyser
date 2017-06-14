#include "WaveOut.h"
//------------------------------------
//  Reliable Software, (c) 2000-2002
//------------------------------------
#include "Thread.h" // Event
#include "WaveFormat.h"
using namespace Wave;

OutDevice::OutDevice ()
{
	_status = MMSYSERR_BADDEVICEID;
}

OutDevice::OutDevice (Wave::Format const & format, Event & event, unsigned idDev)
{
	Open (format, event, idDev);
}

OutDevice::~OutDevice ()
{
	if (Ok())
	{
		::waveOutReset(_handle);
		::waveOutClose (_handle);
	}
}

bool OutDevice::Open (Wave::Format const & format, Event& event, unsigned idDev)
{
	_status = ::waveOutOpen (
        &_handle, 
        idDev, 
        &format, 
        reinterpret_cast<DWORD> (event.Get ()),
        0, // callback instance data
        CALLBACK_EVENT);

	return Ok();
}

void OutDevice::Reset ()
{
	if (Ok())
		::waveOutReset (_handle);
}

bool OutDevice::Close ()
{
	if ( Ok() && ::waveOutClose (_handle) == 0)
	{
		_status = MMSYSERR_BADDEVICEID;
		return true;
	}
	else
		return true;
}

unsigned long OutDevice::GetSamplePosition ()
{
	MMTIME mtime;
	mtime.wType = TIME_SAMPLES;
	::waveOutGetPosition (_handle, &mtime, sizeof (MMTIME));
	return mtime.u.sample;
}

void OutDevice::GetErrorText (char* buf, int len)
{
	::waveOutGetErrorText (_status, buf, len);
}

inline void OutDevice::PrepareAndSend (Wave::Header * pHeader)
{
	::waveOutPrepareHeader(_handle, pHeader, sizeof(WAVEHDR));
	::waveOutWrite (_handle, pHeader, sizeof(WAVEHDR));
}

inline void OutDevice::UnPrepare (Wave::Header * pHeader)
{
	::waveOutUnprepareHeader (_handle, pHeader, sizeof(WAVEHDR));
}
