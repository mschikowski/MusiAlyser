#if !defined (WAVEFORMAT_H)
#define WAVEFORMAT_H
//------------------------------------
//  Reliable Software, (c) 2000-2002
//------------------------------------
#include <windows.h>
#include <mmsystem.h>

namespace Wave
{
	enum { BASE_SAMPLES_SEC = 11025 };

	class Format: public WAVEFORMATEX
	{
	public:
		Format ( 
			unsigned bitsPerSample = 8,
			unsigned channels = 1, // number of channels (mono, stereo)
			unsigned long sampleRate = BASE_SAMPLES_SEC // sample rate
			);

		bool IsInSupported (unsigned idDev = WAVE_MAPPER )
		{
			MMRESULT result = ::waveInOpen
				(0, idDev, this, 0, 0, WAVE_FORMAT_QUERY);
			return result == MMSYSERR_NOERROR;
		}
		bool IsOutSupported (unsigned idDev = WAVE_MAPPER)
		{
			MMRESULT result = ::waveOutOpen
				(0, idDev, this, 0, 0, WAVE_FORMAT_QUERY);
			return result == MMSYSERR_NOERROR;
		}
		bool IsPCM () const { return wFormatTag == WAVE_FORMAT_PCM; }

		int NumChannels () const { return nChannels; }
		int SamplesPerSec () const { return nSamplesPerSec; }
		int BitsPerSample () const { return  wBitsPerSample; }
		int BytesPerSample () const { return (wBitsPerSample * nChannels) / 8; }
	};
}
#endif
