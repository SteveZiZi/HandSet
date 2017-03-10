/// @file
///
/// @brief
///     管理Ft3数据接收缓冲区，获取缓冲区数据信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.3.27
///
/// 修改历史：
///

#pragma once
#include "src\include\stdint.h"
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\include\datatypes.h"
#include "FrameRecvStruct.h"

class CFt3RecvBuffer
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
    CFt3RecvBuffer(uint8_t* pData, uint32_t nFrameCount, uint32_t nBufferLen);
    ~CFt3RecvBuffer(void);

public:
    ///
    /// @brief
    ///    获取缓冲区数据帧数目
    ///
    uint32_t GetBufferFrameCount();

    ///
    /// @brief
    ///    获取缓冲区数据帧类型
    ///
    enumFrameType GetBufferFrameType();

    ///
    /// @brief
    ///    获取缓冲区一帧数据帧长度
    ///
    uint32_t GetBufferAFrameLen();

    ///
    /// @brief
    ///    根据缓冲区头信息获取单个Ft3数据帧长度
    ///
    static uint32_t GetBufferAFrameLen(BufferHeader* pBufferHeader);

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
    ///     nFrameLen  保存缓冲区中一个数据帧的长度
    ///
    /// @return
    ///     true表示数据帧获取成功，false表示失败
    ///
    bool GetBufferFrameContent(uint32_t nIndex, uint8_t *pBuffer, uint32_t nBufferLen, uint32_t& nFrameLen);

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
    ///    获取一个数据帧时间戳信息
    ///
    /// @param[in] 
    ///     nIndex  要获取的数据帧的序号，从0开始
    /// @param[out]
    ///     stTimeStamp 保存要获取的数据帧的时间信息
    ///
    /// @return
    ///     true表示时间戳获取成功，false表示失败
    ///
    bool GetBufferFrameTimeStamp(uint32_t nIndex, UTC_TIME_t& stTimeStamp);

    ///
    /// @brief
    ///    获取一个数据帧时间戳信息
    ///
    /// @param[in] 
    ///     nIndex  要获取的数据帧的序号，从0开始
    /// @param[out]
    ///     stFrameTimeStamp 保存要获取的数据帧的时间信息
    ///
    /// @return
    ///     true表示时间戳获取成功，false表示失败
    ///
    bool GetBufferFrameTimeStamp(uint32_t nIndex, FrameTimeStamp& stFrameTimeStamp);

    ///
    /// @brief
    ///    获取缓冲区数据帧的波特率
    ///
    FT3_BAUD_RATE GetBufferFrameBaudRate();

    ///
    /// @brief
    ///    获取缓冲区数据的采样率
    ///
    uint32_t GetBufferFrameSampleRate();

    ///
    /// @brief
    ///    获取是否超时
    ///
    /// @return
    ///     超时返回true
    ///
    bool IsTimeOut();

private:
    friend class CFrameRecv;
    ///
    /// @brief
    ///    设置缓冲区头部信息
    ///
    void SetBufferHeader(const BufferHeader& rBufferHeader);

private:
    uint32_t        m_nRecvBufferLen;                   ///< 数据缓冲区长度
    uint8_t*        m_pDataBuffer;                      ///< 指向内部实际数据的地址指针
    uint32_t        m_nFrameCount;                      ///< 缓冲区数据帧数目
    BufferHeader    m_stBufferHeader;                   ///< 保存缓冲区头部信息
};
