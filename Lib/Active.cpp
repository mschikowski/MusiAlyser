#include "Active.h"
//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------

using namespace Win;

// The constructor of the derived class
// should call
//    _thread.Resume ();
// at the end of construction

ActiveObject::ActiveObject ()
: _isDying (0),
#pragma warning(disable: 4355) // 'this' used before initialized
  _thread (ThreadEntry, this)
#pragma warning(default: 4355)
{
}

// FlushThread must reset all the events
// on which the thread might be waiting.

void ActiveObject::Kill ()
{
    _isDying++;
    FlushThread ();
    // Let's make sure it's gone
    _thread.WaitForDeath ();
}

DWORD WINAPI ActiveObject::ThreadEntry (void * pArg)
{
    ActiveObject * pActive = (ActiveObject *) pArg;
    pActive->InitThread ();
    pActive->Run ();
    return 0;
}
