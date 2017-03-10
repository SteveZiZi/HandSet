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

#ifndef __BASE_CB_H__
#define __BASE_CB_H__
#include "channel.h"
#include "ParamConfigBase.h"
#include "src/service/datalayer/utility/data_utility.h"
#include "src/service/datalayer/include/datatypes.h"
#include "src/utils/MessageCodec/include/goose_msg.h"
#include "src/utils/MessageCodec/include/sav91_msg.h"
#include "src/utils/MessageCodec/include/sav92_msg.h"
#include "src/utils/MessageCodec/include/nanrui_ft3_msg.h"
#include "src/utils/MessageCodec/include/guowang_ft3_msg.h"

#define SMV_CHANNEL_NUM_MAX             64                              ///< SMV控制块的最大通道数

#define NODE_SmvCb_Description          L"Description"                  ///< 控制块描述
#define NODE_SmvCb_PortName             L"PortName"                     ///< 收发端口
#define NODE_SmvCb_APPID                L"APPID"                        ///< APPID
#define NODE_SmvCb_SRC_MAC              L"SrcMAC"                       ///< 源MAC地址
#define NODE_SmvCb_DES_MAC              L"DesMAC"                       ///< 目标MAC地址

#if !CHUN_HUA
#define NODE_Gocb_VLanTag                L"VLanTag"                       ///< VLAN tag
#endif

#define NODE_SmvCb_VLanID               L"VLanID"                       ///< VLAN ID
#define NODE_SmvCb_VLanPriority         L"VLanPriority"                 ///< VLAN优先级
#define NODE_SmvCb_ConfigRevision       L"ConfigRevision"               ///< 配置版本号
#define NODE_SmvCb_RatedPhaseCurrent    L"RatedPhaseCurrent"            ///< 额定相电流
#define NODE_SmvCb_RatedNeutralCurrent  L"RatedNeutralCurrent"          ///< 额定中线电流
#define NODE_SmvCb_RatedPhaseVoltage    L"RatedPhaseVoltage"            ///< 额定相电压
#define NODE_SmvCb_RatedDelay           L"RatedDelay"                   ///< 额定延时
#define NODE_SmvCb_StatusWord1          L"StatusWord1"                  ///< 状态字#1
#define NODE_SmvCb_StatusWord2          L"StatusWord2"                  ///< 状态字#2
#define NODE_SmvCb_ChannelGroup         L"ChannelGroup"                 ///< 通道组
#define NODE_Channel                    L"Channel"                      ///< 通道
#define NODE_SmvCb_SvID                 L"SvID"                         ///< SvID
#define NODE_SmvCb_DataSetRef           L"DataSet"                      ///< DataSet
#define NODE_SmvCb_DataSetOpt           L"DataSetOpt"                   ///< DataSetOpt
#define NODE_SmvCb_RefrTmOpt            L"RefrTmOpt"                    ///< RefrTmOpt
#define NODE_SmvCb_SmpRateOpt           L"SmpRateOpt"                   ///< SmpRateOpt
#define NODE_SmvCb_SecurityOpt          L"SecurityOpt"                  ///< SecurityOpt
#define NODE_SmvCb_LNName               L"LNName"                       ///< LNName
#define NODE_SmvCb_LDName               L"LDName"                       ///< LDName
#define NODE_SmvCb_DSName               L"DSName"                       ///< DSName
#define NODE_Channel_Quality            L"Quality"                      ///< 通道品质值

class CBaseCb
{
protected:
    CBaseCb(void);
    virtual ~CBaseCb(void);

public:
    ///
    /// @brief
    ///     构造函数
    ///
    /// @param[in]  
    ///     pParamConfigBase    指向参数配置管理器的指针
    ///
    CBaseCb(CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    获得控制块选中状态
    ///
    /// @return
    ///     bool    返回true表示控制块被选中
    ///
    bool IsSelected();

    ///
    /// @brief
    ///    设置控制块选中状态
    ///
    /// @param[in]  bSelected  控制块选中状态
    ///
    void Selected(bool bSelected);

    ///
    /// @brief
    ///    获得控制块描述信息
    ///
    /// @return
    ///     wstring    控制块描述信息
    ///
    std::wstring GetDescription();

    ///
    /// @brief
    ///    设置控制块描述信息
    ///
    /// @param[in]  strDescription  控制块描述信息
    ///
    void SetDescription(std::wstring strDescription);

    ///
    /// @brief
    ///    获得收发端口
    ///
    /// @return
    ///     DsmPortName    收发端口
    ///
    DsmPortName GetPortName();

    ///
    /// @brief
    ///    设置收发端口
    ///
    /// @param[in]  eNewPortName  收发端口
    ///
    void SetPortName(DsmPortName eNewPortName);

    ///
    /// @brief
    ///    获得APPID
    ///
    /// @return
    ///     int    APPID
    ///
    int GetAPPID();

    ///
    /// @brief
    ///    设置APPID
    ///
    /// @param[in]  iNewAPPID  新的APPID
    ///
    /// @return
    ///     bool    true表示保存成功
    ///
    bool SetAPPID(int iNewAPPID);

    ///
    /// @brief
    ///    添加Channel
    ///
    /// @return
    ///     CChannel*    新添加的Channel对象的指针
    ///
    virtual CChannel* AddChannel();

    ///
    /// @brief
    ///    删除指定ID的通道
    ///
    /// @param[in]  pDelChannel 通道对象指针
    ///
    /// @return
    ///     bool    true表示删除成功
    ///
    bool DeleteChannel(CChannel* pDelChannel);

    ///
    /// @brief
    ///    枚举第一个Channel
    ///
    /// @return
    ///     CChannel*    成功时返回Channel指针，失败时返回NULL
    ///
    CChannel* FirstChannel();

    ///
    /// @brief
    ///    枚举下一个Channel
    ///
    /// @param[in]  pCurChannel 当前Channel的指针
    ///
    /// @return
    ///     CChannel*    成功时返回Channel指针，失败时返回NULL
    ///
    CChannel* NextChannel(CChannel* pCurChannel);

    ///
    /// @brief
    ///    枚举上一个Channel
    ///
    /// @param[in]  pCurChannel 当前Channel的指针
    ///
    /// @return
    ///     CChannel*    成功时返回Channel指针，失败时返回NULL
    ///
    CChannel* PrevChannel(CChannel* pCurChannel);

    ///
    /// @brief
    ///    枚举最后一个Channel
    ///
    /// @return
    ///     CChannel*    成功时返回Channel指针，失败时返回NULL
    ///
    CChannel* LastChannel();

    ///
    /// @brief
    ///    获得控制块的第nIndex（从0开始计数）个通道
    ///
    /// @param[in]  nIndex    通道序号
    ///
    /// @return
    ///     CChannel*    成功时返回Channel指针，失败时返回NULL
    ///
    CChannel* GetChannelByIndex(int nIndex);

    ///
    /// @brief
    ///    清除所有通道
    ///
    void ClearAllChannel();

    ///
    /// @brief
    ///    获得通道数
    ///
    /// @return
    ///     int    通道数
    ///
    int GetChannelCount();

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
    bool SetChannelCount(unsigned int uiNewCount);

    ///
    /// @brief
    ///    获得被选中的通道数
    ///
    /// @return
    ///     int    被选中的通道数
    ///
    int GetSelectedChannelCount();

    ///
    /// @brief
    ///    使用指定的通道模板来填充控制块
    ///
    /// @param[in]  eType    模板类型
    ///
    /// @return
    ///     bool    成功返回true
    ///
    virtual bool UseChannelTemplate(enumSmvRecvConfigTemplateType eType);

    ///
    /// @brief
    ///    更新所有控制块中电压通道的一次额定值
    ///
    /// @param[in]  dNewValue    新的电压一次额定值（V）
    ///
    void UpdateRatedPrimaryVoltage(double dNewValue);

    ///
    /// @brief
    ///    更新所有控制块中电压通道的二次额定值
    ///
    /// @param[in]  dNewValue    新的电压二次额定值（V）
    ///
    void UpdateRatedSecondaryVoltage(double dNewValue);

    ///
    /// @brief
    ///    更新所有控制块中电流通道的一次额定值
    ///
    /// @param[in]  dNewValue    新的电流一次额定值（A）
    ///
    void UpdateRatedPrimaryCurrent(double dNewValue);

    ///
    /// @brief
    ///    更新所有控制块中电流通道的二次额定值
    ///
    /// @param[in]  dNewValue    新的电流二次额定值（A）
    ///
    void UpdateRatedSecondaryCurrent(double dNewValue);

    ///
    /// @brief
    ///    更新所有控制块中时间通道的额定值
    ///
    /// @param[in]  dNewValue    新的MU额定延时（us）
    ///
    void UpdateMuRatedDelay(double dNewValue);

protected:
    ///
    /// @brief
    ///    添加Channel
    ///
    /// @param[in]  pNewChannel 指向Channel对象的指针
    ///
    /// @return
    ///     bool    true表示添加成功
    ///
    bool __AddChannel(CChannel* pNewChannel);

    ///
    /// @brief
    ///    深度拷贝一个控制块
    ///
    /// @param[in]  pBaseCb    指向基本控制块对象的指针
    ///
    /// @return
    ///     bool    拷贝成功返回TRUE
    ///
    virtual bool __Copy(CBaseCb* pBaseCb);

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
    virtual bool __Copy(NanRuiFT3Msg* pFt3Msg);

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
    virtual bool __Copy(GuoWangFT3Msg* pFt3ExpandMsg);

protected:
    CParamConfigBase*                   m_pParamConfigBase;             ///< 参数配置管理器

    bool                                m_bSelected;                    ///< 控制块是否被选择
    enumFrameType                       m_eFrameType;                   ///< 控制块类型
    std::wstring                        m_strDescription;               ///< 控制块描述信息
    DsmPortName                         m_ePortName;                    ///< 收发端口
    int                                 m_iAPPID;                       ///< APPID

    CChannel*                           m_pChannelHead;                 ///< 第一个通道
    CChannel*                           m_pChannelTail;                 ///< 最后一个通道
    int                                 m_iChannelCount;                ///< 通道数

};
#endif
