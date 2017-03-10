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
**  Descriptions: 定义处理关于wav文件播放相关的各种操作的类
**----------------------------------------------------------------------*/

#ifndef __WAVOUT_H
#define __WAVOUT_H

#include "xQueueLoop.h"


//定义wav文件录入和输出的位置信息结构
struct WAV_POS_INFO
{
    UINT32  bRun;                               //当前是否仍在播放或者录制
    UINT64  uiSamples;                          //当前已经播放或者录制的毫秒数
    INT32   iVolume;                            //当前采样点的音量，归一化到-65536~65535
};

class CWavOut
{

public:
    CWavOut();
    ~CWavOut();  

    BOOL    OpenWavFile(LPCWSTR lpszFilename);          //打开一个wav文件，失败返回FALSE
    void    CloseWavFile();                             //关闭一个打开的wav文件

    BOOL    Play(BOOL   bLooped=FALSE);                 //播放一个Wav文件，失败返回FALSE
    void    Stop();                                     //停止播放
    void    Pause();                                    //暂停播放
    void    Resume();                                   //回复暂停的播放

    unsigned int GetLength();                           //返回文件的预计时间，以ms为单位
    BOOL    GetPos(WAV_POS_INFO &pos);                  //返回当前播放的位置，以100ms为单位

   
    //BOOL    SeekTo(int iTime);                          //跳至指定时间播放，以100ms为单位
private:
    FILE            *m_pWavFile;
    WAVEFORMATEX    m_wf;                           //一个WAVEFORMATEX结构体
    int             m_iDataStart;                   //音频数据开始的位置
    
    unsigned int    m_uiAudioTimeLen;                //本音频文件所包含音乐的长度，以ms为单位
    HWAVEOUT        m_WaveOutHandle;                //播放设备句柄
    BOOL            m_bLoop;                        //是否循环播放
    int             m_iBufferNum;                   //缓冲区计数*/
    char            *pBuffer1,*pBuffer2;            //定义两个缓冲区
    WAVEHDR         WaveHdr1,WaveHdr2;              //两个WAVEHDR
    int             m_iRequreLen;                   //一般情况下每次填充缓冲区需要读的PCM字节数
    UINT64          m_uiPlayedBuffers;              //已经播放的缓冲区的个数

    
    CRITICAL_SECTION m_cs;                          //一个CRITICAL_SECTION,用于同步
    HANDLE          m_hSemaphore;                   //用于表征当前有多少个缓冲区被设备返回给应用程序
    HANDLE          m_hPlayThread;                  //创建的线程句柄
    CxQueueLoop<WAVEHDR *> m_queue;                 //线程安全的队列
    
    HANDLE          m_hCmdEvent;                    //用于表征当前有期望播放线程执行的命令
    HANDLE          m_hCmdEventRespond;             //播放线程执行命令后的反馈事件    

    typedef enum __CMD_CODE                         //定义希望播放线程执行的命令码
    {
        CMD_GET_POS_INFO                            //获取当前的播放信息

    }CMD_CODE;
    CMD_CODE        m_cmdCode;                      //传递的命令码
    WAV_POS_INFO    m_posInfo;                      //位置信息

    // 	
    void    OnFinishPlay();                             //播放结束时的处理
    static void CALLBACK waveOutProc(HWAVEOUT hwo,UINT uMsg,DWORD_PTR dwInstance,
        DWORD_PTR dwParam1,DWORD dwParam2);
    static DWORD WINAPI PlayWavProc(void *pVoid);

    //读取m_iRequreLen长度的字节数的音频数据至缓冲区,len返回实际读取到的
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
    WAVEFORMATEX    m_wf;                           //一个WAVEFORMATEX结构体


    HWAVEIN         m_hWaveInHandle;                //录制设备句柄   
    int             m_iBufferNum;                   //缓冲区计数*/
    char            *pBuffer1,*pBuffer2;            //定义两个缓冲区
    WAVEHDR         WaveHdr1,WaveHdr2;              //两个WAVEHDR
    int             m_iRequreLen;                   //一般情况下每次填充缓冲区需要读的PCM字节数
    UINT64          m_uiRecordedBuffers;            //已经录制的缓冲区的个数
    BOOL            m_bStop;                        //控制是否停止录音



    CRITICAL_SECTION m_cs;                          //一个CRITICAL_SECTION,用于同步
    HANDLE          m_hSemaphore;                   //用于表征当前有多少个缓冲区被设备返回给应用程序
    HANDLE          m_hRecordThread;                //创建的录音线程句柄
    CxQueueLoop<WAVEHDR *> m_queue;                 //线程安全的队列

    

    HANDLE          m_hCmdEvent;                    //用于表征当前有期望录制线程执行的命令
    HANDLE          m_hCmdEventRespond;             //录制放线程执行命令后的反馈事件    

    typedef enum __CMD_CODE                         //定义希望播放线程执行的命令码
    {
        CMD_GET_POS_INFO                            //获取当前的播放信息

    }CMD_CODE;
    CMD_CODE        m_cmdCode;                      //传递的命令码
    WAV_POS_INFO    m_posInfo;                      //位置信息

    void    OnFinishRecord();                       //录音结束时的处理
    static void CALLBACK waveInProc(HWAVEIN hwo,UINT uMsg,DWORD_PTR dwInstance,
        DWORD_PTR dwParam1,DWORD dwParam2);
    static DWORD WINAPI RecordWavProc(void *pVoid);
};

#endif
