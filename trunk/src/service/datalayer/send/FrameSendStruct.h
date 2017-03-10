///// @file
///
/// @brief
///     ���巢������֡�Ľṹ��������Ϣ
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
///     Descriptions:   ����˷��͡����սṹ��
///

#ifndef __FRAME_SEND_STRUCT_H__
#define __FRAME_SEND_STRUCT_H__
#include <map>
#include <string>
#include <Windows.h>
#include "src\include\stdint.h"
#include "src\dspclient\include\Dsp.h"
#include "src\include\p_wndusermsg_def.h"
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\include\ctrlcode_def.h"

#define WM_DI_CHANGED                   (PRIVATE_WND_USERID + 1)        ///< DI������ͨ����ֵ�����˱仯
#define WM_WHETHER_SWITCH               (PRIVATE_WND_USERID + 2)        ///< ��״̬��������ʱ���Ƿ��ֶ��л�����һ��״̬
#define WM_STATUS_CHANGED               (PRIVATE_WND_USERID + 3)        ///< ���л���״̬�����е���һ��״̬
#define WM_MANUAL_TEST_RESULT           (PRIVATE_WND_USERID + 4)        ///< �ֶ�������Խ��
#define WM_STATUS_SEQUENCE_RESULT       (PRIVATE_WND_USERID + 5)        ///< ״̬���в��Խ��

#define MAX_ASDU_NUM                    (8)                             ///< һ��SMV���ģ����֧��8��ASDU
#define TX_TEST_STYLE_INDEX             (19)                            ///< ����ģʽλ���������ֶ����顢״̬���У�

///
/// @brief
///    Ft3�������ݵ�������Ϣ,���ݸ�DeviceIOControlʹ��
///
struct FT3_TX_CFG{
    uint16_t        nFrameLength;                                       ///< һ�����Ͷ��������ܳ��ȣ�ֵΪ������ĿnFramesCnt �� ����/���� ����֡���ȵĳ˻�
    uint16_t        nFrameCount;                                        ///< һ����FPGA�д��Ͷ���֡����
    uint16_t        uFramesStyle;                                       ///< ��ǰFT3���ĵ�֡��ʽ,��ӦFt3CtlWordSend�ṹ��
    uint16_t        nReserve;
};

///
/// @brief
///    DSP���ݷ���
///
struct CDspSendData
{
    SmvGenData                      m_stSendData;                       ///< ���Ͳ���
    std::map<std::wstring, int>     m_mapChannelName2Index;             ///< ͨ��������>���� ��ӳ��
};

///
/// @brief
///    DSP���ݽ���
///
struct CDspRecvData
{
    SMV_GEN_DATA_CALC_RT            m_stRecvData;                       ///< ���ջ�����
    int                             m_iFrequency;                       ///< ����Ƶ��
    int                             m_iSmvCount;                        ///< ÿ�ܲ��Ĳ�������
    std::map<std::wstring, int>     m_mapChannelName2Index;             ///< ͨ��������>���� ��ӳ��
};

///
/// @brief
///    SMV��GOOSE����״̬
///
enum eSendStatus
{
    SEND_STATUS_STOPPED = 0,                                            ///< ��ֹͣ
    SEND_STATUS_PREPARE,                                                ///< ׼�����Ͳ���
    SEND_STATUS_SENDING                                                 ///< ���ڷ���
};

///
/// @brief
///    ����ģʽ���ֶ����顢״̬���У�
///
enum enumTxTestStyle
{
    TX_TEST_STYLE_MANUAL_TEST = 0,                                      ///< �ֶ����鷢��ģʽ
    TX_TEST_STYLE_STATUS_SEQUENCE                                       ///< ״̬���з���ģʽ
};

///
/// @brief
///    ������
///
enum enumErrorCode
{
    ERROR_CODE_SUCCESS = 0,                                             ///< �ɹ�
    ERROR_CODE_FAILURE,                                                 ///< ʧ��
    ERROR_CODE_CANCEL,                                                  ///< �û�ȡ��
    ERROR_SMV_TOO_MANY_CHANNEL_ASDU,                                    ///< SMV���ƿ��ͨ����̫���ASDU��̫��
    ERROR_GOOSE_TOO_MANY_CHANNEL,                                       ///< GOOSE���ƿ��ͨ����̫��
};

#endif //end __FRAME_SEND_STRUCT_H__