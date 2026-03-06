#include "stdafx.h"
#include <winreg.h>
#include "reg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define MAX_BUF_SIZE		(4096)		// max len of string to read

CRegistry::CRegistry()
{
    m_hKey = NULL;
	m_hOpenKey = NULL;
}



CRegistry::CRegistry (HKEY hOpenKey, LPCSTR pszSubKey)
{
    m_hKey = NULL;
	m_hOpenKey = NULL;
	OpenKey (hOpenKey, pszSubKey);
}


CRegistry::~CRegistry()
{
    if (m_hKey)
        CloseKey();
}


BOOL CRegistry::IsValid()
{
	return (m_hKey != NULL);
}



LONG CRegistry::OpenKey (HKEY hOpenKey, LPCSTR pszSubKey)
{
    if (m_hKey)
        CloseKey();

    LONG lRtn = ::RegOpenKey (hOpenKey, pszSubKey, &m_hKey);
	if (lRtn != ERROR_SUCCESS)
		m_hKey = NULL;

	m_hOpenKey = hOpenKey;
	m_strSubKey = pszSubKey;

    return lRtn;
}



LONG CRegistry::CreateKey (HKEY hOpenKey, LPCSTR pszSubKey)
{
    if (m_hKey)
        CloseKey();

    LONG lRtn = ::RegCreateKey (hOpenKey, pszSubKey, &m_hKey);
	if (lRtn != ERROR_SUCCESS)
		m_hKey = NULL;

	m_hOpenKey = hOpenKey;
	m_strSubKey = pszSubKey;

    return lRtn;
}



LONG CRegistry::DeleteKey (HKEY hOpenKey, LPCSTR pszSubKey)
{
    LONG lRtn = ::RegDeleteKey (hOpenKey, pszSubKey);

    return lRtn;
}



LONG CRegistry::CloseKey ()
{
	LONG lRtn = ERROR_SUCCESS;
	if (m_hKey)
	{
		lRtn = ::RegCloseKey(m_hKey);
		if (lRtn == ERROR_SUCCESS)
		    m_hKey = NULL;
	}

    return lRtn;
}



LONG CRegistry::QueryValueEx (LPCSTR pszValueName, LPDWORD pdwValueType, PBYTE pValueBuf, PULONG plValueSize)
{
    LONG lRtn = ::RegQueryValueEx (m_hKey, pszValueName, NULL, pdwValueType, pValueBuf, plValueSize);

    return lRtn;
}



LONG CRegistry::SetValueEx (LPCSTR pszValueName, DWORD dwValueType, LPCSTR pValueBuf, LONG lValueSize)
{
    LONG lRtn = ::RegSetValueEx (m_hKey, pszValueName, NULL, dwValueType, (CONST BYTE *) pValueBuf, lValueSize);

    return lRtn;
}



LONG CRegistry::DeleteValue (LPCSTR pszValueName)
{
    LONG lRtn = ::RegDeleteValue (m_hKey, pszValueName);

    return lRtn;
}



BOOL CRegistry::ReadStringValue (LPCSTR pszValueName, CString &strRegVal)
{
	// Abort if the key is not valid
	if (!IsValid())
		return FALSE;

	DWORD dwType, dwBufSize = MAX_BUF_SIZE;
	LONG lResult = QueryValueEx (pszValueName, &dwType, (PBYTE) strRegVal.GetBuffer (dwBufSize), &dwBufSize);
	ASSERT (lResult != ERROR_MORE_DATA);	// make sure MAX_BUF_SIZE is big enough

	// If the function succeeded, ensure the type is REG_SZ
	ASSERT (lResult != ERROR_SUCCESS || dwType == REG_SZ);	// this function is only meant for REG_SZ registry entries
	
	// cleanup
	strRegVal.ReleaseBuffer ();
	
	return (lResult == ERROR_SUCCESS);
}



BOOL CRegistry::WriteStringValue (LPCSTR pszValueName, LPCSTR pszRegValue)
{
	// Ensure the key exists and is open
	if (!IsValid() && (CreateKey (m_hOpenKey, m_strSubKey) != ERROR_SUCCESS))
		return FALSE;

	LONG lResult = SetValueEx (	pszValueName,
								REG_SZ,
								pszRegValue,
								lstrlen (pszRegValue) + 1	// +1 to include terminating NULL
								);
	
	return (lResult == ERROR_SUCCESS);
}



BOOL CRegistry::ReadBoolValue (LPCSTR pszValueName, BOOL &b)
{
    CString str;
    BOOL bOK = ReadStringValue (pszValueName, str);
    b = (str == "1");

	return bOK;
}


BOOL CRegistry::WriteBoolValue (LPCSTR pszValueName, BOOL b)
{
    CString str ( (b) ? "1" : "0" );
    return WriteStringValue (pszValueName, str);
}


BOOL CRegistry::ReadDwordValue (LPCSTR pszValueName, DWORD &dw)
{
	// Abort if the key is not valid
	if (!IsValid())
		return FALSE;

	DWORD dwType, dwBufSize = sizeof(dw);
	LONG lResult = QueryValueEx (pszValueName, &dwType, (PBYTE) &dw, &dwBufSize);
	ASSERT (lResult != ERROR_MORE_DATA);	// make sure MAX_BUF_SIZE is big enough
	return (lResult == ERROR_SUCCESS);
}



BOOL CRegistry::WriteDwordValue (LPCSTR pszValueName, DWORD dw)
{
	// Ensure the key exists and is open
	if (!IsValid() && (CreateKey (m_hOpenKey, m_strSubKey) != ERROR_SUCCESS))
		return FALSE;

	LONG lResult = SetValueEx (	pszValueName,
								REG_DWORD,
								(LPCSTR) &dw,
								sizeof(dw)
								);
	
	return (lResult == ERROR_SUCCESS);
}


BOOL CRegistry::ReadBinaryValue (LPCSTR pszValueName, BYTE *pBuf, DWORD dwBufSize)
{
	// Abort if the key is not valid
	if (!IsValid())
		return FALSE;

	DWORD dwType;
	LONG lResult = QueryValueEx (pszValueName, &dwType, pBuf, &dwBufSize);
	ASSERT (lResult != ERROR_MORE_DATA);	// make sure MAX_BUF_SIZE is big enough
	return (lResult == ERROR_SUCCESS);
}



BOOL CRegistry::EnumKeyEx ( DWORD dwIndex, LPTSTR pSubKeyName, DWORD *pdwNameLen, FILETIME *pft )
{
	FILETIME ft;
	if (!pft)
		pft = &ft;	// provide buffer so caller doesn't have to

	LONG lStatus = RegEnumKeyEx ( m_hKey, dwIndex, pSubKeyName, pdwNameLen, NULL, NULL, NULL, &ft );
	return ( lStatus == ERROR_SUCCESS  || lStatus == ERROR_MORE_DATA );
}


CRegistry::operator HKEY()
{
	ASSERT ( IsValid() );
	return m_hKey;
}


BOOL CRegistry::ReadStringValue (HKEY hOpenKey, LPCSTR pszSubKey, LPCSTR pszValueName, CString &strRegVal)
{
	CRegistry reg (hOpenKey, pszSubKey);
	return reg.ReadStringValue (pszValueName, strRegVal);
}



BOOL CRegistry::WriteStringValue (HKEY hOpenKey, LPCSTR pszSubKey, LPCSTR pszValueName, LPCSTR pszRegValue)
{
	CRegistry reg (hOpenKey, pszSubKey);
	return reg.WriteStringValue (pszValueName, pszRegValue);
}



BOOL CRegistry::ReadBoolValue (HKEY hOpenKey, LPCSTR pszSubKey, LPCSTR pszValueName, BOOL &b)
{
	CRegistry reg (hOpenKey, pszSubKey);
	return reg.ReadBoolValue (pszValueName, b);
}



BOOL CRegistry::WriteBoolValue (HKEY hOpenKey, LPCSTR pszSubKey, LPCSTR pszValueName, BOOL b)
{
	CRegistry reg (hOpenKey, pszSubKey);
	return reg.WriteBoolValue (pszValueName, b);
}



BOOL CRegistry::ReadDwordValue (HKEY hOpenKey, LPCSTR pszSubKey, LPCSTR pszValueName, DWORD &dw)
{
	CRegistry reg (hOpenKey, pszSubKey);
	return reg.ReadDwordValue (pszValueName, dw);
}

BOOL CRegistry::WriteDwordValue (HKEY hOpenKey, LPCSTR pszSubKey, LPCSTR pszValueName, DWORD dw)
{
	CRegistry reg (hOpenKey, pszSubKey);
	return reg.WriteDwordValue (pszValueName, dw);
}



BOOL CRegistry::DeleteValue (HKEY hOpenKey, LPCSTR pszSubKey, LPCSTR pszValueName)
{
	CRegistry reg (hOpenKey, pszSubKey);
	return ( reg.DeleteValue (pszValueName) == ERROR_SUCCESS );
}



BOOL CRegistry::EnsureKeyExists (HKEY hOpenKey, LPCSTR pszSubKey)
{
	CRegistry reg;
	return ( reg.CreateKey (hOpenKey, pszSubKey) == ERROR_SUCCESS );
}



