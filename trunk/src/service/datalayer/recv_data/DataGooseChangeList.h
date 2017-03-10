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

#pragma once
#include <deque>
#include <map>
#include "src\utils\MessageCodec\include\goose_types.h"
#include "src\service\datalayer\recv\FrameRecvStruct.h"
#include "src\service\datalayer\recv_data\DataGooseInfo.h"

/*
��λ��Ϣ����ṹ��һ�����ƿ���ܶ�Ӧ�ж�εı�λ��Ϣ��
ÿ�α�λ��Ϣ��Ϊһ������������γ�һ�����ƿ�ı�λ�б�
һ�α�λ��Ϣ�а�����α�λ��ʱ�䣬�Լ���λ��ͨ����Ϣ��
�����ж��ͨ��ͬʱ��λ����Ҫͬʱ��¼���ͨ����Ϣ��
|------------------|------------------------------|
|                  |            |        |        |
|                  |            |        |        |
|                  |            |  ��    |  ��    |
|                  |            |  λ    |  λ    |
|                  | ��һ�α�λ |  ǰ    |  ʱ    |
|                  |    ʱ��    |  ��    |  ��    |
| ���ƿ�           |            |  ��    |  ��    |
|                  |            |        |        |
|                  |            |        |        |
|                  |------------------------------|
|                  |            |        |        |
|                  |            |        |        |
|                  |            |  ��    |  ��    |
|                  | ��N�α�λ  |  λ    |  λ    |
|                  |    ʱ��    |  ǰ    |  ʱ    |
|                  |            |  ��    |  ��    |
|                  |            |  ��    |  ��    |
|                  |            |        |        |
|                  |            |        |        |
|------------------|------------------------------|
*/


///
/// @brief
///    ����һ�α�λ��Ϣ���������ͨ�����ݣ�
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
    ///    ��ȡ��λǰ��Ϣ����
    ///
    CGooseInfo* GetDataBeforeChange()
    {
        return m_pGooseInfo1;
    }

    ///
    /// @brief
    ///    ��ȡ��λ����Ϣ����
    ///
    CGooseInfo* GetDataAfterChange()
    {
        return m_pGooseInfo2;
    }

    ///
    /// @brief
    ///    ��ȡ��λ��ͨ����Ŀ
    ///
    uint32_t GetChangedChannelCounts();

    ///
    /// @brief
    ///    ��ȡ��nIndex����λ��ͨ����ţ�ͨ����Ŵ�0��ʼ��
    ///
    /// @param[in] nIndex   Ҫ��ȡ�ĵڼ�����λ��ͨ����nIndexȡֵ��Χ��0��GetChangedChannelCounts -1.
    ///
    /// @return
    ///     int    ���ص�nIndex����λ��ͨ�����룬ͨ������Ҳ�Ǵ�0��ʼ��,-1��ʾʧ��
    ///
    int GetChangedChannelIndex(uint32_t nIndex);

    ///
    /// @brief
    ///    �ж�ĳ��ͨ�������Ƿ����仯
    ///
    /// @param[in]  nChannelIndex   Ҫ�жϵ�ͨ����������0��ʼ
    ///
    /// @return
    ///     bool    true��ʾ���������ݱ仯
    ///
    bool IsChannelChanged(uint32_t nChannelIndex);

    ///
    /// @brief
    ///    ��ȡ��ǰ��λ��Ϣ�ǵڼ��α�λ
    ///
    uint32_t GetChangeIndex()
    {
        return m_nChangeIndex;
    }

private:
    friend class CFrameRecv;
    ///
    /// @brief
    ///    ���ñ�λǰ��Ϣ����
    ///
    void SetDataBeforeChange(CGooseInfo* pGooseInfo)
    {
        m_pGooseInfo1 = pGooseInfo;
    }

    ///
    /// @brief
    ///    ���ñ�λ����Ϣ����
    ///
    void SetDataAfterChange(CGooseInfo* pGooseInfo)
    {
        m_pGooseInfo2 = pGooseInfo;
    }

    ///
    /// @brief
    ///    ������λ�б���Ϣ,�����ñ�λǰ�ͱ�λ�����ݺ���ò�����λ�б���Ϣ
    ///
    void GenChangeList();

    ///
    /// @brief
    ///    �ж�����GooseData�Ƿ���ͬ
    ///
    /// @return
    ///     ����ͬ����true
    ///
    bool IsGooseDataDiff(GOOSE_DATA* pData1, GOOSE_DATA* pData2);

private:
    CGooseInfo*                 m_pGooseInfo1;                          ///< ��λǰ���ƿ�����
    CGooseInfo*                 m_pGooseInfo2;                          ///< ��λʱ�Ŀ��ƿ�����
    std::vector<uint32_t>       m_vecChangeList;                        ///< �����λͨ����Ϣ
    uint32_t                    m_nChangeIndex;                         ///< ��¼��ǰ�ı�λ��Ϣ��������ƿ�ĵڼ��α�λ��Ϣ

};

///
/// @brief
///    ����һ�����ƿ�ı�λ��Ϣ�б��������ʱ�̱�λ��Ϣ
///
typedef std::vector<CGooseOneTimeChange>    VecGooseChangeList;

///
/// @brief
///    ����һ�����ƿ�������֡������Ϣ��ÿ�α�λʱ����һ������
///
class CGooseChangeListData
{
public:
    CGooseChangeListData();
    ~CGooseChangeListData();

public:
    ///
    /// @brief
    ///    ���һ�α�λʱ����,����CGooseInfo�Ѿ�������ͨ�����ݵ�ָ��
    ///    ������ӵ�pGooseInfoӦ������new����ģ����������ิ��ʱ�ڴ����
    ///    pGooseInfoָ������ݵ��ͷ��ɱ�����й���
    ///
    void AddGooseInfo(CGooseInfo* pGooseInfo);

    ///
    /// @brief
    ///    ��ȡ��λ����Ŀ����������һ�γ�ʼ��Ϣ
    ///
    uint32_t GetGooseInfoCounts();

    ///
    /// @brief
    ///    ��ȡһ����λ��Ϣ����
    ///
    CGooseInfo* GetOneGooseInfo(uint32_t nIndex);

    ///
    /// @brief
    ///    ���������Ϣ
    ///
    void ClearAllGooseInfo();

    ///
    /// @brief
    ///    ��ȡ��ǰ���ƿ��ܹ���λ����,���ܴ��ڱ���ı�λ����
    ///
    uint32_t GetTotalChangeTimes();

    ///
    /// @brief
    ///    ��ȡ�ϴλ�ȡ����ʱ�ı�λ����ֵ
    ///
    uint32_t GetLastGetChangeTimes()
    {
        return m_nLastGetChangeTimes;
    }

    ///
    /// @brief
    ///    �����ϴ����һ�λ�ȡ����λ�б�����ʱ�ı�λ����ֵ
    ///
    void SetLastGetChangeTines(uint32_t nTimes)
    {
        m_nLastGetChangeTimes = nTimes;
    }

private:
    std::deque<CGooseInfo*>                 m_dequeGooseInfo;           ///< ��¼���ƿ�ı�λ��Ϣ��ÿ�α�λ��¼���ε�֡���ݣ���ʱֻ��¼20�α�λ��Ϣ
    uint32_t                                m_nTotalChangeTimes;        ///< ��¼��ǰ���ƿ��ܵı�λ������������ʼ״̬
    uint32_t                                m_nLastGetChangeTimes;      ///< ��¼�ϴλ�ȡ����ʱ�ı�λ��Ϣ�����ⲿ����������
};

///
/// @brief
///    �����һ�����ƿ���صı�λ��Ϣ,ÿ�α�λ��stNum�仯��ʱ���浱�ε�����֡ͨ������
///
typedef std::map<CBaseDetectItem*, CGooseChangeListData*>   MapDetectItemChangeList;

///
/// @brief
///    ����Goose��λ��Ϣ������潻������
///
class CDataGooseChangeList
{
public:
    CDataGooseChangeList();
    ~CDataGooseChangeList();

    ///
    /// @brief
    ///    ��ȡ���ƿ�
    ///
    CBaseDetectItem* GetCtrlBlk()
    {
        return m_pCurrentDetectItem;
    }

    ///
    /// @brief
    ///    ��ȡ��λ����
    ///
    uint32_t GetChangeTimes()
    {
        return m_vecGooseChangeList.size();
    }

    ///
    /// @brief
    ///    ��ȡĳ�α�λʱ��
    ///
    GOOSE_UTC_TIME GetGooseChangeTime(uint32_t nIndex);

    ///
    /// @brief
    ///    ��ȡһ�α�λ����Ϣ
    ///
    CGooseOneTimeChange GetGooseChangeInfo(uint32_t nIndex);

    ///
    /// @brief
    ///    ��ȡĳ�α�λ��λǰ����ָ��
    ///
    /// @return
    ///     CGooseInfo* ��������ָ�룬NULL��ʾʧ��
    ///
    CGooseInfo* GetDataBeforeChange(uint32_t nIndex);

    ///
    /// @brief
    ///    ��ȡĳ�α�λ��λ������ָ��
    ///
    /// @return
    ///     CGooseInfo* ��������ָ�룬NULL��ʾʧ��
    ///
    CGooseInfo* GetDataAfterChange(uint32_t nIndex);

    ///
    /// @brief
    ///    ��ȡͨ����Ŀ,����б�λ����ȡ���±�λʱ��ͨ����
    ///    ���û�б�λ��Ϣ����ʹ��̽��ʱ���ƿ��б����ͨ����
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
    ///    ��ȡ�Ƿ�����µı�λ��Ϣ
    ///
    uint32_t GetNewChangeTimes()
    {
        return m_nNewChangeTimes;;
    }

private:
    ///
    /// @brief
    ///    ���ÿ��ƿ�
    ///
    void SetCtrlBlk(CBaseDetectItem* pDetectItem)
    {
        m_pCurrentDetectItem = pDetectItem;
    }

    ///
    /// @brief
    ///    �����������ı�λ��Ϣ
    ///
    void _ClearChangeList();

    ///
    /// @brief
    ///    ���������ӵ�ͨ����Ŀ
    ///
    void SetNewChangeTimes(uint16_t nNewTimes)
    {
        m_nNewChangeTimes = nNewTimes;
    }

private:
    friend class CFrameRecv;
    CBaseDetectItem*                m_pCurrentDetectItem;               ///< Goose��λ��Ϣ��Ӧ�Ŀ��ƿ�ָ��
    VecGooseChangeList              m_vecGooseChangeList;               ///< ĳ�����ƿ��Ӧ��goose��λ��Ϣ
    uint32_t                        m_nNewChangeTimes;;                 ///< �����µı�λ��Ϣ����
};