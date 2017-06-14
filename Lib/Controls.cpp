//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------
#include "Controls.h"
#include <algorithm>
#include <strstream>
using namespace Win;

void Combo::SelectValue (int val)
{
	std::vector<int>::iterator it;
	it = std::find (_values.begin (), _values.end (), val);
	if (it != _values.end ())
		SelectPos (it - _values.begin ());
	else
	{
		AddValue (val);
		SelectPos (_values.size () - 1);
	}
}

std::string Combo::GetString ()
{
	std::string str;
	unsigned len = GetLen ();
	str.resize (len + 1);
	GetString (&str [0], len + 1);
	str.resize (len);
	return str;
}	

void Combo::AddValue (int val)
{
	char buf [10];
	std::strstream str( buf, 10);
	str << val << std::ends;
	AddString (buf);
	_values.push_back (val);
}

void EditReadOnly::SetNumber (int n)
{
	char buf [10];
	std::strstream str( buf, 10);
	str << n << std::ends;
	SetString (buf);
}

void EditReadOnly::SetHexNumber (int n)
{
	char buf [10];
	std::strstream str( buf, 10);
	str << std::hex << n << std::ends;
	SetString (buf);
}

void EditReadOnly::SetDouble (double x)
{
	char buf [10];
	std::strstream str( buf, 10);
	str << x << std::ends;
	SetString (buf);
}

void ScrollBar::Command (unsigned code, unsigned pos)
{
    unsigned iPos = _iPos; 
    switch (code)
    {
        case SB_TOP:
            iPos = _cPos - 1;
            break;
        case SB_BOTTOM:
            iPos = 0;
            break;
        case SB_LINEUP:
			if (iPos >= 1)
				iPos -= 1;
            break;
        case SB_PAGEUP:
			if (iPos >= _cPage)
				iPos -= _cPage;
            break;
        case SB_LINEDOWN:
            iPos += 1;
            if (iPos >= _cPos)
                iPos = _cPos - 1;
            break;
        case SB_PAGEDOWN:
            iPos += _cPage;
            if (iPos >= _cPos)
                iPos = _cPos - 1;
            break;
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            iPos = pos;
            break;
    }
	assert ((int)iPos >= 0);
    if (iPos != _iPos)
        SetPos (iPos);
}

LRESULT CALLBACK SubProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

SubEdit::SubEdit (HWND hwndParent, int id)
        : Edit (hwndParent, id)
{
	_wprocOld = (ProcPtr) ::GetWindowLong (Hwnd (), GWL_WNDPROC);
    
	::SetWindowLong (Hwnd (), GWL_USERDATA,  (LONG) this);
	::SetWindowLong (Hwnd (), GWL_WNDPROC,  (LONG) SubProc);
}


LRESULT CALLBACK SubProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_GETDLGCODE:
    {
        long code = SubEdit::CallDefault 
            (hWnd, message, wParam, lParam);
        return DLGC_WANTALLKEYS | code;
    }
    case WM_CHAR:
        //Process this message to avoid message beeps.
        if (wParam == VK_RETURN || wParam == VK_TAB)
            return 0;
        break;
    case WM_KEYDOWN:
        if (wParam == VK_RETURN) 
        {
			::PostMessage (GetParent(hWnd), WM_COMMAND, (WPARAM)IDOK, 0L);
            return 0;
        }
        break;
    }
    return SubEdit::CallDefault (hWnd, message, wParam, lParam);
}

