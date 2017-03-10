/// @file
///
/// @brief
///     SCL配置文件管理，使用src\utils\SclAnalysis模块
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///     chao        2013.4.7
///
/// 修改历史：
///     Modified by:    chao
///     Modified date:  2013.6.1
///     Version:        1.0.1
///     Descriptions:   获取了SCL文件中控制块更详细的信息
///

#ifndef __SCL_MANAGER_H__
#define __SCL_MANAGER_H__

#include <map>
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\include\datatypes.h"
#include "src\utils\SclAnalysis\include\sclanalysis.h"
#include "src\utils\SclAnalysis\include\SCDGOOSEAnalysis.h"
#include "src\utils\SclAnalysis\include\SCDSMV92Analysis.h"
#include "src\utils\SclAnalysis\include\ISCLCacheManager.h"
#include "src\service\datalayer\config\GooseCb.h"
#include "src\service\datalayer\config\IEC9_2Cb.h"

class CSCLManager
{
private:
    CSCLManager(void);
    ~CSCLManager(void);

public:
    ///
    /// @brief
    ///    获取静态实例，全局使用一个管理器
    ///
    static CSCLManager* getInstance();

    ///
    /// @brief
    ///    加载scl文件
    ///
    bool Load(std::wstring strFileName);

    ///
    /// @brief
    ///    卸载SCL文件
    ///
    void UnLoad();

    ///
    /// @brief
    ///    指示SCL管理器是否已成功加载了SCL文件
    ///
    /// @return
    ///     true 表示SCL管理器已经成功加载了SCL文件
    ///
    bool IsSclLoaded();

    ///
    /// @brief
    ///    枚举第一个GOOSE控制块
    ///
    /// @return
    ///     CGooseCb*    成功时返回GOOSE控制块指针，失败时返回NULL
    ///
    CGooseCb* FirstGooseCb();

    ///
    /// @brief
    ///    枚举下一个GOOSE控制块
    ///
    /// @param[in]  pCurGocb 当前位置GOOSE控制块的指针
    ///
    /// @return
    ///     CGooseCb*    成功时返回GOOSE控制块指针，失败时返回NULL
    ///
    CGooseCb* NextGooseCb(CGooseCb* pCurGocb);

    ///
    /// @brief
    ///     获得指定APPID的GOOSE控制块
    ///
    /// @param[in]  
    ///     iAPPID  -   控制块的APPID
    ///
    /// @return
    ///     CGooseCb*   成功时返回控制块的指针，失败时返回NULL
    ///
    CGooseCb* GetGooseCb(int iAPPID);

    ///
    /// @brief
    ///    获得GOOSE控制块个数
    ///
    /// @return
    ///     int    控制块个数
    ///
    int GetGooseCbCount();

    ///
    /// @brief
    ///    选中指定APPID的GOOSE控制块
    ///
    void SelectGooseCb(int iAPPID);

    ///
    /// @brief
    ///    清除所有GOOSE控制块的选中状态
    ///
    void UnSelectAllGooseCb();

    ///
    /// @brief
    ///    获得被选中的GOOSE控制块个数
    ///
    /// @return
    ///     int    被选中的控制块个数
    ///
    int GetSelectedGooseCbCount();

    ///
    /// @brief
    ///     枚举第一个IEC81850-9-2控制块
    ///
    /// @return
    ///     CIEC9_2Cb*    成功时返回第一个控制块的指针，失败时返回NULL
    ///
    CIEC9_2Cb* FirstIEC9_2Cb();

    ///
    /// @brief
    ///     枚举下一个IEC81850-9-2控制块
    ///
    /// @param[in]  
    ///     pCurIEC9_2Cb    -   当前位置IEC81850-9-2控制块的指针
    ///
    /// @return
    ///     CIEC9_2Cb*  成功时返回下一个控制块的指针，失败时返回NULL
    ///
    CIEC9_2Cb* NextIEC9_2Cb(CIEC9_2Cb* pCurIEC9_2Cb);

    ///
    /// @brief
    ///     获得指定APPID的IEC81850-9-2控制块
    ///
    /// @param[in]  
    ///     iAPPID    -   控制块的APPID
    ///
    /// @return
    ///     CIEC9_2Cb*  成功时返回控制块的指针，失败时返回NULL
    ///
    CIEC9_2Cb* GetIEC9_2Cb(int iAPPID);

    ///
    /// @brief
    ///    获得IEC81850-9-2控制块个数
    ///
    /// @return
    ///     int    控制块个数
    ///
    int GetIEC9_2CbCount();

    ///
    /// @brief
    ///    选中指定APPID的IEC81850-9-2控制块
    ///
    void SelectIEC9_2Cb(int iAPPID);

    ///
    /// @brief
    ///    清除所有IEC81850-9-2控制块的选中状态
    ///
    void UnSelectAllIEC9_2Cb();

    ///
    /// @brief
    ///    获得被选中的IEC81850-9-2控制块个数
    ///
    /// @return
    ///     int    被选中的控制块个数
    ///
    int GetSelectedIEC9_2CbCount();

private:
    ///
    /// @brief
    ///    对scl文件进行分析，获取scl中需要的信息
    ///
    /// @param[in]  strFileName     要分析的scl文件名
    ///
    /// @return
    ///     true 标识scl信息分析获取成功
    ///
    bool __Analysis(std::wstring strFileName);

    ///
    /// @brief
    ///    加载GOOSE控制块
    ///
    /// @param[in]  pSclCacheMgr scl缓存管理，用于加载卸载scl文件
    ///
    /// @return
    ///     bool    true表示加载成功
    ///
    bool __LoadGooseCb(ISCLCacheManager* pSclCacheMgr);

    ///
    /// @brief
    ///    加载GOOSE控制块的通道
    ///
    /// @param[in]  pGooseAnalysis 指向GOOSE配置解析接口对象的指针
    /// @param[in]  pGooseInfo 指向GOOSE控制块信息结构体的指针
    /// @param[in]  pGooseCb 指向GOOSE控制块对象的指针
    ///
    /// @return
    ///     bool    true表示加载成功
    ///
    bool __LoadGooseChannel(ISCDGOOSEAnalysis* pGooseAnalysis, SCDGOOSEANALYSIS_IEDGOOSE_INFO* pGooseInfo, CGooseCb* pGooseCb);

    ///
    /// @brief
    ///     加载GOOSE控制块的子通道
    ///
    /// @param[in]
    ///     pGooseAnalysis  -   指向GOOSE配置解析接口对象的指针
    /// @param[in]
    ///     pGooseInfo      -   指向GOOSE控制块信息结构体的指针
    /// @param[in]  
    ///     pGooseCb        -   指向GOOSE控制块对象的指针
    /// @param[in]  
    ///     stChannelInfo   -   IED的GOOSE数据集信息(单个通道)
    /// @param[in]  
    ///     pChannel        -   指向GOOSE通道的指针
    ///
    /// @return
    ///     bool    true表示加载成功
    ///
    bool __LoadGooseSubChannel(
        ISCDGOOSEAnalysis*                          pGooseAnalysis, 
        SCDGOOSEANALYSIS_IEDGOOSE_INFO*             pGooseInfo, 
        CGooseCb*                                   pGooseCb, 
        SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO&     stChannelInfo,
        CChannel*                                   pChannel
        );

    ///
    /// @brief
    ///    加载IEC61850-9-2控制块
    ///
    /// @param[in]  pSclCacheMgr scl缓存管理，用于加载卸载scl文件
    ///
    /// @return
    ///     bool    true表示加载成功
    ///
    bool __LoadIEC9_2Cb(ISCLCacheManager* pSclCacheMgr);

    ///
    /// @brief
    ///    加载IEC61850-9-2控制块的通道
    ///
    /// @param[in]  pSmvAnalysis 指向SCD文件SMV92配置解析接口的指针
    /// @param[in]  pSmvInfo 指向SMV92控制块信息的结构体的指针
    /// @param[in]  pIEC9_2Cb 指向IEC61850-9-2控制块对象的指针
    ///
    /// @return
    ///     bool    true表示加载成功
    ///
    bool __LoadIEC9_2Channel(ISCDSMV92Analysis* pSmvAnalysis, SCDSMV92ANALYSIS_IEDSMV_INFO* pSmvInfo, CIEC9_2Cb* pIEC9_2Cb);

    ///
    /// @brief
    ///    清空map保留所有的信息
    ///
    void __ClearAllMap();

    ///
    /// @brief
    ///    获取通道的相别信息
    ///
    /// @param[in] stSmvChannelInfo 采样值数据集中的数据通道信息
    ///
    /// @return
    ///     返回对应的通道相别 A/B/C相
    ///
    ChannelPhase __GetChannelPhase(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo);

    ///
    /// @brief
    ///    获取通道的类型信息
    ///
    /// @param[in] stSmvChannelInfo 采样值数据集中的数据通道信息
    ///
    /// @return
    ///     返回对应的通道类型信息 时间/电压/电流...
    ///
    enumChannelType __GetChannelType(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo);

    ///
    /// @brief
    ///    获取通道的类型信息
    ///
    /// @param[in] stGooseChannelInfo goose数据集中的数据通道信息
    ///
    /// @return
    ///     返回对应的通道类型信息 单电/双点/时间...
    ///
    enumChannelType __GetChannelType(const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO& stGooseChannelInfo);

    ///
    /// @brief
    ///    根据通道描述，判断通道是否是测量类型
    ///
    /// @param[in]  eChannelType    通道类型
    /// @param[in]  strDesc    通道描述
    ///
    /// @return
    ///     bool    true表示是测量类型
    ///
    bool __GetMeasure(enumChannelType eChannelType, std::wstring strDesc);

private:
    bool                                    m_bSclLoaded;               ///< 是否已成功加载了一个SCD文件

    std::map<uint32_t, CIEC9_2Cb*>          m_mapIEC9_2CbGroup;         ///< 保存smv控制块信息的map
    std::map<uint32_t, CGooseCb*>           m_mapGooseCbGroup;          ///< 保存goose控制块信息的map
};

#endif //__SCL_MANAGER_H__