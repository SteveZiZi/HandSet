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
    ///     构造函数
    ///
    /// @param[in]  
    ///     pParamConfigBase    指向参数配置管理器的指针
    ///
    CNetCb(CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    设置源MAC地址
    ///
    /// @param[in]  pucMAC  保存MAC地址的数组
    /// @param[in]  iLen  数组大小
    ///
    void GetSrcMAC(unsigned char* pucMAC, int iLen);

    ///
    /// @brief
    ///    设置源MAC地址
    ///
    /// @param[in]  pMAC  MAC地址数组
    /// @param[in]  iLen  数组元素个数
    ///
    void SetSrcMAC(unsigned char* pMAC, int iLen);

    ///
    /// @brief
    ///    设置目标MAC地址
    ///
    /// @param[in]  pucMAC  保存MAC地址的数组
    /// @param[in]  iLen  数组大小
    ///
    void GetDesMAC(unsigned char* pucMAC, int iLen);

    ///
    /// @brief
    ///    设置目标MAC地址
    ///
    /// @param[in]  pMAC  MAC地址数组
    /// @param[in]  iLen  数组元素个数
    ///
    void SetDesMAC(unsigned char* pMAC, int iLen);

#if !CHUN_HUA
	///
	/// @brief
	///    获得VlanFlag
	///
	/// @return
	///     bool    VlanFlag
	///
	bool GetVlanFlag();
	///
	/// @brief
	///    设置VlanFlag
	///
	/// @return
	///     bool    VlanFlag
	///
	void SetVlanFlag(bool bHashVlan);

#endif
    ///
    /// @brief
    ///    获得VLAN ID
    ///
    /// @return
    ///     int    VLAN ID
    ///
    int GetVlanID();

    ///
    /// @brief
    ///    设置VLAN ID
    ///
    /// @param[in]  iNewVlanID  新的VLAN ID
    ///
    /// @return
    ///     bool    true表示保存成功
    ///
    bool SetVlanID(int iNewVlanID);

    ///
    /// @brief
    ///    获得VLAN优先级
    ///
    /// @return
    ///     int    VLAN优先级
    ///
    int GetVLanPriority();

    ///
    /// @brief
    ///    设置VLAN优先级
    ///
    /// @param[in]  iNewVLanPriority  新的VLAN优先级
    ///
    /// @return
    ///     bool    true表示保存成功
    ///
    bool SetVLanPriority(int iNewVLanPriority);

    ///
    /// @brief
    ///    获得配置版本号
    ///
    /// @return
    ///     int    配置版本号
    ///
    int GetConfigRevision();

    ///
    /// @brief
    ///    设置配置版本号
    ///
    /// @param[in]  iNewConfigRevision  新的配置版本号
    ///
    void SetConfigRevision(int iNewConfigRevision);

protected:
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
    virtual bool __Copy(CNetCb* pNetCb);

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
    virtual bool __Copy(GooseMsg* pGooseMsg);

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
    virtual bool __Copy(Sav91Msg* pSav91Msg);

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
    virtual bool __Copy(Sav92Msg* pSav92Msg);

protected:
    unsigned char                       m_ucSrcMAC[ETHERNET_MAC_LEN];   ///< 源mac地址
    unsigned char                       m_ucDesMAC[ETHERNET_MAC_LEN];   ///< 目标mac地址
    int                                 m_iVLanID;                      ///< VLAN ID
    int                                 m_iVLanPriority;                ///< VLAN优先级
    int                                 m_iConfigRevision;              ///< 配置版本号

#if !CHUN_HUA
	bool                                m_bHasVlanFlag;                    ///< VlanFlag
#endif

};
#endif
