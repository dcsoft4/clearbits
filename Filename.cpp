#include "stdafx.h"
#include "filename.h"

CFileName::CFileName (LPCSTR pFileName)
{
	_splitpath_s(   pFileName,
                    m_strDrive.GetBuffer(_MAX_DRIVE), _MAX_DRIVE,
                    m_strPath.GetBuffer (_MAX_DIR),   _MAX_DIR,
                    m_strFile.GetBuffer (_MAX_FNAME), _MAX_FNAME,
                    m_strExt.GetBuffer (_MAX_EXT),   _MAX_EXT );

    m_strDrive.ReleaseBuffer();
    m_strPath.ReleaseBuffer();
    m_strFile.ReleaseBuffer();
    m_strExt.ReleaseBuffer();
}



void CFileName::SetDrive (LPCSTR pDrive)
{
    m_strDrive = pDrive;
}




void CFileName::SetPath (LPCSTR pPath)
{
    m_strPath = pPath;
}




void CFileName::SetFile (LPCSTR pFile)
{
    m_strFile = pFile;
}




void CFileName::SetExt (LPCSTR pExt)
{
    m_strExt = pExt;
}




CFileName::operator LPCSTR()
{
	_makepath_s( m_strEntireName.GetBuffer(_MAX_PATH), _MAX_PATH,
                 m_strDrive, m_strPath, m_strFile, m_strExt );

    m_strEntireName.ReleaseBuffer();

    return m_strEntireName;
}



CString CFileName::GetDrive()
{
    return m_strDrive;
}



CString CFileName::GetPath()
{
    return m_strPath;
}



CString CFileName::GetFile()
{
    return m_strFile;
}



CString CFileName::GetExt()
{
    return m_strExt;
}

