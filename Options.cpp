#include "stdafx.h"
#include "resource.h"
#include "Reg.h"

#include "Options.h"

// static variables
CString			COptions::m_strRegKey;

	// General Page
CString COptions::m_strMoveDestDir;


void COptions::Init (LPCTSTR pRegKey)
{
	m_strRegKey = pRegKey;

	// Load initial data from registry
	TransferData (TRANSFER_FROM_REGISTRY);
}

void COptions::Cleanup()
{
	TransferData (TRANSFER_TO_REGISTRY);
}



BOOL COptions::ShowUI()
{
    return FALSE;
}


void COptions::TransferData(TransferType type)
{
	CString strDummy;
	TransferData (type, m_strMoveDestDir, strDummy, "", "MoveDestDir");
}



void COptions::TransferData(TransferType type, int &nOpt, int &nDlg, int nDefault, LPCTSTR pRegValueName)
{
	switch (type)
	{
		case TRANSFER_FROM_DIALOG:
			nOpt = nDlg;
			break;

		case TRANSFER_TO_DIALOG:
			nDlg = nOpt;
			break;

		case TRANSFER_FROM_REGISTRY:
			if ( !CRegistry::ReadDwordValue (HKEY_CURRENT_USER, m_strRegKey, pRegValueName, (DWORD &) nOpt) )
				nOpt = nDefault;	// failed to read value from registry so initialize to default
			break;

		case TRANSFER_TO_REGISTRY:
			CRegistry::WriteDwordValue (HKEY_CURRENT_USER, m_strRegKey, pRegValueName, nOpt);
			break;

		case TRANSFER_FROM_DEFAULTS:
			nOpt = nDefault;
			break;

		default:
			ASSERT (FALSE);
			break;
	}
}

void COptions::TransferData(TransferType type, UINT &nOpt, UINT &nDlg, UINT nDefault, LPCTSTR pRegValueName)
{
	switch (type)
	{
		case TRANSFER_FROM_DIALOG:
			nOpt = nDlg;
			break;

		case TRANSFER_TO_DIALOG:
			nDlg = nOpt;
			break;

		case TRANSFER_FROM_REGISTRY:
			if ( !CRegistry::ReadDwordValue (HKEY_CURRENT_USER, m_strRegKey, pRegValueName, (DWORD &) nOpt) )
				nOpt = nDefault;	// failed to read value from registry so initialize to default
			break;

		case TRANSFER_TO_REGISTRY:
			CRegistry::WriteDwordValue (HKEY_CURRENT_USER, m_strRegKey, pRegValueName, nOpt);
			break;

		case TRANSFER_FROM_DEFAULTS:
			nOpt = nDefault;
			break;

		default:
			ASSERT (FALSE);
			break;
	}
}

void COptions::TransferData(TransferType type, CString &strOpt, CString &strDlg, LPCTSTR pDefault, LPCTSTR pRegValueName)
{
	switch (type)
	{
		case TRANSFER_FROM_DIALOG:
			strOpt = strDlg;
			break;

		case TRANSFER_TO_DIALOG:
			strDlg = strOpt;
			break;

		case TRANSFER_FROM_REGISTRY:
			if ( !CRegistry::ReadStringValue (HKEY_CURRENT_USER, m_strRegKey, pRegValueName, strOpt) )
				strOpt = pDefault;	// failed to read value from registry so initialize to default
			break;

		case TRANSFER_TO_REGISTRY:
			CRegistry::WriteStringValue (HKEY_CURRENT_USER, m_strRegKey, pRegValueName, strOpt);
			break;

		case TRANSFER_FROM_DEFAULTS:
			strOpt = pDefault;
			break;

		default:
			ASSERT (FALSE);
			break;
	}
}
