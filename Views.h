#if !defined VIEWS_H
#define VIEWS_H
//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------
#include "ViewControl.h"
#include "Canvas.h"

class Fft;
class SampleIter;

class ViewFreq
{
public:
    ViewFreq (HWND hwndParent);
	~ViewFreq () { ::DestroyWindow (_win); }
    void Update (Fft const & fftTransformer);
    void Clear ();
    void Fake ();
	HWND Hwnd () { return _win; }
	void Init (int points, int point1kHz);
	void PaintScale (Win::PaintCanvas & canvas);
	void SetWidth (int width) { _width = width; }
private:
	FreqController	_ctrl;
	HWND			_win;

    int _xRecord;
	int	_width;
	int	_height;

	int _points2; // Half the number of points = max freq
	int	_point1kHz;	// Point corresponding to 1kHz
};

class ViewWave
{
public:
    ViewWave (HWND hwndParent);
	~ViewWave () { ::DestroyWindow (_win); }
	void Init (unsigned samples);
	void Update (std::vector<int> const & data);
	HWND Hwnd () { return _win; }
private:
	WaveController	_ctrl;
	HWND			_win;

    Win::PolyLine	_poly;
    Win::Pen		_penGreen;

	int				_width;
	int				_height;
};

#endif