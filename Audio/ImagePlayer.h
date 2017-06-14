#if !defined (IMAGEPLAYER_H)
#define IMAGEPLAYER_H
// (c) Reliable Software 2002
#include "WaveOut.h"
#include "Controller.h"
#include <vector>

class BitmapWindowSrc;

class BitmapCtrl: public Win::Controller
{
public:
	enum { SCALE = 2 };
public:
	BitmapCtrl (BitmapWindowSrc & src)
		: _bmp (0), _src (src)
	{}
	void SetBitmap (HBITMAP bmp, int width, int height) 
	{ 
		_bmp = bmp;
		_width = width;
		_height = height;
	}
	bool OnPaint ();
private:
	BitmapWindowSrc & _src;
	HBITMAP _bmp;
	int _width;
	int _height;
};

class BitmapWindowSrc
{
public:
	BitmapWindowSrc (HWND winOwner, char const * fileName);
	~BitmapWindowSrc ();
	void ReadPixels (char * buf, long & size);
	void Rewind ();
private:
	void DisplayBitmap (HWND winOwner);

	BitmapCtrl	_ctrl;
	HWND		_win;
	HBITMAP		_bitmap;
	long		_cur;
	std::vector<char> _buf;
};

class ImagePlayer: public Wave::OutDevice
{
public:
	ImagePlayer (HWND winOwner, char const * fileName, long samplesPerSec)
		: _bitmapWin (winOwner, fileName)
	{
		Wave::Format format (8, 1, samplesPerSec);
		SetFormat (format);
	}
	void InitBuffer (char * buf, unsigned size);
private:
	BitmapWindowSrc _bitmapWin;
};

#endif
