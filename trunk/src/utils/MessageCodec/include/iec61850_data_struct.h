/// @file
///     iec61850_data_struct.h
///
/// @brief
///     ����IEC61850Э��ĳ�����������/�ṹ�ı����ӿ�
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.09.13
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#ifndef IEC61850_DATA_STRUCT_H_

#define IEC61850_DATA_STRUCT_H_

#include "src\include\stdint.h"

//
//=============================================================================
//    FT3֡�е�״̬�ֵı����ӿ�
//=============================================================================
//

///
/// @brief
///     ����FT3֡������
///
typedef enum{
	FT3_FRAME_TYPE_MU_STD = 0,       ///< �ϲ���Ԫ����ı�׼FT3֡����Ҫָ����FT3��52�ֽڱ�׼FT3����IEC60044-8���ݵ�IEC61850-9-1
	FT3_FRAME_TYPE_MU_EX,            ///< �ϲ���Ԫ�������չFT3֡����Ҫָ����FT3
	FT3_FRAME_TYPE_CTORVT            ///< ����/��ѹ�����������FT3֡
}FT3_FRAME_TYPE;

///
/// @brief
///     �������/��ѹ�������ļ����Դ�Ķ���/״̬����
///
typedef enum{
	FT3CLPS_UNKNOWN = 0,                 ///< δ֪
	FT3CLPS_MAINTAIN_OUTPUT,             ///< ������Ӧ�����ֵ�ǰ������ʡ�
	FT3CLPS_DOWN_TO_MAINTAIN_POWER,      ///< ������Ӧ���µ���ά�ֹ��ʡ�
	FT3CLPS_DECREASE_OUTPUT_POWER,       ///< ������Ӧ���µ�������ʡ�
	FT3CLPS_INCREASE_OUTPUT_POWER,       ///< ������Ӧ���ϵ�������ʡ�
	FT3CLPS_SPEED_INCREASE_OUTPUT_POWER, ///< ������Ӧ�����ϵ�������ʡ�
	FT3CLPS_FAILURE,                     ///< ����������
}FT3_CTORVT_LASER_POWER_STATUS;

class Ft3FrameStatusWord
{
public:
	Ft3FrameStatusWord();
	Ft3FrameStatusWord(uint16_t nStatusWord1, uint16_t nStatusWord2, FT3_FRAME_TYPE eFrameType);

	///
	/// @brief
	///     ���ø�״̬������FT3֡������
	///
	/// @param[in]
	///     eFrameType - FT3֡������
	///
	void SetFt3FrameType(FT3_FRAME_TYPE eFrameType);

	///
	/// @brief
	///     ��ȡ״̬��
	///
	/// @param[in]
	///     nStatusWord1 - ���ڱ���״̬��1
	/// @param[in]
	///     nStatusWord2 - ���ڱ���״̬��2
	///
	void GetStatusWord(uint16_t &nStatusWord1, uint16_t &nStatusWord2);

	///
	/// @brief
	///     ����״̬��
	///
	/// @param[in]
	///     nStatusWord1 - ״̬��1
	/// @param[in]
	///     nStatusWord2 - ״̬��2
	///
	void SetStatusWord(uint16_t nStatusWord1, uint16_t nStatusWord2);
	
	///
	/// @brief
	///     ��ȡ�豸��MU�򻥸������Ĺ���״̬��Ϣ���Ƿ�Ҫ��ά�ޣ�
	///
	/// @return
	///     0    - ���ã���Ҫ��ά�ޣ�
	///     1    - ����򱨾���Ҫ��ά�ޣ�
	///     ���� - ��ȡʧ��
	///
	int  IsMaintenanceRequired();

	///
	/// @brief
	///     �����豸��MU�򻥸������Ĺ���״̬���Ƿ�Ҫ��ά�ޣ�
	///
	/// @param[in]
	///     bRequired - true��Ҫ��ά��,�����෴
	///
	void SetMaintenanceRequired(bool bRequired);

	///
	/// @brief
	///     ��ȡ�豸��MU�򻥸������Ĺ���״̬��Ϣ���Ƿ�������״̬��
	///
	/// @return
	///     0    - ��ͨ���������У�
	///     1    - ����
	///     ���� - ��ȡʧ��
	///
	int  IsTestMode();

	///
	/// @brief
	///     �����豸��MU�򻥸������Ĺ���״̬��Ϣ���Ƿ�������״̬��
	///
	/// @param[in]
	///     bTest - true��Ϊ����,����Ϊ��ͨ
	///
	void SetTestMode(bool bTest);

	///
	/// @brief
	///     ��ȡFT3֡��CT���������
	///
	/// @return
	///     0    - ���Ϊi(t)
	///     1    - ���Ϊd(i(t))/dt
	///     ���� - ��ȡʧ��
	///
	int  GetCtOutputType();

	///
	/// @brief
	///     ����FT3֡��CT���������
	///
	/// @param[in]
	///     bOutputIt - true��CT���Ϊi(t),����Ϊd(i(t))/dt
	///
	void SetCtOutputType(bool bOutputIt);

	///
	/// @brief
	///     ��ȡ��������SCP��RangeFlag��Ϣ
	///
	/// @return
	///     0    - ��������SCP=0x01CF
	///     1    - ��������SCP=0x00E7
	///     ���� - ��ȡʧ��
	///
	int  GetRangeFlag();

	///
	/// @brief
	///     ���ñ�������SCP��RangeFlag
	///
	/// @param[in]
	///     bRangeFlag0 - true��RangeFlag=0,��SCP=0x01CF,����RangeFlag=1
	///
	void SetRangeFlag(bool bRangeFlag0);

	///
	/// @brief
	///     ��ȡMU�Ļ���ʱ��ָʾ��Ϣ
	///
	/// @return
	///     0    - ��ͨ���������У�����ʱ������Ч
	///     1    - �����ڼ䣬��ʱ������Ч
	///     ���� - ��ȡʧ��
	///
	int  IsMuWakingUp();

	///
	/// @brief
	///     ����MU�Ļ���ʱ��ָʾ
	///
	/// @param[in]
	///     bWakingUp - true��Ϊ�����ڼ�,����MU��������
	///
	/// @return
	///     true  - �ɹ�
	///     false - ʧ��
	///
	bool SetMuWakingUp(bool bWakingUp);

	///
	/// @brief
	///     ��ȡMU��ͬ��������Ϣ
	///
	/// @return
	///     0    - ���ݼ������ò�ֵ��
	///     1    - ���ݼ������ڲ�ֵ��
	///     ���� - ��ȡʧ��
	///
	int  GetMuSyncMethod();

	///
	/// @brief
	///     ����MU��ͬ������
	///
	/// @param[in]
	///     bUseInterpolation - true����ò�ֵ��,�����෴
	///
	/// @return
	///     true  - �ɹ�
	///     false - ʧ��
	///
	bool SetMuSyncMethod(bool bUseInterpolation);

	///
	/// @brief
	///     ��ȡMU��ͬ��״̬��Ϣ���Ƿ�ͬ����
	///
	/// @return
	///     0    - ����ͬ��
	///     1    - ʱ��ͬ������/��Ч
	///     ���� - ��ȡʧ��
	///
	int  IsMuNotSync();

	///
	/// @brief
	///     ����MU��ͬ��״̬
	///
	/// @param[in]
	///     bSync - true������ͬ��,�����෴
	///
	/// @return
	///     true  - �ɹ�
	///     false - ʧ��
	///
	bool SetMuSync(bool bSync);

	///
	/// @brief
	///     ��ȡ�������ļ���ʱ��ָʾ��Ϣ
	///
	/// @return
	///     0    - ��ͨ���������У�����ʱ������Ч
	///     1    - �����ڼ䣬��ʱ������Ч
	///     ���� - ��ȡʧ��
	///
	int  IsCtorVtFiring();

	///
	/// @brief
	///     ���û������ļ���ʱ��ָʾ
	///
	/// @param[in]
	///     bFiring - true��Ϊ�����ڼ�,���򻥸�����������
	///
	/// @return
	///     true  - �ɹ�
	///     false - ʧ��
	///
	bool SetCtorVtFiring(bool bFiring);

	///
	/// @brief
	///     ��ȡ��������ͬ��������Ϣ
	///
	/// @return
	///     0    - ���ݼ������ò�ֵ��
	///     1    - ���ݼ������ڲ�ֵ��
	///     ���� - ��ȡʧ��
	///
	int  GetCtorVtSyncMethod();

	///
	/// @brief
	///     ���û�������ͬ������
	///
	/// @param[in]
	///     bUseInterpolation - true����ò�ֵ��,�����෴
	///
	/// @return
	///     true  - �ɹ�
	///     false - ʧ��
	///
	bool SetCtorVtSyncMethod(bool bUseInterpolation);

	///
	/// @brief
	///     ��ȡ��������ͬ��״̬��Ϣ
	///
	/// @return
	///     0    - ����ͬ��
	///     1    - ʱ��ͬ������/��Ч
	///     ���� - ��ȡʧ��
	///
	int  IsCtorVtNotSync();

	///
	/// @brief
	///     ���û�������ͬ��״̬
	///
	/// @param[in]
	///     bSync - true������ͬ��,�����෴
	///
	/// @return
	///     true  - �ɹ�
	///     false - ʧ��
	///
	bool SetCtorVtSync(bool bSync);

	///
	/// @brief
	///     ��ȡ��������A/Dת����״̬��Ϣ
	///
	/// @return
	///     0    - A/Dת����ȷ
	///     1    - A/Dת������
	///     ���� - ��ȡʧ��
	///
	int  IsCtorVtAdError();

	///
	/// @brief
	///     ���û�������A/Dת����״̬
	///
	/// @param[in]
	///     bError - true��A/Dת������,�����෴
	///
	/// @return
	///     true  - �ɹ�
	///     false - ʧ��
	///
	bool SetCtorVtAdError(bool bError);

	///
	/// @brief
	///     ��ȡ��������1�ŵ�Դ��״̬��Ϣ
	///
	/// @return
	///     0    - ����
	///     1    - �쳣
	///     ���� - ��ȡʧ��
	///
	int  IsCtorVtPower1stAbnor();

	///
	/// @brief
	///     ���û�������1�ŵ�Դ��״̬
	///
	/// @param[in]
	///     bAbnor - true���쳣,�����෴
	///
	/// @return
	///     true  - �ɹ�
	///     false - ʧ��
	///
	bool SetCtorVtPower1stAbnor(bool bAbnor);

	///
	/// @brief
	///     ��ȡ��������2�ŵ�Դ��״̬��Ϣ
	///
	/// @return
	///     0    - ����
	///     1    - �쳣
	///     ���� - ��ȡʧ��
	///
	int  IsCtorVtPower2ndAbnor();

	///
	/// @brief
	///     ���û�������2�ŵ�Դ��״̬
	///
	/// @param[in]
	///     bAbnor - true���쳣,�����෴
	///
	/// @return
	///     true  - �ɹ�
	///     false - ʧ��
	///
	bool SetCtorVtPower2ndAbnor(bool bAbnor);

	///
	/// @brief
	///     ��ȡ�������ĸ�ѹ��Դ����Ч����Ϣ
	///
	/// @return
	///     0    - ��Ч
	///     1    - ��Ч
	///     ���� - ��ȡʧ��
	///
	int  IsCtorVtHvpsInvalid();

	///
	/// @brief
	///     ���û������ĸ�ѹ��Դ����Ч��
	///
	/// @param[in]
	///     bValid - true����Ч,�����෴
	///
	/// @return
	///     true  - �ɹ�
	///     false - ʧ��
	///
	bool SetCtorVtHvpsValidity(bool bValid);

	///
	/// @brief
	///     ��ȡ�������ļ����Դ�Ķ���/״̬��Ϣ
	///
	/// @return
	///     �����Դ�Ķ���/״̬
	///
	FT3_CTORVT_LASER_POWER_STATUS GetCtorVtLaserPowerStatus();

	///
	/// @brief
	///     ���û������ļ����Դ�Ķ���/״̬
	///
	/// @param[in]
	///     eStatus -  �����Դ�Ķ���/״̬
	///
	/// @return
	///     true  - �ɹ�
	///     false - ʧ��
	///
	bool SetCtorVtLaserPowerStatus(FT3_CTORVT_LASER_POWER_STATUS eStatus);

	///
	/// @brief
	///     ��ȡFT3֡�и�ͨ�����ݵ���Ч����Ϣ
	///
	/// @param[in]
	///     nChannelIdx - ͨ����˳��ţ���0��ʼ
	///
	/// @return
	///     0    - ��Ч
	///     1    - ��Ч
	///     ���� - ��ȡʧ��
	///
	int  IsChannelInvalid(uint32_t nChannelIdx);

	///
	/// @brief
	///     ����FT3֡�и�ͨ�����ݵ���Ч��
	///
	/// @param[in]
	///     nChannelIdx - ͨ����˳��ţ���0��ʼ
	/// @param[in]
	///     bValid      - true����Ч,�����෴
	///
	/// @return
	///     true  - �ɹ�
	///     false - ʧ��
	///
	bool SetChannelValidity(uint32_t nChannelIdx, bool bValid);

	///
	/// @brief
	///     ��ȡ״̬���и�����λ��ֵ
	///
	/// @param[in]
	///     nBitIdx - ����λ��˳��ţ���0��ʼ
	///
	/// @return
	///     ��λ��ֵ - 0��1
	///     ����     - ��ȡʧ��
	///
	int  GetFutureUsedBitVal(uint32_t nBitIdx);

	///
	/// @brief
	///     ����״̬���и�����λ��ֵ
	///
	/// @param[in]
	///     nBitIdx - ͨ����˳��ţ���0��ʼ
	/// @param[in]
	///     bBitVal - true��Ϊ1,����Ϊ0
	///
	/// @return
	///     true  - �ɹ�
	///     false - ʧ��
	///
	bool SetFutureUsedBitVal(uint32_t nBitIdx, bool bBitVal);

private:
	uint16_t m_nStatusWord1;
	uint16_t m_nStatusWord2;
	FT3_FRAME_TYPE m_eFrameType;
	uint16_t m_nChannelBitsNum;
	uint16_t m_nFutureUseBitsNum;
	uint8_t *m_pChannelBit;
	uint8_t *m_pFutureUseBit;
};

//
//=============================================================================
//    IEC61850�����TimeStamp�����е�TimeQuality�ı����ӿ�
//=============================================================================
//
class Iec61850TimeQuality
{
public:
	Iec61850TimeQuality();
	Iec61850TimeQuality(uint8_t tq);

	///
	/// @brief
	///     ��ȡTimeQuality
	///
	/// @return
	///     TimeQualityֵ
	///
	uint8_t GetTimeQuality();

	///
	/// @brief
	///     ����TimeQuality
	///
	/// @param[in]
	///     tq - TimeQualityֵ
	///
	void SetTimeQuality(uint8_t tq);

	///
	/// @brief
	///     �ж������Ƿ���֪
	///
	/// @return
	///     true  - ��֪
	///     false - δ֪
	///
	bool IsLeapSecondKnown();

	///
	/// @brief
	///     ���������Ƿ���֪
	///
	/// @param[in]
	///     bKnown - true����֪,�����෴
	///
	void SetLeapSecondKnown(bool bKnown);

	///
	/// @brief
	///     �ж�ʱ���Ƿ����
	///
	/// @return
	///     true  - ����
	///     false - ����
	///
	bool IsClockFailure();

	///
	/// @brief
	///     ����ʱ�ӹ���״̬
	///
	/// @param[in]
	///     bFail - true��ʱ�ӹ���,�����෴
	///
	void SetClockFailure(bool bFail);

	///
	/// @brief
	///     �ж�ʱ���Ƿ�δͬ��
	///
	/// @return
	///     true  - δͬ��
	///     false - ͬ��
	///
	bool IsClockNotSync();

	///
	/// @brief
	///     ����ʱ��ͬ��״̬
	///
	/// @param[in]
	///     bSync - true��ʱ��ͬ��,�����෴
	///
	void SetClockSync(bool bSync);

	///
	/// @brief
	///     ��ȡIEC61850ʱ��������С�����ֵ�ʱ�侫��
	///
	/// @return
	///     -1   - �Ƿ�
	///     -2   - δ�涨
	///     ���� - ��ֵ��Ӧ����λ���ȣ�0~23λ���ȣ�
	///
	int  GetSecondAccuracy();

	///
	/// @brief
	///     ����IEC61850ʱ��������С�����ֵ�ʱ�侫��
	///
	/// @param[in]
	///     nAccuracy - ��Ӧ����λ���ȵ���ֵ��0~23λ���ȣ�
	///
	/// @return
	///     true  - �ɹ�
	///     false - ʧ��
	///
	bool SetSecondAccuracy(int nAccuracy);

private:
	uint8_t m_nTimeQuality;
};

//
//=============================================================================
//    IEC61850����Ļ�������Quality�ı����ӿ�
//    ע�⣺
//    1��Iec61850SmvQuality������SMV����Iec61850GooseQuality������Goose��
//       �������Ի���һ�£�����SMV����Ϊuint32����Goose������Ϊbit-string��
//    2��IEC61850-9-2LE ��ʹ��validity��test��derived�������ԣ���
//       derived����ΪIEC61850-9-2LE ��Quality���͵����䶨�塣
//=============================================================================
//

///
/// @brief
///     �����й�ֵ�ĵĺϷ�������
///
typedef enum{
	IEC61850_QV_GOOD = 0,             ///< ��
	IEC61850_QV_INVALID,              ///< �Ƿ�/��Ч
	IEC61850_QV_RESERVED,             ///< ����
	IEC61850_QV_QUESTIONABLE          ///< ����
}IEC61850_QUALITY_VALIDITY_TYPE;

///
/// @brief
///     �����й�ֵ����Դ����
///
typedef enum{
	IEC61850_QST_PROCESS = 0,         ///< ���̣���ʾ���ֵ�ɹ���I/O�������ṩ������ĳЩӦ�ù��ܼ������
	IEC61850_QST_SUBSTITUTED,         ///< ��ȡ������ʾ���ֵ�в���Ա����������Զ�Դ�ṩ
}IEC61850_QUALITY_SOURCE_TYPE;

class Iec61850SmvQuality
{
public:
	Iec61850SmvQuality();
	Iec61850SmvQuality(uint32_t q);

	///
	/// @brief
	///     ��ȡQuality��Ʒ��������ֵ
	///
	/// @return
	///     Quality��Ʒ��������ֵ
	///
	uint32_t GetQuality();

	///
	/// @brief
	///     ����Quality��Ʒ��������ֵ
	///
	/// @param[in]
	///     q - Quality��Ʒ��������ֵ
	///
	void SetQuality(uint32_t q);

	///
	/// @brief
	///     ��ȡ��Ч����Ϣ
	///
	/// @return
	///     ��Ч����Ϣ
	///
	IEC61850_QUALITY_VALIDITY_TYPE GetValidity();

	///
	/// @brief
	///     ������Ч��
	///
	/// @param[in]
	///     eValidity - ��Ч��ֵ
	///
	void SetValidity(IEC61850_QUALITY_VALIDITY_TYPE eValidity);

	///
	/// @brief
	///     �ж��й�ֵ�Ƿ����
	///
	/// @return
	///     true  - ���
	///     false - δ���
	///
	bool IsOverflow();

	///
	/// @brief
	///     �����й�ֵ�Ƿ����
	///
	/// @param[in]
	///     bOverflow - true�����,�����෴
	///
	void SetOverflow( bool bOverflow);

	///
	/// @brief
	///     �ж��й�ֵ�Ƿ����̣�����Ԥ�ȶ�������̣����������͵���Ч��Χ��
	///
	/// @return
	///     true  - ��
	///     false - ��
	///
	bool IsOutofRange();

	///
	/// @brief
	///     �����й�ֵ�Ƿ����̣�����Ԥ�ȶ�������̣����������͵���Ч��Χ��
	///
	/// @param[in]
	///     bOutofRange - true������,�����෴
	///
	void SetOutofRange(bool bOutofRange);

	///
	/// @brief
	///     �ж��й�ֵ�Ƿ񻵻�׼ֵ����׼ֵ�޷�У׼��
	///
	/// @return
	///     true  - ��
	///     false - ��
	///
	bool IsBadReference();

	///
	/// @brief
	///     �����й�ֵ�Ƿ񻵻�׼ֵ����׼ֵ�޷�У׼��
	///
	/// @param[in]
	///     bBadReference - true�򻵻�׼ֵ,�����෴
	///
	void SetBadReference(bool bBadReference);

	///
	/// @brief
	///     �ж��й�ֵ�Ƿ��񵴣�������
	///
	/// @return
	///     true  - ��
	///     false - ��
	///
	bool IsOscillatory();

	///
	/// @brief
	///     �����й�ֵ�Ƿ��񵴣�������
	///
	/// @param[in]
	///     bOscillatory - true�򶶶�,�����෴
	///
	void SetOscillatory(bool bOscillatory);

	///
	/// @brief
	///     �ж��й�ֵ�Ƿ�Ϊ����ֵ�����ӹ��ܼ��һ���ڲ����ⲿ���ϣ�
	///
	/// @return
	///     true  - ��
	///     false - ��
	///
	bool IsFailure();

	///
	/// @brief
	///     �����й�ֵ�Ƿ����ֵ�����ӹ��ܼ��һ���ڲ����ⲿ���ϣ�
	///
	/// @param[in]
	///     bFail - true�����,�����෴
	///
	void SetFailure(bool bFail);

	///
	/// @brief
	///     �ж��й�ֵ�Ƿ�Ϊ�����ݣ��ڸ���ʱ�����ڲ����ã�
	///
	/// @return
	///     true  - ��
	///     false - ��
	///
	bool IsOldData();

	///
	/// @brief
	///     �����й�ֵ�Ƿ�����ݣ��ڸ���ʱ�����ڲ����ã�
	///
	/// @param[in]
	///     bOldData - true��Ϊ������,�����෴
	///
	void SetOldData(bool bOldData);

	///
	/// @brief
	///     �ж��й�ֵ�Ƿ�һ�£����������Ѽ����һ�£�
	///
	/// @return
	///     true  - ��
	///     false - ��
	///
	bool IsInconsistent();

	///
	/// @brief
	///     �����й�ֵ�Ƿ�һ�£����������Ѽ����һ�£�
	///
	/// @param[in]
	///     bInconsistent - true��Ϊ��һ��,�����෴
	///
	void SetInconsistent(bool bInconsistent);

	///
	/// @brief
	///     �ж��й�ֵ�Ƿ񲻾�ȷ����ֵ������Դ�������ľ��ȣ�
	///
	/// @return
	///     true  - ��
	///     false - ��
	///
	bool IsInaccurate();

	///
	/// @brief
	///     �����й�ֵ�Ƿ񲻾�ȷ����ֵ������Դ�������ľ��ȣ�
	///
	/// @param[in]
	///     bInaccurate - true��Ϊ����ȷ,�����෴
	///
	void SetInaccurate(bool bInaccurate);

	///
	/// @brief
	///     ��ȡ�й�ֵ����Դ��Ϣ
	///
	/// @return
	///     ��Դ������
	///
	IEC61850_QUALITY_SOURCE_TYPE GetSourceType();

	///
	/// @brief
	///     �����й�ֵ����Դ����
	///
	/// @param[in]
	///     eSourceType - ��Դ������
	///
	void SetSourceType(IEC61850_QUALITY_SOURCE_TYPE eSourceType);

	///
	/// @brief
	///     �ж��Ƿ��ڲ���/����״̬���й�ֵΪ���ڲ��Ե�ֵ��������������Ŀ�ģ�
	///
	/// @return
	///     true  - ��
	///     false - ��
	///
	bool IsTest();

	///
	/// @brief
	///     �����Ƿ��ڲ���/����״̬���й�ֵΪ���ڲ��Ե�ֵ��������������Ŀ�ģ�
	///
	/// @param[in]
	///     bTest - true���ڲ���/����״̬,�����෴
	///
	void SetTest(bool bTest);

	///
	/// @brief
	///     �ж�ֵ��ˢ���Ƿ񱻲���Ա���������й�ֵΪ����ǰ��ֵ��
	///
	/// @return
	///     true  - ��
	///     false - ��
	///
	bool IsOperatorBlocked();

	///
	/// @brief
	///     ����ֵ��ˢ���Ƿ񱻲���Ա���������й�ֵΪ����ǰ��ֵ��
	///
	/// @param[in]
	///     bOperatorBlocked - true�򱻲���Ա����,�����෴
	///
	void SetOperatorBlocked(bool bOperatorBlocked);

	///
	/// @brief
	///     �ж��й�ֵ�Ƿ�������ͨ�����㣩����
	///
	/// @return
	///     true  - ��
	///     false - ��
	///
	bool IsDerived();

	///
	/// @brief
	///     �����й�ֵ�Ƿ�������ͨ�����㣩����
	///
	/// @param[in]
	///     bDerived - true��Ϊ����,�����෴
	///
	void SetDerived(bool bDerived);
private:
	uint32_t m_nQuality;
};

class Iec61850GooseQuality
{
public:
	Iec61850GooseQuality();
	Iec61850GooseQuality(uint16_t q);

	///
	/// @brief
	///     ��ȡQuality��Ʒ��������ֵ
	///
	/// @return
	///     Quality��Ʒ��������ֵ
	///
	uint16_t GetQuality();

	///
	/// @brief
	///     ����Quality��Ʒ��������ֵ
	///
	/// @param[in]
	///     q - Quality��Ʒ��������ֵ
	///
	void SetQuality(uint16_t q);

	///
	/// @brief
	///     ��ȡ��Ч����Ϣ
	///
	/// @return
	///     ��Ч����Ϣ
	///
	IEC61850_QUALITY_VALIDITY_TYPE GetValidity();

	///
	/// @brief
	///     ������Ч��
	///
	/// @param[in]
	///     eValidity - ��Ч��ֵ
	///
	void SetValidity(IEC61850_QUALITY_VALIDITY_TYPE eValidity);

	///
	/// @brief
	///     �ж��й�ֵ�Ƿ����
	///
	/// @return
	///     true  - ���
	///     false - δ���
	///
	bool IsOverflow();

	///
	/// @brief
	///     �����й�ֵ�Ƿ����
	///
	/// @param[in]
	///     bOverflow - true�����,�����෴
	///
	void SetOverflow( bool bOverflow);

	///
	/// @brief
	///     �ж��й�ֵ�Ƿ����̣�����Ԥ�ȶ�������̣����������͵���Ч��Χ��
	///
	/// @return
	///     true  - ��
	///     false - ��
	///
	bool IsOutofRange();

	///
	/// @brief
	///     �����й�ֵ�Ƿ����̣�����Ԥ�ȶ�������̣����������͵���Ч��Χ��
	///
	/// @param[in]
	///     bOutofRange - true������,�����෴
	///
	void SetOutofRange(bool bOutofRange);

	///
	/// @brief
	///     �ж��й�ֵ�Ƿ񻵻�׼ֵ����׼ֵ�޷�У׼��
	///
	/// @return
	///     true  - ��
	///     false - ��
	///
	bool IsBadReference();

	///
	/// @brief
	///     �����й�ֵ�Ƿ񻵻�׼ֵ����׼ֵ�޷�У׼��
	///
	/// @param[in]
	///     bBadReference - true�򻵻�׼ֵ,�����෴
	///
	void SetBadReference(bool bBadReference);

	///
	/// @brief
	///     �ж��й�ֵ�Ƿ��񵴣�������
	///
	/// @return
	///     true  - ��
	///     false - ��
	///
	bool IsOscillatory();

	///
	/// @brief
	///     �����й�ֵ�Ƿ��񵴣�������
	///
	/// @param[in]
	///     bOscillatory - true�򶶶�,�����෴
	///
	void SetOscillatory(bool bOscillatory);

	///
	/// @brief
	///     �ж��й�ֵ�Ƿ�Ϊ����ֵ�����ӹ��ܼ��һ���ڲ����ⲿ���ϣ�
	///
	/// @return
	///     true  - ��
	///     false - ��
	///
	bool IsFailure();

	///
	/// @brief
	///     �����й�ֵ�Ƿ����ֵ�����ӹ��ܼ��һ���ڲ����ⲿ���ϣ�
	///
	/// @param[in]
	///     bFail - true�����,�����෴
	///
	void SetFailure(bool bFail);

	///
	/// @brief
	///     �ж��й�ֵ�Ƿ�Ϊ�����ݣ��ڸ���ʱ�����ڲ����ã�
	///
	/// @return
	///     true  - ��
	///     false - ��
	///
	bool IsOldData();

	///
	/// @brief
	///     �����й�ֵ�Ƿ�����ݣ��ڸ���ʱ�����ڲ����ã�
	///
	/// @param[in]
	///     bOldData - true��Ϊ������,�����෴
	///
	void SetOldData(bool bOldData);

	///
	/// @brief
	///     �ж��й�ֵ�Ƿ�һ�£����������Ѽ����һ�£�
	///
	/// @return
	///     true  - ��
	///     false - ��
	///
	bool IsInconsistent();

	///
	/// @brief
	///     �����й�ֵ�Ƿ�һ�£����������Ѽ����һ�£�
	///
	/// @param[in]
	///     bInconsistent - true��Ϊ��һ��,�����෴
	///
	void SetInconsistent(bool bInconsistent);

	///
	/// @brief
	///     �ж��й�ֵ�Ƿ񲻾�ȷ����ֵ������Դ�������ľ��ȣ�
	///
	/// @return
	///     true  - ��
	///     false - ��
	///
	bool IsInaccurate();

	///
	/// @brief
	///     �����й�ֵ�Ƿ񲻾�ȷ����ֵ������Դ�������ľ��ȣ�
	///
	/// @param[in]
	///     bInaccurate - true��Ϊ����ȷ,�����෴
	///
	void SetInaccurate(bool bInaccurate);

	///
	/// @brief
	///     ��ȡ�й�ֵ����Դ��Ϣ
	///
	/// @return
	///     ��Դ������
	///
	IEC61850_QUALITY_SOURCE_TYPE GetSourceType();

	///
	/// @brief
	///     �����й�ֵ����Դ����
	///
	/// @param[in]
	///     eSourceType - ��Դ������
	///
	void SetSourceType(IEC61850_QUALITY_SOURCE_TYPE eSourceType);

	///
	/// @brief
	///     �ж��Ƿ��ڲ���/����״̬���й�ֵΪ���ڲ��Ե�ֵ��������������Ŀ�ģ�
	///
	/// @return
	///     true  - ��
	///     false - ��
	///
	bool IsTest();

	///
	/// @brief
	///     �����Ƿ��ڲ���/����״̬���й�ֵΪ���ڲ��Ե�ֵ��������������Ŀ�ģ�
	///
	/// @param[in]
	///     bTest - true���ڲ���/����״̬,�����෴
	///
	void SetTest(bool bTest);

	///
	/// @brief
	///     �ж�ֵ��ˢ���Ƿ񱻲���Ա���������й�ֵΪ����ǰ��ֵ��
	///
	/// @return
	///     true  - ��
	///     false - ��
	///
	bool IsOperatorBlocked();

	///
	/// @brief
	///     ����ֵ��ˢ���Ƿ񱻲���Ա���������й�ֵΪ����ǰ��ֵ��
	///
	/// @param[in]
	///     bOperatorBlocked - true�򱻲���Ա����,�����෴
	///
	void SetOperatorBlocked(bool bOperatorBlocked);

protected:
	uint16_t uint16_BitReverse(uint16_t inData);

private:
	uint16_t m_nQuality;
};

#endif
