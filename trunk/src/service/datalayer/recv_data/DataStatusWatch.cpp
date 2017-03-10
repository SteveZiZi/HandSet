/// @file
///
/// @brief
///     描述一个控制块的通道有效性，是否同步等信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.4.23
///
/// 修改历史：
///

#include "StdAfx.h"
#include "DataStatusWatch.h"
#include "src\service\datalayer\config\SmvRecvConfig.h"

#define SMV_9_2_QUOLITY_VALIDITY_MASK    0x00000003        ///< 9-2采样值的品质因数最低两位表示十分有效，00有效
#define SMV_9_2_QUOLITY_TESTFLAG_MASK    0x00000800        ///< 9-2采样值的品质因数检修标志，0表示没有检修
#define SMV_9_2_QUOLITY_DERIVED_MASK     0x00002000        ///< 9-2采样值的品质因数合成标志，0表示非合成量

CDataStatusWatch::CDataStatusWatch(void)
{
}

CDataStatusWatch::~CDataStatusWatch(void)
{
}

///
/// @brief
///    获取数据帧类型
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
///    获取通道品质因数
///
/// @param[in]  nIndex  通道索引号
/// @param[out] nQuality 保存获取的品质因数
///
/// @return
///     bool    获取品质因数成功返回true
///
bool CDataStatusWatch::GetOneChannelQuality( uint32_t nIndex, uint32_t& nQuality )
{
    return m_cSmvCtrBlkQuality.GetChannelQuality(nIndex, nQuality);
}

///
/// @brief
///    获取通道是否有效
///
/// @param[in]  nIndex  通道索引号
///
/// @return
///     bool    返回通道数据是否有效，true标识有效
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
///    设置控制块信息
///
void CDataStatusWatch::SetCtrlBlk( CBaseDetectItem* pDetectItem )
{
    m_pCtrlBlk = pDetectItem;
    m_cSmvCtrBlkQuality.SetCtrlBlk(pDetectItem);
}

///
/// @brief
///    设置mu是否同步
///
void CDataStatusWatch::SetMuSync( bool bSync )
{
    m_cSmvCtrBlkQuality.SetMuSync(bSync);
}

///
/// @brief
///    设置通道品质因数,针对9-2
///
bool CDataStatusWatch::SetChannelQuality( uint32_t nChannelIndex, uint32_t nQuality )
{
    return m_cSmvCtrBlkQuality.SetChannelQuality(nChannelIndex, nQuality);
}

///
/// @brief
///    设置控制块状态字，针对ft3/9-1
///
bool CDataStatusWatch::SetCtrlBlkStatusWords( uint16_t nStatusWord1, uint16_t nStatusWord2 )
{
    return m_cSmvCtrBlkQuality.SetCtrlBlkStatusWords(nStatusWord1, nStatusWord2);
}

///
/// @brief
///    返回一个通道信息
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
///    根据品质因数返回9-2是否检修
///
/// @return
///     检修返回true
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
///    根据品质因数返回9-2是否合成
///
/// @return
///     合成返回true
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
///    获取FT3状态字
///
bool CDataStatusWatch::GetFt3StatusWords( uint16_t& nStatusWrod1, uint16_t& nStatusWord2 )
{
    return m_cSmvCtrBlkQuality.GetCtrlBlkStatusWords(nStatusWrod1, nStatusWord2);
}

///
/// @brief
///    获取ft3/9-1故障报警信息
///
/// @return
///     int32_t 0: 良好； 1：警告或报警（要求维修）
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
///    获取FT3/9-1是否检修
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
///    获取唤醒接通信息
///
/// @return
///     int32_t 0: 接通（正常运行),数据有效； 1：唤醒时间，数据无效
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
