/// @file
///     std52_ft3_msg.h
///
/// @brief
///     ��׼FT3����ֵ����֡��52�ֽڰ汾,��CRC���ı����ӿڼ��ӿڶ���ʵ���Ĵ������ͷŽӿ�
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.09.05
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#ifndef STD52_FT3_MESSAGE_H_

#define STD52_FT3_MESSAGE_H_

#include "std52_ft3_types.h"

///
/// @brief
///     ��׼FT3����ֵ���ģ�52�ֽڰ汾,��CRC���ı����ӿ�
///
class Std52FT3Msg
{
public:
	virtual ~Std52FT3Msg(){};

	///
	/// @brief
	///     �Բ���ı�׼FT3����ֵ���ģ�52�ֽڰ汾,��CRC��֡���н���
	///
	/// @param[in]
	///     msgDataBuf - ���沶��ı���֡���ݵĻ���
	/// @param[in]
	///     bufSize    - �����С
	///
	/// @return
	///     ʵ�ʵĽ������ݴ�С - �ɹ��� 0 - ʧ��
	///
	virtual size_t Decode(const uint16_t *msgDataBuf, const size_t bufSize) = 0;

	///
	/// @brief
	///     �������õĲ������б��룬��ñ�׼FT3����ֵ���ģ�52�ֽڰ汾��֡��ASDU��������CRC����������
	///
	/// @param[in]
	///     msgDataArr - ���汨��֡���ݵ�����
	/// @param[in]
	///     arrSize    - �����С
	///
	/// @return
	///     ���汨��֡���ݵ������ʵ�ʴ�С - �ɹ��� 0 - ʧ��
	///
	virtual size_t Code(uint16_t *msgDataArr, const size_t arrSize) = 0;

	///
	/// @brief
	///     ��ȡASDU�ĳ���
	///
	/// @return
	///     ASDU�ĳ���
	///
	virtual size_t GetASDULen() = 0;

	///
	/// @brief
	///     ��ȡ����֡�е�ASDU�Ĺ�����Ϣ����ASDU�еĲ���ֵ��������Ĳ���
	///
	/// @param[in]
	///     asduPublic - ����ASDU�Ĺ�����Ϣ�Ľṹ��
	///
	/// @return
	///     true - �ɹ��� false - ʧ��
	///
	virtual bool GetASDUPublicInfo(STD52_FT3_ASDU_PUBLIC &asduPublic) = 0;

	///
	/// @brief
	///     ��ȡ����֡�е�ASDU�еĲ���ֵ���ݲ��ֵ�ͨ����
	///
	/// @return
	///     ASDU�еĲ���ֵ���ݲ��ֵ�ͨ����
	///
	virtual size_t GetASDUSmpsNum() = 0;

	///
	/// @brief
	///     ��ȡ����֡�е�ASDU�еĲ���ֵ����
	///
	/// @param[in]
	///     smpValArr  - �������˲ʱֵ������
	/// @param[in]
	///     arrSize    - �����С,��ֵ��>=ͨ����
	///
	/// @return
	///     �����ʵ�ʴ�С
	///
	virtual size_t GetASDUSmps(uint16_t *smpValArr, const size_t arrSize) = 0;

	///
	/// @brief
	///     ��ȡASDU�ڱ���֡�е�ƫ�Ƶ�ַ
	///
	/// @return
	///     ASDU�ڱ���֡�е�ƫ�Ƶ�ַ
	///
	virtual size_t GetASDUOff() = 0;

	///
	/// @brief
	///     ��ȡ����֡�е�ASDU�Ĺ�����Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
	///
	/// @param[in]
	///     asduPublic - ����ASDU�Ĺ�����Ϣ��ƫ�Ƶ�ַ��Ϣ�Ľṹ��
	///
	/// @return
	///     
	///
	virtual void GetASDUPublicInfoOff(STD52_FT3_ASDU_PUBLIC_OFFSET &asduPublicOff) = 0;

	///
	/// @brief
	///     ��ȡ����֡�е�ASDU�еĲ���ֵ�����ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
	///
	/// @param[in]
	///     smpOffArr - �������˲ʱֵ��ƫ�Ƶ�ַ��Ϣ������
	/// @param[in]
	///     arrSize   - ����Ĵ�С,��ֵ��>=ͨ����
	///
	/// @return
	///     �����ʵ�ʴ�С
	///
	virtual size_t GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize) = 0;

	///
	/// @brief
	///     ���ñ���֡�е�ASDU��Ϣ
	///
	/// @param[in]
	///     asduPublic - ASDU�Ĺ�����Ϣ
	/// @param[in]
	///     smpValBuf  - �������˲ʱֵ�Ļ���
	/// @param[in]
	///     bufSize    - �����С
	///
	/// @return
	///     
	///
	virtual void SetASDU(const STD52_FT3_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t bufSize) = 0;
};

///
/// @brief
///     ������׼FT3����ֵ���ģ�52�ֽڰ汾,��CRC���ı����ӿڶ����ʵ��
///
/// @return
///     ָ��Std52FT3Msgʵ����ָ��
///
Std52FT3Msg *CreateStd52FT3MsgImpl();

///
/// @brief
///     �ͷű�׼FT3����ֵ���ģ�52�ֽڰ汾,��CRC���ı����ӿڶ����ʵ��
///
/// @param[in]
///     impl - ָ��Std52FT3Msgʵ����ָ��
///
/// @return
///     
///
void ReleaseStd52FT3MsgImpl(Std52FT3Msg *impl);

#endif
