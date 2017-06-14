#if !defined (CONTROLLER_H)
#define CONTROLLER_H
//----------------------------------------------------
// (c) Reliable Software 2000 -- 2002
//----------------------------------------------------

#include "Procedure.h"

namespace Win
{
    class Controller
    {
        friend LRESULT CALLBACK Procedure (HWND hwnd, 
                        UINT message, WPARAM wParam, LPARAM lParam);
		friend class Maker;

	protected:
		Controller ()
			: _h (0)
		{}
		virtual ~Controller ()
		{}
        void SetWindowHandle (HWND hwnd) { _h = hwnd; }

	public:
		HWND Handle () const { return _h; }
		virtual bool OnCreate () throw ()
			{ return false; }
        virtual bool OnDestroy () throw ()
			{ return false; }
		virtual bool OnSize (int width, int height, int flag) throw ()
			{ return false; }
		virtual bool OnClose () throw ()
			{ return false; }
		virtual bool OnPaint () throw ()
			{ return false; }
		virtual bool OnVScroll (int code, int pos) throw ()
			{ return false; }
		virtual bool OnHScroll (int code, int pos) throw ()
			{ return false; }

    protected:
		HWND  _h;
    };
}

#endif
