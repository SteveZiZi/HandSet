/// @file
///
/// @brief
///     ��¼goose��λ��Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.6.15
///
/// �޸���ʷ��
///

#include "stdafx.h"
#include "DataGooseChangeList.h"

///
/// @brief
///    ��ȡĳ�α�λʱ��
///
GOOSE_UTC_TIME CDataGooseChangeList::GetGooseChangeTime(uint32_t nIndex)
{
    if (nIndex < m_vecGooseChangeList.size())
    {
        if (m_vecGooseChangeList[nIndex].GetDataBeforeChange() && m_vecGooseChangeList[nIndex].GetDataAfterChange())
        {
            return m_vecGooseChangeList[nIndex].GetDataAfterChange()->GetEventTime();
        }
    }
    GOOSE_UTC_TIME stTime;
    stTime.t_s = 0;
    stTime.t_ns = 0;
    return stTime;
}

///
/// @brief
///    ��ȡĳ�α�λ��λǰ����ָ��
///
/// @return
///     CGooseInfo* ��������ָ�룬NULL��ʾʧ��
///
CGooseInfo* CDataGooseChangeList::GetDataBeforeChange( uint32_t nIndex )
{
    if (nIndex < m_vecGooseChangeList.size())
    {
        return m_vecGooseChangeList[nIndex].GetDataBeforeChange();
    }
    return NULL;
}

///
/// @brief
///    ��ȡĳ�α�λ��λ������ָ��
///
/// @return
///     CGooseInfo* ��������ָ�룬NULL��ʾʧ��
///
CGooseInfo* CDataGooseChangeList::GetDataAfterChange( uint32_t nIndex )
{
    if (nIndex < m_vecGooseChangeList.size())
    {
        return m_vecGooseChangeList[nIndex].GetDataAfterChange();
    }
    return NULL;
}

///
/// @brief
///    ��ȡһ�α�λ����Ϣ
///
CGooseOneTimeChange CDataGooseChangeList::GetGooseChangeInfo( uint32_t nIndex )
{
    if (nIndex < m_vecGooseChangeList.size())
    {
        return m_vecGooseChangeList[nIndex];
    }
    return CGooseOneTimeChange();
}

///
/// @brief
///    �����������ı�λ��Ϣ
///
void CDataGooseChangeList::_ClearChangeList()
{
    m_vecGooseChangeList.clear();
}

CDataGooseChangeList::CDataGooseChangeList() : m_nNewChangeTimes(0)
{

}

CDataGooseChangeList::~CDataGooseChangeList()
{

}

///
/// @brief
///    �ж�ĳ��ͨ�������Ƿ����仯
///
/// @param[in]  nChannelIndex   Ҫ�жϵ�ͨ����������0��ʼ
///
/// @return
///     bool    true��ʾ���������ݱ仯
///
bool CGooseOneTimeChange::IsChannelChanged( uint32_t nChannelIndex )
{
    if (m_pGooseInfo1 && m_pGooseInfo2)
    {
        CGooseChannel* pChannel1 = m_pGooseInfo1->GetChanneData(nChannelIndex);
        CGooseChannel* pChannel2 = m_pGooseInfo2->GetChanneData(nChannelIndex);

        GOOSE_DATA* pData1 = NULL;
        GOOSE_DATA* pData2 = NULL;

        if (pChannel1)
        {
            pData1 = pChannel1->GetChannelData();
        }
        if (pChannel2)
        {
            pData2 = pChannel2->GetChannelData();
        }
        return IsGooseDataDiff(pData1, pData2);
    }
    return true;
}

///
/// @brief
///    �ж�����GooseData�Ƿ���ͬ
///
/// @return
///     ����ͬ����true
///
bool CGooseOneTimeChange::IsGooseDataDiff( GOOSE_DATA* pData1, GOOSE_DATA* pData2 )
{
    if (pData1 && pData2)
    {
        if (pData1->type == pData2->type)
        {
            switch(pData1->type)
            {
            case GSEDT_UNKNOW:
            case GSEDT_BOOLEAN:
            case GSEDT_BIT_STR:
            case GSEDT_INT8:
            case GSEDT_INT16:
            case GSEDT_INT24:
            case GSEDT_INT32:
            case GSEDT_UINT8:
            case GSEDT_UINT16:
            case GSEDT_UINT24:
            case GSEDT_UINT32:
            case GSEDT_FLOAT:
            case GSEDT_OCTET_STR:
            case GSEDT_VISIBLE_STR:
                {
                    GOOSE_DATA_VARIANT GDV1(pData1);
                    GOOSE_DATA_VARIANT GDV2(pData2);
                    return (GDV1 != GDV2);
                }
                break;
                //utcʱ�䲻��Ϊ�ж����ݣ�������Ϊ����ͬ��
            case GSEDT_UTC_TIME:
                return false;
                break;
            case GSEDT_STRUCT:
                {
                    GOOSE_STRUCT* pDataStruct1 = (GOOSE_STRUCT*)pData1;
                    GOOSE_STRUCT* pDataStruct2 = (GOOSE_STRUCT*)pData2;
                    uint32_t nSubChannels = pDataStruct1->member.size();
                    bool bRetDiff = false;
                    for (uint32_t i = 0; i < nSubChannels; ++i)
                    {
                        if (IsGooseDataDiff(pDataStruct1->member[i], pDataStruct2->member[i]))
                        {
                            bRetDiff = true;
                            break;
                        }
                    }
                    return bRetDiff;
                }
                break;
                //��������Ĭ����ͬ
            default:
                return false;
                break;
            }
        }
    }
    return true;
}

///
/// @brief
///    ������λ�б���Ϣ,�����ñ�λǰ�ͱ�λ�����ݺ���ò�����λ�б���Ϣ
///
void CGooseOneTimeChange::GenChangeList()
{
    m_vecChangeList.clear();
    if (m_pGooseInfo1 && m_pGooseInfo2)
    {
        uint32_t nChannelCount1 = m_pGooseInfo1->GetChannelCounts();
        uint32_t nChannelCount2 = m_pGooseInfo2->GetChannelCounts();
        uint32_t nMaxChannels = max(nChannelCount1, nChannelCount2);
        for (uint32_t i = 0; i < nMaxChannels; ++i)
        {
            if (IsChannelChanged(i))
            {
                m_vecChangeList.push_back(i);
            }
        }
    }
}

///
/// @brief
///    ��ȡ��nIndex����λ��ͨ����ţ�ͨ����Ŵ�0��ʼ��
///
/// @param[in] nIndex   Ҫ��ȡ�ĵڼ�����λ��ͨ����nIndexȡֵ��Χ��0��GetChangedChannelCounts -1.
///
/// @return
///     uint32_t    ���ص�nIndex����λ��ͨ�����룬ͨ������Ҳ�Ǵ�0��ʼ��
///
int CGooseOneTimeChange::GetChangedChannelIndex( uint32_t nIndex )
{
    if (nIndex < m_vecChangeList.size())
    {
        std::vector<uint32_t>::iterator itr = m_vecChangeList.begin();
        if (itr != m_vecChangeList.end())
        {
            advance(itr, nIndex);
            if (itr != m_vecChangeList.end())
            {
                return (*itr);
            }
        }
    }
    return -1;
}

///
/// @brief
///    ��ȡ��λ��ͨ����Ŀ
///
uint32_t CGooseOneTimeChange::GetChangedChannelCounts()
{
    return m_vecChangeList.size();
}

CGooseChangeListData::CGooseChangeListData(): m_nTotalChangeTimes(0)
                                            , m_nLastGetChangeTimes(0)
{

}

CGooseChangeListData::~CGooseChangeListData()
{
    ClearAllGooseInfo();
}

///
/// @brief
///    ���һ�α�λʱ����,����CGooseInfo�Ѿ�������ͨ�����ݵ�ָ��
///    ������ӵ�pGooseInfoӦ������new����ģ����������ิ��ʱ�ڴ����
///    pGooseInfoָ������ݵ��ͷ��ɱ�����й���
///
void CGooseChangeListData::AddGooseInfo( CGooseInfo* pGooseInfo )
{
    m_dequeGooseInfo.push_back(pGooseInfo);
    m_nTotalChangeTimes++;
    while (m_dequeGooseInfo.size() > DSM_MAX_GOOSE_CHANGELIST_SIZE + 1)
    {
        delete m_dequeGooseInfo.front();
        m_dequeGooseInfo.pop_front();
    }
}

///
/// @brief
///    ��ȡ��λ����Ŀ����������һ�γ�ʼ��Ϣ
///
uint32_t CGooseChangeListData::GetGooseInfoCounts()
{
    return m_dequeGooseInfo.size();
}

///
/// @brief
///    ��ȡһ����λ��Ϣ����
///
CGooseInfo* CGooseChangeListData::GetOneGooseInfo( uint32_t nIndex )
{
    size_t nVecSize = m_dequeGooseInfo.size();
    if (nVecSize > 0  && nIndex < nVecSize)
    {
        std::deque<CGooseInfo*>::iterator itr = m_dequeGooseInfo.begin();
        advance(itr, nIndex);
        if (itr != m_dequeGooseInfo.end())
        {
            return (*itr);
        }
    }
    return NULL;
}

///
/// @brief
///    ���������Ϣ
///
void CGooseChangeListData::ClearAllGooseInfo()
{
     std::deque<CGooseInfo*>::iterator itr = m_dequeGooseInfo.begin();
     while(itr != m_dequeGooseInfo.end())
     {
         if (NULL != (*itr))
         {
             delete (*itr);
             (*itr) = NULL;
         }

         ++itr;
     }
     m_dequeGooseInfo.clear();
}

///
/// @brief
///    ��ȡ��ǰ���ƿ��ܹ���λ����
///
uint32_t CGooseChangeListData::GetTotalChangeTimes()
{
    if (m_nTotalChangeTimes > 1)
    {
        return m_nTotalChangeTimes - 1;
    }
    return 0;
}
