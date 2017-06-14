#if !defined (WINCLASS_H)
#define WINCLASS_H
//
// (c) Reliable Software-2002
//

#include "Procedure.h"

namespace Win
{
	class ClassMaker : public WNDCLASSEX
	{
	public:
		ClassMaker (char const * className, HINSTANCE hInst, WNDPROC WndProc = Win::Procedure);
		void SetBgSysColor (int sysColor)
		{
			hbrBackground = (HBRUSH) (sysColor + 1);
		}
		void SetBgBrush (HBRUSH hbr)
		{
			hbrBackground = hbr;
		}
		void SetSizeRedraw ()
		{
			// Redraw the whole window every time size changes
			style |= (CS_HREDRAW | CS_VREDRAW);
		}
		void Register ();
	private:
		void SetDefaults (WNDPROC wndProc, HINSTANCE hInst);
	};
};

#endif
