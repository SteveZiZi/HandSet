/// @file
///     sav92_msg.h
///
/// @brief
///     IEC61850-9-2LE����֡�ı����ӿڼ��ӿڶ���ʵ���Ĵ������ͷŽӿ�
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
///    |               |           |                                                          |
///
#ifndef SAV92_MESSAGE_H_

#define SAV92_MESSAGE_H_

#include "etypes.h"
#include "sav92_types.h"

///
/// @brief
///     IEC61850-9-2����ֵ���ĵı����ӿ�
///
class Sav92Msg
{
public:
	virtual ~Sav92Msg(){};

	///
	/// @brief
	///     ��IEC61850-9-2����ֵ����֡���н���
	///
	/// @param[in]
	///     msgDataBuf - ���������ı���֡���ݵĻ���
	/// @param[in]
	///     bufSize    - �����С
	///
	/// @return
	///     ʵ�ʵĽ������ݴ�С - �ɹ��� 0 - ʧ��
	///
	virtual size_t Decode(const uint8_t *msgDataBuf, const size_t bufSize) = 0;

	///
	/// @brief
	///     �������õĲ������б��룬���IEC61850-9-2����֡
	///
	/// @param[in]
	///     msgDataArr - ���汨��֡���ݵ�����
	/// @param[in]
	///     arrSize    - �����Ԫ����Ŀ
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
	///     ��ȡ����ISO/IEC8802-3�ı���֡��PDU�Ĺ�����Ϣ����PDU��APDU����Ĳ���
	///
	/// @param[in]
	///     info - �������ISO/IEC8802-3�ı���֡��PDU�Ĺ�����Ϣ�Ľṹ��
	///
	/// @return
	///
	///
	virtual void GetIEC8802Info(IEC8802_3_PDU_PUBLIC &info) = 0;

	///
	/// @brief
	///     ��ȡ����֡��Sav92PDU(TLV��ʽ)�ĳ���
	///
	/// @return
	///     Sav92PDU�ĳ���
	///
	virtual size_t GetSavPduLen() = 0;

	///
	/// @brief
	///     ��ȡ����֡�е�ASDU����Ŀ
	///
	/// @param[in]
	///     noASDU - ��ʾ�ñ���֡������ASDU����Ŀ����������йأ�length��2 byte
	///
	/// @return
	///
	///
	virtual uint16_t GetNoASDU() = 0;

	///
	/// @brief
	///     ��ȡ����֡��ĳһASDU��TLV��ʽ���ĳ���
	///
	/// @return
	///     ASDU��TLV��ʽ���ĳ���
	///
	virtual size_t GetASDULen(const size_t idxSeq) = 0;

	///
	/// @brief
	///     ��ȡ����֡��ĳһASDU��Sequence of Data��TLV��ʽ���ĳ���
	///
	/// @return
	///     Sequence of Data��TLV��ʽ���ĳ���
	///
	virtual size_t GetSeqOfDatLen(const size_t idxSeq) = 0;

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
	virtual bool GetASDUPublicInfo(const size_t idxSeq, SAV92_ASDU_PUBLIC &asduPublic) = 0;

	///
	/// @brief
	///     ��ȡ����֡�е�ASDU�еĲ�����������Ϣ
	///
	/// @param[in]
	///     idxSeq - ����֡��ASDU��˳��ţ���0��ʼ
	/// @param[in]
	///     smpCnt - ���������������Ϣ
	///
	/// @return
	///     true - �ɹ��� false - ʧ��
	///
	virtual bool GetSmpCntInASDU(const size_t idxSeq, uint16_t &smpCnt) = 0;

	///
	/// @brief
	///     ��ȡ����֡�е�ASDU�е�ͬ����ʶλ��Ϣ
	///
	/// @param[in]
	///     idxSeq   - ����֡��ASDU��˳��ţ���0��ʼ
	/// @param[in]
	///     smpSynch - ����ͬ����ʶλ��Ϣ
	///
	/// @return
	///     true - �ɹ��� false - ʧ��
	///
	virtual bool GetSmpSynchInASDU(const size_t idxSeq, bool &smpSynch) = 0;

	///
	/// @brief
	///     ��ȡĳһASDU�еĲ���ֵ���ݲ��ֵ�ͨ����
	///
	/// @param[in]
	///     idxSeq  - ����֡��ASDU��˳��ţ���0��ʼ
	///
	/// @return
	///     ASDU�еĲ���ֵ���ݲ��ֵ�ͨ����
	///
	virtual size_t GetASDUSmpsNum(const size_t idxSeq) = 0;

	///
	/// @brief
	///     ��ȡ����֡��ASDU�еĲ���ֵ����
	///
	/// @param[in]
	///     idxSeq    - ����֡��ASDU��˳��ţ���0��ʼ
	/// @param[in]
	///     smpValArr - �������˲ʱֵ������
	/// @param[in]
	///     smpQltArr - �������˲ʱֵ��Ʒ��ֵ������
	/// @param[in]
	///     arrSize   - ������Ĵ�С
	///
	/// @return
	///     �����ʵ�ʴ�С
	///
	virtual size_t GetASDUSmps(const size_t idxSeq, uint32_t *smpValArr, uint32_t *smpQltArr, const size_t arrSize) = 0;

	///
	/// @brief
	///     ��ȡSavPDU�еĿ�ѡ����Ϣ
	///
	/// @param[in]
	///     asduOpts - �����ѡ����Ϣ�Ľṹ��
	///
	/// @return
	///     
	///
	virtual void GetSavPduOpts(SAV92_SAVPDU_OPTS &pduOpts) = 0;

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
	///     ��ȡ����ISO/IEC8802-3�ı���֡��PDU�Ĺ�����Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
	///
	/// @param[in]
	///     infoOff - �������ISO/IEC8802-3�ı���֡��PDU�Ĺ�����Ϣ��ƫ�Ƶ�ַ��Ϣ�Ľṹ��
	///
	/// @return
	///
	///
	virtual void GetIEC8802InfoOff(IEC8802_3_PDU_PUBLIC_OFFSET &infoOff) = 0;

	///
	/// @brief
	///     ��ȡSav92PDU�ڱ���֡�е�ƫ�Ƶ�ַ
	///
	/// @return
	///     Sav92PDU�ڱ���֡�е�ƫ�Ƶ�ַ
	///
	virtual size_t GetSavPduOff() = 0;

	///
	/// @brief
	///     ��ȡASDU����Ŀ��Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ
	///
	/// @return
	///     ����֡�е�ASDU����Ŀ��Ϣ��ƫ�Ƶ�ַ
	///
	virtual size_t GetNoASDUOff() = 0;

	///
	/// @brief
	///     ��ȡĳ��ASDU�ڱ���֡�е�ƫ�Ƶ�ַ
	///
	/// @param[in]
	///     idxSeq - ����֡��ASDU��˳��ţ���0��ʼ
	///
	/// @return
	///     ASDU�ڱ���֡�е�ƫ�Ƶ�ַ
	///
	virtual size_t GetASDUOff(const size_t idxSeq) = 0;

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
	virtual bool GetASDUPublicInfoOff(const size_t idxSeq, SAV92_ASDU_PUBLIC_OFFSET &asduPublicOff) = 0;

	///
	/// @brief
	///     ��ȡASDU�еĲ���ֵ�����ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
	///
	/// @param[in]
	///     idxSeq       - ����֡��ASDU��˳��ţ���0��ʼ
	/// @param[in]
	///     seqOfDataOff - ����˲ʱֵ���е�ƫ�Ƶ�ַ
	/// @param[in]
	///     smpValOffArr - �������˲ʱֵ��ƫ�Ƶ�ַ��Ϣ������
	/// @param[in]
	///     smpQltOffArr - �������˲ʱֵ��Ʒ��ֵ��ƫ�Ƶ�ַ��Ϣ������
	/// @param[in]
	///     arrSize      - ������Ĵ�С
	///
	/// @return
	///     �����ʵ�ʴ�С
	///
	virtual size_t GetASDUSmpsOff(const size_t idxSeq, size_t &seqOfDataOff, size_t *smpValOffArr,
	                              size_t *smpQltOffArr, const size_t arrSize) = 0;

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
	///     ���Sav92PDU�ı������
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
	///     smpQltBuf  - �������˲ʱֵ��Ʒ��ֵ�Ļ���
	/// @param[in]
	///     bufSize    - ����Ĵ�С,��ͨ����
	///
	/// @return
	///     true - �ɹ��� false - ʧ��
	///
	virtual bool AddAnASDU(const SAV92_ASDU_PUBLIC &asduPublic, const uint32_t *smpValBuf,
		                   const uint32_t *smpQltBuf, const size_t bufSize) = 0;

	///
	/// @brief
	///     ����SavPDU�еĿ�ѡ����Ϣ��������AddAnASDU()֮ǰ����
	///
	/// @param[in]
	///     asduOpts - ��ѡ����Ϣ
	///
	/// @return
	///     
	///
	virtual void SetSavPduOpts(SAV92_SAVPDU_OPTS &pduOpts) = 0;
};

///
/// @brief
///     ����IEC61850-9-2����ֵ���ĵı����ӿڶ����ʵ��
///
/// @return
///     ָ��Sav92Msgʵ����ָ��
///
Sav92Msg *CreateSav92MsgImpl();

///
/// @brief
///     �ͷ�IEC61850-9-2����ֵ���ĵı����ӿڶ����ʵ��
///
/// @param[in]
///     impl - ָ��Sav92Msgʵ����ָ��
///
/// @return
///     
///
void ReleaseSav92MsgImpl(Sav92Msg *impl);

#endif
