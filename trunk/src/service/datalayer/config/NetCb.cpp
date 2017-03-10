///
/// @brief
///     网络控制块基类
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.29
///
/// 修改历史：
///

#include "StdAfx.h"
#include "NetCb.h"

// 预定义的目标MAC地址
static const unsigned char PREDEFINED_NET_DES_MAC[ETHERNET_MAC_LEN] = 
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
///     构造函数
///
CNetCb::CNetCb(void)
{
}

///
/// @brief
///     构造函数
///
/// @param[in]  
///     pParamConfigBase    指向参数配置管理器的指针
///
CNetCb::CNetCb(CParamConfigBase* pParamConfigBase)
:CBaseCb(pParamConfigBase)
,m_iVLanID(0)
,m_iVLanPriority(4)
,m_iConfigRevision(0)

#if !CHUN_HUA
,m_bHasVlanFlag(true)
#endif

{
    // 源MAC地址
    memset(m_ucSrcMAC, 0xFF, sizeof(m_ucSrcMAC)/ sizeof(m_ucSrcMAC[0]));
    m_ucSrcMAC[0] = 0x00;

    // 目标MAC地址
    memcpy_s(m_ucDesMAC, ETHERNET_MAC_LEN, PREDEFINED_NET_DES_MAC, ETHERNET_MAC_LEN);
}

///
/// @brief
///     析构函数
///
CNetCb::~CNetCb(void)
{
}

///
/// @brief
///    设置源MAC地址
///
/// @param[in]  pucMAC  保存MAC地址的数组
/// @param[in]  iLen  数组大小
///
void CNetCb::GetSrcMAC(unsigned char* pucMAC, int iLen)
{
    memcpy_s(pucMAC, iLen, m_ucSrcMAC, ETHERNET_MAC_LEN);
}

///
/// @brief
///    设置源MAC地址
///
/// @param[in]  pMAC  MAC地址数组
/// @param[in]  iLen  数组元素个数
///
void CNetCb::SetSrcMAC(unsigned char* pMAC, int iLen)
{
    if (0 != memcmp(m_ucSrcMAC, pMAC, iLen))
    {
        memcpy_s(m_ucSrcMAC, ETHERNET_MAC_LEN, pMAC, iLen);

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    设置目标MAC地址
///
/// @param[in]  pucMAC  保存MAC地址的数组
/// @param[in]  iLen  数组大小
///
void CNetCb::GetDesMAC(unsigned char* pucMAC, int iLen)
{
    memcpy_s(pucMAC, iLen, m_ucDesMAC, ETHERNET_MAC_LEN);
}

///
/// @brief
///    设置目标MAC地址
///
/// @param[in]  pMAC  MAC地址数组
/// @param[in]  iLen  数组元素个数
///
void CNetCb::SetDesMAC(unsigned char* pMAC, int iLen)
{
    if (0 != memcmp(m_ucDesMAC, pMAC, iLen))
    {
        memcpy_s(m_ucDesMAC, ETHERNET_MAC_LEN, pMAC, iLen);

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得VLAN ID
///
/// @return
///     int    VLAN ID
///
int CNetCb::GetVlanID()
{
    return m_iVLanID;
}

///
/// @brief
///    设置VLAN ID
///
/// @param[in]  iNewVlanID  新的VLAN ID
///
/// @return
///     bool    true表示保存成功
///
bool CNetCb::SetVlanID(int iNewVlanID)
{
    if ((iNewVlanID < 0) || (iNewVlanID > 4095))
    {
        return false;
    }

    if (m_iVLanID != iNewVlanID)
    {
        m_iVLanID = iNewVlanID;

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
///    获得VLAN优先级
///
/// @return
///     int    VLAN优先级
///
int CNetCb::GetVLanPriority()
{
    return m_iVLanPriority;
}

///
/// @brief
///    设置VLAN优先级
///
/// @param[in]  iNewVLanPriority  新的VLAN优先级
///
/// @return
///     bool    true表示保存成功
///
bool CNetCb::SetVLanPriority(int iNewVLanPriority)
{
    if ((iNewVLanPriority < 0) || (iNewVLanPriority > 7))
    {
        return false;
    }

    if (m_iVLanPriority != iNewVLanPriority)
    {
        m_iVLanPriority = iNewVLanPriority;

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
///    获得配置版本号
///
/// @return
///     int    配置版本号
///
int CNetCb::GetConfigRevision()
{
    return m_iConfigRevision;
}

///
/// @brief
///    设置配置版本号
///
/// @param[in]  iNewConfigRevision  新的配置版本号
///
void CNetCb::SetConfigRevision(int iNewConfigRevision)
{
    // 检查输入参数
    if (iNewConfigRevision < 0)
    {
        return ;
    }

    if (m_iConfigRevision != iNewConfigRevision)
    {
        m_iConfigRevision = iNewConfigRevision;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    深度拷贝一个CNetCb控制块
///
/// @param[in]
///     pNetCb    -   CNetCb控制块
///
/// @return
///     bool    拷贝成功返回TRUE
///
bool CNetCb::__Copy( CNetCb* pNetCb )
{
    // 检查输入参数
    if (NULL == pNetCb)
    {
        return false;
    }

    // 拷贝基类的属性
    if (!CBaseCb::__Copy(pNetCb))
    {
        return false;
    }

    // 拷贝MAC地址
    memcpy_s(m_ucSrcMAC, ETHERNET_MAC_LEN, pNetCb->m_ucSrcMAC, ETHERNET_MAC_LEN);
    memcpy_s(m_ucDesMAC, ETHERNET_MAC_LEN, pNetCb->m_ucDesMAC, ETHERNET_MAC_LEN);

    // 拷贝CNetCb的属性

 

    m_iVLanID           = pNetCb->m_iVLanID;
    m_iVLanPriority     = pNetCb->m_iVLanPriority;
    m_iConfigRevision   = pNetCb->m_iConfigRevision;

#if !CHUN_HUA
	m_bHasVlanFlag		= pNetCb->m_bHasVlanFlag;
#endif

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
bool CNetCb::__Copy(GooseMsg* pGooseMsg)
{
    // 输入参数检查
    if (NULL == pGooseMsg)
    {
        return false;
    }

    // 拷贝基类的属性
    if (!CBaseCb::__Copy(pGooseMsg))
    {
        return false;
    }

    // MAC地址
    ETHER_II_HEADER         stEthHeader;
    pGooseMsg->GetEthernetHeader(stEthHeader);
    memcpy_s(m_ucDesMAC, ETHERNET_MAC_LEN, stEthHeader.desMac , ETHER_MAC_LEN);

    // VLAN ID
    IEEE802_1Q_VLAN_INFO    stVlanInfo;
    pGooseMsg->Get8021QVlanInfo(stVlanInfo);
    m_iVLanID       = stVlanInfo.tci.vlanID;
    m_iVLanPriority = stVlanInfo.tci.userPriority;

 

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
bool CNetCb::__Copy(Sav91Msg* pSav91Msg)
{
    // 输入参数检查
    if (NULL == pSav91Msg)
    {
        return false;
    }

    // 拷贝基类的属性
    if (!CBaseCb::__Copy(pSav91Msg))
    {
        return false;
    }

    // MAC地址
    ETHER_II_HEADER         stEthHeader;
    pSav91Msg->GetEthernetHeader(stEthHeader);
    memcpy_s(m_ucDesMAC, ETHERNET_MAC_LEN, stEthHeader.desMac, ETHER_MAC_LEN);

    // VLAN ID
    IEEE802_1Q_VLAN_INFO    stVlanInfo;
    pSav91Msg->Get8021QVlanInfo(stVlanInfo);
    m_iVLanID       = stVlanInfo.tci.vlanID;
    m_iVLanPriority = stVlanInfo.tci.userPriority;

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
bool CNetCb::__Copy(Sav92Msg* pSav92Msg)
{
    // 输入参数检查
    if (NULL == pSav92Msg)
    {
        return false;
    }

    // 拷贝基类的属性
    if (!CBaseCb::__Copy(pSav92Msg))
    {
        return false;
    }

    // MAC地址
    ETHER_II_HEADER         stEthHeader;
    pSav92Msg->GetEthernetHeader(stEthHeader);
    memcpy_s(m_ucDesMAC, ETHERNET_MAC_LEN, stEthHeader.desMac , ETHER_MAC_LEN);

    // VLAN ID
    IEEE802_1Q_VLAN_INFO    stVlanInfo;
    pSav92Msg->Get8021QVlanInfo(stVlanInfo);
    m_iVLanID       = stVlanInfo.tci.vlanID;
    m_iVLanPriority = stVlanInfo.tci.userPriority;

    return true;
}
#if !CHUN_HUA
bool CNetCb::GetVlanFlag()
{
	return m_bHasVlanFlag;
}

void CNetCb::SetVlanFlag(bool bHashVlan)
{
	if (m_bHasVlanFlag != bHashVlan)
	{
		m_bHasVlanFlag = bHashVlan;

		// 标记为已修改
		if (NULL != m_pParamConfigBase)
		{
			m_pParamConfigBase->SetModified(true);
		}
	}
}
#endif