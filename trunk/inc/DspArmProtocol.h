/// @file
///
/// @brief
///     ����DSP��ARM����ʹ�õ����ݽṹ����Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    zhangli    2013.3.13

/// �޸�
///    chao    2013.3.20   ��const���������Ϊ���Ը��õļ���DSP
///    chao    2013.3.21   ��Ӷ��DSP��ARM֮��Ľ���������
///    chao    2013.3.23   ���MAX_SMV_CIRCLE_COUNT
///    chao    2013.3.24   ��SMV_DATA_CACL_RT�����fPhaseSeqAngle��Ա���Դ洢
///                           ������λ��
///    chao    2013.3.27   �������FrameHead�ȶ���ṹ�嶨��
///    chao    2013.3.29   ���DSP2ARM_CONTROLCODE_SMV_DATA���������ڱ�ʶʵ��
///                           ����ֵ����
///    chao    2013.4.2    �޸�MSGTIME��MsgTimeData�Ķ���
///    chao    2013.4.3    ����SmvData�е�fChnannelDataΪfChannelData��д����
///    chao    2013.4.9    �޸�SmvGenData��SmvData���壬ȥ�������ĵ������
///                           Э����ص����ݣ�����Э�����ͣ�һ��ֵ/����ֵ��FT3����
///                           �������ӣ��������ӿɸ���Э�������Ҫ��������ARM��
///                           ȷ����ʹ�㷨������Э������߼��Ϸ��룬����DSP��
///                           ְ�𣬽����㷨���Ӷȣ�����������ٶ�
///    chao    2013.4.11   ���ǵ�����ʱ�����ݻ�ִδ��ͣ�����ȥ��MsgTimeData
///                           �ṹ�嶨�壬����DSP�˻��汨��ʱ�����ݣ�����Config
///                           �ṹ�����nMsgTimeCount��nDefaultTimeDif�Ķ�������
///                           ���ñ�����ɢ�ȼ�������
///    chao    2013.5.10   ��SMV_DATA_CACL_RT->SMV_DATA_CALC_RT
///                           ���θ�Ϊ��ͨ�������SmvGenData�ṹ����Ӧ����������
///                           ��SMV_GEN_DATA_CALC_RT�Ķ������ڷ��ض�ͨ����������
///                           ����
///    chao    2013.5.18   �޸�Config������MAX_SMV_CALC_COUNT��
///                           MAX_MSG_TIME_PER_CIRCLE��MAX_MSG_TIME_CALC_COUNT��
///                           �������壬����MsgTimeData�ṹ�嶨�壬����һ���Է���
///                           �������ʱ������
///    chao    2013.7.3    �޸�Error�ṹ�壬����nCommandID���Ա�����ĸ�����
///                           �Ĵ����޸�Config�ṹ�壬����nSamplingFreq������
///                           ��nRatedFreq�Ƶ�ʵĲ������ã�SMV_DATA_CALC_RT��
///                           ��ֱ��������fDCTHD�ļ�����
///
#ifndef DSP_ARM_PROTOCOL_H_
#define DSP_ARM_PROTOCOL_H_

//����ͳһ��int���Ͷ����ļ�
#ifdef _WIN32
    #include "../src/include/stdint.h"
#else
    #include "stdint.h"
#endif

///
/// @brief
///    ����ARM��DSP֮�乲���ڴ�Ĵ�С
///
#define ARM_DSP_BUF_SIZE  (32*1024)

///
/// @brief
///     SVЭ���������ֵͨ����
///
#define MAX_SMV_CHANNELS  30

///
/// @brief
///     һ���ܲ��ڲ���������ֵ��256��Ӧ������Ϊ256*50=12800
///
#define MAX_SMV_PER_CIRCLE  256
///
/// @brief
///     �������ܲ����Ĳ����������ڼ���г������Ĳ�����Ŀ
///
#define MAX_SMV_CALC_COUNT  (MAX_SMV_PER_CIRCLE*10)
///
/// @brief
///    ��������г������
///
#define MAX_HARM_COUNT  50

///
/// @brief
///    һ�δ���������ʱ����
///
#define MAX_MSG_TIME_PER_CIRCLE 468
///
/// @brief
///    ���㱨��ʱ���������
///
#define MAX_MSG_TIME_CALC_COUNT (MAX_MSG_TIME_PER_CIRCLE*10)
///
/// @brief
///     �������ɵ����ͨ����
///
#define MAX_SMV_GEN_CHANNEL_COUNT 20
///
/// @brief
///    �������ɵ�����������
///
#define MAX_SMV_GEN_COUNT 256

///
/// @brief
///     ARM��DSP�Ŀ�����������룩���������λΪ0
///
#define ARM2DSP_CONTROLCODE_CONFIG       0x01              ///< ��DSP���в�������
#define ARM2DSP_CONTROLCODE_SMV_DATA     0x02              ///< ��DSP���Ͳ���ֵ����
#define ARM2DSP_CONTROLCODE_RESET_SMV    0x03              ///< ���ò���ֵ���ݻ���
#define ARM2DSP_CONTROLCODE_RESET_TIME   0x04              ///< ���ñ������ݻ���
#define ARM2DSP_CONTROLCODE_SMV_GEN      0x05              ///< ֪ͨDSP�����ض����Ͳ�������
#define ARM2DSP_CONTROLCODE_TIME_DATA    0x06              ///< ��DSP���ͱ���ʱ������
#define ARM2DSP_CONTROLCODE_INFO         0x10              ///< �õ�DSP��Ϣ
#define ARM2DSP_CONTROLCODE_STOP_READ    0x7E              ///< ����DSP��ӦARMֹͣ���߳�
#define ARM2DSP_CONTROLCODE_QUIT         0x7F              ///< ֪ͨDSP�˳�

///
/// @brief
///     DSP�������ݸ�ARM�Ŀ�����������룩���������λΪ1
///
#define DSP2ARM_CONTROLCODE_INFO          0x81             ///< DSP����Ӳ����Ϣ
#define DSP2ARM_CONTROLCODE_SMV_DATA      0x82             ///< DSP���ص�ʵ�ʲ���ֵ
#define DSP2ARM_CONTROLCODE_SMV_CALC_RT   0x83             ///< DSP���ؼ�����Чֵ/г��������
#define DSP2ARM_CONTROLCODE_SMV_GEN_RT    0x84             ///< DSP�������ɲ�������
#define DSP2ARM_CONTROLCODE_MSG_DISP_RT   0x85             ///< DSP���ر�����ɢ������
#define DSP2ARM_CONTROLCODE_STOP_READ     0xFE             ///< DSP����ʹARMֹͣ���߳�
#define DSP2ARM_CONTROLCODE_ERROR         0xFF             ///< DSP����ִ��״̬��Ϣ

///
/// @brief
///    ����ֵ�ǵ�ѹ/����ֵ
///     xxxx xxxx һ���ֽڷָ�4λ�͵���λ����4λ���ڱ�ʶ�ǵ�ѹ���ǵ������ݣ�
///     ��4λ���ڱ�ʶ���ʮ�����Ƶ�ABC�ֱ�����һ��AD��ABC���࣬DEF�ֱ������һ��
///     AD��ABC���ࣨ���ں�����㣩
///     ��0xA1�ʹ����һ��AD��A���ѹ��0xA2�����һ��AD��A��������������һ������
///

#define SMV_VCTYPE_VOLTAGE     0x01                        ///< ����ֵ�ǵ�ѹֵ
#define SMV_VCTYPE_CURRENT     0x02                        ///< ����ֵ�ǵ���ֵ
#define SMV_VCTYPE_PHASE_A     0xA0                        ///< ����AD0��A��
#define SMV_VCTYPE_PHASE_B     0xB0                        ///< ����AD0��B��
#define SMV_VCTYPE_PHASE_C     0xC0                        ///< ����AD0��C��
#define SMV_VCTYPE_PHASE_D     0xD0                        ///< ����AD1��A��
#define SMV_VCTYPE_PHASE_E     0xE0                        ///< ����AD1��B��
#define SMV_VCTYPE_PHASE_F     0xF0                        ///< ����AD1��C��

///
/// @brief
///     FrameHead�ṹ����ARM��DSP֮��������ݽ���������֡��֡ͷ������֡Ϊ֡ͷ+
///     ����
///
typedef struct _FrameHead
{
    uint8_t nCommandID;                                             ///< ARM��DSP�Ŀ�����������룩
    uint32_t nDataLength;                                           ///< ���ݳ���
}FrameHead, *PFrameHead;

///
/// @brief
///    DSP���ýṹ��
///
typedef struct _Config
{
    uint8_t  nASDUCount;                                            ///< ASDU�ĸ���
    uint16_t nMsgTimeCalcCount;                                     ///< ����ʱ������ÿ�ﵽ�������ı���ʱ����������㱨����ɢ��
	uint32_t nSamplingFreq;                                         ///< ����Ƶ��
	uint16_t nRatedFreq;                                            ///< �Ƶ��
}Config, *PConfig;

///
/// @brief
///    ��������Ҫ�����Ĳ������ݽṹ��
///
typedef struct
{
    uint8_t nChannelCount;                                                ///< Ҫ���ɲ��ε�ͨ����    
    uint16_t nSmvCount;                                                   ///< Ҫ���ɵĸ�ͨ���Ĳ�������
    uint16_t nSampleRate;                                                 ///< ������

    uint8_t nHarmCount[MAX_SMV_GEN_CHANNEL_COUNT];                        ///< ��ͨ�����ε�г������
    float   fDCValue[MAX_SMV_GEN_CHANNEL_COUNT];                          ///< ��ͨ�����ε�ֱ������
    float   fScaleFactor[MAX_SMV_GEN_CHANNEL_COUNT];                      ///< ��ͨ������Ҫ���������ݵı������ӣ���������Ҫ�Ĳ���ֵ��������

    float   fRmsValue[MAX_SMV_GEN_CHANNEL_COUNT][MAX_HARM_COUNT];         ///< ��ͨ������Ҫ�������θ���г������Чֵ
    float   fPhaseAngle[MAX_SMV_GEN_CHANNEL_COUNT][MAX_HARM_COUNT];       ///< ��ͨ������Ҫ�������θ���г������λ
    float   fFreq[MAX_SMV_GEN_CHANNEL_COUNT][MAX_HARM_COUNT];             ///< ��ͨ������Ҫ�������θ���г����Ƶ��
} SmvGenData;

///
/// @brief
///     ����ARM���͵�DSP�Ĳ���ֵ���ݽṹ
///
typedef struct
{
    uint8_t  nChannelCount;                                         ///< ʵ�ʴ����ͨ����Ŀ
    uint16_t nSmvPerCircle;                                         ///< ʵ�ʴ��������һ���ܲ��Ĳ�����
    uint8_t  nChannelVCType[MAX_SMV_CHANNELS];                      ///< ÿ��ͨ���ĵ�ѹ/��������
    float    fScaleFactor[MAX_SMV_CHANNELS];                        ///< ÿ��ͨ���ı�������
    float    fChannelData[MAX_SMV_CHANNELS][MAX_SMV_PER_CIRCLE];    ///< һ���ܲ��ڸ���ͨ���Ĳ�������
} SmvData, *PSmvData;

///
/// @brief
///    DSP��Ϣ�ṹ��
///
typedef struct _DspInfo
{
    uint8_t nVersion;                                               ///< DSP�汾��
}DspInfo, *PDspInfo;

///
/// @brief
///    ������Ϣ�ṹ��
///
typedef struct _Error
{
    uint8_t nCommandID;                                             ///< ���͵������룬����ָʾ���Ͷ˵���������
    uint16_t nErrno;                                                ///< DSP���صĴ�����
}Error, *PError;

///
/// @brief
///    ����ʱ��ṹ��
///
typedef struct _MSGTIME
{
    uint32_t nSecond;                                               ///< ��
    uint32_t nMicrosecond;                                          ///< ΢��
}MSGTIME;

///
/// @brief
///    ����ʱ�������֡��ʽ
///
typedef struct
{
    uint16_t nMsgTimeCount;                                         ///< ÿ�δ���ı���ʱ����
    MSGTIME  msgTimes[MAX_MSG_TIME_PER_CIRCLE];                     ///< ÿ�δ���ı���ʱ������
}MsgTimeData, *PMsgTimeData;

///
/// @brief
///    ���㲨�����ɷ��ص�����֡��ʽ
///
typedef struct{
    uint8_t nChannelCount;
    uint16_t nSmvCount;                                             ///< Ҫ���ɵĸ�ͨ���Ĳ�������    
    float fSmvGenData[MAX_SMV_GEN_CHANNEL_COUNT][MAX_SMV_GEN_COUNT];
} SMV_GEN_DATA_CALC_RT;

///
/// @brief
///    ���㱨����ɢ�ȷ��ص�����֡��ʽ
///
typedef struct{
    float fDispRate[11];
} MSG_DISP_CALC_RT;
///
/// @brief
///     �������г�����ص�����֡��ʽ
///
typedef struct {
    // ͨ������
    uint8_t  nChannelCount;                                         ///< ʵ�ʴ����ͨ����Ŀ
    uint8_t  nChannelVCType[MAX_SMV_CHANNELS];                      ///< ÿ��ͨ���ĵ�ѹ/��������
    
    // Ƶ�ʺ���Чֵ
    float    fFreq[MAX_SMV_CHANNELS];                               ///< ÿ��ͨ�����ź�Ƶ��
    float    fRmsValue[MAX_SMV_CHANNELS];                           ///< ͨ��������Чֵ
    
    // г��
    float    fHarmContent[MAX_SMV_CHANNELS][MAX_HARM_COUNT];        ///< ��ͨ��г����Чֵ   xxx.xxV xx.xxxA OK   200ms
    float    fHarmAngle[MAX_SMV_CHANNELS][MAX_HARM_COUNT];          ///< ��ͨ��г����λ    xxx.x��            OK   200ms
    float    fDCValue[MAX_SMV_CHANNELS];                            ///< DCֱ����Чֵ        xxx.xxV xx.xxxAOK      200ms 
    
    float    fDCTHD[MAX_SMV_CHANNELS];                              ///< ��ͨ��ֱ��������
    float    fHarmTHD[MAX_SMV_CHANNELS][MAX_HARM_COUNT];            ///< ��ͨ��г��������                OK   200ms
    float    fSumHarmTHD[MAX_SMV_CHANNELS];                         ///< ��ͨ����г��������                      OK   200ms
    
    // ����
    float    fPhaseSeqContent[2][3];                                ///< �������򡢸���        xxx.xxV xx.xxxA  OK    200ms
    float    fPhaseSeqAngle[2][3];                                  ///< �������򡢸��� ��λ��       xxx.xxV xx.xxxA  OK    200ms
    float    fUnbalanceRate[2];                                     ///< ���಻ƽ���
    
    // ����
    float    fActivePower[3];                                       ///< �й����ʣ��ֱ��ӦABC����
    float    fReactivePower[3];                                     ///< �޹����ʣ��ֱ��ӦABC����
    float    fApparentPower[3];                                     ///< ���ڹ��ʣ��ֱ��ӦABC����
    float    fPowerFactor[3];                                       ///< �����������ֱ��ӦABC����
    float    fTotalActivePower;                                     ///< �ܹ���
    float    fTotalReactivePower;                                   ///< ���޹�����
    float    fTotalApparentPower;                                   ///< �����ڹ���
    float    fTotalPowerFactor;                                     ///< �ܹ�������
    
    // ����
    float    fSPAngleDif[3];                                        ///< ͬ����ǲ�
    float    fSPVectorDifContent[3];                                ///< ͬ���������С
    float    fSPVectorDifAngle[3];                                  ///< ͬ��������Ƕ�
    float    fIPVectorDifContent[6];                                ///< ����������С��0-2��SMV1,3-5��SMV2
    float    fIPVectorDifAngle[6];                                  ///< ���������Ƕȣ�0-2��SMV1,3-5��SMV2
} SMV_DATA_CALC_RT;

#endif // DSP_ARM_PROTOCOL_H_
