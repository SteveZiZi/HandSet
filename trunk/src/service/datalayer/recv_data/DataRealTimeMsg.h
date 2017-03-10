/// @file
///
/// @brief
///     ��ȡʵʱ������Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.30
///
/// �޸���ʷ��
///


#pragma once
#include "src\service\datalayer\include\config.h"

class CDataRealTimeMsgData
{
public:
    CBaseDetectItem*    m_pCtrlBlk;                                     ///< ��ǰ����֡��Ӧ�Ŀ��ƿ�
    enumFrameType       m_enFrameType;                                  ///< ��ǰ����֡����
    uint32_t            m_nFrameLen;                                    ///< ����֡ʵ�ʳ���
    uint8_t             m_nDataBuffer[NET_MAX_FRAME_LEN];               ///< ����һ������֡�Ļ�����

    CDataRealTimeMsgData(): m_enFrameType(FRAME_TYPE_UNKOWN)
                          , m_nFrameLen(0)
                          , m_pCtrlBlk(NULL)
    {
        memset(m_nDataBuffer, 0, sizeof(m_nDataBuffer));
    }
};