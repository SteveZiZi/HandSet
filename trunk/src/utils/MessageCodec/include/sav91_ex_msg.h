/// @file
///     sav91_ex_msg.h
///
/// @brief
///     IEC61850-9-1��չ����֡�����Ͼ���������ƶ������������ı����ӿڼ��ӿڶ���ʵ���Ĵ������ͷŽӿ�
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.09.06
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#ifndef SAV91_EX_MESSAGE_H_

#define SAV91_EX_MESSAGE_H_

#include "etypes.h"
#include "sav91_ex_types.h"

///
/// @brief
///     IEC61850-9-1��չ����֡�ı����ӿ�
///
class Sav91ExMsg
{
public:
	virtual ~Sav91ExMsg(){};

	///
	/// @brief
	///     ��IEC61850-9-1��չ����֡���н���
	///
	/// @param[in]
	///     msgDataBuf  - ���������ı���֡���ݵĻ���
	/// @param[in]
	///     bufSize     - �����С
	///
	/// @return
	///     ʵ�ʵĽ������ݴ�С - �ɹ��� 0 - ʧ��
	///
	virtual size_t Decode(const uint8_t *msgDataBuf, const size_t bufSize) = 0;

	///
	/// @brief
	///     �������õĲ������б��룬���IEC61850-9-1��չ����֡
	///
	/// @param[in]
	///     msgDataArr - ����SavPDU������������
	/// @param[in]
	///     arrSize    - �����С
	///
	/// @return
	///     ����IEC61850-9-1��չ����֡�������ʵ�ʴ�С - �ɹ��� 0 - ʧ��
	///
	virtual size_t Code(uint8_t *msgDataArr, const size_t arrSize) = 0;

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
	///     ��ȡIEEE802.1Q VLAN��ǩ��Ϣ
	///
	/// @param[in]
	///     info - ����IEEE802.1Q VLAN��ǩ��Ϣ�Ľṹ��
	///
	/// @return
	///
	///
	virtual void Get8021QVlanInfo(IEEE802_1Q_VLAN_INFO &info) = 0;

	///
	/// @brief
	///     ��ȡISO/IEC8802-3�汾����̫��֡�е�PDU�Ĺ�����Ϣ����PDU��APDU����Ĳ���
	///
	/// @param[in]
	///     info - ����ISO/IEC8802-3�汾����̫��֡�е�PDU�Ĺ�����Ϣ�Ľṹ��
	///
	/// @return
	///
	///
	virtual void GetIEC8802Info(IEC8802_3_PDU_PUBLIC &info) = 0;

	///
	/// @brief
	///     ��ȡ����֡��Sav91ExPDU�ĳ���
	///
	/// @return
	///     Sav91ExPDU�ĳ���
	///
	virtual size_t GetSavPduLen() = 0;

	///
	/// @brief
	///     ��ȡ����֡��ASDU����Ŀ
	///
	/// @return
	///     ����֡������ASDU����Ŀ
	///
	virtual uint16_t GetNoASDU() = 0;

	///
	/// @brief
	///     ��ȡ����֡�е�ASDU�Ĺ�����Ϣ����ASDU�еĲ���ֵ��������Ĳ���
	///
	/// @param[in]
	///     idxSeq     - ����֡��ASDU��˳��ţ���0��ʼ
	/// @param[in]
	///     asduPublic - ����ASDU�Ĺ�����Ϣ�Ľṹ��
	///
	/// @return
	///     true - �ɹ��� false - ʧ��
	///
	virtual bool GetASDUPublicInfo(const size_t idxSeq, SAV91_EX_ASDU_PUBLIC &asduPublic) = 0;

	///
	/// @brief
	///     ��ȡASDU�еĲ���ֵ���ݲ��ֵ�ͨ����
	///
	/// @return
	///     ASDU�Ĳ���ֵ���ݲ��ֵ�ͨ����
	///
	virtual size_t GetASDUSmpsNum() = 0;

	///
	/// @brief
	///     ��ȡ����֡��ASDU�еĲ���ֵ����
	///
	/// @param[in]
	///     idxSeq    - ����֡��ASDU��˳��ţ���0��ʼ
	/// @param[in]
	///     smpValArr - �������˲ʱֵ������
	/// @param[in]
	///     arrSize   - �����С
	///
	/// @return
	///     �����ʵ�ʴ�С
	///
	virtual size_t GetASDUSmps(const size_t idxSeq, uint16_t *smpValArr, const size_t arrSize) = 0;

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
	///     ��ȡIEEE802.1Q VLAN��ǩ��Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
	///
	/// @param[in]
	///     infoOff - ����IEEE802.1Q VLAN��ǩ��Ϣ��ƫ�Ƶ�ַ��Ϣ�Ľṹ��
	///
	/// @return
	///
	///
	virtual void Get8021QVlanInfoOff(IEEE802_1Q_VLAN_INFO_OFFSET &infoOff) = 0;

	///
	/// @brief
	///     ��ȡISO/IEC8802-3�汾����̫��֡�е�PDU�Ĺ�����Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
	///
	/// @param[in]
	///     infoOff - ����ISO/IEC8802-3�汾����̫��֡�е�PDU�Ĺ�����Ϣ��ƫ�Ƶ�ַ��Ϣ�Ľṹ��
	///
	/// @return
	///
	///
	virtual void GetIEC8802InfoOff(IEC8802_3_PDU_PUBLIC_OFFSET &infoOff) = 0;

	///
	/// @brief
	///     ��ȡSav91ExPDU�ڱ���֡�е�ƫ�Ƶ�ַ
	///
	/// @return
	///     Sav91ExPDU�ڱ���֡�е�ƫ�Ƶ�ַ
	///
	virtual size_t GetSavPduOff() = 0;

	///
	/// @brief
	///     ��ȡASDU����Ŀ��Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ
	///
	/// @return
	///     ASDU����Ŀ��Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ
	///
	virtual size_t GetNoASDUOff() = 0;

	///
	/// @brief
	///     ��ȡ����֡�е�ASDU�Ĺ�����Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
	///
	/// @param[in]
	///     idxSeq     - ����֡��ASDU��˳��ţ���0��ʼ
	/// @param[in]
	///     asduPublic - ����ASDU�Ĺ�����Ϣ��ƫ�Ƶ�ַ��Ϣ�Ľṹ��
	///
	/// @return
	///     true - �ɹ��� false - ʧ��
	///
	virtual bool GetASDUPublicInfoOff(const size_t idxSeq, SAV91_EX_ASDU_PUBLIC_OFFSET &asduPublicOff) = 0;

	///
	/// @brief
	///     ��ȡĳһASDU�еĲ���ֵ�����ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
	///
	/// @param[in]
	///     idxSeq    - ����֡��ASDU��˳��ţ���0��ʼ
	/// @param[in]
	///     smpOffArr - �������˲ʱֵ��ƫ�Ƶ�ַ��Ϣ������
	/// @param[in]
	///     arrSize   - �����С
	///
	/// @return
	///     �����ʵ�ʴ�С
	///
	virtual size_t GetASDUSmpsOff(const size_t idxSeq, size_t *smpOffArr, const size_t arrSize) = 0;

	///
	/// @brief
	///     ����Mac��ַ
	///
	/// @param[in]
	///     desMac - ָ��Ŀ��Mac��ַ��ָ��
	/// @param[in]
	///     srcMac - ָ��ԴMac��ַ��ָ��
	/// @param[in]
	///     macLen - MAC��ַ�ĳ���
	///
	/// @return
	///
	///
	virtual void SetMacAddr(const uint8_t *desMac, const uint8_t *srcMac, size_t macLen) = 0;

	///
	/// @brief
	///     ����IEEE802.1Q VLAN��ǩ��Ϣ
	///
	/// @param[in]
	///     userPriority - �û����ȼ���0~7��
	/// @param[in]
	///     vlanID       - �����������ʶ�����ڱ�Ǹñ���Ӧ�÷��͵��ĸ�VLAN��12λ��
	///
	/// @return
	///
	///
	virtual void Set8021QVlanInfo(uint8_t userPriority, uint16_t vlanID) = 0;

	///
	/// @brief
	///     ����AppID
	///
	/// @param[in]
	///     appID - Ӧ����Ϣ��ʶ�룬��������Ӧ�ù�����������GOOSE��ģ��������ֵ��SAV��Э��
	///
	/// @return
	///
	///
	virtual void SetAppID(const uint16_t appID) = 0;

	///
	/// @brief
	///     ���SavPDU�ı������
	///
	/// @return
	///     
	///
	virtual void ClearSavPDU() = 0;

	///
	/// @brief
	///     ���һ��ASDU������֡��
	///
	/// @param[in]
	///     asduPublic - ASDU�Ĺ�����Ϣ
	/// @param[in]
	///     smpValBuf  - �������˲ʱֵ�Ļ���
	/// @param[in]
	///     bufSize    - �����С
	///
	/// @return
	///     true - �ɹ��� false - ʧ��
	///
	virtual bool AddAnASDU(const SAV91_EX_ASDU_PUBLIC &asduPublic, const uint16_t *smpValbuf, const size_t bufSize) = 0;
};

///
/// @brief
///     ����IEC61850-9-1��չ���ĵı����ӿڶ����ʵ��
///
/// @return
///     ָ��Sav91ExMsgʵ����ָ��
///
Sav91ExMsg *CreateSav91ExMsgImpl();

///
/// @brief
///     �ͷ�IEC61850-9-1��չ���ĵı����ӿڶ����ʵ��
///
/// @param[in]
///     impl - ָ��Sav91ExMsgʵ����ָ��
///
/// @return
///     
///
void ReleaseSav91ExMsgImpl(Sav91ExMsg *impl);

#endif
