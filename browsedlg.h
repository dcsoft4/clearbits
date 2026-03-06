#pragma once

class CBrowseDlg
{
public:
    	// construct a dialog with given title and initial dir (both may be NULL):
	CBrowseDlg (HWND hwndOwner, LPCSTR pTitle, LPCSTR pInitialDir);
    
    	// Show dialog and get input:
    BOOL DoModal();

		// Access this member to get selected folder after DoModal() returns:    
	CString m_strDir;    
    
protected:    
	static int CALLBACK BrowseCallbackProc (HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

    HWND m_hwndOwner;
	CString m_strTitle;    
};

