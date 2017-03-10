///
/// @brief
///     保存状态测试结果的类
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.7.1
///
/// 修改历史：
///

#ifndef __STATE_TEST_RESULT_H__
#define __STATE_TEST_RESULT_H__
#include <map>
#include "ChannelRecvValues.h"
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\config\SMVSendVirtualChannel.h"
#include "src\service\datalayer\config\Status.h"

///
/// @brief
///    测试的类型
///
enum enumTestType
{
    TEST_TYPE_UNKNOWN = 0,                                              ///< 未知类型
    TEST_TYPE_SMV,                                                      ///< 手动试验SMV测试的结果集
    TEST_TYPE_GOOSE,                                                    ///< 手动试验GOOSE测试的结果集
    TEST_TYPE_STATE_SEQUENCE                                            ///< 状态序列试验的结果集
};

///
/// @brief
///    发送帧信息
///
class CFrameInfo
{
public:
    ///
    /// @brief
    ///     查找GOOSE控制块中所有需要变位的时间通道，将通道指针保存到容器中
    ///
    void __SaveTimeChannel();

    ///
    /// @brief
    ///    更新发送帧中需要变位的时间通道的值
    ///
    /// @param[in]  pucFrameHead    指向发送帧帧头的指针
    /// @param[in]  stNewTime       新的变位时间
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool __UpdateChannelTime(unsigned char* pucFrameHead, CEventTime stNewTime);

public:
    CGooseCb*                           m_pGooseCb;                     ///< 发送帧对应的GOOSE控制块
    unsigned int                        m_uiFrameOffset;                ///< 帧偏移
    unsigned int                        m_uiFrameLen;                   ///< 帧长度
    std::list<CChannel*>                m_listTimeChannel;              ///< 控制块中需要变位的时间通道
};

///
/// @brief
///    保存端口发送帧的类
///
class CStateSendBuffer
{
public:
    ///
    /// @brief
    ///    构造函数
    ///
    /// @param[in]  ePortName    发送端口
    ///
    CStateSendBuffer(DsmPortName ePortName);

    ///
    /// @brief
    ///    析构函数
    ///
    ~CStateSendBuffer();

    ///
    /// @brief
    ///    申请数据帧缓冲区
    ///
    /// @param[in]  uiBufferSize    缓冲区字节数
    ///
    /// @return
    ///     unsigned char*    指向缓冲区的指针
    ///
    unsigned char* AllocDataBuffer(uint32_t uiBufferSize);

    ///
    /// @brief
    ///    释放数据帧缓冲区
    ///
    void FreeDataBuffer();

    ///
    /// @brief
    ///    修改所有发送帧的变位时间字节
    ///
    /// @param[in]  stStateStartTime    新的变位时间
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool UpdateEventTime(CEventTime stStateStartTime);

    ///
    /// @brief
    ///    修改所有发送帧中变位时间通道的值
    ///
    /// @param[in]  stStateStartTime    新的变位时间
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool UpdateChannelTime(CEventTime stStateStartTime);

    ///
    /// @brief
    ///     对发生了变位的GOOSE控制块进行重新编码
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool RecodeChangedGooseCb();

    ///
    /// @brief
    ///     拷贝发送缓冲
    ///
    /// @param[in]  pOldStateSendBuffer     老的发送缓冲
    ///
    /// @return
    ///     bool    拷贝成功返回true
    ///
    bool Copy(CStateSendBuffer* pOldStateSendBuffer);

    ///
    /// @brief
    ///     查找GOOSE控制块中需要变位的时间通道
    ///
    void __SaveGooseCbTimeChannel();

public:
    DsmPortName                         m_ePortName;                    ///< 发送端口
    IOCTRL_IN_PARAM_NET_BIG             m_stIOCtrlParam;                ///< 待发送参数配置
    unsigned char*                      m_pucDataBuffer;                ///< 待发送的数据帧
    unsigned int                        m_uiDataBufferLen;              ///< 待发送数据帧的长度
    std::list<CFrameInfo>               m_listFrameInfo;                ///< 缓冲区中GOOSE帧的信息
};

///
/// @brief
///    保存状态测试结果的类
///
class CStateTestResult
{
    friend class CFrameSend;
    friend class CFrameSendFt3;
    friend class CFrameSendNet;

public:
    CStateTestResult(enumTestType eType);
    ~CStateTestResult(void);

    ///
    /// @brief
    ///    初始化
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool Init();

    ///
    /// @brief
    ///    获得测试的类型
    ///
    /// @return
    ///     enumTestType    测试的类型
    ///
    enumTestType GetTestType();

    ///
    /// @brief
    ///    释放所有记录通道历史值的对象
    ///
    void ClearAllRecvValues();

    ///
    /// @brief
    ///    获得指定虚拟通道名的保存通道接收值的对象
    ///
    /// @param[in]  strName    虚拟通道名
    ///
    /// @return
    ///     CChannelRecvValues*    保存通道接收值得对象
    ///
    CChannelRecvValues* GetChannelRecvValues(std::wstring strName);

    ///
    /// @brief
    ///    获得状态开始测试的时间
    ///
    /// @return
    ///     std::wstring    开始测试的时间
    ///
    std::wstring GetStartTime();

    ///
    /// @brief
    ///    设置状态开始时间（相对时间）
    ///
    /// @param[in]  stNewTime    状态开始时间
    ///
    void SetStartTime(CEventTime stNewTime);

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
    ///    枚举第一个GOOSE发送控制块
    ///
    /// @return
    ///     CGooseCb*    成功时返回GOOSE控制块指针，失败时返回NULL
    ///
    CGooseCb* FirstGooseSendCb();

    ///
    /// @brief
    ///    枚举下一个GOOSE发送控制块
    ///
    /// @param[in]  pCurGocb 当前位置GOOSE控制块的指针
    ///
    /// @return
    ///     CGooseCb*    成功时返回GOOSE控制块指针，失败时返回NULL
    ///
    CGooseCb* NextGooseSendCb(CGooseCb* pCurGocb);

    ///
    /// @brief
    ///    获得指定GOOSE控制块的索引号
    ///
    /// @param[in]  pCurGocb    指向控制块对象的指针
    ///
    /// @return
    ///     int    控制块索引，失败返回-1
    ///
    int GetIndexByGooseCb(CGooseCb* pCurGocb);

    ///
    /// @brief
    ///    根据索引获得GOOSE控制块
    ///
    /// @param[in]  nCurIndex    控制块索引
    ///
    /// @return
    ///     CGooseCb*    成功时返回GOOSE控制块指针，失败时返回NULL
    ///
    CGooseCb* GetGooseCbByIndex(int nCurIndex);

    ///
    /// @brief
    ///    清除手动试验各GOOSE控制块
    ///
    void ClearGooseSendCbs();

    ///
    /// @brief
    ///    查询SMV通道值是否发生变化
    ///
    /// @return
    ///     bool    true表示发生了变化
    ///
    bool IsSmvChannelChanged() const;

    ///
    /// @brief
    ///    设置SMV通道值是否发生了变化
    ///
    /// @param[in]  val    是否发生了变化
    ///
    void SetSmvChannelChanged(bool val);

    ///
    /// @brief
    ///    查询GOOSE通道值是否发生变化
    ///
    /// @return
    ///     bool    true表示发生了变化
    ///
    bool IsGooseChannelChanged() const;

    ///
    /// @brief
    ///    设置GOOSE通道值是否发生了变化
    ///
    /// @param[in]  val    是否发生了变化
    ///
    void SetGooseChannelChanged(bool val);

    ///
    /// @brief
    ///    查询9-1、FT3、FT3扩展控制块的状态字是否改变
    ///
    /// @return
    ///     bool    true表示发生了变化
    ///
    bool IsStatusWordChanged() const;

    ///
    /// @brief
    ///    设置9-1、FT3、FT3扩展控制块的状态字是否发生了改变
    ///
    /// @param[in]  val    是否发生了改变
    ///
    void SetStatusWordChanged(bool val);

    ///
    /// @brief
    ///    查询9-2通道的品质是否改变
    ///
    /// @return
    ///     bool    true表示发生了变化
    ///
    bool IsQualityChanged() const;

    ///
    /// @brief
    ///    设置9-2通道的品质是否发生了改变
    ///
    /// @param[in]  val    是否发生了改变
    ///
    void SetQualityChanged(bool val);

    ///
    /// @brief
    ///    清除SMV发送帧缓冲区
    ///
    void ClearSmvSendBuffers();

    ///
    /// @brief
    ///    清除GOOSE发送帧缓冲区
    ///
    void ClearGooseSendBuffers();

    ///
    /// @brief
    ///    查询是否完成了测试
    ///
    /// @return
    ///     bool    完成测试返回true
    ///
    bool IsTested() const;

    ///
    /// @brief
    ///     设置测试完成标识
    ///
    /// @param[in]  val     是否完成测试
    ///
    void SetTested(bool val);

protected:
    ///
    /// @brief
    ///    保存手动试验各SMV发送虚拟通道的值
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool __SaveManualTestSMVSendValues();

    ///
    /// @brief
    ///    保存各GOOSE发送控制块的值
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool __SaveGooseSendCbs();

    ///
    /// @brief
    ///    获得手动试验SMV各虚拟通道的值
    ///
    /// @return
    ///     CStatus*    保存虚拟通道值的对象
    ///
    CStatus* __GetSMVSendValues();

    ///
    /// @brief
    ///    把手动试验SMV发送的虚拟通道值与指定的状态结果对象相比
    ///
    /// @param[in]  eCompareType    比较方式
    /// @param[in]  pOldStatusTestResult    指向状态结果对象的指针
    ///
    /// @return
    ///     bool    相同返回true
    ///
    bool __CompareSMVSendValues(enumCompareType eCompareType, CStateTestResult* pOldStatusTestResult);

    ///
    /// @brief
    ///    把手动试验GOOSE发送的控制块与指定的状态结果对象相比
    ///
    /// @param[in]  pOldStatusTestResult    指向状态结果对象的指针
    ///
    /// @return
    ///     bool    相同返回true
    ///
    bool __CompareGooseSendValues(CStateTestResult* pOldStateTestResult);

    ///
    /// @brief
    ///    修改GOOSE控制块中单点/双点类型通道时，需要自动修改相邻/间隔的时间通道的值
    ///    此函数自动寻找寻找变位的单点/双点通道，然后改变相邻或间隔的时间通道的值
    ///
    /// @param[in]  stTestStartTime    整个实验的开始时间
    ///
    void __UpdateGooseChannelTime(CEventTime stTestStartTime);

    ///
    /// @brief
    ///     查找GOOSE控制块中需要变位的时间通道
    ///
    void __SaveGooseCbTimeChannel();

    ///
    /// @brief
    ///     拷贝SMV发送缓冲
    ///
    /// @param[in]  pOldStateTestResult     老的状态测试结果集
    ///
    /// @return
    ///     bool    拷贝成功返回true
    ///
    bool __CopySmvSendBuffers(CStateTestResult* pOldStateTestResult);

    ///
    /// @brief
    ///     拷贝Goose发送缓冲
    ///
    /// @param[in]  pOldStateTestResult     老的状态测试结果集
    ///
    /// @return
    ///     bool    拷贝成功返回true
    ///
    bool __CopyGooseSendBuffers(CStateTestResult* pOldStateTestResult);

private:
    enumTestType                        m_eTestType;                    ///< 测试的类型

    bool                                m_bTested;                      ///< 是否完成了测试
    bool                                m_bSmvChannelChanged;           ///< SMV通道值是否发生变化
    bool                                m_bGooseChannelChanged;         ///< GOOSE通道值是否发生变化
    bool                                m_bStatusWordChanged;           ///< 9-1、FT3、FT3扩展控制块的状态字是否改变
    bool                                m_bQualityChanged;              ///< 9-2通道的品质是否改变

    CEventTime                          m_stStateStartTime;             ///< 当前状态开始的时间（相对时间）
    CStatus                             m_stSMVSendValues;              ///< 手动试验SMV各虚拟通道的发送值
    std::list<CGooseCb *>               m_listGooseSendCbs;             ///< 手动试验GOOSE发送的各个控制块

    std::list<CStateSendBuffer*>        m_listSmvSendBuffers;           ///< 保存SMV待发送的数据帧
    std::list<CStateSendBuffer*>        m_listGooseSendBuffers;         ///< 保存GOOSE待发送的数据帧

    std::map<std::wstring, CChannelRecvValues*> m_mapChannelRecvValues; ///< 存放各DI通道接收值的容器
};
#endif
