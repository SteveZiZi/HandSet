/// @file
///
/// @brief
///     GOOSE���ƿ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.27
///
/// �޸���ʷ��
///

#ifndef __GOOSE_CB_H__
#define __GOOSE_CB_H__
#include <list>
#include <string>
#include "NetCb.h"

///
/// @brief
///    GOOSE���ƿ���
///
class CGooseCb : public CNetCb
{
    friend class CParamConfigGOOSEBase;
    friend class CParamConfigGOOSERecv;
    friend class CParamConfigGOOSESend;
    friend class CParamConfigSMVSend;
    friend class CSCLManager;
    friend class CStateTestResult;
    friend class CFrameInfo;
    friend class CStateSendBuffer;
    friend class CFrameSendNet;

protected:
    CGooseCb(void);
    virtual ~CGooseCb(void);

public:
    ///
    /// @brief
    ///     ���캯��
    ///
    /// @param[in]  
    ///     pParamConfigBase    ָ��������ù�������ָ��
    ///
    CGooseCb(CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    ���TestFlag
    ///
    /// @return
    ///     bool    TestFlag
    ///
    bool GetTestFlag();

    ///
    /// @brief
    ///    ����TestFlag
    ///
    /// @param[in]  bTestFlag  �µ�TestFlag
    ///
    void SetTestFlag(bool bTestFlag);

    ///
    /// @brief
    ///    ���NeedComm
    ///
    /// @return
    ///     bool    NeedComm
    ///
    bool GetNeedComm();

    ///
    /// @brief
    ///    ����NeedComm
    ///
    /// @param[in]  bNeedComm  �µ�NeedComm
    ///
    void SetNeedComm(bool bNeedComm);

    ///
    /// @brief
    ///    ���gocbRef
    ///
    /// @return
    ///     wstring    gocbRef
    ///
    std::wstring GetGocbRef();

    ///
    /// @brief
    ///    ����gocbRef
    ///
    /// @param[in]  strGocbRef  gocbRef
    ///
    void SetGocbRef(std::wstring strGocbRef);

    ///
    /// @brief
    ///    ������ݼ�
    ///
    /// @return
    ///     wstring    ���ݼ�
    ///
    std::wstring GetDataSet();

    ///
    /// @brief
    ///    �������ݼ�
    ///
    /// @param[in]  strDataSet  ���ݼ�
    ///
    void SetDataSet(std::wstring strDataSet);

    ///
    /// @brief
    ///    ���goID
    ///
    /// @return
    ///     wstring    goID
    ///
    std::wstring GetGoID();

    ///
    /// @brief
    ///    ����goID
    ///
    /// @param[in]  strGoID  goID
    ///
    void SetGoID(std::wstring strGoID);

    ///
    /// @brief
    ///    ��ü����ַ���
    ///
    /// @return
    ///     std::wstring    �����ַ���
    ///
    std::wstring GetSecurity();

    ///
    /// @brief
    ///    ���ü����ַ���
    ///
    /// @param[in]  strNewSecurity    �µļ����ַ���
    ///
    void SetSecurity(std::wstring strNewSecurity);

    ///
    /// @brief
    ///    ���״̬���
    ///
    /// @return
    ///     int    ״̬���
    ///
    int GetStNum();

    ///
    /// @brief
    ///    ����״̬���
    ///
    /// @param[in]  iNewStNum    �µ�״̬���
    ///
    void SetStNum(int iNewStNum);

    ///
    /// @brief
    ///    ���ƿ���ͨ���ķ���ֵ�Ƿ����˸ı�
    ///
    /// @return
    ///     bool    ����ֵ�Ƿ񱻸ı�
    ///
    bool IsSendValueChanged();

    ///
    /// @brief
    ///    ���ÿ��ƿ���ͨ���ķ���ֵ�Ƿ����˸ı�
    ///
    /// @param[in]  bChanged    ����ֵ�Ƿ񱻸ı�
    ///
    void SetSendValueChanged(bool bIsChanged);

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
    ///    DO(s, q, t)
    ///
    /// @return
    ///     bool    �ɹ�����true��ʧ�ܷ���false
    ///
    bool DO_s_q_t();

    ///
    /// @brief
    ///    DA(s, q, t)
    ///
    /// @return
    ///     bool    �ɹ�����true��ʧ�ܷ���false
    ///
    bool DA_s_q_t();

    ///
    /// @brief
    ///    DA(s, q)
    ///
    /// @return
    ///     bool    �ɹ�����true��ʧ�ܷ���false
    ///
    bool DA_s_q();

    ///
    /// @brief
    ///    DA(s)
    ///
    /// @return
    ///     bool    �ɹ�����true��ʧ�ܷ���false
    ///
    bool DA_SinglePoint();

    ///
    /// @brief
    ///    DA(˫��)
    ///
    /// @return
    ///     bool    �ɹ�����true��ʧ�ܷ���false
    ///
    bool DA_DoublePoint();

    ///
    /// @brief
    ///    ��ÿ��ƿ�����ͨ��ֵ��ͨ�ű����еĳ���
    ///
    /// @return
    ///     int    ͨ�����ĳ���
    ///
    int GetChannelValueLen();

    ///
    /// @brief
    ///    ������ƿ�������ͨ����ƫ����
    ///
    /// @param[in]  pGooseMsg    GOOSE���ĵı����ӿ�
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool CalcChannelOffset(GooseMsg* pGooseMsg);

protected:
    ///
    /// @brief
    ///    ����<Gocb>���ƿ�
    ///
    /// @param[in]  pXml ָ��XML���������
    ///
    /// @return
    ///     bool    true��ʾ���سɹ�
    ///
    bool __LoadConfig(CMarkup& xml);

    ///
    /// @brief
    ///    ����<Gocb>���ƿ�
    ///
    /// @param[in]  pXml ָ��XML���������
    ///
    /// @return
    ///     bool    true��ʾ����ɹ�
    ///
    bool __SaveConfig(CMarkup& xml);

    ///
    /// @brief
    ///    ��ȿ���һ��GOOSE���ƿ�
    ///
    /// @param[in]
    ///     pGocb    -   GOOSE���ƿ�ָ��
    ///
    /// @return
    ///     bool    �����ɹ�����TRUE
    ///
    virtual bool __Copy(CGooseCb* pGocb);

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
    ///    �ѵ�ǰ���ƿ���ָ�����ƿ���Ƚϣ��ж�ͨ������ֵ�Ƿ����
    ///
    /// @param[in]  pOldGocb    ָ��GOOSE���ƿ�����ָ��
    ///
    /// @return
    ///     bool    ��ȷ���true�����򷵻�false
    ///
    bool __CompareChannelSendValue(CGooseCb* pOldGocb);

    ///
    /// @brief
    ///    �޸�GOOSE���ƿ��е���/˫������ͨ��ʱ����Ҫ�Զ��޸�����/�����ʱ��ͨ����ֵ
    ///    �˺����Զ�Ѱ��Ѱ�ұ�λ�ĵ���/˫��ͨ����Ȼ��ı����ڻ�����ʱ��ͨ����ֵ
    ///
    /// @param[in]  stNewTime    ����/˫��ͨ���ı�λʱ��
    ///
    void __UpdateChannelTime(CEventTime stNewTime);

    ///
    /// @brief
    ///     �Է����˱�λ�Ŀ��ƿ�ͨ���������±���
    ///
    /// @param[in]  pucFrameHead    ָ����֡֡ͷ��ָ��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool __RecodeChangedChannels(unsigned char* pucFrameHead);

    ///
    /// @brief
    ///     ����GOOSE���ƿ���������Ҫ��λ��ʱ��ͨ������ͨ��ָ�뱣�浽������
    ///
    /// @param[out]  listShouldChangeTimeChannel    ������Ҫ��λʱ��ͨ����ָ��
    ///
    void __SaveTimeChannel(std::list<CChannel*>& listShouldChangeTimeChannel);

private:
    bool                                m_bTestFlag;                    ///< testFlag
    bool                                m_bNeedComm;                    ///< NeedComm
    std::wstring                        m_strGocbRef;                   ///< gocbRef
    std::wstring                        m_strDataSet;                   ///< ���ݼ�
    std::wstring                        m_strGoID;                      ///< goID
    std::wstring                        m_strSecurity;                  ///< �����ַ���������ѡ��

    int                                 m_stNum;                        ///< ״̬��ţ����ڼ�¼�����ܹ��ı�λ����
    bool                                m_bIsSendValueChanged;          ///< ���ƿ����Ƿ���ͨ������ֵ���ı�

    GOOSE_PDU_PUBLIC_OFFSET             m_stPduPblcOff;                 ///< ������Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ
};
#endif
