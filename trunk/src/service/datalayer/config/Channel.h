/// @file
///
/// @brief
///     通道
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.28
///
/// 修改历史：
///

#ifndef __CHANNEL_H__
#define __CHANNEL_H__
#include <list>
#include <map>
#include <math.h>
#include <string>
#include "ParamConfigBase.h"
#include "ParamConfigBasic.h"
#include "src\service\datalayer\include\datatypes.h"
#include "src\service\datalayer\send\ChannelRecvValues.h"
#include "src\utils\MessageCodec\include\goose_msg.h"

#define FLOAT_PRECISION                 (0.0005f)                       ///< 浮点数精度

///
/// @brief
///    状态对象类
///
class CStatus;

///
/// @brief
///    通道类
///
class CChannel
{
    friend class CBaseCb;
    friend class CGooseCb;
    friend class CIEC9_1Cb;
    friend class CIEC9_2Cb;
    friend class CFT3Cb;
    friend class CFT3ExpandCb;
    friend class CParamConfigGOOSEBase;
    friend class CParamConfigGOOSESend;
    friend class CParamConfigSMVSend;
    friend class CSCLManager;

protected:
    CChannel(void);
    virtual ~CChannel(void);

public:
    ///
    /// @brief
    ///     构造函数
    ///
    /// @param[in]  pParentBaseCb       指向父控制块的指针
    /// @param[in]  pParentChannel      指向父通道的指针
    /// @param[in]  pParamConfigBase    指向参数配置管理器的指针
    ///
    CChannel(CBaseCb* pParentBaseCb, CChannel* pParentChannel, CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    获得通道（子通道）在所属控制块（父通道）中的索引号（从0开始计数）
    ///
    /// @return
    ///     int    通道的索引号，-1表示失败。
    ///
    int GetIndex();

    ///
    /// @brief
    ///    获得控制块选中状态
    ///
    /// @return
    ///     bool    返回true表示控制块被选中
    ///
    bool IsSelected();

    ///
    /// @brief
    ///    设置控制块选中状态
    ///
    /// @param[in]  bSelected  控制块选中状态
    ///
    void Selected(bool bSelected);

    ///
    /// @brief
    ///    获得通道描述信息
    ///
    /// @return
    ///     wstring    通道描述信息
    ///
    std::wstring GetDescription();

    ///
    /// @brief
    ///    设置通道描述信息
    ///
    /// @param[in]  strDescription  通道描述信息
    ///
    void SetDescription(std::wstring strDescription);

    ///
    /// @brief
    ///    获得通道类型
    ///
    /// @return
    ///     enumChannelType    通道类型
    ///
    enumChannelType GetChannelType();

    ///
    /// @brief
    ///    设置通道类型
    ///
    /// @param[in]  eNewChannelType  通道类型
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool SetChannelType(enumChannelType eNewChannelType);

    ///
    /// @brief
    ///    获得通道测量类型
    ///
    /// @return
    ///     bool    通道是否属于测量类型
    ///
    bool IsMeasure();

    ///
    /// @brief
    ///    设置通道类型
    ///
    /// @param[in]  bNewMeasure  通道是否属于测量类型
    ///
    void SetMeasure(bool bNewMeasure);

    ///
    /// @brief
    ///    获得通道相位
    ///
    /// @return
    ///     enumChannelType    通道相位
    ///
    ChannelPhase GetChannelPhase();

    ///
    /// @brief
    ///    设置通道相位
    ///
    /// @param[in]  eNewChannelPhase  通道相位
    ///
    void SetChannelPhase(ChannelPhase eNewChannelPhase);

    ///
    /// @brief
    ///    获得一次额定值（电流单位为A，电压单位为V）
    ///
    /// @return
    ///     double    一次额定值
    ///
    double GetOnceRatings();

    ///
    /// @brief
    ///    设置一次额定值（电流单位为A，电压单位为V）
    ///
    /// @param[in]  dOnceRatings  一次额定值
    ///
    void SetOnceRatings(double dOnceRatings);

    ///
    /// @brief
    ///    获得二次额定值（电流单位为A，电压单位为V）
    ///
    /// @return
    ///     double    二次额定值
    ///
    double GetSecondaryRatings();

    ///
    /// @brief
    ///    设置二次额定值（电流单位为A，电压单位为V）
    ///
    /// @param[in]  dSecondaryRatings  二次额定值
    ///
    void SetSecondaryRatings(double dSecondaryRatings);

    ///
    /// @brief
    ///    获得通道映射
    ///
    /// @return
    ///     wstring    通道映射
    ///
    std::wstring GetVirtualChannel();

    ///
    /// @brief
    ///    设置通道映射
    ///
    /// @param[in]  strVirtualChannel  通道映射
    ///
    void SetVirtualChannel(std::wstring strVirtualChannel);

    ///
    /// @brief
    ///    枚举第一个子通道
    ///
    /// @return
    ///     CChannel*    成功时返回通道指针，失败时返回NULL
    ///
    CChannel* FirstSubChannel();

    ///
    /// @brief
    ///    获得下一个子通道
    ///
    /// @param[in]  pCurSubChannel 当前子通道的指针
    ///
    /// @return
    ///     CChannel*    成功时返回子通道指针，失败时返回NULL
    ///
    CChannel* NextSubChannel(CChannel* pCurSubChannel);

    ///
    /// @brief
    ///    获得上一个子通道
    ///
    /// @param[in]  pCurSubChannel 当前子通道的指针
    ///
    /// @return
    ///     CChannel*    成功时返回子通道指针，失败时返回NULL
    ///
    CChannel* PrevSubChannel(CChannel* pCurSubChannel);

    ///
    /// @brief
    ///    清除所有子通道
    ///
    void ClearAllSubChannel();

    ///
    /// @brief
    ///    获得子通道数
    ///
    /// @return
    ///     int    子通道数
    ///
    int GetSubChannelCount();

    ///
    /// @brief
    ///     获得通道所属控制块对象的指针
    ///
    /// @return
    ///     CBaseCb*    通道所属控制块对象的指针
    ///
    CBaseCb* GetParentBaseCb();

    ///
    /// @brief
    ///    获得父通道指针
    ///
    /// @return
    ///     CChannel*    父通道指针
    ///
    CChannel* GetParentChannel();

    ///
    /// @brief
    ///    获得GOOSE通道的发送值
    ///
    /// @return
    ///     GOOSE_DATA_VARIANT    GOOSE通道的初始值
    ///
    GOOSE_DATA_VARIANT& GetGooseSendValue();

    ///
    /// @brief
    ///    设置GOOSE通道的发送值
    ///
    /// @param[in]  stNewGooseValue    GOOSE通道的发送值
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool SetGooseSendValue(GOOSE_DATA_VARIANT& stNewGooseValue);

    ///
    /// @brief
    ///    获得GOOSE通道的实时值，在接收到GOOSE报文时会修改通道的实时值
    ///
    /// @return
    ///     GOOSE_DATA_VARIANT    GOOSE通道的实时值
    ///
    GOOSE_DATA_VARIANT& GetGooseRecvValue();

    ///
    /// @brief
    ///    设置GOOSE通道的实时值，在接收到GOOSE报文时，可以调用此函数修改通道的实时值
    ///
    /// @param[in]  stGooseValue    GOOSE通道的实时值
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool SetGooseRecvValue(GOOSE_DATA_VARIANT& stGooseValue);

    ///
    /// @brief
    ///    获得通道品质值
    ///
    /// @return
    ///     unsigned short    通道品质值
    ///
    unsigned short GetQuality();

    ///
    /// @brief
    ///    设置通道品质值
    ///
    /// @param[in]  usNewQuality    新的品质值
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool SetQuality(unsigned short usNewQuality);

    ///
    /// @brief
    ///     通道的发送值是否被改变
    ///
    /// @return
    ///     bool    true表示通道的发送值被改变
    ///
    bool IsSendValueChanged();

    ///
    /// @brief
    ///    设置通道的发送值是否被改变
    ///
    /// @param[in]  bIsChanged    通道值是否被改变
    ///
    void SetSendValueChanged(bool bIsChanged);

    ///
    /// @brief
    ///    获得通道值在通信报文中所占字节数
    ///
    /// @return
    ///     int    字节数
    ///
    int GetValueLength();

protected:

    ///
    /// @brief
    ///    添加子通道
    ///
    /// @return
    ///     CChannel*    成功时返回子通道指针，失败时返回NULL
    ///
    CChannel* __AddSubChannel();

    ///
    /// @brief
    ///    添加子通道
    ///
    /// @param[in]  pNewChannel 指向Channel对象的指针
    ///
    /// @return
    ///     bool    true表示添加成功
    ///
    bool __AddSubChannel(CChannel* pNewChannel);

    ///
    /// @brief
    ///    深度拷贝一个通道
    ///
    /// @param[in]  pChannel    指向Channel对象的指针
    ///
    /// @return
    ///     bool    拷贝成功返回TRUE
    ///
    virtual bool __Copy(CChannel* pChannel);

    ///
    /// @brief
    ///    深度拷贝一个GOOSE_DATA对象
    ///
    /// @param[in]  pGooseData 指向GOOSE_DATA对象的指针
    ///
    /// @return
    ///     bool    拷贝成功返回TRUE
    ///
    virtual bool __Copy(GOOSE_DATA* pGooseData);

    ///
    /// @brief
    ///    设置通道所属的控制块
    ///
    /// @param[in]  pBaseCb    控制块指针
    ///
    /// @return
    ///     void    
    ///
    void __SetParentBaseCb(CBaseCb* pBaseCb);

    ///
    /// @brief
    ///    设置通道所属的父通道指针
    ///
    /// @param[in]  pChannel    父通道指针
    ///
    /// @return
    ///     void    
    ///
    void __SetParentChannel(CChannel* pChannel);

    ///
    /// @brief
    ///    判断通道发送值是否与指定通道的相等
    ///
    /// @param[in]  pOldChannel    指向通道对象的指针
    ///
    /// @return
    ///     bool    通道值相等返回true
    ///
    bool __CompareSendValue(CChannel* pOldChannel);

    ///
    /// @brief
    ///    加载SMV<Channel>
    ///
    /// @param[in]  pXml 指向XML对象的引用
    ///
    /// @return
    ///     bool    true表示加载成功
    ///
    bool __LoadSmvConfig(CMarkup& xml);

    ///
    /// @brief
    ///    保存SMV<Channel>
    ///
    /// @param[in]  pXml 指向XML对象的引用
    ///
    /// @return
    ///     bool    true表示加载成功
    ///
    bool __SaveSmvConfig(CMarkup& xml);

    ///
    /// @brief
    ///    加载GOOSE<Channel>
    ///
    /// @param[in]  pXml 指向XML对象的引用
    ///
    /// @return
    ///     bool    true表示加载成功
    ///
    bool __LoadGooseConfig(CMarkup& xml);

    ///
    /// @brief
    ///    保存GOOSE<Channel>
    ///
    /// @param[in]  pXml 指向XML对象的引用
    ///
    /// @return
    ///     bool    true表示加载成功
    ///
    bool __SaveGooseConfig(CMarkup& xml);

    ///
    /// @brief
    ///    修改GOOSE控制块中单点/双点类型通道时，需要自动修改相邻/间隔的时间通道的值
    ///    此函数自动寻找寻找变位的单点/双点通道，然后改变相邻或间隔的时间通道的值
    ///
    /// @param[in]  stNewTime    单点/双点通道的变位时间
    ///
    void __UpdateSubChannelTime(CEventTime stNewTime);

    ///
    /// @brief
    ///     对发生了变位的子通道进行重新编码
    ///
    /// @param[in]  pucFrameHead    指向发送帧帧头的指针
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool __RecodeChangedSubChannels(unsigned char* pucFrameHead);

    ///
    /// @brief
    ///     查找所有需要变位的时间子通道，将子通道指针保存到容器中
    ///
    /// @param[out]  listShouldChangeTimeChannel    保存需要变位时间通道的指针
    ///
    void __SaveSubTimeChannel(std::list<CChannel*>& listShouldChangeTimeChannel);

    ///
    /// @brief
    ///    计算通道的偏移量
    ///
    /// @param[in]  pGooseData      保存GOOSE通道信息的结构体
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool __CalcChannelOffset( GOOSE_DATA* pGooseData );

private:
    CBaseCb*                            m_pParentBaseCb;                ///< 通道所属的控制块
    CChannel*                           m_pParentChannel;               ///< 子通道的父通道
    CParamConfigBase*                   m_pParamConfigBase;             ///< 参数配置管理器
    CChannel*                           m_pSourceChannel;               ///< 通道拷贝源
    CChannel*                           m_pPrev;                        ///< 前一个通道
    CChannel*                           m_pNext;                        ///< 后一个通道

    bool                                m_bSelected;                    ///< 通道是否被选择
    std::wstring                        m_strDescription;               ///< 通道描述信息
    enumChannelType                     m_eChannelType;                 ///< 通道类型
    unsigned short                      m_usQuality;                    ///< IEC61850-9-2通道品质值
    bool                                m_bMeasure;                     ///< 是否属于测量类型
    ChannelPhase                        m_eChannelPhase;                ///< 相位
    double                              m_dOnceRatings;                 ///< 一次额定值（电流单位为A，电压单位为V）
    double                              m_dSecondaryRatings;            ///< 二次额定值（电流单位为A，电压单位为V）
    std::wstring                        m_strVirtualChannel;            ///< 通道映射

    GOOSE_DATA_VARIANT                  m_stGooseSendValue;             ///< 当前GOOSE通道发送值，用于保存手动试验时配置的GOOSE通道值（需要保存到配置文件中）
    bool                                m_bIsSendValueChanged;          ///< GOOSE通道发送值是否发生了改变（在手动实验时会进行判断）
    GOOSE_DATA_VARIANT                  m_stGooseRecvValue;             ///< 当前GOOSE通道接收值，接收GOOSE报文时用于保存通道的实时值（不需要保存到配置文件中）

    std::list<CChannel *>               m_listSubChannelGroup;          ///< 子通道
};
#endif
