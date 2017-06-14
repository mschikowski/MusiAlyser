#if !defined (AUDIOOUT_H)
#define AUDIOOUT_H
//------------------------------------
//  Reliable Software, (c) 2000-2002
//------------------------------------
#include "WaveHeader.h"
#include <windows.h>
#include <mmsystem.h>
class Event;

namespace Wave
{
	class Format;

	class OutDevice: public AsyncDevice
	{
	public:
		OutDevice ();
		OutDevice (Wave::Format const & format, Event & event, unsigned idDev = WAVE_MAPPER);
		~OutDevice ();
		bool    Open (Wave::Format const & format, Event & event, unsigned idDev = WAVE_MAPPER);
		void    Reset ();
		bool    Close ();
		bool    Ok () { return _status == MMSYSERR_NOERROR; }
		bool    isInUse () { return _status == MMSYSERR_ALLOCATED; }
		unsigned long GetSamplePosition ();
		unsigned GetError () { return _status; }
		void    GetErrorText (char* buf, int len);
		// AsyncDevice
		void OpenDevice (Win::Event & event);
		void StartDevice () { ::waveOutRestart(_handle); }
		void StopDevice ()  { ::waveOutPause(_handle); }
		void PrepareAndSendBuffer (Wave::Header * header);
		void UnprepareBuffer (Wave::Header * header);
	private:
		OutDevice	_device;
		MMRESULT	_status;
	};

}
#endif
