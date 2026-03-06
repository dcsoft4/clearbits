#pragma once

enum TransferType
{
	TRANSFER_FROM_REGISTRY,
	TRANSFER_TO_REGISTRY,
	TRANSFER_FROM_DIALOG,
	TRANSFER_TO_DIALOG,
	TRANSFER_FROM_DEFAULTS
};

class COptions
{
public:

		// e.g. if pRegKey is "Software\\DCSoft\\ClearBits", options will be stored in
		// HKEY_CURRENT_USER\Software\DCSoft\ClearBits		
	static void Init (LPCTSTR pRegKey);	
	static void Cleanup();

    	// Show settings property sheet; returns TRUE if settings have been
		// updated
    static BOOL ShowUI();

// General Page
	static CString		m_strMoveDestDir;		// dest directory to move files to

protected:
    static void TransferData(TransferType type);

    // DC Use templates to create only one function
	static void TransferData(TransferType type, int &nOpt, int &nDlg, int nDefault, LPCTSTR pRegValueName);
	static void TransferData(TransferType type, UINT &nOpt, UINT &nDlg, UINT nDefault, LPCTSTR pRegValueName);
	static void TransferData(TransferType type, CString &strOpt, CString &strDlg, LPCTSTR pDefault, LPCTSTR pRegValueName);

	static CString					m_strRegKey;
};

