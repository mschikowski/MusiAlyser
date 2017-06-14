//----------------------------------------------------
// (c) Reliable Software 2000 -- 2002
//
//----------------------------------------------------
#include "Procedure.h"
#include "Controller.h"
#include "Canvas.h"

using namespace Win;

// Window procedure shared by all window controllers

LRESULT CALLBACK Win::Procedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Controller * pCtrl = reinterpret_cast<Controller *> (::GetWindowLong (hwnd, GWL_USERDATA));

    switch (message)
    {
    case WM_NCCREATE:
        {
			// Connect controller to its window
            CREATESTRUCT const * create = 
                reinterpret_cast<CREATESTRUCT const *> (lParam);
            pCtrl = static_cast<Controller *> (create->lpCreateParams);
            pCtrl->SetWindowHandle (hwnd);
			::SetWindowLong (hwnd, GWL_USERDATA, reinterpret_cast<long> (pCtrl));
        }
        break;
	case WM_CREATE:
		if (pCtrl->OnCreate ())
			return 0;
		break;
    case WM_DESTROY:
        // We're no longer on screen
        pCtrl->OnDestroy ();
        return 0;
	case WM_SIZE:
		if (pCtrl->OnSize (LOWORD (lParam), HIWORD (lParam), wParam))
			return 0;
		break;
    case WM_VSCROLL:
		if (pCtrl->OnVScroll (LOWORD (wParam), HIWORD (wParam)))
			return 0;
		break;
	case WM_HSCROLL:
		if (pCtrl->OnHScroll (LOWORD (wParam), HIWORD (wParam)))
			return 0;
		break;
	case WM_CLOSE:
		if (pCtrl->OnClose ())
			return 0;
		break;
	case WM_PAINT:
		if (pCtrl->OnPaint ())
			return 0;
		break;
	}
    return ::DefWindowProc (hwnd, message, wParam, lParam);
}