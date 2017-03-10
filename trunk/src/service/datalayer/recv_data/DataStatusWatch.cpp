/// @file
///
/// @brief
///     ����һ�����ƿ��ͨ����Ч�ԣ��Ƿ�ͬ������Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.4.23
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "DataStatusWatch.h"
#include "src\service\datalayer\config\SmvRecvConfig.h"

#define SMV_9_2_QUOLITY_VALIDITY_MASK    0x00000003        ///< 9-2����ֵ��Ʒ�����������λ��ʾʮ����Ч��00��Ч
#define SMV_9_2_QUOLITY_TESTFLAG_MASK    0x00000800        ///< 9-2����ֵ��Ʒ���������ޱ�־��0��ʾû�м���
#define SMV_9_2_QUOLITY_DERIVED_MASK     0x00002000        ///< 9-2����ֵ��Ʒ�������ϳɱ�־��0��ʾ�Ǻϳ���

CDataStatusWatch::CDataStatusWatch(void)
{
}

CDataStatusWatch::~CDataStatusWatch(void)
{
}

///
/// @brief
///    ��ȡ����֡����
///
enumFrameType CDataStatusWatch::GetFrameType()
{
    if (m_pCtrlBlk)
    {
        return m_pCtrlBlk->enFrameType;
    }
    return FRAME_TYPE_UNKOWN;
}

///
/// @brief
///    ��ȡͨ��Ʒ������
///
/// @param[in]  nIndex  ͨ��������
/// @param[out] nQuality �����ȡ��Ʒ������
///
/// @return
///     bool    ��ȡƷ�������ɹ�����true
///
bool CDataStatusWatch::GetOneChannelQuality( uint32_t nIndex, uint32_t& nQuality )
{
    return m_cSmvCtrBlkQuality.GetChannelQuality(nIndex, nQuality);
}

///
/// @brief
///    ��ȡͨ���Ƿ���Ч
///
/// @param[in]  nIndex  ͨ��������
///
/// @return
///     bool    ����ͨ�������Ƿ���Ч��true��ʶ��Ч
///
bool CDataStatusWatch::GetOneChannelIsValid( uint32_t nIndex )
{
    return m_cSmvCtrBlkQuality.IsChannelValid(nIndex);
}

bool CDataStatusWatch::GetMuIsSync()
{
    return m_cSmvCtrBlkQuality.IsMuSync();
}

///
/// @brief
///    ���ÿ��ƿ���Ϣ
///
void CDataStatusWatch::SetCtrlBlk( CBaseDetectItem* pDetectItem )
{
    m_pCtrlBlk = pDetectItem;
    m_cSmvCtrBlkQuality.SetCtrlBlk(pDetectItem);
}

///
/// @brief
///    ����mu�Ƿ�ͬ��
///
void CDataStatusWatch::SetMuSync( bool bSync )
{
    m_cSmvCtrBlkQuality.SetMuSync(bSync);
}

///
/// @brief
///    ����ͨ��Ʒ������,���9-2
///
bool CDataStatusWatch::SetChannelQuality( uint32_t nChannelIndex, uint32_t nQuality )
{
    return m_cSmvCtrBlkQuality.SetChannelQuality(nChannelIndex, nQuality);
}

///
/// @brief
///    ���ÿ��ƿ�״̬�֣����ft3/9-1
///
bool CDataStatusWatch::SetCtrlBlkStatusWords( uint16_t nStatusWord1, uint16_t nStatusWord2 )
{
    return m_cSmvCtrBlkQuality.SetCtrlBlkStatusWords(nStatusWord1, nStatusWord2);
}

///
/// @brief
///    ����һ��ͨ����Ϣ
///
CStatusWatchChannelInfo CDataStatusWatch::GetOneChannelInfo( uint32_t nIndex )
{
    CStatusWatchChannelInfo stChannelInfo;
    uint32_t nQuality = 0;
    if (m_cSmvCtrBlkQuality.GetChannelQuality(nIndex, nQuality))
    {
        stChannelInfo.m_nQuality = nQuality;
        stChannelInfo.m_bChannelDerived = _GetQualityDerived(nQuality);
        stChannelInfo.m_bChannelTestFlag = _GetQualityTestFlag(nQuality);
    }
    stChannelInfo.m_nIndex = nIndex;
    stChannelInfo.m_strChannelDesc = GetChannelDesc(nIndex);
    stChannelInfo.m_bChannelValid = GetOneChannelIsValid(nIndex);

    return stChannelInfo;
}

///
/// @brief
///    ����Ʒ����������9-2�Ƿ����
///
/// @return
///     ���޷���true
///
bool CDataStatusWatch::_GetQualityTestFlag( uint32_t nQuality )
{
    
    if (nQuality&SMV_9_2_QUOLITY_TESTFLAG_MASK)
    {
        return true;
    }
    return false;
}

///
/// @brief
///    ����Ʒ����������9-2�Ƿ�ϳ�
///
/// @return
///     �ϳɷ���true
///
bool CDataStatusWatch::_GetQualityDerived( uint32_t nQuality )
{
    if (nQuality&SMV_9_2_QUOLITY_DERIVED_MASK)
    {
        return true;
    }
    return false;
}

///
/// @brief
///    ��ȡFT3״̬��
///
bool CDataStatusWatch::GetFt3StatusWords( uint16_t& nStatusWrod1, uint16_t& nStatusWord2 )
{
    return m_cSmvCtrBlkQuality.GetCtrlBlkStatusWords(nStatusWrod1, nStatusWord2);
}

///
/// @brief
///    ��ȡft3/9-1���ϱ�����Ϣ
///
/// @return
///     int32_t 0: ���ã� 1������򱨾���Ҫ��ά�ޣ�
///
int32_t CDataStatusWatch::GetFt3FaultAlarm()
{
    uint16_t nStatusWord1, nStatusWord2;
    if (m_cSmvCtrBlkQuality.GetCtrlBlkStatusWords(nStatusWord1, nStatusWord2))
    {
        if (nStatusWord1&FT3_STATUS_WORD_NEED_REPAIR)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    return -1;
}

///
/// @brief
///    ��ȡFT3/9-1�Ƿ����
///                      
/// @return
///     
int32_t CDataStatusWatch::GetFt3TestFlag()
{
    uint16_t nStatusWord1, nStatusWord2;
    if (m_cSmvCtrBlkQuality.GetCtrlBlkStatusWords(nStatusWord1, nStatusWord2))
    {
        if (nStatusWord1&FT3_STATUS_WORD_LLNO_MODE)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    return -1;
}

///
/// @brief
///    ��ȡ���ѽ�ͨ��Ϣ
///
/// @return
///     int32_t 0: ��ͨ����������),������Ч�� 1������ʱ�䣬������Ч
///
int32_t CDataStatusWatch::GetFt3WakeupValid()
{
    uint16_t nStatusWord1, nStatusWord2;
    if (m_cSmvCtrBlkQuality.GetCtrlBlkStatusWords(nStatusWord1, nStatusWord2))
    {
        if (nStatusWord1&FT3_STATUS_WORD_WAKEUP_TIME_VALIDITY)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    return -1;
}
