#if !defined controls_h
#define controls_h
//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------
#include <windows.h>
#include <vector>
#include <cassert>

namespace Win
{
	typedef LRESULT (CALLBACK *ProcPtr)(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

	// This is for controls embedded
	// in regular dialog boxes that have
	// a dialog procedure. They can be constructed
	// in response to WM_INITDIALOG message

	class SimpleControl
	{
	public:
		SimpleControl (HWND hwndParent, int id)
			: _hWnd (::GetDlgItem (hwndParent, id))
		{}

		void SetFocus ()
		{
			::SetFocus (_hWnd);
		}

		HWND Hwnd () const { return _hWnd; }
		void Init (HWND hwnd) { _hWnd = hwnd; }

	protected:
		HWND _hWnd;
	};

	class Combo: public SimpleControl
	{
	public:
		Combo (HWND hwndParent, int id)
			: SimpleControl (hwndParent, id)
		{}

		void ResetContents ()
		{
			::SendMessage(Hwnd (), CB_RESETCONTENT, 0, 0);
		}

		void AddValue (int val);

		void SelectPos (unsigned i)
		{
			assert (i < _values.size ());
			::SendMessage (Hwnd (), CB_SETCURSEL, i, 0);
		}

		void SelectValue (int val);

		// code is the HIWORD (wParam)
		static bool IsChanged (int code) 
		{ 
			return code == CBN_SELENDOK;
		}

		int GetValue ()
		{
			return _values [GetIndex()];
		}

		void AddString (char const * str)
		{
			::SendMessage (Hwnd (), CB_ADDSTRING, 0, (LPARAM) str);
		}

		int GetLen ()
		{
			return ::SendMessage (Hwnd (), WM_GETTEXTLENGTH, 0, 0);
		}

		void GetString (char* buf, int len)
		{
			::SendMessage (Hwnd (), WM_GETTEXT, (WPARAM) len, (LPARAM) buf);
		}

		std::string GetString ();

		void SetText (char const * txt)
		{
			::SendMessage(Hwnd (), WM_SETTEXT, 0, (LPARAM) txt); 
		}

	private:

		int GetIndex()
		{
			return ::SendMessage (Hwnd (), CB_GETCURSEL, 0, 0);
		}

		std::vector<int> _values;
	};

	class EditReadOnly: public SimpleControl
	{
	public:
		EditReadOnly (HWND hwndParent, int id)
			: SimpleControl (hwndParent, id)
		{}

		void SetString (char const * buf)
		{
			::SendMessage (Hwnd (), WM_SETTEXT, 0, (LPARAM) buf);
		}

		void SetNumber (int n);
		void SetHexNumber (int n);
		void SetDouble (double x);
	};

	class Edit: public SimpleControl
	{
	public:
		Edit (HWND hwndParent, int id)
			: SimpleControl (hwndParent, id)
		{}

		void SetString (char* buf)
		{
			::SendMessage (Hwnd (), WM_SETTEXT, 0, (LPARAM) buf);
		}

		// code is the HIWORD (wParam)
		static bool IsChanged (int code)
		{ 
			return code == EN_CHANGE;
		}

		int GetLen ()
		{
			return ::SendMessage (Hwnd (), WM_GETTEXTLENGTH, 0, 0);
		}

		void GetString (char* buf, int len)
		{
			::SendMessage (Hwnd (), WM_GETTEXT, (WPARAM) len, (LPARAM) buf);
		}

		std::string GetString ()
		{
			std::string str;
			unsigned len = GetLen ();
			str.resize (len + 1);
			GetString (&str [0], len + 1);
			str.resize (len);
			return str;
		}	

		void Select ()
		{
			::SendMessage (Hwnd (), EM_SETSEL, 0, -1);
		}
	};

	class SubEdit: public Edit
	{
	public:
		SubEdit (HWND hwndParent, int id);

		static LRESULT CallDefault (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
		{
			// Gives compile error if not compiled with STRICT!
			return ::CallWindowProc (This (hWnd)->_wprocOld, hWnd, message, wParam, lParam);
		}

		static SubEdit* This (HWND hwnd)
		{
			return (SubEdit*) ::GetWindowLong (hwnd, GWL_USERDATA);
		}

	private:
		ProcPtr _wprocOld;
	};

	class RadioButton: public SimpleControl
	{
	public:
		RadioButton (HWND hwndParent, int id)
			: SimpleControl (hwndParent, id)
		{}

		// code is the HIWORD (wParam)
		static bool IsClicked (int code)
		{
			return code == BN_CLICKED;
		}
		bool IsChecked ()
		{
			return ::SendMessage (Hwnd (), BM_GETCHECK, 0, 0) == BST_CHECKED;
		}
		void Check()
		{
			::SendMessage (Hwnd (), BM_SETCHECK, (WPARAM)1, 0);
			::SetFocus (Hwnd ());
		}
		void UnCheck()
		{
			::SendMessage (Hwnd (), BM_SETCHECK, 0, 0);
		}
		void Disable()
		{
			::EnableWindow (Hwnd (), FALSE);
		}
	};

	class Button: public SimpleControl
	{
	public:
		Button (HWND hwndParent, int id)
			: SimpleControl (hwndParent, id)
		{}

		void Push ()
		{
			::SendMessage (Hwnd (), BM_SETSTATE, (WPARAM)1, 0);
		}
		void MakeDefault (bool yes = true)
		{
			SetStyle (yes? BS_DEFPUSHBUTTON: BS_PUSHBUTTON);
		}
		void SetStyle (unsigned style)
		{
			::SendMessage (Hwnd (), BM_SETSTYLE, style, MAKELPARAM (TRUE, 0));
		}

		void Pop ()
		{
			::SendMessage (Hwnd (), BM_SETSTATE, 0, 0);
		}
		void SetText (char const * text)
		{
			::SendMessage (Hwnd (), WM_SETTEXT, 0, (LPARAM) text);
		}
		void SetIcon (HICON hIcon)
		{
			::SendMessage (Hwnd (), BM_SETIMAGE, (WPARAM)IMAGE_ICON,
							(LPARAM)(HANDLE)hIcon);
		}
	};

	class ScrollBar: public SimpleControl
	{
	public:
		ScrollBar (HWND hwndParent, int id)
			: SimpleControl (hwndParent, id), _iPos(0), _cPos(0), _cPage (0)
		{}

		void Init (unsigned range, unsigned cPage)
		{
			_cPos = range;
			_cPage = cPage;  // page increment
			SetRange (range);
		}

		// Usage: Command (LOWORD (wParam), HIWORD (wParam));
		void Command (unsigned code, unsigned pos);

		unsigned GetPos ()
		{
			return ::GetScrollPos (_hWnd, SB_CTL);
		}

		void SetPos (unsigned i)
		{
			_iPos = i;
			::SetScrollPos (_hWnd, SB_CTL, i, TRUE);
		}

	protected:

		void SetRange (int range)
		{
			::SetScrollRange (_hWnd, SB_CTL, 0, range - 1, FALSE);
		}

		unsigned	_iPos;
		unsigned	_cPos;
		unsigned	_cPage;
	};

	class ScrollBarMap: public ScrollBar
	{
	public:
		ScrollBarMap (HWND hwndParent, int id)
			: ScrollBar (hwndParent, id)
		{}
		void AddValue (int val)
		{
			_values.push_back (val);
			++_cPos;
		}

		void Init (unsigned range, unsigned cPage)
		{
			assert (range == _cPos);
			ScrollBar::Init (range, cPage);
		}
		void Clear () 
		{
			_values.clear ();
			_cPos = 0;
			_iPos = 0;
		}
		int  GetValue () const 
		{
			assert (_iPos < _values.size ());
			return _values [_iPos]; 
		}
	protected:
		std::vector<int> _values;
	};
}
#endif
