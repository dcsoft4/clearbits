#include "stdafx.h"
#include "ClearBits.h"
#include "ClearBitsDlg.h"
#include "SearchDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchDlg dialog


CSearchDlg::CSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSearchDlg)
	m_strSearch = _T("");
	//}}AFX_DATA_INIT

	m_nSelectedFile = LB_ERR;
}


void CSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchDlg)
	DDX_Control(pDX, IDC_LIST_FOUND, m_lbFound);
	DDX_Text(pDX, IDC_EDIT_SEARCH, m_strSearch);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSearchDlg, CDialog)
	//{{AFX_MSG_MAP(CSearchDlg)
	ON_BN_CLICKED(IDC_BUTTON_GO, OnButtonGo)
	ON_LBN_SELCHANGE(IDC_LIST_FOUND, OnSelchangeListFound)
	ON_LBN_DBLCLK(IDC_LIST_FOUND, OnDblclkListFound)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchDlg message handlers

void CSearchDlg::OnButtonGo() 
{
	// Search for m_strSearch in main dialog's listbox entries
	UpdateData();
	m_strSearch.MakeLower();	// case insensitive
	m_lbFound.ResetContent();	// start with empty found list

	CClearBitsDlg *pMainDlg = (CClearBitsDlg *) AfxGetMainWnd();
	const CListBox *pLB = &pMainDlg->m_lbWaveFiles;
	CString strCurFile;
	for (int i=0; i < pLB->GetCount(); i++)
	{
		pLB->GetText (i, strCurFile);
		strCurFile.MakeLower();		// case insensitive
		if ( strCurFile.Find (m_strSearch) >= 0 )
		{
			pLB->GetText (i, strCurFile);	// get item again to preserve case
			m_lbFound.AddString (strCurFile);
			m_lbFound.SetItemData ( m_lbFound.GetCount() - 1, i );	// item data is index in main list
		}
	}
}

void CSearchDlg::OnSelchangeListFound() 
{
	m_nSelectedFile = (int) m_lbFound.GetItemData ( m_lbFound.GetCurSel() );
}

void CSearchDlg::OnDblclkListFound() 
{
	EndDialog (IDOK);
}
