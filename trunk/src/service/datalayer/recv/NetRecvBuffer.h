/// @file
///
/// @brief
///     网络数据接收缓冲区信息获取
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.3.26
///
/// 修改历史：
///

#pragma once

#include <map>
#include "src\include\stdint.h"
#include "FrameRecvStruct.h"
#include <WinSock2.h>

#pragma comment(lib,"Ws2.lib")

//以太网类型值在数据帧中的位置（偏移值）,如果有vlan，则需要加上4个字节的偏移
static const uint32_t ETHERNET_TYPE_INDEX = 12;
static const uint32_t ETHERNET_9_1_2_TAG_INDEX = 22;
static const uint32_t ETHERNET_VLAN_OFFSET = 4;

static const uint16_t ETHERNET_TYPE_GOOSE = 0x88B8;
static const uint16_t ETHERNET_TYPE_9_1_2 = 0x88BA;
static const uint16_t ETHERNET_TYPE_VLAN = 0x8100;
static const uint8_t  TAG_9_1 = 0x80;
static const uint8_t  TAG_9_2 = 0x60;

class CNetRecvBuffer
{
public:
    ///
    /// @brief
    ///    构造函数
    /// @param[in] 
    ///     pData 数据缓冲区地址
    /// @param[in]
    ///     nBufferLen 缓冲区大小
    ///
    CNetRecvBuffer(uint8_t* pData, uint32_t nFrameCount, uint32_t nBufferLen);
    ~CNetRecvBuffer(void);


public:
    ///
    /// @brief
    ///    获取缓冲区数据帧数目
    ///
    uint32_t GetBufferFrameCount();

    ///
    /// @brief
    ///    获取某个数据帧的信息
    ///
    bool GetBufferFrameInfo(uint32_t nIndex, FrameInfo& stFrameInfo);

    ///
    /// @brief
    ///    获取一个数据帧内容
    ///
    /// @param[in] 
    ///     nIndex  要获取的数据帧的序号，从0开始
    /// @param[in]
    ///     pBuffer 保存要获取的数据帧的内容
    /// @param[in] 
    ///     nBufferLen 缓冲区的大小
    /// @param[out]
    ///     nFrameLen   实际数据帧的长度
    ///
    /// @return
    ///     true表示数据帧获取成功，false表示失败
    ///
    bool GetBufferFrameContent(uint32_t nIndex, uint8_t *pBuffer, uint32_t nBufferLen, uint32_t &nFrameLen);

    ///
    /// @brief
    ///    获取一个数据帧的地址，不复制数据帧内容出来
    ///
    /// @param[in] 
    ///     nIndex  要获取的数据帧的序号，从0开始
    /// @param[out]
    ///     pFrameAddr 保存要获取的数据帧的地址
    /// @param[out]
    ///     nFrameLen  实际数据帧的长度
    ///
    /// @return
    ///     true表示数据帧地址获取成功，false表示失败
    ///
    bool GetBufferFrameAddr(uint32_t nIndex, uint8_t** pFrameAddr, uint32_t& nFrameLen);

    ///
    /// @brief
    ///    获取一个数据帧的类型
    ///
    static enumFrameType GetFrameType(uint8_t* pFrame, uint32_t nFrameLen);

private:
    ///
    /// @brief
    ///    分析缓冲区数据，获取各种信息
    ///
    void _AnalisysBuffer();

private:
    uint8_t*        m_pDataBuffer;                      ///< 指向共享内存内部实际数据的地址指针
    uint32_t        m_nRecvBufferLen;                   ///< 共享内存数据缓冲区总长度
    bool            m_bAnalysised;                      ///< 缓冲区的数据是否被解析过
    NET_MODE        m_enNetMode;                        ///< 获取当前缓冲区网络模式，分析是探测还是过滤后的结果
    enumFrameType   m_enFrameType;                      ///< 缓冲区中数据类型
    uint32_t        m_nFrameCount;                      ///< 数据帧数目
    FrameInfo*      m_pArrayFrameInfo;                  ///< 保存数据帧的位置，长度时间等信息
};
