#include "BufferedDevice.h"
#include "AsyncDevice.h"
#include <cassert>

using namespace Wave;

BufferedDevice::BufferedDevice (unsigned bufCount)
: _device (0),
  _samplesPerBuf (0),
  _bufCount (bufCount),
  _bytesPerBuf (0),
  _iBuf(0)
{
	_headers.resize (_bufCount);
}

void BufferedDevice::PlugInDevice (Wave::AsyncDevice * device, unsigned samplesPerBuf)
{
	_device = device;
	_samplesPerBuf = samplesPerBuf;
	_bytesPerBuf = _samplesPerBuf * _device->GetFormat ().BytesPerSample ();
    _bufArea.resize (_bytesPerBuf * _bufCount);
	_dataSize = SamplesPerBuf () / _device->GetFormat ().NumChannels ();
	InitBuffers ();
}

void BufferedDevice::InitBuffers ()
{
    // Don't initialize the last buffer
    // It will be initialized in the
    // first call to BufferDone
    for (unsigned i = 0; i != _bufCount - 1; i++ )
    {
        _headers[i].lpData = &_bufArea [i * _bytesPerBuf];
        _headers[i].dwBufferLength = _bytesPerBuf;
		_device->InitBuffer (_headers[i].lpData, _bytesPerBuf);
        _headers[i].dwFlags = 0;
        _headers[i].dwLoops = 0;

		_device->PrepareAndSendBuffer (&_headers [i]);
    }
    _iBuf = 0;
}

bool BufferedDevice::IsDataReady () const
{ 
    return _headers [_iBuf].IsDone (); 
}

void BufferedDevice::BufferDone ()
{
	assert (IsDataReady ());
	_device->Unprepare (&_headers [_iBuf]);
    int prevBuf = _iBuf - 1;
    if (prevBuf < 0)
        prevBuf = _bufCount - 1;

    _headers [prevBuf].lpData = &_bufArea [prevBuf * _bytesPerBuf];
    _headers [prevBuf].dwBufferLength = _bytesPerBuf;
	_device->InitBuffer (_headers[prevBuf].lpData, _bytesPerBuf);
    _headers [prevBuf].dwFlags = 0;
    _headers [prevBuf].dwLoops = 0;
    _device->PrepareAndSendBuffer (&_headers [prevBuf]);
}

void BufferedDevice::GetData (std::vector<int> & outBuf)
{
    assert (IsDataReady ());
	BufferDone ();

	outBuf.resize (_dataSize);
	Wave::Format const & format = _device->GetFormat ();
	if (format.BitsPerSample () == 8)
	{
	    unsigned char const * data8 = GetData8 ();
		if (format.NumChannels () == 1)
		{
			for (unsigned i = 0; i != _dataSize; ++i)
				outBuf [i] = (data8 [i] - 128) * 64;
		}
		else // stereo
		{
			for (unsigned i = 0; i != _dataSize; ++i)
				outBuf [i] = (data8 [2*i] + data8 [2*i+1] - 2 * 128) * 128;
		}
	}
	else if (format.BitsPerSample () == 16)
	{
		short const * data16 = GetData16 ();
		if (format.NumChannels () == 1)
		{
			for (unsigned i = 0; i != _dataSize; ++i)
				outBuf [i] = data16 [i];
		}
		else // stereo
		{
			for (unsigned i = 0; i != _dataSize; ++i)
				outBuf [i] = (data16 [2*i] +  data16 [2*i+1]) / 2;
		}
	}
	else
		throw "Unsupported format";
	NextBuffer ();
}

void BufferedDevice::PutData (std::vector<int> const & inBuf)
{
    assert (IsDataReady ());
	BufferDone ();

	assert (inBuf.size () == _dataSize);
	Wave::Format const & format = _device->GetFormat ();
	if (format.BitsPerSample () == 8)
	{
	    unsigned char * data8 = GetData8 ();
		if (format.NumChannels () == 1)
		{
			for (unsigned i = 0; i != _dataSize; ++i)
				data8 [i] = static_cast<unsigned char> (inBuf [i] / 64 + 128);
		}
		else // stereo
		{
			for (unsigned i = 0; i != _dataSize; ++i)
			{
				data8 [2 * i] = static_cast<unsigned char> (inBuf [i] / 64 + 128);
				data8 [2 * i + 1] = data8 [2 * i];
			}
		}
	}
	else if (format.BitsPerSample () == 16)
	{
		short * data16 = GetData16 ();
		if (format.NumChannels () == 1)
		{
			for (unsigned i = 0; i != _dataSize; ++i)
				data16 [i] = static_cast<short> (inBuf [i]);
		}
		else // stereo
		{
			for (unsigned i = 0; i != _dataSize; ++i)
			{
					data16 [2 * i] = static_cast<short> (inBuf [i]);
					data16 [2 * i + 1] = data16 [2 * i];
			}
		}
	}
	else
		throw "Unsupported format";
	NextBuffer ();
}