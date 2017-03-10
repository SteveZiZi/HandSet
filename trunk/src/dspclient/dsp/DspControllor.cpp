///
/// @file
///     DspControllor.cpp
/// @brief
///     DSP端控制器，用于控制DSP端的初始化、结束和数据的发送和接收
/// @note
///     读写过程采用了加锁处理以保证数据的可靠性
///
/// Copyright (c) 2016 XuanTong Inc.
///
/// @author
///     chao 2013/03/19
/// @revise
/// 
#include "stdafx.h"
#include "DSPControllor.h"
#include "../dsplib/loop.h"

#include "../../../inc/DspArmProtocol.h"
#include "DspTransport.h"
Lock CDspController::m_singleLock;
CDspController::CDspController()
:m_fDspRunning(false)
{
    
}
CDspController::~CDspController()
{
    
}
///
/// @brief
///    返回DspController的单例
/// @return
///    CDspController *     单例指针
///
CDspController *CDspController::GetInstance()
{
    static CDspController* dspController = NULL;
    
    if(NULL == dspController)
    {
        m_singleLock.Acquire();
        if(NULL == dspController)
        {
            dspController = new CDspController();
        }
        m_singleLock.Release();
    }
    return dspController;
}
///
/// @brief
///     初始化DSP端
/// @return
///     true：初始化成功；false：初始化失败
///
bool CDspController::DspInit()
{
	//创建用户算法程序，下载用户可执行程序(.out文件到DSP端)并分配GPP和DSP通信共
    //享数据区域
    if(!m_fDspRunning)
    {
        m_fDspRunning  = CDspTransport::Initial(DSP_LOOP_PATH, ARM_DSP_BUF_SIZE);
    }
    return m_fDspRunning;
}

///
/// @brief
///    发送DSP数据帧（命令）
/// @param[in] 
///    nCommandID   命令码，详见DspArmProtocal.h
/// @return
///    int              0：成功；-1：DSP未初始化或初始化失败; -2： DSP发送数据错误
///
int CDspController::FrameSend(const UINT8 nCommandID)
{
    AutoLock autoLock(m_wLock);
    FrameHead fh;
    if(!m_fDspRunning)
    {
        return -1;
    }
    fh.nCommandID = nCommandID;
    fh.nDataLength = 0;

    // 防止发送过快导致DSP卡死
    //Sleep(2);

    CDspTransport::Write(0, (UINT8 *)&fh, sizeof(FrameHead));
    if(!CDspTransport::Send())
    {
        return -2;
    }
    else
    {
        return 0;
    }
}
///
/// @brief
///    发送DSP数据帧（命令+单个数据块）
/// @param[in] 
///    nCommandID       命令码，详见DspArmProtocol.h
///    pData            要发送的数据指针
///    nDataLength      要发送的数据长度
/// @return
///    int              0：成功；
///                     -1：DSP未初始化或初始化失败; 
///                     -2：输入缓冲区有指针为NULL或者数据长度为0
///                     -3：DSP发送数据错误
///
int CDspController::FrameSend(const UINT8 nCommandID, 
                               const void* pData,
                               const UINT32 nDataLength)
{
    AutoLock autoLock(m_wLock);
    FrameHead fh;
    
    if(!m_fDspRunning)
    {
        return -1;
    }
    if(NULL == pData  || 0 == nDataLength)
    {
        return -2;
    }
    fh.nCommandID = nCommandID;
    fh.nDataLength = nDataLength;
    
    // 防止发送过快导致DSP卡死
    //Sleep(2);

    CDspTransport::Write(0, (UINT8 *)&fh, sizeof(FrameHead));
    CDspTransport::Write(sizeof(FrameHead), (UINT8 *)pData, nDataLength);
    if(!CDspTransport::Send())
    {
        return -3;
    }
    else
    {
        return 0;
    }
}

///
/// @brief
///    发送DSP数据帧（命令+多个数据块）
/// @note
///    此函数用于组合多个数据块然后作为整体发送给DSP，比如要组合两个结构体S1和S2，
///    则先构造一个2个元素void*数组：
///         void *pDatas[2] = {&S1, &S2};
///    然后再构造一个2个元素的整形数组用于指定两个结构体的长度：
///         UINT32 nDataLengths[2] = {sizeof(S1),sizeof(S2)};
///    然后调用FrameSend(nCommandID, pDatas, nDataLengths, 2)发送一个组合帧数据，
///    相当于发送了一个这个的数据：|FrameHead|S1|S2| 
/// @param[] 
///    nCommandID       命令码，详见DspArmProtocal.h
///    pDatas           数据块指针数组，用于存储输入的需要组装的数据指针
///    nDataLengths     数据块长度数组，输入的数据指针指向的数据大小
///    nDataNum         数据块个数
/// @return
///    int              0：成功；
///                     -1：DSP未初始化或初始化失败; 
///                     -2：输入缓冲区有指针为NULL或者数据长度为0
///                     -3：DSP发送数据错误；
///
int CDspController::FrameSend(const UINT8 nCommandID, 
                              const void* pDatas[],
                              const UINT32 nDataLengths[],
                              const UINT8 nDataNum)
{
	AutoLock autoLock(m_wLock);
    FrameHead fh;
    
    if(!m_fDspRunning)
    {
        return -1;
    }
	// 组合数据帧
    UINT32 nOffset = 0;
    for(int i=0; i<nDataNum; i++)
    {
        if(pDatas==NULL || pDatas[i]==NULL || nDataLengths[i]==0)
        {
            return -2;
        }
        CDspTransport::Write(sizeof(FrameHead)+nOffset, 
                (UINT8 *)pDatas[i], nDataLengths[i]);
        nOffset += nDataLengths[i];
    }

    // 写入帧头
    fh.nCommandID = nCommandID;
    fh.nDataLength = nOffset;
    
    // 防止发送过快导致DSP卡死
    //Sleep(2);

    CDspTransport::Write(0, (UINT8 *)&fh, sizeof(FrameHead));

    //向DSP端发送数据，启动DSP算法
    if(!CDspTransport::Send())
    {
        return -3;
    }
    return 0;
}

///
/// @brief
///    接收DSP数据
/// @param[out] 
///    nCommandID           从DSP返回的命令码
///    pData                返回DSP缓冲区指针
///    nDataLength          数据的长度
/// @return
///    int                 0：成功；
///                        -1：DSP未初始化或初始化失败
///                        -2：读取操作失败，nDataLength置为0；
///
int CDspController::FrameRecv( UINT8 &nCommandID, void **pData, UINT32 &nDataLength)
{
    AutoLock autoLock(m_rLock);

    if(!m_fDspRunning)
    {
        return -1;
    }
    
    //TRACE(L"TIME1:%d\t",GetTickCount());
    bool fIsOK = CDspTransport::Read();
    //TRACE(L"TIME2:%d\t",GetTickCount());
    if(!fIsOK)
    {
        nDataLength = 0;
        return -2;
    }

    
    UINT8 *pBuf = CDspTransport::GetOutBuffer();

    FrameHead* fh = (FrameHead *)pBuf;
    nCommandID = fh->nCommandID;
    nDataLength = fh->nDataLength;
    *pData = pBuf + sizeof(FrameHead);
#if 0
    //TRACE(L"TIME3:%d\t",GetTickCount());
    if(nCommandID == DSP2ARM_CONTROLCODE_SMV_CALC_RT)
    {
        SMV_DATA_CACL_RT *t = (SMV_DATA_CACL_RT *)*pData;
        TRACE(L"%d\t%d\t%f\t",nDataLength, sizeof(SMV_DATA_CACL_RT), 
            t->fApparentPower[0]);
        TRACE(L"%f\n", t->fApparentPower[0]);
    }
#endif
    return 0;
}

///
/// @brief
///     结束DSP端
///
void CDspController::DspDelete()
{
    if(m_fDspRunning)
    {
	    //向DSP端发送数据,结束DSP_TSK,DSP端多任务时使用
	    FrameSend(ARM2DSP_CONTROLCODE_QUIT);	    
    }
    //卸载DSP端代码，释放资源（内存）
    CDspTransport::Destroy();
    m_fDspRunning  = false;
}

///
/// @brief
///    Dsp初始化是否成功
/// @return
///    bool             true:成功
///                     false:失败
///
bool CDspController::IsInitSuccess()
{
    return m_fDspRunning;
}
