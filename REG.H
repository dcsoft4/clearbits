class CRegistry
{
public:
    CRegistry ();
    CRegistry (HKEY hOpenKey, LPCSTR pszSubKey);
    ~CRegistry();

	BOOL IsValid();
	
    // keys:
    LONG OpenKey (HKEY hOpenKey, LPCSTR pszSubKey);
    LONG CreateKey (HKEY hOpenKey, LPCSTR pszSubKey);
	static LONG DeleteKey (HKEY hOpenKey, LPCSTR pszSubKey);
    LONG CloseKey ();

    // Values
    LONG QueryValueEx (LPCSTR pszValueName, LPDWORD pdwValueType, PBYTE pValueBuf, PULONG plValueSize);
    LONG SetValueEx (LPCSTR pszValueName, DWORD dwValueType, LPCSTR pValueBuf, LONG lValueSize);
    LONG DeleteValue (LPCSTR pszValueName);
	
	// Higher level functions
	BOOL ReadStringValue (LPCSTR pszValueName, CString &strRegVal);
	BOOL WriteStringValue (LPCSTR pszValueName, LPCSTR pszRegValue);

	// BOOL values are written as strings containing "1" or "0"
	BOOL ReadBoolValue (LPCSTR pszValueName, BOOL &b);
	BOOL WriteBoolValue (LPCSTR pszValueName, BOOL b);

	BOOL ReadDwordValue (LPCSTR pszValueName, DWORD &dw);
	BOOL WriteDwordValue (LPCSTR pszValueName, DWORD dw);

	BOOL ReadBinaryValue (LPCSTR pszValueName, BYTE *pBuf, DWORD dwBufSize);


	// Enumerate values
	BOOL EnumKeyEx ( DWORD dwIndex, LPTSTR pSubKeyName, DWORD *pdwNameLen, FILETIME *pft = NULL );

	// Static members not requiring a CRegistry instance
	static BOOL ReadStringValue (HKEY hOpenKey, LPCSTR pszSubKey, LPCSTR pszValueName, CString &strRegVal);
	static BOOL WriteStringValue (HKEY hOpenKey, LPCSTR pszSubKey, LPCSTR pszValueName, LPCSTR pszRegValue);
	static BOOL ReadBoolValue (HKEY hOpenKey, LPCSTR pszSubKey, LPCSTR pszValueName, BOOL &b);
	static BOOL WriteBoolValue (HKEY hOpenKey, LPCSTR pszSubKey, LPCSTR pszValueName, BOOL b);
	static BOOL ReadDwordValue (HKEY hOpenKey, LPCSTR pszSubKey, LPCSTR pszValueName, DWORD &dw);
	static BOOL WriteDwordValue (HKEY hOpenKey, LPCSTR pszSubKey, LPCSTR pszValueName, DWORD dw);

	static BOOL DeleteValue (HKEY hOpenKey, LPCSTR pszSubKey, LPCSTR pszValueName);
	static BOOL EnsureKeyExists (HKEY hOpenKey, LPCSTR pszSubKey);

	operator HKEY();

protected:
	// Open key
    HKEY    m_hKey;

	// Info on open key
	HKEY	m_hOpenKey;
	CString	m_strSubKey;
};
