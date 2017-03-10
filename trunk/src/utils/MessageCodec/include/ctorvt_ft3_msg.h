/// @file
///     ctorvt_ft3_msg.h
///
/// @brief
///     定义电流/电压互感器FT3采样值报文帧的编解码接口及接口对象实例的创建和释放接口
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.09.05
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#ifndef CTORVT_FT3_MESSAGE_H_

#define CTORVT_FT3_MESSAGE_H_

#include "ctorvt_ft3_types.h"

///
/// @brief
///     电流/电压互感器FT3采样值报文的编解码接口
///
class CTorVTFT3Msg
{
public:
	virtual ~CTorVTFT3Msg(){};

	///
	/// @brief
	///     对捕获的电流/电压互感器FT3采样值报文帧进行解码
	///
	/// @param[in]
	///     msgDataBuf - 保存捕获的报文帧数据的缓存
	/// @param[in]
	///     bufSize    - 缓存大小
	///
	/// @return
	///     实际的解码数据大小 - 成功； 0 - 失败
	///
	virtual size_t Decode(const uint16_t *msgDataBuf, const size_t bufSize) = 0;

	///
	/// @brief
	///     根据设置的参数进行编码，获得电流/电压互感器FT3采样值报文帧的ASDU（即不含CRC及启动符）
	///
	/// @param[in]
	///     msgDataArr - 保存报文帧数据的数组
	/// @param[in]
	///     arrSize    - 数组大小
	///
	/// @return
	///     保存报文帧数据的数组的实际大小 - 成功； 0 - 失败
	///
	virtual size_t Code(uint16_t *msgDataArr, const size_t arrSize) = 0;

	///
	/// @brief
	///     获取ASDU的长度
	///
	/// @return
	///     ASDU的长度
	///
	virtual size_t GetASDULen() = 0;

	///
	/// @brief
	///     获取报文帧中的ASDU的公共信息，即ASDU中的采样值数据以外的部分
	///
	/// @param[in]
	///     asduPublic - 保存ASDU的公共信息的结构体
	///
	/// @return
	///     true - 成功； false - 失败
	///
	virtual bool GetASDUPublicInfo(CTORVT_FT3_ASDU_PUBLIC &asduPublic) = 0;

	///
	/// @brief
	///     获取报文帧中的ASDU中的采样值数据部分的通道数
	///
	/// @return
	///     ASDU中的采样值数据部分的通道数
	///
	virtual size_t GetASDUSmpsNum() = 0;

	///
	/// @brief
	///     获取报文帧中的ASDU中的采样值（通道值）数据
	///
	/// @param[in]
	///     smpValArr  - 保存采样瞬时值的数组
	/// @param[in]
	///     arrSize    - 数组大小,其值需>=通道数
	///
	/// @return
	///     数组的实际大小
	///
	virtual size_t GetASDUSmps(uint16_t *smpValArr, const size_t arrSize) = 0;

	///
	/// @brief
	///     获取ASDU在报文帧中的偏移地址
	///
	/// @return
	///     ASDU在报文帧中的偏移地址
	///
	virtual size_t GetASDUOff() = 0;

	///
	/// @brief
	///     获取报文帧中的ASDU的公共信息在报文帧中的偏移地址信息
	///
	/// @param[in]
	///     asduPublic - 保存ASDU的公共信息的偏移地址信息的结构体
	///
	/// @return
	///     
	///
	virtual void GetASDUPublicInfoOff(CTORVT_FT3_ASDU_PUBLIC_OFFSET &asduPublicOff) = 0;

	///
	/// @brief
	///     获取报文帧中的ASDU中的采样值数据在报文帧中的偏移地址信息
	///
	/// @param[in]
	///     smpOffArr - 保存采样瞬时值的偏移地址信息的数组
	/// @param[in]
	///     arrSize   - 数组的大小,其值需>=通道数
	///
	/// @return
	///     数组的实际大小
	///
	virtual size_t GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize) = 0;

	///
	/// @brief
	///     设置报文帧中的ASDU信息
	///
	/// @param[in]
	///     asduPublic - ASDU的公共信息
	/// @param[in]
	///     smpValBuf  - 保存采样瞬时值的缓存
	/// @param[in]
	///     bufSize    - 缓存大小
	///
	/// @return
	///     
	///
	virtual void SetASDU(const CTORVT_FT3_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t bufSize) = 0;
};

///
/// @brief
///     创建电流/电压互感器FT3采样值报文的编解码接口对象的实例
///
/// @param[in]
///     eCTorVTFt3Type - 电流/电压互感器FT3采样值报文的类型
///
/// @return
///     指向CTorVTFT3Msg实例的指针
///
CTorVTFT3Msg *CreateCTorVTFT3MsgImpl(CTORVT_FT3_TYPE eCTorVTFt3Type);

///
/// @brief
///     释放电流/电压互感器FT3采样值报文的编解码接口对象的实例
///
/// @param[in]
///     impl - 指向CTorVTFT3Msg实例的指针
///
/// @return
///     
///
void ReleaseCTorVTFT3MsgImpl(CTorVTFT3Msg *impl);

#endif
