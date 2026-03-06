#pragma once


#include "OXStatic.h"
#include "DibWnd.h"
#include "Hyperlink.h"
#include "SampBuf.h"
#include "MCWaveReader.h"
#include "ResFile.h"


#define NUM_SAMPLE_BUFFERS		(2)


class CPauseInfo
{
public:
	void Set (LPCTSTR pFile, long lPos)
	{
		m_strFile = pFile;
		m_lCurPos = lPos;
	};

	void Reset()
	{
		m_strFile.Empty();
		m_lCurPos = 0;
	};

	BOOL IsValid()
	{
		return ( !m_strFile.IsEmpty() );
	};

	CString		m_strFile;
	long		m_lCurPos;
};


/////////////////////////////////////////////////////////////////////////////
// CClearBitsDlg dialog

class CClearBitsDlg : public CDialog
{
// Construction
public:
	CClearBitsDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CClearBitsDlg)
	enum { IDD = IDD_CLEARBITS_DIALOG };
	COXStatic	m_staticTitle;
	CListBox	m_lbWaveFiles;
	CHyperLink  m_hlLoad;
	CHyperLink	m_hlClear;
	CHyperLink	m_hlDel;
	CHyperLink	m_hlMove;
	BOOL	m_bRandomPlay;
	CString	m_strOutputFormat;
	CString	m_strProgress;
	CComboBox m_cbAlgo;
	int m_nAlgo;
	//}}AFX_DATA
	CDibWnd	m_LogoDibWnd;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClearBitsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CClearBitsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonLoad();
	afx_msg void OnButtonPlayPause();
	afx_msg void OnDestroy();
	afx_msg void OnButtonNext();
	afx_msg void OnButtonPrev();
	afx_msg void OnDblclkListWavefiles();
	afx_msg void OnSelchangeListWavefiles();
	afx_msg void OnButtonSearch();
	afx_msg void OnCheckRandom();
	afx_msg void OnSeekBackFast();
	afx_msg void OnSeekBackSlow();
	afx_msg void OnSeekForwardSlow();
	afx_msg void OnSeekForwardFast();
	afx_msg void OnStaticProgress();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnButtonClear();
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonMove();
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCbnSelchangeComboAlgo();
	//}}AFX_MSG
    afx_msg LRESULT OnBufPlayed (WPARAM, LPARAM);
    afx_msg LRESULT OnAppCommand (WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

	BOOL Play (BOOL bPlayImmediately);
	void Pause();
	void Stop();
	void CloseFile();
	BOOL NextFile (BOOL bPermitRandom);
	BOOL PrevFile();

	McWaveReader	m_WaveReader;

	CPauseInfo		m_PauseInfo;

	BOOL OpenOutDevice (WAVEFORMATEX *pwfx);	// if pwfx is NULL, open device as standard 44.1K/16-bit/stereo
	BOOL CloseOutDevice (BOOL bCloseImmediately);
	void PlayBuffer (CSampleBuffer *pSB);

	LONG GetNextBufSize();
	LONG GetNextBufSize_Fixed();
	LONG GetNextBufSize_VC();
	LONG GetNextBufSize_CAPI();
	LONG GetNextBufSize_RandomOrg();

	LONG GetNextBufSize_RandomOrg_Crypto2();
	LONG GetNextBufSize_RandomOrg_CryptoOptimal();
	LONG GetNextBufSize_WMP();
	LONG GetNextBufSize_SmallBuf();

	void DeleteFileFromDisk();
	void DeleteFileFromPlaylist();

    void _OnButtonPlayPause (BOOL bCtrlKeyPressed);

	void SetAlgo (UINT nNewAlgo);

	BOOL AddToPlayList (LPCTSTR pFilename);
	void SavePlaylist();


	HWAVEOUT			m_hWaveOut;
	WAVEFORMATEX		m_wfx;	// the format used to open the output device
	CSampleBuffer		m_arrSB[NUM_SAMPLE_BUFFERS];

	CString				m_strPlaylistFile;

	HCRYPTPROV			m_hProvider;

    CResFile			m_fileRandomData;

	CFont				m_fontBold;

    int		            m_nCryptoSize;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


