#if !defined FILEPLAYER_H
#define FILEPLAYER_H
//------------------------------------
//  Reliable Software, (c) 2002
//------------------------------------
#include "WaveOut.h"
#include "AudioFile.h"
#include "WinEx.h"
#include <algorithm>
#include <vector>

class FilePlayer: public Wave::OutDevice
{
public:
	FilePlayer (char const * fileName)
		: _file (fileName)
	{
		SetFormat (_file.GetFormat ());
		if (_file.GetSize () == 0)
			throw Win::Exception ("Zero size file");
	}
	void InitBuffer (char * buf, unsigned size);
private:
	Wave::FileIn	_file;
};

inline void FilePlayer::InitBuffer (char * buf, unsigned size)
{
	long totalSizeRead = 0;
	for (;;)
	{
		long sizeRead = size - totalSizeRead;
		_file.Read (buf + totalSizeRead, sizeRead);
		totalSizeRead += sizeRead;
		if (static_cast<unsigned> (totalSizeRead) < size)
			_file.Rewind ();
		else
			break;
	}
};

#endif