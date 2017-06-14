#if !defined (WAVEOUT_H)
#define WAVEOUT_H
//------------------------------------
//  Reliable Software, (c) 2000-2002
//------------------------------------
#include "AsyncDevice.h"
#include <windows.h>
#include <mmsystem.h>

namespace Wave
{
	class Format;

	class OutDevice: public AsyncDevice
	{
	public:
		OutDevice (unsigned devId = DEFAULT_DEVICE);
		OutDevice (Wave::Format const & format, unsigned devId = DEFAULT_DEVICE);
		~OutDevice ();
		void SetFormat (Wave::Format const & format);
		void Open (Win::Event & event);
		void Close ();
		void Start ();
		void Stop ();
		void Prepare (Wave::Header * header);
		void Unprepare (Wave::Header * header);
		void SendBuffer (Wave::Header * header);

		void Reset ();
		bool Ok () { return _status == MMSYSERR_NOERROR; }
		bool isInUse () { return _status == MMSYSERR_ALLOCATED; }
		unsigned long GetSamplePosition ();
		unsigned GetError () { return _status; }
		void     GetErrorText (char* buf, int len);
	private:
		unsigned	_devId;
		HWAVEOUT	_handle;
		MMRESULT	_status;
	};

}
#endif
