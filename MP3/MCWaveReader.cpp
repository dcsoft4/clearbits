/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

 McWaveReader.cpp

<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

#include "stdafx.h"
#include <mmsystem.h>
#include "McWaveReader.h"
#include "id3_int28.h"

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  McWaveReader ƒRƒ“ƒXƒgƒ‰ƒNƒ^^ƒfƒXƒgƒ‰ƒNƒ^
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
McWaveReader::McWaveReader()
{
    m_MmioHn = NULL;
    m_oBuffFormat = NULL;
    m_oMp3WaveFormat = NULL;

    m_AcmHn = NULL;
    m_oAsh = NULL;
    m_aFileBuffer = NULL;
    m_FileBuffSize = 16 * 1024; // ƒtƒ@ƒCƒ‹ƒoƒbƒtƒ@‚Ì‘å‚«‚³ (16KB ‚Í¬‚³‚¢H)
    m_aDecBuffer = NULL;
    m_DecBuffSize = 0;

	// DC
	m_bFileOpen = FALSE;
	m_lEnd = 0;
}

McWaveReader::~McWaveReader()
{
    Terminate();
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  McWaveReader::Create()

  ‰Šú‰»

>>  ¬”Û
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool McWaveReader::Create( void )
{
    m_oBuffFormat = new WAVEFORMATEX;
    m_oMp3WaveFormat = new MPEGLAYER3WAVEFORMAT;
    if( !m_oBuffFormat || !m_oMp3WaveFormat ) return false;

    return true;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  McWaveReader::Terminate()

  I—¹ˆ—
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void McWaveReader::Terminate( void )
{
    Close();

	if (m_oBuffFormat)
	{
		delete m_oBuffFormat;
		m_oBuffFormat = NULL;
	}

	if (m_oMp3WaveFormat)
	{
		delete m_oMp3WaveFormat;
		m_oMp3WaveFormat = NULL;
	}
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  McWaveReader::GetWaveFormat()

  “Ç‚İ‚ñ‚¾ƒf[ƒ^‚ğó‚¯æ‚é‚Ì‚É•K—v‚Èƒoƒbƒtƒ@ƒtƒH[ƒ}ƒbƒg‚ğ“¾‚é

>>  ƒtƒH[ƒ}ƒbƒg‚ğ‹Lq‚µ‚½ WAVEFORMATEX \‘¢‘Ì
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
WAVEFORMATEX* McWaveReader::GetWaveFormat( void )
{
    return m_oBuffFormat;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  McWaveReader::Open()

  ƒtƒ@ƒCƒ‹ƒI[ƒvƒ“

<<  pFilename   ƒtƒ@ƒCƒ‹–¼

>>  ¬”Û
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool McWaveReader::Open( char* pFilename )
{
    MMCKINFO ck;    // RIFF chunk
    MMCKINFO sck;   // Sub chunk of RIFF chunk

	// DC - save filename
	m_strFileName.Empty();

    // ‚Ü‚¸‚Í MMIO ƒI[ƒvƒ“
    m_MmioHn = ::mmioOpenA( pFilename, NULL, MMIO_ALLOCBUF | MMIO_READ );
    if( m_MmioHn == NULL ) return false;

	// DC determine file length, rewind
	m_lEnd = ::mmioSeek ( m_MmioHn, 0, SEEK_END );
	::mmioSeek ( m_MmioHn, 0, SEEK_SET );

	// DC See if file starts with 'R', 'I', 'F', 'F'
	FOURCC riff;
    if( ::mmioRead( m_MmioHn, (HPSTR)&riff, sizeof(riff) ) )
	{
		::mmioSeek ( m_MmioHn, 0, SEEK_SET );	// rewind
		if ( riff == FOURCC_RIFF )	// this is a RIFF file
		{
			if( ::mmioDescend( m_MmioHn, &ck, NULL, 0 ) == 0 )
			{
				if( ck.ckid == FOURCC_RIFF )
				{
					if( ck.fccType == mmioFOURCC('W','A','V','E') )
					{
						m_FormatType = 'P';     // PCM
						goto FORMAT_DECIDED;
					}
					if( ck.fccType == mmioFOURCC('R','M','P','3') )
					{
						m_FormatType = 'R';     // RMP
						goto FORMAT_DECIDED;
					}
				}
			}
		}
	}


    // ƒtƒ@ƒCƒ‹ƒtƒH[ƒ}ƒbƒg‚Ì”»•Ê
    if( ReadMp3FrameHeader() )
    {
        m_FormatType = 'M';         // MP3
        goto FORMAT_DECIDED;
    }

    return false;   // –¢‘Î‰ ƒtƒH[ƒ}ƒbƒg

FORMAT_DECIDED:

    // ƒf[ƒ^‚Ìæ“ª‚ğƒT[ƒ`
    switch( m_FormatType )
    {
    case 'P':
    case 'R':
        // 'data' ƒTƒuƒ`ƒƒƒ“ƒN‚ğ’T‚·
        sck.ckid = mmioFOURCC('d','a','t','a');
        if( ::mmioDescend( m_MmioHn, &sck, &ck, MMIO_FINDCHUNK ) ) return false;

        m_DataOffset = sck.dwDataOffset;

        // 'RIFF' ƒ`ƒƒƒ“ƒN‚ÉƒAƒZƒ“ƒh
        if( ::mmioAscend( m_MmioHn, &sck, 0 ) ) return false;
        break;

    case 'M':
        m_DataOffset = 0;
        break;
    }

    // ƒoƒbƒtƒ@ƒtƒH[ƒ}ƒbƒgİ’è
    switch( m_FormatType )
    {
    case 'P':
        // 'fmt ' ƒTƒuƒ`ƒƒƒ“ƒN‚ğ’T‚·
        if( ::mmioSeek( m_MmioHn, ck.dwDataOffset + sizeof(FOURCC), SEEK_SET )
             == -1 ) return false;

        sck.ckid = mmioFOURCC('f','m','t',' ');
        if( ::mmioDescend( m_MmioHn, &sck, &ck, MMIO_FINDCHUNK ) ) return false;
        
        // PCMWAVEFORMAT \‘¢‘Ì‚ğæ“¾
        PCMWAVEFORMAT pwf;
        if( sck.cksize < (LONG)sizeof(PCMWAVEFORMAT) ) return false;
        if( ::mmioRead( m_MmioHn, (HPSTR)&pwf, sizeof(pwf) ) != sizeof(pwf) ) return false;
        if( pwf.wf.wFormatTag != WAVE_FORMAT_PCM ) return false;
        
        memcpy( m_oBuffFormat, &pwf, sizeof(pwf) );
        m_oBuffFormat->cbSize = 0;
        
        // 'RIFF' ƒ`ƒƒƒ“ƒN‚ÉƒAƒZƒ“ƒh (©‚±‚ê—v‚ç‚ñ‚©‚à)
        if( ::mmioAscend( m_MmioHn, &sck, 0 ) ) return false;
        break;

    case 'R':
    case 'M':
        // Å‰‚ÌƒtƒŒ[ƒ€‚©‚çƒtƒH[ƒ}ƒbƒg‚ğ“¾‚é
        ::mmioSeek( m_MmioHn, m_DataOffset, SEEK_SET );

        if( !ReadMp3FrameHeader() ) return false;

        m_oBuffFormat->wFormatTag = WAVE_FORMAT_PCM;
        m_oBuffFormat->nChannels = m_oMp3WaveFormat->wfx.nChannels;
        m_oBuffFormat->nSamplesPerSec = m_oMp3WaveFormat->wfx.nSamplesPerSec;
        m_oBuffFormat->wBitsPerSample = 16;
        m_oBuffFormat->nBlockAlign = m_oBuffFormat->nChannels * 2;
        m_oBuffFormat->nAvgBytesPerSec = m_oBuffFormat->nSamplesPerSec
                                            * m_oBuffFormat->nBlockAlign;
        m_oBuffFormat->cbSize = 0;
        break;
    }

    // ƒI[ƒvƒ“Š®—¹
    m_bFileOpen = Reset();	// DC

	if (m_bFileOpen)
	{
		// DC - save filename
		m_strFileName = pFilename;
	}

	return m_bFileOpen;		// DC
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  McWaveReader::Close()

  ƒtƒ@ƒCƒ‹ƒNƒ[ƒY
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void McWaveReader::Close( void )
{
    // ACM ƒXƒgƒŠ[ƒ€‚ğ•Â‚¶‚é
    CloseAcmStream();

    // MMIO ‚ğ•Â‚¶‚é
    if( m_MmioHn != NULL )
    {
        ::mmioClose( m_MmioHn, 0 );
        m_MmioHn = NULL;
    }

	m_strFileName.Empty();	// DC
	m_bFileOpen = FALSE;	// DC
}



// DC
bool McWaveReader::IsOpen( void )
{
	return m_bFileOpen;
}



// DC
LPCTSTR McWaveReader::GetFileName()
{
	return m_strFileName;
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  McWaveReader::CloseAcmStream()

  ACM ƒXƒgƒŠ[ƒ€‚ğ•Â‚¶‚é
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void McWaveReader::CloseAcmStream( void )
{
    // ƒwƒbƒ_‚ğŠJ•ú
    if( m_oAsh != NULL )
    {
        if( m_oAsh->fdwStatus != 0 )
        {
            ::acmStreamUnprepareHeader( m_AcmHn, m_oAsh, 0 );
        }
        delete m_oAsh;
        m_oAsh = NULL;
    }

    // ƒXƒgƒŠ[ƒ€ƒnƒ“ƒhƒ‹‚ğƒNƒ[ƒY
    if( m_AcmHn != NULL )
    {
        ::acmStreamClose( m_AcmHn, 0 );
        m_AcmHn = NULL;
    }

    // ƒoƒbƒtƒ@‚ğŠJ•ú
	if (m_aFileBuffer)
	{
		delete[] m_aFileBuffer;
		m_aFileBuffer = NULL;
	}

    if (m_aDecBuffer)
	{
		delete[] m_aDecBuffer;
		m_aDecBuffer = NULL;
	}
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  McWaveReader::Reset()

  ƒI[ƒfƒBƒIƒf[ƒ^‚ğAæ“ª‚©‚ç“Ç‚İo‚¹‚éó‘Ô‚É‚·‚éB
  Šeíƒƒ“ƒo•Ï”‚Ì‰Šú‰»AACM ƒXƒgƒŠ[ƒ€‚ÌƒI[ƒvƒ““™B

>>  ¬”Û
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool McWaveReader::Reset( void )
{
    ::mmioSeek( m_MmioHn, m_DataOffset, SEEK_SET );
    m_Progress = 0;
    m_DecRemainderSize = 0;
    m_bFoundEnd = false;

    if( m_FormatType == 'P' ) return true;  // PCM ƒf[ƒ^‚È‚ç‚±‚±‚Ü‚Å

    // ACM ƒXƒgƒŠ[ƒ€‚ğ‰Šú‰»
    CloseAcmStream();

    if( !ReadMp3FrameHeader() ) goto RESET_FAILED;

    if( ::acmStreamOpen( &m_AcmHn, NULL,
                            (WAVEFORMATEX*)m_oMp3WaveFormat, m_oBuffFormat, NULL, 0, 0, 0 )
        != 0 ) goto RESET_FAILED;

    // ƒtƒ@ƒCƒ‹ƒoƒbƒtƒ@‚ğŠm•Û
    m_aFileBuffer = new BYTE[ m_FileBuffSize ];
    if( m_aFileBuffer == NULL ) goto RESET_FAILED;

    // ƒfƒR[ƒhƒoƒbƒtƒ@‚ğŠm•Û
    if( ::acmStreamSize( m_AcmHn, m_FileBuffSize, &m_DecBuffSize, ACM_STREAMSIZEF_SOURCE )
        != 0 ) goto RESET_FAILED;

    m_aDecBuffer = new BYTE[ m_DecBuffSize ];
    if( m_aDecBuffer == NULL ) goto RESET_FAILED;

    // ACM ƒXƒgƒŠ[ƒ€ƒwƒbƒ_€”õ
    m_oAsh = new ACMSTREAMHEADER;
    memset( m_oAsh, 0, sizeof(ACMSTREAMHEADER) );

    m_oAsh->cbStruct = sizeof(ACMSTREAMHEADER);
    m_oAsh->pbSrc = m_aFileBuffer;
    m_oAsh->cbSrcLength = m_FileBuffSize;
    m_oAsh->pbDst = m_aDecBuffer;
    m_oAsh->cbDstLength = m_DecBuffSize;
    if( ::acmStreamPrepareHeader( m_AcmHn, m_oAsh, 0 )
        != 0 ) goto RESET_FAILED;

    return true;

RESET_FAILED:
    Close();
    return false;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  McWaveReader::ReadMp3FrameHeader()

  Œ»İ‚Ìƒtƒ@ƒCƒ‹ˆÊ’u‚©‚ç MP3 ƒtƒŒ[ƒ€ƒwƒbƒ_‚ğ“Ç‚ŞB
  ƒtƒ@ƒCƒ‹ƒ|ƒCƒ“ƒ^‚Íi‚ß‚È‚¢B

>>  ¬”Û
    m_Mp3WaveFormat     “WŠJ‚³‚ê‚½ƒtƒŒ[ƒ€ƒwƒbƒ_î•ñ
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static const unsigned s_Freqs[2][3] = {
    { 44100, 48000, 32000 },    // MPEG1
    { 22050, 24000, 16000 },    // MPEG2
};

static const unsigned s_Bitrates[2][15] = {
    { 0, 32, 40, 48, 56, 64, 80, 96,112,128,160,192,224,256,320 },  // MPEG1
    { 0,  8, 16, 24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160 },  // MPEG2
};	// ‚ ‚ç‚©‚¶‚ß 1000 ”{‚µ‚Ä‚¨‚¢‚Ä‚à—Ç‚¢

bool McWaveReader::ReadMp3FrameHeader( void )
{
	LONG lOrigPos = ::mmioSeek( m_MmioHn, 0, SEEK_CUR );	

	// Determine if this is an MP3 file with an ID3v2 header
	uchar ID3v2Header[10];	// ID3v2 header is 10 bytes long
    if( ::mmioRead( m_MmioHn, (HPSTR)&ID3v2Header, sizeof(ID3v2Header) ) )
	{
		// Is this an ID3v2 header?
		if (	(memcmp ( "ID3", ID3v2Header, 3 ) == 0) &&	// starts with "ID3"
				(ID3v2Header[3] <= 3) )						// version <= 3
		{
			// Yes, skip past it
			int28 temp = &ID3v2Header[6];
			lsint tagSize = temp.get();
			if (!::mmioSeek( m_MmioHn, tagSize, SEEK_CUR ))
			{
				::mmioSeek( m_MmioHn, lOrigPos, SEEK_SET );	
				return false;
			}
		}
		else	// not an ID3v2 header
		{	    
			// reset file pos
			::mmioSeek( m_MmioHn, lOrigPos, SEEK_SET );	
		}
	}


	// File is now positioned to first possible MP3 header

	// Note:  the header is in big Endian order
	// w.r.t. doc\frame_header.htm, the bit order is:
	// IIJJKLMM EEEEFFGH AAABBCCD AAAAAAAA

	// Search for MP3 header by reading up to 4 KB, looking for the sync byte
	BYTE buf[4 * 1024];
    LONG lRead = ::mmioRead( m_MmioHn, (HPSTR)&buf, sizeof(buf) );
	if (lRead <= 0)	// not even 1 byte was read
	{
		::mmioSeek( m_MmioHn, lOrigPos, SEEK_SET );	
		return false;
	}


	// Look for sync bits, starting from beginning of buffer
    DWORD header = 0;
	BYTE *pEnd = buf + lRead - sizeof(DWORD);	// stop looking short 1 WORD so casting p to (WORD *) doesn't cause access violation
	for (BYTE *p = buf; p < pEnd; p++)
	{
	    // All 11 A (sync bits) are 1; BB == 10 or 11 (MPEG version 1 or 2)
		if ( ((*(WORD *) p) & 0xf0ff) == 0xf0ff )
		{
			// Found it
			header = (*(DWORD *) p);

			// Position file back to header
			//LONG lOfs = lRead - (p - buf);
			//::mmioSeek ( m_MmioHn, -lOfs, SEEK_CUR );

			// Stop looking
			break;
		}
	}

	::mmioSeek( m_MmioHn, lOrigPos, SEEK_SET );	
    if ( !header ) return false;


// *MEMO*  mp3 frame header
//  MSB
//    7   6   5   4   3   2   1   0   
//    <ch    ><m.ex  ><cr><or><emp   >
//    <bitrate       ><smp   ><pt><ex>
//    <sync          ><id><layer ><ep>
//    <sync                          >
//  LSB


    // CC == 01 (Layer 3)
    if( (header & 0x0600) != 0x0200 ) return false;

    // G (padding)
    int patend  = (header >> 17) & 1;

	// index into s_Freqs and s_Bitrates table to select MPEG1 or MPEG2
    int version = (header >> 11) & 1 ^ 1;

	// bitrate index
    int bitrate = (header >> 20) & 0x0f;


    // MPEGLAYER3WAVEFORMAT 
    MPEGLAYER3WAVEFORMAT* p_wf = m_oMp3WaveFormat;
    p_wf->wfx.wFormatTag = WAVE_FORMAT_MPEGLAYER3;
    p_wf->wfx.cbSize = MPEGLAYER3_WFX_EXTRA_BYTES;
    p_wf->wfx.nAvgBytesPerSec = s_Bitrates[version][bitrate] * 1000 / 8;
    p_wf->wfx.nBlockAlign = 1;

    p_wf->wfx.nSamplesPerSec = s_Freqs[version][(header >> 18) & 2];
    p_wf->wfx.wBitsPerSample = 0;
    p_wf->nFramesPerBlock = 1;

	// II == 11 (Single channel)
    if( (header & 0xc0000000) == 0xc0000000 )
    {
        p_wf->wfx.nChannels = 1;
    }
    else
    {
        p_wf->wfx.nChannels = 2;
    }

    p_wf->wID = MPEGLAYER3_ID_MPEG;
    p_wf->fdwFlags = patend;
    p_wf->nBlockSize = (WORD)(s_Bitrates[version][bitrate] * 1000 * 144 / p_wf->wfx.nSamplesPerSec + patend);

    p_wf->nFramesPerBlock = 1;
//  p_wf->nCodecDelay = 700;
    p_wf->nCodecDelay = 0x0571; // ‚Ç‚Á‚¿‚ª—Ç‚¢‚ñ‚¾‚ë‚¤cc

	// H == 1 (Private bit)
    if( (header & 0x0100) == 0 ) p_wf->nBlockSize += 2;

    return true;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  McWaveReader::Read()

  ƒtƒ@ƒCƒ‹‚©‚çƒf[ƒ^‚ğ“Ç‚İ‚İA•K—v‚È‚ç•ÏŠ·‚ğ{‚µA‚Äƒoƒbƒtƒ@‚Éo—Í

<<  Size        —v‹o—ÍƒTƒCƒY
    pBuffer     o—Íƒoƒbƒtƒ@

>>  ¬”Û
    *pWritten   ÀÛ‚Éo—Í‚³‚ê‚½ƒTƒCƒY
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
bool McWaveReader::Read( DWORD Size, BYTE* pBuffer, DWORD* pWritten )
{
    switch( m_FormatType )
    {
    case 'P':
        return Read_Pcm( Size, pBuffer, pWritten );

    case 'R':
    case 'M':
        return Read_Acm( Size, pBuffer, pWritten );
    }

    return false;
}

// for PCM
bool McWaveReader::Read_Pcm( DWORD Size, BYTE* pBuffer, DWORD* pWritten )
{
    DWORD written;

    written = ::mmioRead( m_MmioHn, (HPSTR)pBuffer, Size );

    if( written == -1 ) return false;   // error

    *pWritten = written;
    m_Progress += written;
    return true;
}

// for RMP, MP3
bool McWaveReader::Read_Acm( DWORD Size, BYTE* pBuffer, DWORD* pWritten )
{
    DWORD written = 0;

    // ‘O‰ñ‚ÌƒfƒR[ƒhÏ‚İƒf[ƒ^‚Ìc‚è‚ª‚ ‚ê‚ÎA‚»‚ê‚ğ“]‘—
    if( m_DecRemainderSize != 0 )
    {
        if( Size <= m_DecRemainderSize )
        {
            memcpy( pBuffer, m_pDecRemainder, Size );
            m_DecRemainderSize -= Size;
            m_pDecRemainder += Size;
            written = Size;
            goto EXIT_READ_ACM;
        }
        else
        {
            memcpy( pBuffer, m_pDecRemainder, m_DecRemainderSize );
            Size -= m_DecRemainderSize;
            pBuffer += m_DecRemainderSize;
            written = m_DecRemainderSize;
            m_DecRemainderSize = 0;
        }
    }

    for(;;)
    {
        // ƒfƒR[ƒh
        DWORD dec_size;
        dec_size = Decode();
        if( dec_size == 0 ) break;
        
        // ƒoƒbƒtƒ@‚ÖƒRƒs[
        if( Size < dec_size )
        {
            memcpy( pBuffer, m_aDecBuffer, Size );
            m_DecRemainderSize = dec_size - Size;
            m_pDecRemainder = m_aDecBuffer + Size;
            written += Size;
            break;
        }
        else
        {
            memcpy( pBuffer, m_aDecBuffer, dec_size );
            Size -= dec_size;
            pBuffer += dec_size;
            written += dec_size;
            
            if( Size == 0 ) break;  // —v‹ƒTƒCƒY‚¿‚å‚¤‚Ç‘‚«‚ß‚½
        }
    }

EXIT_READ_ACM:
    *pWritten = written;
    m_Progress += written;
    return true;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  McWaveReader::Decode()

  ˆ³kƒI[ƒfƒBƒI‚ğƒtƒ@ƒCƒ‹ƒoƒbƒtƒ@•ªƒfƒR[ƒh‚·‚é

>>  ƒfƒR[ƒh‚Å‚«‚½ƒf[ƒ^‚ÌƒTƒCƒY
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
DWORD McWaveReader::Decode( void )
{
    if( m_bFoundEnd ) return 0;     // ‘O‰ñ‚ªƒf[ƒ^I’[

    long read_size;

    do {
        // ƒf[ƒ^‚ğƒtƒ@ƒCƒ‹ƒoƒbƒtƒ@‚É“Ç‚İ‚±‚Ş
        read_size = ::mmioRead( m_MmioHn, (HPSTR)m_aFileBuffer, m_FileBuffSize );
        if( read_size == 0 || read_size == -1 )
        {
            m_bFoundEnd = true;
            return 0;
        }

        // ƒRƒ“ƒo[ƒgÀs
        m_oAsh->cbSrcLength = read_size;

        if( ::acmStreamConvert( m_AcmHn, m_oAsh, 0 )
            != 0 ) return 0;

        // ƒf[ƒ^I’[ (not ƒtƒ@ƒCƒ‹I’[) ‚©H
        if( m_oAsh->cbSrcLengthUsed < m_oAsh->cbSrcLength )
        {
			// DC for some reason, ::acmStreamConvert() chose not to decode all the source bytes!
			// Maybe this is not EOF condition...
#if 0
			// Orig:
            m_bFoundEnd = true;
#else
			// position file to re-read bytes that were not decoded
            ::mmioSeek(m_MmioHn, -(LONG)(m_oAsh->cbSrcLength - m_oAsh->cbSrcLengthUsed), SEEK_CUR);
#endif
            break;
        }

    } while( m_oAsh->cbDstLengthUsed == 0 );    // ƒfƒR[ƒhÏ‚İƒf[ƒ^‚ªo‚Ä‚­‚é‚Ü‚ÅŒJ‚è•Ô‚·

    return m_oAsh->cbDstLengthUsed;
}




// DC - Added Seek function

bool McWaveReader::Seek (LONG lTo)
{
	if ( !IsOpen() || m_FormatType != 'P' )	// no file or not PCM file
		return false;


	LONG lCur = ::mmioSeek (m_MmioHn, 0, SEEK_CUR );
	if (lTo < 0)
		lTo = 0;
	else if (m_DataOffset + lTo > m_lEnd)
		lTo = m_lEnd;


	// copied from Reset()
	if ( ::mmioSeek( m_MmioHn, m_DataOffset + lTo, SEEK_SET ) == -1 ) 
	{
		ASSERT (FALSE);
		return false;
	}

	m_Progress = lTo;
    m_DecRemainderSize = 0;
    m_bFoundEnd = false;

	return true;
}

