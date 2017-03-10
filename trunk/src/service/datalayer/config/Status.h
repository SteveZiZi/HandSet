/// @file
///
/// @brief
///     状态序列的状态类
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.6.14
///
/// 修改历史：
///

#ifndef __STATUS_SEQUENCE_H__
#define __STATUS_SEQUENCE_H__
#include <map>
#include "SMVSendVirtualChannel.h"

class CParamConfigBase;

///
/// @brief
///    状态切换方式
///
enum eSwitchType
{
    SWITCH_TYPE_UNKNOWN = 0,
    SWITCH_TYPE_TIME,                                                   ///< 限时切换
    SWITCH_TYPE_DI,                                                     ///< 开入量切换
    SWITCH_TYPE_MANUAL                                                  ///< 手动切换
};

///
/// @brief
///    开入量切换逻辑运算方式
///
enum eLogicalOperator
{
    LOGICAL_UNKNOWN = 0,
    LOGICAL_AND,                                                        ///< 逻辑与
    LOGICAL_OR                                                          ///< 逻辑或
};

///
/// @brief
///    DI、DO状态值结构体
///
struct DI_DO_INFO
{
    std::wstring                        m_strName;                      ///< 虚拟通道名
    bool                                m_bChecked;                     ///< 是否勾选此通道
};

///
/// @brief
///    预定义的DI状态
///
static const DI_DO_INFO PREDEFINED_DI_INFO[] =
{
    {L"DI1",    false},
    {L"DI2",    false},
    {L"DI3",    false},
    {L"DI4",    false},
    {L"DI5",    false},
    {L"DI6",    false},
    {L"DI7",    false},
    {L"DI8",    false}
};

///
/// @brief
///    预定义的DO状态
///
static const DI_DO_INFO PREDEFINED_DO_INFO[] =
{
    {L"DO1",    false},
    {L"DO2",    false},
    {L"DO3",    false},
    {L"DO4",    false},
    {L"DO5",    false},
    {L"DO6",    false}
};

///
/// @brief
///    SMV发送虚拟通道值结构体
///
struct stSMVSendVirtualChannelInfo
{
    std::wstring                        m_strName;                      ///< 虚拟通道名
    double                              m_dAmplitude;                   ///< 幅值（一次值）
    double                              m_dPhaseAngle;                  ///< 相角
    double                              m_dFrequency;                   ///< 频率
};

///
/// @brief
///    预定义的SMV发送虚拟通道值
///
static const stSMVSendVirtualChannelInfo PREDEFINED_SMV_SEND_VC_INFO[] =
{
    {L"Ia1",    3000.0f,    0.0f,   50.0f},
    {L"Ib1",    3000.0f,   -120.0f, 50.0f},
    {L"Ic1",    3000.0f,    120.0f, 50.0f},
    {L"Ix1",    3000.0f,    0.0f,   50.0f},
    {L"Ia2",    3000.0f,    0.0f,   50.0f},
    {L"Ib2",    3000.0f,   -120.0f, 50.0f},
    {L"Ic2",    3000.0f,    120.0f, 50.0f},
    {L"Ix2",    3000.0f,    0.0f,   50.0f},
    {L"Ua1",    63508.0f,   0.0f,   50.0f},
    {L"Ub1",    63508.0f,  -120.0f, 50.0f},
    {L"Uc1",    63508.0f,   120.0f, 50.0f},
    {L"Ux1",    63508.0f,   0.0f,   50.0f},
    {L"Ua2",    63508.0f,   0.0f,   50.0f},
    {L"Ub2",    63508.0f,  -120.0f, 50.0f},
    {L"Uc2",    63508.0f,   120.0f, 50.0f},
    {L"Ux2",    63508.0f,   0.0f,   50.0f},
    {L"t1",     750.0f,     0.0f,   50.0f},
    {L"t2",     750.0f,     0.0f,   50.0f},
    {L"t3",     750.0f,     0.0f,   50.0f},
    {L"t4",     750.0f,     0.0f,   50.0f}
};

///
/// @brief
///    状态类
///
class CStatus
{
    friend class CParamConfigStateSequence;
    friend class CStateTestResult;
    friend class CFrameSend;

protected:
    CStatus(void);

public:
    ///
    /// @brief
    ///    析构函数
    ///
    virtual ~CStatus(void);

    ///
    /// @brief
    ///    构造函数
    ///
    /// @param[in]  nID    状态对象唯一标识ID
    /// @param[in]  pParamConfigBase    指向参数配置管理器的指针
    ///
    CStatus(int nID, CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    获得状态切换方式
    ///
    /// @return
    ///     eSwitchType    状态切换方式
    ///
    eSwitchType GetSwitchType();

    ///
    /// @brief
    ///    设置状态切换方式
    ///
    /// @param[in]  eNewType    新的状态切换方式
    ///
    void SetSwitchType(eSwitchType eNewType);

    ///
    /// @brief
    ///    获得状态切换延时
    ///
    /// @return
    ///     eSwitchType    状态切换延时
    ///
    int GetDelay();

    ///
    /// @brief
    ///    设置状态切换延时
    ///
    /// @param[in]  iDelay    新的状态切换延时
    ///
    void SetDelay(int iDelay);

    ///
    /// @brief
    ///    获得开入量切换逻辑运算方式
    ///
    /// @return
    ///     eLogicalOperator    逻辑运算方式
    ///
    eLogicalOperator GetLogicalOperator();

    ///
    /// @brief
    ///    设置开入量切换逻辑运算方式
    ///
    /// @param[in]  eNewLogical    逻辑运算方式
    ///
    void SetLogicalOperator(eLogicalOperator eNewLogical);

    ///
    /// @brief
    ///    获得指定DI通道的勾选状态
    ///
    /// @param[in]  strDIName    DI通道名
    ///
    /// @return
    ///     bool    DI通道的勾选状态
    ///
    bool IsDIChecked(std::wstring strDIName);

    ///
    /// @brief
    ///    设置指定DI通道的勾选状态
    ///
    /// @param[in]  strDIName    DI通道名
    /// @param[in]  bChecked    是否被勾选
    ///
    void SetDIChecked(std::wstring strDIName, bool bChecked);

    ///
    /// @brief
    ///    获得指定DO通道的值
    ///
    /// @param[in]  strDOName    DO通道名
    ///
    /// @return
    ///     bool    DO通道值
    ///
    bool GetDOValue(std::wstring strDOName);

    ///
    /// @brief
    ///    设置指定DO通道的值
    ///
    /// @param[in]  strDOName    DO通道名
    /// @param[in]  bNewValue    通道布尔值
    ///
    void SetDOValue(std::wstring strDOName, bool bNewValue);

    ///
    /// @brief
    ///    获得SMV发送虚拟通道的数量
    ///
    /// @return
    ///     int    虚拟通道的数量
    ///
    int GetSMVSendVirtualChannelCount();

    ///
    /// @brief
    ///    枚举第一个被映射的SMV发送虚拟通道
    ///
    /// @return
    ///     CVirtualChannel*    指向虚拟通道对象的指针，失败时返回NULL
    ///
    CSMVSendVirtualChannel* FirstSMVSendVirtualChannel();

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
    ///    使所有虚拟通道的谐波类型相同
    ///
    /// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool EqualHarmonicType(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    使同组虚拟通道的谐波幅值相等
    ///
    /// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool EqualHarmonicAmplitude(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    使同组虚拟通道的谐波相角相等
    ///
    /// @param[in]  pCurVirtualChannel    当前虚拟通道对象的指针
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool EqualHarmonicPhaseAngle(CSMVSendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    获得状态测试结果
    ///
    /// @return
    ///     CStateTestResult*    指向测试结果对象的指针
    ///
    CStateTestResult* GetStateTestResult() const;

    ///
    /// @brief
    ///    设置状态测试结果
    ///
    /// @param[in]  pNewStateTestResult    设置测试结果对象的指针
    ///
    void SetStateTestResult(CStateTestResult* pNewStateTestResult);

protected:
    ///
    /// @brief
    ///    深度拷贝一个状态对象
    ///
    /// @param[in]
    ///     pOldStatus    -   已存在的状态对象
    ///
    /// @return
    ///     bool    拷贝成功返回TRUE
    ///
    bool __Copy(CStatus* pOldStatus);

    ///
    /// @brief
    ///    获得状态对象唯一标识ID
    ///
    /// @return
    ///     int    标识ID
    ///
    int __GetID();

    ///
    /// @brief
    ///    加载<Status>
    ///
    /// @param[in]  xml    xml文件对象引用
    ///
    /// @return
    ///     bool    true表示加载成功
    ///
    bool __LoadConfig( CMarkup& xml );

    ///
    /// @brief
    ///    保存<Status>
    ///
    /// @param[in]  xml    xml文件对象引用
    ///
    /// @return
    ///     bool    true表示保存成功
    ///
    bool __SaveConfig( CMarkup& xml );

    ///
    /// @brief
    ///     释放所有SMV发送虚拟通道对象
    ///
    void __ClearSMVSendVirtualChannel();

    ///
    /// @brief
    ///    查找指定名字的SMV发送虚拟通道
    ///
    /// @param[in]  strChannelName    虚拟通道名
    ///
    /// @return
    ///     CSMVSendVirtualChannel*    指向虚拟通道对象的指针
    ///
    CSMVSendVirtualChannel* __GetSMVSendVirtualChannel(std::wstring strChannelName);

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
    ///    给状态对象添加默认的虚拟通道组
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool __AddDefaultSMVSendVirtualChannels();

    ///
    /// @brief
    ///    与指定的状态对象比较SMV虚拟发送通道是否相同
    ///
    /// @param[in]  eCompareType    比较方式
    /// @param[in]  pOldStatus      指向状态对象的指针
    ///
    /// @return
    ///     bool    相同返回true
    ///
    bool __CompareSMVSendVirtualChannels(enumCompareType eCompareType, CStatus* pOldStatus);

    ///
    /// @brief
    ///     查找状态对象中变化的DO
    ///
    /// @param[in]  pOldStatus          待比较的状态
    /// @param[out] mapChangedDoArray   保存值发生了变化的DO通道的名字和值
    ///
    /// @return
    ///     bool    有变化的DO时返回true
    ///
    bool __FindChangedDO(CStatus* pOldStatus, std::map<std::wstring, bool>& mapChangedDoArray);

    ///
    /// @brief
    ///    设置指定名字的虚拟通道的幅值
    ///
    /// @param[in]  strChannelName    虚拟通道名
    /// @param[in]  dNewAmplitude    新的幅值
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool __SetSMVSendVCAmplitude(std::wstring strChannelName, double dNewAmplitude);

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
    int                                 m_nID;                          ///< 控制块唯一标识
    CParamConfigBase*                   m_pParamConfigBase;             ///< 参数配置管理器

    eSwitchType                         m_eSwitchType;                  ///< 状态切换方式
    int                                 m_iDelay;                       ///< 状态切换延时ms

    eLogicalOperator                    m_eLogicalOperator;             ///< 开入量切换逻辑运算方式
    std::map<std::wstring, bool>        m_mapDIGroup;                   ///< 开入量容器
    std::map<std::wstring, bool>        m_mapDOGroup;                   ///< 开出量容器

    std::list<CSMVSendVirtualChannel*>  m_listSMVSendVirtualChannelGroup;///< 虚拟通道容器
    CStateTestResult*                   m_pStateTestResult;             ///< 状态测试结果
};
#endif
