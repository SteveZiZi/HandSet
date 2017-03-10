///
/// @brief
///     ���ƿ����
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.29
///
/// �޸���ʷ��
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

#define SMV_CHANNEL_NUM_MAX             64                              ///< SMV���ƿ�����ͨ����

#define NODE_SmvCb_Description          L"Description"                  ///< ���ƿ�����
#define NODE_SmvCb_PortName             L"PortName"                     ///< �շ��˿�
#define NODE_SmvCb_APPID                L"APPID"                        ///< APPID
#define NODE_SmvCb_SRC_MAC              L"SrcMAC"                       ///< ԴMAC��ַ
#define NODE_SmvCb_DES_MAC              L"DesMAC"                       ///< Ŀ��MAC��ַ

#if !CHUN_HUA
#define NODE_Gocb_VLanTag                L"VLanTag"                       ///< VLAN tag
#endif

#define NODE_SmvCb_VLanID               L"VLanID"                       ///< VLAN ID
#define NODE_SmvCb_VLanPriority         L"VLanPriority"                 ///< VLAN���ȼ�
#define NODE_SmvCb_ConfigRevision       L"ConfigRevision"               ///< ���ð汾��
#define NODE_SmvCb_RatedPhaseCurrent    L"RatedPhaseCurrent"            ///< ������
#define NODE_SmvCb_RatedNeutralCurrent  L"RatedNeutralCurrent"          ///< ����ߵ���
#define NODE_SmvCb_RatedPhaseVoltage    L"RatedPhaseVoltage"            ///< ����ѹ
#define NODE_SmvCb_RatedDelay           L"RatedDelay"                   ///< ���ʱ
#define NODE_SmvCb_StatusWord1          L"StatusWord1"                  ///< ״̬��#1
#define NODE_SmvCb_StatusWord2          L"StatusWord2"                  ///< ״̬��#2
#define NODE_SmvCb_ChannelGroup         L"ChannelGroup"                 ///< ͨ����
#define NODE_Channel                    L"Channel"                      ///< ͨ��
#define NODE_SmvCb_SvID                 L"SvID"                         ///< SvID
#define NODE_SmvCb_DataSetRef           L"DataSet"                      ///< DataSet
#define NODE_SmvCb_DataSetOpt           L"DataSetOpt"                   ///< DataSetOpt
#define NODE_SmvCb_RefrTmOpt            L"RefrTmOpt"                    ///< RefrTmOpt
#define NODE_SmvCb_SmpRateOpt           L"SmpRateOpt"                   ///< SmpRateOpt
#define NODE_SmvCb_SecurityOpt          L"SecurityOpt"                  ///< SecurityOpt
#define NODE_SmvCb_LNName               L"LNName"                       ///< LNName
#define NODE_SmvCb_LDName               L"LDName"                       ///< LDName
#define NODE_SmvCb_DSName               L"DSName"                       ///< DSName
#define NODE_Channel_Quality            L"Quality"                      ///< ͨ��Ʒ��ֵ

class CBaseCb
{
protected:
    CBaseCb(void);
    virtual ~CBaseCb(void);

public:
    ///
    /// @brief
    ///     ���캯��
    ///
    /// @param[in]  
    ///     pParamConfigBase    ָ��������ù�������ָ��
    ///
    CBaseCb(CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    ��ÿ��ƿ�ѡ��״̬
    ///
    /// @return
    ///     bool    ����true��ʾ���ƿ鱻ѡ��
    ///
    bool IsSelected();

    ///
    /// @brief
    ///    ���ÿ��ƿ�ѡ��״̬
    ///
    /// @param[in]  bSelected  ���ƿ�ѡ��״̬
    ///
    void Selected(bool bSelected);

    ///
    /// @brief
    ///    ��ÿ��ƿ�������Ϣ
    ///
    /// @return
    ///     wstring    ���ƿ�������Ϣ
    ///
    std::wstring GetDescription();

    ///
    /// @brief
    ///    ���ÿ��ƿ�������Ϣ
    ///
    /// @param[in]  strDescription  ���ƿ�������Ϣ
    ///
    void SetDescription(std::wstring strDescription);

    ///
    /// @brief
    ///    ����շ��˿�
    ///
    /// @return
    ///     DsmPortName    �շ��˿�
    ///
    DsmPortName GetPortName();

    ///
    /// @brief
    ///    �����շ��˿�
    ///
    /// @param[in]  eNewPortName  �շ��˿�
    ///
    void SetPortName(DsmPortName eNewPortName);

    ///
    /// @brief
    ///    ���APPID
    ///
    /// @return
    ///     int    APPID
    ///
    int GetAPPID();

    ///
    /// @brief
    ///    ����APPID
    ///
    /// @param[in]  iNewAPPID  �µ�APPID
    ///
    /// @return
    ///     bool    true��ʾ����ɹ�
    ///
    bool SetAPPID(int iNewAPPID);

    ///
    /// @brief
    ///    ���Channel
    ///
    /// @return
    ///     CChannel*    ����ӵ�Channel�����ָ��
    ///
    virtual CChannel* AddChannel();

    ///
    /// @brief
    ///    ɾ��ָ��ID��ͨ��
    ///
    /// @param[in]  pDelChannel ͨ������ָ��
    ///
    /// @return
    ///     bool    true��ʾɾ���ɹ�
    ///
    bool DeleteChannel(CChannel* pDelChannel);

    ///
    /// @brief
    ///    ö�ٵ�һ��Channel
    ///
    /// @return
    ///     CChannel*    �ɹ�ʱ����Channelָ�룬ʧ��ʱ����NULL
    ///
    CChannel* FirstChannel();

    ///
    /// @brief
    ///    ö����һ��Channel
    ///
    /// @param[in]  pCurChannel ��ǰChannel��ָ��
    ///
    /// @return
    ///     CChannel*    �ɹ�ʱ����Channelָ�룬ʧ��ʱ����NULL
    ///
    CChannel* NextChannel(CChannel* pCurChannel);

    ///
    /// @brief
    ///    ö����һ��Channel
    ///
    /// @param[in]  pCurChannel ��ǰChannel��ָ��
    ///
    /// @return
    ///     CChannel*    �ɹ�ʱ����Channelָ�룬ʧ��ʱ����NULL
    ///
    CChannel* PrevChannel(CChannel* pCurChannel);

    ///
    /// @brief
    ///    ö�����һ��Channel
    ///
    /// @return
    ///     CChannel*    �ɹ�ʱ����Channelָ�룬ʧ��ʱ����NULL
    ///
    CChannel* LastChannel();

    ///
    /// @brief
    ///    ��ÿ��ƿ�ĵ�nIndex����0��ʼ��������ͨ��
    ///
    /// @param[in]  nIndex    ͨ�����
    ///
    /// @return
    ///     CChannel*    �ɹ�ʱ����Channelָ�룬ʧ��ʱ����NULL
    ///
    CChannel* GetChannelByIndex(int nIndex);

    ///
    /// @brief
    ///    �������ͨ��
    ///
    void ClearAllChannel();

    ///
    /// @brief
    ///    ���ͨ����
    ///
    /// @return
    ///     int    ͨ����
    ///
    int GetChannelCount();

    ///
    /// @brief
    ///    ����ͨ��������
    ///    �����ǰͨ���������µ�ͨ��������ôɾ��ĩβ�Ķ���ͨ��
    ///    �����ǰͨ����С���µ�ͨ��������ô���ͨ��
    ///
    /// @param[in]  uiNewCount    �µ�ͨ����
    ///
    /// @return
    ///     bool    �ɹ�����true��ʧ�ܷ���false
    ///
    bool SetChannelCount(unsigned int uiNewCount);

    ///
    /// @brief
    ///    ��ñ�ѡ�е�ͨ����
    ///
    /// @return
    ///     int    ��ѡ�е�ͨ����
    ///
    int GetSelectedChannelCount();

    ///
    /// @brief
    ///    ʹ��ָ����ͨ��ģ���������ƿ�
    ///
    /// @param[in]  eType    ģ������
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    virtual bool UseChannelTemplate(enumSmvRecvConfigTemplateType eType);

    ///
    /// @brief
    ///    �������п��ƿ��е�ѹͨ����һ�ζֵ
    ///
    /// @param[in]  dNewValue    �µĵ�ѹһ�ζֵ��V��
    ///
    void UpdateRatedPrimaryVoltage(double dNewValue);

    ///
    /// @brief
    ///    �������п��ƿ��е�ѹͨ���Ķ��ζֵ
    ///
    /// @param[in]  dNewValue    �µĵ�ѹ���ζֵ��V��
    ///
    void UpdateRatedSecondaryVoltage(double dNewValue);

    ///
    /// @brief
    ///    �������п��ƿ��е���ͨ����һ�ζֵ
    ///
    /// @param[in]  dNewValue    �µĵ���һ�ζֵ��A��
    ///
    void UpdateRatedPrimaryCurrent(double dNewValue);

    ///
    /// @brief
    ///    �������п��ƿ��е���ͨ���Ķ��ζֵ
    ///
    /// @param[in]  dNewValue    �µĵ������ζֵ��A��
    ///
    void UpdateRatedSecondaryCurrent(double dNewValue);

    ///
    /// @brief
    ///    �������п��ƿ���ʱ��ͨ���Ķֵ
    ///
    /// @param[in]  dNewValue    �µ�MU���ʱ��us��
    ///
    void UpdateMuRatedDelay(double dNewValue);

protected:
    ///
    /// @brief
    ///    ���Channel
    ///
    /// @param[in]  pNewChannel ָ��Channel�����ָ��
    ///
    /// @return
    ///     bool    true��ʾ��ӳɹ�
    ///
    bool __AddChannel(CChannel* pNewChannel);

    ///
    /// @brief
    ///    ��ȿ���һ�����ƿ�
    ///
    /// @param[in]  pBaseCb    ָ��������ƿ�����ָ��
    ///
    /// @return
    ///     bool    �����ɹ�����TRUE
    ///
    virtual bool __Copy(CBaseCb* pBaseCb);

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

    ///
    /// @brief
    ///    ��ȿ���һ��ɨ���б��е�FT3���ƿ�
    ///
    /// @param[in]
    ///     pFt3Msg    -   FT3���ĵı�������
    ///
    /// @return
    ///     bool    �����ɹ�����TRUE
    ///
    virtual bool __Copy(NanRuiFT3Msg* pFt3Msg);

    ///
    /// @brief
    ///    ��ȿ���һ��ɨ���б��е�FT3��չ���ƿ�
    ///
    /// @param[in]
    ///     pFt3ExpandMsg    -   FT3��չ���ĵı�������
    ///
    /// @return
    ///     bool    �����ɹ�����TRUE
    ///
    virtual bool __Copy(GuoWangFT3Msg* pFt3ExpandMsg);

protected:
    CParamConfigBase*                   m_pParamConfigBase;             ///< �������ù�����

    bool                                m_bSelected;                    ///< ���ƿ��Ƿ�ѡ��
    enumFrameType                       m_eFrameType;                   ///< ���ƿ�����
    std::wstring                        m_strDescription;               ///< ���ƿ�������Ϣ
    DsmPortName                         m_ePortName;                    ///< �շ��˿�
    int                                 m_iAPPID;                       ///< APPID

    CChannel*                           m_pChannelHead;                 ///< ��һ��ͨ��
    CChannel*                           m_pChannelTail;                 ///< ���һ��ͨ��
    int                                 m_iChannelCount;                ///< ͨ����

};
#endif
