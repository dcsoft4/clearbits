#pragma once

#include "mmsystem.h"

class CSampleBuffer
{
public:
    CSampleBuffer();
    ~CSampleBuffer();

	BOOL Alloc (UINT nMaxBufSize);
	MMRESULT PrepareOut (HWAVEOUT hWaveOut, LONG nBufSize);
	MMRESULT UnprepareOut (HWAVEOUT hWaveOut);

    LPWAVEHDR	m_pWaveHdr;
    LPBYTE		m_pData;

protected:

    HANDLE		m_hWaveHdr;
	HANDLE		m_hData;

	BOOL		m_bPreparedOut;
};
