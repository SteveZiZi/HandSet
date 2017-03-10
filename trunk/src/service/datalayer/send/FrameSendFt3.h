/// @file
///
/// @brief
///     Ft3���ݷ�����
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

#ifndef __FRAME_SEND_FT3_H__
#define __FRAME_SEND_FT3_H__
#include "FrameSendStruct.h"
#include "StateTestResult.h"
#include "src\service\datalayer\config\FT3Cb.h"

class CFrameSend;
class CFrameSendFt3
{
public:
    CFrameSendFt3(CFrameSend* pFrameSend, HANDLE hGPMDevice, DsmPortName enPortName);
    ~CFrameSendFt3(void);

public:
    ///
    /// @brief
    ///    ��ʼ��Ft3����
    ///
    bool InitFt3(const wchar_t *pNotifyEvtName, const wchar_t *pMapFileName, uint32_t nMapFileLen );

    ///
    /// @brief
    ///    ����ʼ��FT3����
    ///
    void UnInitFt3();

    ///
    /// @brief
    ///    ���ô����͵�SMV���ƿ�
    ///
    /// @param[in]  pNewCb    SMV���ƿ�
    ///
    void SetSMVSendCb(CFT3Cb* pNewCb);

    ///
    /// @brief
    ///    Ԥ�ȼ���˿ڵ�SMV����֡
    ///
    /// @param[in]  stDspRecvData       ����ͨ��һ���������εĲ���ֵ
    /// @param[out] pStateTestResult    �������������֡�Ľṹ��
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
    ///     enumErrorCode   ���ͳɹ�����ERROR_CODE_SUCCESS
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
    ///    ֹͣFt3���ݷ���
    ///
    bool StopFt3Send();

protected:
    ///
    /// @brief
    ///    ���Ft3��������
    ///
    /// @param[out]  stTxCfg    ����������Ϣ�Ľṹ��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool __GetFt3SendCfg(FT3_TX_CFG& stTxCfg);

    ///
    /// @brief
    ///    ���FT3���ķ��ͻ�����
    ///
    /// @param[out]  pDataBuffer        �����ͻ�����
    /// @param[in]   uiDataBufferLen    ���ͻ�������С���ֽڣ�
    /// @param[in]   stDspRecvData      ����ͨ��һ���������εĲ���ֵ
    /// @param[out]  uiFramesTotalLen   ���д�����֡�ĳ��Ⱥ�
    ///
    /// @return
    ///     enumErrorCode   �ɹ�����ERROR_CODE_SUCCESS
    ///
    enumErrorCode __FillFt3SendBuffer(unsigned char* pDataBuffer, unsigned int uiDataBufferLen, CDspRecvData& stDspRecvData, uint32_t& uiFramesTotalLen);

private:
    CFrameSend*                         m_pDataSendManager;             ///< ���ݷ��͹���������ָ��
    HANDLE                              m_hGPMDevice;                   ///< GPMC���߾��
    DsmPortName                         m_enPortName;                   ///< Ft3ʵ����Ӧ��ʵ�ʶ˿�
    bool                                m_bIsInited;                    ///< Ft3�����Ƿ��Ѿ���ʼ��

    HANDLE                              m_hFileMaping;                  ///< ���������ĵ��ڴ���ļ�ӳ����
    Map_File_Header*                    m_pMapHeader;                   ///< ָ���ڴ�ӳ���ļ�����ǰ��
    unsigned char*                      m_pMapDataBuffer;               ///< ָ���ڴ�ӳ���ļ������ݻ�����

    NanRuiFT3Msg*                       m_pNanRuiFT3Msg;                ///< ����FT3֡������
    GuoWangFT3Msg*                      m_pGuoWangFT3Msg;               ///< ����FT3֡������

    uint16_t                            m_usNanRuiBuffer[NANRUI_FT3_MSG_WORD];      ///< ���������֡�Ļ�����
    uint16_t                            m_usGuoWangBuffer[GUOWANG_FT3_MSG_WORD];    ///< ��Ź�������֡�Ļ�����
    uint16_t                            m_usChannelValArr[GUOWANG_FT3_MSG_WORD];    ///< ���ͨ��ֵ������

    CFT3Cb*                             m_pSMVSendCb;                   ///< �����͵�SMV���ƿ�
};
#endif
