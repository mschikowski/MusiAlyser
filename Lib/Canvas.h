#if !defined CANVAS_H
#define CANVAS_H
//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------
#include <windows.h>
#include <cassert>
#include <vector>

namespace Win
{
	class Point: public POINT
	{
	public:
		Point (int xinit = 0, int yinit = 0)
		{
			x = xinit;
			y = yinit;
		}
		void operator-= (Point p)
		{
			x -= p.x;
			y -= p.y;
		}
	};

	class Rectangle: public RECT
	{
	public:
		Rectangle () {}
		Rectangle (int xleft, int ytop, int xright, int ybottom)
		{
			left = xleft;
			top = ytop;
			right = xright;
			bottom = ybottom;
		}
		int Width () const { return right - left; }
		int Height () const { return bottom - top; }
	};

	class ClientRect: public Rectangle
	{
	public:
		ClientRect (HWND hwnd)
		{
			::GetClientRect (hwnd, this);
		}
	};

	class DowRect: public Rectangle
	{
	public:
		DowRect (HWND hwnd)
		{
			::GetWindowRect (hwnd, this);
		}
	};

	class Canvas
	{
	public:
		operator HDC () 
		{ 
			return _hdc; 
		}

		void Point (int x, int y, COLORREF color)
		{
			::SetPixel (_hdc, x, y, color);
		}

		COLORREF GetPixel (int x, int y)
		{
			return ::GetPixel (_hdc, x, y);
		}

		void Line (int x1, int y1, int x2, int y2)
		{
			::MoveToEx (_hdc, x1, y1, 0);
			::LineTo (_hdc, x2, y2);
		}

		void ClearWhite (RECT & rect)
		{
			::FillRect (_hdc, &rect, static_cast<HBRUSH> (::GetStockObject (WHITE_BRUSH)));
		}

		void ClearBlack ( RECT& rect )
		{
			::FillRect (_hdc, &rect, static_cast<HBRUSH> (::GetStockObject (BLACK_BRUSH)));
		}

		void SelectObject (HGDIOBJ obj)
		{
			::SelectObject (_hdc, obj);
		}
		void Text (char const * buf, int x, int y)
		{
			::TextOut (_hdc, x, y, buf, strlen (buf));
		}
		void GetCharSize (int& cxChar, int& cyChar)
		{
			TEXTMETRIC tm;
			::GetTextMetrics (_hdc, &tm);
			cxChar = tm.tmAveCharWidth;
			cyChar = tm.tmHeight + tm.tmExternalLeading;
		}
	protected:
		Canvas(HDC hdc) :_hdc(hdc) {}

		HDC  _hdc;
	};

	// Use for painting after WM_PAINT

	class PaintCanvas: public Canvas
	{
	public:
		PaintCanvas (HWND hwnd)
			: Canvas (::BeginPaint (hwnd, &_paint)),
			_hwnd (hwnd)
		{}

		~PaintCanvas ()
		{
			::EndPaint(_hwnd, &_paint);
		}
		Rectangle const & GetRectangle () const
		{
			return reinterpret_cast<Rectangle const &> (_paint.rcPaint);
		}
	protected:
					 
		PAINTSTRUCT _paint;
		HWND		_hwnd;
	};

	// Device Context
	// Use for painting other than WM_PAINT

	class UpdateCanvas: public Canvas
	{
	public:
		UpdateCanvas (HWND hwnd)
			: Canvas (::GetDC(hwnd)),
			_hwnd(hwnd)
		{}
		
		~UpdateCanvas ()
		{
			::ReleaseDC (_hwnd, _hdc);
		}

	protected:

		HWND _hwnd;
	};

	class MemCanvas: public Canvas
	{
	public:
		MemCanvas (HDC hdc) 
			: Canvas (::CreateCompatibleDC (hdc))
		{}
		~MemCanvas ()
		{
			::DeleteDC(_hdc); 
		}
	};

	class ScreenCanvas: public Canvas
	{
	public:
		ScreenCanvas ()
			: Canvas (::CreateDC ("DISPLAY", 0, 0, 0))
		{}
		~ScreenCanvas ()
		{
			::DeleteDC(_hdc); 
		}
	};

	class StockObject
	{
	public:
		StockObject (HDC hdc, int type)
		: _hdc(hdc)
		{
			_hObjOld = ::SelectObject (_hdc, GetStockObject (type));
		}

		~StockObject ()
		{
			::SelectObject (_hdc, _hObjOld );
		}
	private:
		HGDIOBJ  _hObjOld;
		HDC 	 _hdc;
	};

	class WhitePen : public StockObject
	{
	public:
		WhitePen (HDC hdc): StockObject (hdc, WHITE_PEN) {}
	};

	class BlackPen : public StockObject
	{
	public:
		BlackPen (HDC hdc): StockObject (hdc, BLACK_PEN) {}
	};

	class Pen
	{
	public:
		Pen (COLORREF color)
		{
			_hPen = ::CreatePen (PS_SOLID, 0, color);
		}
		~Pen ()
		{
			::DeleteObject (_hPen);
		}
		operator HPEN () { return _hPen; }
	private:
		HPEN	_hPen;
	};

	class PenHolder
	{
	public:
		PenHolder (HDC hdc, HPEN hPen)
			: _hdc (hdc)
		{
			_hPenOld = (HPEN)::SelectObject (_hdc, hPen); 
		}
		~PenHolder ()
		{
			::SelectObject (_hdc, _hPenOld);
		}
	private:
		HDC 	_hdc;
		HPEN	_hPenOld;
	};

	class ColorPen
	{
	public:
		ColorPen (HDC hdc, COLORREF color)
			: _hdc (hdc)
		{
			_hPen = ::CreatePen (PS_SOLID, 0, color);
			_hPenOld = (HPEN)::SelectObject (_hdc, _hPen); 
		}
		~ColorPen ()
		{
			::SelectObject (_hdc, _hPenOld);
			::DeleteObject (_hPen);
		}
	private:
		HDC 	_hdc;
		HPEN	_hPen;
		HPEN	_hPenOld;
	};

	class ColorBrush
	{
	public:
		ColorBrush (HDC hdc, COLORREF color)
			: _hdc (hdc)
		{
			_hBrush = ::CreateSolidBrush (color);
			_hBrushOld = (HBRUSH)::SelectObject (_hdc, _hBrush); 
		}
		~ColorBrush ()
		{
			::SelectObject (_hdc, _hBrushOld);
			::DeleteObject (_hBrush);
		}
	private:
		HDC 	_hdc;
		HBRUSH	_hBrush;
		HBRUSH	_hBrushOld;
	};

	class TextColor
	{
	public:
		TextColor (Win::Canvas canvas, COLORREF color)
			: _canvas (canvas), 
			  _oldColor (::SetTextColor (_canvas, color))
		{}
		~TextColor ()
		{
			::SetTextColor (_canvas, _oldColor);
		}
	private:
		Win::Canvas	_canvas;
		COLORREF	_oldColor;
	};

	class TextBkg
	{
	public:
		TextBkg (Win::Canvas canvas, COLORREF color)
			: _canvas (canvas), 
				_oldColor (::SetBkColor (_canvas, color))
		{}
		~TextBkg ()
		{
			::SetBkColor (_canvas, _oldColor);
		}
	private:
		Win::Canvas	_canvas;
		COLORREF	_oldColor;
	};


	class PolyLine
	{
	public:
		void Clear () { _points.clear (); }
		void Add (unsigned i, int x, int y)
		{
			Win::Point p (x, y);
			_points.push_back (p);
		}

		void Paint (Canvas & canvas)
		{
			::Polyline (canvas, &_points [0], _points.size ());
		}

		unsigned Points () const { return _points.size (); }
	private:
		std::vector<Win::Point> _points;
	};
}

#endif
