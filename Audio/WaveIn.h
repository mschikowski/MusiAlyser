#if !defined WAVEIN_H
#define WAVEIN_H
//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------
#include "WinEx.h"
#include "AsyncDevice.h"
#include <windows.h>
#include <mmsystem.h>

namespace Wave
{
	class Format;

	class InCapabilities
	{
	public:
		InCapabilities()
		{
			if (::waveInGetDevCaps (WAVE_MAPPER, &_waveInCaps, sizeof(WAVEINCAPS)) != MMSYSERR_NOERROR)
				throw Win::Exception ("Cannot determine	sound card capabilities!");
		}
		/*
			WAVE_FORMAT_1M08 11.025 kHz, mono, 8-bit 
			WAVE_FORMAT_1M16 11.025 kHz, mono, 16-bit 
			WAVE_FORMAT_1S08 11.025 kHz, stereo, 8-bit 
			WAVE_FORMAT_1S16 11.025 kHz, stereo, 16-bit 
			WAVE_FORMAT_2M08 22.05 kHz, mono, 8-bit 
			WAVE_FORMAT_2M16 22.05 kHz, mono, 16-bit 
			WAVE_FORMAT_2S08 22.05 kHz, stereo, 8-bit 
			WAVE_FORMAT_2S16 22.05 kHz, stereo, 16-bit 
			WAVE_FORMAT_4M08 44.1 kHz, mono, 8-bit 
			WAVE_FORMAT_4M16 44.1 kHz, mono, 16-bit 
			WAVE_FORMAT_4S08 44.1 kHz, stereo, 8-bit 
			WAVE_FORMAT_4S16 44.1 kHz, stereo, 16-bit 
			WAVE_FORMAT_96M08 96 kHz, mono, 8-bit 
			WAVE_FORMAT_96S08 96 kHz, stereo, 8-bit 
			WAVE_FORMAT_96M16 96 kHz, mono, 16-bit 
			WAVE_FORMAT_96S16 96 kHz, stereo, 16-bit 
		*/
		bool Is16BitSampling ()
		{
			return (_waveInCaps.dwFormats & WAVE_FORMAT_1M16) ||
				   (_waveInCaps.dwFormats & WAVE_FORMAT_1S16) ||
				   (_waveInCaps.dwFormats & WAVE_FORMAT_2M16) ||
				   (_waveInCaps.dwFormats & WAVE_FORMAT_2S16) ||
				   (_waveInCaps.dwFormats & WAVE_FORMAT_4M16) ||
				   (_waveInCaps.dwFormats & WAVE_FORMAT_4S16);
		}

		bool Is11_025kHz ()
		{
			return (_waveInCaps.dwFormats & WAVE_FORMAT_1M08) ||
				   (_waveInCaps.dwFormats & WAVE_FORMAT_1M16) ||
				   (_waveInCaps.dwFormats & WAVE_FORMAT_1S08) ||
				   (_waveInCaps.dwFormats & WAVE_FORMAT_1S16);
		}

		bool Is22_05kHz	()
		{
			return (_waveInCaps.dwFormats & WAVE_FORMAT_2M08) ||
				   (_waveInCaps.dwFormats & WAVE_FORMAT_2M16) ||
				   (_waveInCaps.dwFormats & WAVE_FORMAT_2S08) ||
				   (_waveInCaps.dwFormats & WAVE_FORMAT_2S16);
		}

		bool Is44_1kHz ()
		{
			return (_waveInCaps.dwFormats & WAVE_FORMAT_4M08) ||
				   (_waveInCaps.dwFormats & WAVE_FORMAT_4M16) ||
				   (_waveInCaps.dwFormats & WAVE_FORMAT_4S08) ||
				   (_waveInCaps.dwFormats & WAVE_FORMAT_4S16);
		}
	private:
		WAVEINCAPS _waveInCaps;
	};

	class InDevice: public AsyncDevice
	{
	public:
		InDevice (Wave::Format const & format, unsigned devId = DEFAULT_DEVICE);
		~InDevice ();
		void Open (Win::Event & event);
		void Close ();
		void Start ();
		void Stop ();
		void Prepare (Wave::Header * header);
		void Unprepare (Wave::Header * header);
		void SendBuffer (Wave::Header * header);

		void Reset ();
		bool Ok () { return _status == MMSYSERR_NOERROR; }
		bool IsInUse () { return _status == MMSYSERR_ALLOCATED; }
		unsigned long GetSamplePosition ();
		unsigned GetError () { return _status; }
		void     GetErrorText (char* buf, int len);
	private:
		unsigned		_devId;
		HWAVEIN			_handle;
		MMRESULT		_status;
	};
}
#endif
