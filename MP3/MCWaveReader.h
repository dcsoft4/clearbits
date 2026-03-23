/*###########################################################################

 http://www.urban.ne.jp/home/mnm/mcwavereader.txt
 Mostly Japanese website by Hitsuji-Saru



 McWaveReader.h

    オーディオファイル リーダー for PCM, MP3, RMP

    ver1.0 2001/05/15-

###########################################################################*/

#pragma once

#include "stdafx.h"

/*------------------------------ クラス定義 -------------------------------*/
class McWaveReader
{
// attribute >>>>>>>>>>>>>>>

    HMMIO m_MmioHn;

    _TCHAR m_FormatType;
    WAVEFORMATEX* m_oBuffFormat;

    long m_DataOffset;          // ファイルのデータ先頭オフセット
    bool m_bFoundEnd;           // データ終端到達フラグ

    MPEGLAYER3WAVEFORMAT* m_oMp3WaveFormat;

    HACMSTREAM m_AcmHn;         // Acm Stream Handle
    ACMSTREAMHEADER* m_oAsh;    // Acm Stream Header
    BYTE* m_aFileBuffer;        // ファイルバッファ
    DWORD m_FileBuffSize;       // ファイルバッファのサイズ
    BYTE* m_aDecBuffer;         // デコードバッファ
    DWORD m_DecBuffSize;        // デコードバッファのサイズ
    DWORD m_DecRemainderSize;   // 未転送のデコード済みデータのサイズ
    BYTE* m_pDecRemainder;      // 未転送のデコード済みデータへのポインタ

    long m_Progress;	        // 出力総量
	long m_lEnd;
	bool m_bFileOpen;
	CString m_strFileName;

// method >>>>>>>>>>>>>>>>>>

protected:
    bool Read_Pcm( DWORD Size, BYTE* pBuffer, DWORD* pWritten );
    bool Read_Acm( DWORD Size, BYTE* pBuffer, DWORD* pWritten );

    bool ReadMp3FrameHeader( void );
    DWORD Decode( void );
    void CloseAcmStream( void );

public:
    McWaveReader();
    virtual ~McWaveReader();

    bool Create( void );
    virtual void Terminate( void );

    bool Open( char* pFilename );
    void Close( void );
    bool Reset( void );

	LPCTSTR GetFileName();
	bool IsOpen( void );

    bool Read( DWORD Size, BYTE* pBuffer, DWORD* pWritten );

	bool Seek (LONG lTo);


    WAVEFORMATEX* GetWaveFormat( void );
    long GetProgress( void ) { return m_Progress; }
};
