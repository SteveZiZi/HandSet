/// @file
///     goose_types.h
///
/// @brief
///     ����Goose����֡����������ʹ�õ���������
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.3.18
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.06.06     |1.0.0.606  |����Goose�������͵�Copy�ӿ�                               |
///    |2013.06.07     |1.0.0.607  |����/�޷�����������ϸ��Ϊ8��16��24��32λ����              |
///    |2013.06.07     |1.0.0.607  |����Float���͵Ľ���                                       |
///
#ifndef GOOSE_TYPES_H_

#define GOOSE_TYPES_H_

#include <string>
#include <vector>
#include "src\include\stdint.h"

///
/// @brief
///     UTCʱ��ṹ��
///
typedef struct tagEventTime
{
    uint32_t ts_s;                                                      ///< ʱ�������������
    uint32_t ts_ns;                                                     ///< ʱ��������������֣���С������
    uint8_t  ts_q;                                                      ///< ʱ�����Ʒ��

    ///
    /// @brief
    ///    ���UTCʱ���ַ�������ʽΪHH:MM:SS.MMM
    ///
    /// @return
    ///    std::wstring    UTCʱ���ַ���
    ///
    std::wstring GetString();

    ///
    /// @brief
    ///     �������������
    ///
    /// @param[in]  stOldTime ��ʱ�����
    ///
    /// @return
    ///     tagEventTime    ʱ������
    ///
    tagEventTime operator-(const tagEventTime& stOldTime);

    ///
    /// @brief
    ///     �ӷ����������
    ///
    /// @param[in]  stAddTime ���ӵ�ʱ��
    ///
    /// @return
    ///     tagEventTime    ʱ������
    ///
    tagEventTime operator+(const tagEventTime& stAddTime);

	///
	/// @brief
	///     ����GOOSE�����е�ʹ��ASN.1��TLV��ʽ��GOOSE���ݷ�����λ��ʱ����Ϣ
	///
	/// @param[in]
	///     pCodeDataBuf - ����������Ļ���
	/// @param[in]
	///     bufSize      - �����С
	///
	/// @return
	///     ��������ʵ�ʴ�С
	///
	size_t CodeGooseEventTime(uint8_t *pCodeDataBuf, size_t bufSize);
}CEventTime;

///
/// @brief
///     ����Goose��׼��GoosePDU��Ӧ�÷������ݵ�Ԫ��ASDU���г�ͨ��������Ĺ�����Ϣ
///
typedef struct
{
	std::string  gocbRef;                  ///< GOOSE���ƿ����ã�length��65 byte
	uint32_t     timeAllowedtoLive;        ///< ������������ʱ�䣬һ��Ϊ����ʱ��T0ֵ��2����length��4 byte
	std::string  datSet;                   ///< GOOSE���ƿ������Ƶ�GOOSE���ݼ������ã�length��65 byte
	std::string  goID;                     ///< ÿ��GOOSE���ĵ�Ψһ�Ա�ʶ��Ĭ��ֵΪGOOSE���ƿ����ã�length��65 byte
	CEventTime   eventTm;                  ///< GOOSE���ݷ�����λ��ʱ�䣬UTCʱ�䣬�����й�ռ8 Byte
	uint32_t     stNum;                    ///< ״̬��ţ���¼GOOSE���ݷ�����λ���ܴ�����length��4 byte
	uint32_t     sqNum;                    ///< ˳��ţ����ڼ�¼��̬����±��ķ�����֡����length��4 byte
	uint8_t      test;                     ///< ���ޱ�ʶ�����ڱ�ʾ������GOOSE���ĵ�װ���Ƿ��ڼ���״̬
	uint32_t     confRev;                  ///< ���ư汾�ţ�����GOOSE���ݼ����ñ��ı�Ĵ�����length��4 byte
	uint8_t      ndsCom;                   ///< ����ָʾGOOSE�Ƿ���Ҫ��������
	uint32_t     numDatSetEntries;         ///< ���ݼ���Ա�ĸ�����length��4 byte
	std::string  security;                 ///< ���ڶԱ��Ľ��м��ܣ��Ա�֤��Ϣ����İ�ȫ,��ѡ��
}GOOSE_PDU_PUBLIC;

///
/// @brief
///     ����Goose��׼��GoosePDU��Ӧ�÷������ݵ�Ԫ��ASDU���г�ͨ��������Ĺ�����Ϣ����չ����
///     ��������Э��涨length��4 byte���ֶθ�Ϊuint64_t����
///
typedef struct
{
	std::string  gocbRef;                  ///< GOOSE���ƿ����ã�length��65 byte
	uint64_t     timeAllowedtoLive;        ///< ������������ʱ�䣬һ��Ϊ����ʱ��T0ֵ��2����length��4 byte
	std::string  datSet;                   ///< GOOSE���ƿ������Ƶ�GOOSE���ݼ������ã�length��65 byte
	std::string  goID;                     ///< ÿ��GOOSE���ĵ�Ψһ�Ա�ʶ��Ĭ��ֵΪGOOSE���ƿ����ã�length��65 byte
	CEventTime   eventTm;                  ///< GOOSE���ݷ�����λ��ʱ�䣬UTCʱ�䣬�����й�ռ8 Byte
	uint64_t     stNum;                    ///< ״̬��ţ���¼GOOSE���ݷ�����λ���ܴ�����length��4 byte
	uint64_t     sqNum;                    ///< ˳��ţ����ڼ�¼��̬����±��ķ�����֡����length��4 byte
	uint8_t      test;                     ///< ���ޱ�ʶ�����ڱ�ʾ������GOOSE���ĵ�װ���Ƿ��ڼ���״̬
	uint64_t     confRev;                  ///< ���ư汾�ţ�����GOOSE���ݼ����ñ��ı�Ĵ�����length��4 byte
	uint8_t      ndsCom;                   ///< ����ָʾGOOSE�Ƿ���Ҫ��������
	uint64_t     numDatSetEntries;         ///< ���ݼ���Ա�ĸ�����length��4 byte
	std::string  security;                 ///< ���ڶԱ��Ľ��м��ܣ��Ա�֤��Ϣ����İ�ȫ,��ѡ��
}GOOSE_PDU_PUBLIC_EX;

///
/// @brief
///     GoosePDU��Ӧ�÷������ݵ�Ԫ��ASDU���г�ͨ��������Ĺ�����Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
typedef struct
{
	size_t gocbRefOff;                     ///< GOOSE���ƿ����õ�ƫ�Ƶ�ַ
	size_t timeAllowedtoLiveOff;           ///< ������������ʱ���ƫ�Ƶ�ַ
	size_t datSetOff;                      ///< GOOSE���ƿ������Ƶ�GOOSE���ݼ������õ�ƫ�Ƶ�ַ
	size_t goIDOff;                        ///< ÿ��GOOSE���ĵ�Ψһ�Ա�ʶ��ƫ�Ƶ�ַ
	size_t eventTmOff;                     ///< GOOSE���ݷ�����λ��ʱ���ƫ�Ƶ�ַ
	size_t stNumOff;                       ///< ״̬��ŵ�ƫ�Ƶ�ַ
	size_t sqNumOff;                       ///< ˳��ŵ�ƫ�Ƶ�ַ
	size_t testOff;                        ///< ���ޱ�ʶ��ƫ�Ƶ�ַ
	size_t confRevOff;                     ///< ���ư汾�ŵ�ƫ�Ƶ�ַ
	size_t ndsComOff;                      ///< ����ָʾGOOSE�Ƿ���Ҫ�������õı�ʶ��ƫ�Ƶ�ַ
	size_t numDatSetEntriesOff;            ///< ���ݼ���Ա�ĸ�����ƫ�Ƶ�ַ
	size_t securityOff;                    ///< ��ȫ���ƫ�Ƶ�ַ
}GOOSE_PDU_PUBLIC_OFFSET;

///
/// @brief
///     GOOSE�������������У�AllData�����õ���������
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
///     GOOSE�������������У�AllData�����õ������������ݵĸ��ṹ��
///
typedef struct {
    GOOSE_DATA_TYPE type;                                               ///< GOOSEͨ������������
    size_t          offset;                                             ///< �������ڱ���֡�е�ƫ�Ƶ�ַ
}GOOSE_DATA, *PGOOSE_DATA;

///
/// @brief
///     δ֪��������
///
typedef struct {
    GOOSE_DATA id;                                                      ///< ���ݱ�ʶ
    uint8_t    gooseTag;                                                ///< GOOSE�����е�Tagֵ
    uint32_t   datLen;                                                  ///< ���ݵ��ֽ���
    uint8_t   *datVal;                                                  ///< ָ�����ݵ��ֽ���ָ��
}GOOSE_UNKNOW;

///
/// @brief
///     GOOSE�������������У�AllData����Boolean��������
///
typedef struct {
    GOOSE_DATA id;                                                      ///< ���ݱ�ʶ
    bool       boolVal;                                                 ///< ����ֵ
}GOOSE_BOOLEAN;

///
/// @brief
///     ˫��ֵ����
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
///     ˫��ֵ����
///
static const std::wstring strDPValue[] = 
{
    L"00",                                                              ///< ˫��ֵinit
    L"01",                                                              ///< ˫��ֵoff
    L"10",                                                              ///< ˫��ֵon
    L"11"                                                               ///< ˫��ֵbad
};

///
/// @brief
///     GOOSE�������������У�AllData����Bit-String��λ������������
///
typedef struct tagGooseBitString{
	GOOSE_DATA id;                                                      ///< ���ݱ�ʶ
	uint8_t    padding;                                                 ///< λ���в�λ����Ŀ����λ�ӵ�λ��ʼ
	uint32_t   strSize;                                                 ///< λ�����ݵ��ֽ���
	uint8_t   *bitStr;                                                  ///< ָ��λ�����ݵ��ֽ���ָ��

    ///
    /// @brief
    ///     ����λ��
    ///
    /// @param[in]  strNew  �µ�λ���ַ���
    ///
    /// @return  
    ///     void    
    ///
    void SetString(std::wstring strNew);

    ///
    /// @brief
    ///     ���λ���ַ���
    ///
    /// @return
    ///     std::wstring    λ���ַ���
    ///
    std::wstring GetString();
}GOOSE_BIT_STRING;

///
/// @brief
///     GOOSE�������������У�AllData����8λInteger����������������
///
typedef struct {
	GOOSE_DATA id;                                                      ///< ���ݱ�ʶ
	int8_t     datVal;                                                  ///< 8λInteger����������������
}GOOSE_INT8;

///
/// @brief
///     GOOSE�������������У�AllData����16λInteger����������������
///
typedef struct {
    GOOSE_DATA id;                                                      ///< ���ݱ�ʶ
    int16_t    datVal;                                                  ///< 16λInteger����������������
}GOOSE_INT16;

///
/// @brief
///     GOOSE�������������У�AllData����24λInteger����������������
///
typedef struct {
    GOOSE_DATA id;                                                      ///< ���ݱ�ʶ
    int32_t    datVal;                                                  ///< 24λInteger����������������
}GOOSE_INT24;

///
/// @brief
///     GOOSE�������������У�AllData����32λInteger����������������
///
typedef struct {
    GOOSE_DATA id;                                                      ///< ���ݱ�ʶ
    int32_t    datVal;                                                  ///< 32λInteger����������������
}GOOSE_INT32;

///
/// @brief
///     GOOSE�������������У�AllData����8λUnsigned���޷�����������������
///
typedef struct {
    GOOSE_DATA id;                                                      ///< ���ݱ�ʶ
    uint8_t    datVal;                                                  ///< 8λUnsigned���޷�����������������
}GOOSE_UINT8;

///
/// @brief
///     GOOSE�������������У�AllData����16λUnsigned���޷�����������������
///
typedef struct {
    GOOSE_DATA id;                                                      ///< ���ݱ�ʶ
    uint16_t   datVal;                                                  ///< 16λUnsigned���޷�����������������
}GOOSE_UINT16;

///
/// @brief
///     GOOSE�������������У�AllData����24λUnsigned���޷�����������������
///
typedef struct {
    GOOSE_DATA id;                                                      ///< ���ݱ�ʶ
    uint32_t   datVal;                                                  ///< 24λUnsigned���޷�����������������
}GOOSE_UINT24;

///
/// @brief
///     GOOSE�������������У�AllData����32λUnsigned���޷�����������������
///
typedef struct {
    GOOSE_DATA id;                                                      ///< ���ݱ�ʶ
    uint32_t   datVal;                                                  ///< 32λUnsigned���޷�����������������
}GOOSE_UINT32;

///
/// @brief
///     GOOSE�������������У�AllData����Float����������������
///     ��32λ��64λ���֣���ӦC/C++��float��double
///     ������Ϊ����IEEE754�ĵ����ȸ���������ʽ������λ1λ������8λ��β��23λ
///
typedef struct {
    GOOSE_DATA id;                                                      ///< ���ݱ�ʶ
    float      datVal;                                                  ///< 32λ�����ȸ�����
}GOOSE_FLOAT;

///
/// @brief
///     GOOSE�������������У�AllData����Octet-String��8λλ�鴮����������
///
typedef struct {
    GOOSE_DATA id;                                                      ///< ���ݱ�ʶ
    uint32_t   strSize;                                                 ///< λ�鴮���ݵ��ֽ���
    uint8_t   *octetStr;                                                ///< ָ��λ�鴮���ݵ�ָ��
}GOOSE_OCTET_STRING;

///
/// @brief
///     GOOSE�������������У�AllData����Visible-String�����Ӵ�����������
///     һ��ΪASCII�ı�����
///
typedef struct {
    GOOSE_DATA id;                                                      ///< ���ݱ�ʶ
    uint32_t   strSize;                                                 ///< ���Ӵ����ݵ��ֽ���
    int8_t    *visibleStr;                                              ///< ָ����Ӵ����ݵ�ָ��
}GOOSE_VISIBLE_STRING;

///
/// @brief
///     GOOSE�������������У�AllData����IEC61850�����UTC-TIME��������
///
typedef struct tagGooseUtcTime{
    GOOSE_DATA id;                                                      ///< ���ݱ�ʶ
    uint32_t   t_s;                                                     ///< ʱ�������������
    uint32_t   t_ns;                                                    ///< ʱ��������������֣���С������
    uint8_t    t_q;                                                     ///< ʱ��Ʒ��

    ///
    /// @brief
    ///    ���UTCʱ��
    ///
    ///
    /// @return
    ///     SYSTEMTIME    UTCʱ��
    ///
    SYSTEMTIME GetUtcTime();

    ///
    /// @brief
    ///    ���UTCʱ���ַ���
    ///
    ///
    /// @return
    ///     std::wstring    UTCʱ���ַ���
    ///
    std::wstring GetUtcString();

    ///
    /// @brief
    ///    ���ָ��ʱ�����1970-01-01 00:00:00������
    ///
    /// @param[in]  st    ָ����ʱ��ṹ��
    ///
    /// @return
    ///     time_t    ��������
    ///
    time_t SystemTimeToTimet( SYSTEMTIME st );

    ///
    /// @brief
    ///    ����UTCʱ��
    ///
    /// @param[in]  sysTime    UTCʱ��
    ///
    void SetUtcTime(SYSTEMTIME sysTime);

    ///
    /// @brief
    ///    ����UTCʱ��
    ///
    /// @param[in]  strTime    UTCʱ���ַ������꣺�£��� ʱ���֣���.���룩
    ///
    void SetUctTime(std::wstring strTime);
}GOOSE_UTC_TIME;

///
/// @brief
///     GOOSE�������������У�AllData����IEC61850�����Struct��������
///
typedef struct {
    GOOSE_DATA id;                                                      ///< ���ݱ�ʶ
    std::vector<GOOSE_DATA *> member;                                   ///< �ṹ���Ա
}GOOSE_STRUCT;

///
/// @brief
///     ����GOOSE_DATA��������
///
/// @param[in]
///     pGseData - ָ��Ҫ���Ƶ�GooseData��ָ��
///
/// @return
///     ָ���µ�GooseData��ָ��
///
GOOSE_DATA *GooseDataCopy(GOOSE_DATA *pGseData);

///
/// @brief
///     �ͷŴ�����GooseData
///
/// @param[in]
///     pGseData - ָ��GooseData��ָ��
///
/// @return
///     
///
void GooseDataFree(GOOSE_DATA *pGseData);

///
/// @brief
///     ��ȡGooseData��TLV��ʽ��ռ�õ��ֽ���(Tag(1�ֽ�)+Len(1~5�ֽ�)+Val)
///
/// @param[in]
///     pGseData - ָ��GooseData��ָ��
///
/// @return
///     GooseData��TLV��ʽ��ռ�õ��ֽ���
///
size_t GooseDataTLVBytes(const GOOSE_DATA *pGseData);

///
/// @brief
///     GOOSEͨ��ֵ���ܱ���
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
    ///     Ĭ�Ϲ��캯��
    ///
    GOOSE_DATA_VARIANT();

    ///
    /// @brief
    ///     �������캯��
    ///
    GOOSE_DATA_VARIANT(const GOOSE_DATA_VARIANT& stGooseDataVariant);

    ///
    /// @brief
    ///    ���캯��
    ///
    /// @param[in]  pstGooseData    ָ��GOOSE_DATA�����ָ��
    ///
    GOOSE_DATA_VARIANT(GOOSE_DATA* pstGooseData);

    ///
    /// @brief
    ///     ��������
    ///
    virtual ~GOOSE_DATA_VARIANT()
    {
        __ReleaseAllData();
    }

    ///
    /// @brief
    ///    �ͷ�����������Ŀռ�
    ///
    ///
    /// @return
    ///     void    
    ///
    void __ReleaseAllData();

    ///
    /// @brief
    ///    ���ظ�ֵ������
    ///
    /// @param[in]  stGooseDataVariant    GOOSEͨ��ֵ���ܱ���������
    ///
    /// @return
    ///     GOOSE_DATA_VARIANT&    GOOSEͨ��ֵ���ܱ���������
    ///
    GOOSE_DATA_VARIANT& operator=(const GOOSE_DATA_VARIANT& stGooseDataVariant);

    ///
    /// @brief
    ///     ������
    ///
    bool operator !=(GOOSE_DATA_VARIANT& stGooseDataVariant) const;

    ///
    /// @brief
    ///    �����������
    ///
    ///
    /// @return
    ///     GOOSE_DATA_TYPE    ��������
    ///
    GOOSE_DATA_TYPE GetType()
    {
        return m_stUnKnown.id.type;
    }

    ///
    /// @brief
    ///    ������������
    ///
    /// @param[in]  eNewDataType    �µ���������
    ///
    void __SetType(GOOSE_DATA_TYPE eNewDataType);

    ///
    /// @brief
    ///    ���캯��
    ///
    /// @param[in]  stBoolean    GOOSE�������������У�AllData����Boolean��������
    ///
    GOOSE_DATA_VARIANT(GOOSE_BOOLEAN& stBoolean)
    {
        memset(this, 0, sizeof(GOOSE_DATA_VARIANT));
        m_stBoolean         = stBoolean;
    }

    ///
    /// @brief
    ///    ����ת��
    ///
    /// @return
    ///    GOOSE_BOOLEAN    GOOSE�������������У�AllData����Boolean��������
    ///
    operator GOOSE_BOOLEAN() const
    {
        return m_stBoolean;
    }

    ///
    /// @brief
    ///    ���캯��
    ///
    /// @param[in]  stInt8    GOOSE�������������У�AllData����8λInteger����������������
    ///
    GOOSE_DATA_VARIANT(GOOSE_INT8& stInt8)
    {
        memset(this, 0, sizeof(GOOSE_DATA_VARIANT));
        m_stInt8            = stInt8;
    }

    ///
    /// @brief
    ///    ���캯��
    ///
    /// @param[in]  stBitString    GOOSE�������������У�AllData����λ����������
    ///
    GOOSE_DATA_VARIANT(GOOSE_BIT_STRING& stBitString)
    {
        memset(this, 0, sizeof(GOOSE_DATA_VARIANT));

        // ��ȿ���
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
    ///    ����ת��
    ///
    /// @return
    ///    GOOSE_INT8    GOOSE�������������У�AllData����8λInteger����������������
    ///
    operator GOOSE_INT8() const
    {
        return m_stInt8;
    }

    ///
    /// @brief
    ///    ����ַ���
    ///
    /// @return
    ///     std::wstring    �ַ���
    ///
    std::wstring GetString();

	///
	/// @brief
	///     ����ASN.1��TLV��ʽ�����е�Len����
	///
	/// @param[in]
	///     eNewDataType - �µ���������
	/// @param[in]
	///     pCodeDataBuf - ����������Ļ���
	/// @param[in]
	///     bufSize      - �����С
	///
	/// @return
	///     ��������ʵ�ʴ�С
	///
	size_t CodeLenOfTLV(size_t valLenOfTLV, uint8_t *pCodeDataBuf, size_t bufSize);

	///
	/// @brief
	///     ����GOOSE�����е�ʹ��ASN.1��TLV��ʽ�Ļ�����������
	///
	/// @param[in]
	///     pCodeDataBuf - ����������Ļ���
	/// @param[in]
	///     bufSize      - �����С
	///
	/// @return
	///     ��������ʵ�ʴ�С
	///
	size_t CodeGooseData(uint8_t *pCodeDataBuf, size_t bufSize);
};

///
/// @brief
///     ����GOOSE�����е�ʹ��ASN.1��TLV��ʽ��StNum�ֶΣ��˴�����ʱstNum���ȹ̶�Ϊ4�ֽ�
///
/// @param[in]
///     stNum        - stNum��ֵ
/// @param[in]
///     pCodeDataBuf - ����������Ļ���
/// @param[in]
///     bufSize      - �����С
///
/// @return
///     ��������ʵ�ʴ�С
///
size_t CodeGooseStNum(uint32_t stNum, uint8_t *pCodeDataBuf, size_t bufSize);

#endif
