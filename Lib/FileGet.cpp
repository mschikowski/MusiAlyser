//------------------------------------
// (c) Reliable Software 1997-2004
//------------------------------------
#include "FileGet.h"
#include "WinEx.h"

FileGetter::FileGetter ()
{
    // Revisit: fix dynamic allocation
    _bufLen = 256;
    _buf = new char [_bufLen];
    memset (_buf, 0, _bufLen);

    lStructSize = sizeof (OPENFILENAME);
    hwndOwner = 0;          // owner window 
    hInstance = 0;          // instance handle 
    lpstrFilter = 0;        // e.g. "Text (*.txt *.me)\0*.txt;*.me\0"
    lpstrCustomFilter = 0; 
    nMaxCustFilter = 0; 
    nFilterIndex = 1;       // which filter to use? First is 1. 
    lpstrFile = _buf;       // buffer with file name and return buffer 
    nMaxFile = _bufLen; 
    lpstrFileTitle = 0; 
    nMaxFileTitle = 0; 
    lpstrInitialDir = 0;    // initial directory path (0 for default)
    lpstrTitle = 0; 
    Flags = OFN_EXPLORER; 
    nFileOffset = 0;        // out: offset of file name in buf 
    nFileExtension = 0;     // out: offset of extension
    lpstrDefExt = 0;        // default extension to append
    lCustData = (long)this; // the "this" pointer
    lpfnHook = 0; 
    lpTemplateName = 0; 
}

bool FileGetter::GetExistingFile (HWND hwnd, char const * title)
{
    hwndOwner = hwnd;
    lpstrTitle = title;
    Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY; 
    if (::GetOpenFileName (this) == FALSE)
    {
		// Check if Cancel or error
		DWORD errCode = ::CommDlgExtendedError ();
		if (errCode != 0)
			throw Win::Exception (title, errCode);
		else
			return false;	// User Canceled dialog
    }
    return true;
}

bool FileGetter::GetNewFile (HWND hwnd, char const * title)
{
    hwndOwner = hwnd;
    lpstrTitle = title;
    Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY; 
    if (::GetSaveFileName (this) == FALSE)
    {
		// Check if Cancel or error
		DWORD errCode = ::CommDlgExtendedError ();
		if (errCode != 0)
			throw Win::Exception (title, errCode);
		else
			return false;	// User Canceled dialog
    }
    return true;
}

UINT msgFileOk = 0;

bool FileGetter::GetMultipleFiles (HWND hwnd, char const * title)
{
    if (msgFileOk == 0)
        msgFileOk = ::RegisterWindowMessage (FILEOKSTRING);

    hwndOwner = hwnd;
    lpstrTitle = title;
    Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT; 
    if (::GetOpenFileName (this) == FALSE)
    {
		// Check if Cancel or error
		DWORD errCode = ::CommDlgExtendedError ();
		if (errCode != 0)
			throw Win::Exception (title, errCode);
		else
			return false;	// User Canceled dialog
    }
    return true;
}

