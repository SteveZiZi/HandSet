/// @file
///
/// @brief
///     ����һЩ������Ϣ�������ȫ����Ϣ�����޸�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.3.23
///
/// �޸���ʷ��
///

#ifndef __CONFIG_H_
#define __CONFIG_H_

///
/// @brief
///    ����RETAILMSG�Ƿ��ӡ������Ϣ��Ĭ��Debug�汾��ӡ��Release�汾����ӡ
///     OUTPUT_LOG_MSG Ϊ���ӡ������Ϣ,MSDN����ʾ˵ʹ��RETAILMSG�������ļ��ߴ�
///     ���Զ��� SHIP_BUILD �꣬��ֹRETAILMSG���ӡ��Ϣ
///     ���Դ�����Ϣʹ��RETAILMSG��ӡ����������Ϣ��ӡ����ͨ��GEN_LOG_MESSAGE�رյ������
///
#include <Dbgapi.h>
#define GEN_LOG_MESSAGE 0
#ifndef OUTPUT_LOG_MSG
#ifdef DEBUG
#define     OUTPUT_LOG_MSG      1
#else
#define     OUTPUT_LOG_MSG      0
#endif
#endif

#include <string>
#include "src\include\stdint.h"
#include "inc\thread_priority.h"

#define     DRIVER_BUFFER_USE_CPY   0                       ///< ʹ�������ṩ������ʱ���������ڴ��Ƿ񵥶�����һ��
#define     ETHERNET_MAC_LEN        6                       ///< ������̫��mac��ַ����
#define     SMV_GOOSE_APPID_LEN     2                       ///< ����smv��goose��appid����
#define     SMV_MAX_POINT_PER_WAVE  256                     ///< ����һ���ܲ�������������12.8k��Ӧ256��
#define     SMV_PORLARITY_CHECK_POSITIVE    1               ///< ���Լ����������
#define     SMV_PORLARITY_CHEKC_NEGATIVE    -1              ///< ���Լ����������

const uint32_t DSM_MAX_SMV_CTRL_BLK   = 6;                  ///< ��������һ�ο���ѡ��Ĳ���ֵ���ƿ���Ŀ��Ӧ�ò�ʹ��
const uint32_t DSM_MAX_GOOSE_CTRL_BLK = 10;                 ///< ��������һ�ο���ѡ���goose���ƿ���Ŀ��Ӧ�ò�ʹ��
const uint32_t DSM_MAX_CONFIG_CTRL_BLK = 2;                 ///< �������������ÿ��ƿ�����������һ�����ÿ��ƿ�����Ϊ1������ʱ���ƿ�����Ϊ2
const uint32_t DSM_MAX_OSCILLOGRAM_WAVE_COUNT = 10;         ///< ���岨����ʾʱ�����ݲ㻺��һ��ͨ���Ĳ����ܲ�����
const uint32_t DSM_DEFAULT_POINT_PER_WARE = 80;             ///< ����Ĭ�ϵ�һ���ܲ��Ĳ���������Ĭ��Ϊ80�㣬��Ӧ������4k
//���岨����ʾʱ�����ݲ㻺��һ�λ���Ĳ��������
const uint32_t DSM_DEFAULT_OSCILLOGRAM_WAVE_POINT = DSM_MAX_OSCILLOGRAM_WAVE_COUNT * DSM_DEFAULT_POINT_PER_WARE;

//����ʱ������������ƿ�������
const uint32_t DSM_DEFAUTL_PHASE_CHECK_DEQUE_SIZE = 80*50;
//����ʱ���͸�dsp��ͨ����Ŀ
#define PHASE_CHECK_CHANNELS    6

const uint32_t DSM_MU_RX_TIMEOUT_MIN        = 1;            ///< MU���ճ�ʱʱ����С1ms
const uint32_t DSM_MU_RX_TIMEOUT_MAX        = 21474;        ///< MU���ճ�ʱʱ�����21474ms

//һ��goose���ƿ鱣���ı�λ��ʷ��¼����
const uint32_t DSM_MAX_GOOSE_CHANGELIST_SIZE = 20;          ///< ��goose��λ�б��У���¼һ��goose���ƿ�ı�λ����,ʵ�ʼ�¼Ҫ��һ�γ�ʼ����

///
/// @brief
///    ����Ĭ��ϵͳ�ɼ�������Ƶ��
///
const uint32_t DSM_CONFIG_DEFAULT_ELEC_FREQUENCY = 50;

///
/// @brief
///    ����һ��Ĭ�Ͻ��յĲ�������ܲ���
///
const uint32_t DSM_CONFIG_DEFAULT_RECV_WAVE_COUNT = 10;

///
/// @brief
///    ����һ�ν������ٵ�����֡�������ù�С���ܻ�Ƶ������֪ͨ�¼�����ҪӦ�ø���Ĵ�������
///
const uint32_t DSM_CONFIG_RECV_SAMPLE_COUNT_MIN = 200;

///
/// @brief
///    ����¼��ʱʹ�û�������С
///
const uint32_t DSM_DEFAULT_RECORD_WAVE_BUFFER_SIZE_IN_BYTE = 20 * 1024 * 1024;

///
/// @brief
///    �Ƿ�ʵʱ¼��,ʵʱ¼��������¼���ļ���С�����ᶪ����
///    ��ʵʱ¼������¼�ƴ��ļ������ܻᶪ��
///
#define DSM_RECORD_WARE_REALTIME    1

///
/// @brief
///    ���岨��ͼ�Ƿ���ʾ��Чֵ/��λ��Ϣ,1Ϊ��ʾ��0Ϊ����ʾ
///
#define DSM_OSCILLOGRAM_SHOW_EFFECT_PHASE   0

///
/// @brief
///    ������12.8k�Ĳ��������Ƿ����²�����4k������Чֵг��������
///
#define DSM_DSP_CALC_128K_RESAMPLE_TO_4K   1

//���������ļ�����ʱĿ¼�ͳ����ļ�Ŀ¼
const std::wstring DEFAULT_CFG_BACKUP = L"config_backup\\";             ///< ���������ļ�·��
const std::wstring DEFAULT_CFG_RUN    = L"dsm_usercfg\\";               ///< ���������ļ�·��,λ�����ִ���ļ���Ŀ¼��ͬ��Ŀ¼


///
/// @brief
///    ����ӿڻ�ȡ���ݵ�����
///
enum NET_MODE{
    _RECV_NONE = -1,
    _9_1_9_2_DETECT = 0,
    _9_1_9_2_FILTER,
    _GOOSE_DETECT,
    _GOOSE_FILTER,
    _IEEE1588_DETECT,
    _IEEE1588_FILTER,

    _MAX_NET_NUM
};

///
/// @brief
///    FT3�������ݵĿ�����Ϣ��16λ
///
union Ft3CtlWordSend{
    // FT3���ĸ�ʽ
    enum eFt3FrameType
    {
        FT3_FRAME_TYPE_UNKNOWN = 0,
        FT3_FRAME_TYPE_NANRUI,
        FT3_FRAME_TYPE_GUOWANG
    };

    struct {
        uint16_t bCntClr : 1;                                             ///<  Bit[0] ����������           
        uint16_t bCRCReverse : 1;                                         ///<  CRC��0��ȡ�� 1����ȡ��      
        uint16_t bCRCValue : 1;                                           ///<  CRC��ֵ��0:0x0000,1:0xFFFF  
        uint16_t bSendMode : 1;                                           ///<  FT3����ģʽ��0:ͬ��         
        uint16_t bFrameType : 4;                                          ///<  ���ĸ�ʽ��1:����2:����    
        uint16_t bSampleRate : 4;                                         ///<  ������                      
        uint16_t bBaudRate : 4;                                           ///<  ������                      
    }CtrlBit;
    uint16_t nData;
};

///
/// @brief
///    FT3�������ݵĿ�����Ϣ��16λ
///
union Ft3CtlWordRecv{
    struct {
        uint16_t bNoErr:1;                                                //ͨѶ���� 0�쳣 1����
        uint16_t bReserve:2;                                              //����
        uint16_t bFrameType:4;                                            //��Լ��ʽ������������
        uint16_t bSampleRate:4;                                           //�����ʿ���λ
        uint16_t bBaudRate:4;                                             //�����ʿ���λ
        uint16_t bCrc:1;                                                  //CRCУ�����λ
    }CtrlBit;
    uint16_t nData;
};

///
/// @brief
///    Net��Ft3��������ͷ����ͷ�ṹ�壬�̶�8*sizeof(uint16_t)����
///
typedef union BufferHeader
{
    struct
    {
        uint16_t        nLoopItem;                      ///< ѭ������ʱ ʹ����һ�������� ֻ���� 0 1
        uint16_t        bAppDispDone;                   ///< ���Ӧ�û�û�д�������һ�α���
        uint16_t        nFramesCount;                   ///< �ڴ�ӳ���ļ��������н��յ����ĵ���֡��
        uint16_t        nIsTimeOut;                     ///< �Ƿ������ճ�ʱ
        Ft3CtlWordRecv  stCtrlWord;                     ///< FT3���ĸ�ʽ������
        uint16_t        nCurTxBufItem;                  ///< ��ǰ���ͻ����������
        uint32_t        nActiveLength;                  ///< ��������Ч����
    } Ft3Header;
    struct
    {
        uint16_t        nLoopItem;                      ///< ѭ������ʱ ʹ����һ�������� ֻ���� 0 1
        uint16_t        bAppDispDone;                   ///< ���Ӧ�û�û�д�������һ�α���
        uint16_t        nFramesCount;                   ///< �ڴ�ӳ���ļ��������н��յ����ĵ���֡��
        uint16_t        nIsTimeOut;                     ///< FPGA�Ƿ���ճ�ʱ
        uint16_t        nNetMode;                       ///< ��ǰ��ڵĹ���ģʽ,�����NET_MODE ö������
        uint16_t        nCurSmvTxBufItem;               ///< ��ǰSmv���ͻ����������
        uint32_t        nActiveLength;                  ///< ��������Ч���ȣ���nFramesCount֡����ռ�õĻ������Ŀռ䳤��
        uint16_t        nCurGooseTxBufItem;             ///< ��ǰGOOSE���ͻ����������
    } NetHeader;
    uint16_t nData[32];
}Map_File_Header;

//FT3�����������ڱ�ʶ����֡ʱ������������,��������ft3��չ֡
enum enumFt3FrameStyle
{
    _FT3_TYPE_NONE = 0,
    _FT3_TYPE_NANRUI,                                       ///< �����ʽ��FT3����
    _FT3_TYPE_GUOWANG,                                      ///< ������ʽ��FT3����
    _FT3_TYPE_SINGLE_PHASE,                                 ///< ���໥������FT3����
    _FT3_TYPE_THREE_PHASE_CUR,                              ///< ���������������FT3����
    _FT3_TYPE_THREE_PHASE_VOL,                              ///< �����ѹ��������FT3����
    _FT3_TYPE_THREE_PHASE_CUR_VOL,                          ///< ���������ѹ��������FT3����
    _FT3_TYPE_RESERVE,                                      ///< ����
    _FT3_TYPE_MAXVALUE
};

//����FT3��ͬ��������֡������Ϣ
const uint16_t FT3_FRAME_LEN[_FT3_TYPE_MAXVALUE] = {0, 56, 74, 26, 34, 28, 48, 0};          //FT3���ĵ��ܳ��� ������ʼ�ַ���CRCУ���ַ�
const uint16_t FT3_FRAME_ACTIVE_LEN[_FT3_TYPE_MAXVALUE] = {0, 48, 64, 20, 28, 22, 40, 0};   //FT3���ݼ��ĳ��� ȥ����ʼ�ַ���CRCУ���ַ�


const uint32_t GUOWANG_CHANNELS_PER_FRAME = 22;             ///< ����һ������֡����22��ͨ��
const uint32_t NANRUI_CHANNELS_PER_FRAME  = 12;             ///< ����һ������֡����12��ͨ��
const uint32_t NET_9_1_CHANNELS_PER_FRAME = 12;             ///< 9-1����֡һ��asdu��ͨ����
const uint32_t NET_9_2_LE_MAX_CHANNELS_PER_ASDU = 64;       ///< 9-2���ݵ����ͨ����Ŀ,Ӧ�÷���������������dsp�����Ĵ�С�޹�
const uint32_t FT3_TIME_STAMP_LEN  = 8;                     ///< FT3֡ǰʱ�������
const uint32_t FT3_9_1_MAX_CHANNEL_COUNT = GUOWANG_CHANNELS_PER_FRAME;  ///< FT3/9-1����֡ͨ�������ֵ

//
//=============================================================================
// ����˿ڻ���������
//=============================================================================
//
//����˿ڣ���ں���̫���ڣ�Smv���ջ�������С����
const uint32_t NET_MAX_RECV_FRAMES            = 10000;                                                                  ///< �����յı���֡��
const uint32_t NET_MAX_FRAME_LEN              = 1536;                                                                   ///< һ֡���ĵ���󳤶�
const uint32_t NET_RECV_MAP_FILE_LEN          = NET_MAX_RECV_FRAMES * NET_MAX_FRAME_LEN + sizeof(BufferHeader) ;        ///< ����̫������ʱ���ڴ�ӳ���ļ���С
const uint32_t NET_RECV_LOOP_BUF0_POS         = 0;                                                                      ///< �����ڴ��Ϊ���������������ǵ�һ����������λ��
const uint32_t NET_RECV_LOOP_BUF1_POS         = (NET_RECV_MAP_FILE_LEN - sizeof(BufferHeader))/2;                       ///< �����ڴ��Ϊ���������������ǵڶ�����������λ��

//����˿�Goose���ݽ��ջ���������
const uint32_t GOOSE_RECV_MAX_GROUP           = 50;                                                                     ///< �����Խ���100��GOOSE
const uint32_t GOOSE_RECV_BUF_LEN             = NET_MAX_FRAME_LEN * GOOSE_RECV_MAX_GROUP;                               ///< ����Ϊ1536*100
const uint32_t GOOSE_RECV_LOOP_BUF0_POS       = NET_RECV_LOOP_BUF0_POS;                                                 ///< �����ڴ��Ϊ���������������ǵ�1����������λ��
const uint32_t GOOSE_RECV_LOOP_BUF1_POS       = NET_RECV_LOOP_BUF1_POS;                                                 ///< �����ڴ��Ϊ���������������ǵ�2����������λ��

//IEEE1588���յĻ�����λ��
const uint32_t IEEE1588_RECV_MAX_GROUP        = 50;                                                                     ///< �����Խ���50��IEEE1588
const uint32_t IEEE1588_RECV_BUF_LEN          = NET_MAX_FRAME_LEN * IEEE1588_RECV_MAX_GROUP;                            ///< ����Ϊ1536*50
const uint32_t IEEE1588_RECV_BUF0_POS         = (GOOSE_RECV_LOOP_BUF0_POS + GOOSE_RECV_BUF_LEN);                        ///< �����ڴ��Ϊ���������������ǵ�1����������λ��
const uint32_t IEEE1588_RECV_BUF1_POS         = (GOOSE_RECV_LOOP_BUF1_POS + GOOSE_RECV_BUF_LEN);                        ///< �����ڴ��Ϊ���������������ǵ�2����������λ��

//SMV���ͻ���������
const uint32_t NET_SEND_MAP_FILE_LEN          = NET_RECV_MAP_FILE_LEN;                                                  ///< ��̫���������ݹ����ڴ��С,Ӧ�ó���������������ڴ��С
const uint32_t NET_SMV_SEND_BUF_LEN           = 7511040;                                                                ///< ����ֵ���ͻ�������С
const uint32_t NET_SMV_SEND_LOOP_BUF0_POS     = IEEE1588_RECV_BUF0_POS + IEEE1588_RECV_BUF_LEN;                         ///< ���͹����ڴ��Ϊ���������������ǵ�һ����������λ��
const uint32_t NET_SMV_SEND_LOOP_BUF1_POS     = IEEE1588_RECV_BUF1_POS + IEEE1588_RECV_BUF_LEN;                         ///< ���͹����ڴ��Ϊ���������������ǵڶ�����������λ��

//GOOSE���ͻ���������
const uint32_t GOOSE_SEND_MAX_GROUP           = 10;                                                                     ///< ��෢��10��GOOSE
const uint32_t GOOSE_SEND_BUF_LEN             = NET_MAX_FRAME_LEN * GOOSE_SEND_MAX_GROUP;                               ///< ����Ϊ10 * 1536
const uint32_t GOOSE_SEND_LOOP_BUF0_POS       = NET_SMV_SEND_LOOP_BUF0_POS + NET_SMV_SEND_BUF_LEN;
const uint32_t GOOSE_SEND_LOOP_BUF1_POS       = NET_SMV_SEND_LOOP_BUF1_POS + NET_SMV_SEND_BUF_LEN;

//
//=============================================================================
// �⴮�ڻ���������
//=============================================================================
//
//�⴮�ڽ������ݻ���������
const uint32_t FT3_MAX_RECV_FRAMES            = 2640;                                                                   ///< Ӧ�ó����ܽ��յ��������
const uint32_t FT3_MAX_FPGA_LEN               = 8192;                                                                   ///< һ��RAM��������С4096 words
const uint32_t FT3_FPGA_RAM_COUNT             = 24;                                                                     ///< �����ڴ滺�����б���FPGA��������Ŀ
const uint32_t FT3_RECV_MAP_FILE_LEN          = FT3_MAX_FPGA_LEN * FT3_FPGA_RAM_COUNT + sizeof(BufferHeader);           ///< �⴮�ڽ���ʱ���ڴ�ӳ���ļ���С
const uint32_t FT3_RECV_LOOP_BUF0_POS         = 0;                                                                      ///< �����ڴ��Ϊ���������������ǵ�һ����������λ��
const uint32_t FT3_RECV_LOOP_BUF1_POS         = (FT3_RECV_MAP_FILE_LEN - sizeof(BufferHeader))/2;                       ///< �����ڴ��Ϊ���������������ǵڶ�����������λ��

/// FT3_SEND_MAP_FILE_LEN FT3����ʱ���ڴ�ӳ���ļ���С  256*84*2 = 42KB
/// �����ձ��ĳ���,ӳ���ļ���ǰ����ͷ��Ϣ������������Ӧ�ò����Ϣ����
const uint32_t FT3_SEND_MAP_FILE_LEN          = 43008;                                                                  ///< FT3�������ݹ����ڴ��С,Ӧ�ó���������������ڴ��С 256*84*2
const uint32_t FT3_SEND_LOOP_BUF0_POS         = 0;                                                                      ///< ���͹����ڴ��Ϊ���������������ǵ�һ����������λ��
const uint32_t FT3_SEND_LOOP_BUF0_LEN         = (FT3_SEND_MAP_FILE_LEN - sizeof(BufferHeader))/2;                       ///< ��һ���������ĳ���
const uint32_t FT3_SEND_LOOP_BUF1_POS         = FT3_SEND_LOOP_BUF0_LEN;                                                 ///< ���͹����ڴ��Ϊ���������������ǵڶ�����������λ��
const uint32_t FT3_SEND_LOOP_BUF1_LEN         = FT3_SEND_LOOP_BUF0_LEN;                                                 ///< �ڶ����������ĳ���

//
//=============================================================================
// ����Ĭ�����ò���������һ�ν��ն�������֡���Ƿ����ó�ʱ��Ϣ������
//=============================================================================
//
//����˿ڽ���Ĭ������
const uint32_t NET_DETECT_RECV_FRAMES         = 400;
const uint32_t NET_DEFAULT_RECV_FRAMES        = 800;                                                                    ///< �������Ĭ��һ�ν��ܵ�����֡������󲻳���NET_MAX_RECV_FRAMES/2
const uint32_t NET_MUL_CTRL_BLKS_RECV_FRAMES  = 1600;                                                                   ///< ͬʱ���ն�����ƿ�ʱ����Ĭ��һ�ν��յ�����֡��
const bool     NET_DEFAULT_RECV_ENABLETIMEOUT = true;                                                                   ///< �������Ĭ���Ƿ����ó�ʱ
const uint32_t NET_DEFAULT_RECV_TIMEOUT       = 0;                                                                      ///< �������Ĭ�ϳ�ʱʱ�䣬��λms,��ʱʱ��Ŀǰ���������ã�������������

//�⴮�ڽ���Ĭ������
const uint32_t FT3_DEFAULT_RECV_FRAMES        = 800;                                                                    ///< FT3Ĭ��һ�ν��ܵ�����֡������󲻳���FT3_MAX_RECV_FRAMES/2
const bool     FT3_DEFAULT_RECV_ENABLETIMEOUT = true;                                                                   ///< FT3����Ĭ���Ƿ����ó�ʱ
const uint32_t FT3_DEFAULT_RECV_TIMEOUT       = 0;                                                                      ///< FT3����Ĭ�ϳ�ʱʱ�䣬��λms,��ʱʱ��Ŀǰ���������ã�������������



///
/// @brief
///    ����������ݺ���������ʱFt3ʹ�õ��¼����ƺ��ļ�ӳ������
///
const std::wstring      FT3_RECV_NOTIFY_EVENT_NAME_A   = L"DSM_FT3_RX1";
const std::wstring      FT3_RECV_MAP_FILE_NAME_A   = L"ShareFT3_RX1";
const std::wstring      FT3_RECV_NOTIFY_EVENT_NAME_B   = L"DSM_FT3_RX2";
const std::wstring      FT3_RECV_MAP_FILE_NAME_B   = L"ShareFT3_RX2";


///
/// @brief
///    �����������ʱ����������ʱ��̫��ʹ�õ��¼����ƺ��ļ�ӳ������
///
const std::wstring      NET_RECV_NOTIFY_EVENT_NAME_A    = L"DSM_FIBER_A";
const std::wstring      NET_RECV_MAP_FILE_NAME_A        = L"ShareFIBER_A";
const std::wstring      NET_RECV_NOTIFY_EVENT_NAME_B    = L"DSM_FIBER_B";
const std::wstring      NET_RECV_MAP_FILE_NAME_B        = L"ShareFIBER_B";
const std::wstring      NET_RECV_NOTIFY_EVENT_NAME_C    = L"DSM_FIBER_C";
const std::wstring      NET_RECV_MAP_FILE_NAME_C        = L"ShareFIBER_C";

///
/// @brief
///    ���巢������ʱ����������ʱFt3ʹ�õ��¼����ƺ��ļ�ӳ������
///
const std::wstring      FT3_SEND_NOTIFY_EVENT_NAME_C    = L"DSM_FT3_TX1";
const std::wstring      FT3_SEND_MAP_FILE_NAME_C        = L"ShareFT3_TX1";

///
/// @brief
///    ���巢�����ݺ���������ʱ��̫��ʹ�õ��¼����ƺ��ļ�ӳ������
///
const std::wstring      NET_SEND_NOTIFY_EVENT_NAME_A    = L"";
const std::wstring      NET_SEND_MAP_FILE_NAME_A        = NET_RECV_MAP_FILE_NAME_A;
const std::wstring      NET_SEND_NOTIFY_EVENT_NAME_B    = L"";
const std::wstring      NET_SEND_MAP_FILE_NAME_B        = NET_RECV_MAP_FILE_NAME_B;
const std::wstring      NET_SEND_NOTIFY_EVENT_NAME_C    = L"";
const std::wstring      NET_SEND_MAP_FILE_NAME_C        = NET_RECV_MAP_FILE_NAME_C;


///
/// @brief
///    ���������ļ�����
///
#define PARAM_CONFIG_BASIC          L"paramconfig_basic.xml"            ///< ϵͳ���û��������ļ���
#define PARAM_CONFIG_SMV_SEND       L"paramconfig_smv_send.xml"         ///< SMV���������ļ���
#define PARAM_CONFIG_GOOSE_SEND     L"paramconfig_goose_send.xml"       ///< GOOSE���������ļ���
#define PARAM_CONFIG_GOOSE_RECV     L"paramconfig_goose_recv.xml"       ///< GOOSE���������ļ���
#define PARAM_CONFIG_MANUAL_TEST    L"paramconfig_manual_test.xml"      ///< �ֶ����������ļ���
#define PARAM_CONFIG_STATE_SEQUENCE L"paramconfig_state_sequence.xml"   ///< ״̬���������ļ���

#define SMV_RECV_CONFIG_PUBLIC      L"smvrecvconfig.xml"                ///< SMV��������

///
/// @brief
///    ����FPGA�ϸ����ӿڵı�ʶ
///    2������̫���ڣ�3��FT3��,����ı�ʶ����û��ʹ�ã�������ʹ�õ���
///    datatypes.h�ļ��е� enum DsmPortName����ֵ�����ﱣ��һ��
///
#define DEVICE_NET_A 0                                                  ///< ����̫����A
#define DEVICE_NET_B 1                                                  ///< ����̫����B
#define DEVICE_NET_C 2                                                  ///< ����̫����
#define DEVICE_FT3_A 3                                                  ///< �⴮��A����������
#define DEVICE_FT3_B 4                                                  ///< �⴮��B����������
#define DEVICE_FT3_C 5                                                  ///< �⴮��C����������

///
/// @brief
///    Ft3 DeviceIoControl���ݲ����ṹ��
///
struct IOCTRL_IN_PARAM_FT3{
    uint32_t nDeviceItem;                                               ///< �豸�� 
    uint32_t nInBuf[4];                                                 ///< ����DeviceIoControl�����������������󳤶�4

    IOCTRL_IN_PARAM_FT3():nDeviceItem(0)
    {
        memset(nInBuf, 0, sizeof(nInBuf));
    }
};

///
/// @brief
///    ����ӿ� DeviceIoControl���ݲ����ṹ�壬����С����ʱʹ��
///
struct IOCTRL_IN_PARAM_NET{
    uint32_t nDeviceItem;                                               ///< �豸�� 
    uint32_t nInBuf[4];                                                 ///< ����DeviceIoControl�����������������󳤶�12

    IOCTRL_IN_PARAM_NET():nDeviceItem(0)
    {
        memset(nInBuf, 0, sizeof(nInBuf));
    }
};

///
/// @brief
///    ����ӿ� DeviceIoControl���ݲ����ṹ�壬������������ʱʹ�ã��������Ϣ
///
struct IOCTRL_IN_PARAM_NET_BIG{
    uint32_t nDeviceItem;                                               ///< �豸�� 
    uint32_t nInBuf[20];                                                ///< ����DeviceIoControl�����������������󳤶�12

    IOCTRL_IN_PARAM_NET_BIG(): nDeviceItem(0)
    {
        memset(nInBuf, 0, sizeof(nInBuf));
    }
};

///
/// @brief
///    ����1588ʱ����Ϣ�ṹ��
///
struct IEEE1588_CLOCK_SYNC_INFO{
    uint32_t nMClockId[2];                                              ///< ��ʱ��ID
    uint32_t nMPortId;                                                  ///< ��ʱ�Ӷ˿�ID
    uint8_t  nSourceMac[8];                                             ///< �����˿ڵ�ԴMAC��ַ
    uint32_t nSClockId[2];                                              ///< ����ʱ��ID
    uint32_t nSPortId;                                                  ///< ����ʱ�Ӷ˿�ID
};


///
/// @brief
///    �������ù���ʹ�õ�mac,appid�ṹ��Ϣ
///
///
struct FpgaNetFilterItem
{
    uint8_t macAddr[ETHERNET_MAC_LEN];
    uint8_t appId[SMV_GOOSE_APPID_LEN];
};

///
/// @brief
///    UTC ʱ��ṹ��
///
#pragma pack(push)
#pragma pack (2)                                                        ///< ����Ϊ2Byte���룬Ĭ����8Byte
typedef struct {
    uint32_t uSecondSinceEpoch;                                         ///< ��
    uint32_t uFractionOfSecond;                                         ///< ΢��
}UTC_TIME_t;
#pragma pack(pop)                                                       ///< �ָ�Ĭ�϶��뷽ʽ

#endif //__CONFIG_H_