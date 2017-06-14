#if !defined (WAVEHEADER_H)
#define WAVEHEADER_H
//------------------------------------
//  Reliable Software, (c) 2000-2002
//------------------------------------
#include <windows.h>
#include <mmsystem.h>
#include <memory>

namespace Wave
{
	class Header: public WAVEHDR
	{
	public:
		Header () 
		{
			std::uninitialized_fill_n (reinterpret_cast<char *> (this), sizeof (WAVEHDR), 0);
		}
		unsigned BufSize () const { return dwBufferLength; }
		bool IsDone () const { return (dwFlags & WHDR_DONE) != 0; }
	};
}

#endif
