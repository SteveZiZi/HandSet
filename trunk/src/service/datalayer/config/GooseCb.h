/// @file
///
/// @brief
///     GOOSE控制块
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.27
///
/// 修改历史：
///

#ifndef __GOOSE_CB_H__
#define __GOOSE_CB_H__
#include <list>
#include <string>
#include "NetCb.h"

///
/// @brief
///    GOOSE控制块类
///
class CGooseCb : public CNetCb
{
    friend class CParamConfigGOOSEBase;
    friend class CParamConfigGOOSERecv;
    friend class CParamConfigGOOSESend;
    friend class CParamConfigSMVSend;
    friend class CSCLManager;
    friend class CStateTestResult;
    friend class CFrameInfo;
    friend class CStateSendBuffer;
    friend class CFrameSendNet;

protected:
    CGooseCb(void);
    virtual ~CGooseCb(void);

public:
    ///
    /// @brief
    ///     构造函数
    ///
    /// @param[in]  
    ///     pParamConfigBase    指向参数配置管理器的指针
    ///
    CGooseCb(CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    获得TestFlag
    ///
    /// @return
    ///     bool    TestFlag
    ///
    bool GetTestFlag();

    ///
    /// @brief
    ///    设置TestFlag
    ///
    /// @param[in]  bTestFlag  新的TestFlag
    ///
    void SetTestFlag(bool bTestFlag);

    ///
    /// @brief
    ///    获得NeedComm
    ///
    /// @return
    ///     bool    NeedComm
    ///
    bool GetNeedComm();

    ///
    /// @brief
    ///    设置NeedComm
    ///
    /// @param[in]  bNeedComm  新的NeedComm
    ///
    void SetNeedComm(bool bNeedComm);

    ///
    /// @brief
    ///    获得gocbRef
    ///
    /// @return
    ///     wstring    gocbRef
    ///
    std::wstring GetGocbRef();

    ///
    /// @brief
    ///    设置gocbRef
    ///
    /// @param[in]  strGocbRef  gocbRef
    ///
    void SetGocbRef(std::wstring strGocbRef);

    ///
    /// @brief
    ///    获得数据集
    ///
    /// @return
    ///     wstring    数据集
    ///
    std::wstring GetDataSet();

    ///
    /// @brief
    ///    设置数据集
    ///
    /// @param[in]  strDataSet  数据集
    ///
    void SetDataSet(std::wstring strDataSet);

    ///
    /// @brief
    ///    获得goID
    ///
    /// @return
    ///     wstring    goID
    ///
    std::wstring GetGoID();

    ///
    /// @brief
    ///    设置goID
    ///
    /// @param[in]  strGoID  goID
    ///
    void SetGoID(std::wstring strGoID);

    ///
    /// @brief
    ///    获得加密字符串
    ///
    /// @return
    ///     std::wstring    加密字符串
    ///
    std::wstring GetSecurity();

    ///
    /// @brief
    ///    设置加密字符串
    ///
    /// @param[in]  strNewSecurity    新的加密字符串
    ///
    void SetSecurity(std::wstring strNewSecurity);

    ///
    /// @brief
    ///    获得状态序号
    ///
    /// @return
    ///     int    状态序号
    ///
    int GetStNum();

    ///
    /// @brief
    ///    设置状态序号
    ///
    /// @param[in]  iNewStNum    新的状态序号
    ///
    void SetStNum(int iNewStNum);

    ///
    /// @brief
    ///    控制块中通道的发送值是否发生了改变
    ///
    /// @return
    ///     bool    发送值是否被改变
    ///
    bool IsSendValueChanged();

    ///
    /// @brief
    ///    设置控制块中通道的发送值是否发生了改变
    ///
    /// @param[in]  bChanged    发送值是否被改变
    ///
    void SetSendValueChanged(bool bIsChanged);

    ///
    /// @brief
    ///    添加Channel
    ///
    /// @return
    ///     CChannel*    新添加的Channel对象的指针
    ///
    virtual CChannel* AddChannel();

    ///
    /// @brief
    ///    DO(s, q, t)
    ///
    /// @return
    ///     bool    成功返回true，失败返回false
    ///
    bool DO_s_q_t();

    ///
    /// @brief
    ///    DA(s, q, t)
    ///
    /// @return
    ///     bool    成功返回true，失败返回false
    ///
    bool DA_s_q_t();

    ///
    /// @brief
    ///    DA(s, q)
    ///
    /// @return
    ///     bool    成功返回true，失败返回false
    ///
    bool DA_s_q();

    ///
    /// @brief
    ///    DA(s)
    ///
    /// @return
    ///     bool    成功返回true，失败返回false
    ///
    bool DA_SinglePoint();

    ///
    /// @brief
    ///    DA(双点)
    ///
    /// @return
    ///     bool    成功返回true，失败返回false
    ///
    bool DA_DoublePoint();

    ///
    /// @brief
    ///    获得控制块所有通道值在通信报文中的长度
    ///
    /// @return
    ///     int    通道报文长度
    ///
    int GetChannelValueLen();

    ///
    /// @brief
    ///    计算控制块中所有通道的偏移量
    ///
    /// @param[in]  pGooseMsg    GOOSE报文的编解码接口
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool CalcChannelOffset(GooseMsg* pGooseMsg);

protected:
    ///
    /// @brief
    ///    加载<Gocb>控制块
    ///
    /// @param[in]  pXml 指向XML对象的引用
    ///
    /// @return
    ///     bool    true表示加载成功
    ///
    bool __LoadConfig(CMarkup& xml);

    ///
    /// @brief
    ///    保存<Gocb>控制块
    ///
    /// @param[in]  pXml 指向XML对象的引用
    ///
    /// @return
    ///     bool    true表示保存成功
    ///
    bool __SaveConfig(CMarkup& xml);

    ///
    /// @brief
    ///    深度拷贝一个GOOSE控制块
    ///
    /// @param[in]
    ///     pGocb    -   GOOSE控制块指针
    ///
    /// @return
    ///     bool    拷贝成功返回TRUE
    ///
    virtual bool __Copy(CGooseCb* pGocb);

    ///
    /// @brief
    ///    深度拷贝一个扫描列表中的GOOSE控制块
    ///
    /// @param[in]
    ///     pGooseMsg    -   GOOSE报文的编解码对象
    ///
    /// @return
    ///     bool    拷贝成功返回TRUE
    ///
    virtual bool __Copy(GooseMsg* pGooseMsg);

    ///
    /// @brief
    ///    把当前控制块与指定控制块相比较，判断通道发送值是否相等
    ///
    /// @param[in]  pOldGocb    指向GOOSE控制块对象的指针
    ///
    /// @return
    ///     bool    相等返回true，否则返回false
    ///
    bool __CompareChannelSendValue(CGooseCb* pOldGocb);

    ///
    /// @brief
    ///    修改GOOSE控制块中单点/双点类型通道时，需要自动修改相邻/间隔的时间通道的值
    ///    此函数自动寻找寻找变位的单点/双点通道，然后改变相邻或间隔的时间通道的值
    ///
    /// @param[in]  stNewTime    单点/双点通道的变位时间
    ///
    void __UpdateChannelTime(CEventTime stNewTime);

    ///
    /// @brief
    ///     对发生了变位的控制块通道进行重新编码
    ///
    /// @param[in]  pucFrameHead    指向发送帧帧头的指针
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool __RecodeChangedChannels(unsigned char* pucFrameHead);

    ///
    /// @brief
    ///     查找GOOSE控制块中所有需要变位的时间通道，将通道指针保存到容器中
    ///
    /// @param[out]  listShouldChangeTimeChannel    保存需要变位时间通道的指针
    ///
    void __SaveTimeChannel(std::list<CChannel*>& listShouldChangeTimeChannel);

private:
    bool                                m_bTestFlag;                    ///< testFlag
    bool                                m_bNeedComm;                    ///< NeedComm
    std::wstring                        m_strGocbRef;                   ///< gocbRef
    std::wstring                        m_strDataSet;                   ///< 数据集
    std::wstring                        m_strGoID;                      ///< goID
    std::wstring                        m_strSecurity;                  ///< 加密字符串串（可选）

    int                                 m_stNum;                        ///< 状态序号，用于记录数据总共的变位次数
    bool                                m_bIsSendValueChanged;          ///< 控制块中是否有通道发送值被改变

    GOOSE_PDU_PUBLIC_OFFSET             m_stPduPblcOff;                 ///< 公共信息在报文帧中的偏移地址
};
#endif
