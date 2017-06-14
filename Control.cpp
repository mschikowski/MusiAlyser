//------------------------------------
//	Reliable Software, (c) 1996-2002
//------------------------------------
#include "Control.h"
#include "Procedure.h"
#include "Painter.h"
#include "Params.h"
#include "Views.h"
#include "WinEx.h"
#include "resource.h"
#include "AudioFile.h"
#include "WaveFormat.h"
#include "WaveIn.h"
#include "Dir.h"
#include "FileGet.h"
#include <cstring>

// Helper functions

bool Is16BitSampling (DWORD	format);
bool Is11_025kHz (DWORD	format);
bool Is22_05kHz	(DWORD format);
bool Is44_1kHz (DWORD format);

char const * GetFileFilter (SourceType type)
{
	// For instance "Text (*.txt *.me)\0*.txt;*.me\0"
	if (type == SRC_WAVE)
		return "Wave file (*.wav)\0*.wav\0";
	if (type == SRC_IMAGE)
		return "Bitmap (*.bmp)\0*.bmp\0";
	if (type == SRC_ANY)
		return "Media (*.wav *.bmp)\0*.wav\0*.bmp\0";
	return 0;
};

BOOL CALLBACK AboutDlgProc
   (HWND hwnd, UINT	message, UINT wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD (wParam))
		{
		case IDC_BUTTON_RELISOFT:
			::ShellExecute (hwnd, 0, "http://www.relisoft.com", 0, 0, SW_SHOWNORMAL);
			return TRUE;
		case IDOK:
		case IDCANCEL:
			::EndDialog	(hwnd, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

DlgController::DlgController (HWND hwnd) 
   :_hWnd (hwnd),
	_hInst (reinterpret_cast<HINSTANCE>	(::GetWindowLong (hwnd,	GWL_HINSTANCE))),

	_buttonStart (hwnd, IDC_BUTTON_START),
	_buttonAbout (hwnd, IDC_BUTTON_ABOUT),
	_radio8	(hwnd, IDC_RADIO_8_BITS),
	_radio16 (hwnd,	IDC_RADIO_16_BITS),
	_scroll	(hwnd, IDC_SCROLLBAR),
	_edit (hwnd, IDC_EDIT_FFTS),
	_comboFreq (hwnd, IDC_COMBO_SAMPLING),
	_comboPoints (hwnd,	IDC_COMBO_POINTS),
	_mike (hwnd, IDC_RADIO_MIKE),
	_wave (hwnd, IDC_RADIO_FILE),
	_image (hwnd, IDC_RADIO_IMAGE),
	_buttonFile (hwnd, IDC_BUTTON_FILE),
	_editFile (hwnd, IDC_EDIT_FILE),

	_isStopped (true),
	_bitsPerSample (16),
	_samplesPerSecond (Wave::BASE_SAMPLES_SEC),
	_fftPoints (FFT_POINTS * 4),
	_samplesPerBuf (FFT_POINTS * 4),
	_painter (hwnd)
{
	// Attach icon to main dialog
	HICON hIcon	= ::LoadIcon (_hInst, MAKEINTRESOURCE (ICO_FFT));
	::SendMessage (hwnd, WM_SETICON, WPARAM	(ICON_SMALL), LPARAM (hIcon));

	// Display RS icon over	the	button
	hIcon =	::LoadIcon (_hInst,	MAKEINTRESOURCE	(ICO_RS));
	_buttonAbout.SetIcon (hIcon);

	ListCurrentDir ();

	if (::waveInGetNumDevs () == 0)
		throw Win::Exception ("No sound	card installed!");

	Wave::InCapabilities waveInCaps;
	_mike.Check ();
	_wave.UnCheck ();
	_image.UnCheck ();
	SetSource (SRC_MIKE);

	// Initialize radio	buttons	(bits per sample)
	if (!waveInCaps.Is16BitSampling ())
	{
		_radio16.Disable();
		_bitsPerSample = 8;
	}

	// Initialize scroll bar (fft's	per	second)
	InitScrollPositions	();
	// Initialize the combo	box	with sampling frequencies
	if (waveInCaps.Is11_025kHz	())
		_comboFreq.AddValue	(Wave::BASE_SAMPLES_SEC);		
	if (waveInCaps.Is22_05kHz ())
		_comboFreq.AddValue	(2 * Wave::BASE_SAMPLES_SEC);  
	if (waveInCaps.Is44_1kHz ())
		_comboFreq.AddValue	(4 * Wave::BASE_SAMPLES_SEC); 
	assert (_samplesPerSecond == Wave::BASE_SAMPLES_SEC);
	// Select lowest available sampling	frequency
	_comboFreq.SelectPos (0); 
	_samplesPerSecond =	_comboFreq.GetValue();

	// Initialize combo	with FFT points
	_comboPoints.AddValue (FFT_POINTS);			// 0
	_comboPoints.AddValue (2 * FFT_POINTS);		// 1
	_comboPoints.AddValue (4 * FFT_POINTS);		// 2
	_comboPoints.AddValue (8 * FFT_POINTS);		// 3
	_comboPoints.AddValue (16 * FFT_POINTS);	// 4
	assert (_fftPoints == 4	* FFT_POINTS);
	_comboPoints.SelectPos (2);	 //	4 *	FFT_POINTS

	_painter.SetFormat (_samplesPerBuf,	_samplesPerSecond, _fftPoints, _bitsPerSample);
	_painter.ReInit	();
	// could be	changed	by ReInit ()
	_painter.GetFormat (_samplesPerSecond, _bitsPerSample);
	UpdateControls ();
	::PostMessage (hwnd, WM_COMMAND, IDC_BUTTON_START, 0);
}

DlgController::~DlgController ()
{
	_painter.Kill ();
	::PostQuitMessage (0);
}

void DlgController::UpdateControls ()
{
	_comboFreq.SelectValue (_samplesPerSecond);
	if (_bitsPerSample == 8)
	{
		_radio8.Check();
		_radio16.UnCheck();
	}
	else
	{
		_radio16.Check();
		_radio8.UnCheck();
	}
}

// called when _samplesPerSec, _fftPoints or _bitsPerSample	changed
void DlgController::ReInit ()
{
	_painter.SetFormat (
			_samplesPerBuf,
			_samplesPerSecond,
			_fftPoints,
			_bitsPerSample);

	_painter.ReInit ();
	_painter.GetFormat (_samplesPerSecond, _bitsPerSample);
	UpdateControls ();
}

void DlgController::InitScrollPositions	()
{
	// This	scrollbar controls fft's per second
	// by storing and returning	samples	per	buffer
	_scroll.Clear ();
	// We perform one fft per buffer
	// so start	with fftPoints large buffer
	// and shrink it down to MIN_SAMP_BUF
	int	sampPerBuf = _fftPoints;
	int	i;
	for	(i = 0;	sampPerBuf > 0; i++)
	{
		_scroll.AddValue (sampPerBuf);
		sampPerBuf -= MIN_SAMP_BUF;
	}

	_scroll.Init (i, 1);

	// let's start with	one	buffer per fft
	_samplesPerBuf = _fftPoints;
	_scroll.SetPos (0);
	// fft's per second
	_edit.SetNumber	(_samplesPerSecond / _samplesPerBuf);
}

void DlgController::OnScroll (int code,	int	pos) 
{ 
	_scroll.Command	(code, pos);
	_samplesPerBuf = _scroll.GetValue(); 
	assert (_samplesPerBuf <= _fftPoints);
	assert (_samplesPerBuf >= MIN_SAMP_BUF);
	_painter.SetFormat (
			_samplesPerBuf,
			_samplesPerSecond,
			_fftPoints,
			_bitsPerSample);
	_painter.ReInit ();
	_edit.SetNumber	(_samplesPerSecond / _samplesPerBuf);
}

void DlgController::Stop ()
{
	_painter.Stop ();
	::EnableWindow (::GetDlgItem (_hWnd, IDC_BUTTON_START), TRUE);
}

// Child control command processing

bool DlgController::OnCommand (int ctrlid, int code)
{
	int	i =	0;
	switch (ctrlid)
	{
	case IDC_BUTTON_ABOUT:
		::DialogBox	(_hInst, MAKEINTRESOURCE (DLG_ABOUT), _hWnd, DLGPROC (AboutDlgProc));
		return true;
	case IDC_BUTTON_START:
		if (_isStopped)
		{
			_isStopped = !_painter.Start();
			if (_isStopped)
			{
				_buttonStart.SetText ("Start");
			}
			else
			{
				_buttonStart.SetText ("Stop");
			}
		}
		else
		{
			Stop ();
			_buttonStart.SetText ("Start");
			_isStopped = true;
		}
		return true;
	case IDC_RADIO_8_BITS:
		if (_radio8.IsClicked (code))
		{
			_bitsPerSample = 8;
			ReInit ();
		}
		return true;
	case IDC_RADIO_16_BITS:
		if (_radio16.IsClicked (code))
		{
			_bitsPerSample = 16;
			ReInit ();
		}
		return true;
	case IDC_RADIO_MIKE:
		if (_mike.IsClicked (code))
		{
			_painter.SetSource (SRC_MIKE);
			ReInit ();
			return true;
		}
		break;
	case IDC_RADIO_FILE:
		if (_wave.IsClicked (code))
		{
			SelectFile (SRC_WAVE);
			return true;
		}
		break;
	case IDC_RADIO_IMAGE:
		if (_image.IsClicked (code))
		{
			SelectFile (SRC_IMAGE);
			return true;
		}
		break;
	case IDC_EDIT_FILE:
		if (_editFile.IsChanged (code))
			::PostMessage (_hWnd, WM_COMMAND, MAKEWPARAM (IDC_BUTTON_FILE, 0), 0);
		break;
	case IDC_BUTTON_FILE:
		{
			std::string name = _editFile.GetString ();
			SourceType type = ValidateFile (name, SRC_ANY);
			if (type == SRC_WAVE)
			{
				_mike.UnCheck ();
				_image.UnCheck ();
				_wave.Check ();
				_painter.SetSource (SRC_WAVE, name.c_str ());
			}
			else if (type == SRC_IMAGE)
			{
				_mike.UnCheck ();
				_wave.UnCheck ();
				_image.Check ();
				_painter.SetSource (SRC_IMAGE, name.c_str ());
			}
			else
			{
				_wave.UnCheck ();
				_image.UnCheck ();
				_mike.Check ();
				_painter.SetSource (SRC_MIKE);
			}
			InitScrollPositions	();
			ReInit ();
		}
		return true;
	case IDC_COMBO_SAMPLING:
		if (_comboFreq.IsChanged (code))
		{
			_samplesPerSecond =	_comboFreq.GetValue	();
			InitScrollPositions	();
			ReInit ();
			return true;
		}	 
		break;
	case IDC_COMBO_POINTS:
		if (_comboPoints.IsChanged(code))
		{
			_fftPoints = _comboPoints.GetValue ();
			InitScrollPositions	();
			ReInit ();
			return true;
		}
	}
	return false;
}

SourceType DlgController::ValidateFile (std::string & name, SourceType type)
{
	SourceType actualType = SRC_NONE;
	if (name.empty ())
	{
		return actualType;
	}
	std::string::size_type i = name.find_last_of ('.');
	if (i != std::string::npos)
	{
		if (stricmp (".wav", &name [i]) == 0)
			actualType = SRC_WAVE;
		else if (stricmp (".bmp", &name [i]) == 0)
			actualType = SRC_IMAGE;
	}

	if (actualType != type)
	{
		return actualType;
	}
	// we have the right extension
	if (::GetFileAttributes (name.c_str ()) == 0xffffffff)
	{
		Win::ClearError ();
		std::string msg ("Cannot open file\n");
		msg += name;
		Display (msg.c_str ());
		return SRC_NONE;
	}
	return actualType;
}

void DlgController::SelectFile (SourceType type)
{
	FileGetter browser;
	browser.SetFilter (GetFileFilter (type));
	if (browser.GetExistingFile (_hWnd, "Select a source file"))
	{
		char const * path = browser.GetPath ();
		_currentDir.Change (browser.GetDirectory ());
		ListCurrentDir ();
		_editFile.SetText (browser.GetFileName ());
		_painter.SetSource (type, path);
	}
	else
	{
		_wave.UnCheck ();
		_image.UnCheck ();
		_mike.Check ();
		_painter.SetSource (SRC_MIKE);
	}
	ReInit ();
}

void DlgController::ListCurrentDir ()
{
	_editFile.ResetContents ();
	// List current directory
	FileSeq seqW ("*.wav");
	if (!seqW.AtEnd ())
		_editFile.SetText (seqW.GetName ());
	for (; !seqW.AtEnd (); seqW.Advance ())
		_editFile.AddString (seqW.GetName ());
	FileSeq seqB ("*.bmp");
	if (seqW.AtEnd () && !seqB.AtEnd ())
		_editFile.SetText (seqB.GetName ());
	for ( ; !seqB.AtEnd (); seqB.Advance ())
		_editFile.AddString (seqB.GetName ());
}

void DlgController::Display (char const * msg)
{
	::MessageBox (_hWnd, msg, "Frequency Analyzer", MB_OK | MB_ICONERROR);
}
