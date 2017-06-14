#if !defined MAIN_H
#define MAIN_H
//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------
#include "WinEx.h"
#include <windows.h>

// procedures called by Windows

BOOL CALLBACK DialogProc
	(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain
    (HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdParam, int cmdShow);

#endif
