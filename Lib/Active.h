#if !defined ACTIVE_H
#define ACTIVE_H
//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------
#include "Thread.h"
#include <windows.h>

class ActiveObject
{
public:
    ActiveObject ();
    virtual ~ActiveObject () {}
    void Kill ();

protected:
    virtual void InitThread () = 0;
    virtual void Run () = 0;
    virtual void FlushThread () = 0;

    static DWORD WINAPI ThreadEntry (void *pArg);
    int             _isDying;
	Win::Thread          _thread;
};

// Last thing in the constructor of a class derived from 
// ActiveObject you must call
//    _thread.Resume ();
// Inside the loop the Run method you must keep checking _isDying
//    if (_isDying)
//         return;

#endif