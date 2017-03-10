/// @file
///     msg_rapid_decoder.h
///
/// @brief
///     ����֡�Ŀ��ٽ���ӿ�
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.5.13
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.05.15     |1.0.0.515  |���Sav92�Ŀ�ѡ���Goose�Ŀ�ѡ�����Ϣ�Ľӿ�             |
///    |2013.05.15     |1.0.0.515  |��ӻ�ȡ����֡��ASDU��Ŀ�Ľӿ�                           |
///    |2013.05.15     |1.0.0.515  |�޸�����/����FT3ͨ�����Ļ�ȡ������DataSetName���жϣ�ֱ�ӷ���ͨ����|
///    |2013.07.15     |1.0.0.709  |��ӻ�ȡЭ�����ݵ�Ԫ���ȵĽӿ�                           |
///    |2013.09.10     |1.0.1.910  |��ӱ���֡���ͣ�������FT3��52�ֽڱ�׼FT3��9-1��չ������������Ӧ�ӿڵ�ʵ��|
///    |2013.09.11     |1.0.1.911  |��������ѹ������FT3֡���ͣ�2010/2011�汾�����жϣ�ͨ��CRCУ�飩|
///    |2013.09.30     |1.0.1.930  |���IEEE1588���ģ����±���֡���ͻ�ȡ�ӿ�                 |
///    |2013.09.30     |1.0.1.930  |��ӻ�ȡIEEE1588���İ汾��Ϣ��Դ�˿ڱ�ʶ��Ϣ�Ľӿ�       |
///
#ifndef MESSAGE_RAPID_DECODER_H_

#define MESSAGE_RAPID_DECODER_H_

#include "src\include\stdint.h"
#include "src\include\pub_datatypes.h"
#include "sav92_types.h"
#include "ieee1588_types.h"


class MsgRapidDecoder
{
public:
	///
	/// @brief
	///     ��ȡ����֡������
	///
	/// @param[in]
	///     msgBuf  - ָ����֡�����ָ��
	/// @param[in]
	///     bufSize - ����֡����Ĵ�С
	///
	/// @return
	///     ��������
	///
	enumFrameType GetMessageType(const uint8_t *msgBuf, size_t bufSize);

	///
	/// @brief
	///     ��ȡ����֡�е�Ŀ��Mac��ַ
	///
	/// @param[in]
	///     msgBuf    - ָ����֡�����ָ��
	/// @param[in]
	///     bufSize   - ����֡����Ĵ�С
	/// @param[in]
	///     dstMacArr - ����Ŀ��Mac��ַ�����飬��С��>=6
	///
	void GetDstMac(const uint8_t *msgBuf, size_t bufSize, uint8_t *dstMacArr);

	///
	/// @brief
	///     ��ȡ����֡�е�ԴMac��ַ
	///
	/// @param[in]
	///     msgBuf    - ָ����֡�����ָ��
	/// @param[in]
	///     bufSize   - ����֡����Ĵ�С
	/// @param[in]
	///     srcMacArr - ����ԴMac��ַ�����飬��С��>=6
	///
	void GetSrcMac(const uint8_t *msgBuf, size_t bufSize, uint8_t *srcMacArr);

	///
	/// @brief
	///     ��ȡ��̫������֡�е���̫������
	///
	/// @param[in]
	///     msgBuf    - ָ����֡�����ָ��
	/// @param[in]
	///     bufSize   - ����֡����Ĵ�С
	/// @param[in]
	///     etherType - ������̫������
	///
	void GetEtherType(const uint8_t *msgBuf, size_t bufSize, uint16_t &etherType);

	///
	/// @brief
	///     ��ȡISO/IEC8802-3��ʽ����Vlan Tag������̫������֡�е�AppID
	///     ������������appID��0
	///
	/// @param[in]
	///     msgBuf  - ָ����֡�����ָ��
	/// @param[in]
	///     bufSize - ����֡����Ĵ�С
	/// @param[in]
	///     appID   - ����AppID
	///
	void GetAppID(const uint8_t *msgBuf, size_t bufSize, uint16_t &appID);

	///
	/// @brief
	///     ��ȡЭ�����ݵ�Ԫ�ĳ��ȣ�ISO/IEC8802-3��ʽ����Vlan Tag������̫������֡�д�AppID��ʼ���ֽ���
	///     ������������len��0
	///
	/// @param[in]
	///     msgBuf  - ָ����֡�����ָ��
	/// @param[in]
	///     bufSize - ����֡����Ĵ�С
	/// @param[in]
	///     len     - ����Э�����ݵ�Ԫ�ĳ���
	///
	void GetPDULen(const uint8_t *msgBuf, size_t bufSize, uint16_t &len);

	///
	/// @brief
	///     ��ȡ����֡�е�����ͨ������Ŀ����Ҫ����GOOSE��SAV91/2��FT3
	///
	/// @param[in]
	///     msgBuf     - ָ����֡�����ָ��
	/// @param[in]
	///     bufSize    - ����֡����Ĵ�С
	/// @param[in]
	///     type       - ������ı��ĵ�����
	/// @param[in]
	///     channelNum - ����ͨ������Ŀ
	///
	void GetChannelNum(const uint8_t *msgBuf, size_t bufSize, const enumFrameType type, size_t &channelNum);

	///
	/// @brief
	///     ��ȡ����֡�е�LNName����Ҫ����SAV91��FT3
	///
	/// @param[in]
	///     msgBuf  - ָ����֡�����ָ��
	/// @param[in]
	///     bufSize - ����֡����Ĵ�С
	/// @param[in]
	///     type    - ������ı��ĵ�����
	/// @param[in]
	///     lnName  - �߼��ڵ���
	///
	void GetLNName(const uint8_t *msgBuf, size_t bufSize, const enumFrameType type, uint16_t &lnName);

	///
	/// @brief
	///     ��ȡ����֡�е�LDName����Ҫ����SAV91��FT3
	///
	/// @param[in]
	///     msgBuf  - ָ����֡�����ָ��
	/// @param[in]
	///     bufSize - ����֡����Ĵ�С
	/// @param[in]
	///     type    - ������ı��ĵ�����
	/// @param[in]
	///     ldName  - �߼��豸��
	///
	void GetLDName(const uint8_t *msgBuf, size_t bufSize, const enumFrameType type, uint16_t &ldName);

	///
	/// @brief
	///     ��ȡ����֡�еĲ����ʣ���Ҫ����SAV91/2��FT3
	///
	/// @param[in]
	///     msgBuf  - ָ����֡�����ָ��
	/// @param[in]
	///     bufSize - ����֡����Ĵ�С
	/// @param[in]
	///     type    - ������ı��ĵ�����
	/// @param[in]
	///     smpRate - ������
	///
	void GetSmpRate(const uint8_t *msgBuf, size_t bufSize, const enumFrameType type, uint16_t &smpRate);

	///
	/// @brief
	///     ��ȡ����֡�е�ASDU��Ŀ����Ҫ����SAV91/2��FT3
	///
	/// @param[in]
	///     msgBuf  - ָ����֡�����ָ��
	/// @param[in]
	///     bufSize - ����֡����Ĵ�С
	/// @param[in]
	///     type    - ������ı��ĵ�����
	/// @param[in]
	///     noASDU  - ASDU��Ŀ
	///
	void GetNoASDU(const uint8_t *msgBuf, size_t bufSize, const enumFrameType type, uint16_t &noASDU);

	///
	/// @brief
	///     ��ȡSav92����֡�еĿ�ѡ����Ϣ
	///
	/// @param[in]
	///     msgBuf  - ָ����֡�����ָ��
	/// @param[in]
	///     bufSize - ����֡����Ĵ�С
	/// @param[in]
	///     opts    - ��ѡ��
	///
	void GetSav92Opts(const uint8_t *msgBuf, size_t bufSize, SAV92_SAVPDU_OPTS &opts);


	///
	/// @brief
	///     �ж�Goose����֡���Ƿ���Security��ѡ��
	///
	/// @param[in]
	///     msgBuf  - ָ����֡�����ָ��
	/// @param[in]
	///     bufSize - ����֡����Ĵ�С
	///
	/// @return
	///     true - �У� false - ��
	///
	bool IsHaveSecurityInGoose(const uint8_t *msgBuf, size_t bufSize);

	///
	/// @brief
	///     ��ȡIEEE1588���ĵİ汾��Ϣ
	///
	/// @param[in]
	///     msgBuf  - ָ��IEEE1588����֡�����ָ��
	/// @param[in]
	///     bufSize - ����֡����Ĵ�С
	/// @param[in]
	///     version - ����IEEE1588���ĵİ汾��Ϣ
	///
	/// @return
	///     
	///
	void GetIeee1588Version(const uint8_t *msgBuf, size_t bufSize, uint8_t &version);

	///
	/// @brief
	///     ��ȡIEEE1588���ĵı��ķ����˿ڣ�Դ�˿ڣ��ı�ʶ��Ϣ
	///
	/// @param[in]
	///     msgBuf    - ָ��IEEE1588����֡�����ָ��
	/// @param[in]
	///     bufSize   - ����֡����Ĵ�С
	/// @param[in]
	///     srcPortID - ���汨�ķ����˿ڣ�Դ�˿ڣ��ı�ʶ��Ϣ�Ľṹ��
	///
	/// @return
	///     
	///
	void GetIeee1588SrcPortID(const uint8_t *msgBuf, size_t bufSize, IEEE1588_SOURCE_PORT_ID &srcPortID);

	///
	/// @brief
	///     ��ȡSync�����뿪��ʱ��ʱ��ʱ�̣���Sync���Ļ���Follow_Up���ĵ�OriginTimestamp�ֶ�
	///
	/// @param[in]
	///     msgBuf   - ָ��IEEE1588����֡�����ָ��
	/// @param[in]
	///     bufSize  - ����֡����Ĵ�С
	/// @param[in]
	///     originTs - OriginTimestamp
	///
	/// @return
	///     true - �����ɹ��� false - ����ʧ��
	///
	bool GetIeee1588OriginTimeStamp(const uint8_t *msgBuf, size_t bufSize, IEEE1588_TIMESTAMP &originTs);

protected:
	///
	/// @brief
	///     ��������
	///
	void GetChannelNumInNanRuiFT3(const uint8_t *msgBuf, size_t bufSize, size_t &channelNum);
	void GetChannelNumInGuoWangFT3(const uint8_t *msgBuf, size_t bufSize, size_t &channelNum);
	void GetChannelNumInSav91(const uint8_t *msgBuf, size_t bufSize, size_t &channelNum);
	void GetChannelNumInSav91Ex(const uint8_t *msgBuf, size_t bufSize, size_t &channelNum);
	void GetChannelNumInSav92(const uint8_t *msgBuf, size_t bufSize, size_t &channelNum);
	void GetChannelNumInGoose(const uint8_t *msgBuf, size_t bufSize, size_t &channelNum);

	void GetLNNameInNanRuiFT3(const uint8_t *msgBuf, size_t bufSize, uint16_t &lnName);
	void GetLNNameInGuoWangFT3(const uint8_t *msgBuf, size_t bufSize, uint16_t &lnName);
	void GetLNNameInStd52FT3(const uint8_t *msgBuf, size_t bufSize, uint16_t &lnName);
	void GetLNNameInSav91(const uint8_t *msgBuf, size_t bufSize, uint16_t &lnName);
	void GetLNNameInSav91Ex(const uint8_t *msgBuf, size_t bufSize, uint16_t &lnName);

	void GetLDNameInNanRuiFT3(const uint8_t *msgBuf, size_t bufSize, uint16_t &ldName);
	void GetLDNameInGuoWangFT3(const uint8_t *msgBuf, size_t bufSize, uint16_t &ldName);
	void GetLDNameInStd52FT3(const uint8_t *msgBuf, size_t bufSize, uint16_t &ldName);
	void GetLDNameInSav91(const uint8_t *msgBuf, size_t bufSize, uint16_t &ldName);
	void GetLDNameInSav91Ex(const uint8_t *msgBuf, size_t bufSize, uint16_t &ldName);

	void GetSmpRateInNanRuiFT3(const uint8_t *msgBuf, size_t bufSize, uint16_t &smpRate);
	void GetSmpRateInSav91(const uint8_t *msgBuf, size_t bufSize, uint16_t &smpRate);
	void GetSmpRateInSav92(const uint8_t *msgBuf, size_t bufSize, uint16_t &smpRate);

	void GetNoASDUInSav91(const uint8_t *msgBuf, size_t bufSize, uint16_t &noASDU);
	void GetNoASDUInSav92(const uint8_t *msgBuf, size_t bufSize, uint16_t &noASDU);

	size_t NextTLVData(const size_t beginIdx, const uint8_t *dataBuf, size_t bufSize, size_t &valIdx);

};

#endif
