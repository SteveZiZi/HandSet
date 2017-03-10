/// @file
///     goose_types.h
///
/// @brief
///     定义Goose报文帧编解码过程中使用的数据类型
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.3.18
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.06.06     |1.0.0.606  |增加Goose数据类型的Copy接口                               |
///    |2013.06.07     |1.0.0.607  |将有/无符号整型类型细化为8、16、24、32位整型              |
///    |2013.06.07     |1.0.0.607  |更改Float类型的解析                                       |
///
#ifndef GOOSE_TYPES_H_

#define GOOSE_TYPES_H_

#include <string>
#include <vector>
#include "src\include\stdint.h"

///
/// @brief
///     UTC时间结构体
///
typedef struct tagEventTime
{
    uint32_t ts_s;                                                      ///< 时间戳的秒数部分
    uint32_t ts_ns;                                                     ///< 时间戳的纳秒数部分，即小数部分
    uint8_t  ts_q;                                                      ///< 时间戳的品质

    ///
    /// @brief
    ///    获得UTC时间字符串，格式为HH:MM:SS.MMM
    ///
    /// @return
    ///    std::wstring    UTC时间字符串
    ///
    std::wstring GetString();

    ///
    /// @brief
    ///     减法运算符重载
    ///
    /// @param[in]  stOldTime 老时间对象
    ///
    /// @return
    ///     tagEventTime    时间对象差
    ///
    tagEventTime operator-(const tagEventTime& stOldTime);

    ///
    /// @brief
    ///     加法运算符重载
    ///
    /// @param[in]  stAddTime 增加的时间
    ///
    /// @return
    ///     tagEventTime    时间对象和
    ///
    tagEventTime operator+(const tagEventTime& stAddTime);

	///
	/// @brief
	///     编码GOOSE报文中的使用ASN.1的TLV格式的GOOSE数据发生变位的时间信息
	///
	/// @param[in]
	///     pCodeDataBuf - 保存编码结果的缓存
	/// @param[in]
	///     bufSize      - 缓存大小
	///
	/// @return
	///     编码结果的实际大小
	///
	size_t CodeGooseEventTime(uint8_t *pCodeDataBuf, size_t bufSize);
}CEventTime;

///
/// @brief
///     符合Goose标准的GoosePDU中应用服务数据单元（ASDU）中除通道数据外的公共信息
///
typedef struct
{
	std::string  gocbRef;                  ///< GOOSE控制块引用，length≤65 byte
	uint32_t     timeAllowedtoLive;        ///< 报文允许生存时间，一般为心跳时间T0值的2倍，length≤4 byte
	std::string  datSet;                   ///< GOOSE控制块所控制的GOOSE数据集的引用，length≤65 byte
	std::string  goID;                     ///< 每个GOOSE报文的唯一性标识，默认值为GOOSE控制块引用，length≤65 byte
	CEventTime   eventTm;                  ///< GOOSE数据发生变位的时间，UTC时间，报文中共占8 Byte
	uint32_t     stNum;                    ///< 状态序号，记录GOOSE数据发生变位的总次数，length≤4 byte
	uint32_t     sqNum;                    ///< 顺序号，用于记录稳态情况下报文发出的帧数，length≤4 byte
	uint8_t      test;                     ///< 检修标识，用于表示发出该GOOSE报文的装置是否处于检修状态
	uint32_t     confRev;                  ///< 配制版本号，代表GOOSE数据集配置被改变的次数，length≤4 byte
	uint8_t      ndsCom;                   ///< 用于指示GOOSE是否需要重新配置
	uint32_t     numDatSetEntries;         ///< 数据集成员的个数，length≤4 byte
	std::string  security;                 ///< 用于对报文进行加密，以保证信息传输的安全,可选项
}GOOSE_PDU_PUBLIC;

///
/// @brief
///     符合Goose标准的GoosePDU中应用服务数据单元（ASDU）中除通道数据外的公共信息的扩展类型
///     该类型中协议规定length≤4 byte的字段改为uint64_t类型
///
typedef struct
{
	std::string  gocbRef;                  ///< GOOSE控制块引用，length≤65 byte
	uint64_t     timeAllowedtoLive;        ///< 报文允许生存时间，一般为心跳时间T0值的2倍，length≤4 byte
	std::string  datSet;                   ///< GOOSE控制块所控制的GOOSE数据集的引用，length≤65 byte
	std::string  goID;                     ///< 每个GOOSE报文的唯一性标识，默认值为GOOSE控制块引用，length≤65 byte
	CEventTime   eventTm;                  ///< GOOSE数据发生变位的时间，UTC时间，报文中共占8 Byte
	uint64_t     stNum;                    ///< 状态序号，记录GOOSE数据发生变位的总次数，length≤4 byte
	uint64_t     sqNum;                    ///< 顺序号，用于记录稳态情况下报文发出的帧数，length≤4 byte
	uint8_t      test;                     ///< 检修标识，用于表示发出该GOOSE报文的装置是否处于检修状态
	uint64_t     confRev;                  ///< 配制版本号，代表GOOSE数据集配置被改变的次数，length≤4 byte
	uint8_t      ndsCom;                   ///< 用于指示GOOSE是否需要重新配置
	uint64_t     numDatSetEntries;         ///< 数据集成员的个数，length≤4 byte
	std::string  security;                 ///< 用于对报文进行加密，以保证信息传输的安全,可选项
}GOOSE_PDU_PUBLIC_EX;

///
/// @brief
///     GoosePDU中应用服务数据单元（ASDU）中除通道数据外的公共信息在报文帧中的偏移地址信息
///
typedef struct
{
	size_t gocbRefOff;                     ///< GOOSE控制块引用的偏移地址
	size_t timeAllowedtoLiveOff;           ///< 报文允许生存时间的偏移地址
	size_t datSetOff;                      ///< GOOSE控制块所控制的GOOSE数据集的引用的偏移地址
	size_t goIDOff;                        ///< 每个GOOSE报文的唯一性标识的偏移地址
	size_t eventTmOff;                     ///< GOOSE数据发生变位的时间的偏移地址
	size_t stNumOff;                       ///< 状态序号的偏移地址
	size_t sqNumOff;                       ///< 顺序号的偏移地址
	size_t testOff;                        ///< 检修标识的偏移地址
	size_t confRevOff;                     ///< 配制版本号的偏移地址
	size_t ndsComOff;                      ///< 用于指示GOOSE是否需要重新配置的标识的偏移地址
	size_t numDatSetEntriesOff;            ///< 数据集成员的个数的偏移地址
	size_t securityOff;                    ///< 安全项的偏移地址
}GOOSE_PDU_PUBLIC_OFFSET;

///
/// @brief
///     GOOSE报文中数据序列（AllData）常用的数据类型
///
typedef enum {
	GSEDT_UNKNOW = 0,
	GSEDT_BOOLEAN,
	GSEDT_BIT_STR,
	GSEDT_INT8,
	GSEDT_INT16,
	GSEDT_INT24,
	GSEDT_INT32,
	GSEDT_UINT8,
	GSEDT_UINT16,
	GSEDT_UINT24,
	GSEDT_UINT32,
	GSEDT_FLOAT,
	GSEDT_OCTET_STR,
	GSEDT_VISIBLE_STR,
	GSEDT_UTC_TIME,
	GSEDT_STRUCT
}GOOSE_DATA_TYPE;

///
/// @brief
///     GOOSE报文中数据序列（AllData）常用的数据类型数据的父结构体
///
typedef struct {
    GOOSE_DATA_TYPE type;                                               ///< GOOSE通道的数据类型
    size_t          offset;                                             ///< 该数据在报文帧中的偏移地址
}GOOSE_DATA, *PGOOSE_DATA;

///
/// @brief
///     未知类型数据
///
typedef struct {
    GOOSE_DATA id;                                                      ///< 数据标识
    uint8_t    gooseTag;                                                ///< GOOSE报文中的Tag值
    uint32_t   datLen;                                                  ///< 数据的字节数
    uint8_t   *datVal;                                                  ///< 指向数据的字节型指针
}GOOSE_UNKNOW;

///
/// @brief
///     GOOSE报文中数据序列（AllData）的Boolean类型数据
///
typedef struct {
    GOOSE_DATA id;                                                      ///< 数据标识
    bool       boolVal;                                                 ///< 布尔值
}GOOSE_BOOLEAN;

///
/// @brief
///     双点值索引
///
enum eDPValueIndex
{
    DP_VALUE_INIT = 0,
    DP_VALUE_OFF,
    DP_VALUE_ON,
    DP_VALUE_BAD
};

///
/// @brief
///     双点值数组
///
static const std::wstring strDPValue[] = 
{
    L"00",                                                              ///< 双点值init
    L"01",                                                              ///< 双点值off
    L"10",                                                              ///< 双点值on
    L"11"                                                               ///< 双点值bad
};

///
/// @brief
///     GOOSE报文中数据序列（AllData）的Bit-String（位串）类型数据
///
typedef struct tagGooseBitString{
	GOOSE_DATA id;                                                      ///< 数据标识
	uint8_t    padding;                                                 ///< 位串中补位的数目，补位从低位开始
	uint32_t   strSize;                                                 ///< 位串数据的字节数
	uint8_t   *bitStr;                                                  ///< 指向位串数据的字节型指针

    ///
    /// @brief
    ///     设置位串
    ///
    /// @param[in]  strNew  新的位串字符串
    ///
    /// @return  
    ///     void    
    ///
    void SetString(std::wstring strNew);

    ///
    /// @brief
    ///     获得位串字符串
    ///
    /// @return
    ///     std::wstring    位串字符串
    ///
    std::wstring GetString();
}GOOSE_BIT_STRING;

///
/// @brief
///     GOOSE报文中数据序列（AllData）的8位Integer（整数）类型数据
///
typedef struct {
	GOOSE_DATA id;                                                      ///< 数据标识
	int8_t     datVal;                                                  ///< 8位Integer（整数）类型数据
}GOOSE_INT8;

///
/// @brief
///     GOOSE报文中数据序列（AllData）的16位Integer（整数）类型数据
///
typedef struct {
    GOOSE_DATA id;                                                      ///< 数据标识
    int16_t    datVal;                                                  ///< 16位Integer（整数）类型数据
}GOOSE_INT16;

///
/// @brief
///     GOOSE报文中数据序列（AllData）的24位Integer（整数）类型数据
///
typedef struct {
    GOOSE_DATA id;                                                      ///< 数据标识
    int32_t    datVal;                                                  ///< 24位Integer（整数）类型数据
}GOOSE_INT24;

///
/// @brief
///     GOOSE报文中数据序列（AllData）的32位Integer（整数）类型数据
///
typedef struct {
    GOOSE_DATA id;                                                      ///< 数据标识
    int32_t    datVal;                                                  ///< 32位Integer（整数）类型数据
}GOOSE_INT32;

///
/// @brief
///     GOOSE报文中数据序列（AllData）的8位Unsigned（无符号整数）类型数据
///
typedef struct {
    GOOSE_DATA id;                                                      ///< 数据标识
    uint8_t    datVal;                                                  ///< 8位Unsigned（无符号整数）类型数据
}GOOSE_UINT8;

///
/// @brief
///     GOOSE报文中数据序列（AllData）的16位Unsigned（无符号整数）类型数据
///
typedef struct {
    GOOSE_DATA id;                                                      ///< 数据标识
    uint16_t   datVal;                                                  ///< 16位Unsigned（无符号整数）类型数据
}GOOSE_UINT16;

///
/// @brief
///     GOOSE报文中数据序列（AllData）的24位Unsigned（无符号整数）类型数据
///
typedef struct {
    GOOSE_DATA id;                                                      ///< 数据标识
    uint32_t   datVal;                                                  ///< 24位Unsigned（无符号整数）类型数据
}GOOSE_UINT24;

///
/// @brief
///     GOOSE报文中数据序列（AllData）的32位Unsigned（无符号整数）类型数据
///
typedef struct {
    GOOSE_DATA id;                                                      ///< 数据标识
    uint32_t   datVal;                                                  ///< 32位Unsigned（无符号整数）类型数据
}GOOSE_UINT32;

///
/// @brief
///     GOOSE报文中数据序列（AllData）的Float（整数）类型数据
///     有32位和64位两种，对应C/C++的float和double
///     该类型为符合IEEE754的单精度浮点数，格式：符号位1位，阶码8位，尾数23位
///
typedef struct {
    GOOSE_DATA id;                                                      ///< 数据标识
    float      datVal;                                                  ///< 32位单精度浮点数
}GOOSE_FLOAT;

///
/// @brief
///     GOOSE报文中数据序列（AllData）的Octet-String（8位位组串）类型数据
///
typedef struct {
    GOOSE_DATA id;                                                      ///< 数据标识
    uint32_t   strSize;                                                 ///< 位组串数据的字节数
    uint8_t   *octetStr;                                                ///< 指向位组串数据的指针
}GOOSE_OCTET_STRING;

///
/// @brief
///     GOOSE报文中数据序列（AllData）的Visible-String（可视串）类型数据
///     一般为ASCII文本数据
///
typedef struct {
    GOOSE_DATA id;                                                      ///< 数据标识
    uint32_t   strSize;                                                 ///< 可视串数据的字节数
    int8_t    *visibleStr;                                              ///< 指向可视串数据的指针
}GOOSE_VISIBLE_STRING;

///
/// @brief
///     GOOSE报文中数据序列（AllData）的IEC61850定义的UTC-TIME类型数据
///
typedef struct tagGooseUtcTime{
    GOOSE_DATA id;                                                      ///< 数据标识
    uint32_t   t_s;                                                     ///< 时间戳的秒数部分
    uint32_t   t_ns;                                                    ///< 时间戳的纳秒数部分，即小数部分
    uint8_t    t_q;                                                     ///< 时间品质

    ///
    /// @brief
    ///    获得UTC时间
    ///
    ///
    /// @return
    ///     SYSTEMTIME    UTC时间
    ///
    SYSTEMTIME GetUtcTime();

    ///
    /// @brief
    ///    获得UTC时间字符串
    ///
    ///
    /// @return
    ///     std::wstring    UTC时间字符串
    ///
    std::wstring GetUtcString();

    ///
    /// @brief
    ///    获得指定时间距离1970-01-01 00:00:00的秒数
    ///
    /// @param[in]  st    指定的时间结构体
    ///
    /// @return
    ///     time_t    距今的秒数
    ///
    time_t SystemTimeToTimet( SYSTEMTIME st );

    ///
    /// @brief
    ///    设置UTC时间
    ///
    /// @param[in]  sysTime    UTC时间
    ///
    void SetUtcTime(SYSTEMTIME sysTime);

    ///
    /// @brief
    ///    设置UTC时间
    ///
    /// @param[in]  strTime    UTC时间字符串（年：月：日 时：分：秒.毫秒）
    ///
    void SetUctTime(std::wstring strTime);
}GOOSE_UTC_TIME;

///
/// @brief
///     GOOSE报文中数据序列（AllData）的IEC61850定义的Struct类型数据
///
typedef struct {
    GOOSE_DATA id;                                                      ///< 数据标识
    std::vector<GOOSE_DATA *> member;                                   ///< 结构体成员
}GOOSE_STRUCT;

///
/// @brief
///     复制GOOSE_DATA类型数据
///
/// @param[in]
///     pGseData - 指向要复制的GooseData的指针
///
/// @return
///     指向新的GooseData的指针
///
GOOSE_DATA *GooseDataCopy(GOOSE_DATA *pGseData);

///
/// @brief
///     释放创建的GooseData
///
/// @param[in]
///     pGseData - 指向GooseData的指针
///
/// @return
///     
///
void GooseDataFree(GOOSE_DATA *pGseData);

///
/// @brief
///     获取GooseData的TLV形式所占用的字节数(Tag(1字节)+Len(1~5字节)+Val)
///
/// @param[in]
///     pGseData - 指向GooseData的指针
///
/// @return
///     GooseData的TLV形式所占用的字节数
///
size_t GooseDataTLVBytes(const GOOSE_DATA *pGseData);

///
/// @brief
///     GOOSE通道值万能变量
///
struct GOOSE_DATA_VARIANT
{
    union
    {
        GOOSE_UNKNOW            m_stUnKnown;
        GOOSE_BOOLEAN           m_stBoolean;
        GOOSE_BIT_STRING        m_stBitString;
        GOOSE_INT8              m_stInt8;
        GOOSE_INT16             m_stInt16;
        GOOSE_INT24             m_stInt24;
        GOOSE_INT32             m_stInt32;
        GOOSE_UINT8             m_stUInt8;
        GOOSE_UINT16            m_stUInt16;
        GOOSE_UINT24            m_stUInt24;
        GOOSE_UINT32            m_stUInt32;
        GOOSE_FLOAT             m_stFloat;
        GOOSE_OCTET_STRING      m_stOctetString;
        GOOSE_VISIBLE_STRING    m_stVisibleString;
        GOOSE_UTC_TIME          m_stUtcTime;
    };

    ///
    /// @brief
    ///     默认构造函数
    ///
    GOOSE_DATA_VARIANT();

    ///
    /// @brief
    ///     拷贝构造函数
    ///
    GOOSE_DATA_VARIANT(const GOOSE_DATA_VARIANT& stGooseDataVariant);

    ///
    /// @brief
    ///    构造函数
    ///
    /// @param[in]  pstGooseData    指向GOOSE_DATA对象的指针
    ///
    GOOSE_DATA_VARIANT(GOOSE_DATA* pstGooseData);

    ///
    /// @brief
    ///     析构函数
    ///
    virtual ~GOOSE_DATA_VARIANT()
    {
        __ReleaseAllData();
    }

    ///
    /// @brief
    ///    释放所有已申请的空间
    ///
    ///
    /// @return
    ///     void    
    ///
    void __ReleaseAllData();

    ///
    /// @brief
    ///    重载赋值操作符
    ///
    /// @param[in]  stGooseDataVariant    GOOSE通道值万能变量的引用
    ///
    /// @return
    ///     GOOSE_DATA_VARIANT&    GOOSE通道值万能变量的引用
    ///
    GOOSE_DATA_VARIANT& operator=(const GOOSE_DATA_VARIANT& stGooseDataVariant);

    ///
    /// @brief
    ///     不等于
    ///
    bool operator !=(GOOSE_DATA_VARIANT& stGooseDataVariant) const;

    ///
    /// @brief
    ///    获得数据类型
    ///
    ///
    /// @return
    ///     GOOSE_DATA_TYPE    数据类型
    ///
    GOOSE_DATA_TYPE GetType()
    {
        return m_stUnKnown.id.type;
    }

    ///
    /// @brief
    ///    设置数据类型
    ///
    /// @param[in]  eNewDataType    新的数据类型
    ///
    void __SetType(GOOSE_DATA_TYPE eNewDataType);

    ///
    /// @brief
    ///    构造函数
    ///
    /// @param[in]  stBoolean    GOOSE报文中数据序列（AllData）的Boolean类型数据
    ///
    GOOSE_DATA_VARIANT(GOOSE_BOOLEAN& stBoolean)
    {
        memset(this, 0, sizeof(GOOSE_DATA_VARIANT));
        m_stBoolean         = stBoolean;
    }

    ///
    /// @brief
    ///    类型转换
    ///
    /// @return
    ///    GOOSE_BOOLEAN    GOOSE报文中数据序列（AllData）的Boolean类型数据
    ///
    operator GOOSE_BOOLEAN() const
    {
        return m_stBoolean;
    }

    ///
    /// @brief
    ///    构造函数
    ///
    /// @param[in]  stInt8    GOOSE报文中数据序列（AllData）的8位Integer（整数）类型数据
    ///
    GOOSE_DATA_VARIANT(GOOSE_INT8& stInt8)
    {
        memset(this, 0, sizeof(GOOSE_DATA_VARIANT));
        m_stInt8            = stInt8;
    }

    ///
    /// @brief
    ///    构造函数
    ///
    /// @param[in]  stBitString    GOOSE报文中数据序列（AllData）的位串类型数据
    ///
    GOOSE_DATA_VARIANT(GOOSE_BIT_STRING& stBitString)
    {
        memset(this, 0, sizeof(GOOSE_DATA_VARIANT));

        // 深度拷贝
        m_stBitString           = stBitString;
        m_stBitString.bitStr    = new uint8_t[m_stBitString.strSize];
        if (m_stBitString.bitStr)
        {
            memset(m_stBitString.bitStr, 0, m_stBitString.strSize);
            memcpy_s(
                m_stBitString.bitStr,
                m_stBitString.strSize,
                stBitString.bitStr,
                stBitString.strSize
                );
        }
    }

    ///
    /// @brief
    ///    类型转换
    ///
    /// @return
    ///    GOOSE_INT8    GOOSE报文中数据序列（AllData）的8位Integer（整数）类型数据
    ///
    operator GOOSE_INT8() const
    {
        return m_stInt8;
    }

    ///
    /// @brief
    ///    获得字符串
    ///
    /// @return
    ///     std::wstring    字符串
    ///
    std::wstring GetString();

	///
	/// @brief
	///     编码ASN.1的TLV格式数据中的Len部分
	///
	/// @param[in]
	///     eNewDataType - 新的数据类型
	/// @param[in]
	///     pCodeDataBuf - 保存编码结果的缓存
	/// @param[in]
	///     bufSize      - 缓存大小
	///
	/// @return
	///     编码结果的实际大小
	///
	size_t CodeLenOfTLV(size_t valLenOfTLV, uint8_t *pCodeDataBuf, size_t bufSize);

	///
	/// @brief
	///     编码GOOSE报文中的使用ASN.1的TLV格式的基本类型数据
	///
	/// @param[in]
	///     pCodeDataBuf - 保存编码结果的缓存
	/// @param[in]
	///     bufSize      - 缓存大小
	///
	/// @return
	///     编码结果的实际大小
	///
	size_t CodeGooseData(uint8_t *pCodeDataBuf, size_t bufSize);
};

///
/// @brief
///     编码GOOSE报文中的使用ASN.1的TLV格式的StNum字段，此处编码时stNum长度固定为4字节
///
/// @param[in]
///     stNum        - stNum的值
/// @param[in]
///     pCodeDataBuf - 保存编码结果的缓存
/// @param[in]
///     bufSize      - 缓存大小
///
/// @return
///     编码结果的实际大小
///
size_t CodeGooseStNum(uint32_t stNum, uint8_t *pCodeDataBuf, size_t bufSize);

#endif
