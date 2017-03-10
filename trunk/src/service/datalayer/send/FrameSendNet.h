/// @file
///
/// @brief
///     �������ݷ�����
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.3.23
///
/// �޸���ʷ��
///     Modified by:    chao
///     Modified date:  2013.6.19
///     Version:        1.0.1
///     Descriptions:   ����˷��͹���
///

#ifndef __FRAME_SEND_NET_H__
#define __FRAME_SEND_NET_H__
#include "FrameSendStruct.h"
#include "StateTestResult.h"
#include "src\service\datalayer\config\GooseCb.h"
#include "src\service\datalayer\config\IEC9_1Cb.h"
#include "src\service\datalayer\config\IEC9_2Cb.h"

#define ETHER_IEC91_CHANNEL_MAX           (ETHER_FRAME_MAX_LEN/2)       ///< IEC61850-9-1���ͨ����
#define ETHER_IEC92_CHANNEL_MAX           (ETHER_FRAME_MAX_LEN/4)       ///< IEC61850-9-2���ͨ����

class CFrameSend;
class CFrameSendNet
{
public:
    CFrameSendNet(CFrameSend* pFrameSend, HANDLE hGPMDevice, DsmPortName enPortName);
    ~CFrameSendNet(void);

public:
    ///
    /// @brief
    ///    ��ʼ��Net����
    ///
    bool InitNet(const wchar_t *pNotifyEvtName, const wchar_t *pMapFileName, uint32_t nMapFileLen );

    ///
    /// @brief
    ///    ����ʼ��Net����
    ///
    void UnInitNet();

    ///
    /// @brief
    ///    ��ձ��������SMV���ƿ������
    ///
    void RemoveAllSMVSendCbs();

    ///
    /// @brief
    ///    ��Ӵ����͵�SMV���ƿ�
    ///
    /// @param[in]  pNewCb    SMV���ƿ�
    ///
    /// @return
    ///     bool    ��ӳɹ�����true
    ///
    bool AddSMVSendCb(CBaseCb* pNewCb);

    ///
    /// @brief
    ///    ��ձ��������GOOSE���ƿ������
    ///
    void RemoveAllGOOSESendCbs();

    ///
    /// @brief
    ///    ��Ӵ����͵�GOOSE���ƿ�
    ///
    /// @param[in]  pNewCb    GOOSE���ƿ�
    ///
    /// @return
    ///     bool    ��ӳɹ�����true
    ///
    bool AddGooseSendCb(CGooseCb* pNewCb);

    ///
    /// @brief
    ///    Ԥ�ȼ���˿ڵ�SMV����֡
    ///
    /// @param[in]  stDspRecvData    ����ͨ��һ���������εĲ���ֵ
    /// @param[out] pStateTestResult �������������֡�Ľṹ��
    ///
    /// @return
    ///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
    ///
    enumErrorCode PrepareSmvSendBuffer(CDspRecvData& stDspRecvData, CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///    ����SMV���ƿ�
    ///
    /// @param[in]  stDspRecvData    ����ͨ��һ���������εĲ���ֵ
    ///
    /// @return
    ///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
    ///
    enumErrorCode SendSMVCbs(CDspRecvData& stDspRecvData);

    ///
    /// @brief
    ///    ���ͻ������е�����֡
    ///
    /// @param[in]  pStateSendBuffer    ָ�򻺳�����ָ��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool SendSMVCbs(CStateSendBuffer* pStateSendBuffer);

    ///
    /// @brief
    ///     ����SMV����֡
    ///
    /// @param[in]  stDspRecvData   ����ͨ��һ���������εĲ���ֵ
    ///
    /// @return
    ///     bool    ���ͳɹ�����true
    ///
    bool UpdateSMVFrames(CDspRecvData& stDspRecvData);

    ///
    /// @brief
    ///    ����SMV�������е�����֡
    ///
    /// @param[in]  pStateSendBuffer    ָ�򻺳�����ָ��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool UpdateSMVFrames(CStateSendBuffer* pStateSendBuffer);

    ///
    /// @brief
    ///    ֹͣSMV�ײ����ݷ���
    ///
    bool StopSMVSend();

    ///
    /// @brief
    ///    ׼��GOOSE���ƿ鷢��֡
    ///
    /// @param[in]  pStateTestResult    ָ�����������ָ�룬���ڱ���GOOSE����֡
    ///
    /// @return
    ///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
    ///
    enumErrorCode PrepareGooseSendBuffer(CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///    ׼��GOOSE���ƿ����֡
    ///
    /// @param[in]  pStateSendBuffer    ָ��GOOSE����֡
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool PrepareGooseUpdateBuffer(CStateSendBuffer* pStateSendBuffer);

    ///
    /// @brief
    ///    ����GOOSE���ƿ�
    ///
    /// @return
    ///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
    ///
    enumErrorCode SendGooseCbs();

    ///
    /// @brief
    ///    ����GOOSE���ƿ�
    ///
    /// @param[in]  pStateSendBuffer    ָ�򻺳�����ָ��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool SendGooseCbs(CStateSendBuffer* pStateSendBuffer);

    ///
    /// @brief
    ///     ����GOOSE����֡
    ///
    /// @return
    ///     bool    ���ͳɹ�����true
    ///
    bool UpdateGooseFrames();

    ///
    /// @brief
    ///     ����GOOSE����֡
    ///
    /// @param[in]  pStateSendBuffer    ָ�򻺳�����ָ��
    ///
    /// @return
    ///     bool    ���ͳɹ�����true
    ///
    bool UpdateGooseFrames(CStateSendBuffer* pStateSendBuffer);

    ///
    /// @brief
    ///    ֹͣGOOSE�ײ����ݷ���
    ///
    bool StopGooseSend();

    ///
    /// @brief
    ///    ������ƿ�������ͨ����ƫ����
    ///
    /// @param[in]  pStateSendBuffer    ָ����֡��������ָ��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool CalcChannelOffset(CStateSendBuffer* pStateSendBuffer);

protected:
    ///
    /// @brief
    ///    ��䱨�ķ��ͻ�������IEC61850-9-1��
    ///
    /// @param[out]  pDataBuffer        �����ͻ�����
    /// @param[in]   uiDataBufferLen    ���ͻ�������С���ֽڣ�
    /// @param[in]   stDspRecvData      ����ͨ��һ���������εĲ���ֵ
    /// @param[out]  stMuSendParam      ����ӿ�DeviceIoControl()�����ݲ����ṹ��
    /// @param[out]  uiFramesTotalLen   ���д�����֡�ĳ��Ⱥͣ��ֽڣ�
    ///
    /// @return
    ///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
    ///
    enumErrorCode __FillIec91SendBuffer(unsigned char* pDataBuffer, unsigned int uiDataBufferLen, CDspRecvData& stDspRecvData, IOCTRL_IN_PARAM_NET_BIG& stMuSendParam, uint32_t& uiFramesTotalLen);

    ///
    /// @brief
    ///    ��䱨�ķ��ͻ�������IEC61850-9-2��
    ///
    /// @param[out]  pDataBuffer        �����ͻ�����
    /// @param[in]   uiDataBufferLen    ���ͻ�������С���ֽڣ�
    /// @param[in]   stDspRecvData      ����ͨ��һ���������εĲ���ֵ
    /// @param[out]  stMuSendParam      ����ӿ�DeviceIoControl()�����ݲ����ṹ��
    /// @param[out]  uiFramesTotalLen   ���д�����֡�ĳ��Ⱥͣ��ֽڣ�
    ///
    /// @return
    ///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
    ///
    enumErrorCode __FillIec92SendBuffer( unsigned char* pDataBuffer, unsigned int uiDataBufferLen, CDspRecvData& stDspRecvData, IOCTRL_IN_PARAM_NET_BIG& stMuSendParam, uint32_t& uiFramesTotalLen);

    ///
    /// @brief
    ///    ��䱨�ķ��ͻ�������GOOSE��
    ///
    /// @param[in]  pDataBuffer         �����ͻ�����
    /// @param[in]  uiDataBufferLen     ���ͻ�������С���ֽڣ�
    /// @param[in]  stUtcTime           ���ĸı�ʱ��
    /// @param[out] stMuSendParam       ����ӿ� DeviceIoControl���ݲ����ṹ��
    ///
    /// @return
    ///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
    ///
    enumErrorCode __FillGooseSendBuffer(unsigned char* pDataBuffer, unsigned int uiDataBufferLen, UTC_TIME_t& stUtcTime, IOCTRL_IN_PARAM_NET_BIG& stMuSendParam);

    ///
    /// @brief
    ///    ��䱨�ķ��ͻ�������GOOSE��
    ///
    /// @param[in]  pDataBuffer         �ݴ淢��֡�Ļ�����
    /// @param[in]  uiDataBufferLen     ����֡�������Ĵ�С���ֽڣ�
    /// @param[out] pNewSendBuffer      ָ����֡�����ָ�루���ڱ��淢��֡��Ϣ��
    /// @param[out] uiFramesTotalLen    ���д�����֡�ĳ��Ⱥͣ��ֽڣ�
    ///
    /// @return
    ///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
    ///
    enumErrorCode __FillGooseSendBuffer(unsigned char* pDataBuffer, unsigned int uiDataBufferLen, CStateSendBuffer* pNewSendBuffer, uint32_t& uiFramesTotalLen);

private:
    CFrameSend*                         m_pDataSendManager;             ///< ���ݷ��͹���������ָ��
    HANDLE                              m_hGPMDevice;                   ///< GPMC���߾��
    DsmPortName                         m_enPortName;                   ///< Ft3ʵ����Ӧ��ʵ�ʶ˿�
    bool                                m_bIsInited;                    ///< ��������Ƿ��Ѿ���ʼ����

    HANDLE                              m_hFileMaping;                  ///< ���������ĵ��ڴ���ļ�ӳ����
    Map_File_Header*                    m_pMapHeader;                   ///< ָ���ڴ�ӳ���ļ�����ǰ��
    unsigned char*                      m_pMapDataBuffer;               ///< ָ���ڴ�ӳ���ļ������ݻ�����
    
    Sav91Msg*                           m_pSav91Msg;                    ///< IEC61850-9-1����ֵ���ĵı����ӿ�
    Sav92Msg*                           m_pSav92Msg;                    ///< IEC61850-9-2����ֵ���ĵı����ӿ�
    GooseMsg*                           m_pGooseMsg;                    ///< GOOSE���ĵı����ӿ�

    uint8_t                             m_pucFrameBuffer[ETHER_FRAME_MAX_LEN];          ///< ������籨��֡�Ļ�����
    uint16_t                            m_pusIEC91ValArr[ETHER_IEC91_CHANNEL_MAX];      ///< ���IEC61850-9-1һ֡����������ͨ��ֵ�Ļ�����
    uint32_t                            m_puiIEC92ValArr[ETHER_IEC92_CHANNEL_MAX];      ///< ���IEC61850-9-2һ֡����������ͨ��ֵ�Ļ�����
    uint32_t                            m_puiIEC92QualityArr[ETHER_IEC92_CHANNEL_MAX];  ///< ���IEC61850-9-2һ֡����������ͨ��Ʒ��ֵ�û�����

    std::vector<CBaseCb*>               m_vecSMVSendCbs;                ///< �����͵�SMV���ƿ�
    std::vector<CGooseCb*>              m_vecGOOSESendCbs;              ///< �����͵�GOOSE���ƿ�
};
#endif
