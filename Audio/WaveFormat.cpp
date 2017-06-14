#include "WaveFormat.h"
//------------------------------------
//  Reliable Software, (c) 2000-2002
//------------------------------------
using namespace Wave;

Format::Format ( 
	unsigned bitsPerSample,
	unsigned channels, // number of channels (mono = 1, stereo = 2)
	unsigned long sampleRate)
{
	wFormatTag = WAVE_FORMAT_PCM;
	nChannels = channels;
	nSamplesPerSec = sampleRate;
	nAvgBytesPerSec = sampleRate * channels * bitsPerSample/8;
	nBlockAlign = nChannels * bitsPerSample/8;
	wBitsPerSample = bitsPerSample;
	cbSize = 0;
}