/// @file
///
/// @brief
///     手动试验参数配置
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.06.07
///
/// 修改历史：
///

#ifndef __PARAM_CONFIG_MANUAL_TEST_H__
#define __PARAM_CONFIG_MANUAL_TEST_H__
#include <map>
#include "paramconfigbase.h"
#include "SMVSendVirtualChannel.h"

///
/// @brief
///    SMV变化项
///
enum eSMVChangeItem
{
    CHANGE_ITEM_UNKNOWN = 0,                                            ///< 未知
    CHANGE_ITEM_AMPLITUDE,                                              ///< 幅值
    CHANGE_ITEM_PHASE_ANGLE,                                            ///< 相角
    CHANGE_ITEM_FREQUENCY                                               ///< 频率
};

///
/// @brief
///    修改生效方式
///
enum eModifyEffectiveMode
{
    EFFECTIVE_MODE_UNKNOWN = 0,                                         ///< 未知
    EFFECTIVE_MODE_IMMEDIATE,                                           ///< 即时生效
    EFFECTIVE_MODE_MANUALLY                                             ///< 手动生效
};

///
/// @brief
///    试验开始方式
///
enum eStartMode
{
    START_MODE_UNKNOWN = 0,                                             ///< 未知
    START_MODE_APART,                                                   ///< 分别开始
    START_MODE_TOGETHER                                                 ///< 同时开始
};

///
/// @brief
///    手动试验
///
class CParamconfigManualTest :
    public CParamConfigBase
{
    friend class CSMVSendVirtualChannel;
    friend class CStateTestResult;

protected:
    CParamconfigManualTest(void);
    virtual ~CParamconfigManualTest(void);

public:
    ///
    /// @brief
    ///     获取配置实例接口
    ///
    static CParamconfigManualTest* getInstance();

    ///
    /// @brief
    ///    加载配置文件
    ///
    /// @param[in]  strConfigFile 文件路径
    ///
    /// @return
    ///     bool    true表示加载成功
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
    ///    生成所有SMV发送虚拟通道的真实通道映射表
    ///    注意：调用FirstSMVSendVirtualChannel()函数时会自动调用此函数，
    ///           如果需要重新生成映射表，可以手动调用此函数
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool RebuildSMVSendChannelMap();

    ///
    /// @brief
    ///    枚举第一个被映射的SMV发送虚拟通道
    ///
    /// @param[in]  bRebuildMap     是否需要重建SMV发送映射表、GOOSE接收映射表
    ///
    /// @return
    ///     CVirtualChannel*    指向虚拟通道对象的指针，失败时返回NULL
    ///
    CSMVSendVirtualChannel* FirstSMVSendVirtualChannel(bool bRebuildMap = true);

    ///
    /// @brief
    ///    获得下一个被映射的SMV发送虚拟通道
    ///
    /// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
    ///
    /// @return
    ///     CVirtualChannel*    指向虚拟通道对象的指针，失败时返回NULL
    ///
    CSMVSendVirtualChannel* NextSMVSendVirtualChannel(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    获得SMV变化项
    ///
    /// @return
    ///     eSMVChangeItem    SMV变化项
    ///
    eSMVChangeItem GetSMVChangeItem();

    ///
    /// @brief
    ///    设置SMV变化项
    ///
    /// @param[in]  newSMVChangeItem    新的SMV变化项
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool SetSMVChangeItem(eSMVChangeItem newSMVChangeItem);

    ///
    /// @brief
    ///    获得修改生效方式
    ///
    /// @return
    ///     eModifyEffectiveMode    修改生效方式
    ///
    eModifyEffectiveMode GetModifyEffectiveMode();

    ///
    /// @brief
    ///    设置修改生效方式
    ///
    /// @param[in]  newModifyEffectiveMode    新的修改生效方式
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool SetModifyEffectiveMode(eModifyEffectiveMode newModifyEffectiveMode);

    ///
    /// @brief
    ///    获得试验开始方式
    ///
    /// @return
    ///     eStartMode    试验开始方式
    ///
    eStartMode GetStartMode();

    ///
    /// @brief
    ///    设置试验开始方式
    ///
    /// @param[in]  newStartMode    新的试验开始方式
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool SetStartMode(eStartMode newStartMode);

    ///
    /// @brief
    ///    等幅值
    ///
    /// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
    ///
    /// @return
    ///     bool    成功返回true，失败返回false
    ///
    bool EqualAmplitude(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    额定值
    ///
    /// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
    ///
    /// @return
    ///     bool    成功返回true，失败返回false
    ///
    bool RatedAmplitude(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    三相平衡
    ///
    /// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
    ///
    /// @return
    ///     bool    成功返回true，失败返回false
    ///
    bool ThreePhaseBalance(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    等相角
    ///
    /// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
    ///
    /// @return
    ///     bool    成功返回true，失败返回false
    ///
    bool EqualPhaseAngle(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    相角正序
    ///
    /// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
    ///
    /// @return
    ///     bool    成功返回true，失败返回false
    ///
    bool PhaseAnglePositiveSequence(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    相角反序
    ///
    /// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
    ///
    /// @return
    ///     bool    成功返回true，失败返回false
    ///
    bool PhaseAngleReverseSequence(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    等频率
    ///
    /// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
    ///
    /// @return
    ///     bool    成功返回true，失败返回false
    ///
    bool EqualFrequency(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    设置所有虚拟通道的频率为指定值
    ///
    /// @param[in]  dNewFrequency    虚拟通道频率
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool SetAllVCFrequency(double dNewFrequency);

    ///
    /// @brief
    ///    获得是否需要显示开入量
    ///
    bool GetIsShowDI() const
    {
        return m_bIsShowDI;
    }

    ///
    /// @brief
    ///    设置是否需要显示开入量
    ///
    /// @param[in]  val    是否显示
    ///
    void SetIsShowDI(bool val)
    {
        m_bIsShowDI = val;
    }

protected:

    ///
    /// @brief
    ///    添加SMV发送虚拟通道到容器中
    ///
    /// @param[in]  pVirtualChannel    SMV发送虚拟通道指针
    ///
    /// @return
    ///     bool    true表示添加成功
    ///
    bool __AddSMVSendVirtualChannel(CSMVSendVirtualChannel* pVirtualChannel);

    ///
    /// @brief
    ///     释放所有SMV发送虚拟通道对象
    ///
    void __ClearSMVSendVirtualChannel();

    ///
    /// @brief
    ///    查找映射到指定SMV发送虚拟通道的真实通道，把真实通道的指针添加虚拟通道的容器中
    ///
    /// @param[in]  pVirtualChannel    虚拟通道指针
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool __FillSMVSendChannelGroup(CSMVSendVirtualChannel* pVirtualChannel);

    ///
    /// @brief
    ///    获得指定名字的虚拟通道
    ///
    /// @param[in]  strChannelName    虚拟通道名
    ///
    /// @return
    ///     CSMVSendVirtualChannel*    指向虚拟通道对象的指针
    ///
    CSMVSendVirtualChannel* __GetSMVSendVirtualChannel(std::wstring strChannelName);

    ///
    /// @brief
    ///    设置指定名字的虚拟通道的幅值（一次值）
    ///
    /// @param[in]  strChannelName    虚拟通道名
    /// @param[in]  dNewAmplitude    新的幅值（一次值）
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool __SetSMVSendVCPrimaryAmplitude(std::wstring strChannelName, double dNewAmplitude);

    ///
    /// @brief
    ///    设置指定名字的虚拟通道的相角
    ///
    /// @param[in]  strChannelName    虚拟通道名
    /// @param[in]  dNewPhaseAngle    新的相角
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool __SetSMVSendVCPhaseAngle(std::wstring strChannelName, double dNewPhaseAngle);

    ///
    /// @brief
    ///    获得指定名字的虚拟通道的相角
    ///
    /// @param[in]  strChannelName    虚拟通道名
    ///
    /// @return
    ///     double    相角值
    ///
    double __GetSMVSendVCPhaseAngle(std::wstring strChannelName);

protected:
    eSMVChangeItem                              m_eSMVChangeItem;           ///< SMV变化项
    eModifyEffectiveMode                        m_eModifyEffectiveMode;     ///< 修改生效方式
    eStartMode                                  m_eStartMode;               ///< 试验开始方式
    bool                                        m_bIsShowDI;                ///< 是否显示开入量对话框

    std::list<CSMVSendVirtualChannel*>          m_listSMVSendVirtualChannelGroup;   ///< 虚拟通道容器
};
#endif
