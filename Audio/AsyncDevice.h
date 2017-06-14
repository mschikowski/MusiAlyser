#if !defined ASYNCDEVICE_H
#define ASYNCDEVICE_H
//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------
#include "WaveFormat.h"
namespace Win { class Event; }

namespace Wave
{
	class Header;

	enum
	{ 
		DEFAULT_DEVICE = WAVE_MAPPER 
	};

	// Abstracts both input and output wave devices in the asynchronous mode
	class AsyncDevice
	{
	public:
		virtual ~AsyncDevice () {}
		virtual void Open (Win::Event & event) = 0;
		virtual void Close () = 0;
		virtual void Start () = 0;
		virtual void Stop () = 0;
		virtual void Prepare (Wave::Header * header) = 0;
		virtual void Unprepare (Wave::Header * header) = 0;
		virtual void InitBuffer (char * buf, unsigned size) {} // do nothing for input devices
		virtual void SendBuffer (Wave::Header * header) = 0;

		bool IsStarted () const { return _isStarted; }
		void PrepareAndSendBuffer (Wave::Header * header)
		{
			Prepare (header);
			SendBuffer (header);
		}
		Wave::Format const & GetFormat () const { return _format; }
	protected:
		void Started (bool isStarted) { _isStarted = isStarted; }
		Wave::Format	_format;
		bool			_isStarted;
	};
}

#endif
