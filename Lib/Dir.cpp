//-------------------------------------
// (c) Reliable Software 1998, 99, 2000
//-------------------------------------

#include "Dir.h"
#include "WinEx.h"
#include <cassert>

FileSeq::FileSeq (char const * pattern)
	:_atEnd (false),
	 _handle (::FindFirstFile (pattern, &_data))
{
    if (_handle == INVALID_HANDLE_VALUE)
    {
        int err = ::GetLastError ();
        if (err == ERROR_FILE_NOT_FOUND)
		{
            _atEnd = true;
			Win::ClearError ();
		}
        else
        {
            throw Win::Exception ("Cannot list files.");
        }
    }
	// skip "." and ".." directories
    while (!AtEnd () && IsFolder () && IsDots ())
		Advance ();
}

void FileSeq::Advance ()
{
	_atEnd = !::FindNextFile (_handle, &_data);
	if (_atEnd)
		Win::ClearError ();
}

char const * FileSeq::GetName () const
{
	assert (!_atEnd);
    return _data.cFileName;
}

CurrentDir::CurrentDir ()
{
	_dir.resize (MAX_PATH);
	unsigned len = ::GetCurrentDirectory (_dir.size (), &_dir [0]);
	if (len > _dir.size ())
	{
		_dir.resize (len);
		len = ::GetCurrentDirectory (_dir.size (), &_dir [0]);
	}
	if (len == 0)
		throw Win::Exception ("Internal error: Cannot get current directory.");
	_dir.resize (len);
}

void CurrentDir::Change (std::string const & path)
{
	_dir = path;
	if (!::SetCurrentDirectory (_dir.c_str ()))
		throw Win::Exception ("Internal error: Cannot set current directory.");
}


