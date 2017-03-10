///
/// @brief
///     ����״̬���Խ������
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.7.1
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "StateTestResult.h"
#include "src\service\datalayer\config\ParamconfigManualTest.h"
#include "src\service\datalayer\config\GOOSERecvVirtualChannel.h"
#include "src\service\datalayer\config\ParamConfigGooseSend.h"


///
/// @brief
///    ���캯��
///
CStateTestResult::CStateTestResult(enumTestType eType)
: m_eTestType(eType)
, m_bSmvChannelChanged(false)
, m_bGooseChannelChanged(false)
, m_bStatusWordChanged(false)
, m_bQualityChanged(false)
, m_bTested(false)
{
    m_mapChannelRecvValues.clear();
    m_listGooseSendCbs.clear();
    m_listSmvSendBuffers.clear();
    m_listGooseSendBuffers.clear();
    memset(&m_stStateStartTime, 0, sizeof(m_stStateStartTime));
}

///
/// @brief
///    ��������
///
CStateTestResult::~CStateTestResult(void)
{
    // �ͷ����м�¼ͨ����ʷֵ�Ķ���
    ClearAllRecvValues();

    // ����ֶ������GOOSE���ƿ�
    ClearGooseSendCbs();

    // ���SMV����֡������
    ClearSmvSendBuffers();

    // ���GOOSE����֡������
    ClearGooseSendBuffers();
}

///
/// @brief
///    ��ʼ��
///
/// @return
///     bool    �ɹ�����true
///
bool CStateTestResult::Init()
{
    CChannelRecvValues* pChannelRecvValues  = NULL;
    std::map<std::wstring, CChannelRecvValues*>::iterator iter;


    // �ͷ����м�¼ͨ����ʷֵ�Ķ���
    ClearAllRecvValues();

    // �����������Ĭ�϶���
    for (int i = 0; i < sizeof(strGOOSERecvVCName)/sizeof(strGOOSERecvVCName[0]); i++)
    {
        // newһ����¼ͨ����ʷֵ�Ķ���
        pChannelRecvValues = new CChannelRecvValues(strGOOSERecvVCName[i]);
        if (NULL == pChannelRecvValues)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CStateTestResult::Init(): New CChannelRecvValues Object failed \n"));
            return false;
        }

        // ��Ӽ�¼ͨ����ʷֵ�Ķ���������
        iter = m_mapChannelRecvValues.find(strGOOSERecvVCName[i]);
        if (iter != m_mapChannelRecvValues.end())
        {
            RETAILMSG(OUTPUT_LOG_MSG, (L"CStateTestResult::Init(): Virtual channel already exists \n"));
            delete pChannelRecvValues;
            return false;
        }
        m_mapChannelRecvValues.insert(std::pair<std::wstring, CChannelRecvValues*>(strGOOSERecvVCName[i], pChannelRecvValues));
    }

    return true;
}

///
/// @brief
///    ��ò��Ե�����
///
/// @return
///     eTestType    ���Ե�����
///
enumTestType CStateTestResult::GetTestType()
{
    return m_eTestType;
}

///
/// @brief
///    �ͷ����м�¼ͨ����ʷֵ�Ķ���
///
void CStateTestResult::ClearAllRecvValues()
{
    CChannelRecvValues* pChannelRecvValues  = NULL;
    std::map<std::wstring, CChannelRecvValues*>::iterator iter;


    // �ͷż�¼ͨ����ʷֵ�Ķ���
    iter = m_mapChannelRecvValues.begin();
    while(m_mapChannelRecvValues.end() != iter)
    {
        pChannelRecvValues = iter->second;
        if (pChannelRecvValues)
        {
            delete pChannelRecvValues;
        }

        iter++;
    }

    // �������
    m_mapChannelRecvValues.clear();
}

///
/// @brief
///    ���ָ������ͨ�����ı���ͨ������ֵ�Ķ���
///
/// @param[in]  strName    ����ͨ����
///
/// @return
///     CChannelRecvValues*    ����ͨ������ֵ�ö���
///
CChannelRecvValues* CStateTestResult::GetChannelRecvValues( std::wstring strName )
{
    std::map<std::wstring, CChannelRecvValues*>::iterator iter;


    // ����ָ�����ֵ�����ͨ��
    iter = m_mapChannelRecvValues.find(strName);
    if (m_mapChannelRecvValues.end() == iter)
    {
        return NULL;
    }

    return (iter->second);
}

///
/// @brief
///    ���״̬��ʼ���Ե�ʱ��
///
/// @return
///     std::wstring    ��ʼ���Ե�ʱ��
///
std::wstring CStateTestResult::GetStartTime()
{
    return m_stStateStartTime.GetString();
}

///
/// @brief
///    ����״̬��ʼʱ�䣨���ʱ�䣩
///
/// @param[in]  stNewTime    ״̬��ʼʱ��
///
void CStateTestResult::SetStartTime( CEventTime stNewTime )
{
    m_stStateStartTime = stNewTime;
}

///
/// @brief
///    �����ֶ������SMV��������ͨ����ֵ
///
/// @return
///     bool    �ɹ�����true
///
bool CStateTestResult::__SaveManualTestSMVSendValues()
{
    // ����ֶ�����������ù�����
    CParamconfigManualTest* pParamConfigManualTest = CParamconfigManualTest::getInstance();
    if (NULL == pParamConfigManualTest)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CStateTestResult::__AddManualTestSMVSendValues(): Get CParamconfigManualTest Object failed \n"));
        return false;
    }

    // �ͷ�����SMV��������ͨ������
    m_stSMVSendValues.__ClearSMVSendVirtualChannel();

    // �����ֶ������SMV��������ͨ��
    std::list<CSMVSendVirtualChannel*>::iterator iter;
    for (iter = pParamConfigManualTest->m_listSMVSendVirtualChannelGroup.begin(); iter != pParamConfigManualTest->m_listSMVSendVirtualChannelGroup.end(); iter++)
    {
        CSMVSendVirtualChannel* pOldVirtualChannel = *iter;
        if (pOldVirtualChannel)
        {
            // newһ��CSMVSendVirtualChannel����Ϊ�˷�ֵ�ı�ȣ�ע��˴��������ͨ���������ó�FUNCTION_TYPE_STATE_SEQUENCE����
            CSMVSendVirtualChannel* pNewVirtualChannel = new CSMVSendVirtualChannel(FUNCTION_TYPE_STATE_SEQUENCE, NULL);
            if (NULL == pNewVirtualChannel)
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CStateTestResult::__AddManualTestSMVSendValues(): New CSMVSendVirtualChannel Object failed \n"));
                return false;
            }

            // ��������ͨ����ֵ
            if (!pNewVirtualChannel->__Copy(pOldVirtualChannel))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CStateTestResult::__AddManualTestSMVSendValues(): Copy Virtual Channel failed \n"));
                delete pNewVirtualChannel;
                return false;
            }

            // ����µ�����ͨ����������
            if (!m_stSMVSendValues.__AddSMVSendVirtualChannel(pNewVirtualChannel))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CStateTestResult::__AddManualTestSMVSendValues(): Add Virtual Channel failed \n"));
                delete pNewVirtualChannel;
                return false;
            }
        }
    }

    return true;
}

///
/// @brief
///    ö�ٵ�һ����ӳ���SMV��������ͨ��
///
/// @return
///     CVirtualChannel*    ָ������ͨ�������ָ�룬ʧ��ʱ����NULL
///
CSMVSendVirtualChannel* CStateTestResult::FirstSMVSendVirtualChannel()
{
    return m_stSMVSendValues.FirstSMVSendVirtualChannel();
}

///
/// @brief
///    �����һ����ӳ���SMV��������ͨ��
///
/// @param[in]  pCurVirtualChannel    ��ǰ����ͨ�������ָ��
///
/// @return
///     CVirtualChannel*    ָ������ͨ�������ָ�룬ʧ��ʱ����NULL
///
CSMVSendVirtualChannel* CStateTestResult::NextSMVSendVirtualChannel( CSMVSendVirtualChannel* pCurVirtualChannel )
{
    return m_stSMVSendValues.NextSMVSendVirtualChannel(pCurVirtualChannel);
}

///
/// @brief
///    �����GOOSE���Ϳ��ƿ��ֵ
///
/// @return
///     bool    �ɹ�����true
///
bool CStateTestResult::__SaveGooseSendCbs()
{
    // ���GOOSE���Ͳ������ù�����
    CParamConfigGOOSESend* pParamConfigGooseSend = CParamConfigGOOSESend::getInstance();
    if (NULL == pParamConfigGooseSend)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CStateTestResult::__SaveGooseSendCbs(): Get CParamConfigGOOSESend Object failed \n"));
        return false;
    }

    // �����GOOSE���ƿ�
    ClearGooseSendCbs();

    // �������б���ѡ��GOOSE���Ϳ��ƿ�
    std::list<CGooseCb*>::iterator iter;
    for (iter = pParamConfigGooseSend->m_listGocbGroup.begin(); iter != pParamConfigGooseSend->m_listGocbGroup.end(); iter++)
    {
        CGooseCb* pOldGocb = *iter;
        if (pOldGocb && pOldGocb->IsSelected())
        {
            // newһ��GOOSE���ƿ����
            CGooseCb* pNewGocb = new CGooseCb(NULL);
            if (NULL == pNewGocb)
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CStateTestResult::__SaveGooseSendCbs(): New GOOSE control block object failed \n"));
                return false;
            }
            // ���Ϊѡ��
            pNewGocb->Selected(true);

            // ��������ͨ����ֵ
            if (!pNewGocb->__Copy(pOldGocb))
            {
                RETAILMSG(OUTPUT_LOG_MSG, (L"CStateTestResult::__SaveGooseSendCbs(): Copy GOOSE control block failed \n"));
                delete pNewGocb;
                return false;
            }

            // ��ӿ��ƿ鵽������
            m_listGooseSendCbs.push_back(pNewGocb);
        }
    }

    return true;
}

///
/// @brief
///    ö�ٵ�һ��GOOSE���ƿ�
///
/// @return
///     CGooseCb*    �ɹ�ʱ����GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
///
CGooseCb* CStateTestResult::FirstGooseSendCb()
{
    std::list<CGooseCb*>::iterator iter = m_listGooseSendCbs.begin();
    if (m_listGooseSendCbs.end() != iter)
    {
        return *iter;
    }

    return NULL;
}

///
/// @brief
///    ö����һ��GOOSE���Ϳ��ƿ�
///
/// @param[in]  pCurGocb ��ǰλ��GOOSE���ƿ��ָ��
///
/// @return
///     CGooseCb*    �ɹ�ʱ����GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
///
CGooseCb* CStateTestResult::NextGooseSendCb( CGooseCb* pCurGocb )
{
    std::list<CGooseCb*>::iterator  iter;


    iter = m_listGooseSendCbs.begin();
    while (m_listGooseSendCbs.end() != iter)
    {
        // ���ҵ�ǰGOOSE���ƿ��λ��
        if (*iter == pCurGocb)
        {
            // �����һ��GOOSE���ƿ��λ��
            if (m_listGooseSendCbs.end() == ++iter)
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
///    ���ָ�����ƿ��������
///
/// @param[in]  pCurGocb    ָ����ƿ�����ָ��
///
/// @return
///     int    ���ƿ�������ʧ�ܷ���-1
///
int CStateTestResult::GetIndexByGooseCb( CGooseCb* pCurGocb )
{
    std::list<CGooseCb*>::iterator  iter;
    int                             nIndex = -1;


    for (nIndex = 0, iter = m_listGooseSendCbs.begin(); m_listGooseSendCbs.end() != iter; nIndex++, iter++)
    {
        // ���ҵ�ǰGOOSE���ƿ��λ��
        if (*iter == pCurGocb)
        {
            break;
        }
    }

    return nIndex;
}

///
/// @brief
///    �����������GOOSE���ƿ�
///
/// @param[in]  nCurIndex    ���ƿ�����
///
/// @return
///     CGooseCb*    �ɹ�ʱ����GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
///
CGooseCb* CStateTestResult::GetGooseCbByIndex( int nCurIndex )
{
    std::list<CGooseCb*>::iterator  iter;
    int                             nIndex;


    for (nIndex = 0, iter = m_listGooseSendCbs.begin(); m_listGooseSendCbs.end() != iter; nIndex++, iter++)
    {
        // ���ָ�������ŵĿ��ƿ�
        if (nIndex == nCurIndex)
        {
            return (*iter);
        }
    }

    return NULL;
}

///
/// @brief
///    ����ֶ������GOOSE���ƿ�
///
void CStateTestResult::ClearGooseSendCbs()
{
    // �����������ͷŶ���
    std::list<CGooseCb*>::iterator iter = m_listGooseSendCbs.begin();
    while (m_listGooseSendCbs.end() != iter)
    {
        CGooseCb* pGocb = *iter;
        if (pGocb)
        {
            delete pGocb;
        }

        iter++;
    }

    // �������
    m_listGooseSendCbs.clear();
}

///
/// @brief
///    ����ֶ�����SMV������ͨ����ֵ
///
/// @return
///     CStatus*    ��������ͨ��ֵ�Ķ���
///
CStatus* CStateTestResult::__GetSMVSendValues()
{
    return &m_stSMVSendValues;
}

///
/// @brief
///    ���ֶ�����SMV���͵�����ͨ��ֵ��ָ����״̬����������
///
/// @param[in]  eCompareType    �ȽϷ�ʽ
/// @param[in]  pOldStatusTestResult    ָ��״̬��������ָ��
///
/// @return
///     bool    ��ͬ����true
///
bool CStateTestResult::__CompareSMVSendValues( enumCompareType eCompareType, CStateTestResult* pOldStatusTestResult )
{
    return m_stSMVSendValues.__CompareSMVSendVirtualChannels(eCompareType, pOldStatusTestResult->__GetSMVSendValues());
}

///
/// @brief
///    ���ֶ�����GOOSE���͵Ŀ��ƿ���ָ����״̬����������
///
/// @param[in]  pOldStatusTestResult    ָ��״̬��������ָ��
///
/// @return
///     bool    ��ͬ����true
///
bool CStateTestResult::__CompareGooseSendValues( CStateTestResult* pOldStateTestResult )
{
    bool                            bRet    = true;


    // �ȽϿ��ƿ�����
    if (m_listGooseSendCbs.size() != pOldStateTestResult->m_listGooseSendCbs.size())
    {
        return false;
    }

    // ����ÿһ�����ƿ飬�ȽϿ��ƿ�ͨ��ֵ
    std::list<CGooseCb *>::iterator oldIter = pOldStateTestResult->m_listGooseSendCbs.begin();
    std::list<CGooseCb *>::iterator newIter = m_listGooseSendCbs.begin();
    while((oldIter != pOldStateTestResult->m_listGooseSendCbs.end()) && (newIter != m_listGooseSendCbs.end()))
    {
        CGooseCb* pOldGocb = *oldIter;
        CGooseCb* pNewGocb = *newIter;
        if ((NULL != pOldGocb) && (NULL != pNewGocb))
        {
            if (!pNewGocb->__CompareChannelSendValue(pOldGocb))
            {
                // ��ǿ��ƿ�Ϊ���޸�
                pNewGocb->SetSendValueChanged(true);

                // ������λ����
                pNewGocb->SetStNum(pOldGocb->GetStNum() + 1);
                bRet = false;
            }
            else
            {
                // ������λ����
                pNewGocb->SetStNum(pOldGocb->GetStNum());
            }
        }

        oldIter++;
        newIter++;
    }

    return bRet;
}

///
/// @brief
///    ��ѯSMVͨ��ֵ�Ƿ����仯
///
/// @return
///     bool    true��ʾ�����˱仯
///
bool CStateTestResult::IsSmvChannelChanged() const
{
    return m_bSmvChannelChanged;
}

///
/// @brief
///    ����SMVͨ��ֵ�Ƿ����仯
///
/// @param[in]  val    �Ƿ����˱仯
///
void CStateTestResult::SetSmvChannelChanged( bool val )
{
    m_bSmvChannelChanged = val;
}

///
/// @brief
///    ��ѯGOOSEͨ��ֵ�Ƿ����仯
///
/// @return
///     bool    true��ʾ�����˱仯
///
bool CStateTestResult::IsGooseChannelChanged() const
{
    return m_bGooseChannelChanged;
}

///
/// @brief
///    ����GOOSEͨ��ֵ�Ƿ����˱仯
///
/// @param[in]  val    �Ƿ����˱仯
///
void CStateTestResult::SetGooseChannelChanged( bool val )
{
    m_bGooseChannelChanged = val;
}

///
/// @brief
///    ��ѯ9-1��FT3��FT3��չ���ƿ��״̬���Ƿ�ı�
///
/// @return
///     bool    true��ʾ�����˱仯
///
bool CStateTestResult::IsStatusWordChanged() const
{
    return m_bStatusWordChanged;
}

///
/// @brief
///    ����9-1��FT3��FT3��չ���ƿ��״̬���Ƿ����˸ı�
///
/// @param[in]  val    �Ƿ����˸ı�
///
void CStateTestResult::SetStatusWordChanged( bool val )
{
    m_bStatusWordChanged = val;
}

///
/// @brief
///    ��ѯ9-2ͨ����Ʒ���Ƿ�ı�
///
/// @return
///     bool    true��ʾ�����˱仯
///
bool CStateTestResult::IsQualityChanged() const
{
    return m_bQualityChanged;
}

///
/// @brief
///    ����9-2ͨ����Ʒ���Ƿ����˸ı�
///
/// @param[in]  val    �Ƿ����˸ı�
///
void CStateTestResult::SetQualityChanged( bool val )
{
    m_bQualityChanged = val;
}

///
/// @brief
///    ���SMV����֡������
///
void CStateTestResult::ClearSmvSendBuffers()
{
    // ����������ɾ��ÿһ������������
    std::list<CStateSendBuffer*>::iterator iter = m_listSmvSendBuffers.begin();
    while (m_listSmvSendBuffers.end() != iter)
    {
        // ���ұ�λ���ƿ�
        CStateSendBuffer* pSendBuffer = *iter;
        if (pSendBuffer)
        {
            delete pSendBuffer;
        }

        iter++;
    }

    // �������
    m_listSmvSendBuffers.clear();
}

///
/// @brief
///    ���GOOSE����֡������
///
void CStateTestResult::ClearGooseSendBuffers()
{
    // ����������ɾ��ÿһ������������
    std::list<CStateSendBuffer*>::iterator iter = m_listGooseSendBuffers.begin();
    while (m_listGooseSendBuffers.end() != iter)
    {
        // ���ұ�λ���ƿ�
        CStateSendBuffer* pSendBuffer = *iter;
        if (pSendBuffer)
        {
            delete pSendBuffer;
        }

        iter++;
    }

    // �������
    m_listGooseSendBuffers.clear();
}

///
/// @brief
///    �޸�GOOSE���ƿ��е���/˫������ͨ��ʱ����Ҫ�Զ��޸�����/�����ʱ��ͨ����ֵ
///    �˺����Զ�Ѱ��Ѱ�ұ�λ�ĵ���/˫��ͨ����Ȼ��ı����ڻ�����ʱ��ͨ����ֵ
///
/// @param[in]  stTestStartTime    ����ʵ��Ŀ�ʼʱ��
///
void CStateTestResult::__UpdateGooseChannelTime(CEventTime stTestStartTime)
{
    // �������������ұ�λ�Ŀ��ƿ�
    std::list<CGooseCb*>::iterator iter = m_listGooseSendCbs.begin();
    while (m_listGooseSendCbs.end() != iter)
    {
        // ���ұ�λ���ƿ�
        CGooseCb* pGocb = *iter;
        if (pGocb && pGocb->IsSendValueChanged())
        {
            // �Զ�Ѱ��Ѱ�ұ�λ�ĵ���/˫��ͨ����Ȼ��ı����ڻ�����ʱ��ͨ����ֵ
            pGocb->__UpdateChannelTime(stTestStartTime + m_stStateStartTime);
        }

        iter++;
    }
}

///
/// @brief
///    ��ѯ�Ƿ�����˲���
///
/// @return
///     bool    ��ɲ��Է���true
///
bool CStateTestResult::IsTested() const
{
    return m_bTested;
}

///
/// @brief
///     ���ò�����ɱ�ʶ
///
/// @param[in]  val     �Ƿ���ɲ���
///
void CStateTestResult::SetTested( bool val )
{
    m_bTested = val;
}

///
/// @brief
///     ����SMV���ͻ���
///
/// @param[in]  pOldStateTestResult     �ϵ�״̬���Խ����
///
/// @return
///     bool    �����ɹ�����true
///
bool CStateTestResult::__CopySmvSendBuffers( CStateTestResult* pOldStateTestResult )
{
    // ����������
    if (NULL == pOldStateTestResult)
    {
        return false;
    }

    // ���SMV����֡������
    ClearSmvSendBuffers();

    // ���ο���״̬�������SMV���ͻ���
    std::list<CStateSendBuffer*>::iterator OldIter = pOldStateTestResult->m_listSmvSendBuffers.begin();
    for (; OldIter != pOldStateTestResult->m_listSmvSendBuffers.end(); OldIter++)
    {
        CStateSendBuffer* pOldStateSendBuffer = *OldIter;
        if (pOldStateSendBuffer)
        {
            // ����һ���µ�״̬���ͻ���
            CStateSendBuffer* pNewStateSendBuffer = new CStateSendBuffer(pOldStateSendBuffer->m_ePortName);
            if (NULL == pNewStateSendBuffer)
            {
                return false;
            }

            // ����״̬���ͻ���
            if (!pNewStateSendBuffer->Copy(pOldStateSendBuffer))
            {
                delete pNewStateSendBuffer;
                return false;
            }

            // ���״̬���ͻ��嵽������
            m_listSmvSendBuffers.push_back(pNewStateSendBuffer);
        }
    }

    return true;
}

///
/// @brief
///     ����Goose���ͻ���
///
/// @param[in]  pOldStateTestResult     �ϵ�״̬���Խ����
///
/// @return
///     bool    �����ɹ�����true
///
bool CStateTestResult::__CopyGooseSendBuffers( CStateTestResult* pOldStateTestResult )
{
    // ����������
    if (NULL == pOldStateTestResult)
    {
        return false;
    }

    // ���SMV����֡������
    ClearGooseSendBuffers();

    // ���ο���״̬�������SMV���ͻ���
    std::list<CStateSendBuffer*>::iterator OldIter = pOldStateTestResult->m_listGooseSendBuffers.begin();
    for (; OldIter != pOldStateTestResult->m_listGooseSendBuffers.end(); OldIter++)
    {
        CStateSendBuffer* pOldStateSendBuffer = *OldIter;
        if (pOldStateSendBuffer)
        {
            // ����һ���µ�״̬���ͻ���
            CStateSendBuffer* pNewStateSendBuffer = new CStateSendBuffer(pOldStateSendBuffer->m_ePortName);
            if (NULL == pNewStateSendBuffer)
            {
                return false;
            }

            // ����״̬���ͻ���
            if (!pNewStateSendBuffer->Copy(pOldStateSendBuffer))
            {
                delete pNewStateSendBuffer;
                return false;
            }

            // ��λ��λ��ʶ
            pNewStateSendBuffer->m_stIOCtrlParam.nInBuf[1] = 0;

            // ���״̬���ͻ��嵽������
            m_listGooseSendBuffers.push_back(pNewStateSendBuffer);
        }
    }

    return true;
}

///
/// @brief
///     ����GOOSE���ƿ�����Ҫ��λ��ʱ��ͨ��
///
void CStateTestResult::__SaveGooseCbTimeChannel()
{
    std::list<CStateSendBuffer*>::iterator iter = m_listGooseSendBuffers.begin();
    for (; iter != m_listGooseSendBuffers.end(); iter++)
    {
        CStateSendBuffer* pStateSendBuffer = *iter;
        if (pStateSendBuffer)
        {
            // ����GOOSE���ƿ�����Ҫ��λ��ʱ��ͨ��
            pStateSendBuffer->__SaveGooseCbTimeChannel();
        }
    }
}

///
/// @brief
///    ���캯��
///
/// @param[in]  ePortName    ���Ͷ˿�
///
CStateSendBuffer::CStateSendBuffer( DsmPortName ePortName )
: m_ePortName(ePortName)
, m_pucDataBuffer(NULL)
, m_uiDataBufferLen(0)
{
    memset(&m_stIOCtrlParam, 0, sizeof(m_stIOCtrlParam));
    m_listFrameInfo.clear();
}

///
/// @brief
///    ��������
///
CStateSendBuffer::~CStateSendBuffer()
{
    FreeDataBuffer();
}

///
/// @brief
///    ��������֡������
///
/// @param[in]  uiBufferSize    �������ֽ���
///
/// @return
///     unsigned char*    ָ�򻺳�����ָ��
///
unsigned char* CStateSendBuffer::AllocDataBuffer( uint32_t uiBufferSize )
{
    // �ͷ���ǰ�Ļ�����
    FreeDataBuffer();

    // �����ڴ�
    m_uiDataBufferLen   = uiBufferSize;
    m_pucDataBuffer     = new unsigned char[uiBufferSize];

    return m_pucDataBuffer;
}

///
/// @brief
///    �ͷ�����֡������
///
void CStateSendBuffer::FreeDataBuffer()
{
    if (m_pucDataBuffer)
    {
        delete[] m_pucDataBuffer;
        m_pucDataBuffer = NULL;
    }

    m_uiDataBufferLen = 0;
}

///
/// @brief
///    �޸����з���֡�ı�λʱ���ֽ�
///
/// @param[in]  stStateStartTime    �µı�λʱ��
///
/// @return
///     bool    �ɹ�����true
///
bool CStateSendBuffer::UpdateEventTime(CEventTime stStateStartTime)
{
    unsigned char*  pDataBuffer = NULL;
    std::list<CFrameInfo>::iterator  iter;


    // ���������ͻ����е�ÿһ֡
    for (iter = m_listFrameInfo.begin(); iter != m_listFrameInfo.end(); iter++)
    {
        // ��÷���֡��Ϣ�ṹ��
        CFrameInfo& stFrameInfo = *iter;
        if (NULL == stFrameInfo.m_pGooseCb)
        {
            return false;
        }

        // ��ñ�λʱ���ֽڵ�λ�ã����ͻ����ַ + ֡ƫ�� + ��λʱ��ƫ�ƣ�
        pDataBuffer = m_pucDataBuffer + stFrameInfo.m_uiFrameOffset + stFrameInfo.m_pGooseCb->m_stPduPblcOff.eventTmOff;

        // �޸ı�λʱ���ֽڵ�ֵ
        if (0 == stStateStartTime.CodeGooseEventTime(pDataBuffer, stFrameInfo.m_uiFrameLen))
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    �޸����з���֡�б�λʱ��ͨ����ֵ
///
/// @param[in]  stStateStartTime    �µı�λʱ��
///
/// @return
///     bool    �ɹ�����true
///
bool CStateSendBuffer::UpdateChannelTime( CEventTime stStateStartTime )
{
    unsigned char*  pucFrameHead = NULL;
    std::list<CFrameInfo>::iterator  iter;


    // ����ÿһ�����ƿ�
    for (iter = m_listFrameInfo.begin(); iter != m_listFrameInfo.end(); iter++)
    {
        CFrameInfo& stFrameInfo = *iter;
        if (stFrameInfo.m_pGooseCb)
        {
            // ��÷���֡����ʼλ�ã����ͻ����ַ + ֡ƫ�ƣ�
            pucFrameHead = m_pucDataBuffer + stFrameInfo.m_uiFrameOffset;

            // ���·���֡�б�λ��ʱ��ͨ����ֵ
            if (!stFrameInfo.__UpdateChannelTime(pucFrameHead, stStateStartTime))
            {
                return false;
            }
        }
    }

    return true;
}

///
/// @brief
///     �Է����˱�λ��GOOSE���ƿ�������±���
///
/// @return
///     bool    �ɹ�����true
///
bool CStateSendBuffer::RecodeChangedGooseCb()
{
    unsigned char*  pucFrameHead    = NULL;
    unsigned char*  pucFrameStNum   = NULL;
    std::list<CFrameInfo>::iterator  iter;


    // ����ÿһ�����ƿ�
    for (iter = m_listFrameInfo.begin(); iter != m_listFrameInfo.end(); iter++)
    {
        CFrameInfo& stFrameInfo = *iter;
        if (stFrameInfo.m_pGooseCb && stFrameInfo.m_pGooseCb->IsSendValueChanged())
        {
            // ��÷���֡����ʼλ�ã����ͻ����ַ + ֡ƫ�ƣ�
            pucFrameHead = m_pucDataBuffer + stFrameInfo.m_uiFrameOffset;

            // �Է����˱�λ�Ŀ��ƿ�ͨ���������±���
            if (!stFrameInfo.m_pGooseCb->__RecodeChangedChannels(pucFrameHead))
            {
                return false;
            }

            // ���stNum�ֶε�λ�ã����ͻ����ַ + ֡ƫ�� + ��λʱ��ƫ�ƣ�
            pucFrameStNum = m_pucDataBuffer + stFrameInfo.m_uiFrameOffset + stFrameInfo.m_pGooseCb->m_stPduPblcOff.stNumOff;

            // �޸�stNum�ֶε�ֵ
            CodeGooseStNum(stFrameInfo.m_pGooseCb->GetStNum(), pucFrameStNum, stFrameInfo.m_uiFrameLen);

            // ��λ��λ��ʶ
            stFrameInfo.m_pGooseCb->SetSendValueChanged(false);
        }
    }

    return true;
}

///
/// @brief
///     �������ͻ���
///
/// @param[in]  pOldStateSendBuffer     �ϵķ��ͻ���
///
/// @return
///     bool    �����ɹ�����true
///
bool CStateSendBuffer::Copy( CStateSendBuffer* pOldStateSendBuffer )
{
    // �ͷ���ǰ�Ļ�����
    FreeDataBuffer();

    // ��������������Ϣ
    m_ePortName         = pOldStateSendBuffer->m_ePortName;
    m_stIOCtrlParam     = pOldStateSendBuffer->m_stIOCtrlParam;
    m_uiDataBufferLen   = pOldStateSendBuffer->m_uiDataBufferLen;
    m_listFrameInfo.assign(pOldStateSendBuffer->m_listFrameInfo.begin(), pOldStateSendBuffer->m_listFrameInfo.end());

    // ���뻺�����ռ�
    m_pucDataBuffer     = new unsigned char[m_uiDataBufferLen];
    if (NULL == m_pucDataBuffer)
    {
        return false;
    }

    // ����������
    if(0 != memcpy_s(m_pucDataBuffer, m_uiDataBufferLen, pOldStateSendBuffer->m_pucDataBuffer, pOldStateSendBuffer->m_uiDataBufferLen))
    {
        FreeDataBuffer();
        return false;
    }

    return true;
}

///
/// @brief
///     ����GOOSE���ƿ�����Ҫ��λ��ʱ��ͨ��
///
void CStateSendBuffer::__SaveGooseCbTimeChannel()
{
    std::list<CFrameInfo>::iterator  iter;


    // ���������ͻ����е�ÿһ֡
    for (iter = m_listFrameInfo.begin(); iter != m_listFrameInfo.end(); iter++)
    {
        // ��÷���֡��Ϣ�ṹ��
        CFrameInfo& stFrameInfo = *iter;

        // ����GOOSE���ƿ���������Ҫ��λ��ʱ��ͨ������ͨ��ָ�뱣�浽������
        stFrameInfo.__SaveTimeChannel();
    }
}

///
/// @brief
///     ����GOOSE���ƿ���������Ҫ��λ��ʱ��ͨ������ͨ��ָ�뱣�浽������
///
void CFrameInfo::__SaveTimeChannel()
{
    if (m_pGooseCb)
    {
        // ����GOOSE���ƿ���������Ҫ��λ��ʱ��ͨ������ͨ��ָ�뱣�浽������
        m_pGooseCb->__SaveTimeChannel(m_listTimeChannel);
    }
}

///
/// @brief
///    ���·���֡����Ҫ��λ��ʱ��ͨ����ֵ
///
/// @param[in]  pucFrameHead    ָ����֡֡ͷ��ָ��
/// @param[in]  stNewTime       �µı�λʱ��
///
/// @return
///     bool    �ɹ�����true
///
bool CFrameInfo::__UpdateChannelTime( unsigned char* pucFrameHead, CEventTime stNewTime )
{
    CChannel*       pChannel;
    std::list<CChannel*>::iterator  iter;


    // ����ÿһ����Ҫ��λ��ʱ��ͨ��
    iter = m_listTimeChannel.begin();
    for (; iter != m_listTimeChannel.end(); iter++)
    {
        pChannel = *iter;

        // �����ʱ������ͨ��
        if (pChannel && (CHANNEL_TYPE_GOOSE_TIME == pChannel->GetChannelType()))
        {
            // ���ͨ������ֵ
            GOOSE_DATA_VARIANT& stSendValue = pChannel->GetGooseSendValue();
            if (GSEDT_UTC_TIME != stSendValue.m_stUnKnown.id.type)
            {
                return false;
            }

            // �޸�ͨ������ֵ
            stSendValue.m_stUtcTime.t_s     = stNewTime.ts_s;
            stSendValue.m_stUtcTime.t_ns    = stNewTime.ts_ns;

            // �޸Ļ�������ʱ��ͨ���ı���ֵ
            stSendValue.CodeGooseData(pucFrameHead + stSendValue.m_stUtcTime.id.offset, 10);
        }
    }

    return true;
}
