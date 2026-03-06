#include "stdafx.h"
#include <shlobj.h>
#include "FileName.h"
#include "browsedlg.h"

CBrowseDlg::CBrowseDlg (HWND hwndOwner, LPCSTR pTitle, LPCSTR pInitialDir)
{
    m_hwndOwner = hwndOwner;
    m_strTitle = pTitle;
    m_strDir = pInitialDir;
}    


BOOL CBrowseDlg::DoModal()
{
    BOOL bOK = FALSE;
    
    LPMALLOC pMalloc;
    if (::SHGetMalloc(&pMalloc) == NOERROR)
    {
        BROWSEINFO bi;
        char pszBuffer[MAX_PATH];
        // Get help on BROWSEINFO struct - it's got all the bit settings.
        bi.hwndOwner = m_hwndOwner;
        bi.pidlRoot = NULL;
        bi.pszDisplayName = pszBuffer;
        bi.lpszTitle = m_strTitle;
        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_USENEWUI | BIF_STATUSTEXT;
        bi.lpfn = BrowseCallbackProc;
        bi.lParam = (LPARAM) this;
        // This next call issues the dialog box.
        LPITEMIDLIST pidl = ::SHBrowseForFolder(&bi);
        if (pidl)
        {
            if (::SHGetPathFromIDList(pidl, pszBuffer) && *pszBuffer)	// user selected something within file system
			{
                m_strDir = pszBuffer;
				bOK = TRUE;
			}

            // Free the PIDL allocated by SHBrowseForFolder.
            pMalloc->Free(pidl);
        }
        // Release the shell's allocator.
        pMalloc->Release();
    }
    
	return bOK;
}    


int CALLBACK CBrowseDlg::BrowseCallbackProc (HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    switch (uMsg)
    {
        case BFFM_INITIALIZED:
		{
			// Set initial folder:
			CBrowseDlg *pThis = (CBrowseDlg *) lpData;
            ::SendMessage (hwnd, BFFM_SETSELECTION, TRUE, (LPARAM) (LPCSTR) pThis->m_strDir);
            break;
		}

		case BFFM_SELCHANGED:
		{
			// Get display name of currently selected folder:
	        CHAR szBuffer[MAX_PATH];

			BOOL bIsInFileSystem = ::SHGetPathFromIDList ((LPCITEMIDLIST) lParam, szBuffer);
			if (bIsInFileSystem)
			{
				// Update status line:
				::SendMessage (hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM) szBuffer);
			}

			// Disable OK button if selected item is not part of file system
	        ::SendMessage (hwnd, BFFM_ENABLEOK, NULL, (LPARAM) bIsInFileSystem);
            
			break;
		}
    }
    
	return 0;
}    
