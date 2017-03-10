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

#ifndef __NET_CB_H__
#define __NET_CB_H__
#include "basecb.h"
#include <src/service/datalayer/include/config.h>

class CNetCb : public CBaseCb
{
protected:
    CNetCb(void);
    virtual ~CNetCb(void);

public:
    ///
    /// @brief
    ///     ���캯��
    ///
    /// @param[in]  
    ///     pParamConfigBase    ָ��������ù�������ָ��
    ///
    CNetCb(CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    ����ԴMAC��ַ
    ///
    /// @param[in]  pucMAC  ����MAC��ַ������
    /// @param[in]  iLen  �����С
    ///
    void GetSrcMAC(unsigned char* pucMAC, int iLen);

    ///
    /// @brief
    ///    ����ԴMAC��ַ
    ///
    /// @param[in]  pMAC  MAC��ַ����
    /// @param[in]  iLen  ����Ԫ�ظ���
    ///
    void SetSrcMAC(unsigned char* pMAC, int iLen);

    ///
    /// @brief
    ///    ����Ŀ��MAC��ַ
    ///
    /// @param[in]  pucMAC  ����MAC��ַ������
    /// @param[in]  iLen  �����С
    ///
    void GetDesMAC(unsigned char* pucMAC, int iLen);

    ///
    /// @brief
    ///    ����Ŀ��MAC��ַ
    ///
    /// @param[in]  pMAC  MAC��ַ����
    /// @param[in]  iLen  ����Ԫ�ظ���
    ///
    void SetDesMAC(unsigned char* pMAC, int iLen);

#if !CHUN_HUA
	///
	/// @brief
	///    ���VlanFlag
	///
	/// @return
	///     bool    VlanFlag
	///
	bool GetVlanFlag();
	///
	/// @brief
	///    ����VlanFlag
	///
	/// @return
	///     bool    VlanFlag
	///
	void SetVlanFlag(bool bHashVlan);

#endif
    ///
    /// @brief
    ///    ���VLAN ID
    ///
    /// @return
    ///     int    VLAN ID
    ///
    int GetVlanID();

    ///
    /// @brief
    ///    ����VLAN ID
    ///
    /// @param[in]  iNewVlanID  �µ�VLAN ID
    ///
    /// @return
    ///     bool    true��ʾ����ɹ�
    ///
    bool SetVlanID(int iNewVlanID);

    ///
    /// @brief
    ///    ���VLAN���ȼ�
    ///
    /// @return
    ///     int    VLAN���ȼ�
    ///
    int GetVLanPriority();

    ///
    /// @brief
    ///    ����VLAN���ȼ�
    ///
    /// @param[in]  iNewVLanPriority  �µ�VLAN���ȼ�
    ///
    /// @return
    ///     bool    true��ʾ����ɹ�
    ///
    bool SetVLanPriority(int iNewVLanPriority);

    ///
    /// @brief
    ///    ������ð汾��
    ///
    /// @return
    ///     int    ���ð汾��
    ///
    int GetConfigRevision();

    ///
    /// @brief
    ///    �������ð汾��
    ///
    /// @param[in]  iNewConfigRevision  �µ����ð汾��
    ///
    void SetConfigRevision(int iNewConfigRevision);

protected:
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
    virtual bool __Copy(CNetCb* pNetCb);

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
    virtual bool __Copy(GooseMsg* pGooseMsg);

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
    virtual bool __Copy(Sav91Msg* pSav91Msg);

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
    virtual bool __Copy(Sav92Msg* pSav92Msg);

protected:
    unsigned char                       m_ucSrcMAC[ETHERNET_MAC_LEN];   ///< Դmac��ַ
    unsigned char                       m_ucDesMAC[ETHERNET_MAC_LEN];   ///< Ŀ��mac��ַ
    int                                 m_iVLanID;                      ///< VLAN ID
    int                                 m_iVLanPriority;                ///< VLAN���ȼ�
    int                                 m_iConfigRevision;              ///< ���ð汾��

#if !CHUN_HUA
	bool                                m_bHasVlanFlag;                    ///< VlanFlag
#endif

};
#endif
