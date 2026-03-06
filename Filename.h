#pragma once

class CFileName
{
public:
    CFileName (LPCSTR pFileName);

        // return fully qualified filename (e.g. "c:\windows\cdplayer.exe"):
    operator LPCSTR();

    CString GetDrive();
    CString GetPath();
    CString GetFile();
    CString GetExt();

    void SetDrive (LPCSTR pDrive);
    void SetPath (LPCSTR pPath);
    void SetFile (LPCSTR pFile);
    void SetExt (LPCSTR pExt);

protected:
    CString m_strDrive, m_strPath, m_strFile, m_strExt;
    CString m_strEntireName;
};


