/// @file
///
/// @brief
///     GOOSE控制块
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.27
///
/// 修改历史：
///

#include "StdAfx.h"
#include "GooseCb.h"

#define NODE_Gocb_Description           L"Description"                  ///< 控制块描述
#define NODE_Gocb_PortName              L"PortName"                     ///< 收发端口
#define NODE_Gocb_APPID                 L"APPID"                        ///< APPID
#define NODE_Gocb_SRC_MAC               L"SrcMAC"                       ///< 源MAC
#define NODE_Gocb_DES_MAC               L"DesMAC"                       ///< 目标MAC
#define NODE_Gocb_VLanID                L"VLanID"                       ///< VLAN ID
#define NODE_Gocb_VLanPriority          L"VLanPriority"                 ///< VLAN优先级
#define NODE_Gocb_ConfigRevision        L"ConfigRevision"               ///< 配置版本号
#define NODE_Gocb_TestFlag              L"TestFlag"                     ///< TestFlag
#define NODE_Gocb_NeedComm              L"NeedComm"                     ///< NeedComm
#define NODE_Gocb_GocbRef               L"GocbRef"                      ///< GocbRef
#define NODE_Gocb_DataSet               L"DataSet"                      ///< 数据集
#define NODE_Gocb_GoID                  L"GoID"                         ///< GoID
#define NODE_Gocb_ChannelGroup          L"ChannelGroup"                 ///< ChannelGroup
#define NODE_Channel                    L"Channel"                      ///< 通道
#define NODE_Channel_Description        L"Description"                  ///< 通道描述
#define NODE_Channel_Type               L"Type"                         ///< 通道类型
#define NODE_Channel_VirtualChannel     L"VirtualChannel"               ///< 映射
#define GOOSE_DATA_LEN_MAX              (1233)                          ///< GOOSE报文数据区的最大长度

#if !CHUN_HUA
#define NODE_Gocb_VLanTag                L"VLanTag"                       ///< VLAN tag
#endif

// 预定义的目标MAC地址
static const unsigned char PREDEFINED_GOOSE_DES_MAC[ETHERNET_MAC_LEN] = 
{
    0x01,
    0x0c,
    0xcd,
    0x04,
    0x00,
    0x01
};

///
/// @brief
///    构造函数
///
CGooseCb::CGooseCb(void)
{
}

///
/// @brief
///     构造函数
///
/// @param[in]  
///     pParamConfigBase    指向参数配置管理器的指针
///
CGooseCb::CGooseCb(CParamConfigBase* pParamConfigBase)
:CNetCb(pParamConfigBase)
,m_bTestFlag(false)
,m_bNeedComm(false)
,m_strGocbRef(L"gocbRef")
,m_strDataSet(L"dataSet")
,m_strGoID(L"goID")
,m_strSecurity(L"")
,m_stNum(1)
,m_bIsSendValueChanged(false)
{
    m_eFrameType    = FRAME_TYPE_NET_GOOSE;
    m_ePortName     = NET_PORT_LIGHT_A;
    m_iAPPID        = 0x2001;
#if SHENG_ZE
    m_strDescription= L"新增GOOSE发送";
#else
	m_strDescription= L"新增GSE发送";
#endif
    // 清空偏移量
    memset(&m_stPduPblcOff, 0, sizeof(m_stPduPblcOff));

    // 目标MAC地址
    memcpy_s(m_ucDesMAC, ETHERNET_MAC_LEN, PREDEFINED_GOOSE_DES_MAC, ETHERNET_MAC_LEN);
}

///
/// @brief
///    析构函数
///
CGooseCb::~CGooseCb(void)
{
}

///
/// @brief
///    获得TestFlag
///
/// @return
///     bool    TestFlag
///
bool CGooseCb::GetTestFlag()
{
    return m_bTestFlag;
}

///
/// @brief
///    设置TestFlag
///
/// @param[in]  bTestFlag  新的TestFlag
///
void CGooseCb::SetTestFlag(bool bTestFlag)
{
    if (m_bTestFlag != bTestFlag)
    {
        m_bTestFlag = bTestFlag;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得NeedComm
///
/// @return
///     bool    NeedComm
///
bool CGooseCb::GetNeedComm()
{
    return m_bNeedComm;
}

///
/// @brief
///    设置NeedComm
///
/// @param[in]  bNeedComm  新的NeedComm
///
void CGooseCb::SetNeedComm(bool bNeedComm)
{
    if (m_bNeedComm != bNeedComm)
    {
        m_bNeedComm = bNeedComm;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得gocbRef
///
/// @return
///     wstring    gocbRef
///
std::wstring CGooseCb::GetGocbRef()
{
    return m_strGocbRef;
}

///
/// @brief
///    设置gocbRef
///
/// @param[in]  strGocbRef  gocbRef
///
void CGooseCb::SetGocbRef(std::wstring strGocbRef)
{
    if (m_strGocbRef != strGocbRef)
    {
        m_strGocbRef = strGocbRef;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得数据集
///
/// @return
///     wstring    数据集
///
std::wstring CGooseCb::GetDataSet()
{
    return m_strDataSet;
}

///
/// @brief
///    设置数据集
///
/// @param[in]  strDataSet  数据集
///
void CGooseCb::SetDataSet(std::wstring strDataSet)
{
    if (m_strDataSet != strDataSet)
    {
        m_strDataSet = strDataSet;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得goID
///
/// @return
///     wstring    goID
///
std::wstring CGooseCb::GetGoID()
{
    return m_strGoID;
}

///
/// @brief
///    设置goID
///
/// @param[in]  strGoID  goID
///
void CGooseCb::SetGoID(std::wstring strGoID)
{
    if (m_strGoID != strGoID)
    {
        m_strGoID = strGoID;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    深度拷贝一个GOOSE控制块
///
/// @param[in]
///     pGocb    -   GOOSE控制块指针
///
/// @return
///     bool    拷贝成功返回TRUE
///
bool CGooseCb::__Copy( CGooseCb* pGocb )
{
    // 检查输入参数
    if (NULL == pGocb)
    {
        return false;
    }

    // 拷贝基类的属性
    if (!CNetCb::__Copy(pGocb))
    {
        return false;
    }

    // 拷贝CGooseCb的属性
    m_bTestFlag     = pGocb->m_bTestFlag;
    m_bNeedComm     = pGocb->m_bNeedComm;
    m_strGocbRef    = pGocb->m_strGocbRef;
    m_strDataSet    = pGocb->m_strDataSet;
    m_strGoID       = pGocb->m_strGoID;
    m_strSecurity   = pGocb->m_strSecurity;

    // 拷贝公共信息部分在报文帧中的偏移地址信息（状态序列时会用到）
    m_stPduPblcOff  = pGocb->m_stPduPblcOff;

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
bool CGooseCb::__Copy(GooseMsg* pGooseMsg)
{
    GOOSE_PDU_PUBLIC    stPublicInfo;


    // 输入参数检查
    if (NULL == pGooseMsg)
    {
        return false;
    }

    // 拷贝基类的属性
    if (!CNetCb::__Copy(pGooseMsg))
    {
        return false;
    }

    // 获取报文帧中的GoosePdu的公共信息
    if (!pGooseMsg->GetPduPublicInfo(stPublicInfo))
    {
        return false;
    }

    // 拷贝CGooseCb的属性
    m_bTestFlag         = stPublicInfo.test? true: false;    
    m_bNeedComm         = stPublicInfo.ndsCom? true: false;
    m_iConfigRevision   = stPublicInfo.confRev;
    m_strGocbRef.assign(stPublicInfo.gocbRef.begin(), stPublicInfo.gocbRef.end());
    m_strDataSet.assign(stPublicInfo.datSet.begin(), stPublicInfo.datSet.end());
    m_strGoID.assign(stPublicInfo.goID.begin(), stPublicInfo.goID.end());

    return true;
}

///
/// @brief
///    添加Channel
///
/// @return
///     CChannel*    新添加的Channel对象的指针
///
CChannel* CGooseCb::AddChannel()
{
    int     iTotalLen = 0;


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
    if (0 == GetChannelCount())
    {
        pChannel->SetDescription(_T("GOOSE通道"));                      ///< 通道描述
        pChannel->SetChannelType(CHANNEL_TYPE_GOOSE_POINT);             ///< 类型
    }
    else
    {
        pChannel->__Copy(LastChannel());                                ///< 复制前一个通道
    }

    // 判断通道长度是否超限
    iTotalLen = GetChannelValueLen();
    if (iTotalLen + pChannel->GetValueLength() > GOOSE_DATA_LEN_MAX)
    {
        delete pChannel;
        return NULL;
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
///    获得加密字符串
///
/// @return
///     std::wstring    加密字符串
///
std::wstring CGooseCb::GetSecurity()
{
    return m_strSecurity;
}

///
/// @brief
///    设置加密字符串
///
/// @param[in]  strNewSecurity    新的加密字符串
///
void CGooseCb::SetSecurity( std::wstring strNewSecurity )
{
    if (m_strSecurity != strNewSecurity)
    {
        m_strSecurity = strNewSecurity;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得状态序号
///
/// @return
///     int    状态序号
///
int CGooseCb::GetStNum()
{
    return m_stNum;
}

///
/// @brief
///    设置状态序号
///
/// @param[in]  iNewStNum    新的状态序号
///
void CGooseCb::SetStNum( int iNewStNum )
{
    if (m_stNum != iNewStNum)
    {
        m_stNum = iNewStNum;
    }
}

///
/// @brief
///    控制块中通道的发送值是否发生了改变
///
/// @return
///     bool    发送值是否被改变
///
bool CGooseCb::IsSendValueChanged()
{
    return m_bIsSendValueChanged;
}

///
/// @brief
///    设置控制块中通道的发送值是否发生了改变
///
/// @param[in]  bChanged    发送值是否被改变
///
void CGooseCb::SetSendValueChanged( bool bChanged )
{
    if (m_bIsSendValueChanged != bChanged)
    {
        m_bIsSendValueChanged = bChanged;
    }
}

///
/// @brief
///    把当前控制块与指定控制块相比较，判断通道发送值是否相等
///
/// @param[in]  pOldGocb    指向GOOSE控制块对象的指针
///
/// @return
///     bool    相等返回true，否则返回false
///
bool CGooseCb::__CompareChannelSendValue( CGooseCb* pOldGocb )
{
    bool                            bRet        = true;
    CChannel*                       pOldChannel = NULL;
    CChannel*                       pNewChannel = NULL;


    // 判断通道数是否相等
    if (m_iChannelCount != pOldGocb->m_iChannelCount)
    {
        return false;
    }

    // 依次比较每一个通道的发送值是否相等
    for (pOldChannel = pOldGocb->m_pChannelHead, pNewChannel = m_pChannelHead;
        pOldChannel != NULL && pNewChannel != NULL;
        pOldChannel = pOldChannel->m_pNext, pNewChannel = pNewChannel->m_pNext)
    {
        if (CHANNEL_TYPE_GOOSE_TIME != pNewChannel->GetChannelType())
        {
            // 比较通道发送值
            if (!pNewChannel->__CompareSendValue(pOldChannel))
            {
                // 标记通道的发送值发生了改变
                pNewChannel->SetSendValueChanged(true);
                bRet = false;
            }
        }
    }

    return bRet;
}

///
/// @brief
///    DO(s, q, t)
///
/// @return
///     bool    成功返回true，失败返回false
///
bool CGooseCb::DO_s_q_t()
{
    CChannel*                       pChannel   = NULL;


    // 遍历控制块的所有通道
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        // 将通道类型设置为结构类型
        if (!pChannel->SetChannelType(CHANNEL_TYPE_GOOSE_STRUCT))
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    DA(s, q, t)
///
/// @return
///     bool    成功返回true，失败返回false
///
bool CGooseCb::DA_s_q_t()
{
    CChannel*                       pChannel    = NULL;
    enumChannelType                 eNewType    = CHANNEL_TYPE_GOOSE_POINT;


    // 遍历控制块的所有通道
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        bool    bRet = false;


        // 设置通道类型
        if (CHANNEL_TYPE_GOOSE_POINT == eNewType)
        {
            bRet = pChannel->SetChannelType(eNewType);
            eNewType = CHANNEL_TYPE_GOOSE_QUALITY;
        }
        else if (CHANNEL_TYPE_GOOSE_QUALITY == eNewType)
        {
            bRet = pChannel->SetChannelType(eNewType);
            eNewType = CHANNEL_TYPE_GOOSE_TIME;
        }
        else if (CHANNEL_TYPE_GOOSE_TIME == eNewType)
        {
            bRet = pChannel->SetChannelType(eNewType);
            eNewType = CHANNEL_TYPE_GOOSE_POINT;
        }

        // 检查返回值
        if (!bRet)
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    DA(s, q)
///
/// @return
///     bool    成功返回true，失败返回false
///
bool CGooseCb::DA_s_q()
{
    CChannel*                       pChannel    = NULL;
    enumChannelType                 eNewType    = CHANNEL_TYPE_GOOSE_POINT;


    // 遍历控制块的所有通道
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        bool    bRet = false;


        // 设置通道类型
        if (CHANNEL_TYPE_GOOSE_POINT == eNewType)
        {
            bRet = pChannel->SetChannelType(eNewType);
            eNewType = CHANNEL_TYPE_GOOSE_QUALITY;
        }
        else if (CHANNEL_TYPE_GOOSE_QUALITY == eNewType)
        {
            bRet = pChannel->SetChannelType(eNewType);
            eNewType = CHANNEL_TYPE_GOOSE_POINT;
        }

        // 检查返回值
        if (!bRet)
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    DA(s)
///
/// @return
///     bool    成功返回true，失败返回false
///
bool CGooseCb::DA_SinglePoint()
{
    CChannel*                       pChannel   = NULL;


    // 遍历控制块的所有通道
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        // 将通道类型设置为单点类型
        if (!pChannel->SetChannelType(CHANNEL_TYPE_GOOSE_POINT))
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    DA(双点)
///
/// @return
///     bool    成功返回true，失败返回false
///
bool CGooseCb::DA_DoublePoint()
{
    CChannel*                       pChannel   = NULL;


    // 遍历控制块的所有通道
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        // 将通道类型设置为双点类型
        if (!pChannel->SetChannelType(CHANNEL_TYPE_GOOSE_DOUBLE_POINT))
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    获得控制块所有通道值在通信报文中的长度
///
/// @return
///     int    通道报文长度
///
int CGooseCb::GetChannelValueLen()
{
    int                             iTotalLen   = 0;
    CChannel*                       pChannel    = NULL;


    // 遍历控制块的所有通道
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        // 累加每一个通道的长度
        iTotalLen += pChannel->GetValueLength();
    }

    return iTotalLen;
}

///
/// @brief
///    计算控制块中所有通道的偏移量
///
/// @param[in]  pGooseMsg    GOOSE报文的编解码接口
///
/// @return
///     bool    成功返回true
///
bool CGooseCb::CalcChannelOffset( GooseMsg* pGooseMsg )
{
    int                             i           = 0;
    CChannel*                       pChannel    = NULL;
    GOOSE_DATA*                     pGooseData  = NULL;


    // 检查输入参数
    if (NULL == pGooseMsg)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CGooseCb::__CalcChannelOffset(): Parameter error \n"));
        return false;
    }

    // 遍历控制块的所有通道
    for (i = 0, pChannel = m_pChannelHead; pChannel != NULL; i++, pChannel = pChannel->m_pNext)
    {
        // 获得通道偏移量
        pGooseData = pGooseMsg->GetAChannel(i);
        if (NULL == pGooseData)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CGooseCb::__CalcChannelOffset(): GetAChannel failed \n"));
            return false;
        }

        // 计算每一个通道在报文帧中的偏移
        if (!pChannel->__CalcChannelOffset(pGooseData))
        {
            return false;
        }
    }

    // 获得GoosePdu的公共信息部分在报文帧中的偏移地址信息
    pGooseMsg->GetPduPublicInfoOff(m_stPduPblcOff);

    return true;
}

///
/// @brief
///    加载<Gocb>控制块
///
/// @param[in]  pXml 指向XML对象的引用
///
/// @return
///     bool    true表示加载成功
///
bool CGooseCb::__LoadConfig( CMarkup& xml )
{
    // <Description>
    if (!xml.FindElem(NODE_Gocb_Description))
    {
        return false;
    }
    m_strDescription = xml.GetData();

    // <PortName>
    if (!xml.FindElem(NODE_Gocb_PortName))
    {
        return false;
    }
    m_ePortName = (DsmPortName)_wtoi(xml.GetData().c_str());

    // <APPID>
    if (!xml.FindElem(NODE_Gocb_APPID))
    {
        return false;
    }
    m_iAPPID = _wtoi(xml.GetData().c_str());

    // <SrcMAC>
    unsigned char cBuffer[ETHERNET_MAC_LEN] = {'0'};
    if (!xml.FindElem(NODE_Gocb_SRC_MAC))
    {
        return false;
    }
    ConvMacWStr2Char(xml.GetData(), cBuffer, ETHERNET_MAC_LEN);
    memcpy_s(m_ucSrcMAC, ETHERNET_MAC_LEN, cBuffer, ETHERNET_MAC_LEN);

    // <DesMAC>
    if (!xml.FindElem(NODE_Gocb_DES_MAC))
    {
        return false;
    }
    ConvMacWStr2Char(xml.GetData(), cBuffer, ETHERNET_MAC_LEN);
    memcpy_s(m_ucDesMAC, ETHERNET_MAC_LEN, cBuffer, ETHERNET_MAC_LEN);

#if !CHUN_HUA
	// <VLanTag>
	if (!xml.FindElem(NODE_Gocb_VLanTag))
	{
		return false;
	}
	m_bHasVlanFlag = _wtoi(xml.GetData().c_str());
#endif

    // <VLanID>
    if (!xml.FindElem(NODE_Gocb_VLanID))
    {
        return false;
    }
    m_iVLanID = _wtoi(xml.GetData().c_str());

    // <VLanPriority>
    if (!xml.FindElem(NODE_Gocb_VLanPriority))
    {
        return false;
    }
    m_iVLanPriority = _wtoi(xml.GetData().c_str());

    // <ConfigRevision>
    if (!xml.FindElem(NODE_Gocb_ConfigRevision))
    {
        return false;
    }
    m_iConfigRevision = _wtoi(xml.GetData().c_str());

    // <TestFlag>
    if (!xml.FindElem(NODE_Gocb_TestFlag))
    {
        return false;
    }
    m_bTestFlag = (_wtoi(xml.GetData().c_str())? true: false);

    // <NeedComm>
    if (!xml.FindElem(NODE_Gocb_NeedComm))
    {
        return false;
    }
    m_bNeedComm = (_wtoi(xml.GetData().c_str())? true: false);

    // <GocbRef>
    if (!xml.FindElem(NODE_Gocb_GocbRef))
    {
        return false;
    }
    m_strGocbRef = xml.GetData();

    // <DataSet>
    if (!xml.FindElem(NODE_Gocb_DataSet))
    {
        return false;
    }
    m_strDataSet = xml.GetData();

    // <GoID>
    if (!xml.FindElem(NODE_Gocb_GoID))
    {
        return false;
    }
    m_strGoID = xml.GetData();

    // <ChannelGroup>
    if (xml.FindElem(NODE_Gocb_ChannelGroup) && xml.IntoElem())
    {
        //
        // find <Channel>
        //
        while(xml.FindElem(NODE_Channel))
        {
            CChannel*   pChannel = new CChannel(this, NULL, m_pParamConfigBase);
            if (NULL == pChannel)
            {
                return false;
            }

            // 设置通道选中状态
            int iSelected = _wtoi(xml.GetAttrib(L"selected").c_str());
            if (iSelected != 0)
            {
                pChannel->m_bSelected   = true;
            }

            // inside <Channel>
            if (!xml.IntoElem())
            {
                delete pChannel;
                return false;
            }

            // 加载通道
            if (!pChannel->__LoadGooseConfig(xml))
            {
                delete pChannel;
                return false;
            }

            // outside <Channel>
            if (!xml.OutOfElem())
            {
                delete pChannel;
                return false;
            }

            // 添加GOOSE控制块对象指针到容器
            if (!__AddChannel(pChannel))
            {
                delete pChannel;
                return false;
            }
        }

        // quit <ChannelGroup>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    保存<Gocb>控制块
///
/// @param[in]  pXml 指向XML对象的引用
///
/// @return
///     bool    true表示保存成功
///
bool CGooseCb::__SaveConfig( CMarkup& xml )
{
    // <Description>
    if (!xml.AddElem(NODE_Gocb_Description) || !xml.SetData(GetDescription()))
    {
        return false;
    }

    // <PortName>
    if (!xml.AddElem(NODE_Gocb_PortName) || !xml.SetData((int)GetPortName()))
    {
        return false;
    }

    // <APPID>
    if (!xml.AddElem(NODE_Gocb_APPID) || !xml.SetData(GetAPPID()))
    {
        return false;
    }

    // <SrcMAC>
    unsigned char cBuffer[ETHERNET_MAC_LEN] = {'0'};
    GetSrcMAC(cBuffer, ETHERNET_MAC_LEN);
    if (!xml.AddElem(NODE_Gocb_SRC_MAC) || !xml.SetData(ConvMacChar2WStr(cBuffer, ETHERNET_MAC_LEN)))
    {
        return false;
    }

    // <DesMAC>
    GetDesMAC(cBuffer, ETHERNET_MAC_LEN);
    if (!xml.AddElem(NODE_Gocb_DES_MAC) || !xml.SetData(ConvMacChar2WStr(cBuffer, ETHERNET_MAC_LEN)))
    {
        return false;
    }

#if !CHUN_HUA
	// <VLanTag>
	if (!xml.AddElem(NODE_Gocb_VLanTag) || !xml.SetData(GetVlanFlag()))
	{
		return false;
	}
#endif

    // <VLanID>
    if (!xml.AddElem(NODE_Gocb_VLanID) || !xml.SetData(GetVlanID()))
    {
        return false;
    }

    // <VLanPriority>
    if (!xml.AddElem(NODE_Gocb_VLanPriority) || !xml.SetData(GetVLanPriority()))
    {
        return false;
    }

    // <ConfigRevision>
    if (!xml.AddElem(NODE_Gocb_ConfigRevision) || !xml.SetData(GetConfigRevision()))
    {
        return false;
    }

    // <TestFlag>
    if (!xml.AddElem(NODE_Gocb_TestFlag) || !xml.SetData(GetTestFlag()))
    {
        return false;
    }

    // <NeedComm>
    if (!xml.AddElem(NODE_Gocb_NeedComm) || !xml.SetData(GetNeedComm()))
    {
        return false;
    }

    // <GocbRef>
    if (!xml.AddElem(NODE_Gocb_GocbRef) || !xml.SetData(GetGocbRef()))
    {
        return false;
    }

    // <DataSet>
    if (!xml.AddElem(NODE_Gocb_DataSet) || !xml.SetData(GetDataSet()))
    {
        return false;
    }

    // <GoID>
    if (!xml.AddElem(NODE_Gocb_GoID) || !xml.SetData(GetGoID()))
    {
        return false;
    }

    //
    // 遍历容器，保存所有Channel
    //
    if (GetChannelCount() > 0)
    {
        // <ChannelGroup>
        if (!xml.AddElem(NODE_Gocb_ChannelGroup) || !xml.IntoElem())
        {
            return false;
        }

        // 单个通道
        CChannel*   pChannel    = FirstChannel();
        while(pChannel)
        {
            // <Channel>
            if (!xml.AddElem(NODE_Channel))
            {
                return false;
            }

            // <Channel selected="">
            int iSelected = pChannel->IsSelected()? 1: 0;
            if (!xml.SetAttrib(L"selected", iSelected))
            {
                return false;
            }

            // inside <Channel>
            if (!xml.IntoElem())
            {
                return false;
            }

            // 保存Channel
            if (!pChannel->__SaveGooseConfig(xml))
            {
                return false;
            }

            // outside <Channel>
            if (!xml.OutOfElem())
            {
                return false;
            }

            pChannel = NextChannel(pChannel);
        }

        // Quit <ChannelGroup>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    修改GOOSE控制块中单点/双点类型通道时，需要自动修改相邻/间隔的时间通道的值
///    此函数自动寻找寻找变位的单点/双点通道，然后改变相邻或间隔的时间通道的值
///
/// @param[in]  stNewTime    单点/双点通道的变位时间
///
void CGooseCb::__UpdateChannelTime( CEventTime stNewTime )
{
    CChannel*                       pSrcChannel     = NULL;
    CChannel*                       pChannel        = NULL;
    CChannel*                       pNextChannel    = NULL;
    CChannel*                       pThirdChannel   = NULL;


    // 遍历控制块的所有通道
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        // 查找变位的通道
        if (pChannel->IsSendValueChanged())
        {
            // 如果变位的是单点、双点类型的通道
            if (CHANNEL_TYPE_GOOSE_POINT == pChannel->GetChannelType() ||
                CHANNEL_TYPE_GOOSE_DOUBLE_POINT == pChannel->GetChannelType())
            {
                // 如果相邻的通道是时间类型
                pNextChannel = NextChannel(pChannel);
                if (pNextChannel && CHANNEL_TYPE_GOOSE_TIME == pNextChannel->GetChannelType())
                {
                    // 更新相邻的时间通道的值
                    GOOSE_DATA_VARIANT& stChannelValue = pNextChannel->GetGooseSendValue();
                    stChannelValue.m_stUtcTime.t_s  = stNewTime.ts_s;
                    stChannelValue.m_stUtcTime.t_ns = stNewTime.ts_ns;

                    // 更新源时间通道的值
                    pSrcChannel = pNextChannel->m_pSourceChannel;
                    if (pSrcChannel)
                    {
                        GOOSE_DATA_VARIANT& stSrcChannelValue = pSrcChannel->GetGooseSendValue();
                        stSrcChannelValue.m_stUtcTime.t_s  = stNewTime.ts_s;
                        stSrcChannelValue.m_stUtcTime.t_ns = stNewTime.ts_ns;
                    }
                }
                // 如果相邻的通道是品质类型
                else if (pNextChannel && CHANNEL_TYPE_GOOSE_QUALITY == pNextChannel->GetChannelType())
                {
                    // 判断间隔的通道是否是时间类型
                    pThirdChannel = NextChannel(pNextChannel);
                    if (pThirdChannel && CHANNEL_TYPE_GOOSE_TIME == pThirdChannel->GetChannelType())
                    {
                        // 更新间隔的时间通道的值
                        GOOSE_DATA_VARIANT& stChannelValue = pThirdChannel->GetGooseSendValue();
                        stChannelValue.m_stUtcTime.t_s  = stNewTime.ts_s;
                        stChannelValue.m_stUtcTime.t_ns = stNewTime.ts_ns;

                        // 更新源时间通道的值
                        pSrcChannel = pThirdChannel->m_pSourceChannel;
                        if (pSrcChannel)
                        {
                            GOOSE_DATA_VARIANT& stSrcChannelValue = pSrcChannel->GetGooseSendValue();
                            stSrcChannelValue.m_stUtcTime.t_s  = stNewTime.ts_s;
                            stSrcChannelValue.m_stUtcTime.t_ns = stNewTime.ts_ns;
                        }
                    }
                }
            }
            // 如果变位的是结构类型的通道
            else if (CHANNEL_TYPE_GOOSE_STRUCT == pChannel->GetChannelType())
            {
                // 更新子通道的时间
                pChannel->__UpdateSubChannelTime(stNewTime);
            }
        }
    }
}

///
/// @brief
///     对发生了变位的控制块通道进行重新编码
///
/// @param[in]  pucFrameHead    指向发送帧帧头的指针
///
/// @return
///     bool    成功返回true
///
bool CGooseCb::__RecodeChangedChannels( unsigned char* pucFrameHead )
{
    CChannel*                       pChannel    = NULL;


    // 遍历控制块的所有通道
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        // 查找变位的通道
        if (pChannel->IsSendValueChanged())
        {
            // 如果是单点类型通道
            if (CHANNEL_TYPE_GOOSE_POINT == pChannel->GetChannelType())
            {
                // 获得通道发送值
                GOOSE_DATA_VARIANT& stSendValue = pChannel->GetGooseSendValue();
                if (GSEDT_BOOLEAN != stSendValue.m_stUnKnown.id.type)
                {
                    return false;
                }

                // 修改缓冲区中单点通道的报文值
                stSendValue.CodeGooseData(pucFrameHead + stSendValue.m_stUtcTime.id.offset, 3);
            }
            // 如果是双点类型通道
            else if (CHANNEL_TYPE_GOOSE_DOUBLE_POINT == pChannel->GetChannelType())
            {
                // 获得通道发送值
                GOOSE_DATA_VARIANT& stSendValue = pChannel->GetGooseSendValue();
                if (GSEDT_BIT_STR != stSendValue.m_stUnKnown.id.type)
                {
                    return false;
                }

                // 修改缓冲区中双点通道的报文值
                stSendValue.CodeGooseData(pucFrameHead + stSendValue.m_stUtcTime.id.offset, 4);
            }
            // 如果是结构体类型通道
            else if (CHANNEL_TYPE_GOOSE_STRUCT == pChannel->GetChannelType())
            {
                // 对发生了变位的子通道进行重新编码
                if (!pChannel->__RecodeChangedSubChannels(pucFrameHead))
                {
                    return false;
                }
            }

            // 复位变位标识
            pChannel->SetSendValueChanged(false);
        }
    }

    return true;
}

///
/// @brief
///     查找GOOSE控制块中所有需要变位的时间通道，将通道指针保存到容器中
///
/// @param[out]  listShouldChangeTimeChannel    保存需要变位时间通道的指针
///
void CGooseCb::__SaveTimeChannel( std::list<CChannel*>& listShouldChangeTimeChannel )
{
    CChannel*                       pChannel        = NULL;
    CChannel*                       pNextChannel    = NULL;
    CChannel*                       pThirdChannel   = NULL;


    // 清空容器
    listShouldChangeTimeChannel.clear();

    // 遍历控制块的所有通道
    for (pChannel = m_pChannelHead; pChannel != NULL; pChannel = pChannel->m_pNext)
    {
        // 查找变位的通道
        if (pChannel->IsSendValueChanged())
        {
            // 如果变位的是单点、双点类型的通道
            if (CHANNEL_TYPE_GOOSE_POINT == pChannel->GetChannelType() ||
                CHANNEL_TYPE_GOOSE_DOUBLE_POINT == pChannel->GetChannelType())
            {
                // 如果相邻的通道是时间类型
                pNextChannel = NextChannel(pChannel);
                if (pNextChannel && CHANNEL_TYPE_GOOSE_TIME == pNextChannel->GetChannelType())
                {
                    // 记录需要变位的时间通道
                    listShouldChangeTimeChannel.push_back(pNextChannel);
                }
                // 如果相邻的通道是品质类型
                else if (pNextChannel && CHANNEL_TYPE_GOOSE_QUALITY == pNextChannel->GetChannelType())
                {
                    // 判断间隔的通道是否是时间类型
                    pThirdChannel = NextChannel(pNextChannel);
                    if (pThirdChannel && CHANNEL_TYPE_GOOSE_TIME == pThirdChannel->GetChannelType())
                    {
                        // 记录需要变位的时间通道
                        listShouldChangeTimeChannel.push_back(pThirdChannel);
                    }
                }
            }
            // 如果变位的是结构类型的通道
            else if (CHANNEL_TYPE_GOOSE_STRUCT == pChannel->GetChannelType())
            {
                // 查找所有需要变位的时间子通道
                pChannel->__SaveSubTimeChannel(listShouldChangeTimeChannel);
            }
        }
    }
}
