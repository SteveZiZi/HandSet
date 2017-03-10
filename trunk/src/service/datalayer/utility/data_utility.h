/// @file
///
/// @brief
///     数据层使用的辅助小函数
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.23
///
/// 修改历史：
///
#ifndef DATA_UTILITY_H__
#define DATA_UTILITY_H__

#include <string>
#include "src\utils\SmvScaler\SmvScaler.h"
#include "src\service\datalayer\config\ParamConfigBasic.h"
#include "src\service\datalayer\config\SmvRecvConfigPublic.h"
#include "src\service\datalayer\include\datatypes.h"


///
/// @brief
///    获取当前模块所在的路径
///
/// @note
///     c:\a\b\c.exe 返回 c:\a\b\
///
const std::wstring GetAppDir();

///
/// @brief
///    获取当前程序运行时配置文件路径
///
/// @note
///     c:\a\b\c.exe 返回 c:\a\e\
///     e为b同级目录，保存运行时配置
///
const std::wstring GetAppRunTimeCfgDir();

///
/// @brief
///     转换合适的浮点字符串
///
/// @param[in]   strVal           输入字符串
/// @param[out]  rstrResult       输出符合要求的浮点字符串
/// @param[out]  pdResult         如果指定了缓冲区，用double类型保存转换后的数据
///
/// @return
///     bool            转换成功返回true，失败返回false
///
bool StrFloatConvert(const std::wstring& strVal, std::wstring* pstrResult, double* pdResult = NULL);

///
/// @brief
///     转换合适的整型字符串
///
/// @param[in]   strVal           输入字符串
/// @param[out]  rstrResult       输出符合要求的浮点字符串
/// @param[out]  pnResult         如果指定了缓冲区，用uint32_t类型保存转换后的数据
///
/// @return
///     bool            转换成功返回true，失败返回false
///
bool StrUint32Convert(const std::wstring& strVal, std::wstring* pstrResult, uint32_t* pnResult = NULL);

///
/// @brief
///    浮点数转字符串
///
/// @param[in]      fVal        要转换的浮点数
/// @param[out]     strResult   保存转换后的结果
/// @param[in]      nPoints     小数点后位数
///
/// @return
///     bool       转换成功返回头true
///
bool FloatStrConvert(double fVal, std::wstring& strResult, uint32_t nPoints = 3);

///
/// @brief
///     将MAC地址由wstring转换为char数组
///
/// @param[in]
///     strMAC  - 要转换的MAC地址字符串
/// @param[in]
///     pucMAC  - 保存转换结果的char数组的指针
/// @param[in]
///     iLen    - 数组大小
///
/// @return
///     bool       转换成功返回true
///
bool ConvMacWStr2Char(const std::wstring strMAC, unsigned char* pucMAC, int iLen);

///
/// @brief
///     将MAC地址由char数组转换为wstring
///
/// @param[in]
///     pucMAC  - 存放MAC地址的char数组指针
/// @param[in]
///     iLen    - 数组大小
///
/// @return
///     wstring   MAC字符串
///
std::wstring ConvMacChar2WStr(unsigned char* pucMAC, int iLen);

///
/// @brief
///     将b码utc时间转换为SYSTEMTIME
///
/// @return
///     SYSTEMTIME    SystemTime表示的UTC时间
///
SYSTEMTIME UtcTime2SystemTime(UTC_TIME_t stUtcTime);

///
/// @brief
///    将字符串中指定的子字符串替换成新的字符串
///
/// @param[in]  str    字符串
/// @param[in]  old_value    老的子字符串
/// @param[in]  new_value    新的子字符串
///
void ReplaceSubStr(std::wstring& str, const std::wstring old_value, const std::wstring new_value);

///
/// @brief
///    根据通道属性，重新生成通道的描述信息
///
/// @param[in]  strOldDescription   当前通道描述信息
/// @param[in]  eOldChannelType     老的通道类型
/// @param[in]  eNewChannelType     新的通道类型
/// @param[in]  eOldChannelPhase    老的通道相角
/// @param[in]  eNewChannelPhase    新的通道相角
/// @param[in]  bOldIsMeasure       原来通道是否是测量类型
/// @param[in]  bNewIsMeasure       现在通道是否是测量类型
///
/// @return
///     std::wstring    新生成的描述信息
///
std::wstring GenerateChannelDes(std::wstring        strOldDescription, 
                                enumChannelType     eOldChannelType,
                                enumChannelType     eNewChannelType, 
                                ChannelPhase        eOldChannelPhase,
                                ChannelPhase        eNewChannelPhase, 
                                bool                bOldIsMeasure,
                                bool                bNewIsMeasure);

#endif //DATA_UTILITY_H__
