///
/// @brief
///     ״̬���в�������
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.6.14
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "ParamConfigStateSequence.h"
#include "src\service\datalayer\config\ParamconfigManualTest.h"
#include "src\service\datalayer\config\ParamConfigGOOSERecv.h"
#include "src\service\datalayer\config\ParamConfigGOOSESend.h"

#define NODE_ROOT                       L"config"                       ///< ���ڵ�
#define NODE_StatusGroup                L"StatusGroup"                  ///< ״̬����
#define NODE_Status                     L"Status"                       ///< ״̬

///
/// @brief
///     ���캯��
///
CParamConfigStateSequence::CParamConfigStateSequence(void)
: m_bIsShowDI(false)
{
}

///
/// @brief
///     ��������
///
CParamConfigStateSequence::~CParamConfigStateSequence(void)
{
    // �������״̬����
    ClearAllStatus();
}

///
/// @brief
///     ��ȡ����ʵ���ӿ�
///
CParamConfigStateSequence* CParamConfigStateSequence::getInstance()
{
    static CParamConfigStateSequence s_paramConfigStateSequence;
    return &s_paramConfigStateSequence;
}

///
/// @brief
///    ���������ļ�
///
/// @param[in]  strConfigFile �ļ�·��
///
/// @return
///     bool    true��ʾ���سɹ�
///
bool CParamConfigStateSequence::LoadConfig( std::wstring strConfigFile )
{
    CMarkup                             xml;                            ///< ����xmlʹ�õ�CMarkUp����


    if (strConfigFile.empty())
    {
        return false;
    }
    m_strConfigFile = strConfigFile;

    // ����XML�ļ�
    if (!xml.Load(m_strConfigFile))
    {
        return false;
    }

    // �������״̬����
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

                // ����GOOSE���ƿ�
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

                // ���״̬����ָ�뵽����
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

    // ���Ϊδ����
    SetModified(false);
    return true;
}

///
/// @brief
///    ���״̬����
///
/// @param[in]  pNewStatus    ָ��״̬�����ָ��
///
/// @return
///     bool    true��ʾ��ӳɹ�
///
bool CParamConfigStateSequence::__AddStatus( CStatus* pNewStatus )
{
    if (NULL == pNewStatus)
    {
        return false;
    }

    // ���״̬����Ψһ��ʶID
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

    // ���״̬����������
    m_listStatusGroup.push_back(pNewStatus);
    return true;
}

///
/// @brief
///    ���������ļ�
///
/// @return
///     bool    true��ʾ����ɹ�
///
bool CParamConfigStateSequence::SaveConfig()
{
    CMarkup                             xml;                            ///< ����xmlʹ�õ�CMarkUp����


    // ���û���κθĶ�����ô������
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
    // ������������������״̬����
    //
    if (m_listStatusGroup.size() > 0)
    {
        // <StatusGroup>
        if (!xml.AddElem(NODE_StatusGroup) || !xml.IntoElem())
        {
            return false;
        }

        // ����״̬����
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

                // ����GOOSE���ƿ�
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

    // ���xmlͷ
    xml.ResetPos();
    xml.InsertNode( CMarkup::MNT_PROCESSING_INSTRUCTION, L"xml" );
    xml.SetAttrib( L"version", L"1.0" );
    xml.SetAttrib( L"encoding", L"utf-8" );

    // ����xml�ļ�
    if (!xml.Save(m_strConfigFile))
    {
        return false;
    }

    // ���Ϊδ�޸�
    SetModified(false);
    return true;
}

///
/// @brief
///    �������״̬����
///
void CParamConfigStateSequence::ClearAllStatus()
{
    // �����������ͷŶ���
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
        // �������
        m_listStatusGroup.clear();

        // ���Ϊ���޸�
        SetModified(true);
    }
}

///
/// @brief
///    ö�ٵ�һ��״̬
///
/// @param[in]  bRebuildMap     �Ƿ���Ҫ�ؽ�SMV����ӳ���GOOSE����ӳ���GOOSE����ӳ���
///
/// @return
///     CStatus*    ָ��״̬�����ָ��
///
CStatus* CParamConfigStateSequence::FirstStatus( bool bRebuildMap /*= true*/ )
{
    if (bRebuildMap)
    {
        //
        // �ֶ����顢״̬���й���ͬ��������ͨ������ͬһ��ͨ������
        // �˴�ʹ���ֶ�����������ù���������������SMV��������ͨ������ʵͨ��ӳ���
        //
        CParamconfigManualTest* pParamConfigManualTest = CParamconfigManualTest::getInstance();
        if (pParamConfigManualTest)
        {
            pParamConfigManualTest->RebuildSMVSendChannelMap();
        }

        // ���GOOSE���Ͳ������ù�����������������GOOSE��������ͨ������ʵͨ��ӳ���
        CParamConfigGOOSESend* pParamConfigGOOSESend = CParamConfigGOOSESend::getInstance();
        if (pParamConfigGOOSESend)
        {
            pParamConfigGOOSESend->RebuildGOOSESendChannelMap();
        }

        // ���GOOSE���ղ������ù�����������������GOOSE��������ͨ������ʵͨ��ӳ���
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
///    ö����һ��״̬
///
/// @param[in]  pCurStatus    ָ��ǰ״̬�����ָ��
///
/// @return
///     CStatus*    ָ��״̬�����ָ�룬ʧ��ʱ����NULL
///
CStatus* CParamConfigStateSequence::NextStatus( CStatus* pCurStatus )
{
    CStatus*                        pStatus   = NULL;
    std::list<CStatus*>::iterator   iter;


    iter = m_listStatusGroup.begin();
    while (m_listStatusGroup.end() != iter)
    {
        pStatus = *iter;

        // ���ҵ�ǰ״̬�����λ��
        if (pStatus == pCurStatus)
        {
            // �����һ��״̬�����λ��
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
///    ������һ��״̬
///
/// @return
///     CStatus*    ָ��״̬�����ָ�룬ʧ��ʱ����NULL
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
///    ���״̬����
///
/// @return
///     int    ״̬����
///
int CParamConfigStateSequence::GetStatusCount()
{
    return (int)m_listStatusGroup.size();
}

///
/// @brief
///    ���һ��״̬����
///
/// @return
///     CStatus*    ָ��״̬�����ָ��
///
CStatus* CParamConfigStateSequence::AddStatus()
{
    // newһ��״̬����
    CStatus*  pStatus = new CStatus(__GetAvailableStatusID(), this);
    if (NULL == pStatus)
    {
        return NULL;
    }

    //
    // �����ǰû��״̬������ô����Ĭ��״̬����
    // ����Ѵ���״̬������ô�������һ��״̬����
    //
    if (0 == m_listStatusGroup.size())
    {
        // ��״̬�������Ĭ�ϵ�����ͨ����
        if (!pStatus->__AddDefaultSMVSendVirtualChannels())
        {
            delete pStatus;
            return NULL;
        }
    }
    else
    {
        // �������һ��״̬����
        if (!pStatus->__Copy(LastStatus()))
        {
            delete pStatus;
            return NULL;
        }
    }

    // ���״̬����ָ�뵽������
    if (!__AddStatus(pStatus))
    {
        delete pStatus;
        return NULL;
    }

    // ���Ϊ���޸�
    SetModified(true);
    return pStatus;
}

///
/// @brief
///    ��ȡһ�����õ�״̬�����ڲ�ID
///
/// @return
///     int    ״̬�����ڲ�ID
///
int CParamConfigStateSequence::__GetAvailableStatusID()
{
    int     nID = 1;


    while(1)
    {
        // ����Ƿ����ָ��ID��״̬����
        if (!__IsStatusExists(nID))
        {
            break;
        }

        // ID�ŵ���
        nID++;
    }

    return nID;
}

///
/// @brief
///    �ж�ָ���ڲ�ID�ŵ�״̬�����Ƿ����
///
/// @param[in]  nID    ״̬�����ڲ�ID��
///
/// @return
///     bool    true��ʾָ��ID��״̬�������
///
bool CParamConfigStateSequence::__IsStatusExists( int nID )
{
    std::list<CStatus*>::iterator iter;


    // ������������������Ƿ����ָ��ID��״̬����
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
///    ɾ��ָ����״̬����
///
/// @param[in]  pCurStatus    ָ��״̬�����ָ��
///
/// @return
///     bool    true��ʾɾ���ɹ�
///
bool CParamConfigStateSequence::DeleteStatus( CStatus* pCurStatus )
{
    // ����������
    if (NULL == pCurStatus)
    {
        return false;
    }

    // �����������ͷŶ���
    std::list<CStatus*>::iterator iter = m_listStatusGroup.begin();
    while (m_listStatusGroup.end() != iter)
    {
        CStatus* pStatus = *iter;
        if (pStatus == pCurStatus)
        {
            m_listStatusGroup.remove(pStatus);
            delete pStatus;

            // ���Ϊ���޸�
            SetModified(true);
            return true;
        }

        iter++;
    }

    return true;
}

///
/// @brief
///    ʹ�ù��ϼ�����
///
/// @param[in]  pStatus   Ӧ�õ�����״̬
/// @param[in]  iGroup    Ӧ�õ�����ͨ����1��2��
///
/// @return
///     bool    �ɹ�����true
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


    // ����������
    if ((1 != iGroup) && (2 != iGroup))
    {
        return false;
    }

    // ����ģ��
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

    // ��·�迹Zl
    stZl.md = m_faultCalc.GetLineImpedanceZl().GetImpedance();
    stZl.dg = m_faultCalc.GetLineImpedanceZl().GetAngle();

    // ϵͳ���򲹳�ϵ��Ks
    stKs.md = m_faultCalc.GetSystemCompensateKs().GetModelValue();
    stKs.dg = m_faultCalc.GetSystemCompensateKs().GetAngle();

    // ��·���򲹳�ϵ��Kl
    stKl.md = m_faultCalc.GetLineCompensateKl().GetModelValue();
    stKl.dg = m_faultCalc.GetLineCompensateKl().GetAngle();

    // ���ɵ��������ɹ���
    stLc.md = m_faultCalc.GetLoadCurrent();
    stLc.dg = m_faultCalc.GetLoadAngle();

    // ���ϼ���
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
