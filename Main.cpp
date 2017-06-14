#include "Main.h"
//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------
#include "WinClass.h"
#include "Control.h"
#include "WinEx.h"
#include "resource.h"
#include <sstream>

// The main window is a modeless dialog box

int WINAPI WinMain
   (HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdParam, int cmdShow)
{
	Win::ClassMaker viewClass ("WinViewClass", hInst);
	viewClass.SetSizeRedraw ();
	HBRUSH brush = reinterpret_cast<HBRUSH> (::GetStockObject (BLACK_BRUSH));
	viewClass.SetBgBrush (brush);
	viewClass.Register ();

	HWND hDialog = ::CreateDialog (hInst, MAKEINTRESOURCE (DLG_MAIN), 0, DialogProc);

    if (hDialog == 0)
    {
		std::ostringstream buf;
		buf << "Error 0x" << std::hex << GetLastError ();
		::MessageBox (0, buf.str ().c_str (), "CreateDialog", MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }

    MSG  msg;
	int status;
	while ((status = ::GetMessage (&msg, NULL, 0, 0 )) != 0)
    {
		if (status == -1)
			break;
		if (!::IsDialogMessage (hDialog, &msg))
        {
			::TranslateMessage (&msg);
			::DispatchMessage (&msg);
        }
    }

    return msg.wParam;
}
