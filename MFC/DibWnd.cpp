#include "stdafx.h"
#include "DibWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CDibWnd

CDibWnd::CDibWnd()
{
	m_hBitmap = NULL;
	m_hPalette = NULL;
    m_nBitmapWidth = 0;
    m_nBitmapHeight = 0;
}



CDibWnd::~CDibWnd()
{
	if (m_hBitmap)
        DeleteObject (m_hBitmap);

	if (m_hPalette)
        DeleteObject (m_hPalette);
}


BOOL CDibWnd::SetDib (HINSTANCE hInst, WORD resId, BOOL bMapColors)
{
    // Use LoadImage() to get the image loaded into a DIBSection
	m_hBitmap = (HBITMAP) LoadImage( hInst, MAKEINTRESOURCE (resId), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | ( (bMapColors) ? LR_LOADMAP3DCOLORS : 0 ) );
	if ( ! m_hBitmap )
		return FALSE;

    // Get the dimensions and color depth of the DIBSection
	BITMAP  bm;
	GetObject ( m_hBitmap, sizeof(BITMAP), &bm );
	m_nBitmapWidth = bm.bmWidth;
	m_nBitmapHeight = bm.bmHeight;

	// If the DIBSection is 256 color or less, it has a color table
	if( ( bm.bmBitsPixel * bm.bmPlanes ) <= 8 )
	{
		// Create a memory DC and select the DIBSection into it
		HDC hMemDC = CreateCompatibleDC( NULL );
		HBITMAP hOldBitmap = (HBITMAP) SelectObject( hMemDC, m_hBitmap );

		// Get the DIBSection's color table
		RGBQUAD       rgb[256];
		GetDIBColorTable( hMemDC, 0, 256, rgb );

		// Create a palette from the color table
		LPLOGPALETTE pLogPal = (LPLOGPALETTE) malloc( sizeof(LOGPALETTE) + (256*sizeof(PALETTEENTRY)) );
		pLogPal->palVersion = 0x300;
		pLogPal->palNumEntries = 256;
		for( UINT i=0;i<256;i++)
		{
			pLogPal->palPalEntry[i].peRed = rgb[i].rgbRed;
			pLogPal->palPalEntry[i].peGreen = rgb[i].rgbGreen;
			pLogPal->palPalEntry[i].peBlue = rgb[i].rgbBlue;
			pLogPal->palPalEntry[i].peFlags = 0;
		}
		m_hPalette = CreatePalette( pLogPal );

		// Clean up
		free( pLogPal );
		SelectObject( hMemDC, hOldBitmap );
		DeleteDC( hMemDC );
	}
	else	// It has no color table, so use a halftone palette
	{
		HDC hRefDC = ::GetDC( NULL );
		m_hPalette = CreateHalftonePalette( hRefDC );
		::ReleaseDC( NULL, hRefDC );
	}

	return TRUE;
} 



UINT CDibWnd::OnQueryNewPalette (HWND hWnd)
{
	/*
    ** Realize palette and redraw
    */
	HDC hDC = ::GetDC (hWnd);
	HPALETTE hPalOld = SelectPalette (hDC, m_hPalette, FALSE);
	UINT nColorsChanged = RealizePalette (hDC);
	SelectPalette (hDC, hPalOld, FALSE);
	::ReleaseDC (hWnd, hDC);

	::InvalidateRect (hWnd, NULL, TRUE);

	// return non-zero if we changed the palette:
	return nColorsChanged;
}



UINT CDibWnd::OnPaletteChanged (HWND hWnd, WPARAM wParam)
{
    // abort if this window is the one that changed the palette:
	if ( (HWND) wParam == hWnd)
	    return 0;
	    
	// no, some other window did; process
    return OnQueryNewPalette (hWnd);
}



BEGIN_MESSAGE_MAP(CDibWnd, CWnd)
	//{{AFX_MSG_MAP(CDibWnd)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDibWnd message handlers

void CDibWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	HDC hDC = dc.GetSafeHdc();

	CRect r;
	GetClientRect (&r);

	HDC hMemDC = CreateCompatibleDC( hDC );
	HBITMAP hOldBitmap = (HBITMAP) SelectObject( hMemDC, m_hBitmap );
	HPALETTE hOldPalette = SelectPalette( hDC, m_hPalette, FALSE );
	RealizePalette( hDC );

	BitBlt( hDC, 0, 0, m_nBitmapWidth, m_nBitmapHeight,
        	hMemDC, 0, 0, SRCCOPY );

	SelectObject( hMemDC, hOldBitmap );
	SelectPalette( hDC, hOldPalette, FALSE );
}
