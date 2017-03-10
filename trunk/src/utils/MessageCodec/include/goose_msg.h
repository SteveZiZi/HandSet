/// @file
///     goose_msg.h
///
/// @brief
///     Goose����֡�����Ľӿڼ��ӿڶ���ʵ���Ĵ������ͷŽӿ�
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.3.18
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.06.05     |1.0.0.605  |����GOOSE���ĵ�ͨ��������Ϣ�Ļ�ȡ�ӿ�                     |
///
#ifndef GOOSE_MESSAGE_H_

#define GOOSE_MESSAGE_H_

#include "src\include\stdint.h"
#include "etypes.h"
#include "goose_types.h"

///
/// @brief
///     IEC61850 GOOSE���ĵı����ӿ�
///
class GooseMsg
{
public:
	virtual ~GooseMsg(){};

	///
	/// @brief
	///     ��Goose����֡���н���
	///
	/// @param[in]
	///     msgDataBuf - ����������Goose����֡���ݵĻ���
	/// @param[in]
	///     bufSize    - �����С
	///
	/// @return
	///     ʵ�ʵĽ������ݴ�С - �ɹ��� 0 - ʧ��
	///
	virtual size_t Decode(const uint8_t *msgDataBuf, const size_t bufSize) = 0;

	///
	/// @brief
	///     �������õĲ������б��룬�õ�GoosePdu
	///
	/// @param[in]
	///     msgDataArr - ����Goose����֡���ݵ�����
	/// @param[in]
	///     arrSize    - �����С
	///
	/// @return
	///     ���汨��֡���ݵ������ʵ�ʴ�С - �ɹ��� 0 - ʧ��
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
	///     ��ȡ����֡��goosePDU�ĳ��ȣ�������TLV���ݵĳ���
	///
	/// @return
	///     goosePDU�ĳ���
	///
	virtual size_t GetGoosePduLen() = 0;

	///
	/// @brief
	///     ��ȡ����֡��Goose�������У�All Data���ĳ��ȣ�����TLV���ݣ�
	///
	/// @return
	///     Goose�������У�All Data���ĳ��ȣ�����TLV���ݣ�
	///
	virtual size_t GetGooseDataLen() = 0;

	///
	/// @brief
	///     ��ȡ����֡�е�GoosePdu�Ĺ�����Ϣ����GoosePdu�е�ͨ����������Ĳ���
	///
	/// @param[in]
	///     pduPublic - ����ASDU�Ĺ�����Ϣ�Ľṹ��
	///
	/// @return
	///     true - �ɹ��� false - ʧ��
	///
	virtual bool GetPduPublicInfo(GOOSE_PDU_PUBLIC &pduPublic) = 0;

	///
	/// @brief
	///     ��ȡ����֡�е�GoosePdu�Ĺ�����Ϣ����GoosePdu�е�ͨ����������Ĳ���
	///
	/// @param[in]
	///     pduPublic - ����ASDU�Ĺ�����Ϣ�Ľṹ��
	///
	/// @return
	///     true - �ɹ��� false - ʧ��
	///
	virtual bool GetPduPublicInfo(GOOSE_PDU_PUBLIC_EX &pduPublic) = 0;

	///
	/// @brief
	///     ��ȡ����֡�е�����ͨ����ͨ����
	///
	/// @return
	///     ����ͨ����ͨ����
	///
	virtual size_t GetChannelNum() = 0;

	///
	/// @brief
	///     ��ȡGoosePdu�еĵ���ͨ��������
	///     ע�⣬���ص�GOOSE_DATAָ���ɱ�������Լ�ά�����ⲿ�ͷŻ��������
	///
	/// @param[in]
	///     idxSeq     - GOOSE����ͨ����˳��ţ���0��ʼ
	///
	/// @return
	///     ָ��Gooseͨ�����ݵ�ָ��
	///
	virtual GOOSE_DATA * GetAChannel(size_t idxSeq) = 0;

	///
	/// @brief
	///     ��ȡSecurity��ѡ���������Ϣ
	///
	/// @return
	///     true - �����к���Security�� false - ������
	///
	virtual bool GetSecurityOpt() = 0;

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
	///     ��ȡgoosePDU�ڱ���֡��ƫ�Ƶ�ַ
	///
	/// @return
	///     goosePDU�ڱ���֡��ƫ�Ƶ�ַ
	///
	virtual size_t GetGoosePduOff() = 0;

	///
	/// @brief
	///     ��ȡ����֡��Goose�������У�All Data���ڱ���֡�е�ƫ�Ƶ�ַ
	///
	/// @return
	///     Goose�������У�All Data�����ڱ���֡�е�ƫ�Ƶ�ַ
	///
	virtual size_t GetGooseDataOff() = 0;

	///
	/// @brief
	///     ��ȡGoosePdu�Ĺ�����Ϣ�����ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
	///
	/// @param[in]
	///     pduPblcOff - ����GoosePdu�Ĺ�����Ϣ�����ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ�Ľṹ��
	///
	/// @return
	///     
	///
	virtual void GetPduPublicInfoOff(GOOSE_PDU_PUBLIC_OFFSET &pduPblcOff) = 0;

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
	virtual void SetMacAddr(const uint8_t *desMac, const uint8_t *srcMac, size_t macLen, bool isHasVlan) = 0;

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
	virtual void Set8021QVlanInfo(uint8_t userPriority, uint16_t vlanID, bool isHasVlan) = 0;

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
	///     ����GoosePdu�й������ֵ���Ϣ������ͨ����������Ĳ��ֵ���Ϣ
	///
	/// @param[in]
	///     pduPblc - GoosePdu�й������ֵ���Ϣ
	///
	/// @return
	///
	///
	virtual void SetPduPublicInfo(GOOSE_PDU_PUBLIC &pduPblc) = 0;

	///
	/// @brief
	///     ���һ������ͨ��
	///     ע�⣬��ӵ�GOOSE_DATA���ڲ�����ȿ��������ಢ�����GOOSE_DATAָ�����ά����˭����˭�ͷ�
	///
	/// @param[in]
	///     datBuf  - ���浥��GOOSE����ͨ�����е����ݵĻ���
	/// @param[in]
	///     bufSize - �����С��������GOOSE����ͨ����������Ŀ��Ϊ1����ʾ��ͨ�������ݷǽṹ�����ͣ�
	///
	/// @return
	///     true - �ɹ��� false - ʧ��
	///
	virtual bool   AddAChannel(const GOOSE_DATA *channel) = 0;

	///
	/// @brief
	///     ����Security��ѡ�����Ϣ
	///
	/// @param[in]
	///     opt - �������Ƿ��������
	///
	/// @return
	///     
	///
	virtual void SetSecurityOpt(bool opt) = 0;

	///
	/// @brief
	///     ��ձ���֡��GoosePDU�ı������
	///
	/// @return
	///     
	///
	virtual void ClearGoosePDU() = 0;
};

///
/// @brief
///     ����IEC61850 GOOSE���ĵı����ӿڶ����ʵ��
///
/// @return
///     ָ��GooseMsgʵ����ָ��
///
GooseMsg *CreateGooseMsgImpl();

///
/// @brief
///     �ͷ�IEC61850 GOOSE���ĵı����ӿڶ����ʵ��
///
/// @param[in]
///     impl - ָ��GooseMsgʵ����ָ��
///
/// @return
///     
///
void ReleaseGooseMsgImpl(GooseMsg *impl);

#endif
