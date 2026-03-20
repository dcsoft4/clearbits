// SeekDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClearBits.h"
#include "SeekDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSeekDlg dialog


CSeekDlg::CSeekDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSeekDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSeekDlg)
	m_strSeekPos = _T("");
	//}}AFX_DATA_INIT
}


void CSeekDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSeekDlg)
	DDX_Text(pDX, IDC_EDIT_SEEKPOS, m_strSeekPos);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSeekDlg, CDialog)
	//{{AFX_MSG_MAP(CSeekDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSeekDlg message handlers
