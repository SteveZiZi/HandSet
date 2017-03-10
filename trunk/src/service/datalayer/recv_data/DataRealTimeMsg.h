/// @file
///
/// @brief
///     获取实时报文信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.30
///
/// 修改历史：
///


#pragma once
#include "src\service\datalayer\include\config.h"

class CDataRealTimeMsgData
{
public:
    CBaseDetectItem*    m_pCtrlBlk;                                     ///< 当前数据帧对应的控制块
    enumFrameType       m_enFrameType;                                  ///< 当前数据帧类型
    uint32_t            m_nFrameLen;                                    ///< 数据帧实际长度
    uint8_t             m_nDataBuffer[NET_MAX_FRAME_LEN];               ///< 保存一个数据帧的缓冲区

    CDataRealTimeMsgData(): m_enFrameType(FRAME_TYPE_UNKOWN)
                          , m_nFrameLen(0)
                          , m_pCtrlBlk(NULL)
    {
        memset(m_nDataBuffer, 0, sizeof(m_nDataBuffer));
    }
};