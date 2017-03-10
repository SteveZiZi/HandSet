/// @file
///
/// @brief
///     ͨ��
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.28
///
/// �޸���ʷ��
///

#ifndef __CHANNEL_H__
#define __CHANNEL_H__
#include <list>
#include <map>
#include <math.h>
#include <string>
#include "ParamConfigBase.h"
#include "ParamConfigBasic.h"
#include "src\service\datalayer\include\datatypes.h"
#include "src\service\datalayer\send\ChannelRecvValues.h"
#include "src\utils\MessageCodec\include\goose_msg.h"

#define FLOAT_PRECISION                 (0.0005f)                       ///< ����������

///
/// @brief
///    ״̬������
///
class CStatus;

///
/// @brief
///    ͨ����
///
class CChannel
{
    friend class CBaseCb;
    friend class CGooseCb;
    friend class CIEC9_1Cb;
    friend class CIEC9_2Cb;
    friend class CFT3Cb;
    friend class CFT3ExpandCb;
    friend class CParamConfigGOOSEBase;
    friend class CParamConfigGOOSESend;
    friend class CParamConfigSMVSend;
    friend class CSCLManager;

protected:
    CChannel(void);
    virtual ~CChannel(void);

public:
    ///
    /// @brief
    ///     ���캯��
    ///
    /// @param[in]  pParentBaseCb       ָ�򸸿��ƿ��ָ��
    /// @param[in]  pParentChannel      ָ��ͨ����ָ��
    /// @param[in]  pParamConfigBase    ָ��������ù�������ָ��
    ///
    CChannel(CBaseCb* pParentBaseCb, CChannel* pParentChannel, CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    ���ͨ������ͨ�������������ƿ飨��ͨ�����е������ţ���0��ʼ������
    ///
    /// @return
    ///     int    ͨ���������ţ�-1��ʾʧ�ܡ�
    ///
    int GetIndex();

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
    ///    ���ͨ��������Ϣ
    ///
    /// @return
    ///     wstring    ͨ��������Ϣ
    ///
    std::wstring GetDescription();

    ///
    /// @brief
    ///    ����ͨ��������Ϣ
    ///
    /// @param[in]  strDescription  ͨ��������Ϣ
    ///
    void SetDescription(std::wstring strDescription);

    ///
    /// @brief
    ///    ���ͨ������
    ///
    /// @return
    ///     enumChannelType    ͨ������
    ///
    enumChannelType GetChannelType();

    ///
    /// @brief
    ///    ����ͨ������
    ///
    /// @param[in]  eNewChannelType  ͨ������
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool SetChannelType(enumChannelType eNewChannelType);

    ///
    /// @brief
    ///    ���ͨ����������
    ///
    /// @return
    ///     bool    ͨ���Ƿ����ڲ�������
    ///
    bool IsMeasure();

    ///
    /// @brief
    ///    ����ͨ������
    ///
    /// @param[in]  bNewMeasure  ͨ���Ƿ����ڲ�������
    ///
    void SetMeasure(bool bNewMeasure);

    ///
    /// @brief
    ///    ���ͨ����λ
    ///
    /// @return
    ///     enumChannelType    ͨ����λ
    ///
    ChannelPhase GetChannelPhase();

    ///
    /// @brief
    ///    ����ͨ����λ
    ///
    /// @param[in]  eNewChannelPhase  ͨ����λ
    ///
    void SetChannelPhase(ChannelPhase eNewChannelPhase);

    ///
    /// @brief
    ///    ���һ�ζֵ��������λΪA����ѹ��λΪV��
    ///
    /// @return
    ///     double    һ�ζֵ
    ///
    double GetOnceRatings();

    ///
    /// @brief
    ///    ����һ�ζֵ��������λΪA����ѹ��λΪV��
    ///
    /// @param[in]  dOnceRatings  һ�ζֵ
    ///
    void SetOnceRatings(double dOnceRatings);

    ///
    /// @brief
    ///    ��ö��ζֵ��������λΪA����ѹ��λΪV��
    ///
    /// @return
    ///     double    ���ζֵ
    ///
    double GetSecondaryRatings();

    ///
    /// @brief
    ///    ���ö��ζֵ��������λΪA����ѹ��λΪV��
    ///
    /// @param[in]  dSecondaryRatings  ���ζֵ
    ///
    void SetSecondaryRatings(double dSecondaryRatings);

    ///
    /// @brief
    ///    ���ͨ��ӳ��
    ///
    /// @return
    ///     wstring    ͨ��ӳ��
    ///
    std::wstring GetVirtualChannel();

    ///
    /// @brief
    ///    ����ͨ��ӳ��
    ///
    /// @param[in]  strVirtualChannel  ͨ��ӳ��
    ///
    void SetVirtualChannel(std::wstring strVirtualChannel);

    ///
    /// @brief
    ///    ö�ٵ�һ����ͨ��
    ///
    /// @return
    ///     CChannel*    �ɹ�ʱ����ͨ��ָ�룬ʧ��ʱ����NULL
    ///
    CChannel* FirstSubChannel();

    ///
    /// @brief
    ///    �����һ����ͨ��
    ///
    /// @param[in]  pCurSubChannel ��ǰ��ͨ����ָ��
    ///
    /// @return
    ///     CChannel*    �ɹ�ʱ������ͨ��ָ�룬ʧ��ʱ����NULL
    ///
    CChannel* NextSubChannel(CChannel* pCurSubChannel);

    ///
    /// @brief
    ///    �����һ����ͨ��
    ///
    /// @param[in]  pCurSubChannel ��ǰ��ͨ����ָ��
    ///
    /// @return
    ///     CChannel*    �ɹ�ʱ������ͨ��ָ�룬ʧ��ʱ����NULL
    ///
    CChannel* PrevSubChannel(CChannel* pCurSubChannel);

    ///
    /// @brief
    ///    ���������ͨ��
    ///
    void ClearAllSubChannel();

    ///
    /// @brief
    ///    �����ͨ����
    ///
    /// @return
    ///     int    ��ͨ����
    ///
    int GetSubChannelCount();

    ///
    /// @brief
    ///     ���ͨ���������ƿ�����ָ��
    ///
    /// @return
    ///     CBaseCb*    ͨ���������ƿ�����ָ��
    ///
    CBaseCb* GetParentBaseCb();

    ///
    /// @brief
    ///    ��ø�ͨ��ָ��
    ///
    /// @return
    ///     CChannel*    ��ͨ��ָ��
    ///
    CChannel* GetParentChannel();

    ///
    /// @brief
    ///    ���GOOSEͨ���ķ���ֵ
    ///
    /// @return
    ///     GOOSE_DATA_VARIANT    GOOSEͨ���ĳ�ʼֵ
    ///
    GOOSE_DATA_VARIANT& GetGooseSendValue();

    ///
    /// @brief
    ///    ����GOOSEͨ���ķ���ֵ
    ///
    /// @param[in]  stNewGooseValue    GOOSEͨ���ķ���ֵ
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool SetGooseSendValue(GOOSE_DATA_VARIANT& stNewGooseValue);

    ///
    /// @brief
    ///    ���GOOSEͨ����ʵʱֵ���ڽ��յ�GOOSE����ʱ���޸�ͨ����ʵʱֵ
    ///
    /// @return
    ///     GOOSE_DATA_VARIANT    GOOSEͨ����ʵʱֵ
    ///
    GOOSE_DATA_VARIANT& GetGooseRecvValue();

    ///
    /// @brief
    ///    ����GOOSEͨ����ʵʱֵ���ڽ��յ�GOOSE����ʱ�����Ե��ô˺����޸�ͨ����ʵʱֵ
    ///
    /// @param[in]  stGooseValue    GOOSEͨ����ʵʱֵ
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool SetGooseRecvValue(GOOSE_DATA_VARIANT& stGooseValue);

    ///
    /// @brief
    ///    ���ͨ��Ʒ��ֵ
    ///
    /// @return
    ///     unsigned short    ͨ��Ʒ��ֵ
    ///
    unsigned short GetQuality();

    ///
    /// @brief
    ///    ����ͨ��Ʒ��ֵ
    ///
    /// @param[in]  usNewQuality    �µ�Ʒ��ֵ
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool SetQuality(unsigned short usNewQuality);

    ///
    /// @brief
    ///     ͨ���ķ���ֵ�Ƿ񱻸ı�
    ///
    /// @return
    ///     bool    true��ʾͨ���ķ���ֵ���ı�
    ///
    bool IsSendValueChanged();

    ///
    /// @brief
    ///    ����ͨ���ķ���ֵ�Ƿ񱻸ı�
    ///
    /// @param[in]  bIsChanged    ͨ��ֵ�Ƿ񱻸ı�
    ///
    void SetSendValueChanged(bool bIsChanged);

    ///
    /// @brief
    ///    ���ͨ��ֵ��ͨ�ű�������ռ�ֽ���
    ///
    /// @return
    ///     int    �ֽ���
    ///
    int GetValueLength();

protected:

    ///
    /// @brief
    ///    �����ͨ��
    ///
    /// @return
    ///     CChannel*    �ɹ�ʱ������ͨ��ָ�룬ʧ��ʱ����NULL
    ///
    CChannel* __AddSubChannel();

    ///
    /// @brief
    ///    �����ͨ��
    ///
    /// @param[in]  pNewChannel ָ��Channel�����ָ��
    ///
    /// @return
    ///     bool    true��ʾ��ӳɹ�
    ///
    bool __AddSubChannel(CChannel* pNewChannel);

    ///
    /// @brief
    ///    ��ȿ���һ��ͨ��
    ///
    /// @param[in]  pChannel    ָ��Channel�����ָ��
    ///
    /// @return
    ///     bool    �����ɹ�����TRUE
    ///
    virtual bool __Copy(CChannel* pChannel);

    ///
    /// @brief
    ///    ��ȿ���һ��GOOSE_DATA����
    ///
    /// @param[in]  pGooseData ָ��GOOSE_DATA�����ָ��
    ///
    /// @return
    ///     bool    �����ɹ�����TRUE
    ///
    virtual bool __Copy(GOOSE_DATA* pGooseData);

    ///
    /// @brief
    ///    ����ͨ�������Ŀ��ƿ�
    ///
    /// @param[in]  pBaseCb    ���ƿ�ָ��
    ///
    /// @return
    ///     void    
    ///
    void __SetParentBaseCb(CBaseCb* pBaseCb);

    ///
    /// @brief
    ///    ����ͨ�������ĸ�ͨ��ָ��
    ///
    /// @param[in]  pChannel    ��ͨ��ָ��
    ///
    /// @return
    ///     void    
    ///
    void __SetParentChannel(CChannel* pChannel);

    ///
    /// @brief
    ///    �ж�ͨ������ֵ�Ƿ���ָ��ͨ�������
    ///
    /// @param[in]  pOldChannel    ָ��ͨ�������ָ��
    ///
    /// @return
    ///     bool    ͨ��ֵ��ȷ���true
    ///
    bool __CompareSendValue(CChannel* pOldChannel);

    ///
    /// @brief
    ///    ����SMV<Channel>
    ///
    /// @param[in]  pXml ָ��XML���������
    ///
    /// @return
    ///     bool    true��ʾ���سɹ�
    ///
    bool __LoadSmvConfig(CMarkup& xml);

    ///
    /// @brief
    ///    ����SMV<Channel>
    ///
    /// @param[in]  pXml ָ��XML���������
    ///
    /// @return
    ///     bool    true��ʾ���سɹ�
    ///
    bool __SaveSmvConfig(CMarkup& xml);

    ///
    /// @brief
    ///    ����GOOSE<Channel>
    ///
    /// @param[in]  pXml ָ��XML���������
    ///
    /// @return
    ///     bool    true��ʾ���سɹ�
    ///
    bool __LoadGooseConfig(CMarkup& xml);

    ///
    /// @brief
    ///    ����GOOSE<Channel>
    ///
    /// @param[in]  pXml ָ��XML���������
    ///
    /// @return
    ///     bool    true��ʾ���سɹ�
    ///
    bool __SaveGooseConfig(CMarkup& xml);

    ///
    /// @brief
    ///    �޸�GOOSE���ƿ��е���/˫������ͨ��ʱ����Ҫ�Զ��޸�����/�����ʱ��ͨ����ֵ
    ///    �˺����Զ�Ѱ��Ѱ�ұ�λ�ĵ���/˫��ͨ����Ȼ��ı����ڻ�����ʱ��ͨ����ֵ
    ///
    /// @param[in]  stNewTime    ����/˫��ͨ���ı�λʱ��
    ///
    void __UpdateSubChannelTime(CEventTime stNewTime);

    ///
    /// @brief
    ///     �Է����˱�λ����ͨ���������±���
    ///
    /// @param[in]  pucFrameHead    ָ����֡֡ͷ��ָ��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool __RecodeChangedSubChannels(unsigned char* pucFrameHead);

    ///
    /// @brief
    ///     ����������Ҫ��λ��ʱ����ͨ��������ͨ��ָ�뱣�浽������
    ///
    /// @param[out]  listShouldChangeTimeChannel    ������Ҫ��λʱ��ͨ����ָ��
    ///
    void __SaveSubTimeChannel(std::list<CChannel*>& listShouldChangeTimeChannel);

    ///
    /// @brief
    ///    ����ͨ����ƫ����
    ///
    /// @param[in]  pGooseData      ����GOOSEͨ����Ϣ�Ľṹ��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool __CalcChannelOffset( GOOSE_DATA* pGooseData );

private:
    CBaseCb*                            m_pParentBaseCb;                ///< ͨ�������Ŀ��ƿ�
    CChannel*                           m_pParentChannel;               ///< ��ͨ���ĸ�ͨ��
    CParamConfigBase*                   m_pParamConfigBase;             ///< �������ù�����
    CChannel*                           m_pSourceChannel;               ///< ͨ������Դ
    CChannel*                           m_pPrev;                        ///< ǰһ��ͨ��
    CChannel*                           m_pNext;                        ///< ��һ��ͨ��

    bool                                m_bSelected;                    ///< ͨ���Ƿ�ѡ��
    std::wstring                        m_strDescription;               ///< ͨ��������Ϣ
    enumChannelType                     m_eChannelType;                 ///< ͨ������
    unsigned short                      m_usQuality;                    ///< IEC61850-9-2ͨ��Ʒ��ֵ
    bool                                m_bMeasure;                     ///< �Ƿ����ڲ�������
    ChannelPhase                        m_eChannelPhase;                ///< ��λ
    double                              m_dOnceRatings;                 ///< һ�ζֵ��������λΪA����ѹ��λΪV��
    double                              m_dSecondaryRatings;            ///< ���ζֵ��������λΪA����ѹ��λΪV��
    std::wstring                        m_strVirtualChannel;            ///< ͨ��ӳ��

    GOOSE_DATA_VARIANT                  m_stGooseSendValue;             ///< ��ǰGOOSEͨ������ֵ�����ڱ����ֶ�����ʱ���õ�GOOSEͨ��ֵ����Ҫ���浽�����ļ��У�
    bool                                m_bIsSendValueChanged;          ///< GOOSEͨ������ֵ�Ƿ����˸ı䣨���ֶ�ʵ��ʱ������жϣ�
    GOOSE_DATA_VARIANT                  m_stGooseRecvValue;             ///< ��ǰGOOSEͨ������ֵ������GOOSE����ʱ���ڱ���ͨ����ʵʱֵ������Ҫ���浽�����ļ��У�

    std::list<CChannel *>               m_listSubChannelGroup;          ///< ��ͨ��
};
#endif
