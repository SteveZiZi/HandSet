/// @file
///
/// @brief
///     记录goose变位信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.6.15
///
/// 修改历史：
///

#include "stdafx.h"
#include "DataGooseChangeList.h"

///
/// @brief
///    获取某次变位时间
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
///    获取某次变位变位前数据指针
///
/// @return
///     CGooseInfo* 返回数据指针，NULL表示失败
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
///    获取某次变位变位后数据指针
///
/// @return
///     CGooseInfo* 返回数据指针，NULL表示失败
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
///    获取一次变位的信息
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
///    清除容器保存的变位信息
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
///    判断某个通道数据是否发生变化
///
/// @param[in]  nChannelIndex   要判断的通道索引，从0开始
///
/// @return
///     bool    true表示发生了数据变化
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
///    判断两个GooseData是否不相同
///
/// @return
///     不相同返回true
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
                //utc时间不作为判断依据，总是认为是相同的
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
                //其他类型默认相同
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
///    产生变位列表信息,在设置变位前和变位后数据后调用产生变位列表信息
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
///    获取第nIndex个变位的通道序号（通道序号从0开始）
///
/// @param[in] nIndex   要获取的第几个变位的通道，nIndex取值范围从0到GetChangedChannelCounts -1.
///
/// @return
///     uint32_t    返回第nIndex个变位的通道号码，通道号码也是从0开始的
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
///    获取变位的通道数目
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
///    添加一次变位时数据,由于CGooseInfo已经包含了通道数据的指针
///    这里添加的pGooseInfo应该是用new申请的，尽量减少类复制时内存操作
///    pGooseInfo指向的数据的释放由本类进行管理
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
///    获取变位总条目数，包括第一次初始信息
///
uint32_t CGooseChangeListData::GetGooseInfoCounts()
{
    return m_dequeGooseInfo.size();
}

///
/// @brief
///    获取一个变位信息数据
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
///    清空所有信息
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
///    获取当前控制块总共变位次数
///
uint32_t CGooseChangeListData::GetTotalChangeTimes()
{
    if (m_nTotalChangeTimes > 1)
    {
        return m_nTotalChangeTimes - 1;
    }
    return 0;
}
