#include "stdafx.h"
#include "ClearBits.h"
#include "ClearBitsDlg.h"
#include "Options.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CClearBitsApp, CWinApp)
	//{{AFX_MSG_MAP(CClearBitsApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


CClearBitsApp::CClearBitsApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CClearBitsApp object

CClearBitsApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CClearBitsApp initialization

BOOL CClearBitsApp::InitInstance()
{
	// To use BIF_USENEWUI flag in ::SHBrowseForFolder(), app must call AfxOleInit()
	AfxOleInit();

	SetRegistryKey (_T("DCSoft"));

#if 0
	AfxMessageBox ("Setting process and thread priority", MB_OK);
	if ( !::SetPriorityClass (GetCurrentProcess(), NORMAL_PRIORITY_CLASS) ||	// 
		 !::SetThreadPriority (GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL) ) // THREAD_PRIORITY_BELOW_NORMAL
	{
		AfxMessageBox ("Could not set priority", MB_OK);
	}
#endif


	// Init accelerators
    m_hAccelTable = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));

	// Init Options
	CString strOptionsRegKey;
	strOptionsRegKey.LoadString (IDS_OPTIONS_REGKEY);	// e.g. "Software\\DCSoft\\ClearBits"
	COptions::Init (strOptionsRegKey);


	CClearBitsDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}


	// Cleanup Options
	COptions::Cleanup();


	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL CClearBitsApp::ProcessMessageFilter(int code, LPMSG lpMsg) 
{
	// Make accelerators work in a MFC dialog-based app
	if (code < 0)
		CWinApp::ProcessMessageFilter(code, lpMsg);

	if (m_pMainWnd->GetSafeHwnd() && m_hAccelTable)
	{
		if (::TranslateAccelerator(m_pMainWnd->GetSafeHwnd(), m_hAccelTable, lpMsg))
			return(TRUE);
	}

	return CWinApp::ProcessMessageFilter(code, lpMsg);
}
