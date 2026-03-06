#pragma once

class CResFile : public CMemFile
{
public:
	CResFile();
	~CResFile();

	BOOL LoadFromResource (HINSTANCE hInst, UINT id, LPCTSTR pType);

protected:
	HRSRC m_hMem;
};

