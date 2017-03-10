#ifndef GOOSE_DATA_SEG_H_

#define GOOSE_DATA_SEG_H_

#include <vector>
#include "../include/goose_types.h"

class MsgDataElem;

///
/// @brief
///     GOOSE通道的报文数据的结构体
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
	                                             ///< Goose通道的报文值的数据表
	size_t m_GseDataLen;                         ///< Goose数据序列（All Data）的长度（整个TLV数据）
	size_t m_GseDataOff;                         ///< Goose数据序列（All Data）的在报文帧中的偏移地址
	std::vector<GOOSE_DATA *> m_vChannelActData; ///< Goose通道的真实值的数据表
	bool   m_bChange2ActData;                    ///< Goose通道的报文值是否已转换为真实值
};

#endif
