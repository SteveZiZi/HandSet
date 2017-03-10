/// @file
///     ieee1588_msg.h
///
/// @brief
///     IEEE1588����֡�����Ľӿڼ��ӿڶ���ʵ���Ĵ������ͷŽӿ�
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.9.29
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.09.29     |1.0.0.929  |��ǰ�汾���ṩ����̫������ͷ��IEEE1588����ͷ�Ľ���        |
///
#ifndef IEEE1588_MESSAGE_H_

#define IEEE1588_MESSAGE_H_

#include "etypes.h"
#include "ieee1588_types.h"

///
/// @brief
///     IEEE1588���ĵı����ӿ�
///
class Ieee1588Msg
{
public:
	virtual ~Ieee1588Msg(){};

	///
	/// @brief
	///     ��IEEE1588����֡���н���
	///
	/// @param[in]
	///     msgDataBuf - ����������IEEE1588����֡���ݵĻ���
	/// @param[in]
	///     bufSize    - �����С
	///
	/// @return
	///     ʵ�ʵĽ������ݴ�С - �ɹ��� 0 - ʧ��
	///
	virtual size_t Decode(const uint8_t *msgDataBuf, const size_t bufSize) = 0;

	///
	/// @brief
	///     ��ȡ��̫������֡ͷ��Ϣ
	///
	/// @param[in]
	///     ethHdr - ������̫������֡ͷ��Ϣ�Ľṹ��
	///
	/// @return
	///
	///
	virtual void GetEthernetHeader(ETHER_II_HEADER &ethHdr) = 0;

	///
	/// @brief
	///     ��ȡIEEE1588����ͷ��Ϣ
	///
	/// @param[in]
	///     ieee1588Hdr - ����IEEE1588����ͷ��Ϣ�Ľṹ��
	///
	/// @return
	///
	///
	virtual void GetIeee1588Header(IEEE1588_HEADER &ieee1588Hdr) = 0;

	///
	/// @brief
	///     ��ȡSync�����뿪��ʱ��ʱ��ʱ�̣���Sync���Ļ���Follow_Up���ĵ�OriginTimestamp�ֶ�
	///
	/// @param[in]
	///     originTs - OriginTimestamp
	///
	/// @return
	///     true - �����ɹ��� false - ����ʧ��
	///
	virtual bool GetOriginTimestamp(IEEE1588_TIMESTAMP &originTimestamp) = 0;

	///
	/// @brief
	///     ��ȡ��̫������֡ͷ��Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
	///
	/// @param[in]
	///     ethHdrOff - ������̫������֡ͷ��Ϣ��ƫ�Ƶ�ַ��Ϣ�Ľṹ��
	///
	/// @return
	///
	///
	virtual void GetEthernetHeaderOff(ETHER_II_HEADER_OFFSET &ethHdrOff) = 0;

	///
	/// @brief
	///     ��ȡIEEE1588����ͷ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
	///
	/// @param[in]
	///     ieee1588HdrOff - ����IEEE1588����ͷ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ�Ľṹ��
	///
	/// @return
	///
	///
	virtual void GetIeee1588HeaderOff(IEEE1588_HEADER_OFFSET &ieee1588HdrOff) = 0;

	///
	/// @brief
	///     ��ȡSync�����뿪��ʱ��ʱ��ʱ�̣���Sync���Ļ���Follow_Up���ĵ�OriginTimestamp�ֶε�ƫ�Ƶ�ַ��Ϣ
	///
	/// @param[in]
	///     originTimestampOff - OriginTimestamp��ƫ�Ƶ�ַ
	///
	/// @return
	///     true - �����ɹ��� false - ����ʧ��
	///
	virtual bool GetOriginTimestampOff(IEEE1588_TIMESTAMP_OFFSET &originTimestampOff) = 0;
};

///
/// @brief
///     ����IEEE1588���ĵı����ӿڶ����ʵ��
///
/// @return
///     ָ��Ieee1588Msgʵ����ָ��
///
Ieee1588Msg *CreateIeee1588MsgImpl();

///
/// @brief
///     �ͷ�IEEE1588���ĵı����ӿڶ����ʵ��
///
/// @param[in]
///     impl - ָ��Ieee1588Msgʵ����ָ��
///
/// @return
///     
///
void ReleaseIeee1588MsgImpl(Ieee1588Msg *impl);

#endif // IEEE1588_MESSAGE_H_
