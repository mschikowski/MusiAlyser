#if !defined THREAD_H
#define THREAD_H
//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------
#include <windows.h>

namespace Win
{
	class Thread
	{
	public:
		Thread (DWORD (WINAPI * pFun) (void* arg), void* pArg)
		{
			_handle = ::CreateThread (
				0, // Security attributes
				0, // Stack size
				pFun, 
				pArg, 
				CREATE_SUSPENDED, 
				&_tid);
		}
		~Thread () { CloseHandle (_handle); }
		void Resume () { ResumeThread (_handle); }
		void WaitForDeath ()
		{
			::WaitForSingleObject (_handle, 2000);
		}
	private:
		HANDLE _handle;
		DWORD  _tid;     // thread id
	};

	class CritSect
	{
		friend class Lock;
	public:
		CritSect () { ::InitializeCriticalSection (&_critSection); }
		~CritSect () { ::DeleteCriticalSection (&_critSection); }
	private:
		void Acquire () 
		{ 
			::EnterCriticalSection (&_critSection);
		}
		void Release () 
		{ 
			::LeaveCriticalSection (&_critSection);
		}

		CRITICAL_SECTION _critSection;
	};

	class Lock 
	{
	public:
		// Acquire the state of the semaphore
		Lock (CritSect & mutex ) 
			: _critSect(mutex) 
		{
			_critSect.Acquire();
		}
		// Release the state of the semaphore
		~Lock ()
		{
			_critSect.Release();
		}
	private:
		CritSect & _critSect;
	};

	class Event
	{
	public:
		Event ()
		{
			// start in non-signaled state (open)
			// auto reset (block) after every Wait
			_handle = ::CreateEvent (0, FALSE, FALSE, 0);
		}

		~Event ()
		{
			::CloseHandle (_handle);
		}

		// put into signaled state
		void Release () { ::SetEvent (_handle); }
		void Block () { ::ResetEvent (_handle); }
		void Wait ()
		{
			// Wait until event is in signaled (green) state
			::WaitForSingleObject(_handle, INFINITE);
		}
		HANDLE Get () { return _handle; }
	private:
		HANDLE _handle;
	};
}
#endif