#if !defined BUFFEREDDEVICE_H
#define BUFFEREDDEVICE_H
//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------
#include "WaveHeader.h"
#include <vector>
namespace Win { class Event; }

namespace Wave
{
	class Header;
	class AsyncDevice;

	class BufferedDevice
	{
	public:
		BufferedDevice (unsigned bufCount);
		void PlugInDevice (Wave::AsyncDevice * device, unsigned samplesPerBuf);
		bool IsDataReady () const;
		unsigned SamplesPerBuf () const { return _samplesPerBuf; }
		void NextBuffer ()
		{
			++_iBuf;
			if (_iBuf == _bufCount)
				_iBuf = 0;
		}
		void GetData (std::vector<int> & outBuf);
		void PutData (std::vector<int> const & inBuf);
		unsigned GetDataSize () const { return _dataSize; }
	protected:
		void InitBuffers ();
	    void BufferDone ();
		unsigned char * GetData8 () const
		{
			return reinterpret_cast<unsigned char *> (_headers [_iBuf].lpData);
		}
		short * GetData16 () const
		{
			return reinterpret_cast<short *> (_headers [_iBuf].lpData);
		}
	protected:
		Wave::AsyncDevice *			_device;

		unsigned					_bufCount;	// number of buffers
		std::vector<char>			_bufArea;	// pool of buffers
		std::vector<Wave::Header>	_headers;	// pool of headers
		int							_iBuf;		// current buffer index

		unsigned					_samplesPerBuf;
		unsigned					_bytesPerBuf;
		unsigned					_dataSize;  // size of in/out buffer
	};
}

#endif
