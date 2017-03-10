#ifndef GOOSE_DATA_SEG_H_

#define GOOSE_DATA_SEG_H_

#include <vector>
#include "../include/goose_types.h"

class MsgDataElem;

///
/// @brief
///     GOOSEͨ���ı������ݵĽṹ��
///
struct GOOSE_CHANNEL_MSG_DATA
{
	MsgDataElem *channelData;
	std::vector<GOOSE_CHANNEL_MSG_DATA *> vSubChannelData;
};

class GooseDataSeg
{
public:
	GooseDataSeg();
	~GooseDataSeg();

	size_t Decode(const size_t offset, const uint8_t *msgDataBuf, const size_t bufSize);
	size_t Code(uint8_t *gseDataArr, const size_t arrSize);

	void   ResetChannelDecodedFlag();
	size_t GetChannelNum();
	GOOSE_DATA *GetAChannel(size_t idxSeq, const uint8_t *msgDataBuf, const size_t bufSize);
	size_t GetGooseDataLen();

	size_t GetGooseDataOff();

	bool   AddAChannel(const GOOSE_DATA *datArr);
	void   Clear();

protected:
	GOOSE_CHANNEL_MSG_DATA *DecodeGooseChannel(MsgDataElem *channelData, const uint8_t *msgDataBuf, const size_t bufSize);
	size_t CodeGooseChannel(GOOSE_CHANNEL_MSG_DATA *channelData, uint8_t *channelDataArr, const size_t arrSize);
	void ChannelMsgToAct(const uint8_t *msgDataBuf, const size_t bufSize);

	GOOSE_DATA *MsgDataToActData(GOOSE_CHANNEL_MSG_DATA *channel, const uint8_t *msgDataBuf, const size_t bufSize);
	GOOSE_CHANNEL_MSG_DATA *ActDataToMsgData(const GOOSE_DATA *gseData);

	void ReleaaeChannelMsgData(std::vector<GOOSE_CHANNEL_MSG_DATA *> &vt);
	void ReleaaeChannelActData(std::vector<GOOSE_DATA *> &vt);

private:
	std::vector<GOOSE_CHANNEL_MSG_DATA *> m_vChannelMsgData;   
	                                             ///< Gooseͨ���ı���ֵ�����ݱ�
	size_t m_GseDataLen;                         ///< Goose�������У�All Data���ĳ��ȣ�����TLV���ݣ�
	size_t m_GseDataOff;                         ///< Goose�������У�All Data�����ڱ���֡�е�ƫ�Ƶ�ַ
	std::vector<GOOSE_DATA *> m_vChannelActData; ///< Gooseͨ������ʵֵ�����ݱ�
	bool   m_bChange2ActData;                    ///< Gooseͨ���ı���ֵ�Ƿ���ת��Ϊ��ʵֵ
};

#endif
