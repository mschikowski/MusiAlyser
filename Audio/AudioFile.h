#if !defined (AUDIOFILE_H)
#define AUDIOFILE_H
//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------
#include "WaveFormat.h"
#include "WinEx.h"
#include <windows.h>
#include <mmsystem.h>
#include <memory>
#include <vector>
#include <cassert>

namespace Wave
{
	class ChunkInfo: public MMCKINFO
	{
	public:
		ChunkInfo ()
		{
			memset (this, sizeof (MMCKINFO), 0);
		}
		void SetType (char const type [])
		{
			fccType = ::mmioStringToFOURCC (type, 0);
		}
		void SetId (char const type [])
		{
			ckid = ::mmioStringToFOURCC (type, 0);
		}
		unsigned long GetSize () const { return cksize; }
		void SetSize (unsigned long size) { cksize = size; }
	};

	class File
	{
	public:
		~File ()
		{
			while (_chunkStack.size () != 0)
				Ascend ();
			::mmioClose (_h, 0);
		}
		Wave::Format const & GetFormat () const
		{
			return _format;
		}
	protected:
		File (HMMIO h) : _h (h)
		{
			if (_h == 0)
				throw Win::Exception ("Cannot open file");
		}
		void SetFormat (Wave::Format const & format)
		{
			_format = format;
		}
		void Ascend () throw ()
		{
			assert (_chunkStack.size () > 0);
			::mmioAscend(_h, CurChunk (), 0);
			_chunkStack.pop_back ();
		}
		void Descend (char const * id)
		{
			Wave::ChunkInfo chunk;
			chunk.SetId (id);
			Descend (chunk, MMIO_FINDCHUNK);
		}
		void DescendToRiff (char const * type)
		{
			Wave::ChunkInfo chunk;
			chunk.SetType (type);
			Descend (chunk, MMIO_FINDRIFF);
		}
		void Descend (ChunkInfo & chunk, unsigned flags)
		{
			MMRESULT err = ::mmioDescend (_h, &chunk, CurChunk (), flags);
			if (err == MMIOERR_CHUNKNOTFOUND)
				throw Win::Exception ("Cannot find chunk");
			else if (err != MMSYSERR_NOERROR)
				throw Win::Exception ("Error finding chunk");
			_chunkStack.push_back (chunk);
		}
		void CreateChunk (char const * id)
		{
			Wave::ChunkInfo chunk;
			chunk.SetId (id);
			MMRESULT err = ::mmioCreateChunk (_h, &chunk, MMIO_CREATERIFF);
			if (err != MMSYSERR_NOERROR)
				throw Win::Exception ("Error creating chunk");
			_chunkStack.push_back (chunk);

		}
		Wave::ChunkInfo * CurChunk ()
		{
			if (_chunkStack.size () != 0)
				return &_chunkStack.back ();
			else
				return 0;
		}
		unsigned ChunkSize () const 
		{
			assert (_chunkStack.size () > 0);
			return _chunkStack.back ().GetSize ();
		}
		void SetChunkSize (unsigned size)
		{
			assert (_chunkStack.size () > 0);
			_chunkStack.back ().SetSize (size);
		}
	protected:
		Wave::Format	_format;
		bool			_descended;
		HMMIO			_h;
		std::vector<Wave::ChunkInfo> _chunkStack;
	};

	class FileIn: public File
	{
	public:
		FileIn (char const * path);
		unsigned GetSize () const { return _dataSize; }
		void Read (char * buf, long & size);
		void Rewind ();
	private:
		unsigned _dataSize;
	};

	class FileOut: public File
	{
	public:
		FileOut (char const * path, Wave::Format const & format);
		~FileOut ();
		void Write (char const * buf, long size);
	private:
	private:
		bool _isInData;
	};
}
#endif
