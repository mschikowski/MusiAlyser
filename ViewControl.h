#if !defined VIEWCONTROL_H
#define VIEWCONTROL_H
//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------
#include "Controller.h"
#include "MetaCtrl.h"
#include <cassert>
class ViewFreq;

class WaveController: public Win::Controller
{
public:
	WaveController () {}
};

class FreqController: public Win::Controller
{
public:
	FreqController (ViewFreq & view)
		: _view (view)
	{}
	bool OnPaint () throw ();
	bool OnSize (int width, int height, int flag) throw ();
private:
	ViewFreq & _view;
};
#endif
