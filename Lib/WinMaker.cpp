//------------------------------------------
// (c) Reliable Software, 1997 -- 2002
//------------------------------------------

#include "WinMaker.h"

#include "WinEx.h"

namespace Win
{
	Maker::Maker (char const * className, HINSTANCE hInst)
	  : _exStyle (0),       // extended window style
		_className (className), // pointer to registered class name
		_windowName (0),    // pointer to window name
		_style (0), // window style
		_x (CW_USEDEFAULT), // horizontal position of window
		_y (0),             // vertical position of window
		_width (CW_USEDEFAULT), // window width  
		_height (0),        // window height
		_hWndParent (0),    // handle to parent or owner window
		_hMenu (0),         // handle to menu, or child-window identifier
		_hInstance (hInst), // handle to application instance
		_data (0)           // pointer to window-creation data
	{
	}

	void Maker::SetPosition (int x, int y, int width, int height)
	{
		_x = x;
		_y = y;
		_width = width;
		_height = height;
	}

	HWND Maker::Create (Controller * controller, char const * title)
	{
		HWND h = ::CreateWindowEx (
			_exStyle,
			_className,
			title,
			_style,
			_x,
			_y,
			_width,
			_height,
			_hWndParent,
			_hMenu,
			_hInstance,
			controller);

		if (h == 0)
			throw Win::Exception ("Internal error: Window Creation Failed.");
		return h;
	}
}
