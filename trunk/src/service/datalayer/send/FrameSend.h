/// @file
///
/// @brief
///     ������FPGADE������������ݷ��Ͳ���
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.3.19
///
/// �޸���ʷ��
///     Modified by:    chao
///     Modified date:  2013.6.19
///     Version:        1.0.1
///     Descriptions:   ����˷��͹���
///

#ifndef __FRAME_SEND_H__
#define __FRAME_SEND_H__
#include <afxmt.h>
#include "FrameSendFt3.h"
#include "FrameSendNet.h"
#include "StateTestResult.h"
#include "HardwareTimer.h"
#include "src\service\datalayer\config\Status.h"

class CFrameSend
{
    friend class CFrameSendFt3;
    friend class CFrameSendNet;

protected:
    CFrameSend();
    ~CFrameSend(void);

public:
    ///
    /// @brief
    ///    ��ȡ���͹�����ʵ��
    ///
    static CFrameSend* getInstance();

    ///
    /// @brief
    ///    ��ʼ�����͹�����
    ///
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool InitFrameSend(HANDLE hGPMDevice);

    ///
    /// @brief
    ///    ж�ط��͹�����
    ///
    void UnInitFrameSend();

    ///
    /// @brief
    ///    �Ƿ��ѳ�ʼ��
    ///
    ///
    /// @return
    ///     bool    �Ƿ��ѳ�ʼ��
    ///
    bool IsInited();

    ///
    /// @brief
    ///     ���GPMC���߾��
    ///
    /// @return
    ///     HANDLE    GPMC���߾��
    ///
    HANDLE GetGPMDevice();

    ///
    /// @brief
    ///    ���֪ͨ����
    ///
    ///
    /// @return
    ///     HWND    ���ھ��
    ///
    HWND GetNotifyHwnd();

    ///
    /// @brief
    ///    ���SMV���͵�״̬
    ///
    ///
    /// @return
    ///     eSendStatus    SMV����״̬
    ///
    eSendStatus GetManualTestSMVSendStatus();

    ///
    /// @brief
    ///    ���GOOSE�ķ���״̬
    ///
    ///
    /// @return
    ///     eSendStatus    GOOSE�ķ���״̬
    ///
    eSendStatus GetManualTestGooseSendStatus();

    ///
    /// @brief
    ///    ��õ�ǰ���ڽ��е��������ͣ��ֶ����顢״̬���У�
    ///
    /// @return
    ///     eFunctionType    ��ǰ���ڽ��е���������
    ///
    eFunctionType GetCurFunctionType();

    ///
    /// @brief
    ///    ��ʼ�ֶ�����SMV����
    ///
    /// @param[in]  hWnd    hWnd  ���ھ������DIֵ�����仯ʱ���ᷢ��WM_DI_CHANGED��Ϣ���˴���
    ///
    /// @return
    ///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
    ///
    enumErrorCode StartManualTestSMV(HWND hWnd);

    ///
    /// @brief
    ///    �����ֶ�����SMV���Ե�����
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool UpateManualTestSMV();

    ///
    /// @brief
    ///    ֹͣ�ֶ�����SMV����
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool StopManualTestSMV();

    ///
    /// @brief
    ///    ��ʼ�ֶ�����GOOSE����
    ///
    /// @param[in]  hWnd    hWnd  ���ھ������DIֵ�����仯ʱ���ᷢ��WM_DI_CHANGED��Ϣ���˴���
    ///
    /// @return
    ///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
    ///
    enumErrorCode StartManualTestGoose(HWND hWnd);

    ///
    /// @brief
    ///    �����ֶ�����GOOSE���Ե�����
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool UpateManualTestGoose();

    ///
    /// @brief
    ///    ֹͣ�ֶ�����GOOSE����
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool StopManualTestGoose();

    ///
    /// @brief
    ///    ����ֶ�������������
    ///
    /// @return
    ///     unsigned int    �ֶ�������������
    ///
    unsigned int GetManualTestResultCount();

    ///
    /// @brief
    ///    ���ָ�������ŵ��ֶ�������������
    ///
    /// @param[in]  nIndex    �����������(��0��ʼ����)
    ///
    /// @return
    ///     CStateTestResult*    ���ָ��״̬���Խ���Ķ���ʧ��ʱ����NULL
    ///
    CStateTestResult* GetManualTestResult(unsigned int nIndex);

    ///
    /// @brief
    ///    ���ָ���ֶ��������������������
    ///
    /// @param[in]  pCurResult    �ֶ�������������
    ///
    /// @return
    ///     int    ����������ţ���0��ʼ������ʧ��ʱ����-1
    ///
    int GetManualTestResultIndex(CStateTestResult* pCurResult);

    ///
    /// @brief
    ///    ��õ�һ���ֶ�������������
    ///
    /// @return
    ///     CStateTestResult*    ָ�����������ָ�룬ʧ��ʱ����NULL
    ///
    CStateTestResult* FirstManualTestResult();

    ///
    /// @brief
    ///    �����һ���ֶ�������������
    ///
    /// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
    ///
    /// @return
    ///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
    ///
    CStateTestResult* NextManualTestResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    �����һ���ֶ�������������
    ///
    /// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
    ///
    /// @return
    ///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
    ///
    CStateTestResult* PrevManualTestResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    ������һ���ֶ�������������
    ///
    /// @return
    ///     CStateTestResult*    ָ�����������ָ�룬ʧ��ʱ����NULL
    ///
    CStateTestResult* LastManualTestResult();

    ///
    /// @brief
    ///    ��õ�һ������SMV����ͨ����Ϣ���ֶ�������������
    ///
    /// @return
    ///     CStateTestResult*    ָ�����������ָ�룬ʧ��ʱ����NULL
    ///
    CStateTestResult* FirstManualTestSMVResult();

    ///
    /// @brief
    ///    �����һ������SMV����ͨ����Ϣ���ֶ�������������
    ///
    /// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
    ///
    /// @return
    ///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
    ///
    CStateTestResult* NextManualTestSMVResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    �����һ������SMV����ͨ����Ϣ���ֶ�������������
    ///
    /// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
    ///
    /// @return
    ///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
    ///
    CStateTestResult* PrevManualTestSMVResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    ������һ������SMV����ͨ����Ϣ���ֶ�������������
    ///
    /// @return
    ///     CStateTestResult*    ָ�����������ָ�룬ʧ��ʱ����NULL
    ///
    CStateTestResult* LastManualTestSMVResult();

    ///
    /// @brief
    ///    ��õ�һ������GOOSE���ƿ���ֶ�������������
    ///
    /// @return
    ///     CStateTestResult*    ָ�����������ָ�룬ʧ��ʱ����NULL
    ///
    CStateTestResult* FirstManualTestGooseResult();

    ///
    /// @brief
    ///    �����һ������GOOSE���ƿ���ֶ�������������
    ///
    /// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
    ///
    /// @return
    ///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
    ///
    CStateTestResult* NextManualTestGooseResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    �����һ������GOOSE���ƿ���ֶ�������������
    ///
    /// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
    ///
    /// @return
    ///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
    ///
    CStateTestResult* PrevManualTestGooseResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    ������һ������GOOSE���ƿ���ֶ�������������
    ///
    /// @return
    ///     CStateTestResult*    ָ�����������ָ�룬ʧ��ʱ����NULL
    ///
    CStateTestResult* LastManualTestGooseResult();

    ///
    /// @brief
    ///    ������е��ֶ���������
    ///
    void ClearAllManualTestResult();

    ///
    /// @brief
    ///    ��ʼ״̬���в���
    ///
    /// @param[in]  hWnd  ���ھ������DIֵ�����仯ʱ���ᷢ��WM_DI_CHANGED��Ϣ���˴���
    ///                             ����Ҫѯ���û��Ƿ��ֶ��л�ʱ���ᷢ��WM_WHETHER_SWITCH��Ϣ���˴���
    ///                             ��״̬�л��󣬻ᷢ��WM_STATUS_CHANGED��Ϣ���˴���
    ///                             ��������ɺ󣬻ᷢ��WM_TEST_COMPLETED��Ϣ���˴���
    ///
    /// @return
    ///     enumErrorCode   ���������̳߳ɹ�����ERROR_CODE_SUCCESS
    ///
    enumErrorCode StartStateSequenceTest(HWND hWnd);

    ///
    /// @brief
    ///    ֹͣ״̬���в���
    ///
    bool StopStateSequenceTest();

    ///
    /// @brief
    ///    ��õ�ǰ���ڷ��͵ڼ���״̬����0��ʼ������
    ///
    int GetCurStateIndex();

    ///
    /// @brief
    ///    ���״̬���н��������
    ///
    /// @return
    ///     unsigned int    ״̬���н��������
    ///
    unsigned int GetStateSequenceResultCount();

    ///
    /// @brief
    ///    ���ָ��״̬�ŵ�״̬���н��������
    ///
    /// @param[in]  iStateIndex    ״̬��(��0��ʼ����)
    ///
    /// @return
    ///     CStateTestResult*    ���ָ��״̬���Խ���Ķ���ʧ��ʱ����NULL
    ///
    CStateTestResult* GetStateSequenceResult(unsigned int iStateIndex);

    ///
    /// @brief
    ///    ������е�״̬���н����
    ///
    void ClearAllStateSequenceResult();

    ///
    /// @brief
    ///     ����ֶ�ʵ��Ŀ�ʼʱ��
    ///
    /// @return
    ///     CEventTime    ��ʼʱ��
    ///
    CEventTime GetManualTestStartTime();

    ///
    /// @brief
    ///     ���״̬���в��ԵĿ�ʼʱ��
    ///
    /// @return
    ///     CEventTime    ��ʼʱ��
    ///
    CEventTime GetStateSequenceStartTime();

    ///
    /// @brief
    ///    ��õ�ǰ��FPGAʱ��
    ///
    /// @param[out]  stCurFpgaTime    ���FPGAʱ��Ķ���
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool GetCurrentFpgaTime(CEventTime& stCurFpgaTime);

    ///
    /// @brief
    ///    ���һ����־��¼
    ///
    /// @param[in]  strText    ������
    /// @param[in]  ulTime    ʱ��ֵ
    ///
    void AddLogs(std::string strText, unsigned long ulTime);

    ///
    /// @brief
    ///    ˢ�»���������־��¼����־�ļ���
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool FlushLogs();

protected:
    ///
    /// @brief
    ///    �����ֶ�����SMV�ķ���״̬
    ///
    /// @param[in]  eNewStatus    �µ�SMV����״̬
    ///
    void __SetManualTestSMVSendingStatus(eSendStatus eNewStatus);

    ///
    /// @brief
    ///    �����ֶ�����Goose�ķ���״̬
    ///
    /// @param[in]  eNewStatus    �µ�GOOSE����״̬
    ///
    void __SetManualTestGooseSendStatus(eSendStatus eNewStatus);

    ///
    /// @brief
    ///    ���õ�ǰ�����ڽ��е���������
    ///
    /// @param[in]  eNewType    �µ���������
    ///
    void __SetCurFunctionType(eFunctionType eNewType);

    ///
    /// @brief
    ///    ���ɱ�ӳ������ͨ��һ���������εĲ���ֵ
    ///
    /// @param[in]  pStatus         ָ��һ��״̬�����ָ��
    /// @param[out] stDspRecvData   ��ż�����Ĳ���ֵ�Ľṹ��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool __GenVirtualChannelSamples(CStatus* pStatus, CDspRecvData& stDspRecvData);

    ///
    /// @brief
    ///    ���SMV����Ƶ��
    ///
    /// @return
    ///     int    ����Ƶ��
    ///
    int __GetSampleFrequency();

    ///
    /// @brief
    ///    ���õ�ǰ���ڷ��͵ĵڼ���״̬(��0��ʼ����)
    ///
    /// @param[in]  iNewStateIndex    
    ///
    void __SetCurStateIndex(int iNewStateIndex);

    ///
    /// @brief
    ///    ���һ���ֶ�������Խ��������������
    ///
    /// @param[in]  eType    ���Ե�����
    ///
    /// @return
    ///     CStateTestResult*    CStateTestResult*    �ɹ�ʱ����״̬���Խ�������ָ�룬ʧ��ʱ����NULL
    ///
    CStateTestResult* __AddManualTestResult(enumTestType eType);

    ///
    /// @brief
    ///    ɾ��ָ�����ֶ�������Խ������
    ///
    /// @param[in]  pDeleteTestResult    ��ɾ�����ֶ�������Խ������
    ///
    void __DeleteManualTestResult(CStateTestResult* pDeleteTestResult);

    ///
    /// @brief
    ///    ���һ��״̬���в��Խ��������������
    ///
    /// @return
    ///     CStateTestResult*    �ɹ�ʱ����״̬���Խ�������ָ�룬ʧ��ʱ����NULL
    ///
    CStateTestResult* __AddStateSequenceResult();

    ///
    /// @brief
    ///    ����SMV���ƿ鵽�������Ͷ˿�
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool __DistributeSmvCbs();

    ///
    /// @brief
    ///    ����GOOSE���ƿ鵽�������Ͷ˿�
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool __DistributeGooseCbs();

    ///
    /// @brief
    ///    ����GOOSE���ƿ鵽�������Ͷ˿�
    ///
    /// @param[in]  pStateTestResult    ָ�����������ָ�룬�������˴����͵�GOOSE���ƿ�
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool __DistributeGooseCbs(CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///    ׼��SMV����֡
    ///
    /// @param[in]  pStateTestResult    ָ�����������ָ�룬���ڱ���SMV����֡
    ///
    /// @return
    ///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
    ///
    enumErrorCode __PrepareSmvSendBuffer(CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///    ����SMV���ƿ�
    ///
    /// @param[in]  stFunctionType  �������ͣ��ֶ����顢״̬���У�
    /// @param[in]  pStateTestResult    ָ�����������ָ��
    ///
    /// @return
    ///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
    ///
    enumErrorCode __SendSMVCbs(eFunctionType stFunctionType, CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///    ����SMV����֡
    ///
    /// @param[in]  stFunctionType  ��������(�ֶ����顢״̬����)
    /// @param[in]  pStateTestResult    ָ�����������ָ��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool __UpdateSMVSend(eFunctionType stFunctionType, CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///    ֹͣSMV����
    ///
    bool __StopSMVSend();

    ///
    /// @brief
    ///    ׼��GOOSE���ƿ鷢��֡
    ///
    /// @param[in]  pStateTestResult    ָ�����������ָ�룬���ڱ���GOOSE����֡
    ///
    /// @return
    ///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
    ///
    enumErrorCode __PrepareGooseSendBuffer(CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///    ׼��GOOSE���ƿ����֡
    ///
    /// @param[in]  pStateTestResult    ָ�����������ָ�룬���ڱ���GOOSE����֡
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool __PrepareGooseUpdateBuffer(CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///    ����GOOSE���ƿ�
    ///
    /// @param[in]  stFunctionType      �������ͣ��ֶ����顢״̬���У�
    /// @param[in]  pStateTestResult    ����״̬���Խ���Ķ����ڲ������д����͵�GOOSE���ƿ�
    ///
    /// @return
    ///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
    ///
    enumErrorCode __SendGooseCbs(eFunctionType stFunctionType, CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///    ����GOOSE����֡
    ///
    /// @param[in]  stFunctionType  �������ͣ��ֶ����顢״̬���У�
    /// @param[in]  pStateTestResult    ����״̬���Խ���Ķ����ڲ������д����͵�GOOSE���ƿ�
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool __UpdateGooseSend(eFunctionType stFunctionType, CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///    ֹͣGOOSE����
    ///
    bool __StopGooseSend();

    ///
    /// @brief
    ///    ������ƿ�������ͨ����ƫ����
    ///
    /// @param[in]  pStateTestResult    ָ�����������ָ��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool __CalcChannelOffset(CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///     �������Ƶ�SMV����״̬�������ظ�״̬��Ӧ�ķ��ͻ���
    ///
    /// @param[in]  pCurStateTestResult ��ǰSMV����״̬��Ӧ�Ĳ��Խ����
    /// @param[in]  iStateIndex         ��ǰSMV����״̬��������
    ///
    /// @return
    ///     CStateTestResult*   ���Ʒ���״̬�Ĳ��Խ������ʧ��ʱ����NULL
    ///
    CStateTestResult* __FindSimilarSmvTestResult(CStateTestResult* pCurStateTestResult, int iStateIndex);

private:
    HANDLE                              m_hGPMDevice;                   ///< GPMC���߾��
    bool                                m_bIsInited;                    ///< �Ƿ��Ѿ���ʼ��
    CHardwareTimer                      m_objHardwareTimer;             ///< Ӳ����ʱ��

    CFrameSendFt3*                      m_pFrameFt3C;                   ///< �⴮��C�����ͱ���
    CFrameSendNet*                      m_pFrameNetA;                   ///< ����̫��A
    CFrameSendNet*                      m_pFrameNetB;                   ///< ����̫��B
    CFrameSendNet*                      m_pFrameNetC;                   ///< ����̫��

    eSendStatus                         m_eManualTestSMVSendStatus;     ///< �ֶ�����SMV����״̬
    eSendStatus                         m_eManualTestGooseSendStatus;   ///< �ֶ�����GOOSE����״̬

    int                                 m_iCurStateIndex;               ///< ��ǰ���ڷ��͵ĵڼ���״̬��ÿһ���޸��ֶ������ֵ��Ҳ�൱�ڲ�����һ����״̬��
    std::vector<CStateTestResult*>      m_vecManualTestResult;          ///< ����ֶ�����GOOSE����ֵ������
    std::vector<CStateTestResult*>      m_vecStateSequenceResult;       ///< ���״̬����GOOSE����ֵ������

    eFunctionType                       m_eCurFunctionType;             ///< ��ǰ���ڽ��е��������ͣ��ֶ����顢״̬���У�
    CCriticalSection                    m_criticalFunctionType;         ///< ���������ٽ��������ڻ�����������͵Ķ�д

    CEventTime                          m_stManualTestStartTime;        ///< �ֶ����鿪ʼʱ��
    CEventTime                          m_stStateSequenceStartTime;     ///< ״̬���п�ʼʱ��

    CEvent                              m_eventCancelSMV;               ///< ����ָʾ�Ƿ���ʱȡ��SMV������֡
    CEvent                              m_eventCancelGoose;             ///< ����ָʾ�Ƿ���ʱȡ��GOOSE������֡
    CEvent                              m_eventCancelStateSequence;     ///< ����ָʾ�Ƿ���ʱȡ��״̬��������

    HWND                                m_hNotifyHwnd;                  ///< ���ھ������DI�仯ʱ����WM_STATUS_CHANGED��Ϣ���˴���
    std::vector<std::pair<std::string, unsigned long> >  m_listTextTime;
};
#endif
