///
/// @brief
///     ����״̬���Խ������
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.7.1
///
/// �޸���ʷ��
///

#ifndef __STATE_TEST_RESULT_H__
#define __STATE_TEST_RESULT_H__
#include <map>
#include "ChannelRecvValues.h"
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\config\SMVSendVirtualChannel.h"
#include "src\service\datalayer\config\Status.h"

///
/// @brief
///    ���Ե�����
///
enum enumTestType
{
    TEST_TYPE_UNKNOWN = 0,                                              ///< δ֪����
    TEST_TYPE_SMV,                                                      ///< �ֶ�����SMV���ԵĽ����
    TEST_TYPE_GOOSE,                                                    ///< �ֶ�����GOOSE���ԵĽ����
    TEST_TYPE_STATE_SEQUENCE                                            ///< ״̬��������Ľ����
};

///
/// @brief
///    ����֡��Ϣ
///
class CFrameInfo
{
public:
    ///
    /// @brief
    ///     ����GOOSE���ƿ���������Ҫ��λ��ʱ��ͨ������ͨ��ָ�뱣�浽������
    ///
    void __SaveTimeChannel();

    ///
    /// @brief
    ///    ���·���֡����Ҫ��λ��ʱ��ͨ����ֵ
    ///
    /// @param[in]  pucFrameHead    ָ����֡֡ͷ��ָ��
    /// @param[in]  stNewTime       �µı�λʱ��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool __UpdateChannelTime(unsigned char* pucFrameHead, CEventTime stNewTime);

public:
    CGooseCb*                           m_pGooseCb;                     ///< ����֡��Ӧ��GOOSE���ƿ�
    unsigned int                        m_uiFrameOffset;                ///< ֡ƫ��
    unsigned int                        m_uiFrameLen;                   ///< ֡����
    std::list<CChannel*>                m_listTimeChannel;              ///< ���ƿ�����Ҫ��λ��ʱ��ͨ��
};

///
/// @brief
///    ����˿ڷ���֡����
///
class CStateSendBuffer
{
public:
    ///
    /// @brief
    ///    ���캯��
    ///
    /// @param[in]  ePortName    ���Ͷ˿�
    ///
    CStateSendBuffer(DsmPortName ePortName);

    ///
    /// @brief
    ///    ��������
    ///
    ~CStateSendBuffer();

    ///
    /// @brief
    ///    ��������֡������
    ///
    /// @param[in]  uiBufferSize    �������ֽ���
    ///
    /// @return
    ///     unsigned char*    ָ�򻺳�����ָ��
    ///
    unsigned char* AllocDataBuffer(uint32_t uiBufferSize);

    ///
    /// @brief
    ///    �ͷ�����֡������
    ///
    void FreeDataBuffer();

    ///
    /// @brief
    ///    �޸����з���֡�ı�λʱ���ֽ�
    ///
    /// @param[in]  stStateStartTime    �µı�λʱ��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool UpdateEventTime(CEventTime stStateStartTime);

    ///
    /// @brief
    ///    �޸����з���֡�б�λʱ��ͨ����ֵ
    ///
    /// @param[in]  stStateStartTime    �µı�λʱ��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool UpdateChannelTime(CEventTime stStateStartTime);

    ///
    /// @brief
    ///     �Է����˱�λ��GOOSE���ƿ�������±���
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool RecodeChangedGooseCb();

    ///
    /// @brief
    ///     �������ͻ���
    ///
    /// @param[in]  pOldStateSendBuffer     �ϵķ��ͻ���
    ///
    /// @return
    ///     bool    �����ɹ�����true
    ///
    bool Copy(CStateSendBuffer* pOldStateSendBuffer);

    ///
    /// @brief
    ///     ����GOOSE���ƿ�����Ҫ��λ��ʱ��ͨ��
    ///
    void __SaveGooseCbTimeChannel();

public:
    DsmPortName                         m_ePortName;                    ///< ���Ͷ˿�
    IOCTRL_IN_PARAM_NET_BIG             m_stIOCtrlParam;                ///< �����Ͳ�������
    unsigned char*                      m_pucDataBuffer;                ///< �����͵�����֡
    unsigned int                        m_uiDataBufferLen;              ///< ����������֡�ĳ���
    std::list<CFrameInfo>               m_listFrameInfo;                ///< ��������GOOSE֡����Ϣ
};

///
/// @brief
///    ����״̬���Խ������
///
class CStateTestResult
{
    friend class CFrameSend;
    friend class CFrameSendFt3;
    friend class CFrameSendNet;

public:
    CStateTestResult(enumTestType eType);
    ~CStateTestResult(void);

    ///
    /// @brief
    ///    ��ʼ��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool Init();

    ///
    /// @brief
    ///    ��ò��Ե�����
    ///
    /// @return
    ///     enumTestType    ���Ե�����
    ///
    enumTestType GetTestType();

    ///
    /// @brief
    ///    �ͷ����м�¼ͨ����ʷֵ�Ķ���
    ///
    void ClearAllRecvValues();

    ///
    /// @brief
    ///    ���ָ������ͨ�����ı���ͨ������ֵ�Ķ���
    ///
    /// @param[in]  strName    ����ͨ����
    ///
    /// @return
    ///     CChannelRecvValues*    ����ͨ������ֵ�ö���
    ///
    CChannelRecvValues* GetChannelRecvValues(std::wstring strName);

    ///
    /// @brief
    ///    ���״̬��ʼ���Ե�ʱ��
    ///
    /// @return
    ///     std::wstring    ��ʼ���Ե�ʱ��
    ///
    std::wstring GetStartTime();

    ///
    /// @brief
    ///    ����״̬��ʼʱ�䣨���ʱ�䣩
    ///
    /// @param[in]  stNewTime    ״̬��ʼʱ��
    ///
    void SetStartTime(CEventTime stNewTime);

    ///
    /// @brief
    ///    ö�ٵ�һ����ӳ���SMV��������ͨ��
    ///
    /// @return
    ///     CVirtualChannel*    ָ������ͨ�������ָ�룬ʧ��ʱ����NULL
    ///
    CSMVSendVirtualChannel* FirstSMVSendVirtualChannel();

    ///
    /// @brief
    ///    �����һ����ӳ���SMV��������ͨ��
    ///
    /// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
    ///
    /// @return
    ///     CVirtualChannel*    ָ������ͨ�������ָ�룬ʧ��ʱ����NULL
    ///
    CSMVSendVirtualChannel* NextSMVSendVirtualChannel(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    ö�ٵ�һ��GOOSE���Ϳ��ƿ�
    ///
    /// @return
    ///     CGooseCb*    �ɹ�ʱ����GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CGooseCb* FirstGooseSendCb();

    ///
    /// @brief
    ///    ö����һ��GOOSE���Ϳ��ƿ�
    ///
    /// @param[in]  pCurGocb ��ǰλ��GOOSE���ƿ��ָ��
    ///
    /// @return
    ///     CGooseCb*    �ɹ�ʱ����GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CGooseCb* NextGooseSendCb(CGooseCb* pCurGocb);

    ///
    /// @brief
    ///    ���ָ��GOOSE���ƿ��������
    ///
    /// @param[in]  pCurGocb    ָ����ƿ�����ָ��
    ///
    /// @return
    ///     int    ���ƿ�������ʧ�ܷ���-1
    ///
    int GetIndexByGooseCb(CGooseCb* pCurGocb);

    ///
    /// @brief
    ///    �����������GOOSE���ƿ�
    ///
    /// @param[in]  nCurIndex    ���ƿ�����
    ///
    /// @return
    ///     CGooseCb*    �ɹ�ʱ����GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CGooseCb* GetGooseCbByIndex(int nCurIndex);

    ///
    /// @brief
    ///    ����ֶ������GOOSE���ƿ�
    ///
    void ClearGooseSendCbs();

    ///
    /// @brief
    ///    ��ѯSMVͨ��ֵ�Ƿ����仯
    ///
    /// @return
    ///     bool    true��ʾ�����˱仯
    ///
    bool IsSmvChannelChanged() const;

    ///
    /// @brief
    ///    ����SMVͨ��ֵ�Ƿ����˱仯
    ///
    /// @param[in]  val    �Ƿ����˱仯
    ///
    void SetSmvChannelChanged(bool val);

    ///
    /// @brief
    ///    ��ѯGOOSEͨ��ֵ�Ƿ����仯
    ///
    /// @return
    ///     bool    true��ʾ�����˱仯
    ///
    bool IsGooseChannelChanged() const;

    ///
    /// @brief
    ///    ����GOOSEͨ��ֵ�Ƿ����˱仯
    ///
    /// @param[in]  val    �Ƿ����˱仯
    ///
    void SetGooseChannelChanged(bool val);

    ///
    /// @brief
    ///    ��ѯ9-1��FT3��FT3��չ���ƿ��״̬���Ƿ�ı�
    ///
    /// @return
    ///     bool    true��ʾ�����˱仯
    ///
    bool IsStatusWordChanged() const;

    ///
    /// @brief
    ///    ����9-1��FT3��FT3��չ���ƿ��״̬���Ƿ����˸ı�
    ///
    /// @param[in]  val    �Ƿ����˸ı�
    ///
    void SetStatusWordChanged(bool val);

    ///
    /// @brief
    ///    ��ѯ9-2ͨ����Ʒ���Ƿ�ı�
    ///
    /// @return
    ///     bool    true��ʾ�����˱仯
    ///
    bool IsQualityChanged() const;

    ///
    /// @brief
    ///    ����9-2ͨ����Ʒ���Ƿ����˸ı�
    ///
    /// @param[in]  val    �Ƿ����˸ı�
    ///
    void SetQualityChanged(bool val);

    ///
    /// @brief
    ///    ���SMV����֡������
    ///
    void ClearSmvSendBuffers();

    ///
    /// @brief
    ///    ���GOOSE����֡������
    ///
    void ClearGooseSendBuffers();

    ///
    /// @brief
    ///    ��ѯ�Ƿ�����˲���
    ///
    /// @return
    ///     bool    ��ɲ��Է���true
    ///
    bool IsTested() const;

    ///
    /// @brief
    ///     ���ò�����ɱ�ʶ
    ///
    /// @param[in]  val     �Ƿ���ɲ���
    ///
    void SetTested(bool val);

protected:
    ///
    /// @brief
    ///    �����ֶ������SMV��������ͨ����ֵ
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool __SaveManualTestSMVSendValues();

    ///
    /// @brief
    ///    �����GOOSE���Ϳ��ƿ��ֵ
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool __SaveGooseSendCbs();

    ///
    /// @brief
    ///    ����ֶ�����SMV������ͨ����ֵ
    ///
    /// @return
    ///     CStatus*    ��������ͨ��ֵ�Ķ���
    ///
    CStatus* __GetSMVSendValues();

    ///
    /// @brief
    ///    ���ֶ�����SMV���͵�����ͨ��ֵ��ָ����״̬����������
    ///
    /// @param[in]  eCompareType    �ȽϷ�ʽ
    /// @param[in]  pOldStatusTestResult    ָ��״̬��������ָ��
    ///
    /// @return
    ///     bool    ��ͬ����true
    ///
    bool __CompareSMVSendValues(enumCompareType eCompareType, CStateTestResult* pOldStatusTestResult);

    ///
    /// @brief
    ///    ���ֶ�����GOOSE���͵Ŀ��ƿ���ָ����״̬����������
    ///
    /// @param[in]  pOldStatusTestResult    ָ��״̬��������ָ��
    ///
    /// @return
    ///     bool    ��ͬ����true
    ///
    bool __CompareGooseSendValues(CStateTestResult* pOldStateTestResult);

    ///
    /// @brief
    ///    �޸�GOOSE���ƿ��е���/˫������ͨ��ʱ����Ҫ�Զ��޸�����/�����ʱ��ͨ����ֵ
    ///    �˺����Զ�Ѱ��Ѱ�ұ�λ�ĵ���/˫��ͨ����Ȼ��ı����ڻ�����ʱ��ͨ����ֵ
    ///
    /// @param[in]  stTestStartTime    ����ʵ��Ŀ�ʼʱ��
    ///
    void __UpdateGooseChannelTime(CEventTime stTestStartTime);

    ///
    /// @brief
    ///     ����GOOSE���ƿ�����Ҫ��λ��ʱ��ͨ��
    ///
    void __SaveGooseCbTimeChannel();

    ///
    /// @brief
    ///     ����SMV���ͻ���
    ///
    /// @param[in]  pOldStateTestResult     �ϵ�״̬���Խ����
    ///
    /// @return
    ///     bool    �����ɹ�����true
    ///
    bool __CopySmvSendBuffers(CStateTestResult* pOldStateTestResult);

    ///
    /// @brief
    ///     ����Goose���ͻ���
    ///
    /// @param[in]  pOldStateTestResult     �ϵ�״̬���Խ����
    ///
    /// @return
    ///     bool    �����ɹ�����true
    ///
    bool __CopyGooseSendBuffers(CStateTestResult* pOldStateTestResult);

private:
    enumTestType                        m_eTestType;                    ///< ���Ե�����

    bool                                m_bTested;                      ///< �Ƿ�����˲���
    bool                                m_bSmvChannelChanged;           ///< SMVͨ��ֵ�Ƿ����仯
    bool                                m_bGooseChannelChanged;         ///< GOOSEͨ��ֵ�Ƿ����仯
    bool                                m_bStatusWordChanged;           ///< 9-1��FT3��FT3��չ���ƿ��״̬���Ƿ�ı�
    bool                                m_bQualityChanged;              ///< 9-2ͨ����Ʒ���Ƿ�ı�

    CEventTime                          m_stStateStartTime;             ///< ��ǰ״̬��ʼ��ʱ�䣨���ʱ�䣩
    CStatus                             m_stSMVSendValues;              ///< �ֶ�����SMV������ͨ���ķ���ֵ
    std::list<CGooseCb *>               m_listGooseSendCbs;             ///< �ֶ�����GOOSE���͵ĸ������ƿ�

    std::list<CStateSendBuffer*>        m_listSmvSendBuffers;           ///< ����SMV�����͵�����֡
    std::list<CStateSendBuffer*>        m_listGooseSendBuffers;         ///< ����GOOSE�����͵�����֡

    std::map<std::wstring, CChannelRecvValues*> m_mapChannelRecvValues; ///< ��Ÿ�DIͨ������ֵ������
};
#endif
