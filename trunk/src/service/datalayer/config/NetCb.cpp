///
/// @brief
///     ������ƿ����
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.29
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "NetCb.h"

// Ԥ�����Ŀ��MAC��ַ
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
///     ���캯��
///
CNetCb::CNetCb(void)
{
}

///
/// @brief
///     ���캯��
///
/// @param[in]  
///     pParamConfigBase    ָ��������ù�������ָ��
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
    // ԴMAC��ַ
    memset(m_ucSrcMAC, 0xFF, sizeof(m_ucSrcMAC)/ sizeof(m_ucSrcMAC[0]));
    m_ucSrcMAC[0] = 0x00;

    // Ŀ��MAC��ַ
    memcpy_s(m_ucDesMAC, ETHERNET_MAC_LEN, PREDEFINED_NET_DES_MAC, ETHERNET_MAC_LEN);
}

///
/// @brief
///     ��������
///
CNetCb::~CNetCb(void)
{
}

///
/// @brief
///    ����ԴMAC��ַ
///
/// @param[in]  pucMAC  ����MAC��ַ������
/// @param[in]  iLen  �����С
///
void CNetCb::GetSrcMAC(unsigned char* pucMAC, int iLen)
{
    memcpy_s(pucMAC, iLen, m_ucSrcMAC, ETHERNET_MAC_LEN);
}

///
/// @brief
///    ����ԴMAC��ַ
///
/// @param[in]  pMAC  MAC��ַ����
/// @param[in]  iLen  ����Ԫ�ظ���
///
void CNetCb::SetSrcMAC(unsigned char* pMAC, int iLen)
{
    if (0 != memcmp(m_ucSrcMAC, pMAC, iLen))
    {
        memcpy_s(m_ucSrcMAC, ETHERNET_MAC_LEN, pMAC, iLen);

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ����Ŀ��MAC��ַ
///
/// @param[in]  pucMAC  ����MAC��ַ������
/// @param[in]  iLen  �����С
///
void CNetCb::GetDesMAC(unsigned char* pucMAC, int iLen)
{
    memcpy_s(pucMAC, iLen, m_ucDesMAC, ETHERNET_MAC_LEN);
}

///
/// @brief
///    ����Ŀ��MAC��ַ
///
/// @param[in]  pMAC  MAC��ַ����
/// @param[in]  iLen  ����Ԫ�ظ���
///
void CNetCb::SetDesMAC(unsigned char* pMAC, int iLen)
{
    if (0 != memcmp(m_ucDesMAC, pMAC, iLen))
    {
        memcpy_s(m_ucDesMAC, ETHERNET_MAC_LEN, pMAC, iLen);

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ���VLAN ID
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
///    ����VLAN ID
///
/// @param[in]  iNewVlanID  �µ�VLAN ID
///
/// @return
///     bool    true��ʾ����ɹ�
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

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }

    return true;
}

///
/// @brief
///    ���VLAN���ȼ�
///
/// @return
///     int    VLAN���ȼ�
///
int CNetCb::GetVLanPriority()
{
    return m_iVLanPriority;
}

///
/// @brief
///    ����VLAN���ȼ�
///
/// @param[in]  iNewVLanPriority  �µ�VLAN���ȼ�
///
/// @return
///     bool    true��ʾ����ɹ�
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

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }

    return true;
}

///
/// @brief
///    ������ð汾��
///
/// @return
///     int    ���ð汾��
///
int CNetCb::GetConfigRevision()
{
    return m_iConfigRevision;
}

///
/// @brief
///    �������ð汾��
///
/// @param[in]  iNewConfigRevision  �µ����ð汾��
///
void CNetCb::SetConfigRevision(int iNewConfigRevision)
{
    // ����������
    if (iNewConfigRevision < 0)
    {
        return ;
    }

    if (m_iConfigRevision != iNewConfigRevision)
    {
        m_iConfigRevision = iNewConfigRevision;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ��ȿ���һ��CNetCb���ƿ�
///
/// @param[in]
///     pNetCb    -   CNetCb���ƿ�
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CNetCb::__Copy( CNetCb* pNetCb )
{
    // ����������
    if (NULL == pNetCb)
    {
        return false;
    }

    // �������������
    if (!CBaseCb::__Copy(pNetCb))
    {
        return false;
    }

    // ����MAC��ַ
    memcpy_s(m_ucSrcMAC, ETHERNET_MAC_LEN, pNetCb->m_ucSrcMAC, ETHERNET_MAC_LEN);
    memcpy_s(m_ucDesMAC, ETHERNET_MAC_LEN, pNetCb->m_ucDesMAC, ETHERNET_MAC_LEN);

    // ����CNetCb������

 

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
///    ��ȿ���һ��ɨ���б��е�GOOSE���ƿ�
///
/// @param[in]
///     pGooseMsg    -   GOOSE���ĵı�������
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CNetCb::__Copy(GooseMsg* pGooseMsg)
{
    // ����������
    if (NULL == pGooseMsg)
    {
        return false;
    }

    // �������������
    if (!CBaseCb::__Copy(pGooseMsg))
    {
        return false;
    }

    // MAC��ַ
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
///    ��ȿ���һ��ɨ���б��е�IEC61850-9-1���ƿ�
///
/// @param[in]
///     Sav91Msg    -   IEC61850-9-1���ĵı�������
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CNetCb::__Copy(Sav91Msg* pSav91Msg)
{
    // ����������
    if (NULL == pSav91Msg)
    {
        return false;
    }

    // �������������
    if (!CBaseCb::__Copy(pSav91Msg))
    {
        return false;
    }

    // MAC��ַ
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
///    ��ȿ���һ��ɨ���б��е�IEC61850-9-2���ƿ�
///
/// @param[in]
///     pSav92Msg    -   IEC61850-9-2���ĵı�������
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CNetCb::__Copy(Sav92Msg* pSav92Msg)
{
    // ����������
    if (NULL == pSav92Msg)
    {
        return false;
    }

    // �������������
    if (!CBaseCb::__Copy(pSav92Msg))
    {
        return false;
    }

    // MAC��ַ
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

		// ���Ϊ���޸�
		if (NULL != m_pParamConfigBase)
		{
			m_pParamConfigBase->SetModified(true);
		}
	}
}
#endif