/// @file
///
/// @brief
///     �������ݷ��ͽ����õ�һЩ���Ͷ���
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.3.22
///
/// �޸���ʷ��
///

#ifndef _DATA_TYPES_H
#define _DATA_TYPES_H

#include <string>
#include "src\include\stdint.h"
#include "src\include\pub_datatypes.h"

///
/// @brief
///    ����⴮�ڱ�ʶ����ʶ���������� IOCTRL_IN_PARAM ��nDeviceItem �豸��ʶ��ֵһ��
///    ����ֱ������DeviceIoControl������
///
enum DsmPortName{
    NET_PORT_LIGHT_A = 0,                                               ///< ��̫�����A
    NET_PORT_LIGHT_B = 1,                                               ///< ��̫�����B
    NET_PORT_ELECTRONIC = 2,                                            ///< ����̫����
    FT3_PORT_LIGHT_A = 3,                                               ///< ���A,����
    FT3_PORT_LIGHT_B = 4,                                               ///< ���B,����
    FT3_PORT_LIGHT_C = 5,                                               ///< ���C,����
    LIGHT_PORT_IRIG_B = 6,                                              ///< b��ͬ���ӿ�
    DSM_PORT_UNKOWN                                                     ///< δ֪�˿�
};


///
/// @brief
///    ����FT3ʹ�õĲ����ʣ������ʶ�����ֵ��FT3�������е�������ͬ
///    ��Ϊʵ����ֵ��ft3������֮���ת����ʹ�ã�Ӧ�ò�ʵ�ʻ�ȡ������ʵ�ʵĲ���ֵ����4000��������ֵ
///
enum FT3_SAMP_RATE{
    FT3_SAMP_RATE_UNKOWN = 0,                                           ///< δ֪������
    FT3_SAMP_RATE_4K,                                                   ///< ������4000
    FT3_SAMP_RATE_5K,                                                   ///< ������5000
    FT3_SAMP_RATE_8K,                                                   ///< ������8000
    FT3_SAMP_RATE_10K,                                                  ///< ������10000
    FT3_SAMP_RATE_12P8K                                                 ///< ������12800
};

///
/// @brief
///    ����Ft3ʹ�õĲ�����,�����ʶ�����ֵ��FT3�������е�������ͬ
///
enum FT3_BAUD_RATE{
    FT3_BAUD_RATE_UNKOWN = 0,                                           ///< δ֪������
    FT3_BAUD_RATE_2P5M = 1,                                             ///< ������2.5M
    FT3_BAUD_RATE_5M = 2,                                               ///< ������5M
    FT3_BAUD_RATE_10M = 3                                               ///< ������10M
};

enum FrameSendType{
    SEND_NONE = 0,
    SEND_MU_SMV   = 0x55AA1122,
    SEND_FT3_SMV  = 0x55AA2211,
    SEND_GOOSE    = 0xAA551212
};

///
/// @brief
///    ����ͨ�������Ϣ
///
enum ChannelPhase{
    CHANNEL_PHASE_UNKOWN  = 0,                                          ///< ͨ�����δ֪
    CHANNEL_PHASE_A = 1,                                                ///< ͨ�����ΪA��
    CHANNEL_PHASE_B = 2,                                                ///< ͨ�����ΪB��
    CHANNEL_PHASE_C = 3,                                                ///< ͨ�����ΪC��
    CHANNEL_PHASE_X = 4,                                                ///< ͨ�����ΪX�࣬ͬ��/ĸ��/��ȡ
    CHANNEL_PHASE_N = 5                                                 ///< ͨ�����ΪN�࣬���Ե�
};

///
/// @brief
///    scl�ļ��п��ƿ�����
///
enum CBType
{
    CB_TYPE_UNKOWN = 0,                                                 ///< ���ƿ�����δ֪
    CB_TYPE_SMV,                                                        ///< ���ƿ�����Ϊ����ֵ
    CB_TYPE_GOOSE,                                                      ///< ���ƿ�����Ϊgoose
};

///
/// @brief
///    ��������
///
enum eCurrentType
{
    CURRENT_TYPE_UNKNOWN = 0,
    CURRENT_TYPE_AC,                                                    ///< ������
    CURRENT_TYPE_DC                                                     ///< ֱ����
};

///
/// @brief
///    Fpga�����ʱ��ͬ����ʽ
///
enum FpgaTimeSyncMode{
    FPGA_TIME_SYNC_NONE       = 0,                                      ///< ��ʹ��Уʱ
    FPGA_TIME_SYNC_IRIGB_POS  = 1,                                      ///< ����B��
    FPGA_TIME_SYNC_IRIGB_NEG  = 2,                                      ///< ����B��
    FPGA_TIME_SYNC_IEEE1588_A = 3,                                      ///< IEEE1588��ʱ
    FPGA_TIME_SYNC_IEEE1588_B = 4,                                      ///< IEEE1588��ʱ
    FPGA_TIME_SYNC_IEEE1588_C = 5                                       ///< IEEE1588��ʱ
};

///
/// @brief
///    ����⴮�ڷ����������ͣ�FT3/B�룩
///
enum enumFt3TxStyle{
    FT3_TX_STYLE_FT3 = 0,                                               ///< ����FT3����
    FT3_TX_STYLE_IRIG_B                                                 ///< ����B������
};


///
/// @brief
///    ���嵱ǰ�Ĳ���ģʽ
///
enum DSM_OPERATE_MODE
{
    DSM_MODE_UNKOWN = 0,                                                ///< ģʽδ֪
    DSM_MODE_MAIN_PAGE,                                                 ///< ��ǰ����������
    DSM_MODE_QUIT_CURRENT_PAGE,                                         ///< �˳�ĳ��ҵ�����ʱ����ģʽ
    DSM_MODE_DETECT,                                                    ///< ̽��
    DSM_MODE_FILTER,                                                    ///< ����
    DSM_MODE_SMV_RECV_OSCILLOGRAM,                                      ///< ����ͼ
    DSM_MODE_SMV_RECV_EFFECTIVE_VALUE,                                  ///< ��Чֵ
    DSM_MODE_SMV_RECV_PHASOR,                                           ///< ����
    DSM_MODE_SMV_RECV_XU_LIANG,                                         ///< ����
    DSM_MODE_SMV_RECV_POWER,                                            ///< ����
    DSM_MODE_SMV_RECV_HARMONIC,                                         ///< г��
    DSM_MODE_SMV_RECV_DOUBLE_AD,                                        ///< ˫AD
    DSM_MODE_SMV_RECV_PHASE_CHECK,                                      ///< ����
    DSM_MODE_SMV_RECV_POLARITY,                                         ///< ����
    DSM_MODE_SMV_RECV_STATUS_WATCH,                                     ///< ״̬����
    DSM_MODE_SMV_RECV_DISPERSION,                                       ///< ������ɢ��
    DSM_MODE_SMV_RECV_FRAME_STATIC,                                     ///< ����ͳ��
    DSM_MODE_SMV_RECV_FRAME_MONITOR,                                    ///< ���ļ��
    DSM_MODE_SMV_RECV_RECORD_WAVE,                                      ///< net smv¼��
    DSM_MODE_SMV_RECV_REAL_TIME_MSG,                                    ///< ʵʱ����

    DSM_MODE_GOOSE_RECV_REALTIME_VAL,                                   ///< ʵʱֵ
    DSM_MODE_GOOSE_RECV_CHANGE_LIST,                                    ///< ��λ�б�
    DSM_MODE_GOOSE_RECV_FRAME_STATIC,                                   ///< ����ͳ��
    DSM_MODE_GOOSE_RECV_FRAME_MONITOR,                                  ///< ���ļ��
    DSM_MODE_GOOSE_RECV_REAL_TIME_MSG,                                  ///< ʵʱ����
    DSM_MODE_GOOSE_RECV_RECORD_WAVE,                                    ///< ¼��
    DSM_MODE_GOOSE_RECV_DI,                                             ///< �ֶ����顢״̬���е�DI

    DSM_MODE_FT3_RECV_RECORD_WAVE,                                      ///< ft3¼����ʹ��pcap��ʽ

    DSM_MODE_NET_RECV_1588                                              ///< ����1588����֡

};

///
/// @brief
///    ���岻ͬ�ĳ�������
///
enum enumSmvRecvConfigTemplateType
{
    SMV_RECV_CONFIG_TEMPLATE_TYPE_UNKNOWN = 0,

    SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_BODIAN,                       ///< �����豸����Ft3��׼֡ͨ��Ĭ������,12ͨ��
    SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_KAIMO,                        ///< ��Ĭ�豸����Ft3��׼֡ͨ��Ĭ������,12ͨ��
    SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_60044_8,                      ///< IEC60044-8����ͨ��Ĭ������,12ͨ��

    SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_BODIAN,                       ///< �����豸����Ft3��չ֡ͨ��Ĭ������,22ͨ��
    SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_KAIMO,                        ///< ��Ĭ�豸����Ft3��չ֡ͨ��Ĭ������,22ͨ��
    SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_BOOK,                         ///< ���豸����ͨ��Ĭ������,22ͨ��

    SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_BODIAN,                           ///< ����9-2Ĭ��ͨ��������Ϣ,8ͨ��
    SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_KAIMO,                            ///< ��Ĭ9-2Ĭ��ͨ��������Ϣ,12ͨ��
    SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_BOOK,                             ///< �����ܱ��վԭ�����Լ�����P62ҳ��18ͨ��ģ��

    SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2,                                  ///< ���豸����9-2ͨ��Ĭ������,30ͨ��
    SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_LESS_CHANNEL,                     ///< ���豸����9-2ͨ����ͨ����С��13��Ĭ������,12ͨ��

    SMV_RECV_CONFIG_TEMPLATE_FROME_SCL,                                 ///< ��SCL�ļ����ض�Ӧ��������Ϣ
};

///
/// @brief
///    ����Ԥ����ͨ����Ϣ�ṹ��
///
struct PREDEFINED_CHANNEL_INFO
{
    uint32_t                nIndex;                     ///< ͨ������
    enumChannelType         enChannelType;              ///< ͨ������
    ChannelPhase            enChannelPhase;             ///< ͨ�����
    enumChannelValueType    enChannelValueType;         ///< ͨ���Ǳ������ݻ��ǲ�������
    std::wstring            strChannelDesc;             ///< ͨ������
    std::wstring            strVirtualChannel;          ///< ӳ�������ͨ����
};

///
/// @brief
///    ��׼Ft3Ԥ��ͨ����Ϣ��IEEE60044-8ģ��
///
static const PREDEFINED_CHANNEL_INFO PREDEFINED_CHANNEL_INFO_FT3_STD_IEEE60044_8[] = {
    { 0,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_PROTECT,  L"����A�����",         L"Ia1"},
    { 1,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_PROTECT,  L"����B�����",         L"Ib1"},
    { 2,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_PROTECT,  L"����C�����",         L"Ic1"},
    { 3,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_MEASURE,  L"���Ե����",          L"Ix1"},
    { 4,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_MEASURE,  L"����A�����",         L"Ia1"},
    { 5,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_MEASURE,  L"����B�����",         L"Ib1"},
    { 6,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_MEASURE,  L"����C�����",         L"Ic1"},
    { 7,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"A���ѹ",             L"Ua1"},
    { 8,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"B���ѹ",             L"Ub1"},
    { 9,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"C���ѹ",             L"Uc1"},
    { 10,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"���Ե��ѹ",          L"Ux1"},
    { 11,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"ĸ�ߵ�ѹ",            L"Ux1"}
};


///
/// @brief
///    �����׼Ft3Ԥ��ͨ����Ϣ����ĬԤ����ͨ��
///
static const PREDEFINED_CHANNEL_INFO PREDEFINED_CHANNEL_INFO_FT3_STD_KAIMO[] = {
    { 0,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_PROTECT,  L"����A�����",         L"Ia1"},
    { 1,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_PROTECT,  L"����B�����",         L"Ib1"},
    { 2,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_PROTECT,  L"����C�����",         L"Ic1"},
    { 3,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_PROTECT,  L"���򱣻�����",        L"Ix1"},
    { 4,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_MEASURE,  L"����A�����",         L"Ia1"},
    { 5,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_MEASURE,  L"����B�����",         L"Ib1"},
    { 6,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_MEASURE,  L"����C�����",         L"Ic1"},
    { 7,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"A���ѹ",             L"Ua1"},
    { 8,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"B���ѹ",             L"Ub1"},
    { 9,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"C���ѹ",             L"Uc1"},
    { 10,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"���򱣻���ѹ",        L"Ux1"},
    { 11,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"ĸ�ߵ�ѹ",            L"Ux1"}
};

///
/// @brief
///    �����׼Ft3Ԥ��ͨ����Ϣ������Ԥ����ͨ��
///
static const PREDEFINED_CHANNEL_INFO PREDEFINED_CHANNEL_INFO_FT3_STD_BODIAN[] = {
    { 0,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_PROTECT,  L"����A�����",         L"Ia1"},
    { 1,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_PROTECT,  L"����B�����",         L"Ib1"},
    { 2,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_PROTECT,  L"����C�����",         L"Ic1"},
    { 3,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_PROTECT,  L"��������",            L"Ix1"},
    { 4,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_MEASURE,  L"����A�����",         L"Ia1"},
    { 5,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_MEASURE,  L"����B�����",         L"Ib1"},
    { 6,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_MEASURE,  L"����C�����",         L"Ic1"},
    { 7,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"A���ѹ",             L"Ua1"},
    { 8,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"B���ѹ",             L"Ub1"},
    { 9,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"C���ѹ",             L"Uc1"},
    { 10,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"��ѹ",                L"Ux1"},
    { 11,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"��ѹ",                L"Ux1"}
};


///
/// @brief
///    ����Ԥ����ͨ����Ϣ
///
static const PREDEFINED_CHANNEL_INFO PREDEFINED_CHANNEL_INFO_FT3_EXT_GUOWANG[] = {
    { 0,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_PROTECT,    L"����A�����1",     L"Ia1"},
    { 1,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_PROTECT,    L"����A�����2",     L"Ia1"},
    { 2,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_PROTECT,    L"����B�����1",     L"Ib1"},
    { 3,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_PROTECT,    L"����B�����2",     L"Ib1"},
    { 4,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_PROTECT,    L"����C�����1",     L"Ic1"},
    { 5,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_PROTECT,    L"����C�����2",     L"Ic1"},
    { 6,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_MEASURE,    L"����A�����",      L"Ia1"},
    { 7,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_MEASURE,    L"����B�����",      L"Ib1"},
    { 8,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_MEASURE,    L"����C�����",      L"Ic1"},
    { 9,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_MEASURE,    L"���Ե����1",      L"Ix1"},
    { 10,    CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_MEASURE,    L"���Ե����2",      L"Ix1"},
    { 11,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"A���ѹ1",         L"Ua1"},
    { 12,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"A���ѹ2",         L"Ua1"},
    { 13,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"B���ѹ1",         L"Ub1"},
    { 14,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"B���ѹ2",         L"Ub1"},
    { 15,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"C���ѹ1",         L"Uc1"},
    { 16,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"C���ѹ2",         L"Uc1"},
    { 17,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"���Ե��ѹ1",      L"Ux1"},
    { 18,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"���Ե��ѹ2",      L"Ux1"},
    { 19,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"ĸ�ߵ�ѹ1",        L"Ux2"},
    { 20,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"ĸ�ߵ�ѹ2",        L"Ux2"},
    { 21,    CHANNEL_TYPE_UNKOWN,          CHANNEL_PHASE_UNKOWN,   CHANNEL_VALUE_TYPE_UNKNOWN,    L"����",             L""},
};

///
/// @brief
///    ��Ĭft3��չԤ����ͨ����Ϣ
///
static const PREDEFINED_CHANNEL_INFO PREDEFINED_CHANNEL_INFO_FT3_EXT_KAIMO[] = {
    { 0,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_PROTECT,    L"����A�����1",     L"Ia1"},
    { 1,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_PROTECT,    L"����A�����2",     L"Ia1"},
    { 2,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_PROTECT,    L"����B�����1",     L"Ib1"},
    { 3,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_PROTECT,    L"����B�����2",     L"Ib1"},
    { 4,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_PROTECT,    L"����C�����1",     L"Ic1"},
    { 5,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_PROTECT,    L"����C�����2",     L"Ic1"},
    { 6,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_PROTECT,    L"���򱣻�����1",    L"Ix1"},
    { 7,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_PROTECT,    L"���򱣻�����2",    L"Ix1"},
    { 8,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_MEASURE,    L"����A�����",      L"Ia1"},
    { 9,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_MEASURE,    L"����B�����",      L"Ib1"},
    { 10,    CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_MEASURE,    L"����C�����",      L"Ic1"},
    { 11,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"A�ౣ����ѹ1",     L"Ua1"},
    { 12,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"A�ౣ����ѹ2",     L"Ua1"},
    { 13,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"B�ౣ����ѹ1",     L"Ub1"},
    { 14,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"B�ౣ����ѹ2",     L"Ub1"},
    { 15,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"C�ౣ����ѹ1",     L"Uc1"},
    { 16,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"C�ౣ����ѹ2",     L"Uc1"},
    { 17,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"���򱣻���ѹ1",    L"Ux1"},
    { 18,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"���򱣻���ѹ2",    L"Ux1"},
    { 19,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"ĸ�ߵ�ѹ1",        L"Ux2"},
    { 20,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,    L"ĸ�ߵ�ѹ2",        L"Ux2"},
    { 21,    CHANNEL_TYPE_UNKOWN,          CHANNEL_PHASE_UNKOWN,   CHANNEL_VALUE_TYPE_UNKNOWN,    L"����",             L""},
};

///
/// @brief
///    ����ft3��չԤ����ͨ����Ϣ
///
static const PREDEFINED_CHANNEL_INFO PREDEFINED_CHANNEL_INFO_FT3_EXT_BODIAN[] = {
    { 0,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_PROTECT,  L"����A�����",         L"Ia1"},
    { 1,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_PROTECT,  L"����B�����",         L"Ib1"},
    { 2,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_PROTECT,  L"����C�����",         L"Ic1"},
    { 3,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_PROTECT,  L"��������",            L"Ix1"},
    { 4,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_MEASURE,  L"����A�����",         L"Ia1"},
    { 5,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_MEASURE,  L"����B�����",         L"Ib1"},
    { 6,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_MEASURE,  L"����C�����",         L"Ic1"},
    { 7,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"A���ѹ",             L"Ua1"},
    { 8,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"B���ѹ",             L"Ub1"},
    { 9,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"C���ѹ",             L"Uc1"},
    { 10,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"��ѹ",                L"Ux1"},
    { 11,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"��ѹ",                L"Ux1"},
    { 12,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"��ѹ",                L"Ux1"},
    { 13,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"��ѹ",                L"Ux1"},
    { 14,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"��ѹ",                L"Ux1"},
    { 15,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"��ѹ",                L"Ux1"},
    { 16,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"��ѹ",                L"Ux1"},
    { 17,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"��ѹ",                L"Ux1"},
    { 18,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"��ѹ",                L"Ux1"},
    { 19,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"��ѹ",                L"Ux1"},
    { 20,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"��ѹ",                L"Ux1"},
    { 21,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"��ѹ",                L"Ux1"},
};

///
/// @brief
///    9-2Le��û�н����ļ�ʱʹ�õ�Ԥ����ͨ����Ϣ
///
static const PREDEFINED_CHANNEL_INFO PREDEFINED_CHANNEL_INFO_9_2LE[] = {
    { 0,     CHANNEL_TYPE_SMV_TIME,        CHANNEL_PHASE_UNKOWN,   CHANNEL_VALUE_TYPE_UNKNOWN,     L"���ʱ",       L""},
    { 1,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"A�ౣ������AD1", L""},
    { 2,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"A�ౣ������AD2", L""},
    { 3,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"B�ౣ������AD1", L""},
    { 4,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"B�ౣ������AD2", L""},
    { 5,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"C�ౣ������AD1", L""},
    { 6,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"C�ౣ������AD2", L""},
    { 7,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"A���������",    L""},
    { 8,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"B���������",    L""},
    { 9,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"C���������",    L""},
    { 10,    CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"���Ե����1",    L""},
    { 11,    CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"���Ե����2",    L""},
    { 12,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"A���ѹ1",       L""},
    { 13,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"A���ѹ2",       L""},
    { 14,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"B���ѹ1",       L""},
    { 15,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"B���ѹ2",       L""},
    { 16,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"C���ѹ1",       L""},
    { 17,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"C���ѹ2",       L""},
    { 18,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"���Ե��ѹ1",    L""},
    { 19,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"���Ե��ѹ2",    L""},
    { 20,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"ĸ�ߵ�ѹ1",      L""},
    { 21,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"ĸ�ߵ�ѹ2",      L""},
    { 22,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"��ѹ",           L""},
    { 23,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"��ѹ",           L""},
    { 24,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"��ѹ",           L""},
    { 25,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"��ѹ",           L""},
    { 26,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"��ѹ",           L""},
    { 27,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"��ѹ",           L""},
    { 28,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"��ѹ",           L""},
    { 29,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"��ѹ",           L""}
};

///
/// @brief
///    9-2����Ĭ��ͨ��
///
static const PREDEFINED_CHANNEL_INFO PREDEFINED_CHANNEL_INFO_9_2LE_BODIAN[] = {
    { 0,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_PROTECT,  L"��������1",         L"Ia1"},
    { 1,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_PROTECT,  L"��������2",         L"Ib1"},
    { 2,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_PROTECT,  L"��������3",         L"Ic1"},
    { 3,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_PROTECT,  L"��������4",         L"Ix1"},
    { 4,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"������ѹ1",         L"Ua1"},
    { 5,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"������ѹ2",         L"Ub1"},
    { 6,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"������ѹ3",         L"Uc1"},
    { 7,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"������ѹ4",         L"Ux1"}
};

///
/// @brief
///    9-2��ĬĬ��ͨ��
///
static const PREDEFINED_CHANNEL_INFO PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO[] = {
    { 0,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_PROTECT,  L"����A�����",         L"Ia1"},
    { 1,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_PROTECT,  L"����B�����",         L"Ib1"},
    { 2,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_PROTECT,  L"����C�����",         L"Ic1"},
    { 3,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_PROTECT,  L"���򱣻�����",        L"Ix1"},
    { 4,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_PROTECT,  L"����A�����",         L"Ia1"},
    { 5,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_PROTECT,  L"����B�����",         L"Ib1"},
    { 6,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_PROTECT,  L"����C�����",         L"Ic1"},
    { 7,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"A���ѹ",             L"Ua1"},
    { 8,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"B���ѹ",             L"Ub1"},
    { 9,     CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"C���ѹ",             L"Uc1"},
    { 10,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"���򱣻���ѹ",        L"Ux1"},
    { 11,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_X,       CHANNEL_VALUE_TYPE_UNKNOWN,  L"ĸ�ߵ�ѹ",            L"Ux1"}
};

///
/// @brief
///    9-2Le��û�н����ļ�ʱʹ�õ�Ԥ����ͨ����Ϣ,ͨ��С��13��ʱʹ��
///
#define PREDEFINED_CHANNEL_INFO_9_2LE_LESS PREDEFINED_CHANNEL_INFO_9_2LE_KAIMO

///
/// @brief
///    �����ܱ��վԭ�����Լ�����P62ҳ��18ͨ��ģ��
///
static const PREDEFINED_CHANNEL_INFO PREDEFINED_CHANNEL_INFO_9_2LE_BOOK[] = {
    { 0,     CHANNEL_TYPE_SMV_TIME,        CHANNEL_PHASE_UNKOWN,   CHANNEL_VALUE_TYPE_UNKNOWN,     L"���ʱ",        L"t1"},
    { 1,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_PROTECT,     L"A�ౣ������AD1",  L"Ia1"},
    { 2,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_PROTECT,     L"A�ౣ������AD2",  L"Ia1"},
    { 3,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_PROTECT,     L"B�ౣ������AD1",  L"Ib1"},
    { 4,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_PROTECT,     L"B�ౣ������AD2",  L"Ib1"},
    { 5,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_PROTECT,     L"C�ౣ������AD1",  L"Ic1"},
    { 6,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_PROTECT,     L"C�ౣ������AD2",  L"Ic1"},
    { 7,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_PROTECT,     L"A���������",     L"Ia1"},
    { 8,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_PROTECT,     L"B���������",     L"Ib1"},
    { 9,     CHANNEL_TYPE_SMV_CURRENT,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_PROTECT,     L"C���������",     L"Ic1"},
    { 10,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"A�ౣ����ѹAD1",  L"Ua1"},
    { 11,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_A,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"A�ౣ����ѹAD2",  L"Ua1"},
    { 12,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"B�ౣ����ѹAD1",  L"Ub1"},
    { 13,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_B,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"B�ౣ����ѹAD2",  L"Ub1"},
    { 14,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"C�ౣ����ѹAD1",  L"Uc1"},
    { 15,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_C,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"C�ౣ����ѹAD2",  L"Uc1"},
    { 16,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"ĸ�ߵ�ѹAD1",     L"Ux1"},
    { 17,    CHANNEL_TYPE_SMV_VOLTAGE,     CHANNEL_PHASE_N,        CHANNEL_VALUE_TYPE_UNKNOWN,     L"ĸ�ߵ�ѹAD2",     L"Ux1"}
};

///
/// @brief
///    ����ͨ��ģ��
///
struct SmvChannelTemplate 
{
    enumSmvRecvConfigTemplateType   enTemplateType;                     ///< ģ���ʶ
    const PREDEFINED_CHANNEL_INFO*  pChannelTempalte;                   ///< ָ��ͨ��ģ��
    uint32_t                        nTemplateSize;                      ///< ͨ����Ŀ
};

#define ARRAY_SIZE(ARRAY)  (sizeof(ARRAY)/sizeof(ARRAY[0]))

#endif //end _DATA_TYPES_H 