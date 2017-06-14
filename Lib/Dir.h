#if !defined (DIR_H)
#define DIR_H
//-------------------------------------
// (c) Reliable Software 1998, 99, 2000
//-------------------------------------

#include <windows.h>
#include <string>

class FileSeq
{
public:
	FileSeq (char const * pattern);
    ~FileSeq ()
    {
		if (_handle != INVALID_HANDLE_VALUE)
			::FindClose (_handle);
    }
    bool AtEnd () const
    {
        return _atEnd;
    }
	virtual void Advance ();
    char const * GetName () const;
	bool IsFolder () const { return (_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0; }
	bool IsReadOnly () const { return (_data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0; }
	bool IsDots () const 
	{ 	
		return _data.cFileName [0] == '.' && (_data.cFileName [1] == '\0' // single dot
				|| (_data.cFileName [1] == '.' && _data.cFileName [2] == '\0')); // double dot
	}
protected:
    bool            _atEnd;
    HANDLE          _handle;
    WIN32_FIND_DATA _data;
};

class CurrentDir
{
public:
	CurrentDir ();
	void Change (std::string const & path);
	std::string const Get () const { return _dir; }
private:
	std::string _dir;
};

#endif
