// (c) Reliable Software 2002
#include "ImagePlayer.h"
#include "WinMaker.h"
#include "Canvas.h"
#include <complex>

bool BitmapCtrl::OnPaint ()
{
	Win::PaintCanvas canvas (_h);
	Win::MemCanvas memCanvas (canvas); // compatible memory canvas
	memCanvas.SelectObject (_bmp);
	::StretchBlt (canvas, 0, 0, _width/SCALE, _height/SCALE, 
				memCanvas, 0, 0, _width, _height, SRCCOPY);
	int x = _width/(2 * SCALE);
	Win::ColorPen pen (canvas, RGB (255, 0, 0));
	canvas.Line (x - 1, 0, x - 1, _height/SCALE - 1);
	//canvas.Line (x + 1, 0, x + 1, _height/SCALE - 1);
	return true;
}

BitmapWindowSrc::BitmapWindowSrc (HWND winOwner, char const * fileName)
#pragma warning(disable: 4355) // 'this' used before initialized
    : _ctrl (*this),
#pragma warning(default: 4355)
	_cur (0), _bitmap (0), _win (0)
{
	_bitmap = reinterpret_cast<HBITMAP> (::LoadImage (0, fileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
	DisplayBitmap (winOwner);
}

void BitmapWindowSrc::DisplayBitmap (HWND winOwner)
{
	HINSTANCE hInst = reinterpret_cast<HINSTANCE> (::GetWindowLong (winOwner,	GWL_HINSTANCE));

	BITMAP bm;
	::GetObject (_bitmap, sizeof (bm), &bm);
	int width = bm.bmWidth;
	int height = bm.bmHeight;

	_ctrl.SetBitmap (_bitmap, width, height);

	Win::Maker winMaker ("WinViewClass", hInst);
	winMaker.SetOwner (winOwner);
	//winMaker.AddHScrollBar ();
	//winMaker.AddVScrollBar ();
	width /= BitmapCtrl::SCALE;
	height /= BitmapCtrl::SCALE;
	width += /*::GetSystemMetrics (SM_CXVSCROLL) +*/ 2 * ::GetSystemMetrics (SM_CXFIXEDFRAME), 
	height += /*::GetSystemMetrics (SM_CYHSCROLL) +*/ 2 * ::GetSystemMetrics (SM_CYFIXEDFRAME) + ::GetSystemMetrics (SM_CYCAPTION);


	Win::DowRect ownerRect (winOwner);


	Win::Rectangle rect;
	::SystemParametersInfo (SPI_GETWORKAREA, 0, &rect, 0);
	int x = ownerRect.right;
	int y = ownerRect.top;
	if (x + width + 10 > rect.Width ())
		x = rect.right - width - 10;
	if (y + height + 10 > rect.Height ())
		y = rect.bottom - height - 10;
	winMaker.SetPosition (x, y, width, height);
	_win = winMaker.Create (&_ctrl, "Bitmap");

	Win::ScreenCanvas screenCanvas;
	Win::MemCanvas memCanvas (screenCanvas);
	memCanvas.SelectObject (_bitmap);
	int xOff = bm.bmWidth/2;
	_buf.resize (bm.bmHeight);
	for (int i = 0; i < bm.bmHeight; ++i)
	{
		COLORREF color = memCanvas.GetPixel (xOff, i);
		_buf [i] = GetRValue (color);
	}
	::ShowWindow (_win, SW_SHOWNORMAL);
}

BitmapWindowSrc::~BitmapWindowSrc ()
{
	if (_win)
		::DestroyWindow (_win);
	if (_bitmap)
		::DeleteObject (_bitmap);
}


void BitmapWindowSrc::ReadPixels (char * buf, long & size)
{
	size = min(size, _buf.size () - _cur);
	if (size != 0)
	{
		for (long i = 0; i < size; ++i)
			buf [i] = _buf [_cur + i];
		_cur += size;
	}
}

void BitmapWindowSrc::Rewind ()
{
	_cur = 0;
}

void ImagePlayer::InitBuffer (char * buf, unsigned size)
{
	long totalSizeRead = 0;
	for (;;)
	{
		long sizeRead = size - totalSizeRead;
		_bitmapWin.ReadPixels (buf + totalSizeRead, sizeRead);
		totalSizeRead += sizeRead;
		if (static_cast<unsigned> (totalSizeRead) < size)
			_bitmapWin.Rewind ();
		else
			break;
	}
};

