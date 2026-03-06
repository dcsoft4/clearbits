#include "stdafx.h"
#include "ResFile.h"

CResFile::CResFile ()
{
	m_hMem = NULL;
}


CResFile::~CResFile ()
{
	// Doc says FreeResource() is obsolete, but there is no equivalent function
	// that works with user-defined resource types (like "TEXT")
	FreeResource (m_hMem);

	m_hMem = NULL;
}


BOOL CResFile::LoadFromResource (HINSTANCE hInst, UINT id, LPCTSTR pType)
{
	HRSRC hResource = FindResource (hInst, MAKEINTRESOURCE(id), pType );
	if ( !hResource )
		return FALSE;

	DWORD dwSize = SizeofResource (hInst, hResource);
	if ( !dwSize )
		return FALSE;
	
	m_hMem = (HRSRC) LoadResource (hInst, hResource);
	if ( !m_hMem )
		return FALSE;

	LPBYTE pLockedMem = (LPBYTE) LockResource (m_hMem);
	if ( !pLockedMem )
		return FALSE;

	// This is the memory to be accessed by the CFile
	Attach (pLockedMem, dwSize);

	return TRUE;
}
