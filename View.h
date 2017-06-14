#if !defined view_h
#define view_h
//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------
#include "Canvas.h"

class Fft;
class SampleIter;
class Painter;

class View
{
public:
    View ();
    void Paint (HWND hwnd, Painter & disp);
private:
    void PaintTimeScale (HWND hwnd, Painter & disp);
    void PaintFreqScale (HWND hwnd, Painter & disp);

    StaticEnhMetafileControl    _scaleFreq;
    StaticEnhMetafileControl    _scaleTime;
};

#endif
