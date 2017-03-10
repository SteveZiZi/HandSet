/************************Copyright(c)***********************************
**             GuangZhou XuanTong Electric Technology Co.,LTD.
**
**                  http://www.xtoee.com.cn
**
**---------FILE Info----------------------------------------------------
**  File Name: WavOut.h
**  Latest modified Dat:2010-03-15
**  Last Version: 1.0
**  Description: create the file :WavOut.h
**----------------------------------------------------------------------
**  Created By: chao
**  Created date:2010-03-15
**  Version: 1.0
**  Descriptions: ���崦�����wav�ļ�������صĸ��ֲ�������
**----------------------------------------------------------------------*/

#ifndef __WAVOUT_H
#define __WAVOUT_H

#include "xQueueLoop.h"


//����wav�ļ�¼��������λ����Ϣ�ṹ
struct WAV_POS_INFO
{
    UINT32  bRun;                               //��ǰ�Ƿ����ڲ��Ż���¼��
    UINT64  uiSamples;                          //��ǰ�Ѿ����Ż���¼�Ƶĺ�����
    INT32   iVolume;                            //��ǰ���������������һ����-65536~65535
};

class CWavOut
{

public:
    CWavOut();
    ~CWavOut();  

    BOOL    OpenWavFile(LPCWSTR lpszFilename);          //��һ��wav�ļ���ʧ�ܷ���FALSE
    void    CloseWavFile();                             //�ر�һ���򿪵�wav�ļ�

    BOOL    Play(BOOL   bLooped=FALSE);                 //����һ��Wav�ļ���ʧ�ܷ���FALSE
    void    Stop();                                     //ֹͣ����
    void    Pause();                                    //��ͣ����
    void    Resume();                                   //�ظ���ͣ�Ĳ���

    unsigned int GetLength();                           //�����ļ���Ԥ��ʱ�䣬��msΪ��λ
    BOOL    GetPos(WAV_POS_INFO &pos);                  //���ص�ǰ���ŵ�λ�ã���100msΪ��λ

   
    //BOOL    SeekTo(int iTime);                          //����ָ��ʱ�䲥�ţ���100msΪ��λ
private:
    FILE            *m_pWavFile;
    WAVEFORMATEX    m_wf;                           //һ��WAVEFORMATEX�ṹ��
    int             m_iDataStart;                   //��Ƶ���ݿ�ʼ��λ��
    
    unsigned int    m_uiAudioTimeLen;                //����Ƶ�ļ����������ֵĳ��ȣ���msΪ��λ
    HWAVEOUT        m_WaveOutHandle;                //�����豸���
    BOOL            m_bLoop;                        //�Ƿ�ѭ������
    int             m_iBufferNum;                   //����������*/
    char            *pBuffer1,*pBuffer2;            //��������������
    WAVEHDR         WaveHdr1,WaveHdr2;              //����WAVEHDR
    int             m_iRequreLen;                   //һ�������ÿ����仺������Ҫ����PCM�ֽ���
    UINT64          m_uiPlayedBuffers;              //�Ѿ����ŵĻ������ĸ���

    
    CRITICAL_SECTION m_cs;                          //һ��CRITICAL_SECTION,����ͬ��
    HANDLE          m_hSemaphore;                   //���ڱ�����ǰ�ж��ٸ����������豸���ظ�Ӧ�ó���
    HANDLE          m_hPlayThread;                  //�������߳̾��
    CxQueueLoop<WAVEHDR *> m_queue;                 //�̰߳�ȫ�Ķ���
    
    HANDLE          m_hCmdEvent;                    //���ڱ�����ǰ�����������߳�ִ�е�����
    HANDLE          m_hCmdEventRespond;             //�����߳�ִ�������ķ����¼�    

    typedef enum __CMD_CODE                         //����ϣ�������߳�ִ�е�������
    {
        CMD_GET_POS_INFO                            //��ȡ��ǰ�Ĳ�����Ϣ

    }CMD_CODE;
    CMD_CODE        m_cmdCode;                      //���ݵ�������
    WAV_POS_INFO    m_posInfo;                      //λ����Ϣ

    // 	
    void    OnFinishPlay();                             //���Ž���ʱ�Ĵ���
    static void CALLBACK waveOutProc(HWAVEOUT hwo,UINT uMsg,DWORD_PTR dwInstance,
        DWORD_PTR dwParam1,DWORD dwParam2);
    static DWORD WINAPI PlayWavProc(void *pVoid);

    //��ȡm_iRequreLen���ȵ��ֽ�������Ƶ������������,len����ʵ�ʶ�ȡ����
    BOOL    FillTheAudioBuffer(char *pBuffer,int  &len);
     
};


class  CWavIn
{
public:
    CWavIn();
    ~CWavIn();

    BOOL        CreateWavFile(LPCWSTR strWavFile);
    void        SetRecordFormat(const WAVEFORMATEX  &wf);
    BOOL        BeginRecord();

    BOOL        Pause();
    BOOL        Resume();
    BOOL        Stop();

    BOOL        GetPos(WAV_POS_INFO &pos);
private:
    FILE            *m_pWavFile;
    WAVEFORMATEX    m_wf;                           //һ��WAVEFORMATEX�ṹ��


    HWAVEIN         m_hWaveInHandle;                //¼���豸���   
    int             m_iBufferNum;                   //����������*/
    char            *pBuffer1,*pBuffer2;            //��������������
    WAVEHDR         WaveHdr1,WaveHdr2;              //����WAVEHDR
    int             m_iRequreLen;                   //һ�������ÿ����仺������Ҫ����PCM�ֽ���
    UINT64          m_uiRecordedBuffers;            //�Ѿ�¼�ƵĻ������ĸ���
    BOOL            m_bStop;                        //�����Ƿ�ֹͣ¼��



    CRITICAL_SECTION m_cs;                          //һ��CRITICAL_SECTION,����ͬ��
    HANDLE          m_hSemaphore;                   //���ڱ�����ǰ�ж��ٸ����������豸���ظ�Ӧ�ó���
    HANDLE          m_hRecordThread;                //������¼���߳̾��
    CxQueueLoop<WAVEHDR *> m_queue;                 //�̰߳�ȫ�Ķ���

    

    HANDLE          m_hCmdEvent;                    //���ڱ�����ǰ������¼���߳�ִ�е�����
    HANDLE          m_hCmdEventRespond;             //¼�Ʒ��߳�ִ�������ķ����¼�    

    typedef enum __CMD_CODE                         //����ϣ�������߳�ִ�е�������
    {
        CMD_GET_POS_INFO                            //��ȡ��ǰ�Ĳ�����Ϣ

    }CMD_CODE;
    CMD_CODE        m_cmdCode;                      //���ݵ�������
    WAV_POS_INFO    m_posInfo;                      //λ����Ϣ

    void    OnFinishRecord();                       //¼������ʱ�Ĵ���
    static void CALLBACK waveInProc(HWAVEIN hwo,UINT uMsg,DWORD_PTR dwInstance,
        DWORD_PTR dwParam1,DWORD dwParam2);
    static DWORD WINAPI RecordWavProc(void *pVoid);
};

#endif
