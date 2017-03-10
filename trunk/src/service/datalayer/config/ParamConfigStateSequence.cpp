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

#include "StdAfx.h"
#include "ParamConfigStateSequence.h"
#include "src\service\datalayer\config\ParamconfigManualTest.h"
#include "src\service\datalayer\config\ParamConfigGOOSERecv.h"
#include "src\service\datalayer\config\ParamConfigGOOSESend.h"

#define NODE_ROOT                       L"config"                       ///< 根节点
#define NODE_StatusGroup                L"StatusGroup"                  ///< 状态序列
#define NODE_Status                     L"Status"                       ///< 状态

///
/// @brief
///     构造函数
///
CParamConfigStateSequence::CParamConfigStateSequence(void)
: m_bIsShowDI(false)
{
}

///
/// @brief
///     析构函数
///
CParamConfigStateSequence::~CParamConfigStateSequence(void)
{
    // 清除所有状态对象
    ClearAllStatus();
}

///
/// @brief
///     获取配置实例接口
///
CParamConfigStateSequence* CParamConfigStateSequence::getInstance()
{
    static CParamConfigStateSequence s_paramConfigStateSequence;
    return &s_paramConfigStateSequence;
}

///
/// @brief
///    加载配置文件
///
/// @param[in]  strConfigFile 文件路径
///
/// @return
///     bool    true表示加载成功
///
bool CParamConfigStateSequence::LoadConfig( std::wstring strConfigFile )
{
    CMarkup                             xml;                            ///< 解析xml使用的CMarkUp对象


    if (strConfigFile.empty())
    {
        return false;
    }
    m_strConfigFile = strConfigFile;

    // 加载XML文件
    if (!xml.Load(m_strConfigFile))
    {
        return false;
    }

    // 清除所有状态对象
    ClearAllStatus();

    //
    // find and inside <config>
    //
    if (!xml.FindElem(NODE_ROOT) || !xml.IntoElem())
    {
        return false;
    }

    //
    // find and inside <StatusGroup>
    //
    if (xml.FindElem(NODE_StatusGroup) && xml.IntoElem())
    {
        //
        // find <Status>
        //
        while(xml.FindElem(NODE_Status))
        {
            int nID = _wtoi(xml.GetAttrib(L"id").c_str());
            if (0 != nID)
            {
                CStatus*      pStatus = new CStatus(nID, this);
                if (NULL == pStatus)
                {
                    return false;
                }

                // inside <Status>
                if (!xml.IntoElem())
                {
                    delete pStatus;
                    return false;
                }

                // 加载GOOSE控制块
                if (!pStatus->__LoadConfig(xml))
                {
                    delete pStatus;
                    return false;
                }

                // outside <Status>
                if (!xml.OutOfElem())
                {
                    delete pStatus;
                    return false;
                }

                // 添加状态对象指针到容器
                if (!__AddStatus(pStatus))
                {
                    delete pStatus;
                    return false;
                }
            }
        }

        // quit <StatusGroup>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    // 标记为未更改
    SetModified(false);
    return true;
}

///
/// @brief
///    添加状态对象
///
/// @param[in]  pNewStatus    指向状态对象的指针
///
/// @return
///     bool    true表示添加成功
///
bool CParamConfigStateSequence::__AddStatus( CStatus* pNewStatus )
{
    if (NULL == pNewStatus)
    {
        return false;
    }

    // 获得状态对象唯一标识ID
    unsigned int nID = pNewStatus->__GetID();

    std::list<CStatus*>::iterator iter = m_listStatusGroup.begin();
    while (m_listStatusGroup.end() != iter)
    {
        CStatus* pStatus = *iter;
        if (pStatus)
        {
            if (pStatus->__GetID() == nID)
            {
                return false ;
            }
        }

        iter++;
    }

    // 添加状态对象到容器中
    m_listStatusGroup.push_back(pNewStatus);
    return true;
}

///
/// @brief
///    保存配置文件
///
/// @return
///     bool    true表示保存成功
///
bool CParamConfigStateSequence::SaveConfig()
{
    CMarkup                             xml;                            ///< 解析xml使用的CMarkUp对象


    // 如果没有任何改动，那么不保存
    if (!m_bModified)
    {
        return true;
    }

    // add and inside <config>
    if (!xml.AddElem(NODE_ROOT) || !xml.IntoElem())
    {
        return false;
    }

    //
    // 遍历容器，保存所有状态对象
    //
    if (m_listStatusGroup.size() > 0)
    {
        // <StatusGroup>
        if (!xml.AddElem(NODE_StatusGroup) || !xml.IntoElem())
        {
            return false;
        }

        // 单个状态对象
        std::list<CStatus*>::iterator iter = m_listStatusGroup.begin();
        while (m_listStatusGroup.end() != iter)
        {
            CStatus* pStatus = *iter;
            if (pStatus)
            {
                // <Status>
                if (!xml.AddElem(NODE_Status))
                {
                    return false;
                }

                // <Status id="">
                if (!xml.SetAttrib(L"id", pStatus->__GetID()))
                {
                    return false;
                }

                // inside <Status>
                if (!xml.IntoElem())
                {
                    return false;
                }

                // 保存GOOSE控制块
                if (!pStatus->__SaveConfig(xml))
                {
                    return false;
                }

                // outside <Status>
                if (!xml.OutOfElem())
                {
                    return false;
                }
            }

            iter++;
        }

        // Quit <StatusGroup>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }

    // 添加xml头
    xml.ResetPos();
    xml.InsertNode( CMarkup::MNT_PROCESSING_INSTRUCTION, L"xml" );
    xml.SetAttrib( L"version", L"1.0" );
    xml.SetAttrib( L"encoding", L"utf-8" );

    // 保存xml文件
    if (!xml.Save(m_strConfigFile))
    {
        return false;
    }

    // 标记为未修改
    SetModified(false);
    return true;
}

///
/// @brief
///    清除所有状态对象
///
void CParamConfigStateSequence::ClearAllStatus()
{
    // 遍历容器，释放对象
    std::list<CStatus*>::iterator iter = m_listStatusGroup.begin();
    while (m_listStatusGroup.end() != iter)
    {
        CStatus* pStatus = *iter;
        if (pStatus)
        {
            delete pStatus;
        }

        iter++;
    }

    if (m_listStatusGroup.size())
    {
        // 清空容器
        m_listStatusGroup.clear();

        // 标记为已修改
        SetModified(true);
    }
}

///
/// @brief
///    枚举第一组状态
///
/// @param[in]  bRebuildMap     是否需要重建SMV发送映射表、GOOSE发送映射表、GOOSE接收映射表
///
/// @return
///     CStatus*    指向状态对象的指针
///
CStatus* CParamConfigStateSequence::FirstStatus( bool bRebuildMap /*= true*/ )
{
    if (bRebuildMap)
    {
        //
        // 手动试验、状态序列功能同名的虚拟通道共用同一个通道容器
        // 此处使用手动试验参数配置管理器，生成所有SMV发送虚拟通道的真实通道映射表
        //
        CParamconfigManualTest* pParamConfigManualTest = CParamconfigManualTest::getInstance();
        if (pParamConfigManualTest)
        {
            pParamConfigManualTest->RebuildSMVSendChannelMap();
        }

        // 获得GOOSE发送参数配置管理器，并生成所有GOOSE发送虚拟通道的真实通道映射表
        CParamConfigGOOSESend* pParamConfigGOOSESend = CParamConfigGOOSESend::getInstance();
        if (pParamConfigGOOSESend)
        {
            pParamConfigGOOSESend->RebuildGOOSESendChannelMap();
        }

        // 获得GOOSE接收参数配置管理器，并生成所有GOOSE接收虚拟通道的真实通道映射表
        CParamConfigGOOSERecv* pParamConfigGooseRecv = CParamConfigGOOSERecv::getInstance();
        if (pParamConfigGooseRecv)
        {
            pParamConfigGooseRecv->RebuildGOOSERecvChannelMap();
        }
    }

    std::list<CStatus*>::iterator iter = m_listStatusGroup.begin();
    if (m_listStatusGroup.end() != iter)
    {
        return *iter;
    }

    return NULL;
}

///
/// @brief
///    枚举下一组状态
///
/// @param[in]  pCurStatus    指向当前状态对象的指针
///
/// @return
///     CStatus*    指向状态对象的指针，失败时返回NULL
///
CStatus* CParamConfigStateSequence::NextStatus( CStatus* pCurStatus )
{
    CStatus*                        pStatus   = NULL;
    std::list<CStatus*>::iterator   iter;


    iter = m_listStatusGroup.begin();
    while (m_listStatusGroup.end() != iter)
    {
        pStatus = *iter;

        // 查找当前状态对象的位置
        if (pStatus == pCurStatus)
        {
            // 获得下一个状态对象的位置
            iter++;
            if (m_listStatusGroup.end() == iter)
            {
                return NULL;
            }

            return (*iter);
        }

        iter++;
    }

    return NULL;
}

///
/// @brief
///    获得最后一组状态
///
/// @return
///     CStatus*    指向状态对象的指针，失败时返回NULL
///
CStatus* CParamConfigStateSequence::LastStatus()
{
    std::list<CStatus*>::reverse_iterator rIter = m_listStatusGroup.rbegin();
    if (m_listStatusGroup.rend() != rIter)
    {
        return (*rIter);
    }

    return NULL;
}

///
/// @brief
///    获得状态个数
///
/// @return
///     int    状态个数
///
int CParamConfigStateSequence::GetStatusCount()
{
    return (int)m_listStatusGroup.size();
}

///
/// @brief
///    添加一个状态对象
///
/// @return
///     CStatus*    指向状态对象的指针
///
CStatus* CParamConfigStateSequence::AddStatus()
{
    // new一个状态对象
    CStatus*  pStatus = new CStatus(__GetAvailableStatusID(), this);
    if (NULL == pStatus)
    {
        return NULL;
    }

    //
    // 如果当前没有状态对象，那么创建默认状态对象
    // 如果已存在状态对象，那么拷贝最后一个状态对象
    //
    if (0 == m_listStatusGroup.size())
    {
        // 给状态对象添加默认的虚拟通道组
        if (!pStatus->__AddDefaultSMVSendVirtualChannels())
        {
            delete pStatus;
            return NULL;
        }
    }
    else
    {
        // 拷贝最后一个状态对象
        if (!pStatus->__Copy(LastStatus()))
        {
            delete pStatus;
            return NULL;
        }
    }

    // 添加状态对象指针到容器中
    if (!__AddStatus(pStatus))
    {
        delete pStatus;
        return NULL;
    }

    // 标记为已修改
    SetModified(true);
    return pStatus;
}

///
/// @brief
///    获取一个可用的状态对象内部ID
///
/// @return
///     int    状态对象内部ID
///
int CParamConfigStateSequence::__GetAvailableStatusID()
{
    int     nID = 1;


    while(1)
    {
        // 检查是否存在指定ID的状态对象
        if (!__IsStatusExists(nID))
        {
            break;
        }

        // ID号递增
        nID++;
    }

    return nID;
}

///
/// @brief
///    判断指定内部ID号的状态对象是否存在
///
/// @param[in]  nID    状态对象内部ID号
///
/// @return
///     bool    true表示指定ID的状态对象存在
///
bool CParamConfigStateSequence::__IsStatusExists( int nID )
{
    std::list<CStatus*>::iterator iter;


    // 遍历整个容器，检查是否存在指定ID的状态对象
    iter    = m_listStatusGroup.begin();
    while (m_listStatusGroup.end() != iter)
    {
        CStatus* pStatus = *iter;
        if (pStatus && (pStatus->__GetID() == nID))
        {
            return true;
        }

        iter++;
    }

    return false;
}

///
/// @brief
///    删除指定的状态对象
///
/// @param[in]  pCurStatus    指向状态对象的指针
///
/// @return
///     bool    true表示删除成功
///
bool CParamConfigStateSequence::DeleteStatus( CStatus* pCurStatus )
{
    // 检查输入参数
    if (NULL == pCurStatus)
    {
        return false;
    }

    // 遍历容器，释放对象
    std::list<CStatus*>::iterator iter = m_listStatusGroup.begin();
    while (m_listStatusGroup.end() != iter)
    {
        CStatus* pStatus = *iter;
        if (pStatus == pCurStatus)
        {
            m_listStatusGroup.remove(pStatus);
            delete pStatus;

            // 标记为已修改
            SetModified(true);
            return true;
        }

        iter++;
    }

    return true;
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
bool CParamConfigStateSequence::UseFaultCalc( CStatus* pStatus, int iGroup )
{
    int                 iRet;
    SCCalc::CalcModel   stCalcModel;
    SCCalc::Vector      stZl;
    SCCalc::Vector      stKs;
    SCCalc::Vector      stKl;
    SCCalc::Vector      stLc;
    SCCalc::Result      stResult;
    CString             csText;


    // 检查输入参数
    if ((1 != iGroup) && (2 != iGroup))
    {
        return false;
    }

    // 计算模型
    stCalcModel.type = m_faultCalc.GetCalcModelType();
    switch (stCalcModel.type)
    {
    case SCCalc::CURRENT_FIXED:
        stCalcModel.value.cv    = m_faultCalc.GetFaultCurrent();
        break;

    case SCCalc::VOLTAGE_FIXED:
        stCalcModel.value.cv    = m_faultCalc.GetFaultVoltage();
        break;

    case SCCalc::SYSTEM_IMPEDANCE_FIXED:
        stCalcModel.value.zs.md = m_faultCalc.GetSystemImpedanceZs().GetImpedance();
        stCalcModel.value.zs.dg = m_faultCalc.GetSystemImpedanceZs().GetAngle();
        break;

    default:
        return false;
    }

    // 线路阻抗Zl
    stZl.md = m_faultCalc.GetLineImpedanceZl().GetImpedance();
    stZl.dg = m_faultCalc.GetLineImpedanceZl().GetAngle();

    // 系统零序补偿系数Ks
    stKs.md = m_faultCalc.GetSystemCompensateKs().GetModelValue();
    stKs.dg = m_faultCalc.GetSystemCompensateKs().GetAngle();

    // 线路零序补偿系数Kl
    stKl.md = m_faultCalc.GetLineCompensateKl().GetModelValue();
    stKl.dg = m_faultCalc.GetLineCompensateKl().GetAngle();

    // 负荷电流、负荷功角
    stLc.md = m_faultCalc.GetLoadCurrent();
    stLc.dg = m_faultCalc.GetLoadAngle();

    // 故障计算
    iRet = SCCalc::Calc(m_faultCalc.GetFaultType()
        , m_faultCalc.GetRatedVoltage()
        , m_faultCalc.GetDirection()
        , stCalcModel
        , stZl
        , stKs
        , stKl
        , stLc
        , stResult);
    if (-1 == iRet)
    {
        return false;
    }

    // Ua*
    csText.Format(_T("Ua%d"), iGroup);
    pStatus->__SetSMVSendVCAmplitude(csText.GetString(), SCCalc::_Va(stResult).md);
    pStatus->__SetSMVSendVCPhaseAngle(csText.GetString(), SCCalc::_Va(stResult).dg);

    // Ub*
    csText.Format(_T("Ub%d"), iGroup);
    pStatus->__SetSMVSendVCAmplitude(csText.GetString(), SCCalc::_Vb(stResult).md);
    pStatus->__SetSMVSendVCPhaseAngle(csText.GetString(), SCCalc::_Vb(stResult).dg);

    // Uc*
    csText.Format(_T("Uc%d"), iGroup);
    pStatus->__SetSMVSendVCAmplitude(csText.GetString(), SCCalc::_Vc(stResult).md);
    pStatus->__SetSMVSendVCPhaseAngle(csText.GetString(), SCCalc::_Vc(stResult).dg);

    // Ia*
    csText.Format(_T("Ia%d"), iGroup);
    pStatus->__SetSMVSendVCAmplitude(csText.GetString(), SCCalc::_Ia(stResult).md);
    pStatus->__SetSMVSendVCPhaseAngle(csText.GetString(), SCCalc::_Ia(stResult).dg);

    // Ib*
    csText.Format(_T("Ib%d"), iGroup);
    pStatus->__SetSMVSendVCAmplitude(csText.GetString(), SCCalc::_Ib(stResult).md);
    pStatus->__SetSMVSendVCPhaseAngle(csText.GetString(), SCCalc::_Ib(stResult).dg);

    // Ic*
    csText.Format(_T("Ic%d"), iGroup);
    pStatus->__SetSMVSendVCAmplitude(csText.GetString(), SCCalc::_Ic(stResult).md);
    pStatus->__SetSMVSendVCPhaseAngle(csText.GetString(), SCCalc::_Ic(stResult).dg);

    return true;
}
