/// @file
///
/// @brief
///     SMV发送配置
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.24
///
/// 修改历史：
///

#ifndef __PARAM_CONFIG_SMV_SEND_H__
#define __PARAM_CONFIG_SMV_SEND_H__
#include <map>
#include <vector>
#include "FT3Cb.h"
#include "IEC9_1Cb.h"
#include "IEC9_2Cb.h"
#include "ParamConfigBase.h"
#include "src/utils/cmarkup/markup.h"
#include "src/service/datalayer/include/datatypes.h"
#include "src/service/datalayer/recv/FrameRecvStruct.h"

class CParamConfigSMVSend : public CParamConfigBase
{
private:
    CParamConfigSMVSend(void);
    ~CParamConfigSMVSend(void);

public:
    ///
    /// @brief
    ///     获取配置实例接口
    ///
    static CParamConfigSMVSend* getInstance();

    ///
    /// @brief
    ///    加载配置文件
    ///
    bool LoadConfig( std::wstring strConfigFile );

    ///
    /// @brief
    ///    保存配置文件
    ///
    /// @return
    ///     bool    true表示保存成功
    ///
    bool SaveConfig();

    ///
    /// @brief
    ///    获得SMV类型
    ///
    /// @return
    ///     enumFrameType    获得的SMV类型
    ///
    enumFrameType GetSMVType();

    ///
    /// @brief
    ///    设置SMV类型
    ///
    /// @param[in]  eNewSMVType SMV类型
    ///
    void SetSMVType(enumFrameType eNewSMVType);

    ///
    /// @brief
    ///    获得采样值显示方式
    ///
    /// @return
    ///     enumSmvValueType    采样值显示方式
    ///
    enumSmvValueType GetSVDisplay();

    ///
    /// @brief
    ///    设置采样值显示方式
    ///
    /// @param[in]  eNewSVDisplay 采样值显示方式
    ///
    void SetSVDisplay(enumSmvValueType eNewSVDisplay);

    ///
    /// @brief
    ///    获得采样频率
    ///
    /// @return
    ///     FT3_SAMP_RATE    采样频率
    ///
    FT3_SAMP_RATE GetFrequency();

    ///
    /// @brief
    ///    设置采样频率
    ///
    /// @param[in]  eNewFrequency 采样频率
    ///
    void SetFrequency(FT3_SAMP_RATE eNewFrequency);

    ///
    /// @brief
    ///    获得波特率
    ///
    /// @return
    ///     FT3_BAUD_RATE    波特率
    ///
    FT3_BAUD_RATE GetBaudRate();

    ///
    /// @brief
    ///    设置波特率
    ///
    /// @param[in]  eBaudRate 波特率
    ///
    void SetBaudRate(FT3_BAUD_RATE eNewBaudRate);

    ///
    /// @brief
    ///    获得ASDU数目
    ///
    /// @return
    ///     int    ASDU数目，返回-1表示查询失败
    ///
    int GetASDUCount();

    ///
    /// @brief
    ///    设置ASDU数目
    ///
    /// @param[in]  iNewASDUCount ASDU数目
    ///
    /// @return
    ///     bool    true表示保存成功
    ///
    bool SetASDUCount(int iNewASDUCount);

    ///
    /// @brief
    ///    获得指定虚拟通道的交直流设置
    ///
    /// @param[in]  strChannelName 虚拟通道名
    ///
    /// @return
    ///     eCurrentType    虚拟通道的交直流设置
    ///
    eCurrentType GetCurrentType(std::wstring strChannelName);

    ///
    /// @brief
    ///    设置指定虚拟通道的交直流设置
    ///
    /// @param[in]  strChannelName 虚拟通道名
    /// @param[in]  eChannelCurType 交直流设置
    ///
    /// @return
    ///     bool    true表示设置成功
    ///
    bool SetCurrentType(std::wstring strChannelName, eCurrentType eChannelCurType);

    ///
    /// @brief
    ///    枚举第一个IEC61850-9-1控制块
    ///
    /// @return
    ///     CIEC9_1Cb*    成功时返回IEC61850-9-1控制块指针，失败时返回NULL
    ///
    CIEC9_1Cb* FirstIEC9_1Cb();

    ///
    /// @brief
    ///    获得下一个IEC61850-9-1控制块
    ///
    /// @param[in]  pCurIEC9_1Cb 当前IEC61850-9-1控制块的指针
    ///
    /// @return
    ///     CIEC9_1Cb*    成功时返回IEC61850-9-1控制块指针，失败时返回NULL
    ///
    CIEC9_1Cb* NextIEC9_1Cb(CIEC9_1Cb* pCurIEC9_1Cb);

    ///
    /// @brief
    ///    获得上一个IEC61850-9-1控制块
    ///
    /// @param[in]  pCurIEC9_1Cb 当前IEC61850-9-1控制块的指针
    ///
    /// @return
    ///     CIEC9_1Cb*    成功时返回IEC61850-9-1控制块指针，失败时返回NULL
    ///
    CIEC9_1Cb* PrevIEC9_1Cb(CIEC9_1Cb* pCurIEC9_1Cb);

    ///
    /// @brief
    ///    获得最后一个IEC61850-9-1控制块
    ///
    /// @return
    ///     CIEC9_1Cb*    成功时返回IEC61850-9-1控制块指针，失败时返回NULL
    ///
    CIEC9_1Cb* LastIEC9_1Cb();

    ///
    /// @brief
    ///    获得被选中的IEC61850-9-1控制块个数
    ///
    /// @return
    ///     int    被选中的控制块个数
    ///
    int GetSelectedIEC9_1CbCount();

    ///
    /// @brief
    ///    添加IEC61850-9-1控制块
    ///
    /// @return
    ///     CIEC9_1Cb*    新添加的IEC61850-9-1控制块对象的指针
    ///
    CIEC9_1Cb* AddIEC9_1Cb();

    ///
    /// @brief
    ///     添加IEC61850-9-1控制块（此函数会深度拷贝一份形参所指对象，然后添加到内部容器中）
    ///
    /// @param[in]
    ///     pNetCb   -   指向控制块的指针
    ///
    /// @return
    ///     CIEC9_1Cb*   成功时返回新添加的IEC61850-9-1控制块指针，失败时返回NULL
    ///
    CIEC9_1Cb* AddIEC9_1Cb(CNetCb* pNetCb);

    ///
    /// @brief
    ///     添加扫描列表中的SMV控制块到容器中（此函数会深度拷贝一份形参所指对象，然后添加到内部容器中）
    ///
    /// @param[in]
    ///     pDetectedIEC9_1Item    -   扫描列表中的SMV控制块指针
    ///
    /// @return
    ///     CIEC9_1Cb*    成功时返回新添加的IEC61850-9-1控制块指针，失败时返回NULL
    ///
    CIEC9_1Cb* AddIEC9_1Cb(CDetectItemFrame* pDetectedIEC9_1Item);

    ///
    /// @brief
    ///    删除IEC61850-9-1控制块
    ///
    /// @param[in]  pCurIEC9_1Cb IEC61850-9-1控制块指针
    ///
    /// @return
    ///     bool    true表示删除成功
    ///
    bool DeleteIEC9_1Cb(CIEC9_1Cb* pCurIEC9_1Cb);

    ///
    /// @brief
    ///    删除指定APPID的IEC61850-9-1控制块
    ///
    /// @param[in]  iAPPID 待删除的控制块APPID
    ///
    /// @return
    ///     bool    true表示删除成功
    ///
    bool DeleteIEC9_1Cb(int iAPPID);

    ///
    /// @brief
    ///    清除所有IEC61850-9-1控制块
    ///
    void ClearAllIEC9_1Cb();

    ///
    /// @brief
    ///    枚举第一个IEC61850-9-2控制块
    ///
    /// @return
    ///     CIEC9_2Cb*    成功时返回IEC61850-9-2控制块指针，失败时返回NULL
    ///
    CIEC9_2Cb* FirstIEC9_2Cb();

    ///
    /// @brief
    ///    获得下一个IEC61850-9-2控制块
    ///
    /// @param[in]  pCurIEC9_2Cb 当前IEC61850-9-2控制块的指针
    ///
    /// @return
    ///     CIEC9_2Cb*    成功时返回IEC61850-9-2控制块指针，失败时返回NULL
    ///
    CIEC9_2Cb* NextIEC9_2Cb(CIEC9_2Cb* pCurIEC9_2Cb);

    ///
    /// @brief
    ///    获得上一个IEC61850-9-2控制块
    ///
    /// @param[in]  pCurIEC9_2Cb 当前IEC61850-9-2控制块的指针
    ///
    /// @return
    ///     CIEC9_2Cb*    成功时返回IEC61850-9-2控制块指针，失败时返回NULL
    ///
    CIEC9_2Cb* PrevIEC9_2Cb(CIEC9_2Cb* pCurIEC9_2Cb);

    ///
    /// @brief
    ///    获得最后一个IEC61850-9-2控制块
    ///
    /// @return
    ///     CIEC9_2Cb*    成功时返回IEC61850-9-2控制块指针，失败时返回NULL
    ///
    CIEC9_2Cb* LastIEC9_2Cb();

    ///
    /// @brief
    ///    获得被选中的IEC61850-9-2控制块个数
    ///
    /// @return
    ///     int    被选中的控制块个数
    ///
    int GetSelectedIEC9_2CbCount();

    ///
    /// @brief
    ///    添加IEC61850-9-2控制块
    ///
    /// @return
    ///     CIEC9_2Cb*    新添加的IEC61850-9-2控制块对象的指针
    ///
    CIEC9_2Cb* AddIEC9_2Cb();

    ///
    /// @brief
    ///     添加IEC61850-9-2控制块（此函数会深度拷贝一份形参所指对象，然后添加到内部容器中）
    ///
    /// @param[in]
    ///     pIEC9_2Cb   -   IEC61850-9-2控制块指针
    ///
    /// @return
    ///     CIEC9_2Cb*   成功时返回新添加的IEC61850-9-2控制块指针，失败时返回NULL
    ///
    CIEC9_2Cb* AddIEC9_2Cb(CIEC9_2Cb* pIEC9_2Cb);

    ///
    /// @brief
    ///     添加扫描列表中的SMV控制块到容器中（此函数会深度拷贝一份形参所指对象，然后添加到内部容器中）
    ///
    /// @param[in]
    ///     pDetectedIEC9_2Item   -   扫描列表中的SMV控制块指针
    ///
    /// @return
    ///     CIEC9_2Cb*    成功时返回新添加的IEC61850-9-2控制块指针，失败时返回NULL
    ///
    CIEC9_2Cb* AddIEC9_2Cb(CDetectItemFrame* pDetectedIEC9_2Item);

    ///
    /// @brief
    ///    删除IEC61850-9-2控制块
    ///
    /// @param[in]  pCurIEC9_2Cb IEC61850-9-2控制块指针
    ///
    /// @return
    ///     bool    true表示删除成功
    ///
    bool DeleteIEC9_2Cb(CIEC9_2Cb* pCurIEC9_2Cb);

    ///
    /// @brief
    ///    删除指定APPID的IEC61850-9-2控制块
    ///
    /// @param[in]  iAPPID 待删除的控制块APPID
    ///
    /// @return
    ///     bool    true表示删除成功
    ///
    bool DeleteIEC9_2Cb(int iAPPID);

    ///
    /// @brief
    ///    清除所有IEC61850-9-2控制块
    ///
    void ClearAllIEC9_2Cb();

    ///
    /// @brief
    ///    获得第一个FT3控制块
    ///
    /// @return
    ///     CFT3Cb*    成功时返回CFT3控制块指针，失败时返回NULL
    ///
    CFT3Cb* FirstFt3Cb();

    ///
    /// @brief
    ///    获得下一个FT3控制块
    ///
    /// @param[in]  pCurFt3Cb 当前FT3控制块的指针
    ///
    /// @return
    ///     CFT3Cb*    成功时返回FT3控制块指针，失败时返回NULL
    ///
    CFT3Cb* NextFt3Cb(CFT3Cb* pCurFt3Cb);

    ///
    /// @brief
    ///    获得上一个FT3控制块
    ///
    /// @param[in]  pCurFt3Cb 当前FT3控制块的指针
    ///
    /// @return
    ///     CFT3Cb*    成功时返回FT3控制块指针，失败时返回NULL
    ///
    CFT3Cb* PrevFt3Cb(CFT3Cb* pCurFt3Cb);

    ///
    /// @brief
    ///    获得最后一个FT3控制块
    ///
    /// @return
    ///     CFT3Cb*    成功时返回FT3控制块指针，失败时返回NULL
    ///
    CFT3Cb* LastFt3Cb();

    ///
    /// @brief
    ///    获得被选中的FT3控制块个数
    ///
    /// @return
    ///     int    被选中的控制块个数
    ///
    int GetSelectedFt3CbCount();

    ///
    /// @brief
    ///    添加FT3控制块
    ///
    /// @return
    ///     CFT3Cb*    新添加的FT3控制块对象的指针
    ///
    CFT3Cb* AddFt3Cb();

    ///
    /// @brief
    ///     添加FT3控制块（此函数会深度拷贝一份形参所指对象，然后添加到内部容器中）
    ///
    /// @param[in]
    ///     pBaseCb   -   控制块指针
    ///
    /// @return
    ///     CFT3Cb*   成功时返回新添加的FT3控制块指针，失败时返回NULL
    ///
    CFT3Cb* AddFt3Cb(CBaseCb* pBaseCb);

    ///
    /// @brief
    ///     添加扫描列表中的SMV控制块到容器中（此函数会深度拷贝一份形参所指对象，然后添加到内部容器中）
    ///
    /// @param[in]
    ///     pDetectedFt3Item   -   扫描列表中的SMV控制块指针
    ///
    /// @return
    ///     CFT3Cb*    成功时返回新添加的FT3控制块指针，失败时返回NULL
    ///
    CFT3Cb* AddFt3Cb(CDetectItemFrame* pDetectedFt3Item);

    ///
    /// @brief
    ///    删除FT3控制块
    ///
    /// @param[in]  pCurFt3Cb FT3控制块指针
    ///
    /// @return
    ///     bool    true表示删除成功
    ///
    bool DeleteFt3Cb(CFT3Cb* pCurFt3Cb);

    ///
    /// @brief
    ///    删除指定APPID（即LDName）的FT3控制块
    ///
    /// @param[in]  iAPPID 待删除的控制块APPID（即LDName）
    ///
    /// @return
    ///     bool    true表示删除成功
    ///
    bool DeleteFt3Cb(int iAPPID);

    ///
    /// @brief
    ///    清除所有FT3控制块
    ///
    void ClearAllFt3Cb();

    ///
    /// @brief
    ///    获得第一个FT3扩展控制块
    ///
    /// @return
    ///     CFT3ExpandCb*    成功时返回CFT3扩展控制块指针，失败时返回NULL
    ///
    CFT3ExpandCb* FirstFt3ExpandCb();

    ///
    /// @brief
    ///    获得下一个FT3扩展控制块
    ///
    /// @param[in]  pCurFt3ExpandCb 当前FT3扩展控制块的指针
    ///
    /// @return
    ///     CFT3ExpandCb*    成功时返回FT3扩展控制块指针，失败时返回NULL
    ///
    CFT3ExpandCb* NextFt3ExpandCb(CFT3ExpandCb* pCurFt3ExpandCb);

    ///
    /// @brief
    ///    获得上一个FT3扩展控制块
    ///
    /// @param[in]  pCurFt3ExpandCb 当前FT3扩展控制块的指针
    ///
    /// @return
    ///     CFT3ExpandCb*    成功时返回FT3扩展控制块指针，失败时返回NULL
    ///
    CFT3ExpandCb* PrevFt3ExpandCb(CFT3ExpandCb* pCurFt3ExpandCb);

    ///
    /// @brief
    ///    获得最后一个FT3扩展控制块
    ///
    /// @return
    ///     CFT3ExpandCb*    成功时返回FT3扩展控制块指针，失败时返回NULL
    ///
    CFT3ExpandCb* LastFt3ExpandCb();

    ///
    /// @brief
    ///    获得被选中的FT3扩展控制块个数
    ///
    /// @return
    ///     int    被选中的控制块个数
    ///
    int GetSelectedFt3ExpandCbCount();

    ///
    /// @brief
    ///    添加FT3扩展控制块
    ///
    /// @return
    ///     CFT3ExpandCb*    新添加的FT3扩展控制块对象的指针
    ///
    CFT3ExpandCb* AddFt3ExpandCb();

    ///
    /// @brief
    ///     添加FT3扩展控制块（此函数会深度拷贝一份形参所指对象，然后添加到内部容器中）
    ///
    /// @param[in]
    ///     pBaseCb   -   控制块指针
    ///
    /// @return
    ///     CFT3ExpandCb*   成功时返回新添加的FT3扩展控制块指针，失败时返回NULL
    ///
    CFT3ExpandCb* AddFt3ExpandCb(CBaseCb* pBaseCb);

    ///
    /// @brief
    ///     添加扫描列表中的SMV控制块到容器中（此函数会深度拷贝一份形参所指对象，然后添加到内部容器中）
    ///
    /// @param[in]
    ///     pDetectedFt3ExpandItem   -   扫描列表中的SMV控制块指针
    ///
    /// @return
    ///     CFT3ExpandCb*    成功时返回新添加的FT3扩展控制块指针，失败时返回NULL
    ///
    CFT3ExpandCb* AddFt3ExpandCb(CDetectItemFrame* pDetectedFt3ExpandItem);

    ///
    /// @brief
    ///    删除FT3扩展控制块
    ///
    /// @param[in]  pCurFt3ExpandCb FT3扩展控制块指针
    ///
    /// @return
    ///     bool    true表示删除成功
    ///
    bool DeleteFt3ExpandCb(CFT3ExpandCb* pCurFt3ExpandCb);

    ///
    /// @brief
    ///    删除指定APPID（即LDName）的FT3扩展控制块
    ///
    /// @param[in]  iAPPID 待删除的控制块APPID（即LDName）
    ///
    /// @return
    ///     bool    true表示删除成功
    ///
    bool DeleteFt3ExpandCb(int iAPPID);

    ///
    /// @brief
    ///    清除所有FT3扩展控制块
    ///
    void ClearAllFt3ExpandCb();

    ///
    /// @brief
    ///    更新所有控制块中电压通道的一次额定值
    ///
    /// @param[in]  dNewValue    新的电压一次额定值（V）
    ///
    void UpdateRatedPrimaryVoltage(double dNewValue);

    ///
    /// @brief
    ///    更新所有控制块中电压通道的二次额定值
    ///
    /// @param[in]  dNewValue    新的电压二次额定值（V）
    ///
    void UpdateRatedSecondaryVoltage(double dNewValue);

    ///
    /// @brief
    ///    更新所有控制块中电流通道的一次额定值
    ///
    /// @param[in]  dNewValue    新的电流一次额定值（A）
    ///
    void UpdateRatedPrimaryCurrent(double dNewValue);

    ///
    /// @brief
    ///    更新所有控制块中电流通道的二次额定值
    ///
    /// @param[in]  dNewValue    新的电流二次额定值（A）
    ///
    void UpdateRatedSecondaryCurrent(double dNewValue);

    ///
    /// @brief
    ///    更新所有控制块中时间通道的额定值
    ///
    /// @param[in]  dNewValue    新的MU额定延时（us）
    ///
    void UpdateMuRatedDelay(double dNewValue);

    ///
    /// @brief
    ///     尝试勾选指定的IEC61850-9-1控制块
    ///
    /// @param[in]  pIec9_1Cb   指向控制块对象的指针
    ///
    /// @return
    ///     bool    勾选成功返回true
    ///
    bool TryToSelectIec9_1Cb(CIEC9_1Cb* pIec9_1Cb);

    ///
    /// @brief
    ///     尝试勾选指定的IEC61850-9-2控制块
    ///
    /// @param[in]  pIec9_2Cb   指向控制块对象的指针
    ///
    /// @return
    ///     bool    勾选成功返回true
    ///
    bool TryToSelectIec9_2Cb(CIEC9_2Cb* pIec9_2Cb);

    ///
    /// @brief
    ///     修正各种类型控制块的勾选数
    ///
    void ReviseSelectedCbCount();

private:
    ///
    /// @brief
    ///    添加IEC61850-9-1控制块
    ///
    /// @param[in]  pNewCb 指向IEC61850-9-1控制块对象的指针
    ///
    /// @return
    ///     bool    true表示添加成功
    ///
    bool __AddIEC9_1Cb(CIEC9_1Cb* pNewCb);

    ///
    /// @brief
    ///    添加IEC61850-9-2控制块
    ///
    /// @param[in]  pNewCb 指向IEC61850-9-2控制块对象的指针
    ///
    /// @return
    ///     bool    true表示添加成功
    ///
    bool __AddIEC9_2Cb(CIEC9_2Cb* pNewCb);

    ///
    /// @brief
    ///    添加FT3控制块
    ///
    /// @param[in]  pNewCb 指向FT3控制块对象的指针
    ///
    /// @return
    ///     bool    true表示添加成功
    ///
    bool __AddFt3Cb(CFT3Cb* pNewCb);

    ///
    /// @brief
    ///    添加FT3扩展控制块
    ///
    /// @param[in]  pNewCb 指向FT3扩展控制块对象的指针
    ///
    /// @return
    ///     bool    true表示添加成功
    ///
    bool __AddFt3ExpandCb(CFT3ExpandCb* pNewCb);

    ///
    /// @brief
    ///     获得指定APPID的IEC61850-9-1控制块（注意相同APPID的控制块可能有多个，此处只获得第一个匹配的控制块）
    ///
    /// @param[in]  
    ///     iAPPID  -   控制块的APPID
    ///
    /// @return
    ///     CGooseCb*   成功时返回控制块的指针，失败时返回NULL
    ///
    CIEC9_1Cb* __GetIEC9_1Cb(int iAPPID);

    ///
    /// @brief
    ///     获得指定APPID的IEC61850-9-2控制块（注意相同APPID的控制块可能有多个，此处只获得第一个匹配的控制块）
    ///
    /// @param[in]  
    ///     iAPPID  -   控制块的APPID
    ///
    /// @return
    ///     CGooseCb*   成功时返回控制块的指针，失败时返回NULL
    ///
    CIEC9_2Cb* __GetIEC9_2Cb(int iAPPID);

    ///
    /// @brief
    ///     获得指定APPID的FT3控制块（注意相同APPID的控制块可能有多个，此处只获得第一个匹配的控制块）
    ///
    /// @param[in]  
    ///     iAPPID  -   控制块的APPID
    ///
    /// @return
    ///     CFT3Cb*     成功时返回控制块的指针，失败时返回NULL
    ///
    CFT3Cb* __GetFt3Cb(int iAPPID);

    ///
    /// @brief
    ///     获得指定APPID的FT3扩展控制块（注意相同APPID的控制块可能有多个，此处只获得第一个匹配的控制块）
    ///
    /// @param[in]  
    ///     iAPPID  -       控制块的APPID
    ///
    /// @return
    ///     CFT3ExpandCb*   成功时返回控制块的指针，失败时返回NULL
    ///
    CFT3ExpandCb* __GetFt3ExpandCb(int iAPPID);

private:
    enumFrameType                           m_eSMVType;                 ///< SMV类型
    enumSmvValueType                        m_eSVDisplay;               ///< 采样值显示
    FT3_SAMP_RATE                           m_eFrequency;               ///< 采样频率
    FT3_BAUD_RATE                           m_eBuadRate;                ///< 波特率
    unsigned int                            m_uiASDUCount;              ///< ASDU数目

    std::map<std::wstring , eCurrentType>   m_mapCurrentType;           ///< 交直流设置
    std::list<CIEC9_1Cb *>                  m_listIEC9_1CbGroup;        ///< IEC61850-9-1控制块容器
    std::list<CIEC9_2Cb *>                  m_listIEC9_2CbGroup;        ///< IEC61850-9-2控制块容器
    std::list<CFT3Cb *>                     m_listFt3CbGroup;           ///< FT3控制块容器
    std::list<CFT3ExpandCb *>               m_listFt3ExpandCbGroup;     ///< FT3扩展控制块容器
};
#endif
