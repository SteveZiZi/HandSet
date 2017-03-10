///
/// @brief
///     保存状态测试结果的类
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.7.1
///
/// 修改历史：
///

#include "StdAfx.h"
#include "StateTestResult.h"
#include "src\service\datalayer\config\ParamconfigManualTest.h"
#include "src\service\datalayer\config\GOOSERecvVirtualChannel.h"
#include "src\service\datalayer\config\ParamConfigGooseSend.h"


///
/// @brief
///    构造函数
///
CStateTestResult::CStateTestResult(enumTestType eType)
: m_eTestType(eType)
, m_bSmvChannelChanged(false)
, m_bGooseChannelChanged(false)
, m_bStatusWordChanged(false)
, m_bQualityChanged(false)
, m_bTested(false)
{
    m_mapChannelRecvValues.clear();
    m_listGooseSendCbs.clear();
    m_listSmvSendBuffers.clear();
    m_listGooseSendBuffers.clear();
    memset(&m_stStateStartTime, 0, sizeof(m_stStateStartTime));
}

///
/// @brief
///    析构函数
///
CStateTestResult::~CStateTestResult(void)
{
    // 释放所有记录通道历史值的对象
    ClearAllRecvValues();

    // 清除手动试验各GOOSE控制块
    ClearGooseSendCbs();

    // 清除SMV发送帧缓冲区
    ClearSmvSendBuffers();

    // 清除GOOSE发送帧缓冲区
    ClearGooseSendBuffers();
}

///
/// @brief
///    初始化
///
/// @return
///     bool    成功返回true
///
bool CStateTestResult::Init()
{
    CChannelRecvValues* pChannelRecvValues  = NULL;
    std::map<std::wstring, CChannelRecvValues*>::iterator iter;


    // 释放所有记录通道历史值的对象
    ClearAllRecvValues();

    // 向容器中添加默认对象
    for (int i = 0; i < sizeof(strGOOSERecvVCName)/sizeof(strGOOSERecvVCName[0]); i++)
    {
        // new一个记录通道历史值的对象
        pChannelRecvValues = new CChannelRecvValues(strGOOSERecvVCName[i]);
        if (NULL == pChannelRecvValues)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CStateTestResult::Init(): New CChannelRecvValues Object failed \n"));
            return false;
        }

        // 添加记录通道历史值的对象到容器中
        iter = m_mapChannelRecvValues.find(strGOOSERecvVCName[i]);
        if (iter != m_mapChannelRecvValues.end())
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CStateTestResult::Init(): Virtual channel already exists \n"));
            delete pChannelRecvValues;
            return false;
        }
        m_mapChannelRecvValues.insert(std::pair<std::wstring, CChannelRecvValues*>(strGOOSERecvVCName[i], pChannelRecvValues));
    }

    return true;
}

///
/// @brief
///    获得测试的类型
///
/// @return
///     eTestType    测试的类型
///
enumTestType CStateTestResult::GetTestType()
{
    return m_eTestType;
}

///
/// @brief
///    释放所有记录通道历史值的对象
///
void CStateTestResult::ClearAllRecvValues()
{
    CChannelRecvValues* pChannelRecvValues  = NULL;
    std::map<std::wstring, CChannelRecvValues*>::iterator iter;


    // 释放记录通道历史值的对象
    iter = m_mapChannelRecvValues.begin();
    while(m_mapChannelRecvValues.end() != iter)
    {
        pChannelRecvValues = iter->second;
        if (pChannelRecvValues)
        {
            delete pChannelRecvValues;
        }

        iter++;
    }

    // 清空容器
    m_mapChannelRecvValues.clear();
}

///
/// @brief
///    获得指定虚拟通道名的保存通道接收值的对象
///
/// @param[in]  strName    虚拟通道名
///
/// @return
///     CChannelRecvValues*    保存通道接收值得对象
///
CChannelRecvValues* CStateTestResult::GetChannelRecvValues( std::wstring strName )
{
    std::map<std::wstring, CChannelRecvValues*>::iterator iter;


    // 查找指定名字的虚拟通道
    iter = m_mapChannelRecvValues.find(strName);
    if (m_mapChannelRecvValues.end() == iter)
    {
        return NULL;
    }

    return (iter->second);
}

///
/// @brief
///    获得状态开始测试的时间
///
/// @return
///     std::wstring    开始测试的时间
///
std::wstring CStateTestResult::GetStartTime()
{
    return m_stStateStartTime.GetString();
}

///
/// @brief
///    设置状态开始时间（相对时间）
///
/// @param[in]  stNewTime    状态开始时间
///
void CStateTestResult::SetStartTime( CEventTime stNewTime )
{
    m_stStateStartTime = stNewTime;
}

///
/// @brief
///    保存手动试验各SMV发送虚拟通道的值
///
/// @return
///     bool    成功返回true
///
bool CStateTestResult::__SaveManualTestSMVSendValues()
{
    // 获得手动试验参数配置管理器
    CParamconfigManualTest* pParamConfigManualTest = CParamconfigManualTest::getInstance();
    if (NULL == pParamConfigManualTest)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CStateTestResult::__AddManualTestSMVSendValues(): Get CParamconfigManualTest Object failed \n"));
        return false;
    }

    // 释放所有SMV发送虚拟通道对象
    m_stSMVSendValues.__ClearSMVSendVirtualChannel();

    // 拷贝手动试验各SMV发送虚拟通道
    std::list<CSMVSendVirtualChannel*>::iterator iter;
    for (iter = pParamConfigManualTest->m_listSMVSendVirtualChannelGroup.begin(); iter != pParamConfigManualTest->m_listSMVSendVirtualChannelGroup.end(); iter++)
    {
        CSMVSendVirtualChannel* pOldVirtualChannel = *iter;
        if (pOldVirtualChannel)
        {
            // new一个CSMVSendVirtualChannel对象，为了幅值的变比，注意此处需把虚拟通道类型设置成FUNCTION_TYPE_STATE_SEQUENCE类型
            CSMVSendVirtualChannel* pNewVirtualChannel = new CSMVSendVirtualChannel(FUNCTION_TYPE_STATE_SEQUENCE, NULL);
            if (NULL == pNewVirtualChannel)
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CStateTestResult::__AddManualTestSMVSendValues(): New CSMVSendVirtualChannel Object failed \n"));
                return false;
            }

            // 拷贝虚拟通道的值
            if (!pNewVirtualChannel->__Copy(pOldVirtualChannel))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CStateTestResult::__AddManualTestSMVSendValues(): Copy Virtual Channel failed \n"));
                delete pNewVirtualChannel;
                return false;
            }

            // 添加新的虚拟通道到容器中
            if (!m_stSMVSendValues.__AddSMVSendVirtualChannel(pNewVirtualChannel))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CStateTestResult::__AddManualTestSMVSendValues(): Add Virtual Channel failed \n"));
                delete pNewVirtualChannel;
                return false;
            }
        }
    }

    return true;
}

///
/// @brief
///    枚举第一个被映射的SMV发送虚拟通道
///
/// @return
///     CVirtualChannel*    指向虚拟通道对象的指针，失败时返回NULL
///
CSMVSendVirtualChannel* CStateTestResult::FirstSMVSendVirtualChannel()
{
    return m_stSMVSendValues.FirstSMVSendVirtualChannel();
}

///
/// @brief
///    获得下一个被映射的SMV发送虚拟通道
///
/// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
///
/// @return
///     CVirtualChannel*    指向虚拟通道对象的指针，失败时返回NULL
///
CSMVSendVirtualChannel* CStateTestResult::NextSMVSendVirtualChannel( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    return m_stSMVSendValues.NextSMVSendVirtualChannel(pCurVirtualChannel);
}

///
/// @brief
///    保存各GOOSE发送控制块的值
///
/// @return
///     bool    成功返回true
///
bool CStateTestResult::__SaveGooseSendCbs()
{
    // 获得GOOSE发送参数配置管理器
    CParamConfigGOOSESend* pParamConfigGooseSend = CParamConfigGOOSESend::getInstance();
    if (NULL == pParamConfigGooseSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CStateTestResult::__SaveGooseSendCbs(): Get CParamConfigGOOSESend Object failed \n"));
        return false;
    }

    // 清除各GOOSE控制块
    ClearGooseSendCbs();

    // 拷贝所有被勾选的GOOSE发送控制块
    std::list<CGooseCb*>::iterator iter;
    for (iter = pParamConfigGooseSend->m_listGocbGroup.begin(); iter != pParamConfigGooseSend->m_listGocbGroup.end(); iter++)
    {
        CGooseCb* pOldGocb = *iter;
        if (pOldGocb && pOldGocb->IsSelected())
        {
            // new一个GOOSE控制块对象
            CGooseCb* pNewGocb = new CGooseCb(NULL);
            if (NULL == pNewGocb)
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CStateTestResult::__SaveGooseSendCbs(): New GOOSE control block object failed \n"));
                return false;
            }
            // 标记为选中
            pNewGocb->Selected(true);

            // 拷贝虚拟通道的值
            if (!pNewGocb->__Copy(pOldGocb))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CStateTestResult::__SaveGooseSendCbs(): Copy GOOSE control block failed \n"));
                delete pNewGocb;
                return false;
            }

            // 添加控制块到容器中
            m_listGooseSendCbs.push_back(pNewGocb);
        }
    }

    return true;
}

///
/// @brief
///    枚举第一个GOOSE控制块
///
/// @return
///     CGooseCb*    成功时返回GOOSE控制块指针，失败时返回NULL
///
CGooseCb* CStateTestResult::FirstGooseSendCb()
{
    std::list<CGooseCb*>::iterator iter = m_listGooseSendCbs.begin();
    if (m_listGooseSendCbs.end() != iter)
    {
        return *iter;
    }

    return NULL;
}

///
/// @brief
///    枚举下一个GOOSE发送控制块
///
/// @param[in]  pCurGocb 当前位置GOOSE控制块的指针
///
/// @return
///     CGooseCb*    成功时返回GOOSE控制块指针，失败时返回NULL
///
CGooseCb* CStateTestResult::NextGooseSendCb( CGooseCb* pCurGocb )
{
    std::list<CGooseCb*>::iterator  iter;


    iter = m_listGooseSendCbs.begin();
    while (m_listGooseSendCbs.end() != iter)
    {
        // 查找当前GOOSE控制块的位置
        if (*iter == pCurGocb)
        {
            // 获得下一个GOOSE控制块的位置
            if (m_listGooseSendCbs.end() == ++iter)
            {
                return NULL;
            }

            return (*iter);
        }

        iter++;
    }

    return NULL;
}

///
/// @brief
///    获得指定控制块的索引号
///
/// @param[in]  pCurGocb    指向控制块对象的指针
///
/// @return
///     int    控制块索引，失败返回-1
///
int CStateTestResult::GetIndexByGooseCb( CGooseCb* pCurGocb )
{
    std::list<CGooseCb*>::iterator  iter;
    int                             nIndex = -1;


    for (nIndex = 0, iter = m_listGooseSendCbs.begin(); m_listGooseSendCbs.end() != iter; nIndex++, iter++)
    {
        // 查找当前GOOSE控制块的位置
        if (*iter == pCurGocb)
        {
            break;
        }
    }

    return nIndex;
}

///
/// @brief
///    根据索引获得GOOSE控制块
///
/// @param[in]  nCurIndex    控制块索引
///
/// @return
///     CGooseCb*    成功时返回GOOSE控制块指针，失败时返回NULL
///
CGooseCb* CStateTestResult::GetGooseCbByIndex( int nCurIndex )
{
    std::list<CGooseCb*>::iterator  iter;
    int                             nIndex;


    for (nIndex = 0, iter = m_listGooseSendCbs.begin(); m_listGooseSendCbs.end() != iter; nIndex++, iter++)
    {
        // 获得指定索引号的控制块
        if (nIndex == nCurIndex)
        {
            return (*iter);
        }
    }

    return NULL;
}

///
/// @brief
///    清除手动试验各GOOSE控制块
///
void CStateTestResult::ClearGooseSendCbs()
{
    // 遍历容器，释放对象
    std::list<CGooseCb*>::iterator iter = m_listGooseSendCbs.begin();
    while (m_listGooseSendCbs.end() != iter)
    {
        CGooseCb* pGocb = *iter;
        if (pGocb)
        {
            delete pGocb;
        }

        iter++;
    }

    // 清空容器
    m_listGooseSendCbs.clear();
}

///
/// @brief
///    获得手动试验SMV各虚拟通道的值
///
/// @return
///     CStatus*    保存虚拟通道值的对象
///
CStatus* CStateTestResult::__GetSMVSendValues()
{
    return &m_stSMVSendValues;
}

///
/// @brief
///    把手动试验SMV发送的虚拟通道值与指定的状态结果对象相比
///
/// @param[in]  eCompareType    比较方式
/// @param[in]  pOldStatusTestResult    指向状态结果对象的指针
///
/// @return
///     bool    相同返回true
///
bool CStateTestResult::__CompareSMVSendValues( enumCompareType eCompareType, CStateTestResult* pOldStatusTestResult )
{
    return m_stSMVSendValues.__CompareSMVSendVirtualChannels(eCompareType, pOldStatusTestResult->__GetSMVSendValues());
}

///
/// @brief
///    把手动试验GOOSE发送的控制块与指定的状态结果对象相比
///
/// @param[in]  pOldStatusTestResult    指向状态结果对象的指针
///
/// @return
///     bool    相同返回true
///
bool CStateTestResult::__CompareGooseSendValues( CStateTestResult* pOldStateTestResult )
{
    bool                            bRet    = true;


    // 比较控制块数量
    if (m_listGooseSendCbs.size() != pOldStateTestResult->m_listGooseSendCbs.size())
    {
        return false;
    }

    // 遍历每一个控制块，比较控制块通道值
    std::list<CGooseCb *>::iterator oldIter = pOldStateTestResult->m_listGooseSendCbs.begin();
    std::list<CGooseCb *>::iterator newIter = m_listGooseSendCbs.begin();
    while((oldIter != pOldStateTestResult->m_listGooseSendCbs.end()) && (newIter != m_listGooseSendCbs.end()))
    {
        CGooseCb* pOldGocb = *oldIter;
        CGooseCb* pNewGocb = *newIter;
        if ((NULL != pOldGocb) && (NULL != pNewGocb))
        {
            if (!pNewGocb->__CompareChannelSendValue(pOldGocb))
            {
                // 标记控制块为已修改
                pNewGocb->SetSendValueChanged(true);

                // 递增变位次数
                pNewGocb->SetStNum(pOldGocb->GetStNum() + 1);
                bRet = false;
            }
            else
            {
                // 拷贝变位次数
                pNewGocb->SetStNum(pOldGocb->GetStNum());
            }
        }

        oldIter++;
        newIter++;
    }

    return bRet;
}

///
/// @brief
///    查询SMV通道值是否发生变化
///
/// @return
///     bool    true表示发生了变化
///
bool CStateTestResult::IsSmvChannelChanged() const
{
    return m_bSmvChannelChanged;
}

///
/// @brief
///    设置SMV通道值是否发生变化
///
/// @param[in]  val    是否发生了变化
///
void CStateTestResult::SetSmvChannelChanged( bool val )
{
    m_bSmvChannelChanged = val;
}

///
/// @brief
///    查询GOOSE通道值是否发生变化
///
/// @return
///     bool    true表示发生了变化
///
bool CStateTestResult::IsGooseChannelChanged() const
{
    return m_bGooseChannelChanged;
}

///
/// @brief
///    设置GOOSE通道值是否发生了变化
///
/// @param[in]  val    是否发生了变化
///
void CStateTestResult::SetGooseChannelChanged( bool val )
{
    m_bGooseChannelChanged = val;
}

///
/// @brief
///    查询9-1、FT3、FT3扩展控制块的状态字是否改变
///
/// @return
///     bool    true表示发生了变化
///
bool CStateTestResult::IsStatusWordChanged() const
{
    return m_bStatusWordChanged;
}

///
/// @brief
///    设置9-1、FT3、FT3扩展控制块的状态字是否发生了改变
///
/// @param[in]  val    是否发生了改变
///
void CStateTestResult::SetStatusWordChanged( bool val )
{
    m_bStatusWordChanged = val;
}

///
/// @brief
///    查询9-2通道的品质是否改变
///
/// @return
///     bool    true表示发生了变化
///
bool CStateTestResult::IsQualityChanged() const
{
    return m_bQualityChanged;
}

///
/// @brief
///    设置9-2通道的品质是否发生了改变
///
/// @param[in]  val    是否发生了改变
///
void CStateTestResult::SetQualityChanged( bool val )
{
    m_bQualityChanged = val;
}

///
/// @brief
///    清除SMV发送帧缓冲区
///
void CStateTestResult::ClearSmvSendBuffers()
{
    // 遍历容器，删除每一个缓冲区对象
    std::list<CStateSendBuffer*>::iterator iter = m_listSmvSendBuffers.begin();
    while (m_listSmvSendBuffers.end() != iter)
    {
        // 查找变位控制块
        CStateSendBuffer* pSendBuffer = *iter;
        if (pSendBuffer)
        {
            delete pSendBuffer;
        }

        iter++;
    }

    // 清空容器
    m_listSmvSendBuffers.clear();
}

///
/// @brief
///    清除GOOSE发送帧缓冲区
///
void CStateTestResult::ClearGooseSendBuffers()
{
    // 遍历容器，删除每一个缓冲区对象
    std::list<CStateSendBuffer*>::iterator iter = m_listGooseSendBuffers.begin();
    while (m_listGooseSendBuffers.end() != iter)
    {
        // 查找变位控制块
        CStateSendBuffer* pSendBuffer = *iter;
        if (pSendBuffer)
        {
            delete pSendBuffer;
        }

        iter++;
    }

    // 清空容器
    m_listGooseSendBuffers.clear();
}

///
/// @brief
///    修改GOOSE控制块中单点/双点类型通道时，需要自动修改相邻/间隔的时间通道的值
///    此函数自动寻找寻找变位的单点/双点通道，然后改变相邻或间隔的时间通道的值
///
/// @param[in]  stTestStartTime    整个实验的开始时间
///
void CStateTestResult::__UpdateGooseChannelTime(CEventTime stTestStartTime)
{
    // 遍历容器，查找变位的控制块
    std::list<CGooseCb*>::iterator iter = m_listGooseSendCbs.begin();
    while (m_listGooseSendCbs.end() != iter)
    {
        // 查找变位控制块
        CGooseCb* pGocb = *iter;
        if (pGocb && pGocb->IsSendValueChanged())
        {
            // 自动寻找寻找变位的单点/双点通道，然后改变相邻或间隔的时间通道的值
            pGocb->__UpdateChannelTime(stTestStartTime + m_stStateStartTime);
        }

        iter++;
    }
}

///
/// @brief
///    查询是否完成了测试
///
/// @return
///     bool    完成测试返回true
///
bool CStateTestResult::IsTested() const
{
    return m_bTested;
}

///
/// @brief
///     设置测试完成标识
///
/// @param[in]  val     是否完成测试
///
void CStateTestResult::SetTested( bool val )
{
    m_bTested = val;
}

///
/// @brief
///     拷贝SMV发送缓冲
///
/// @param[in]  pOldStateTestResult     老的状态测试结果集
///
/// @return
///     bool    拷贝成功返回true
///
bool CStateTestResult::__CopySmvSendBuffers( CStateTestResult* pOldStateTestResult )
{
    // 检查输入参数
    if (NULL == pOldStateTestResult)
    {
        return false;
    }

    // 清除SMV发送帧缓冲区
    ClearSmvSendBuffers();

    // 依次拷贝状态结果集的SMV发送缓冲
    std::list<CStateSendBuffer*>::iterator OldIter = pOldStateTestResult->m_listSmvSendBuffers.begin();
    for (; OldIter != pOldStateTestResult->m_listSmvSendBuffers.end(); OldIter++)
    {
        CStateSendBuffer* pOldStateSendBuffer = *OldIter;
        if (pOldStateSendBuffer)
        {
            // 申请一个新的状态发送缓冲
            CStateSendBuffer* pNewStateSendBuffer = new CStateSendBuffer(pOldStateSendBuffer->m_ePortName);
            if (NULL == pNewStateSendBuffer)
            {
                return false;
            }

            // 拷贝状态发送缓冲
            if (!pNewStateSendBuffer->Copy(pOldStateSendBuffer))
            {
                delete pNewStateSendBuffer;
                return false;
            }

            // 添加状态发送缓冲到容器中
            m_listSmvSendBuffers.push_back(pNewStateSendBuffer);
        }
    }

    return true;
}

///
/// @brief
///     拷贝Goose发送缓冲
///
/// @param[in]  pOldStateTestResult     老的状态测试结果集
///
/// @return
///     bool    拷贝成功返回true
///
bool CStateTestResult::__CopyGooseSendBuffers( CStateTestResult* pOldStateTestResult )
{
    // 检查输入参数
    if (NULL == pOldStateTestResult)
    {
        return false;
    }

    // 清除SMV发送帧缓冲区
    ClearGooseSendBuffers();

    // 依次拷贝状态结果集的SMV发送缓冲
    std::list<CStateSendBuffer*>::iterator OldIter = pOldStateTestResult->m_listGooseSendBuffers.begin();
    for (; OldIter != pOldStateTestResult->m_listGooseSendBuffers.end(); OldIter++)
    {
        CStateSendBuffer* pOldStateSendBuffer = *OldIter;
        if (pOldStateSendBuffer)
        {
            // 申请一个新的状态发送缓冲
            CStateSendBuffer* pNewStateSendBuffer = new CStateSendBuffer(pOldStateSendBuffer->m_ePortName);
            if (NULL == pNewStateSendBuffer)
            {
                return false;
            }

            // 拷贝状态发送缓冲
            if (!pNewStateSendBuffer->Copy(pOldStateSendBuffer))
            {
                delete pNewStateSendBuffer;
                return false;
            }

            // 复位变位标识
            pNewStateSendBuffer->m_stIOCtrlParam.nInBuf[1] = 0;

            // 添加状态发送缓冲到容器中
            m_listGooseSendBuffers.push_back(pNewStateSendBuffer);
        }
    }

    return true;
}

///
/// @brief
///     查找GOOSE控制块中需要变位的时间通道
///
void CStateTestResult::__SaveGooseCbTimeChannel()
{
    std::list<CStateSendBuffer*>::iterator iter = m_listGooseSendBuffers.begin();
    for (; iter != m_listGooseSendBuffers.end(); iter++)
    {
        CStateSendBuffer* pStateSendBuffer = *iter;
        if (pStateSendBuffer)
        {
            // 查找GOOSE控制块中需要变位的时间通道
            pStateSendBuffer->__SaveGooseCbTimeChannel();
        }
    }
}

///
/// @brief
///    构造函数
///
/// @param[in]  ePortName    发送端口
///
CStateSendBuffer::CStateSendBuffer( DsmPortName ePortName )
: m_ePortName(ePortName)
, m_pucDataBuffer(NULL)
, m_uiDataBufferLen(0)
{
    memset(&m_stIOCtrlParam, 0, sizeof(m_stIOCtrlParam));
    m_listFrameInfo.clear();
}

///
/// @brief
///    析构函数
///
CStateSendBuffer::~CStateSendBuffer()
{
    FreeDataBuffer();
}

///
/// @brief
///    申请数据帧缓冲区
///
/// @param[in]  uiBufferSize    缓冲区字节数
///
/// @return
///     unsigned char*    指向缓冲区的指针
///
unsigned char* CStateSendBuffer::AllocDataBuffer( uint32_t uiBufferSize )
{
    // 释放先前的缓冲区
    FreeDataBuffer();

    // 申请内存
    m_uiDataBufferLen   = uiBufferSize;
    m_pucDataBuffer     = new unsigned char[uiBufferSize];

    return m_pucDataBuffer;
}

///
/// @brief
///    释放数据帧缓冲区
///
void CStateSendBuffer::FreeDataBuffer()
{
    if (m_pucDataBuffer)
    {
        delete[] m_pucDataBuffer;
        m_pucDataBuffer = NULL;
    }

    m_uiDataBufferLen = 0;
}

///
/// @brief
///    修改所有发送帧的变位时间字节
///
/// @param[in]  stStateStartTime    新的变位时间
///
/// @return
///     bool    成功返回true
///
bool CStateSendBuffer::UpdateEventTime(CEventTime stStateStartTime)
{
    unsigned char*  pDataBuffer = NULL;
    std::list<CFrameInfo>::iterator  iter;


    // 遍历待发送缓冲中的每一帧
    for (iter = m_listFrameInfo.begin(); iter != m_listFrameInfo.end(); iter++)
    {
        // 获得发送帧信息结构体
        CFrameInfo& stFrameInfo = *iter;
        if (NULL == stFrameInfo.m_pGooseCb)
        {
            return false;
        }

        // 获得变位时间字节的位置（发送缓冲地址 + 帧偏移 + 变位时间偏移）
        pDataBuffer = m_pucDataBuffer + stFrameInfo.m_uiFrameOffset + stFrameInfo.m_pGooseCb->m_stPduPblcOff.eventTmOff;

        // 修改变位时间字节的值
        if (0 == stStateStartTime.CodeGooseEventTime(pDataBuffer, stFrameInfo.m_uiFrameLen))
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    修改所有发送帧中变位时间通道的值
///
/// @param[in]  stStateStartTime    新的变位时间
///
/// @return
///     bool    成功返回true
///
bool CStateSendBuffer::UpdateChannelTime( CEventTime stStateStartTime )
{
    unsigned char*  pucFrameHead = NULL;
    std::list<CFrameInfo>::iterator  iter;


    // 遍历每一个控制块
    for (iter = m_listFrameInfo.begin(); iter != m_listFrameInfo.end(); iter++)
    {
        CFrameInfo& stFrameInfo = *iter;
        if (stFrameInfo.m_pGooseCb)
        {
            // 获得发送帧的起始位置（发送缓冲地址 + 帧偏移）
            pucFrameHead = m_pucDataBuffer + stFrameInfo.m_uiFrameOffset;

            // 更新发送帧中变位的时间通道的值
            if (!stFrameInfo.__UpdateChannelTime(pucFrameHead, stStateStartTime))
            {
                return false;
            }
        }
    }

    return true;
}

///
/// @brief
///     对发生了变位的GOOSE控制块进行重新编码
///
/// @return
///     bool    成功返回true
///
bool CStateSendBuffer::RecodeChangedGooseCb()
{
    unsigned char*  pucFrameHead    = NULL;
    unsigned char*  pucFrameStNum   = NULL;
    std::list<CFrameInfo>::iterator  iter;


    // 遍历每一个控制块
    for (iter = m_listFrameInfo.begin(); iter != m_listFrameInfo.end(); iter++)
    {
        CFrameInfo& stFrameInfo = *iter;
        if (stFrameInfo.m_pGooseCb && stFrameInfo.m_pGooseCb->IsSendValueChanged())
        {
            // 获得发送帧的起始位置（发送缓冲地址 + 帧偏移）
            pucFrameHead = m_pucDataBuffer + stFrameInfo.m_uiFrameOffset;

            // 对发生了变位的控制块通道进行重新编码
            if (!stFrameInfo.m_pGooseCb->__RecodeChangedChannels(pucFrameHead))
            {
                return false;
            }

            // 获得stNum字段的位置（发送缓冲地址 + 帧偏移 + 变位时间偏移）
            pucFrameStNum = m_pucDataBuffer + stFrameInfo.m_uiFrameOffset + stFrameInfo.m_pGooseCb->m_stPduPblcOff.stNumOff;

            // 修改stNum字段的值
            CodeGooseStNum(stFrameInfo.m_pGooseCb->GetStNum(), pucFrameStNum, stFrameInfo.m_uiFrameLen);

            // 复位变位标识
            stFrameInfo.m_pGooseCb->SetSendValueChanged(false);
        }
    }

    return true;
}

///
/// @brief
///     拷贝发送缓冲
///
/// @param[in]  pOldStateSendBuffer     老的发送缓冲
///
/// @return
///     bool    拷贝成功返回true
///
bool CStateSendBuffer::Copy( CStateSendBuffer* pOldStateSendBuffer )
{
    // 释放先前的缓冲区
    FreeDataBuffer();

    // 拷贝发送配置信息
    m_ePortName         = pOldStateSendBuffer->m_ePortName;
    m_stIOCtrlParam     = pOldStateSendBuffer->m_stIOCtrlParam;
    m_uiDataBufferLen   = pOldStateSendBuffer->m_uiDataBufferLen;
    m_listFrameInfo.assign(pOldStateSendBuffer->m_listFrameInfo.begin(), pOldStateSendBuffer->m_listFrameInfo.end());

    // 申请缓冲区空间
    m_pucDataBuffer     = new unsigned char[m_uiDataBufferLen];
    if (NULL == m_pucDataBuffer)
    {
        return false;
    }

    // 拷贝缓冲区
    if(0 != memcpy_s(m_pucDataBuffer, m_uiDataBufferLen, pOldStateSendBuffer->m_pucDataBuffer, pOldStateSendBuffer->m_uiDataBufferLen))
    {
        FreeDataBuffer();
        return false;
    }

    return true;
}

///
/// @brief
///     查找GOOSE控制块中需要变位的时间通道
///
void CStateSendBuffer::__SaveGooseCbTimeChannel()
{
    std::list<CFrameInfo>::iterator  iter;


    // 遍历待发送缓冲中的每一帧
    for (iter = m_listFrameInfo.begin(); iter != m_listFrameInfo.end(); iter++)
    {
        // 获得发送帧信息结构体
        CFrameInfo& stFrameInfo = *iter;

        // 查找GOOSE控制块中所有需要变位的时间通道，将通道指针保存到容器中
        stFrameInfo.__SaveTimeChannel();
    }
}

///
/// @brief
///     查找GOOSE控制块中所有需要变位的时间通道，将通道指针保存到容器中
///
void CFrameInfo::__SaveTimeChannel()
{
    if (m_pGooseCb)
    {
        // 查找GOOSE控制块中所有需要变位的时间通道，将通道指针保存到容器中
        m_pGooseCb->__SaveTimeChannel(m_listTimeChannel);
    }
}

///
/// @brief
///    更新发送帧中需要变位的时间通道的值
///
/// @param[in]  pucFrameHead    指向发送帧帧头的指针
/// @param[in]  stNewTime       新的变位时间
///
/// @return
///     bool    成功返回true
///
bool CFrameInfo::__UpdateChannelTime( unsigned char* pucFrameHead, CEventTime stNewTime )
{
    CChannel*       pChannel;
    std::list<CChannel*>::iterator  iter;


    // 遍历每一个需要变位的时间通道
    iter = m_listTimeChannel.begin();
    for (; iter != m_listTimeChannel.end(); iter++)
    {
        pChannel = *iter;

        // 如果是时间类型通道
        if (pChannel && (CHANNEL_TYPE_GOOSE_TIME == pChannel->GetChannelType()))
        {
            // 获得通道发送值
            GOOSE_DATA_VARIANT& stSendValue = pChannel->GetGooseSendValue();
            if (GSEDT_UTC_TIME != stSendValue.m_stUnKnown.id.type)
            {
                return false;
            }

            // 修改通道发送值
            stSendValue.m_stUtcTime.t_s     = stNewTime.ts_s;
            stSendValue.m_stUtcTime.t_ns    = stNewTime.ts_ns;

            // 修改缓冲区中时间通道的报文值
            stSendValue.CodeGooseData(pucFrameHead + stSendValue.m_stUtcTime.id.offset, 10);
        }
    }

    return true;
}
