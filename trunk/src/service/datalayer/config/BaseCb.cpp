///
/// @brief
///     控制块基类
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.29
///
/// 修改历史：
///

#include "StdAfx.h"
#include "BaseCb.h"
#include "src\service\datalayer\scl\SCLManager.h"
#include "src\service\datalayer\include\datatypes.h"

///
/// @brief
///     构造函数
///
CBaseCb::CBaseCb(void)
{
}

///
/// @brief
///     构造函数
///
/// @param[in]  
///     pParamConfigBase    指向参数配置管理器的指针
///
CBaseCb::CBaseCb(CParamConfigBase* pParamConfigBase)
: m_pParamConfigBase(pParamConfigBase)
, m_bSelected(false)
, m_eFrameType(FRAME_TYPE_UNKOWN)
#if SHENG_ZE
, m_strDescription(L"新增SMV发送")
#else
, m_strDescription(L"新增SV发送")
#endif
, m_ePortName(DSM_PORT_UNKOWN)
, m_iAPPID(0x4001)
, m_pChannelHead(NULL)
, m_pChannelTail(NULL)
, m_iChannelCount(0)
{
}

///
/// @brief
///    析构函数
///
CBaseCb::~CBaseCb(void)
{
    // 清除所有通道
    ClearAllChannel();
}

///
/// @brief
///    获得控制块选中状态
///
/// @return
///     bool    返回true表示控制块被选中
///
bool CBaseCb::IsSelected()
{
    return m_bSelected;
}

///
/// @brief
///    设置控制块选中状态
///
/// @param[in]  bSelected  控制块选中状态
///
void CBaseCb::Selected(bool bSelected)
{
    if (m_bSelected != bSelected)
    {
        m_bSelected = bSelected;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得控制块描述信息
///
/// @return
///     wstring    控制块描述信息
///
std::wstring CBaseCb::GetDescription()
{
    return m_strDescription;
}

///
/// @brief
///    设置控制块描述
///
/// @param[in]  strDescription  控制块描述
///
void CBaseCb::SetDescription(std::wstring strDescription)
{
    if (m_strDescription != strDescription)
    {
        m_strDescription = strDescription;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得收发端口
///
/// @return
///     DsmPortName    收发端口
///
DsmPortName CBaseCb::GetPortName()
{
    return m_ePortName;
}

///
/// @brief
///    设置收发端口
///
/// @param[in]  eNewPortName  收发端口
///
void CBaseCb::SetPortName(DsmPortName eNewPortName)
{
    if (m_ePortName != eNewPortName)
    {
        m_ePortName = eNewPortName;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得APPID
///
/// @return
///     int    APPID
///
int CBaseCb::GetAPPID()
{
    return m_iAPPID;
}

///
/// @brief
///    设置APPID
///
/// @param[in]  iNewAPPID  新的APPID
///
/// @return
///     bool    true表示保存成功
///
bool CBaseCb::SetAPPID(int iNewAPPID)
{
    if ((iNewAPPID < 0) || (iNewAPPID > 65535))
    {
        return false;
    }

    if (m_iAPPID != iNewAPPID)
    {
        m_iAPPID = iNewAPPID;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }

    return true;
}

///
/// @brief
///    添加Channel
///
/// @param[in]  pNewChannel 指向Channel对象的指针
///
/// @return
///     bool    true表示添加成功
///
bool CBaseCb::__AddChannel(CChannel* pNewChannel)
{
    if (NULL == pNewChannel)
    {
        return false;
    }

    // 更改通道的前后指针
    pNewChannel->m_pPrev    = m_pChannelTail;
    pNewChannel->m_pNext    = NULL;
    pNewChannel->__SetParentBaseCb(this);

    // 更改通道的首指针
    if (NULL == m_pChannelHead)
    {
        m_pChannelHead = pNewChannel;
    }

    // 更改通道的尾指针
    if (m_pChannelTail)
    {
        m_pChannelTail->m_pNext = pNewChannel;
    }
    m_pChannelTail = pNewChannel;

    // 增加通道数
    m_iChannelCount++;
    return true;
}

///
/// @brief
///    添加Channel
///
/// @return
///     CChannel*    新添加的Channel对象的指针
///
CChannel* CBaseCb::AddChannel()
{
    int     iChannelCount;


    // 检查通道数是否已达最大限制
    iChannelCount = GetChannelCount();
    if (iChannelCount >= SMV_CHANNEL_NUM_MAX)
    {
        return NULL;
    }

    // new一个通道对象
    CChannel*  pChannel = new CChannel(this, NULL, m_pParamConfigBase);
    if (NULL == pChannel)
    {
        return NULL;
    }

    //
    // 如果当前控制块没有通道，那么设置通道一次值、二次值为基本设置中的缺省值
    // 如果当前控制块有通道，那么复制最后一个通道的属性给新增的通道
    //
    if (0 == iChannelCount)
    {
        pChannel->SetDescription(_T("电流通道"));                      ///< 通道描述
        pChannel->SetChannelType(CHANNEL_TYPE_SMV_CURRENT);             ///< 类型
        pChannel->SetChannelPhase(CHANNEL_PHASE_A);                     ///< 相位
    }
    else
    {
        pChannel->__Copy(LastChannel());                                ///< 复制前一个通道
    }

    // 添加通道对象指针到容器中
    if (!__AddChannel(pChannel))
    {
        delete pChannel;
        return NULL;
    }

    // 标记为已修改
    if (NULL != m_pParamConfigBase)
    {
        m_pParamConfigBase->SetModified(true);
    }

    return pChannel;
}

///
/// @brief
///    删除指定ID的通道
///
/// @param[in]  pDelChannel 通道对象指针
///
/// @return
///     bool    true表示删除成功
///
bool CBaseCb::DeleteChannel(CChannel* pDelChannel)
{
    // 标识大于等于1
    if ((NULL == pDelChannel) || (0 == m_iChannelCount))
    {
        return false;
    }

    bool    bFlag = false;
    // 如果只有一个通道
    if ((m_pChannelHead == m_pChannelTail) && (m_pChannelHead == pDelChannel))
    {
        m_pChannelHead  = NULL;
        m_pChannelTail  = NULL;
        bFlag = true;
    }
    // 如果有多个通道
    else
    {
        // 如果删除第一个通道
        if (m_pChannelHead == pDelChannel)
        {
            m_pChannelHead  = m_pChannelHead->m_pNext;
            m_pChannelHead->m_pPrev = NULL;
            bFlag = true;
        }
        // 如果删除最后一个通道
        else if (m_pChannelTail == pDelChannel)
        {
            m_pChannelTail  = m_pChannelTail->m_pPrev;
            m_pChannelTail->m_pNext = NULL;
            bFlag = true;
        }
        // 如果删除其他通道
        else
        {
            // 遍历所有通道
            for (CChannel* pChannel = m_pChannelTail->m_pPrev; pChannel != NULL; pChannel = pChannel->m_pPrev)
            {
                // 如果找到待删除的通道
                if (pChannel == pDelChannel)
                {
                    pChannel->m_pPrev->m_pNext = pChannel->m_pNext;
                    pChannel->m_pNext->m_pPrev = pChannel->m_pPrev;
                    bFlag = true;
                    break;
                }
            }
        }
    }

    // 如果找到了待删除的通道
    if (bFlag)
    {
        m_iChannelCount--;
        delete pDelChannel;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }

    return bFlag;
}

///
/// @brief
///    枚举第一个Channel
///
/// @return
///     CChannel*    成功时返回GOOSE控制块指针，失败时返回NULL
///
CChannel* CBaseCb::FirstChannel()
{
    return m_pChannelHead;
}

///
/// @brief
///    枚举下一个Channel
///
/// @param[in]  pCurChannel 当前Channel的指针
///
/// @return
///     CChannel*    成功时返回Channel指针，失败时返回NULL
///
CChannel* CBaseCb::NextChannel(CChannel* pCurChannel)
{
    if (NULL == pCurChannel)
    {
        return NULL;
    }

    return pCurChannel->m_pNext;
}

///
/// @brief
///    枚举上一个Channel
///
/// @param[in]  pCurChannel 当前Channel的指针
///
/// @return
///     CChannel*    成功时返回Channel指针，失败时返回NULL
///
CChannel* CBaseCb::PrevChannel(CChannel* pCurChannel)
{
    if (NULL == pCurChannel)
    {
        return NULL;
    }

    return pCurChannel->m_pPrev;
}

///
/// @brief
///    枚举最后一个Channel
///
/// @return
///     CChannel*    成功时返回Channel指针，失败时返回NULL
///
CChannel* CBaseCb::LastChannel()
{
    return m_pChannelTail;
}

///
/// @brief
///    清除所有通道
///
void CBaseCb::ClearAllChannel()
{
    // 遍历链表，释放对象
    for (CChannel* pChannel = m_pChannelTail; pChannel != NULL; pChannel = pChannel->m_pPrev)
    {
        delete pChannel;
    }

    // 复位链表标记变量
    if (m_iChannelCount)
    {
        m_pChannelHead  = NULL;
        m_pChannelTail  = NULL;
        m_iChannelCount = 0;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得通道数
///
/// @return
///     int    通道数
///
int CBaseCb::GetChannelCount()
{
    return m_iChannelCount;
}

///
/// @brief
///    获得被选中的通道数
///
/// @return
///     int    被选中的通道数
///
int CBaseCb::GetSelectedChannelCount()
{
    int                             iCount      = 0;
    CChannel*                       pChannel    = NULL;


    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        // 判断通道是否被选中
        if (pChannel && pChannel->IsSelected())
        {
            iCount++;
        }
    }

    return iCount;
}

///
/// @brief
///    深度拷贝一个控制块
///
/// @param[in]  pBaseCb    指向基本控制块对象的指针
///
/// @return
///     bool    拷贝成功返回TRUE
///
bool CBaseCb::__Copy( CBaseCb* pBaseCb )
{
    CChannel*           pChannel    = NULL;
    CChannel*           pNewChannel = NULL;


    // 检查输入参数
    if (NULL == pBaseCb)
    {
        return false;
    }

    // 拷贝基本属性
    m_strDescription    = pBaseCb->m_strDescription;
    m_ePortName         = pBaseCb->m_ePortName;
    m_iAPPID            = pBaseCb->m_iAPPID;



    // 清空所有通道
    ClearAllChannel();

    // 拷贝通道信息
    for (pChannel = pBaseCb->m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        pNewChannel = new CChannel(this, NULL, m_pParamConfigBase);
        if (NULL == pNewChannel)
        {
            return false;
        }

        // 深度拷贝通道
        if (!pNewChannel->__Copy(pChannel))
        {
            delete pNewChannel;
            return false;
        }

        // IEC61850 9-2通道没有测量类型
        if (FRAME_TYPE_NET_9_2 == m_eFrameType)
        {
            pNewChannel->m_bMeasure = false;
        }

        // 添加通道到容器中
        if (!__AddChannel(pNewChannel))
        {
            delete pNewChannel;
            return false;
        }
    }

    return true;
}

///
/// @brief
///    深度拷贝一个扫描列表中的GOOSE控制块
///
/// @param[in]
///     pGooseMsg    -   GOOSE报文的编解码对象
///
/// @return
///     bool    拷贝成功返回TRUE
///
bool CBaseCb::__Copy(GooseMsg* pGooseMsg)
{
    int                 iChannelCount   = 0;

    CChannel*           pNewChannel     = NULL;
    CSCLManager*        pSCLManager     = NULL;
    CBaseCb*            pBasecb         = NULL;


    // 检查输入参数
    if (NULL == pGooseMsg)
    {
        return false;
    }

    IEC8802_3_PDU_PUBLIC    stPduPublic;
    pGooseMsg->GetIEC8802Info(stPduPublic);
    m_iAPPID                = stPduPublic.appID;                        ///< APPID

    // 获得全站配置文件管理器对象
    pSCLManager = CSCLManager::getInstance();
    if (pSCLManager)
    {
        pBasecb = pSCLManager->GetGooseCb(m_iAPPID);
        if (pBasecb)
        {
            m_strDescription = pBasecb->GetDescription();               ///< 控制块描述信息
        }
    }

    // 清空所有通道
    ClearAllChannel();

    //
    // 拷贝通道
    //
    iChannelCount = (int)pGooseMsg->GetChannelNum();
    for (int i = 0; i < iChannelCount; i++)
    {
        GOOSE_DATA* pGooseData = pGooseMsg->GetAChannel(i);
        if (pGooseData && (GSEDT_UNKNOW != pGooseData->type))
        {
            pNewChannel = new CChannel(this, NULL, m_pParamConfigBase);
            if (NULL == pNewChannel)
            {
                return false;
            }

            // 深度拷贝通道
            if (!pNewChannel->__Copy(pGooseData))
            {
                delete pNewChannel;
                return false;
            }

            //
            // 设置通道描述信息
            //
            if (pBasecb)
            {
                // 获得全站配置文件中对应编号的通道
                CChannel* pSclChannel = pBasecb->GetChannelByIndex(i);
                if (pSclChannel)
                {
                    // 拷贝全站配置文件中对应通道的描述信息
                    pNewChannel->m_strDescription   = pSclChannel->GetDescription();
                }
            }

            // 添加通道到容器中
            if (!__AddChannel(pNewChannel))
            {
                delete pNewChannel;
                return false;
            }
        }
    }

    return true;
}

///
/// @brief
///    深度拷贝一个扫描列表中的IEC61850-9-1控制块
///
/// @param[in]
///     Sav91Msg    -   IEC61850-9-1报文的编解码对象
///
/// @return
///     bool    拷贝成功返回TRUE
///
bool CBaseCb::__Copy(Sav91Msg* pSav91Msg)
{
    int                 iChannelCount   = 0;

    CChannel*           pNewChannel     = NULL;
    CSCLManager*        pSCLManager     = NULL;
    CBaseCb*            pBasecb         = NULL;


    // 检查输入参数
    if (NULL == pSav91Msg)
    {
        return false;
    }

    IEC8802_3_PDU_PUBLIC    stPduPublic;
    pSav91Msg->GetIEC8802Info(stPduPublic);
    m_iAPPID                = stPduPublic.appID;                        ///< APPID

    // 获得全站配置文件管理器对象
    pSCLManager = CSCLManager::getInstance();
    if (pSCLManager)
    {
        pBasecb = pSCLManager->GetIEC9_2Cb(m_iAPPID);
        if (pBasecb)
        {
            m_strDescription = pBasecb->GetDescription();               ///< 控制块描述信息
        }
    }

    // 清空所有通道
    ClearAllChannel();

    //
    // 拷贝通道
    //
    iChannelCount = (int)pSav91Msg->GetASDUDefaultSmpsNum();
    for (int i = 0; i < iChannelCount; i++)
    {
        pNewChannel = new CChannel(this, NULL, m_pParamConfigBase);
        if (NULL == pNewChannel)
        {
            return false;
        }

        // 如果全站配置文件中存在相同APPID的控制块，那么复制其属性
        if (pBasecb && (i < pBasecb->GetChannelCount()))
        {
            pNewChannel->__Copy(pBasecb->GetChannelByIndex(i));
        }
        // 如果全站配置文件中不含相同APPID的控制块，那么使用缺省属性
        else
        {
            // 由于无法从通道报文中获得通道描述、类型、相位、测量标志，所以此处用缺省数组来赋值
            if (i < sizeof(PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8)/sizeof(PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[0]))
            {
                // 通道类型
                pNewChannel->SetChannelType(PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].enChannelType);

                // 通道描述
                pNewChannel->m_strDescription   = PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].strChannelDesc;

                // 相位
                pNewChannel->m_eChannelPhase    = PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].enChannelPhase;

                // 是否属于测量类型
                pNewChannel->m_bMeasure         = (CHANNEL_VALUE_TYPE_MEASURE == PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].enChannelValueType);

                // 通道映射
                pNewChannel->m_strVirtualChannel= PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].strVirtualChannel;
            }
            else
            {
                wchar_t            cBuffer[100] = {0};


                // 通道描述
                _snwprintf(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]), L"通道#%d", i + 1);
                pNewChannel->m_strDescription   = cBuffer;
            }
        }

        // 添加通道到容器中
        if (!__AddChannel(pNewChannel))
        {
            delete pNewChannel;
            return false;
        }
    }

    return true;
}

///
/// @brief
///    深度拷贝一个扫描列表中的IEC61850-9-2控制块
///
/// @param[in]
///     pSav92Msg    -   IEC61850-9-2报文的编解码对象
///
/// @return
///     bool    拷贝成功返回TRUE
///
bool CBaseCb::__Copy(Sav92Msg* pSav92Msg)
{
    int                 iChannelCount   = 0;

    CChannel*           pNewChannel     = NULL;
    CSCLManager*        pSCLManager     = NULL;
    CBaseCb*            pBasecb         = NULL;


    // 检查输入参数
    if (NULL == pSav92Msg)
    {
        return false;
    }

    IEC8802_3_PDU_PUBLIC    stPduPublic;
    pSav92Msg->GetIEC8802Info(stPduPublic);
    m_iAPPID                = stPduPublic.appID;                        ///< APPID

    // 获得全站配置文件管理器对象
    pSCLManager = CSCLManager::getInstance();
    if (pSCLManager)
    {
        pBasecb = pSCLManager->GetIEC9_2Cb(m_iAPPID);
        if (pBasecb)
        {
            m_strDescription = pBasecb->GetDescription();               ///< 控制块描述信息
        }
    }

    // 清空所有通道
    ClearAllChannel();

    //
    // 拷贝通道
    //
    iChannelCount = (int)pSav92Msg->GetASDUSmpsNum(0);
    for (int i = 0; i < iChannelCount; i++)
    {
        pNewChannel = new CChannel(this, NULL, m_pParamConfigBase);
        if (NULL == pNewChannel)
        {
            return false;
        }

        // 如果全站配置文件中存在相同APPID的控制块，那么复制其属性
        if (pBasecb && (i < pBasecb->GetChannelCount()))
        {
            pNewChannel->__Copy(pBasecb->GetChannelByIndex(i));
            pNewChannel->m_bMeasure = false;
        }
        // 如果全站配置文件中不含相同APPID的控制块，那么使用缺省属性
        else
        {
            // 由于无法从通道报文中获得通道描述、类型、相位、测量标志，所以此处用缺省数组来赋值
            if (i < sizeof(PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO)/sizeof(PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO[0]))
            {
                // 通道类型
                pNewChannel->SetChannelType(PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO[i].enChannelType);

                // 通道描述
                pNewChannel->m_strDescription   = PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO[i].strChannelDesc;

                // 相位
                pNewChannel->m_eChannelPhase    = PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO[i].enChannelPhase;

                // 是否属于测量类型
                pNewChannel->m_bMeasure         = (CHANNEL_VALUE_TYPE_MEASURE == PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO[i].enChannelValueType);

                // 通道映射
                pNewChannel->m_strVirtualChannel= PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO[i].strVirtualChannel;
            }
            else
            {
                wchar_t            cBuffer[100] = {0};


                // 通道描述
                _snwprintf(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]), L"通道#%d", i + 1);
                pNewChannel->m_strDescription   = cBuffer;
            }
        }

        // 添加通道到容器中
        if (!__AddChannel(pNewChannel))
        {
            delete pNewChannel;
            return false;
        }
    }

    return true;
}

///
/// @brief
///    深度拷贝一个扫描列表中的FT3控制块
///
/// @param[in]
///     pFt3Msg    -   FT3报文的编解码对象
///
/// @return
///     bool    拷贝成功返回TRUE
///
bool CBaseCb::__Copy(NanRuiFT3Msg* pFt3Msg)
{
    int                 iChannelCount   = 0;

    CChannel*           pNewChannel     = NULL;
    CSCLManager*        pSCLManager     = NULL;
    CBaseCb*            pBasecb         = NULL;


    // 检查输入参数
    if (NULL == pFt3Msg)
    {
        return false;
    }

    // 获得全站配置文件管理器对象
    pSCLManager = CSCLManager::getInstance();
    if (pSCLManager)
    {
        pBasecb = pSCLManager->GetIEC9_2Cb(m_iAPPID);
        if (pBasecb)
        {
            m_strDescription = pBasecb->GetDescription();               ///< 控制块描述信息
        }
    }

    // 清空所有通道
    ClearAllChannel();

    //
    // 拷贝通道
    //
    iChannelCount = (int)pFt3Msg->GetASDUSmpsNum();
    for (int i = 0; i < iChannelCount; i++)
    {
        pNewChannel = new CChannel(this, NULL, m_pParamConfigBase);
        if (NULL == pNewChannel)
        {
            return false;
        }

        // 如果全站配置文件中存在相同APPID的控制块，那么复制其属性
        if (pBasecb && (i < pBasecb->GetChannelCount()))
        {
            pNewChannel->__Copy(pBasecb->GetChannelByIndex(i));
        }
        // 如果全站配置文件中不含相同APPID的控制块，那么使用缺省属性
        else
        {
            // 由于无法从通道报文中获得通道描述、类型、相位、测量标志，所以此处用缺省数组来赋值
            if (i < sizeof(PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8)/sizeof(PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[0]))
            {
                // 通道类型
                pNewChannel->SetChannelType(PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].enChannelType);

                // 通道描述
                pNewChannel->m_strDescription   = PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].strChannelDesc;

                // 相位
                pNewChannel->m_eChannelPhase    = PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].enChannelPhase;

                // 是否属于测量类型
                pNewChannel->m_bMeasure         = (CHANNEL_VALUE_TYPE_MEASURE == PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].enChannelValueType);

                // 通道映射
                pNewChannel->m_strVirtualChannel= PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[i].strVirtualChannel;
            }
            else
            {
                wchar_t            cBuffer[100] = {0};


                // 通道描述
                _snwprintf(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]), L"通道#%d", i + 1);
                pNewChannel->m_strDescription   = cBuffer;
            }
        }

        // 添加通道到容器中
        if (!__AddChannel(pNewChannel))
        {
            delete pNewChannel;
            return false;
        }
    }

    return true;
}

///
/// @brief
///    深度拷贝一个扫描列表中的FT3扩展控制块
///
/// @param[in]
///     pFt3ExpandMsg    -   FT3扩展报文的编解码对象
///
/// @return
///     bool    拷贝成功返回TRUE
///
bool CBaseCb::__Copy(GuoWangFT3Msg* pFt3ExpandMsg)
{
    int                 iChannelCount   = 0;

    CChannel*           pNewChannel     = NULL;
    CSCLManager*        pSCLManager     = NULL;
    CBaseCb*            pBasecb         = NULL;


    // 检查输入参数
    if (NULL == pFt3ExpandMsg)
    {
        return false;
    }

    // 获得全站配置文件管理器对象
    pSCLManager = CSCLManager::getInstance();
    if (pSCLManager)
    {
        pBasecb = pSCLManager->GetIEC9_2Cb(m_iAPPID);
        if (pBasecb)
        {
            m_strDescription = pBasecb->GetDescription();               ///< 控制块描述信息
        }
    }

    // 清空所有通道
    ClearAllChannel();

    //
    // 拷贝通道
    //
    iChannelCount = (int)pFt3ExpandMsg->GetASDUSmpsNum();
    for (int i = 0; i < iChannelCount; i++)
    {
        pNewChannel = new CChannel(this, NULL, m_pParamConfigBase);
        if (NULL == pNewChannel)
        {
            return false;
        }

        // 如果全站配置文件中存在相同APPID的控制块，那么复制其属性
        if (pBasecb && (i < pBasecb->GetChannelCount()))
        {
            pNewChannel->__Copy(pBasecb->GetChannelByIndex(i));
        }
        // 如果全站配置文件中不含相同APPID的控制块，那么使用缺省属性
        else
        {
            // 由于无法从通道报文中获得通道描述、类型、相位、测量标志，所以此处用缺省数组来赋值
            if (i < sizeof(PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG)/sizeof(PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[0]))
            {
                // 通道类型
                pNewChannel->SetChannelType(PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[i].enChannelType);

                // 通道描述
                pNewChannel->m_strDescription   = PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[i].strChannelDesc;

                // 相位
                pNewChannel->m_eChannelPhase    = PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[i].enChannelPhase;

                // 是否属于测量类型
                pNewChannel->m_bMeasure         = (CHANNEL_VALUE_TYPE_MEASURE == PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[i].enChannelValueType);

                // 通道映射
                pNewChannel->m_strVirtualChannel= PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[i].strVirtualChannel;
            }
            else
            {
                wchar_t            cBuffer[100] = {0};


                // 通道描述
                _snwprintf(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]), L"通道#%d", i + 1);
                pNewChannel->m_strDescription   = cBuffer;
            }
        }

        // 添加通道到容器中
        if (!__AddChannel(pNewChannel))
        {
            delete pNewChannel;
            return false;
        }
    }

    return true;
}

///
/// @brief
///    获得控制块的第nIndex（从0开始计数）个通道
///
/// @param[in]  nIndex    通道序号
///
/// @return
///     CChannel*    成功时返回Channel指针，失败时返回NULL
///
CChannel* CBaseCb::GetChannelByIndex( int nIndex )
{
    int                             iCount      = 0;
    CChannel*                       pChannel    = NULL;


    // 检查索引是否在合法范围
    if ((nIndex < 0) || (nIndex >= m_iChannelCount))
    {
        return NULL;
    }

    // 查找第nIndex个通道，返回通道指针
    for(pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        if (iCount == nIndex)
        {
            return pChannel;
        }

        iCount++;
    }

    return NULL;
}

///
/// @brief
///    设置通道的数量
///    如果当前通道数大于新的通道数，那么删除末尾的多余通道
///    如果当前通道数小于新的通道数，那么添加通道
///
/// @param[in]  uiNewCount    新的通道数
///
/// @return
///     bool    成功返回true，失败返回false
///
bool CBaseCb::SetChannelCount( unsigned int uiNewCount )
{
    unsigned int     uiOldCount;


    // 检查输入参数
    if (uiNewCount < 1)
    {
        return false;
    }

    // 如果新的通道数大于原通道数
    uiOldCount = m_iChannelCount;
    if (uiNewCount > uiOldCount)
    {
        // 向容器中添加新的通道
        for (unsigned int i = uiOldCount; i < uiNewCount; i++)
        {
            if (NULL == AddChannel())
            {
                return false;
            }
        }
    }
    // 如果新的通道数小于原通道数
    else if (uiNewCount < uiOldCount)
    {
        for (unsigned int i = uiOldCount; i > uiNewCount; i--)
        {
            if (!DeleteChannel(LastChannel()))
            {
                return false;
            }
        }
    }
    // 如果新的通道数等于原通道数
    else
    {
        // 不做任何操作
    }

    return true;
}

///
/// @brief
///    使用指定的通道模板来填充控制块
///
/// @param[in]  eType    模板类型
///
/// @return
///     bool    成功返回true
///
bool CBaseCb::UseChannelTemplate( enumSmvRecvConfigTemplateType eType )
{
    return false;
}

///
/// @brief
///    更新所有控制块中电压通道的一次额定值
///
/// @param[in]  dNewValue    新的电压一次额定值（V）
///
void CBaseCb::UpdateRatedPrimaryVoltage( double dNewValue )
{
    CChannel*                       pChannel    = NULL;


    // 遍历所有通道
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        if (pChannel->GetChannelType() == CHANNEL_TYPE_SMV_VOLTAGE)
        {
            pChannel->SetOnceRatings(dNewValue);
        }
    }
}

///
/// @brief
///    更新所有控制块中电压通道的二次额定值
///
/// @param[in]  dNewValue    新的电压二次额定值（V）
///
void CBaseCb::UpdateRatedSecondaryVoltage( double dNewValue )
{
    CChannel*                       pChannel    = NULL;


    // 遍历所有通道
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        if (pChannel->GetChannelType() == CHANNEL_TYPE_SMV_VOLTAGE)
        {
            pChannel->SetSecondaryRatings(dNewValue);
        }
    }
}

///
/// @brief
///    更新所有控制块中电流通道的一次额定值
///
/// @param[in]  dNewValue    新的电流一次额定值（A）
///
void CBaseCb::UpdateRatedPrimaryCurrent( double dNewValue )
{
    CChannel*                       pChannel    = NULL;


    // 遍历所有通道
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        if (pChannel->GetChannelType() == CHANNEL_TYPE_SMV_CURRENT)
        {
            pChannel->SetOnceRatings(dNewValue);
        }
    }
}

///
/// @brief
///    更新所有控制块中电流通道的二次额定值
///
/// @param[in]  dNewValue    新的电流二次额定值（A）
///
void CBaseCb::UpdateRatedSecondaryCurrent( double dNewValue )
{
    CChannel*                       pChannel    = NULL;


    // 遍历所有通道
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        if (pChannel->GetChannelType() == CHANNEL_TYPE_SMV_CURRENT)
        {
            pChannel->SetSecondaryRatings(dNewValue);
        }
    }
}

///
/// @brief
///    更新所有控制块中时间通道的额定值
///
/// @param[in]  dNewValue    新的MU额定延时（us）
///
void CBaseCb::UpdateMuRatedDelay( double dNewValue )
{
    CChannel*                       pChannel    = NULL;


    // 遍历所有通道
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        if (pChannel->GetChannelType() == CHANNEL_TYPE_SMV_TIME)
        {
            pChannel->SetOnceRatings(dNewValue);
            pChannel->SetSecondaryRatings(dNewValue);
        }
    }
}
