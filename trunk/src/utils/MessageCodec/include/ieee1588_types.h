/// @file
///     ieee1588_types.h
///
/// @brief
///     ���徫��ʱ��ͬ��Э�� IEEE1588 ����֡����������ʹ�õ���������
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.09.29
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#ifndef IEEE1588_TYPES_H_

#define IEEE1588_TYPES_H_

#include "src\include\stdint.h"

///
/// @brief
///     IEEE1588Э��ı�������
///
typedef enum
{
	IEEE1588_TYPE_UNKNOWN = 0,                 ///< δ֪
	IEEE1588_TYPE_SYNC,                        ///< ͬ������
	IEEE1588_TYPE_DELAY_REQUEST,               ///< �ӳ�������Ӧ���Ƶ�������
	IEEE1588_TYPE_PDELAY_REQUEST,              ///< �Ե��ӳٻ��Ƶ�������
	IEEE1588_TYPE_PDELAY_RESPONSE,             ///< �Ե��ӳٻ��Ƶ���Ӧ����
	IEEE1588_TYPE_FOLLOW_UP,                   ///< ���汨��
	IEEE1588_TYPE_DELAY_RESPONSE,              ///< �ӳ�������Ӧ���Ƶ���Ӧ����
	IEEE1588_TYPE_PDELAY_RESPONSE_FOLLOW_UP,   ///< �Ե��ӳٻ��Ƶĸ��汨��
	IEEE1588_TYPE_ANNOUNCE,                    ///< Announce�����е���Ϣ��Ҫ����ϵͳ�������ʱ�ӵ�ѡ�����
	IEEE1588_TYPE_SIGNALING,                   ///< Signaling��������PTPʱ�ӽڵ������Ŀ�ĵ�ͨѶ
	IEEE1588_TYPE_MANAGEMENT                   ///< Management��Ϣ���ڲ�ѯ�͸���ʱ����ά����PTP���ݼ�
}IEEE1588_MESSAGE_TYPE;

///
/// @brief
///     IEEE1588���ĵ�������Ϣ�ֶ�
///
typedef struct
{
	uint8_t  transportSpecific : 4;            ///< ��ʶ����IEEE1588ͨ�ŵĵײ�Э�飬Ĭ��Ϊ0
	uint8_t  messageType : 4;                  ///< ��ǰ�������������ͣ�Sync��Follow_Up...��

	///
	/// @brief
	///     ��ȡ��ǰ��������������
	///
	/// @return
	///     ��ǰ��������������
	///
	IEEE1588_MESSAGE_TYPE GetMessageType();

	///
	/// @brief
	///     ���õ�ǰ��������������
	///
	/// @param[in]
	///     eMessageType - ��ǰ��������������
	///
	void SetMessageType(IEEE1588_MESSAGE_TYPE eMessageType);

}IEEE1588_TYPE_FIELD;

///
/// @brief
///     IEEE1588���ĵİ汾��Ϣ�ֶ�
///
typedef struct
{
	uint8_t  reserved : 4;                     ///< ���ڽ�����չ
	uint8_t  versionPTP : 4;                   ///< IEEE1588Э��İ汾�ţ�V1��V2
}IEEE1588_VERSION_FIELD;

///
/// @brief
///     IEEE1588���ĵı���ͷ�еı�־λ����
///
typedef struct
{
	uint16_t iFlags;

	///
	/// @brief
	///     �жϷ����˿��Ƿ�Ϊ��Master״̬
	///
	/// @return
	///     �� - true
	///     �� - false
	///
	bool IsPtpAlternateMaster();

	///
	/// @brief
	///     ���ñ��ķ����˿��Ƿ�Ϊ��Master״̬
	///
	/// @param[in]
	///     bAlternateMaster - Ϊ��Master״̬��Ϊtrue�������෴
	///
	void SetPtpAlternateMaster(bool bAlternateMaster);

	///
	/// @brief
	///     �ж�ͬ��ʱ���Ƿ�Ϊ������ʱ��
	///
	/// @return
	///     �� - true
	///     �� - false
	///
	bool IsPtpTwoStep();

	///
	/// @brief
	///     ����ͬ��ʱ���Ƿ�Ϊ������ʱ��
	///
	/// @param[in]
	///     bTwoStep - ������ʱ����Ϊtrue��������ʱ����Ϊfalse
	///
	void SetPtpTwoStep(bool bTwoStep);

	///
	/// @brief
	///     �жϸñ����Ƿ��Ե�����ʽ����
	///
	/// @return
	///     �Ե�����ʽ���� - true
	///     �Զಥ��ʽ���� - false
	///
	bool IsPtpUnicast();

	///
	/// @brief
	///     ���øñ����Ƿ��Ե�����ʽ����
	///
	/// @param[in]
	///     bTwoStep - �Ե�����ʽ������Ϊtrue���Զಥ��ʽ������Ϊfalse
	///
	void SetPtpUnicast(bool bUnicast);

	///
	/// @brief
	///     �ж�ʱ���Ƿ�alternate PTP�����ļ����壨��־1��
	///
	/// @return
	///     �� - true
	///     �� - false
	///
	bool IsPtpProfileSpecific1();

	///
	/// @brief
	///     ����ʱ���Ƿ�alternate PTP�����ļ����壨��־1��
	///
	/// @param[in]
	///     bSpecific - ����Ϊtrue�������෴
	///
	void SetPtpProfileSpecific1(bool bSpecific);

	///
	/// @brief
	///     �ж�ʱ���Ƿ�alternate PTP�����ļ����壨��־2��
	///
	/// @return
	///     �� - true
	///     �� - false
	///
	bool IsPtpProfileSpecific2();

	///
	/// @brief
	///     ����ʱ���Ƿ�alternate PTP�����ļ����壨��־2��
	///
	/// @param[in]
	///     bSpecific - ����Ϊtrue�������෴
	///
	void SetPtpProfileSpecific2(bool bSpecific);

	///
	/// @brief
	///     �жϱ����Ƿ��������Ϣ��ȫ����
	///
	/// @return
	///     �� - true
	///     �� - false
	///
	bool IsPtpSecurity();

	///
	/// @brief
	///     ���ñ����Ƿ��������Ϣ��ȫ����
	///
	/// @param[in]
	///     bSecurity - ����Ϊtrue�������෴
	///
	void SetPtpSecurity(bool bSecurity);

	///
	/// @brief
	///     �жϵ������1min�Ƿ�Ϊ61s
	///
	/// @return
	///     �� - true
	///     �� - false
	///
	bool IsPtpLI61();

	///
	/// @brief
	///     ���õ������1min�Ƿ�Ϊ61s
	///
	/// @param[in]
	///     bLI61 - ���ڲ���PTPʱԪ�Ķ�ʱϵͳ����ֵΪtrue���ʾ�������1minΪ61s��
	///             ���ڲ�����PTPʱԪ�Ķ�ʱϵͳ����ֵΪfalse��
	///
	void SetPtpLI61(bool bLI61);

	///
	/// @brief
	///     �жϵ������1min�Ƿ�Ϊ59s
	///
	/// @return
	///     �� - true
	///     �� - false
	///
	bool IsPtpLI59();

	///
	/// @brief
	///     ���õ������1min�Ƿ�Ϊ59s
	///
	/// @param[in]
	///     bLI59 - ���ڲ���PTPʱԪ�Ķ�ʱϵͳ����ֵΪtrue���ʾ�������1minΪ59s��
	///             ���ڲ�����PTPʱԪ�Ķ�ʱϵͳ����ֵΪfalse��
	///
	void SetPtpLI59(bool bLI59);

	///
	/// @brief
	///     �ж��Ƿ����UTC��ʾʱ����Ϣ
	///
	/// @return
	///     �� - true
	///     �� - false
	///
	bool IsPtpUtcReasonable();

	///
	/// @brief
	///     �����Ƿ����UTC��ʾʱ����Ϣ
	///
	/// @param[in]
	///     bUtcReasonable - ����Ϊtrue�������෴
	///
	void SetPtpUtcReasonable(bool bUtcReasonable);

	///
	/// @brief
	///     �ж���߼�����ʱ���Ƿ����PTPʱ����
	///
	/// @return
	///     �� - true
	///     �� - false
	///
	bool IsPtpTimescale();

	///
	/// @brief
	///     ������߼�����ʱ���Ƿ����PTPʱ����
	///
	/// @param[in]
	///     bTimescale - ����Ϊtrue�������෴
	///
	void SetPtpTimescale(bool bTimescale);

	///
	/// @brief
	///     �ж�ʱ���ߵ�ʱ��ƫ���Ƿ��׷�ݵ���߼�����ʱ��
	///
	/// @return
	///     �� - true
	///     �� - false
	///
	bool IsTimeTraceble();

	///
	/// @brief
	///     ����ʱ���ߵ�ʱ��ƫ���Ƿ��׷�ݵ���߼�����ʱ��
	///
	/// @param[in]
	///     bTimeTraceble - ����Ϊtrue�������෴
	///
	void SetTimeTraceble(bool bTimeTraceble);

	///
	/// @brief
	///     �ж�Ƶ�ʾ�������Ƿ����׷�ݵ���߼�����ʱ��
	///
	/// @return
	///     �� - true
	///     �� - false
	///
	bool IsFrequencyTraceable();

	///
	/// @brief
	///     ����Ƶ�ʾ�������Ƿ����׷�ݵ���߼�����ʱ��
	///
	/// @param[in]
	///     bTraceable - ����Ϊtrue�������෴
	///
	void SetFrequencyTraceable(bool bTraceable);

}IEEE1588_HEADER_FLAGS;

///
/// @brief
///     IEEE1588���ķ����˿ڣ�Դ�˿ڣ�����Ϣ
///
typedef struct
{
	uint64_t clockIdentity;                    ///< ������ǰ���ĵ�ʱ�ӵĵ�ַ��ÿ��ʱ�Ӷ�ӵ��һ��Ψһֵ��
	uint16_t sourcePortID;                     ///< ��ʱ���ϵĶ˿ں�
}IEEE1588_SOURCE_PORT_ID;

///
/// @brief
///     IEEE1588���ĵı���ͷ
///
typedef struct
{
	IEEE1588_TYPE_FIELD type;                  ///< ��ǰ���ĵ�������Ϣ
	IEEE1588_VERSION_FIELD version;            ///< ��ǰ���ĵİ汾��Ϣ
	uint16_t messageLength;                    ///< ��ǰ���ĵĳ��ȣ��ӱ���ͷ��һ���ֽڿ�ʼ������������׺��ռ���ֽ���
	uint8_t  domainNumber;                     ///< ��ʾPTP��ţ�ȡֵ��ΧΪ0~255��Ĭ��ֵΪ0
	uint8_t  reserved1;                        ///< �����ֶ�1
	IEEE1588_HEADER_FLAGS flags;               ///< IEEE1588��־λ���ϣ�ֻ�õ����ֶε�����12bit������4bitΪ����λ����Щ����λ��Ĭ��ֵΪFALSE
	uint64_t correctionField;                  ///< ��ʾ��������ֵ��Ҫ����2��-16�η����ܻ����������Ϊ��λ����ʵ������ʱ��ֵ
	uint32_t reserved2;                        ///< �����ֶ�2
	IEEE1588_SOURCE_PORT_ID sourcePortIdentity;///< ��ǰ���ķ����˿ڵ�Ψһ��ʶ
	uint16_t sequenceId;                       ///< ��ʾPTP���ĵĵ������
	uint8_t  controlField;                     ///< ���ø��ֶε�Ŀ������IEEE1588��׼��1�汣�ּ���
	uint8_t  logMessageInterval;               ///< ����������֡ͬ���ͱ��ĵı���֮���ƽ�����ͼ������ʵֵΪlog2(t)
}IEEE1588_HEADER;

///
/// @brief
///     IEEE1588���ĵ�ʱ���
///
typedef struct
{
	uint64_t t_s;                              ///< ʱ������������֣���λΪ�룬��ΧΪ0~2^48-1
	uint32_t t_ns;                             ///< ʱ�����С�����֣���λΪ���룬��ΧΪ0~10^9-1
}IEEE1588_TIMESTAMP;

///
/// @brief
///     IEEE1588���ķ����˿ڣ�Դ�˿ڣ�����Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
typedef struct
{
	size_t clockIdentityOff;                   ///< ������ǰ���ĵ�ʱ�ӵĵ�ַ��ƫ�Ƶ�ַ
	size_t sourcePortIDOff;                    ///< ��ʱ���ϵĶ˿ںŵ�ƫ�Ƶ�ַ
}IEEE1588_SOURCE_PORT_ID_OFFSET;

///
/// @brief
///     IEEE1588���ĵı���ͷ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
typedef struct
{
	size_t typeOff;                            ///< ��ǰ���ĵ�������Ϣ��ƫ�Ƶ�ַ
	size_t versionOff;                         ///< ��ǰ���ĵİ汾��Ϣ��ƫ�Ƶ�ַ
	size_t messageLengthOff;                   ///< ��ǰ���ĵĳ��ȵ�ƫ�Ƶ�ַ
	size_t domainNumberOff;                    ///< PTP��ŵ�ƫ�Ƶ�ַ
	size_t reserved1Off;                       ///< �����ֶ�1��ƫ�Ƶ�ַ
	size_t flagsOff;                           ///< IEEE1588��־λ���ϵ�ƫ�Ƶ�ַ
	size_t correctionFieldOff;                 ///< �������ƫ�Ƶ�ַ
	size_t reserved2Off;                       ///< �����ֶ�2��ƫ�Ƶ�ַ
	IEEE1588_SOURCE_PORT_ID_OFFSET sourcePortIdentityOff;///< ��ǰ���ķ����˿ڵ�Ψһ��ʶ��ƫ�Ƶ�ַ
	size_t sequenceIdOff;                      ///< PTP���ĵĵ�����ŵ�ƫ�Ƶ�ַ
	size_t controlFieldOff;                    ///< ��IEEE1588��׼��1����������õĵ�ƫ�Ƶ�ַ
	size_t logMessageIntervalOff;              ///< ����������֡ͬ���ͱ��ĵı���֮���ƽ�����ͼ����ƫ�Ƶ�ַ
}IEEE1588_HEADER_OFFSET;

///
/// @brief
///     IEEE1588���ĵ�ʱ�����ƫ�Ƶ�ַ��Ϣ
///
typedef struct
{
	size_t t_s;                                ///< ʱ������������ֵ�ƫ�Ƶ�ַ
	size_t t_ns;                               ///< ʱ�����С�����ֵ�ƫ�Ƶ�ַ
}IEEE1588_TIMESTAMP_OFFSET;

#endif// IEEE1588_TYPES_H_
