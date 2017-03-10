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

#pragma once
#include <deque>
#include <map>
#include "src\utils\MessageCodec\include\goose_types.h"
#include "src\service\datalayer\recv\FrameRecvStruct.h"
#include "src\service\datalayer\recv_data\DataGooseInfo.h"

/*
变位信息保存结构，一个控制块可能对应有多次的变位信息，
每次变位信息作为一项，放入容器中形成一个控制块的变位列表
一次变位信息中包含这次变位的时间，以及变位的通道信息，
可能有多个通道同时变位，需要同时记录多个通道信息。
|------------------|------------------------------|
|                  |            |        |        |
|                  |            |        |        |
|                  |            |  变    |  变    |
|                  |            |  位    |  位    |
|                  | 第一次变位 |  前    |  时    |
|                  |    时间    |  数    |  数    |
| 控制块           |            |  据    |  据    |
|                  |            |        |        |
|                  |            |        |        |
|                  |------------------------------|
|                  |            |        |        |
|                  |            |        |        |
|                  |            |  变    |  变    |
|                  | 第N次变位  |  位    |  位    |
|                  |    时间    |  前    |  时    |
|                  |            |  数    |  数    |
|                  |            |  据    |  据    |
|                  |            |        |        |
|                  |            |        |        |
|------------------|------------------------------|
*/


///
/// @brief
///    定义一次变位信息（包含多个通道数据）
///
class CGooseOneTimeChange
{
public:
    CGooseOneTimeChange() : m_pGooseInfo1(NULL)
                          , m_pGooseInfo2(NULL)
                          , m_nChangeIndex(0)
    {

    }
    ~CGooseOneTimeChange()
    {

    }

public:
    ///
    /// @brief
    ///    获取变位前信息数据
    ///
    CGooseInfo* GetDataBeforeChange()
    {
        return m_pGooseInfo1;
    }

    ///
    /// @brief
    ///    获取变位后信息数据
    ///
    CGooseInfo* GetDataAfterChange()
    {
        return m_pGooseInfo2;
    }

    ///
    /// @brief
    ///    获取变位的通道数目
    ///
    uint32_t GetChangedChannelCounts();

    ///
    /// @brief
    ///    获取第nIndex个变位的通道序号（通道序号从0开始）
    ///
    /// @param[in] nIndex   要获取的第几个变位的通道，nIndex取值范围从0到GetChangedChannelCounts -1.
    ///
    /// @return
    ///     int    返回第nIndex个变位的通道号码，通道号码也是从0开始的,-1表示失败
    ///
    int GetChangedChannelIndex(uint32_t nIndex);

    ///
    /// @brief
    ///    判断某个通道数据是否发生变化
    ///
    /// @param[in]  nChannelIndex   要判断的通道索引，从0开始
    ///
    /// @return
    ///     bool    true表示发生了数据变化
    ///
    bool IsChannelChanged(uint32_t nChannelIndex);

    ///
    /// @brief
    ///    获取当前变位信息是第几次变位
    ///
    uint32_t GetChangeIndex()
    {
        return m_nChangeIndex;
    }

private:
    friend class CFrameRecv;
    ///
    /// @brief
    ///    设置变位前信息数据
    ///
    void SetDataBeforeChange(CGooseInfo* pGooseInfo)
    {
        m_pGooseInfo1 = pGooseInfo;
    }

    ///
    /// @brief
    ///    设置变位后信息数据
    ///
    void SetDataAfterChange(CGooseInfo* pGooseInfo)
    {
        m_pGooseInfo2 = pGooseInfo;
    }

    ///
    /// @brief
    ///    产生变位列表信息,在设置变位前和变位后数据后调用产生变位列表信息
    ///
    void GenChangeList();

    ///
    /// @brief
    ///    判断两个GooseData是否不相同
    ///
    /// @return
    ///     不相同返回true
    ///
    bool IsGooseDataDiff(GOOSE_DATA* pData1, GOOSE_DATA* pData2);

private:
    CGooseInfo*                 m_pGooseInfo1;                          ///< 变位前控制块数据
    CGooseInfo*                 m_pGooseInfo2;                          ///< 变位时的控制块数据
    std::vector<uint32_t>       m_vecChangeList;                        ///< 保存变位通道信息
    uint32_t                    m_nChangeIndex;                         ///< 记录当前的变位信息是这个控制块的第几次变位信息

};

///
/// @brief
///    定义一个控制块的变位信息列表，包含多个时刻变位信息
///
typedef std::vector<CGooseOneTimeChange>    VecGooseChangeList;

///
/// @brief
///    定义一个控制块块的数据帧内容信息，每次变位时保存一次数据
///
class CGooseChangeListData
{
public:
    CGooseChangeListData();
    ~CGooseChangeListData();

public:
    ///
    /// @brief
    ///    添加一次变位时数据,由于CGooseInfo已经包含了通道数据的指针
    ///    这里添加的pGooseInfo应该是用new申请的，尽量减少类复制时内存操作
    ///    pGooseInfo指向的数据的释放由本类进行管理
    ///
    void AddGooseInfo(CGooseInfo* pGooseInfo);

    ///
    /// @brief
    ///    获取变位总条目数，包括第一次初始信息
    ///
    uint32_t GetGooseInfoCounts();

    ///
    /// @brief
    ///    获取一个变位信息数据
    ///
    CGooseInfo* GetOneGooseInfo(uint32_t nIndex);

    ///
    /// @brief
    ///    清空所有信息
    ///
    void ClearAllGooseInfo();

    ///
    /// @brief
    ///    获取当前控制块总共变位次数,可能大于保存的变位次数
    ///
    uint32_t GetTotalChangeTimes();

    ///
    /// @brief
    ///    获取上次获取数据时的变位次数值
    ///
    uint32_t GetLastGetChangeTimes()
    {
        return m_nLastGetChangeTimes;
    }

    ///
    /// @brief
    ///    设置上次最后一次获取到变位列表数据时的变位次数值
    ///
    void SetLastGetChangeTines(uint32_t nTimes)
    {
        m_nLastGetChangeTimes = nTimes;
    }

private:
    std::deque<CGooseInfo*>                 m_dequeGooseInfo;           ///< 记录控制块的变位信息，每次变位记录当次的帧内容，暂时只记录20次变位信息
    uint32_t                                m_nTotalChangeTimes;        ///< 记录当前控制块总的变位次数，包括初始状态
    uint32_t                                m_nLastGetChangeTimes;      ///< 记录上次获取数据时的变位信息，由外部调用者设置
};

///
/// @brief
///    定义和一个控制块相关的变位信息,每次变位（stNum变化）时保存当次的数据帧通道数据
///
typedef std::map<CBaseDetectItem*, CGooseChangeListData*>   MapDetectItemChangeList;

///
/// @brief
///    保存Goose变位信息，与界面交互数据
///
class CDataGooseChangeList
{
public:
    CDataGooseChangeList();
    ~CDataGooseChangeList();

    ///
    /// @brief
    ///    获取控制块
    ///
    CBaseDetectItem* GetCtrlBlk()
    {
        return m_pCurrentDetectItem;
    }

    ///
    /// @brief
    ///    获取变位次数
    ///
    uint32_t GetChangeTimes()
    {
        return m_vecGooseChangeList.size();
    }

    ///
    /// @brief
    ///    获取某次变位时间
    ///
    GOOSE_UTC_TIME GetGooseChangeTime(uint32_t nIndex);

    ///
    /// @brief
    ///    获取一次变位的信息
    ///
    CGooseOneTimeChange GetGooseChangeInfo(uint32_t nIndex);

    ///
    /// @brief
    ///    获取某次变位变位前数据指针
    ///
    /// @return
    ///     CGooseInfo* 返回数据指针，NULL表示失败
    ///
    CGooseInfo* GetDataBeforeChange(uint32_t nIndex);

    ///
    /// @brief
    ///    获取某次变位变位后数据指针
    ///
    /// @return
    ///     CGooseInfo* 返回数据指针，NULL表示失败
    ///
    CGooseInfo* GetDataAfterChange(uint32_t nIndex);

    ///
    /// @brief
    ///    获取通道数目,如果有变位，获取最新变位时的通道数
    ///    如果没有变位信息，则使用探测时控制块中保存的通道数
    ///
    uint32_t GetChannelCounts()
    {
        if (!m_vecGooseChangeList.empty())
        {
            CGooseInfo* pGooseInfo = m_vecGooseChangeList.back().GetDataAfterChange();
            if (pGooseInfo)
            {
                return pGooseInfo->GetChannelCounts();
            }
        }

        if (m_pCurrentDetectItem)
        {
            return m_pCurrentDetectItem->nChannelCount;
        }
        return 0;
    }

    ///
    /// @brief
    ///    获取是否包含新的变位信息
    ///
    uint32_t GetNewChangeTimes()
    {
        return m_nNewChangeTimes;;
    }

private:
    ///
    /// @brief
    ///    设置控制块
    ///
    void SetCtrlBlk(CBaseDetectItem* pDetectItem)
    {
        m_pCurrentDetectItem = pDetectItem;
    }

    ///
    /// @brief
    ///    清除容器保存的变位信息
    ///
    void _ClearChangeList();

    ///
    /// @brief
    ///    设置新增加的通道数目
    ///
    void SetNewChangeTimes(uint16_t nNewTimes)
    {
        m_nNewChangeTimes = nNewTimes;
    }

private:
    friend class CFrameRecv;
    CBaseDetectItem*                m_pCurrentDetectItem;               ///< Goose变位信息对应的控制块指针
    VecGooseChangeList              m_vecGooseChangeList;               ///< 某个控制块对应的goose变位信息
    uint32_t                        m_nNewChangeTimes;;                 ///< 包含新的变位信息个数
};