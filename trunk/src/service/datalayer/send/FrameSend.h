/// @file
///
/// @brief
///     管理与FPGADE交互，完成数据发送操作
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.3.19
///
/// 修改历史：
///     Modified by:    chao
///     Modified date:  2013.6.19
///     Version:        1.0.1
///     Descriptions:   添加了发送功能
///

#ifndef __FRAME_SEND_H__
#define __FRAME_SEND_H__
#include <afxmt.h>
#include "FrameSendFt3.h"
#include "FrameSendNet.h"
#include "StateTestResult.h"
#include "HardwareTimer.h"
#include "src\service\datalayer\config\Status.h"

class CFrameSend
{
    friend class CFrameSendFt3;
    friend class CFrameSendNet;

protected:
    CFrameSend();
    ~CFrameSend(void);

public:
    ///
    /// @brief
    ///    获取发送管理器实例
    ///
    static CFrameSend* getInstance();

    ///
    /// @brief
    ///    初始化发送管理器
    ///
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool InitFrameSend(HANDLE hGPMDevice);

    ///
    /// @brief
    ///    卸载发送管理器
    ///
    void UnInitFrameSend();

    ///
    /// @brief
    ///    是否已初始化
    ///
    ///
    /// @return
    ///     bool    是否已初始化
    ///
    bool IsInited();

    ///
    /// @brief
    ///     获得GPMC总线句柄
    ///
    /// @return
    ///     HANDLE    GPMC总线句柄
    ///
    HANDLE GetGPMDevice();

    ///
    /// @brief
    ///    获得通知窗口
    ///
    ///
    /// @return
    ///     HWND    窗口句柄
    ///
    HWND GetNotifyHwnd();

    ///
    /// @brief
    ///    获得SMV发送的状态
    ///
    ///
    /// @return
    ///     eSendStatus    SMV发送状态
    ///
    eSendStatus GetManualTestSMVSendStatus();

    ///
    /// @brief
    ///    获得GOOSE的发送状态
    ///
    ///
    /// @return
    ///     eSendStatus    GOOSE的发送状态
    ///
    eSendStatus GetManualTestGooseSendStatus();

    ///
    /// @brief
    ///    获得当前正在进行的事务类型（手动试验、状态序列）
    ///
    /// @return
    ///     eFunctionType    当前正在进行的事务类型
    ///
    eFunctionType GetCurFunctionType();

    ///
    /// @brief
    ///    开始手动试验SMV测试
    ///
    /// @param[in]  hWnd    hWnd  窗口句柄，当DI值发生变化时，会发送WM_DI_CHANGED消息给此窗口
    ///
    /// @return
    ///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
    ///
    enumErrorCode StartManualTestSMV(HWND hWnd);

    ///
    /// @brief
    ///    更新手动试验SMV测试的数据
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool UpateManualTestSMV();

    ///
    /// @brief
    ///    停止手动试验SMV测试
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool StopManualTestSMV();

    ///
    /// @brief
    ///    开始手动试验GOOSE测试
    ///
    /// @param[in]  hWnd    hWnd  窗口句柄，当DI值发生变化时，会发送WM_DI_CHANGED消息给此窗口
    ///
    /// @return
    ///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
    ///
    enumErrorCode StartManualTestGoose(HWND hWnd);

    ///
    /// @brief
    ///    更新手动试验GOOSE测试的数据
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool UpateManualTestGoose();

    ///
    /// @brief
    ///    停止手动试验GOOSE测试
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool StopManualTestGoose();

    ///
    /// @brief
    ///    获得手动试验结果集数量
    ///
    /// @return
    ///     unsigned int    手动试验结果集数量
    ///
    unsigned int GetManualTestResultCount();

    ///
    /// @brief
    ///    获得指定索引号的手动试验结果集对象
    ///
    /// @param[in]  nIndex    结果集索引号(从0开始计数)
    ///
    /// @return
    ///     CStateTestResult*    存放指定状态测试结果的对象，失败时返回NULL
    ///
    CStateTestResult* GetManualTestResult(unsigned int nIndex);

    ///
    /// @brief
    ///    获得指定手动试验结果集对象的索引号
    ///
    /// @param[in]  pCurResult    手动试验结果集对象
    ///
    /// @return
    ///     int    结果集索引号，从0开始计数，失败时返回-1
    ///
    int GetManualTestResultIndex(CStateTestResult* pCurResult);

    ///
    /// @brief
    ///    获得第一个手动试验结果集对象
    ///
    /// @return
    ///     CStateTestResult*    指向结果集对象的指针，失败时返回NULL
    ///
    CStateTestResult* FirstManualTestResult();

    ///
    /// @brief
    ///    获得下一个手动试验结果集对象
    ///
    /// @param[in]  pCurTestResult    指向当前结果集对象的指针
    ///
    /// @return
    ///     CStateTestResult*    指向下一个结果集对象的指针，失败时返回NULL
    ///
    CStateTestResult* NextManualTestResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    获得上一个手动试验结果集对象
    ///
    /// @param[in]  pCurTestResult    指向当前结果集对象的指针
    ///
    /// @return
    ///     CStateTestResult*    指向上一个结果集对象的指针，失败时返回NULL
    ///
    CStateTestResult* PrevManualTestResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    获得最后一个手动试验结果集对象
    ///
    /// @return
    ///     CStateTestResult*    指向结果集对象的指针，失败时返回NULL
    ///
    CStateTestResult* LastManualTestResult();

    ///
    /// @brief
    ///    获得第一个含有SMV虚拟通道信息的手动试验结果集对象
    ///
    /// @return
    ///     CStateTestResult*    指向结果集对象的指针，失败时返回NULL
    ///
    CStateTestResult* FirstManualTestSMVResult();

    ///
    /// @brief
    ///    获得下一个含有SMV虚拟通道信息的手动试验结果集对象
    ///
    /// @param[in]  pCurTestResult    指向当前结果集对象的指针
    ///
    /// @return
    ///     CStateTestResult*    指向下一个结果集对象的指针，失败时返回NULL
    ///
    CStateTestResult* NextManualTestSMVResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    获得上一个含有SMV虚拟通道信息的手动试验结果集对象
    ///
    /// @param[in]  pCurTestResult    指向当前结果集对象的指针
    ///
    /// @return
    ///     CStateTestResult*    指向上一个结果集对象的指针，失败时返回NULL
    ///
    CStateTestResult* PrevManualTestSMVResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    获得最后一个含有SMV虚拟通道信息的手动试验结果集对象
    ///
    /// @return
    ///     CStateTestResult*    指向结果集对象的指针，失败时返回NULL
    ///
    CStateTestResult* LastManualTestSMVResult();

    ///
    /// @brief
    ///    获得第一个含有GOOSE控制块的手动试验结果集对象
    ///
    /// @return
    ///     CStateTestResult*    指向结果集对象的指针，失败时返回NULL
    ///
    CStateTestResult* FirstManualTestGooseResult();

    ///
    /// @brief
    ///    获得下一个含有GOOSE控制块的手动试验结果集对象
    ///
    /// @param[in]  pCurTestResult    指向当前结果集对象的指针
    ///
    /// @return
    ///     CStateTestResult*    指向下一个结果集对象的指针，失败时返回NULL
    ///
    CStateTestResult* NextManualTestGooseResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    获得上一个含有GOOSE控制块的手动试验结果集对象
    ///
    /// @param[in]  pCurTestResult    指向当前结果集对象的指针
    ///
    /// @return
    ///     CStateTestResult*    指向上一个结果集对象的指针，失败时返回NULL
    ///
    CStateTestResult* PrevManualTestGooseResult(CStateTestResult* pCurTestResult);

    ///
    /// @brief
    ///    获得最后一个含有GOOSE控制块的手动试验结果集对象
    ///
    /// @return
    ///     CStateTestResult*    指向结果集对象的指针，失败时返回NULL
    ///
    CStateTestResult* LastManualTestGooseResult();

    ///
    /// @brief
    ///    清除所有的手动试验结果集
    ///
    void ClearAllManualTestResult();

    ///
    /// @brief
    ///    开始状态序列测试
    ///
    /// @param[in]  hWnd  窗口句柄，当DI值发生变化时，会发送WM_DI_CHANGED消息给此窗口
    ///                             当需要询问用户是否手动切换时，会发送WM_WHETHER_SWITCH消息给此窗口
    ///                             当状态切换后，会发送WM_STATUS_CHANGED消息给此窗口
    ///                             当测试完成后，会发送WM_TEST_COMPLETED消息给此窗口
    ///
    /// @return
    ///     enumErrorCode   创建发送线程成功返回ERROR_CODE_SUCCESS
    ///
    enumErrorCode StartStateSequenceTest(HWND hWnd);

    ///
    /// @brief
    ///    停止状态序列测试
    ///
    bool StopStateSequenceTest();

    ///
    /// @brief
    ///    获得当前正在发送第几个状态（从0开始计数）
    ///
    int GetCurStateIndex();

    ///
    /// @brief
    ///    获得状态序列结果集数量
    ///
    /// @return
    ///     unsigned int    状态序列结果集数量
    ///
    unsigned int GetStateSequenceResultCount();

    ///
    /// @brief
    ///    获得指定状态号的状态序列结果集对象
    ///
    /// @param[in]  iStateIndex    状态号(从0开始计数)
    ///
    /// @return
    ///     CStateTestResult*    存放指定状态测试结果的对象，失败时返回NULL
    ///
    CStateTestResult* GetStateSequenceResult(unsigned int iStateIndex);

    ///
    /// @brief
    ///    清除所有的状态序列结果集
    ///
    void ClearAllStateSequenceResult();

    ///
    /// @brief
    ///     获得手动实验的开始时间
    ///
    /// @return
    ///     CEventTime    开始时间
    ///
    CEventTime GetManualTestStartTime();

    ///
    /// @brief
    ///     获得状态序列测试的开始时间
    ///
    /// @return
    ///     CEventTime    开始时间
    ///
    CEventTime GetStateSequenceStartTime();

    ///
    /// @brief
    ///    获得当前的FPGA时间
    ///
    /// @param[out]  stCurFpgaTime    存放FPGA时间的对象
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool GetCurrentFpgaTime(CEventTime& stCurFpgaTime);

    ///
    /// @brief
    ///    添加一条日志记录
    ///
    /// @param[in]  strText    函数名
    /// @param[in]  ulTime    时间值
    ///
    void AddLogs(std::string strText, unsigned long ulTime);

    ///
    /// @brief
    ///    刷新缓冲区的日志记录到日志文件中
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool FlushLogs();

protected:
    ///
    /// @brief
    ///    设置手动试验SMV的发送状态
    ///
    /// @param[in]  eNewStatus    新的SMV发送状态
    ///
    void __SetManualTestSMVSendingStatus(eSendStatus eNewStatus);

    ///
    /// @brief
    ///    设置手动试验Goose的发送状态
    ///
    /// @param[in]  eNewStatus    新的GOOSE发送状态
    ///
    void __SetManualTestGooseSendStatus(eSendStatus eNewStatus);

    ///
    /// @brief
    ///    设置当前的正在进行的事务类型
    ///
    /// @param[in]  eNewType    新的事务类型
    ///
    void __SetCurFunctionType(eFunctionType eNewType);

    ///
    /// @brief
    ///    生成被映射虚拟通道一个完整波形的采样值
    ///
    /// @param[in]  pStatus         指向一个状态对象的指针
    /// @param[out] stDspRecvData   存放计算出的采样值的结构体
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool __GenVirtualChannelSamples(CStatus* pStatus, CDspRecvData& stDspRecvData);

    ///
    /// @brief
    ///    获得SMV采样频率
    ///
    /// @return
    ///     int    采样频率
    ///
    int __GetSampleFrequency();

    ///
    /// @brief
    ///    设置当前正在发送的第几个状态(从0开始计数)
    ///
    /// @param[in]  iNewStateIndex    
    ///
    void __SetCurStateIndex(int iNewStateIndex);

    ///
    /// @brief
    ///    添加一个手动试验测试结果集对象到容器中
    ///
    /// @param[in]  eType    测试的类型
    ///
    /// @return
    ///     CStateTestResult*    CStateTestResult*    成功时返回状态测试结果对象的指针，失败时返回NULL
    ///
    CStateTestResult* __AddManualTestResult(enumTestType eType);

    ///
    /// @brief
    ///    删除指定的手动试验测试结果对象
    ///
    /// @param[in]  pDeleteTestResult    待删除的手动试验测试结果对象
    ///
    void __DeleteManualTestResult(CStateTestResult* pDeleteTestResult);

    ///
    /// @brief
    ///    添加一个状态序列测试结果集对象到容器中
    ///
    /// @return
    ///     CStateTestResult*    成功时返回状态测试结果对象的指针，失败时返回NULL
    ///
    CStateTestResult* __AddStateSequenceResult();

    ///
    /// @brief
    ///    分配SMV控制块到各个发送端口
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool __DistributeSmvCbs();

    ///
    /// @brief
    ///    分配GOOSE控制块到各个发送端口
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool __DistributeGooseCbs();

    ///
    /// @brief
    ///    分配GOOSE控制块到各个发送端口
    ///
    /// @param[in]  pStateTestResult    指向结果集对象的指针，它保存了待发送的GOOSE控制块
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool __DistributeGooseCbs(CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///    准备SMV发送帧
    ///
    /// @param[in]  pStateTestResult    指向结果集对象的指针，用于保存SMV发送帧
    ///
    /// @return
    ///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
    ///
    enumErrorCode __PrepareSmvSendBuffer(CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///    发送SMV控制块
    ///
    /// @param[in]  stFunctionType  功能类型（手动试验、状态序列）
    /// @param[in]  pStateTestResult    指向结果集对象的指针
    ///
    /// @return
    ///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
    ///
    enumErrorCode __SendSMVCbs(eFunctionType stFunctionType, CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///    更新SMV发送帧
    ///
    /// @param[in]  stFunctionType  功能类型(手动试验、状态序列)
    /// @param[in]  pStateTestResult    指向结果集对象的指针
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool __UpdateSMVSend(eFunctionType stFunctionType, CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///    停止SMV发送
    ///
    bool __StopSMVSend();

    ///
    /// @brief
    ///    准备GOOSE控制块发送帧
    ///
    /// @param[in]  pStateTestResult    指向结果集对象的指针，用于保存GOOSE发送帧
    ///
    /// @return
    ///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
    ///
    enumErrorCode __PrepareGooseSendBuffer(CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///    准备GOOSE控制块更新帧
    ///
    /// @param[in]  pStateTestResult    指向结果集对象的指针，用于保存GOOSE发送帧
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool __PrepareGooseUpdateBuffer(CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///    发送GOOSE控制块
    ///
    /// @param[in]  stFunctionType      功能类型（手动试验、状态序列）
    /// @param[in]  pStateTestResult    保存状态测试结果的对象，内部保存有待发送的GOOSE控制块
    ///
    /// @return
    ///     enumErrorCode   成功返回ERROR_CODE_SUCCESS
    ///
    enumErrorCode __SendGooseCbs(eFunctionType stFunctionType, CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///    更新GOOSE发送帧
    ///
    /// @param[in]  stFunctionType  功能类型（手动试验、状态序列）
    /// @param[in]  pStateTestResult    保存状态测试结果的对象，内部保存有待发送的GOOSE控制块
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool __UpdateGooseSend(eFunctionType stFunctionType, CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///    停止GOOSE发送
    ///
    bool __StopGooseSend();

    ///
    /// @brief
    ///    计算控制块中所有通道的偏移量
    ///
    /// @param[in]  pStateTestResult    指向结果集对象的指针
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool __CalcChannelOffset(CStateTestResult* pStateTestResult);

    ///
    /// @brief
    ///     查找相似的SMV发送状态，并返回该状态对应的发送缓冲
    ///
    /// @param[in]  pCurStateTestResult 当前SMV发送状态对应的测试结果集
    /// @param[in]  iStateIndex         当前SMV发送状态的索引号
    ///
    /// @return
    ///     CStateTestResult*   相似发送状态的测试结果集，失败时返回NULL
    ///
    CStateTestResult* __FindSimilarSmvTestResult(CStateTestResult* pCurStateTestResult, int iStateIndex);

private:
    HANDLE                              m_hGPMDevice;                   ///< GPMC总线句柄
    bool                                m_bIsInited;                    ///< 是否已经初始化
    CHardwareTimer                      m_objHardwareTimer;             ///< 硬件定时器

    CFrameSendFt3*                      m_pFrameFt3C;                   ///< 光串口C，发送报文
    CFrameSendNet*                      m_pFrameNetA;                   ///< 光以太网A
    CFrameSendNet*                      m_pFrameNetB;                   ///< 光以太网B
    CFrameSendNet*                      m_pFrameNetC;                   ///< 电以太网

    eSendStatus                         m_eManualTestSMVSendStatus;     ///< 手动试验SMV发送状态
    eSendStatus                         m_eManualTestGooseSendStatus;   ///< 手动试验GOOSE发送状态

    int                                 m_iCurStateIndex;               ///< 当前正在发送的第几个状态（每一次修改手动试验的值，也相当于产生了一个新状态）
    std::vector<CStateTestResult*>      m_vecManualTestResult;          ///< 存放手动试验GOOSE接收值的容器
    std::vector<CStateTestResult*>      m_vecStateSequenceResult;       ///< 存放状态序列GOOSE接收值的容器

    eFunctionType                       m_eCurFunctionType;             ///< 当前正在进行的事务类型（手动试验、状态序列）
    CCriticalSection                    m_criticalFunctionType;         ///< 事务类型临界区，用于互斥对事务类型的读写

    CEventTime                          m_stManualTestStartTime;        ///< 手动试验开始时间
    CEventTime                          m_stStateSequenceStartTime;     ///< 状态序列开始时间

    CEvent                              m_eventCancelSMV;               ///< 用于指示是否临时取消SMV发送组帧
    CEvent                              m_eventCancelGoose;             ///< 用于指示是否临时取消GOOSE发送组帧
    CEvent                              m_eventCancelStateSequence;     ///< 用于指示是否临时取消状态序列试验

    HWND                                m_hNotifyHwnd;                  ///< 窗口句柄，当DI变化时发送WM_STATUS_CHANGED消息给此窗口
    std::vector<std::pair<std::string, unsigned long> >  m_listTextTime;
};
#endif
