#pragma once


// This is a CWnd that renders a DIB (256 color bitmap)


/////////////////////////////////////////////////////////////////////////////
// CDibWnd window


class CDibWnd : public CWnd
{
// Construction
public:
	CDibWnd();

// Attributes
public:
	BOOL SetDib (HINSTANCE hInst, WORD resId, BOOL bMapColors = FALSE);

// Operations
public:
    UINT OnQueryNewPalette (HWND hWnd);
    UINT OnPaletteChanged (HWND hWnd, WPARAM wParam);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDibWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDibWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDibWnd)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	HBITMAP m_hBitmap;
	HPALETTE m_hPalette;
	int m_nBitmapWidth, m_nBitmapHeight;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
