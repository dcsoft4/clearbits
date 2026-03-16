#include "stdafx.h"
#include "sampbuf.h"

CSampleBuffer::CSampleBuffer()
{
    m_pWaveHdr = NULL;
    m_pData = NULL;
	m_hWaveHdr = NULL;
	m_hData = NULL;

	m_bPreparedOut = FALSE;
}


CSampleBuffer::~CSampleBuffer()
{
    if (m_pWaveHdr)
        GlobalUnlock (m_hWaveHdr);

	if (m_hWaveHdr)
        GlobalFree (m_hWaveHdr);

    if (m_pData)
        GlobalUnlock (m_hData);

    if (m_hData)
        GlobalFree (m_hData);
}



BOOL CSampleBuffer::Alloc (UINT nMaxBufSize)
{
	if ( m_pWaveHdr && m_pData )	// already allocated
		return TRUE;

    // allocate wave hdr and associated 64K byte buffer
    m_hWaveHdr = GlobalAlloc (GMEM_SHARE | GMEM_MOVEABLE, sizeof (WAVEHDR));
    m_hData = GlobalAlloc (GMEM_SHARE | GMEM_MOVEABLE, nMaxBufSize);
    m_pWaveHdr = (LPWAVEHDR) GlobalLock (m_hWaveHdr);
    m_pData = (LPBYTE) GlobalLock (m_hData);
    if (!m_pWaveHdr || !m_pData)
		return FALSE;
    
    m_pWaveHdr->lpData = (LPSTR) m_pData;
    m_pWaveHdr->dwFlags = 0;
	m_pWaveHdr->dwUser = (DWORD_PTR) this;		// pointer to itself
	return TRUE;
}


MMRESULT CSampleBuffer::PrepareOut (HWAVEOUT hWaveOut, LONG nBufSize)
{
	if ( !hWaveOut )
		return MMSYSERR_INVALHANDLE;

	ASSERT ( m_pWaveHdr && m_pData );

	// Prepare the buffer for output
	m_pWaveHdr->dwFlags = 0;
	m_pWaveHdr->dwLoops = 0;
    m_pWaveHdr->dwBufferLength = nBufSize;
	MMRESULT result = waveOutPrepareHeader (hWaveOut, m_pWaveHdr, sizeof (WAVEHDR));
	ASSERT (result == MMSYSERR_NOERROR);
	if ( result == MMSYSERR_NOERROR )
		m_bPreparedOut = TRUE;

	return result;
}

MMRESULT CSampleBuffer::UnprepareOut (HWAVEOUT hWaveOut)
{
	if ( !hWaveOut || !m_bPreparedOut )	// nothing to do
		return MMSYSERR_NOERROR;

	// UnPrepare the buffer
    MMRESULT result = waveOutUnprepareHeader (hWaveOut, m_pWaveHdr, sizeof (WAVEHDR));
	ASSERT (result == MMSYSERR_NOERROR);
	if ( result == MMSYSERR_NOERROR )
		m_bPreparedOut = FALSE;

	return result;
}
