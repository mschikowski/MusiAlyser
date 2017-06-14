#if !defined WINEX_H
#define WINEX_H
//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------
#include <windows.h>

namespace Win
{
	inline void ClearError () { ::SetLastError (0); }

	class Exception
	{
	public:
		Exception (char const * msg)
			: _err (::GetLastError()), _msg (msg)
		{}
		Exception (char const * msg, DWORD err)
			: _err (err), _msg (msg)
		{}
		DWORD GetError() const { return _err; }
		char const * GetMessage () const { return _msg; }
	private:
		DWORD _err;
		char const * _msg;
	};
}
#endif