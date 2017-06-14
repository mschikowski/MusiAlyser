#include "Views.h"
//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------
#include "Fft.h"
#include "Canvas.h"
#include "WinClass.h"
#include "WinMaker.h"

COLORREF ColorScale [] = 
{
	RGB(0, 0, 128), // dk blue
    RGB(0, 0, 255), // lt blue
    RGB(0, 255, 0), // lt green
    RGB(0, 255, 0),
    RGB(255, 255, 0), // lt yellow
    RGB(255, 255, 0),
    RGB(255, 255, 0),
    RGB(255, 255, 0),
    RGB(255, 128, 0), // orange
    RGB(255, 128, 0),
    RGB(255, 128, 0),
    RGB(255, 128, 0),
    RGB(255, 128, 0),
    RGB(255, 128, 0),
    RGB(255, 128, 0),
    RGB(255, 128, 0),
    RGB(255, 0, 0) // lt red
};

COLORREF MapColor (int s)
{
	s /= 16;
	if (s > sizeof (ColorScale) - 1)
		s = sizeof (ColorScale) - 1;
	return ColorScale [s];
}

// ViewFreq -------------

static int SCALE_WIDTH = 30;
static int NOTCH_1 = 5;
static int NOTCH_2 = 10;
static int NOTCH_3 = SCALE_WIDTH;

ViewFreq::ViewFreq (HWND hwndParent) 
#pragma warning(disable: 4355) // 'this' used before initialized
    : _ctrl (*this),
#pragma warning(default: 4355)
	  _xRecord (SCALE_WIDTH + 1),
	  _points2 (0),
	  _point1kHz (0)
{
	HINSTANCE hInst = reinterpret_cast<HINSTANCE> (::GetWindowLong (hwndParent, GWL_HINSTANCE));

	Win::Maker viewMaker ("WinViewClass", hInst);
	viewMaker.SetOwner (hwndParent);
	viewMaker.AddResize ();

	// Position below owner
	Win::DowRect ownerRect (hwndParent);
	viewMaker.SetPosition (ownerRect.left, ownerRect.bottom, 
		ownerRect.Width () + 300, ownerRect.Height ());
	_win = viewMaker.Create (&_ctrl, "Frequency Spectrum");
	::ShowWindow (_win, SW_SHOWNORMAL);

    Win::ClientRect rect (Hwnd ());
	_width = rect.right - rect.left;
	_height = rect.bottom - rect.top;
}

void ViewFreq::Clear ()
{
    if (Hwnd ())
    {
        Win::UpdateCanvas canvas (Hwnd ());
        Win::ClientRect rect (Hwnd ());
		rect.left += SCALE_WIDTH;
        canvas.ClearWhite (rect);
    }
}

void ViewFreq::Init (int points, int point1kHz)
{
	_points2 = points / 2;
	_point1kHz = point1kHz;

	Win::DowRect winRect (Hwnd ());
	Win::ClientRect clientRect (Hwnd ());
	// show up to _points/4 (half of Nyquist frequency)
	int delta = _points2 - clientRect.Height ();
	winRect.bottom += delta;
	::InvalidateRect (Hwnd (), 0, TRUE);
	::MoveWindow (Hwnd (), winRect.left, winRect.top, winRect.Width (), winRect.Height (), TRUE);
	_height += delta;
}

void ViewFreq::PaintScale (Win::PaintCanvas & canvas)
{
	if (_points2 == 0)
		return;
	Win::Rectangle const & updRect = canvas.GetRectangle ();
	if (updRect.left > SCALE_WIDTH)
		return;
	Win::Rectangle rect (1, 1, SCALE_WIDTH, _height - 1);
    canvas.ClearBlack (rect);
    int x0 = rect.right;
    int y0 = rect.bottom - 1;

	Win::WhitePen pen (canvas);
	Win::TextColor textColor (canvas, RGB (255, 255, 255)); // white text
	Win::TextBkg textBkg (canvas, RGB (0, 0, 0)); // on black
	int charX, charY;
	canvas.GetCharSize (charX, charY);
	// Draw vertical line
    canvas.Line (x0, rect.top, x0, rect.bottom);
    // draw a notch every 100 Hz
    int s1000 = _point1kHz;
	assert (s1000 != 0);
	int s100 = _point1kHz / 10;
    int maxS = _points2;
    int count = 0;
    int xx = x0;
	int bigNotchCount = 0;
    if (s100 < 3)
    {
        for (int s = 0; s < maxS; s += s1000)
        {
            int y = y0 - s;
            if (y <= 0)
                break;
			if (count % 10 == 0) // very big notch
			{
				canvas.Line (x0 - NOTCH_3, y, x0, y);
				if (bigNotchCount == 1)
				{
					canvas.Text ("10k", 0, y - charY/2);
				}
				++bigNotchCount;
			}
			else
			{
	            canvas.Line (x0 - NOTCH_2, y, x0, y);
			}
            count++;
        }
    }
    else
    {
        for (int s = 0; s < maxS; s += s100)
        {
            int y = y0 - s;
            if ( y <= 0 )
                break;
			if (count % 10 == 0) // big notch
			{
				canvas.Line (x0 - NOTCH_2, y, x0, y);
				if (bigNotchCount == 1)
				{
					canvas.Text ("1k", 0, y - charY/2);
				}
				++bigNotchCount;
			}
			else
			{
	            canvas.Line (x0 - NOTCH_1, y, x0, y);
			}
            count++;
        }
    }
}

void ViewFreq::Update (Fft const &fftTransformer)
{
    Win::UpdateCanvas canvas (Hwnd ());

    {
        // Erase background for current spectrum
        Win::BlackPen pen(canvas);
        canvas.Line (_xRecord, 0, _xRecord, _height);
        canvas.Line (_xRecord + 1, 0, _xRecord + 1, _height);
    }

	int pts = fftTransformer.Points() / 2;
    for (int i = 0; i < pts; i++ )
    {
        int s = static_cast<int> (fftTransformer.GetIntensity(i)) / 128;
        
        if (s > 8)
        {
	        COLORREF color = MapColor (s);

            canvas.Point (_xRecord, _height - i - 1, color);
            canvas.Point (_xRecord + 1, _height - i - 1, color);
        }
    }

    _xRecord += 2;
    if (_xRecord >= _width)
        _xRecord = SCALE_WIDTH + 1;

    // Draw vertical mark
    {
        Win::WhitePen pen(canvas);
        canvas.Line (_xRecord, 0, _xRecord, _height);
    }
}

void ViewFreq::Fake ()
{
    Win::UpdateCanvas canvas (Hwnd ());
    Win::ClientRect rect (Hwnd ());

    {
        // Erase background for current spectrum
        Win::BlackPen pen (canvas);
        canvas.Line (_xRecord, 0, _xRecord, _height);
        canvas.Line (_xRecord + 1, 0, _xRecord + 1, _height);
    }


    _xRecord += 2;
    if (_xRecord >= _width)
        _xRecord = 0;

    {
        // Draw white vertical mark
        Win::WhitePen pen(canvas);
        canvas.Line (_xRecord, 0, _xRecord, _height);
    }
}

// ViewWave ---------------

ViewWave::ViewWave (HWND hwndParent)
    : _penGreen (RGB (0, 255, 128))
{
	HINSTANCE hInst = reinterpret_cast<HINSTANCE> (::GetWindowLong (hwndParent, GWL_HINSTANCE));

	Win::Maker viewMaker ("WinViewClass", hInst);
	viewMaker.SetOwner (hwndParent);
	// Position to the right of owner
	Win::DowRect ownerRect (hwndParent);
	viewMaker.SetPosition (ownerRect.right, ownerRect.top, 300, ownerRect.Height ());
	_win = viewMaker.Create (&_ctrl, "Wave");
	::ShowWindow (_win, SW_SHOWNORMAL);

    Win::ClientRect rect (Hwnd ());
	_width = rect.right - rect.left;
	_height = rect.bottom - rect.top;
}

void ViewWave::Init (unsigned samples)
{
	Win::DowRect winRect (Hwnd ());
	Win::ClientRect clientRect (Hwnd ());
	int delta = samples - clientRect.Width ();
	winRect.right += delta;
	::MoveWindow (Hwnd (), winRect.left, winRect.top, winRect.Width (), winRect.Height (), TRUE);
	_width += delta;
}

void ViewWave::Update (std::vector<int> const & data)
{
    Win::UpdateCanvas canvas (Hwnd ());
    Win::ClientRect rect (Hwnd ());
    canvas.ClearBlack (rect);
	_poly.Clear ();

	int cMaxPoints = data.size ();
    for (int i = 0; i != cMaxPoints; ++i)
    {
        int s = data [i]/128 + (_height - 1) / 2;

        if (s < 0)
            _poly.Add (i, i, -1);
        else if (s >= _height)
            _poly.Add (i, i, _height);
        else
            _poly.Add (i, i, s);
    }

    Win::PenHolder pen (canvas, _penGreen);
    _poly.Paint (canvas);
}
