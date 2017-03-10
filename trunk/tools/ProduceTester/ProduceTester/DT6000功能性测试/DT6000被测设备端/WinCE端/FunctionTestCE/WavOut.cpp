#include "stdafx.h"
#include <mmsystem.h>
#include "WavOut.h"
#include <tchar.h>
#include "assertMsg.h"

#ifndef  WINCE
#include <process.h>
#endif


#define     DIV_FACTORS     5       //�������Ĵ�С�൱��1s���ݵļ���֮һ


#pragma pack(2) 
struct RIFFChunk                    /* ����*.wav RIFF��*/
{
    char     chunkid[4];            /* RIFF���ʶ*/
    UINT32   uiChunkSize;
    char     waveID[4];             /* *.wav�ļ���ʶ*/
};

struct FmtChunk                     /* define FmtChunk*/
{
    char        chunkid[4];
    UINT32      uChunkSize;
    WAVEFORMATEX wf;
};

struct DataChunkHeader
{
    char chunkid[4];
    UINT32 uiChuankSize;
};


#pragma pack()

CWavOut::CWavOut():
m_hSemaphore(NULL),
m_iDataStart(0),
m_pWavFile(NULL),
m_iRequreLen(0),
m_WaveOutHandle(NULL),
m_bLoop(FALSE),
m_hPlayThread(NULL),
m_hCmdEvent(NULL),
m_hCmdEventRespond(NULL),
m_uiAudioTimeLen(0),
m_uiPlayedBuffers(0)
{
    InitializeCriticalSection(&m_cs);    
}



CWavOut::~CWavOut()
{
    if (m_pWavFile)
    {
        Stop();
        CloseWavFile();
    }
    DeleteCriticalSection(&m_cs);

}

void CWavOut::Stop()
{
    EnterCriticalSection(&m_cs);
    m_bLoop=FALSE;

    if (m_pWavFile !=NULL)
    {
        fseek(m_pWavFile ,0,SEEK_END);
    }
    
    if (m_WaveOutHandle !=NULL)
    {
        waveOutReset(m_WaveOutHandle);
    }
    LeaveCriticalSection(&m_cs);

    if (m_hPlayThread !=NULL)
    {
        WaitForSingleObject(m_hPlayThread,INFINITE);
        CloseHandle(m_hPlayThread);
        m_hPlayThread=NULL;
    }
}


void CWavOut::Pause()
{
    EnterCriticalSection(&m_cs);
    assert(m_WaveOutHandle!=0);  
    if (m_WaveOutHandle !=NULL)
    {
        waveOutPause(m_WaveOutHandle);
    }
    
    LeaveCriticalSection(&m_cs);
}

void CWavOut::Resume()
{
    EnterCriticalSection(&m_cs);
    assert(m_WaveOutHandle!=0);  
    if (m_WaveOutHandle !=NULL)
    {
        waveOutRestart(m_WaveOutHandle);
    }

    LeaveCriticalSection(&m_cs);
}



void CWavOut::CloseWavFile()
{
    EnterCriticalSection(&m_cs);
    assertMsg(m_WaveOutHandle==NULL,L"��ǰ�ƺ��ļ����ڲ���");
    if (m_pWavFile !=NULL)
    {
        fclose(m_pWavFile);
        m_pWavFile=NULL;
    }

    LeaveCriticalSection(&m_cs);
}


BOOL CWavOut::OpenWavFile(LPCWSTR lpszFilename)
{
    assertMsg(m_pWavFile==NULL,L"�ƺ��Ѿ�����һ���ļ������ȵ���CloseWavFile�ر��ļ�");

    if( (m_pWavFile=_wfopen(lpszFilename,TEXT("rb")))==NULL )
    {      
        return FALSE;
    }
    
    struct RIFFChunk riffchunk;
    struct FmtChunk fmtchunk;
    UINT32 iTempLen=0;
    char   cpTempChar[4];   
    
    /* ����riff��*/
    fread(&riffchunk,sizeof(struct RIFFChunk),1,m_pWavFile);
    if(    riffchunk.chunkid[0]!='R'   \
        || riffchunk.chunkid[1]!='I'   \
        || riffchunk.chunkid[2]!='F'   \
        || riffchunk.chunkid[3]!='F')
    {
        goto error1;
    }
    if(    riffchunk.waveID[0]!='W'   \
        || riffchunk.waveID[1]!='A'   \
        || riffchunk.waveID[2]!='V'   \
        || riffchunk.waveID[3]!='E')
    {
        goto error1;
    }

    /* ����fmt��*/
    fmtchunk.uChunkSize=0;
    do                                                                  /*   ����fmt��*/
    {
        fseek(m_pWavFile,(long)fmtchunk.uChunkSize,SEEK_CUR);
        fread(&fmtchunk,8,1,m_pWavFile);
        
    } while(  (fmtchunk.chunkid[0]!='f'   \
        || fmtchunk.chunkid[1]!='m'   \
        || fmtchunk.chunkid[2]!='t'   \
        || fmtchunk.chunkid[3]!=' ')  \
        && !feof(m_pWavFile));

    fread(&m_wf,sizeof(WAVEFORMATEX)-sizeof(WORD),1,m_pWavFile);
    if(feof(m_pWavFile))
        goto error1;
    if(m_wf.wFormatTag!=1)
        goto error1;    
   
    if (fmtchunk.uChunkSize>sizeof(WAVEFORMATEX)-sizeof(WORD))
      fread(&m_wf.cbSize,sizeof(WORD),1,m_pWavFile);     
    else
         m_wf.cbSize=0;
         fseek(m_pWavFile,m_wf.cbSize,SEEK_CUR);    
    
    /*����data��*/
    do{
        fseek(m_pWavFile,(long)iTempLen,SEEK_CUR);
        fread(cpTempChar,1,4,m_pWavFile);
        fread(&iTempLen,sizeof(UINT32),1,m_pWavFile);
    }while (  (cpTempChar[0]!='d'  \
        || cpTempChar[1]!='a'  \
        || cpTempChar[2]!='t'  \
        || cpTempChar[3]!='a' )\
        && !feof(m_pWavFile));

    assert(iTempLen);                       //���ݳ���Ϊ0�ľ���
    if(feof(m_pWavFile) || iTempLen==0)
    {
        goto error1;
    }
    m_iDataStart=ftell(m_pWavFile);
    fseek(m_pWavFile,0,SEEK_END);

    unsigned long t=ftell(m_pWavFile);
    fseek(m_pWavFile,m_iDataStart,SEEK_SET);
    m_uiAudioTimeLen=(unsigned int )( (double)(t-m_iDataStart)/m_wf.nAvgBytesPerSec*1000 );

    return TRUE;  
error1:
    if(m_pWavFile!=NULL)
        fclose(m_pWavFile);
    m_pWavFile=NULL;
    return FALSE;
}

 BOOL CWavOut::Play(BOOL bLooped/* =FALSE */)
{
    assertMsg(m_WaveOutHandle==0,L"��ǰ�ƺ��Ѿ��ڲ�����");
    if (m_hPlayThread !=NULL)
    {
        CloseHandle(m_hPlayThread);
        m_hPlayThread=NULL;
    }

    int         len;    
    int         ret;

    m_bLoop=bLooped;
    m_uiPlayedBuffers=0;
    //�����ź���
    m_hSemaphore=CreateSemaphore(NULL,0,5,NULL);
    if(m_hSemaphore ==NULL)
    {
        return FALSE;
    }
    //���������¼�����
    m_hCmdEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
    m_hCmdEventRespond=CreateEvent(NULL,FALSE,FALSE,NULL);

    if (m_hCmdEventRespond==NULL || m_hCmdEvent ==NULL )
    {
        goto error;
    }


    //����200ms��������
    m_iRequreLen=m_wf.nAvgBytesPerSec/DIV_FACTORS; 
    //�򿪲����豸
    m_iBufferNum=0;
    ret=waveOutOpen(&m_WaveOutHandle,WAVE_MAPPER,&m_wf,(DWORD_PTR)waveOutProc,(DWORD_PTR)this,CALLBACK_FUNCTION);
    if(ret!=MMSYSERR_NOERROR)
    {
        assert(ret!=MMSYSERR_NOERROR);
        goto error;
    }  
    memset(&WaveHdr1,0,sizeof(WAVEHDR));
    memset(&WaveHdr2,0,sizeof(WAVEHDR));
    
    //�����ڴ�
    pBuffer1=(char *)malloc(m_iRequreLen+10);
    pBuffer2=(char *)malloc(m_iRequreLen+10);

    if (pBuffer1==NULL ||pBuffer2==NULL)
    {
        goto error;
    }

    
    fseek(m_pWavFile,m_iDataStart,SEEK_SET);
    //Ԥ��400ms��������������
    BOOL        bRet=FALSE;
    bRet=FillTheAudioBuffer(pBuffer1,len);
    if (len<=0)
    {
        goto error;             //û�ж�����Ƶ����
    }
    WaveHdr1.lpData=pBuffer1;   
    WaveHdr1.dwBufferLength=len;    
    m_iBufferNum++; 

   waveOutPrepareHeader(m_WaveOutHandle,&WaveHdr1,sizeof(WAVEHDR));    
   if (bRet)
   {
       //��һ���������Ա������������ڶ�������
       FillTheAudioBuffer(pBuffer2,len);
       if (len >0)
       {
           WaveHdr2.lpData=pBuffer2; 
           WaveHdr2.dwBufferLength=len;       
           m_iBufferNum++; 
           waveOutPrepareHeader(m_WaveOutHandle,&WaveHdr2,sizeof(WAVEHDR));
       }          
   }
   //����ȡ������Ƶ���ݴ����豸
   waveOutWrite(m_WaveOutHandle,&WaveHdr1,sizeof(WAVEHDR)); 
   if (len >0)
   {
       waveOutWrite(m_WaveOutHandle,&WaveHdr2,sizeof(WAVEHDR));
   }
   
   //���������߳�
#ifndef  WINCE
    m_hPlayThread=(HANDLE)_beginthreadex(NULL,0,(unsigned int (__stdcall *)(void *))PlayWavProc,
        this,0,0);
#else
   m_hPlayThread=CreateThread(NULL,0,PlayWavProc,this,0,0);
#endif
   
   if (m_hPlayThread ==NULL )
   {
       goto error;
   }

   return TRUE;
error:   

   if (m_hCmdEvent)
   {
       CloseHandle(m_hCmdEvent);
       m_hCmdEvent=NULL;
   }
   if (m_hCmdEventRespond)
   {
       CloseHandle(m_hCmdEventRespond);
       m_hCmdEventRespond=NULL;
   }

   if (m_hSemaphore !=NULL)
   {
       CloseHandle(m_hSemaphore);
       m_hSemaphore=NULL;
   }
   if (m_WaveOutHandle !=NULL)
   {
       waveOutClose(m_WaveOutHandle);
       m_WaveOutHandle=NULL;
   }
   free(pBuffer1);
   free(pBuffer2);

   m_iBufferNum=0;
   m_WaveOutHandle=0;
   return FALSE;
}

unsigned int CWavOut::GetLength()
{
    return m_uiAudioTimeLen;
}

BOOL  CWavOut::GetPos(WAV_POS_INFO &pos)
{
    m_cmdCode=CMD_GET_POS_INFO;
    if (m_hCmdEvent !=NULL)
    {
        SetEvent(m_hCmdEvent);

        DWORD dwRet=WaitForSingleObject(m_hCmdEventRespond,INFINITE);
        if (dwRet !=WAIT_OBJECT_0 )
        {
            m_posInfo.bRun=FALSE;        
        }

        memcpy(&pos,&m_posInfo,sizeof(pos));
    }
    else
    {
        pos.bRun=FALSE;
    }
    return TRUE;
    
}
// 
// BOOL CWavOut::IsSeekAble()
// {
//     return TRUE;
// }
// 
// BOOL CWavOut::SeekTo(int iTime)
// {
//     assert(m_WaveOutHandle!=0);
//     EnterCriticalSection(&m_cs);
//     if(m_IsPlaying && !m_IsStop)
//     {       
//         m_iCurPos=m_wf.nAvgBytesPerSec/10*iTime;
//         fseek(m_pWavFile,m_iCurPos+m_iDataStart,SEEK_SET);
//         m_iSeekNum=m_iBufferNum;   
//         LeaveCriticalSection(&m_cs);
//         waveOutReset(m_WaveOutHandle);
//         return TRUE;
//     }
//     LeaveCriticalSection(&m_cs);
//     return FALSE;
// }
// 
void CALLBACK CWavOut::waveOutProc( HWAVEOUT hwo,  UINT uMsg,DWORD_PTR dwInstance,
                                   DWORD_PTR dwParam1, DWORD dwParam2 )
{
    if(uMsg==WOM_DONE)
    {  
        CWavOut *pWavOut=(CWavOut *)dwInstance;
        pWavOut->m_queue.InQueue((WAVEHDR *)dwParam1);
        ReleaseSemaphore(pWavOut->m_hSemaphore,1,NULL);
    }
}

void CWavOut::OnFinishPlay()
{
    free(pBuffer2);
    free(pBuffer1);
    pBuffer1=NULL;
    pBuffer2=NULL;

    if (m_WaveOutHandle !=NULL)
    {
        waveOutClose(m_WaveOutHandle);       
        m_WaveOutHandle=NULL;  
    }

    if (m_hSemaphore !=NULL)
    {
        CloseHandle(m_hSemaphore);
        m_hSemaphore=NULL;
    }
    
    if (m_hCmdEvent)
    {
        CloseHandle(m_hCmdEvent);
        m_hCmdEvent=NULL;
    }

    if (m_hCmdEventRespond)
    {
        CloseHandle(m_hCmdEventRespond);
        m_hCmdEventRespond=NULL;
    }

}

DWORD CWavOut::PlayWavProc(void *pVoid)
{
    CWavOut *pWavOut=(CWavOut *)pVoid;
    WAVEHDR *pHeader;
    int len;
    while (1) 
    {
        HANDLE h[2]={pWavOut->m_hSemaphore,pWavOut->m_hCmdEvent};
        DWORD dwRet=WaitForMultipleObjects(2,h,FALSE,INFINITE);
        
        if (dwRet==WAIT_OBJECT_0)
        {
            EnterCriticalSection(&pWavOut->m_cs);
            pWavOut->m_queue.OutQueue(pHeader);
            waveOutUnprepareHeader(pWavOut->m_WaveOutHandle,pHeader,sizeof(WAVEHDR));
            pWavOut->m_uiPlayedBuffers++;

            pWavOut->FillTheAudioBuffer(pHeader->lpData,len);       
            if(len>0)
            {
                pHeader->dwBufferLength=len;
                waveOutPrepareHeader(pWavOut->m_WaveOutHandle,pHeader,sizeof(WAVEHDR));
                waveOutWrite(pWavOut->m_WaveOutHandle,pHeader,sizeof(WAVEHDR));
            }
            else
            {
                pWavOut->m_iBufferNum--;
                if (pWavOut->m_iBufferNum==0)
                {
                    //�����ǰ�Ѿ�û�л������ڲ��ţ���ֹͣ����
                    pWavOut->OnFinishPlay();
                    LeaveCriticalSection(&pWavOut->m_cs);
                    break;
                }            
            }

            LeaveCriticalSection(&pWavOut->m_cs);
        }
        else
        {
            EnterCriticalSection(&pWavOut->m_cs);
            switch(pWavOut->m_cmdCode)
            {
            case  CMD_GET_POS_INFO:
                {
                    MMTIME mmTime;
                    mmTime.wType = TIME_BYTES;
                    waveOutGetPosition(pWavOut->m_WaveOutHandle,&mmTime,sizeof(mmTime));
                    assertMsg(mmTime.wType==TIME_BYTES,L"�˴��в�֧�ִ����豸" );
                   
                    UINT64  uiBytes=0;          //�Ѿ����ŵ��ֽ���
                    UINT64  uiTmp;

                    WORD nBlockAlign=pWavOut->m_wf.nBlockAlign;
                    WORD nBytePerSample=(pWavOut->m_wf.wBitsPerSample)/8;

                    //���㵱ǰ�����Ѿ����ŵ��ֽ���
                    uiTmp =(pWavOut->m_uiPlayedBuffers)*
                        (pWavOut->m_wf.nAvgBytesPerSec/DIV_FACTORS);

                    //��ֹmmTime�е��ֽ������
                    uiBytes=mmTime.u.cb;
                    while(uiBytes <uiTmp)
                    {
                        uiBytes+=((UINT64)(MAXDWORD)+1);
                    }

                    //����Ĵ�����Ϊ�˷�ֹ��ȡ����ʱ������ֵ
                    if (uiBytes >nBlockAlign)
                    {
                        uiBytes-=nBlockAlign;
                    }

                    pWavOut->m_posInfo.bRun=TRUE;
                    pWavOut->m_posInfo.uiSamples=(uiBytes)*1000/pWavOut->m_wf.nAvgBytesPerSec;

                    //���㵱ǰ�Ĳ���λ��λ���ĸ�������
                    UINT64 bufferIndex=(uiBytes)/(pWavOut->m_wf.nAvgBytesPerSec/DIV_FACTORS);
                    UINT    pos=(UINT)( (uiBytes)%(pWavOut->m_wf.nAvgBytesPerSec/DIV_FACTORS) );
                    bufferIndex =bufferIndex%2;

                    char *pBuffer= bufferIndex==0?pWavOut->pBuffer1:pWavOut->pBuffer2;

                    INT16           v;
                    UINT            vt=0;

                    assertMsg((pWavOut->m_wf.wBitsPerSample)%8==0 ,
                        L"�ݲ�֧�ֲ������Ȳ�Ϊ8λ����������wav�ļ�" );                   

                    for (int i=0;i<nBytePerSample;i++)
                    {
                        BYTE b=pBuffer[pos+i] ;
                        vt|=(UINT)(b<< (i*8) );
                    }

                    if (nBytePerSample ==1)
                    {
                        v=(vt-128)*256;
                    }
                    else if (nBytePerSample ==2)
                    {
                        v=(INT16)vt;
                    }
                    else
                    {
                        v=(INT16)v>>((nBytePerSample-2)*8 );
                    }
                    pWavOut->m_posInfo.iVolume=v; 
                }
                break;
            default:
                break;
            }
            SetEvent(pWavOut->m_hCmdEventRespond);
            LeaveCriticalSection(&pWavOut->m_cs);
        }

    }
    return 0;
}

BOOL CWavOut::FillTheAudioBuffer(char *pBuffer,int &iReadLen)
{
    int     len=0;
    int     ReadCount=0;

RETRY:
    len=(int )fread(pBuffer+ReadCount,1,(m_iRequreLen-ReadCount),m_pWavFile);
    ReadCount+=len;

    if (ReadCount <m_iRequreLen)
    {
        if (m_bLoop)
        {
            fseek(m_pWavFile,m_iDataStart,SEEK_SET);
            goto RETRY;
        }
    }

    iReadLen=ReadCount;    
    return ReadCount >=m_iRequreLen?TRUE:FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//������Ƶ������
CWavIn::CWavIn():
m_pWavFile(NULL),
m_hWaveInHandle(NULL),
m_hSemaphore(NULL),
m_hCmdEvent(NULL),
m_hCmdEventRespond(NULL),
m_hRecordThread(NULL)
{
    //�趨Ĭ�ϵ���Ƶ�����ʽ
    m_wf.wFormatTag=1;
    m_wf.cbSize=0;

    m_wf.nChannels=1;               //��ͨ��
    m_wf.nSamplesPerSec=44100;
    m_wf.wBitsPerSample=16;
    m_wf.nBlockAlign=2;
    m_wf.nAvgBytesPerSec=88200;

    InitializeCriticalSection(&m_cs);
}

CWavIn::~CWavIn()
{
    DeleteCriticalSection(&m_cs);
}

BOOL CWavIn::CreateWavFile(LPCWSTR strWavFile)
{
    assertMsg(m_pWavFile==NULL,L"�ƺ��Ѿ�����һ��¼���ļ�");

    //��¼�������ɵ�wav�ļ�ֻ����RIFF,fmt,data������
    m_pWavFile=_wfopen(strWavFile,L"wb");
    if (m_pWavFile ==NULL)
    {
        return FALSE;
    }

    RIFFChunk   riff={{'R','I','F','F'},0 ,{'W','A','V','E'}};
    FmtChunk    fmtChank={{'f','m','t',' '},sizeof(m_wf)-sizeof(WORD),m_wf };
    DataChunkHeader dch={{'d','a','t','a'},0 };

    //д��RIFF�ļ�ͷ
    fwrite(&riff,sizeof(riff),1,m_pWavFile);
    //д��fmt��
    fwrite(&fmtChank,sizeof(fmtChank.chunkid)+sizeof(fmtChank.uChunkSize)+fmtChank.uChunkSize,
        1,m_pWavFile  );
    //д��data���ͷ
    fwrite(&dch,sizeof(dch),1,m_pWavFile);
    return TRUE;
}

void CWavIn::SetRecordFormat(const WAVEFORMATEX &wf)
{
    assertMsg(m_pWavFile==NULL,L"�������Ѿ���¼���ļ������������¼�Ƹ�ʽ");
    assertMsg(wf.cbSize==0,L"��֧�����ָ�ʽ��¼��");

    m_wf=wf;
}

BOOL CWavIn::BeginRecord()
{
    assertMsg(m_pWavFile !=NULL,L"¼����Ŀ���ļ���û�д���");
    assertMsg(m_hWaveInHandle==NULL,L"��ǰ�Ѿ�����¼��״̬��");

    m_uiRecordedBuffers=0;
    m_bStop=FALSE;

    //�������ڿ��Ƶ��ź������¼�����
    m_hSemaphore=CreateSemaphore(NULL,0,5,NULL);
    if(m_hSemaphore ==NULL)
    {
        return FALSE;
    }
    m_hCmdEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
    m_hCmdEventRespond=CreateEvent(NULL,FALSE,FALSE,NULL);
    if (m_hCmdEventRespond==NULL || m_hCmdEvent ==NULL )
    {
        goto error;
    }

    MMRESULT mmRet;
    //��¼���豸
    mmRet=waveInOpen(&m_hWaveInHandle,WAVE_MAPPER,&m_wf,
        (DWORD_PTR )waveInProc,(DWORD_PTR)this,CALLBACK_FUNCTION);
    if (mmRet !=MMSYSERR_NOERROR)
    {
        goto error;
    }

    memset(&WaveHdr1,0,sizeof(WAVEHDR));
    memset(&WaveHdr2,0,sizeof(WAVEHDR));
    //����ÿ����Ƶ�������ĳ���
    m_iRequreLen=m_wf.nAvgBytesPerSec/DIV_FACTORS;

    //�����ڴ�
    pBuffer1=(char *)malloc(m_iRequreLen+10);
    pBuffer2=(char *)malloc(m_iRequreLen+10);
    if (pBuffer1==NULL ||pBuffer2==NULL)
    {
        goto error;
    }

    WaveHdr1.dwBufferLength=m_iRequreLen;
    WaveHdr1.lpData=pBuffer1;
    WaveHdr2.dwBufferLength=m_iRequreLen;
    WaveHdr2.lpData=pBuffer2;

    waveInPrepareHeader(m_hWaveInHandle,&WaveHdr1,sizeof(WaveHdr1) );
    waveInPrepareHeader(m_hWaveInHandle,&WaveHdr2,sizeof(WaveHdr2) );
    m_iBufferNum=2;
    waveInAddBuffer(m_hWaveInHandle,&WaveHdr1,sizeof(WaveHdr1) );
    waveInAddBuffer(m_hWaveInHandle,&WaveHdr2,sizeof(WaveHdr2) );

    waveInStart(m_hWaveInHandle);
    //����¼���߳�
#ifndef  WINCE
    m_hRecordThread=(HANDLE)_beginthreadex(NULL,0,(unsigned int (__stdcall *)(void *))RecordWavProc,
        this,0,0);
#else
    m_hRecordThread=CreateThread(NULL,0,RecordWavProc,this,0,0);
#endif
    
    return TRUE;

error:
    if (m_hSemaphore !=NULL)
    {
        CloseHandle(m_hSemaphore);
        m_hSemaphore=NULL;
    }

    if (m_hCmdEvent !=NULL)
    {
        CloseHandle(m_hCmdEvent);
        m_hCmdEvent=NULL;
    }
    if (m_hCmdEventRespond !=NULL)
    {
        CloseHandle(m_hCmdEventRespond);
        m_hCmdEventRespond=NULL;
    }

    if (pBuffer1 )
    {
        free(pBuffer1);
        pBuffer1=NULL;
    }
    if (pBuffer2)
    {
        free(pBuffer2);
        pBuffer2=NULL;
    }

    if (m_hWaveInHandle !=NULL)
    {
        waveInClose(m_hWaveInHandle);
        m_hWaveInHandle=NULL;
    }      

    return FALSE;
}

BOOL CWavIn::Stop()
{
    m_bStop=TRUE;
    if (m_hWaveInHandle)
    {
        waveInReset(m_hWaveInHandle);
    }
    if (m_hRecordThread !=NULL)
    {
        WaitForSingleObject(m_hRecordThread,INFINITE);
        m_hRecordThread=NULL;
    }

    if (m_pWavFile)
    {
        //�������ݵĳ���
        fseek(m_pWavFile,0,SEEK_END);
        unsigned long lFileLen=ftell(m_pWavFile);
        
        UINT32 size=lFileLen-sizeof(RIFFChunk)+4 ;
        fseek(m_pWavFile,4,SEEK_SET );
        fwrite(&size,1,sizeof(size),m_pWavFile ) ;

        size=lFileLen-sizeof(RIFFChunk)-sizeof(FmtChunk)-sizeof(DataChunkHeader);
        fseek(m_pWavFile,sizeof(RIFFChunk)+sizeof(FmtChunk)-2+4,
            SEEK_SET);
        fwrite( &size,1,sizeof(size),m_pWavFile );
        fclose(m_pWavFile);
        m_pWavFile=NULL;        
    }
    return TRUE;
}

void CWavIn::waveInProc(HWAVEIN hwo,UINT uMsg,DWORD_PTR dwInstance, DWORD_PTR dwParam1,DWORD dwParam2)
{
    if(uMsg==WIM_DATA)
    {  
        CWavIn *pWavIn=(CWavIn *)dwInstance;
        pWavIn->m_queue.InQueue((WAVEHDR *)dwParam1);
        ReleaseSemaphore(pWavIn->m_hSemaphore,1,NULL);
    }
}

BOOL CWavIn::GetPos(WAV_POS_INFO &pos)
{
    m_cmdCode=CMD_GET_POS_INFO;
    if (m_hCmdEvent !=NULL)
    {
        SetEvent(m_hCmdEvent);

        DWORD dwRet=WaitForSingleObject(m_hCmdEventRespond,INFINITE);
        if (dwRet !=WAIT_OBJECT_0 )
        {
            m_posInfo.bRun=FALSE;        
        }

        memcpy(&pos,&m_posInfo,sizeof(pos));
    }
    else
    {
        pos.bRun=FALSE;
    }
    return TRUE;
}

DWORD CWavIn::RecordWavProc(void *pVoid)
{
    CWavIn *pWavIn=(CWavIn *)pVoid;
    WAVEHDR *pHeader;

    while (1) 
    {
        HANDLE h[2]={pWavIn->m_hSemaphore,pWavIn->m_hCmdEvent};
        DWORD dwRet=WaitForMultipleObjects(2,h,FALSE,INFINITE);

        if (dwRet==WAIT_OBJECT_0)
        {
            EnterCriticalSection(&pWavIn->m_cs);
            pWavIn->m_queue.OutQueue(pHeader);
            waveInUnprepareHeader(pWavIn->m_hWaveInHandle,pHeader,sizeof(WAVEHDR));
            pWavIn->m_uiRecordedBuffers++;

            //��¼�Ƶ���Ƶ����д���ļ�
            fwrite(pHeader->lpData,1,pHeader->dwBytesRecorded,pWavIn->m_pWavFile);

            if (!pWavIn->m_bStop)
            {
                pHeader->dwBufferLength=pWavIn->m_iRequreLen;
                waveInPrepareHeader(pWavIn->m_hWaveInHandle,pHeader,sizeof(WAVEHDR) );
                waveInAddBuffer(pWavIn->m_hWaveInHandle,pHeader,sizeof(WAVEHDR) );
            }
            else
            {
                pWavIn->m_iBufferNum--;
                if(pWavIn->m_iBufferNum==0)
                {
                    //���л��������Ѿ����أ��˳�¼���߳�
                    pWavIn->OnFinishRecord();
                    LeaveCriticalSection(&pWavIn->m_cs);
                    break;
                }

            }         

            LeaveCriticalSection(&pWavIn->m_cs);
        }
        else
        {
            //����������ôִ�ж��������
            EnterCriticalSection(&pWavIn->m_cs);
            switch(pWavIn->m_cmdCode)
            {
            case  CMD_GET_POS_INFO:
                {
                    MMTIME mmTime;
                    mmTime.wType = TIME_BYTES;
                    waveInGetPosition(pWavIn->m_hWaveInHandle,&mmTime,sizeof(mmTime));
                    assertMsg(mmTime.wType==TIME_BYTES,L"�˴��в�֧�ִ����豸" );

                    UINT64  uiBytes=0;          //�Ѿ����ŵ��ֽ���
                    UINT64  uiTmp;
// 
                    WORD nBlockAlign=pWavIn->m_wf.nBlockAlign;
                    WORD nBytePerSample=(pWavIn->m_wf.wBitsPerSample)/8;

                    //���㵱ǰ�����Ѿ����ŵ��ֽ���
                    uiTmp =(pWavIn->m_uiRecordedBuffers)*
                        (pWavIn->m_wf.nAvgBytesPerSec/DIV_FACTORS);

                    //��ֹmmTime�е��ֽ������
                    uiBytes=mmTime.u.cb;
                    while(uiBytes <uiTmp)
                    {
                        uiBytes+=((UINT64)(MAXDWORD)+1);
                    }

                    //����Ĵ�����Ϊ�˷�ֹ��ȡ����ʱ������ֵ
                    if (uiBytes >nBlockAlign)
                    {
                        uiBytes-=nBlockAlign;
                    }

                    pWavIn->m_posInfo.bRun=TRUE;
                    pWavIn->m_posInfo.uiSamples=(uiBytes)*1000/pWavIn->m_wf.nAvgBytesPerSec;
// 
                    //���㵱ǰ�Ĳ���λ��λ���ĸ�������
                    UINT64 bufferIndex=(uiBytes)/(pWavIn->m_wf.nAvgBytesPerSec/DIV_FACTORS);
                    UINT    pos=(UINT)( (uiBytes)%(pWavIn->m_wf.nAvgBytesPerSec/DIV_FACTORS) );
                    bufferIndex =bufferIndex%2;

                    char *pBuffer= bufferIndex==0?pWavIn->pBuffer1:pWavIn->pBuffer2;

                    INT16           v;
                    UINT            vt=0;

                    assertMsg((pWavIn->m_wf.wBitsPerSample)%8==0 ,
                        L"�ݲ�֧�ֲ������Ȳ�Ϊ8λ����������wav�ļ�" );                   

                    for (int i=0;i<nBytePerSample;i++)
                    {
                        BYTE b=pBuffer[pos+i] ;
                        vt|=(UINT)(b<< (i*8) );
                    }

                    if (nBytePerSample ==1)
                    {
                        v=(vt-128)*256;
                    }
                    else if (nBytePerSample ==2)
                    {
                        v=(INT16)vt;
                    }
                    else
                    {
                        v=(INT16)v>>((nBytePerSample-2)*8 );
                    }
                    pWavIn->m_posInfo.iVolume=v; 
                }
                break;
            default:
                break;
            }
            SetEvent(pWavIn->m_hCmdEventRespond);
            LeaveCriticalSection(&pWavIn->m_cs);

        }
    }

    return 0;
}

void CWavIn::OnFinishRecord()
{
    if (m_hWaveInHandle )
    {
        waveInClose(m_hWaveInHandle);
        m_hWaveInHandle=NULL;
    }

    if (m_hSemaphore)
    {
        CloseHandle(m_hSemaphore);
        m_hSemaphore=NULL;
    }

    if (m_hCmdEvent)
    {
        CloseHandle(m_hCmdEvent);
        m_hCmdEvent=NULL;
    }

    if (m_hCmdEventRespond)
    {
        CloseHandle(m_hCmdEventRespond);
        m_hCmdEventRespond=NULL;
    }

    free(pBuffer1);
    free(pBuffer2);

    pBuffer2=NULL;
    pBuffer1=NULL;
}
