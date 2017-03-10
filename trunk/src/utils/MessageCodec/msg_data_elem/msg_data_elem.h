/// @file
///     msg_data_elem.h
///
/// @brief
///     ����Ԫ�ض���ı����ӿ�
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
///    |2013.05.14     |1.0.0.514  |����Ԫ�ص�Ԫ�غ�Ԫ������ֵ��ƫ�Ƶ�ַ�����ȵ���Ϣ��ȡ     |
///
#ifndef MSG_DATA_ELEM_H_

#define MSG_DATA_ELEM_H_

#include "src\include\stdint.h"

///
/// @brief
///     ��Tagֵ
///
/// @note
///     TAGΪTAG_NONE��ʾһ������,������ΪTLV����
///
#define TAG_NONE                  0x00

///
/// @brief
///     Sav91������PDU��Tagֵ
///
#define SAV91_TAG_SAV_PDU         0x80

///
/// @brief
///     Sav91��չ���ģ��Ͼ�������磩��PDU��Tagֵ
///
#define SAV91_EX_TAG_SAV_PDU      0x80

///
/// @brief
///     Sav92�����и�TLV���ݵ�Tagֵ
///
#define SAV92_TAG_SAV_PDU         0x60         ///< Sav92����PDU
#define SAV92_TAG_NO_ASDU         0x80         ///< noASDU��ASDU��Ŀ��
#define SAV92_TAG_SECURITY        0x81         ///< security����ȫ��Ϣ��
#define SAV92_TAG_SEQ_ASDU        0xA2         ///< Sequence of ASDU��ASDU���У�
#define SAV92_TAG_ASDU            0x30         ///< ASDU
#define SAV92_TAG_SV_ID           0x80         ///< svID������ֵ���ƿ�ID��
#define SAV92_TAG_DATA_SET        0x81         ///< datset���������ݼ���
#define SAV92_TAG_SMP_CNT         0x82         ///< smpCnt��������������
#define SAV92_TAG_CONF_REV        0x83         ///< confRev�����ð汾��
#define SAV92_TAG_REFR_TM         0x84         ///< refrTm��ˢ��ʱ�䣩
#define SAV92_TAG_SMP_SYNCH       0x85         ///< smpSynch��ͬ����ʶλ��
#define SAV92_TAG_SMP_RATE        0x86         ///< smpRate�������ʣ�
#define SAV92_TAG_SEQ_OF_DATA     0x87         ///< Sequence of Data(����ֵ����)

///
/// @brief
///     GOOSE�����и�TLV���ݵ�Tagֵ
///
#define GOOSE_TAG_GOOSE_PDU       0x61         ///< GOOSE���ĵ�PDU
#define GOOSE_TAG_GOCB_REF        0x80         ///< gocbRef��GOOSE���ƿ�������
#define GOOSE_TAG_TIME_ALLOWED    0x81         ///< timeAllowedtoLive��������������ʱ�䣩
#define GOOSE_TAG_DATA_SET        0x82         ///< datSet��GOOSE���ݼ���
#define GOOSE_TAG_GO_ID           0x83         ///< goID��GOOSE���ĵ�Ψһ�Ա�ʶ��
#define GOOSE_TAG_EVENT_TIME      0x84         ///< t(GOOSE���ݱ�λʱ�估��ʱ���Ʒ��)
#define GOOSE_TAG_ST_NUM          0x85         ///< stNum��״̬��ţ�
#define GOOSE_TAG_SQ_NUM          0x86         ///< sqNum������˳��ţ�
#define GOOSE_TAG_TEST            0x87         ///< test�����ޱ�ʶ��
#define GOOSE_TAG_CONF_REV        0x88         ///< confRev�����ư汾�ţ�
#define GOOSE_TAG_NDS_COM         0x89         ///< ndsCom��GOOSE�Ƿ������ã�
#define GOOSE_TAG_NUM_DS_ENTRIES  0x8A         ///< numDatSetEntries�����ݼ���Ŀ����
#define GOOSE_TAG_ALL_DATA        0xAB         ///< allData��GOOSE�������У�
#define GOOSE_TAG_SECURITY        0x8C         ///< security����ȫ��Ϣ��

///
/// @brief
///     GOOSE��������������(allData)���������͵�Tagֵ
///
#define GOOSE_DATA_TAG_ARRAY         0xA1      ///< ��������
#define GOOSE_DATA_TAG_STRUCT        0xA2      ///< �ṹ������
#define GOOSE_DATA_TAG_BOOLEAN       0x83      ///< ������
#define GOOSE_DATA_TAG_BIT_STR       0x84      ///< λ����
#define GOOSE_DATA_TAG_INTEGER       0x85      ///< ����
#define GOOSE_DATA_TAG_UNSIGNED      0x86      ///< �޷�������
#define GOOSE_DATA_TAG_FLOAT_POINT   0x87      ///< ������
#define GOOSE_DATA_TAG_OCTET_STR     0x89      ///< 8λλ�鴮����
#define GOOSE_DATA_TAG_VISIBLE_STR   0x8A      ///< ���Ӵ�����
#define GOOSE_DATA_TAG_GENERAL_TIME  0x8B      ///< ����ʱ������
#define GOOSE_DATA_TAG_BINARY_TIME   0x8C      ///< ����ʱ������
#define GOOSE_DATA_TAG_BCD           0x8D      ///< BCD������
#define GOOSE_DATA_TAG_BOOLEAN_ARRAY 0x8E      ///< ��������������
#define GOOSE_DATA_TAG_OBJ_ID        0x8F      ///< ����ID����
#define GOOSE_DATA_TAG_MMS_STR       0x90      ///< MMS������
#define GOOSE_DATA_TAG_UTC_TIME      0x91      ///< UTCʱ������

///
/// @brief
///     ����Ԫ�ص�����
///
typedef enum{DT_BASE_DATA = 0, DT_TLV_DATA} DATA_ELEM_TYPE;

///
/// @brief
///     ����Ԫ�صĲ����ӿ�
///
class MsgDataElem
{
public:
	virtual ~MsgDataElem(){};

	///
	/// @brief
	///     ���ض�����������һ������Ԫ��
	///
	/// @param[in]
	///     offset  - ������Ԫ����֡�е�ƫ�Ƶ�ַ
	/// @param[in]
	///     inData  - �����������
	/// @param[in]
	///     datSize - ���������ݵĳ���
	///
	/// @return
	///     ʵ�ʵĽ������ݴ�С - �ɹ��� 0 - ʧ��
	///
	virtual size_t Decode(const size_t offset, const uint8_t *inData, const size_t datSize) = 0;

	///
	/// @brief
	///     �������õ���Ϣ���ض����������б��룬���һ������Ԫ��
	///
	/// @param[in]
	///     outData - ��������Ԫ�ص�����
	/// @param[in]
	///     datSize - �����Ԫ����Ŀ
	///
	/// @return
	///     ����Ԫ�ص�ʵ�ʴ�С - �ɹ��� 0 - ʧ��
	///
	virtual size_t Code(uint8_t *outData, const size_t datSize) = 0;
	
	///
	/// @brief
	///     ��ȡ������Ԫ�صı�ǩֵ
	///
	/// @return
	///     ����Ԫ�صı�ǩֵ
	///
	virtual uint8_t GetTag() const = 0;

	///
	/// @brief
	///     ��ȡ������Ԫ����֡�е�ƫ�Ƶ�ַ����ΪTLV���ݣ���Ϊ��TLV���ݵ���֡�е�ƫ�Ƶ�ַ
	///
	/// @return
	///     ����Ԫ����֡�е�ƫ�Ƶ�ַ
	///
	virtual size_t GetElemOffset() const = 0;

	///
	/// @brief
	///     ��ȡ������Ԫ�صĳ��ȣ���ΪTLV���ݣ���Ϊ��Value���ֵĳ���
	///
	/// @return
	///     ����Ԫ�صĳ���
	///
	virtual size_t GetElemLen() const = 0;

	///
	/// @brief
	///     ��ȡ������Ԫ����֡�е�ƫ�Ƶ�ַ����ΪTLV���ݣ���Ϊ��TLV���ݵ���֡�е�ƫ�Ƶ�ַ
	///
	/// @return
	///     ����Ԫ����֡�е�ƫ�Ƶ�ַ
	///
	virtual size_t GetValueOffset() const = 0;

	///
	/// @brief
	///     ��ȡ������Ԫ�صĳ��ȣ���ΪTLV���ݣ���Ϊ��Value���ֵĳ���
	///
	/// @return
	///     ����Ԫ�صĳ���
	///
	virtual size_t GetValueLen() const = 0;

	///
	/// @brief
	///     ��ȡ������Ԫ�ص�ֵ��Ϣ����ΪTLV���ݣ���Ϊָ����Value����
	///
	/// @return
	///     ָ��Ԫ�ص�ֵ���ݵ�ָ��;ʧ���򷵻�NULL
	///
	virtual const uint8_t * GetValue() const = 0;

	///
	/// @brief
	///     ���ø�����Ԫ�صı�ǩֵ
	///
	/// @param[in]
	///     tag - ����Ԫ�صı�ǩֵ
	///
	/// @return
	///     
	///
	virtual void SetTag(const uint8_t tag) = 0;

	///
	/// @brief
	///     ���ø�����Ԫ�ص�ֵ��Ϣ����ΪTLV���ݣ���Ϊ������Valueֵ
	///
	/// @param[in]
	///     inData  - ����Ԫ�ص�ֵ��Ϣ
	/// @param[in]
	///     datSize - ����Ԫ�ص�ֵ��Ϣ�ĳ���
	///
	/// @return
	///     
	///
	virtual void SetValue(const uint8_t *inData, const size_t datSize) = 0;
};

///
/// @brief
///     ��������Ԫ�ض����ʵ��
///
/// @return
///     ָ��MsgDataElemʵ����ָ��
///
MsgDataElem *CreateMsgDataElemImpl(DATA_ELEM_TYPE dt);

///
/// @brief
///     �ͷ�����Ԫ�ض����ʵ��
///
/// @param[in]
///     impl - ָ��MsgDataElemʵ����ָ��
///
/// @return
///     
///
void ReleaseMsgDataElemImpl(MsgDataElem *impl);

#endif
