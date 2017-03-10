///
/// @brief
///     状态序列参数配置
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.6.14
///
/// 修改历史：
///

#ifndef __PARAM_CONFIG_STATE_SEQUENCE_H__
#define __PARAM_CONFIG_STATE_SEQUENCE_H__
#include <list>
#include "paramconfigbase.h"
#include "Status.h"
#include "FaultCalc.h"


class CParamConfigStateSequence :
    public CParamConfigBase
{
protected:
    CParamConfigStateSequence(void);
    virtual ~CParamConfigStateSequence(void);

public:
    ///
    /// @brief
    ///     获取配置实例接口
    ///
    static CParamConfigStateSequence* getInstance();

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
    ///    枚举第一组状态
    ///
    /// @param[in]  bRebuildMap     是否需要重建SMV发送映射表、GOOSE发送映射表、GOOSE接收映射表
    ///
    /// @return
    ///     CStatus*    指向状态对象的指针，失败时返回NULL
    ///
    CStatus* FirstStatus(bool bRebuildMap = true);

    ///
    /// @brief
    ///    枚举下一组状态
    ///
    /// @param[in]  pCurStatus    指向当前状态对象的指针
    ///
    /// @return
    ///     CStatus*    指向状态对象的指针，失败时返回NULL
    ///
    CStatus* NextStatus(CStatus* pCurStatus);

    ///
    /// @brief
    ///    获得最后一组状态
    ///
    /// @return
    ///     CStatus*    指向状态对象的指针，失败时返回NULL
    ///
    CStatus* LastStatus();

    ///
    /// @brief
    ///    添加一个状态对象
    ///
    /// @return
    ///     CStatus*    指向状态对象的指针
    ///
    CStatus* AddStatus();

    ///
    /// @brief
    ///    删除指定的状态对象
    ///
    /// @param[in]  pCurStatus    指向状态对象的指针
    ///
    /// @return
    ///     bool    true表示删除成功
    ///
    bool DeleteStatus(CStatus* pCurStatus);

    ///
    /// @brief
    ///    获得状态个数
    ///
    /// @return
    ///     int    状态个数
    ///
    int GetStatusCount();

    ///
    /// @brief
    ///    清除所有状态对象
    ///
    void ClearAllStatus();

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

    ///
    /// @brief
    ///    获得故障计算对象
    ///
    /// @return
    ///     CFaultCalc*    指向故障计算对象的指针
    ///
    CFaultCalc* GetFaultCalc()
    {
        return &m_faultCalc;
    }

    ///
    /// @brief
    ///    使用故障计算结果
    ///
    /// @param[in]  pStatus   应用到哪组状态
    /// @param[in]  iGroup    应用到哪组通道（1或2）
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool UseFaultCalc(CStatus* pStatus, int iGroup);

protected:
    ///
    /// @brief
    ///    添加状态对象
    ///
    /// @param[in]  pNewStatus    指向状态对象的指针
    ///
    /// @return
    ///     bool    true表示添加成功
    ///
    bool __AddStatus(CStatus* pNewStatus);

    ///
    /// @brief
    ///    获取一个可用的状态对象内部ID
    ///
    /// @return
    ///     int    状态对象内部ID
    ///
    int __GetAvailableStatusID();

    ///
    /// @brief
    ///    判断指定内部ID号的状态对象是否存在
    ///
    /// @param[in]  nID    状态对象内部ID号
    ///
    /// @return
    ///     bool    true表示指定ID的状态对象存在
    ///
    bool __IsStatusExists(int nID);

private:
    std::list<CStatus *>                m_listStatusGroup;              ///< 状态序列容器
    bool                                m_bIsShowDI;                    ///< 是否显示开入量对话框
    CFaultCalc                          m_faultCalc;                    ///< 故障计算
};
#endif
