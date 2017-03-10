///
/// @brief
///     ���ͨ�����н���ֵ����
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.7.1
///
/// �޸���ʷ��
///

#ifndef __CHANNEL_RECV_VALUES__
#define __CHANNEL_RECV_VALUES__
#include <string>
#include <list>
#include <afxmt.h>
#include "src\utils\MessageCodec\include\goose_types.h"

///
/// @brief
///    ͨ����ʷֵ
///
struct CHistoryData
{
    CEventTime              m_stChangeTime;                             ///< ͨ��ֵ�����仯��ʱ��
    GOOSE_DATA_VARIANT      m_stHistoryValue;                           ///< ͨ����ʷֵ

    ///
    /// @brief
    ///    ͨ��ֵ�����仯��ʱ��
    ///
    /// @return
    ///    std::wstring    UTCʱ���ַ���
    ///
    std::wstring GetChangeTimeString()
    {
        int         iMilliSecond;
        wchar_t     cBuffer[128]    = {0};


        // ��ʽ����ʱ���ַ���
        iMilliSecond = m_stChangeTime.ts_s*1000 + m_stChangeTime.ts_ns/1000000;
        swprintf_s(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]), L"%d.0", iMilliSecond);
        return cBuffer;
    }

    ///
    /// @brief
    ///    ���ָ��ͨ����ʷֵ��ָ��
    ///
    /// @return
    ///     GOOSE_DATA_VARIANT*    ָ��ͨ����ʷֵ��ָ��
    ///
    GOOSE_DATA_VARIANT* GetGooseData()
    {
        return &m_stHistoryValue;
    }
};

///
/// @brief
///    ���ͨ�����н���ֵ����
///
class CChannelRecvValues
{
public:
    CChannelRecvValues(std::wstring strName);
    ~CChannelRecvValues(void);

    ///
    /// @brief
    ///    ���ͨ�����н���ֵ
    ///
    void ClearAllRecvValues();

    ///
    /// @brief
    ///    ���GOOSEͨ����ʷ����ֵ
    ///
    /// @param[in]  stChangeTime    GOOSEֵ��λʱ�䣨�����ʵ�鿪ʼ��ʱ�䣩
    /// @param[in]  stNewGooseData  GOOSEͨ��ֵ
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool SaveRecvValue(CEventTime stChangeTime, GOOSE_DATA_VARIANT& stNewGooseData);

    ///
    /// @brief
    ///    �������ͨ����
    ///
    /// @return
    ///     std::wstring    ����ͨ����
    ///
    std::wstring GetChannelName();

    ///
    /// @brief
    ///    ���ͨ��ֵ��λ����
    ///
    /// @return
    ///     int    ��λ����
    ///
    int GetChangeTimes();

    ///
    /// @brief
    ///    ��õ�һ������ֵ
    ///
    /// @return
    ///     CHistoryData*    ָ��ͨ����ʷֵ�����ָ�룬ʧ��ʱ����NULL
    ///
    CHistoryData* FirstRecvData();

    ///
    /// @brief
    ///    �����һ������ֵ
    ///
    /// @param[in]  pCurHistoryData    ָ��ǰͨ����ʷֵ�����ָ��
    ///
    /// @return
    ///     CHistoryData*    ָ����һ��ͨ����ʷֵ�����ָ�룬ʧ��ʱ����NULL
    ///
    CHistoryData* NextHistoryData(CHistoryData* pCurHistoryData);

private:
    std::wstring                    m_strName;                          ///< ����ͨ����
    int                             m_iChangeTimes;                     ///< ��λ����

    CCriticalSection                m_criticalHistoryData;              ///< �����������ٽ��������ڻ���Խ���ֵ�����ķ���
    std::list<CHistoryData*>        m_listHistory;                      ///< ����ͨ������ֵ������
};
#endif
