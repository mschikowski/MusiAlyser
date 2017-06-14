//
// (c) Reliable Software, 1997-2002
//
#include "WinClass.h"
#include "WinEx.h"

namespace Win
{
	ClassMaker::ClassMaker (char const * className, HINSTANCE hInst, WNDPROC wndProc)
	{
		lpszClassName = className;
		SetDefaults (wndProc, hInst);
	}

	void ClassMaker::SetDefaults (WNDPROC wndProc, HINSTANCE hInst)
	{
		// Provide reasonable default values
		cbSize = sizeof (WNDCLASSEX);
		style = 0;
		lpfnWndProc = wndProc;
		hInstance = hInst;
		hIcon = 0;
		hIconSm = 0;
		lpszMenuName = 0;
		cbClsExtra = 0;
		cbWndExtra = 0;
		hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
		hCursor = ::LoadCursor (0, IDC_ARROW);
	}

	void ClassMaker::Register ()
	{
		if (::RegisterClassEx (this) == 0)
		{
			int err = ::GetLastError ();
			if (err != ERROR_CLASS_ALREADY_EXISTS)
				throw Win::Exception ("Internal error: RegisterClassEx failed.");
		}
		Win::ClearError (); // Windows bug: sets error even when successful
	}
};
