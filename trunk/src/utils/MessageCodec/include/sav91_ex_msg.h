/// @file
///     sav91_ex_msg.h
///
/// @brief
///     IEC61850-9-1扩展报文帧（由南京新宁光电制定，不常见）的编解码接口及接口对象实例的创建和释放接口
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.09.06
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#ifndef SAV91_EX_MESSAGE_H_

#define SAV91_EX_MESSAGE_H_

#include "etypes.h"
#include "sav91_ex_types.h"

///
/// @brief
///     IEC61850-9-1扩展报文帧的编解码接口
///
class Sav91ExMsg
{
public:
	virtual ~Sav91ExMsg(){};

	///
	/// @brief
	///     对IEC61850-9-1扩展报文帧进行解码
	///
	/// @param[in]
	///     msgDataBuf  - 保存待解码的报文帧数据的缓存
	/// @param[in]
	///     bufSize     - 缓存大小
	///
	/// @return
	///     实际的解码数据大小 - 成功； 0 - 失败
	///
	virtual size_t Decode(const uint8_t *msgDataBuf, const size_t bufSize) = 0;

	///
	/// @brief
	///     根据设置的参数进行编码，获得IEC61850-9-1扩展报文帧
	///
	/// @param[in]
	///     msgDataArr - 保存SavPDU数据流的数组
	/// @param[in]
	///     arrSize    - 数组大小
	///
	/// @return
	///     保存IEC61850-9-1扩展报文帧的数组的实际大小 - 成功； 0 - 失败
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
	///     获取ISO/IEC8802-3版本的以太网帧中的PDU的公共信息，即PDU中APDU以外的部分
	///
	/// @param[in]
	///     info - 保存ISO/IEC8802-3版本的以太网帧中的PDU的公共信息的结构体
	///
	/// @return
	///
	///
	virtual void GetIEC8802Info(IEC8802_3_PDU_PUBLIC &info) = 0;

	///
	/// @brief
	///     获取报文帧中Sav91ExPDU的长度
	///
	/// @return
	///     Sav91ExPDU的长度
	///
	virtual size_t GetSavPduLen() = 0;

	///
	/// @brief
	///     获取报文帧中ASDU的数目
	///
	/// @return
	///     报文帧包含的ASDU的数目
	///
	virtual uint16_t GetNoASDU() = 0;

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
	virtual bool GetASDUPublicInfo(const size_t idxSeq, SAV91_EX_ASDU_PUBLIC &asduPublic) = 0;

	///
	/// @brief
	///     获取ASDU中的采样值数据部分的通道数
	///
	/// @return
	///     ASDU的采样值数据部分的通道数
	///
	virtual size_t GetASDUSmpsNum() = 0;

	///
	/// @brief
	///     获取报文帧中ASDU中的采样值数据
	///
	/// @param[in]
	///     idxSeq    - 报文帧中ASDU的顺序号，从0开始
	/// @param[in]
	///     smpValArr - 保存采样瞬时值的数组
	/// @param[in]
	///     arrSize   - 数组大小
	///
	/// @return
	///     数组的实际大小
	///
	virtual size_t GetASDUSmps(const size_t idxSeq, uint16_t *smpValArr, const size_t arrSize) = 0;

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
	///     获取ISO/IEC8802-3版本的以太网帧中的PDU的公共信息在报文帧中的偏移地址信息
	///
	/// @param[in]
	///     infoOff - 保存ISO/IEC8802-3版本的以太网帧中的PDU的公共信息的偏移地址信息的结构体
	///
	/// @return
	///
	///
	virtual void GetIEC8802InfoOff(IEC8802_3_PDU_PUBLIC_OFFSET &infoOff) = 0;

	///
	/// @brief
	///     获取Sav91ExPDU在报文帧中的偏移地址
	///
	/// @return
	///     Sav91ExPDU在报文帧中的偏移地址
	///
	virtual size_t GetSavPduOff() = 0;

	///
	/// @brief
	///     获取ASDU的数目信息在报文帧中的偏移地址
	///
	/// @return
	///     ASDU的数目信息在报文帧中的偏移地址
	///
	virtual size_t GetNoASDUOff() = 0;

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
	virtual bool GetASDUPublicInfoOff(const size_t idxSeq, SAV91_EX_ASDU_PUBLIC_OFFSET &asduPublicOff) = 0;

	///
	/// @brief
	///     获取某一ASDU中的采样值数据在报文帧中的偏移地址信息
	///
	/// @param[in]
	///     idxSeq    - 报文帧中ASDU的顺序号，从0开始
	/// @param[in]
	///     smpOffArr - 保存采样瞬时值的偏移地址信息的数组
	/// @param[in]
	///     arrSize   - 数组大小
	///
	/// @return
	///     数组的实际大小
	///
	virtual size_t GetASDUSmpsOff(const size_t idxSeq, size_t *smpOffArr, const size_t arrSize) = 0;

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
	virtual void SetMacAddr(const uint8_t *desMac, const uint8_t *srcMac, size_t macLen) = 0;

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
	virtual void Set8021QVlanInfo(uint8_t userPriority, uint16_t vlanID) = 0;

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
	///     清空SavPDU的编解码结果
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
	///     bufSize    - 缓存大小
	///
	/// @return
	///     true - 成功； false - 失败
	///
	virtual bool AddAnASDU(const SAV91_EX_ASDU_PUBLIC &asduPublic, const uint16_t *smpValbuf, const size_t bufSize) = 0;
};

///
/// @brief
///     创建IEC61850-9-1扩展报文的编解码接口对象的实例
///
/// @return
///     指向Sav91ExMsg实例的指针
///
Sav91ExMsg *CreateSav91ExMsgImpl();

///
/// @brief
///     释放IEC61850-9-1扩展报文的编解码接口对象的实例
///
/// @param[in]
///     impl - 指向Sav91ExMsg实例的指针
///
/// @return
///     
///
void ReleaseSav91ExMsgImpl(Sav91ExMsg *impl);

#endif
