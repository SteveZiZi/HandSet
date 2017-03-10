/// @file
///     goose_msg.h
///
/// @brief
///     Goose报文帧编解码的接口及接口对象实例的创建和释放接口
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
///    |2013.06.05     |1.0.0.605  |更改GOOSE报文的通道数据信息的获取接口                     |
///
#ifndef GOOSE_MESSAGE_H_

#define GOOSE_MESSAGE_H_

#include "src\include\stdint.h"
#include "etypes.h"
#include "goose_types.h"

///
/// @brief
///     IEC61850 GOOSE报文的编解码接口
///
class GooseMsg
{
public:
	virtual ~GooseMsg(){};

	///
	/// @brief
	///     对Goose报文帧进行解码
	///
	/// @param[in]
	///     msgDataBuf - 保存待解码的Goose报文帧数据的缓存
	/// @param[in]
	///     bufSize    - 缓存大小
	///
	/// @return
	///     实际的解码数据大小 - 成功； 0 - 失败
	///
	virtual size_t Decode(const uint8_t *msgDataBuf, const size_t bufSize) = 0;

	///
	/// @brief
	///     根据设置的参数进行编码，得到GoosePdu
	///
	/// @param[in]
	///     msgDataArr - 保存Goose报文帧数据的数组
	/// @param[in]
	///     arrSize    - 数组大小
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
	///     获取报文帧中goosePDU的长度，即整个TLV数据的长度
	///
	/// @return
	///     goosePDU的长度
	///
	virtual size_t GetGoosePduLen() = 0;

	///
	/// @brief
	///     获取报文帧中Goose数据序列（All Data）的长度（整个TLV数据）
	///
	/// @return
	///     Goose数据序列（All Data）的长度（整个TLV数据）
	///
	virtual size_t GetGooseDataLen() = 0;

	///
	/// @brief
	///     获取报文帧中的GoosePdu的公共信息，即GoosePdu中的通道数据以外的部分
	///
	/// @param[in]
	///     pduPublic - 保存ASDU的公共信息的结构体
	///
	/// @return
	///     true - 成功； false - 失败
	///
	virtual bool GetPduPublicInfo(GOOSE_PDU_PUBLIC &pduPublic) = 0;

	///
	/// @brief
	///     获取报文帧中的GoosePdu的公共信息，即GoosePdu中的通道数据以外的部分
	///
	/// @param[in]
	///     pduPublic - 保存ASDU的公共信息的结构体
	///
	/// @return
	///     true - 成功； false - 失败
	///
	virtual bool GetPduPublicInfo(GOOSE_PDU_PUBLIC_EX &pduPublic) = 0;

	///
	/// @brief
	///     获取报文帧中的数据通道的通道数
	///
	/// @return
	///     数据通道的通道数
	///
	virtual size_t GetChannelNum() = 0;

	///
	/// @brief
	///     获取GoosePdu中的单个通道的数据
	///     注意，返回的GOOSE_DATA指针由编解码类自己维护，外部释放会引起错误
	///
	/// @param[in]
	///     idxSeq     - GOOSE数据通道的顺序号，从0开始
	///
	/// @return
	///     指向Goose通道数据的指针
	///
	virtual GOOSE_DATA * GetAChannel(size_t idxSeq) = 0;

	///
	/// @brief
	///     获取Security可选项的配置信息
	///
	/// @return
	///     true - 报文中含有Security； false - 不含有
	///
	virtual bool GetSecurityOpt() = 0;

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
	///     获取goosePDU在报文帧中偏移地址
	///
	/// @return
	///     goosePDU在报文帧中偏移地址
	///
	virtual size_t GetGoosePduOff() = 0;

	///
	/// @brief
	///     获取报文帧中Goose数据序列（All Data）在报文帧中的偏移地址
	///
	/// @return
	///     Goose数据序列（All Data）的在报文帧中的偏移地址
	///
	virtual size_t GetGooseDataOff() = 0;

	///
	/// @brief
	///     获取GoosePdu的公共信息部分在报文帧中的偏移地址信息
	///
	/// @param[in]
	///     pduPblcOff - 保存GoosePdu的公共信息部分在报文帧中的偏移地址信息的结构体
	///
	/// @return
	///     
	///
	virtual void GetPduPublicInfoOff(GOOSE_PDU_PUBLIC_OFFSET &pduPblcOff) = 0;

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
	///     设置GoosePdu中公共部分的信息，即除通道数据以外的部分的信息
	///
	/// @param[in]
	///     pduPblc - GoosePdu中公共部分的信息
	///
	/// @return
	///
	///
	virtual void SetPduPublicInfo(GOOSE_PDU_PUBLIC &pduPblc) = 0;

	///
	/// @brief
	///     添加一个数据通道
	///     注意，添加的GOOSE_DATA在内部会深度拷贝，该类并不会对GOOSE_DATA指针进行维护，谁申请谁释放
	///
	/// @param[in]
	///     datBuf  - 保存单个GOOSE数据通道含有的数据的缓存
	/// @param[in]
	///     bufSize - 缓存大小，即单个GOOSE数据通道的数据数目（为1，表示该通道的数据非结构体类型）
	///
	/// @return
	///     true - 成功； false - 失败
	///
	virtual bool   AddAChannel(const GOOSE_DATA *channel) = 0;

	///
	/// @brief
	///     设置Security可选项的信息
	///
	/// @param[in]
	///     opt - 报文中是否包含该项
	///
	/// @return
	///     
	///
	virtual void SetSecurityOpt(bool opt) = 0;

	///
	/// @brief
	///     清空报文帧中GoosePDU的编解码结果
	///
	/// @return
	///     
	///
	virtual void ClearGoosePDU() = 0;
};

///
/// @brief
///     创建IEC61850 GOOSE报文的编解码接口对象的实例
///
/// @return
///     指向GooseMsg实例的指针
///
GooseMsg *CreateGooseMsgImpl();

///
/// @brief
///     释放IEC61850 GOOSE报文的编解码接口对象的实例
///
/// @param[in]
///     impl - 指向GooseMsg实例的指针
///
/// @return
///     
///
void ReleaseGooseMsgImpl(GooseMsg *impl);

#endif
