#if !defined (WINMAKER_H)
#define WINMAKER_H
//-----------------------------------------
// Copyright Reliable Software 2000 -- 2002
//-----------------------------------------
#include <windows.h>

namespace Win
{
	class Controller;

	class Maker
	{
	public:
		Maker (char const * className, HINSTANCE hInstance);
		void SetPosition (int x, int y, int width, int height);
		void SetSize (int width, int height)
		{
			_width = width;
			_height = height;
		}
		void AddCaption (char const * caption)
		{
			_windowName = caption;
		}
		unsigned GetStyle () const { return _style; }
		unsigned GetExStyle () const { return _exStyle; }
		void SetOwner (HWND winOwner) { _hWndParent = winOwner; }
		void SetMaximized () { _style |= WS_MAXIMIZE; }
		void AddSysMenu () { _style |= WS_SYSMENU; }
		void AddVScrollBar () { _style |= WS_VSCROLL; }
		void AddHScrollBar () { _style |= WS_HSCROLL; }
		void AddDlgBorder () { _style |= WS_DLGFRAME; }
		void AddBorder () { _style |= WS_BORDER; }
		void AddResize () { _style |= WS_THICKFRAME; }
		void AddTitleBar () { _style |= WS_CAPTION; }
		void ClipChildren() { _style |= WS_CLIPCHILDREN; }
		void ClipSiblings() { _style |= WS_CLIPSIBLINGS; }
		void AddClientEdge() { _exStyle |= WS_EX_CLIENTEDGE; }
		void AddCreationData (void * data) { _data = data; }
		// Turn off the "visible" bit for those makers that make visible windows by default
		void SetInvisible () { _style &= ~WS_VISIBLE; }

		HWND Create (Controller * controller, char const * title);

		void Show (int nCmdShow = SW_SHOWNORMAL);

	protected:
		DWORD       _exStyle;       // extended window style
		char const *_className;     // pointer to registered class name
		char const *_windowName;    // pointer to window name
		DWORD       _style;         // window style
		int         _x;             // horizontal position of window
		int         _y;             // vertical position of window
		int         _width;         // window width  
		int         _height;        // window height
		HWND	    _hWndParent;    // handle to parent or owner window
		HMENU       _hMenu;         // handle to menu, or child-window identifier
		HINSTANCE   _hInstance;     // handle to application instance
		void *      _data;          // pointer to window-creation data
	};
};

#endif
