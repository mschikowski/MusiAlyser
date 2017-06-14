//------------------------------------
//  Reliable Software, (c) 2000-2002
//------------------------------------
#include "WaveOut.h"
#include "WaveHeader.h"
#include "Thread.h"
#include "WinEx.h"

using namespace Wave;

OutDevice::OutDevice (unsigned devId)
	: _devId (devId), _handle (0), _status (MMSYSERR_BADDEVICEID)
{
}

OutDevice::OutDevice (Wave::Format const & format, unsigned devId)
	: _devId (devId), _handle (0), _status (MMSYSERR_BADDEVICEID)
{
	SetFormat (format);
}

void OutDevice::SetFormat (Wave::Format const & format)
{
	_format = format;
}

void OutDevice::Start ()
{
	_status = ::waveOutRestart(_handle);
	Started (true);
}

void OutDevice::Stop ()
{
	_status = ::waveOutPause(_handle);
	Started (false);
}

OutDevice::~OutDevice ()
{
	if (Ok())
	{
		::waveOutReset(_handle);
		::waveOutClose (_handle);
	}
}

void OutDevice::Open (Win::Event & event)
{
	_status = ::waveOutOpen (
		&_handle, 
		_devId, 
		&_format, 
		reinterpret_cast<DWORD> (event.Get ()),
		0, // callback instance data
		CALLBACK_EVENT);
	if (!Ok ())
		throw Win::Exception ("Cannot open device");
}

void OutDevice::Prepare (Wave::Header * pHeader)
{
	::waveOutPrepareHeader(_handle, pHeader, sizeof(WAVEHDR));
}

void OutDevice::SendBuffer (Wave::Header * pHeader)
{
	::waveOutWrite (_handle, pHeader, sizeof(WAVEHDR));
}

void OutDevice::Unprepare (Wave::Header * pHeader)
{
	::waveOutUnprepareHeader (_handle, pHeader, sizeof(WAVEHDR));
}

void OutDevice::Reset ()
{
	if (Ok())
		::waveOutReset (_handle);
}

void OutDevice::Close ()
{
	if (Ok ())
	{
		::waveOutReset(_handle);
		if (::waveOutClose (_handle) != 0)
			throw Win::Exception ("Cannot close device");
		_status = MMSYSERR_BADDEVICEID;
	}
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
