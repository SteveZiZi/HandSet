/// @file
///     sav92_msg.h
///
/// @brief
///     IEC61850-9-2LE报文帧的编解码接口及接口对象实例的创建和释放接口
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.3.18
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#ifndef SAV92_MESSAGE_H_

#define SAV92_MESSAGE_H_

#include "etypes.h"
#include "sav92_types.h"

///
/// @brief
///     IEC61850-9-2采样值报文的编解码接口
///
class Sav92Msg
{
public:
	virtual ~Sav92Msg(){};

	///
	/// @brief
	///     对IEC61850-9-2采样值报文帧进行解码
	///
	/// @param[in]
	///     msgDataBuf - 保存待解码的报文帧数据的缓存
	/// @param[in]
	///     bufSize    - 缓存大小
	///
	/// @return
	///     实际的解码数据大小 - 成功； 0 - 失败
	///
	virtual size_t Decode(const uint8_t *msgDataBuf, const size_t bufSize) = 0;

	///
	/// @brief
	///     根据设置的参数进行编码，获得IEC61850-9-2报文帧
	///
	/// @param[in]
	///     msgDataArr - 保存报文帧数据的数组
	/// @param[in]
	///     arrSize    - 数组的元素数目
	///
	/// @return
	///     保存报文帧数据的数组的实际大小 - 成功； 0 - 失败
	///
	virtual size_t Code(uint8_t *msgDataArr, const size_t arrSize) = 0;

	///
	/// @brief
	///     获取以太网报文帧头信息
	///
	/// @param[in]
	///     ethHdr - 保存以太网报文帧头信息的结构体
	///
	/// @return
	///
	///
	virtual void GetEthernetHeader(ETHER_II_HEADER &ethHdr) = 0;

	///
	/// @brief
	///     获取IEEE802.1Q VLAN标签信息
	///
	/// @param[in]
	///     info - 保存IEEE802.1Q VLAN标签信息的结构体
	///
	/// @return
	///
	///
	virtual void Get8021QVlanInfo(IEEE802_1Q_VLAN_INFO &info) = 0;

	///
	/// @brief
	///     获取符合ISO/IEC8802-3的报文帧的PDU的公共信息，即PDU中APDU以外的部分
	///
	/// @param[in]
	///     info - 保存符合ISO/IEC8802-3的报文帧的PDU的公共信息的结构体
	///
	/// @return
	///
	///
	virtual void GetIEC8802Info(IEC8802_3_PDU_PUBLIC &info) = 0;

	///
	/// @brief
	///     获取报文帧中Sav92PDU(TLV形式)的长度
	///
	/// @return
	///     Sav92PDU的长度
	///
	virtual size_t GetSavPduLen() = 0;

	///
	/// @brief
	///     获取报文帧中的ASDU的数目
	///
	/// @param[in]
	///     noASDU - 表示该报文帧包含的ASDU的数目，与采样率有关，length≤2 byte
	///
	/// @return
	///
	///
	virtual uint16_t GetNoASDU() = 0;

	///
	/// @brief
	///     获取报文帧中某一ASDU（TLV形式）的长度
	///
	/// @return
	///     ASDU（TLV形式）的长度
	///
	virtual size_t GetASDULen(const size_t idxSeq) = 0;

	///
	/// @brief
	///     获取报文帧中某一ASDU的Sequence of Data（TLV形式）的长度
	///
	/// @return
	///     Sequence of Data（TLV形式）的长度
	///
	virtual size_t GetSeqOfDatLen(const size_t idxSeq) = 0;

	///
	/// @brief
	///     获取报文帧中的ASDU的公共信息，即ASDU中的采样值数据以外的部分
	///
	/// @param[in]
	///     idxSeq     - 报文帧中ASDU的顺序号，从0开始
	/// @param[in]
	///     asduPublic - 保存ASDU的公共信息的结构体
	///
	/// @return
	///     true - 成功； false - 失败
	///
	virtual bool GetASDUPublicInfo(const size_t idxSeq, SAV92_ASDU_PUBLIC &asduPublic) = 0;

	///
	/// @brief
	///     获取报文帧中的ASDU中的采样计数器信息
	///
	/// @param[in]
	///     idxSeq - 报文帧中ASDU的顺序号，从0开始
	/// @param[in]
	///     smpCnt - 保存采样计数器信息
	///
	/// @return
	///     true - 成功； false - 失败
	///
	virtual bool GetSmpCntInASDU(const size_t idxSeq, uint16_t &smpCnt) = 0;

	///
	/// @brief
	///     获取报文帧中的ASDU中的同步标识位信息
	///
	/// @param[in]
	///     idxSeq   - 报文帧中ASDU的顺序号，从0开始
	/// @param[in]
	///     smpSynch - 保存同步标识位信息
	///
	/// @return
	///     true - 成功； false - 失败
	///
	virtual bool GetSmpSynchInASDU(const size_t idxSeq, bool &smpSynch) = 0;

	///
	/// @brief
	///     获取某一ASDU中的采样值数据部分的通道数
	///
	/// @param[in]
	///     idxSeq  - 报文帧中ASDU的顺序号，从0开始
	///
	/// @return
	///     ASDU中的采样值数据部分的通道数
	///
	virtual size_t GetASDUSmpsNum(const size_t idxSeq) = 0;

	///
	/// @brief
	///     获取报文帧中ASDU中的采样值数据
	///
	/// @param[in]
	///     idxSeq    - 报文帧中ASDU的顺序号，从0开始
	/// @param[in]
	///     smpValArr - 保存采样瞬时值的数组
	/// @param[in]
	///     smpQltArr - 保存采样瞬时值的品质值的数组
	/// @param[in]
	///     arrSize   - 两数组的大小
	///
	/// @return
	///     数组的实际大小
	///
	virtual size_t GetASDUSmps(const size_t idxSeq, uint32_t *smpValArr, uint32_t *smpQltArr, const size_t arrSize) = 0;

	///
	/// @brief
	///     获取SavPDU中的可选项信息
	///
	/// @param[in]
	///     asduOpts - 保存可选项信息的结构体
	///
	/// @return
	///     
	///
	virtual void GetSavPduOpts(SAV92_SAVPDU_OPTS &pduOpts) = 0;

	///
	/// @brief
	///     获取以太网报文帧头信息在报文帧中的偏移地址信息
	///
	/// @param[in]
	///     ethHdrOff - 保存以太网报文帧头信息的偏移地址信息的结构体
	///
	/// @return
	///
	///
	virtual void GetEthernetHeaderOff(ETHER_II_HEADER_OFFSET &ethHdrOff) = 0;

	///
	/// @brief
	///     获取IEEE802.1Q VLAN标签信息在报文帧中的偏移地址信息
	///
	/// @param[in]
	///     infoOff - 保存IEEE802.1Q VLAN标签信息的偏移地址信息的结构体
	///
	/// @return
	///
	///
	virtual void Get8021QVlanInfoOff(IEEE802_1Q_VLAN_INFO_OFFSET &infoOff) = 0;

	///
	/// @brief
	///     获取符合ISO/IEC8802-3的报文帧的PDU的公共信息在报文帧中的偏移地址信息
	///
	/// @param[in]
	///     infoOff - 保存符合ISO/IEC8802-3的报文帧的PDU的公共信息的偏移地址信息的结构体
	///
	/// @return
	///
	///
	virtual void GetIEC8802InfoOff(IEC8802_3_PDU_PUBLIC_OFFSET &infoOff) = 0;

	///
	/// @brief
	///     获取Sav92PDU在报文帧中的偏移地址
	///
	/// @return
	///     Sav92PDU在报文帧中的偏移地址
	///
	virtual size_t GetSavPduOff() = 0;

	///
	/// @brief
	///     获取ASDU的数目信息在报文帧中的偏移地址
	///
	/// @return
	///     报文帧中的ASDU的数目信息的偏移地址
	///
	virtual size_t GetNoASDUOff() = 0;

	///
	/// @brief
	///     获取某个ASDU在报文帧中的偏移地址
	///
	/// @param[in]
	///     idxSeq - 报文帧中ASDU的顺序号，从0开始
	///
	/// @return
	///     ASDU在报文帧中的偏移地址
	///
	virtual size_t GetASDUOff(const size_t idxSeq) = 0;

	///
	/// @brief
	///     获取报文帧中的ASDU的公共信息在报文帧中的偏移地址信息
	///
	/// @param[in]
	///     idxSeq     - 报文帧中ASDU的顺序号，从0开始
	/// @param[in]
	///     asduPublic - 保存ASDU的公共信息的偏移地址信息的结构体
	///
	/// @return
	///     true - 成功； false - 失败
	///
	virtual bool GetASDUPublicInfoOff(const size_t idxSeq, SAV92_ASDU_PUBLIC_OFFSET &asduPublicOff) = 0;

	///
	/// @brief
	///     获取ASDU中的采样值数据在报文帧中的偏移地址信息
	///
	/// @param[in]
	///     idxSeq       - 报文帧中ASDU的顺序号，从0开始
	/// @param[in]
	///     seqOfDataOff - 采样瞬时值序列的偏移地址
	/// @param[in]
	///     smpValOffArr - 保存采样瞬时值的偏移地址信息的数组
	/// @param[in]
	///     smpQltOffArr - 保存采样瞬时值的品质值的偏移地址信息的数组
	/// @param[in]
	///     arrSize      - 两数组的大小
	///
	/// @return
	///     数组的实际大小
	///
	virtual size_t GetASDUSmpsOff(const size_t idxSeq, size_t &seqOfDataOff, size_t *smpValOffArr,
	                              size_t *smpQltOffArr, const size_t arrSize) = 0;

	///
	/// @brief
	///     设置Mac地址
	///
	/// @param[in]
	///     desMac - 指向目的Mac地址的指针
	/// @param[in]
	///     srcMac - 指向源Mac地址的指针
	/// @param[in]
	///     macLen - MAC地址的长度
	///
	/// @return
	///
	///
	virtual void SetMacAddr(const uint8_t *desMac, const uint8_t *srcMac, size_t macLen, bool isHasVlan) = 0;

	///
	/// @brief
	///     设置IEEE802.1Q VLAN标签信息
	///
	/// @param[in]
	///     userPriority - 用户优先级（0~7）
	/// @param[in]
	///     vlanID       - 虚拟局域网标识，用于标记该报文应该发送到哪个VLAN（12位）
	///
	/// @return
	///
	///
	virtual void Set8021QVlanInfo(uint8_t userPriority, uint16_t vlanID, bool isHasVlan) = 0;

	///
	/// @brief
	///     设置AppID
	///
	/// @param[in]
	///     appID - 应用信息标识码，用于区别应用关联，即区分GOOSE和模拟量采样值（SAV）协议
	///
	/// @return
	///
	///
	virtual void SetAppID(const uint16_t appID) = 0;

	///
	/// @brief
	///     清空Sav92PDU的编解码结果
	///
	/// @return
	///     
	///
	virtual void ClearSavPDU() = 0;

	///
	/// @brief
	///     添加一个ASDU到报文帧中
	///
	/// @param[in]
	///     asduPublic - ASDU的公共信息
	/// @param[in]
	///     smpValBuf  - 保存采样瞬时值的缓存
	/// @param[in]
	///     smpQltBuf  - 保存采样瞬时值的品质值的缓存
	/// @param[in]
	///     bufSize    - 缓存的大小,即通道数
	///
	/// @return
	///     true - 成功； false - 失败
	///
	virtual bool AddAnASDU(const SAV92_ASDU_PUBLIC &asduPublic, const uint32_t *smpValBuf,
		                   const uint32_t *smpQltBuf, const size_t bufSize) = 0;

	///
	/// @brief
	///     设置SavPDU中的可选项信息，必须在AddAnASDU()之前设置
	///
	/// @param[in]
	///     asduOpts - 可选项信息
	///
	/// @return
	///     
	///
	virtual void SetSavPduOpts(SAV92_SAVPDU_OPTS &pduOpts) = 0;
};

///
/// @brief
///     创建IEC61850-9-2采样值报文的编解码接口对象的实例
///
/// @return
///     指向Sav92Msg实例的指针
///
Sav92Msg *CreateSav92MsgImpl();

///
/// @brief
///     释放IEC61850-9-2采样值报文的编解码接口对象的实例
///
/// @param[in]
///     impl - 指向Sav92Msg实例的指针
///
/// @return
///     
///
void ReleaseSav92MsgImpl(Sav92Msg *impl);

#endif
