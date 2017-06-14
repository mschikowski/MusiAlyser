#include "AudioFile.h"
//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------
#include <cassert>
using namespace Wave;

FileIn::FileIn (char const * path)
	: File (::mmioOpen (const_cast<char *> (path), 
					  0, MMIO_READ | MMIO_ALLOCBUF))
{
	DescendToRiff ("WAVE");
	Descend ("fmt ");
	int size = CurChunk ()->GetSize ();
	std::vector<char> buf (size);
	if (::mmioRead(_h, (HPSTR) (&buf [0]), size) != size)
	{
		throw Win::Exception ("Format read failed");
	}
	memcpy (&_format, &buf [0], sizeof (Wave::Format));
	if (!_format.IsPCM ())
		throw Win::Exception ("Unknown WAV format");
	Ascend ();

	Descend ("data");
	_dataSize = ChunkSize ();
	if (_dataSize == 0)
		throw Win::Exception ("Data chunk is empty");
}

void FileIn::Rewind ()
{
	while (_chunkStack.size () != 0)
		Ascend ();
	::mmioSeek (_h, 0, SEEK_SET);
	DescendToRiff ("WAVE");
	Descend ("data");
}

void FileIn::Read (char * buf, long & size)
{
    size = ::mmioRead(_h, (HPSTR) buf, size);
	if (size == -1)
		throw Win::Exception ("File read failed");
}

// Revisit: error checking

FileOut::FileOut (char const * path, Wave::Format const & format)
	: File (::mmioOpen (const_cast<char *> (path), 
					  0, MMIO_CREATE | MMIO_READWRITE)),
	  _isInData (false)
{
	SetFormat (format);

	CreateChunk ("WAVE");

	CreateChunk ("fmt ");
	Write (reinterpret_cast<char const *> (&format), sizeof (WAVEFORMATEX));
	Ascend ();

	// Data chunk
	CreateChunk ("data");
}

void FileOut::Write (char const * buf, long size)
{
	assert (_chunkStack.size () != 0);
    ::mmioWrite (_h, buf, size);
}