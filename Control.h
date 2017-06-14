#if	!defined control_h
#define	control_h
//------------------------------------
//	Reliable Software, (c) 1996-2002
//------------------------------------
#include "Views.h"
#include "Painter.h"
#include "Dir.h"
#include <windows.h>
#include <cassert>

class DlgController
{
public:
	DlgController(HWND hwnd);
	~DlgController ();
	bool OnCommand (int	id,	int	code);
	void OnScroll (int cmd,	int	pos); 

	void ReInit	();
	void Stop ();

private:
	SourceType ValidateFile (std::string & name, SourceType type);
	void SelectFile (SourceType type);
	void ListCurrentDir ();
	void Display (char const * errMsg);
	void SetSource (SourceType src, char const * name = 0)	
	{
		_painter.SetSource (src, name);
	}
	void InitScrollPositions ();
	void UpdateControls	();
private:
	HWND				_hWnd;
	HINSTANCE			_hInst;
	CurrentDir			_currentDir;

	bool				_isStopped;

	unsigned			_bitsPerSample;
	unsigned			_samplesPerSecond;
	unsigned			_fftPoints;
	unsigned			_samplesPerBuf;

	Win::Button			_buttonStart;
	Win::Button			_buttonAbout;
	Win::EditReadOnly	_edit;
	Win::Combo			_comboFreq;
	Win::Combo			_comboPoints;
	Win::RadioButton	_radio8;
	Win::RadioButton	_radio16;
	Win::ScrollBarMap	_scroll;
	Win::RadioButton	_mike;
	Win::RadioButton	_wave;
	Win::RadioButton	_image;
	Win::Button			_buttonFile;
	Win::Combo			_editFile;

	Painter				_painter;
};

#endif
