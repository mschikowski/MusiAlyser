#include "DlgProcedure.h"
#include "Control.h"
#include "WinEx.h"

// Main dialog handler
BOOL CALLBACK DialogProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static DlgController* pControl = 0;
    switch (message)
    {
    case WM_INITDIALOG:
        try
        {
            pControl = new DlgController (hwnd);
        }
        catch (Win::Exception e)
        {
            ::MessageBox (0, e.GetMessage (), "Exception", MB_ICONEXCLAMATION | MB_OK);
			return FALSE;
        }
        catch (...)
        {
            ::MessageBox (0, "Unknown", "Exception", MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
        }
        return TRUE;        
    case WM_COMMAND:
		if (pControl)
			return pControl->OnCommand(LOWORD (wParam), HIWORD (wParam))? TRUE: FALSE;
        return TRUE;
    case WM_HSCROLL:
		if (pControl)
			pControl->OnScroll (LOWORD (wParam), HIWORD (wParam));
        return 0;
	case WM_DESTROY:
		::PostQuitMessage (0);
		return 0;
    case WM_CLOSE:
		::DestroyWindow (hwnd);
        delete pControl;
        return 0;
    }
    return FALSE;
}
