//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------
#include "ViewControl.h"
#include "Views.h"

bool FreqController::OnPaint () throw ()
{
	Win::PaintCanvas canvas (Handle ());
	_view.PaintScale (canvas);
	return true;
}

bool FreqController::OnSize (int width, int height, int flag) throw ()
{
	_view.SetWidth (width);
	return true;
}

