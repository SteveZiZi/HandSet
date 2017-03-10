#ifndef MSG_BASE_DATA_ELEM

#define MSG_BASE_DATA_ELEM

#include "msg_data_elem.h"

class MsgBaseDataElem:public MsgDataElem
{
public:
	MsgBaseDataElem();
	virtual ~MsgBaseDataElem();

	virtual size_t Decode(const size_t offset, const uint8_t *inData, const size_t datSize);
	virtual size_t Code(uint8_t *outData, const size_t datSize);

	virtual uint8_t GetTag() const ;
	virtual size_t  GetElemOffset() const ;
	virtual size_t  GetElemLen() const ;
	virtual size_t  GetValueOffset() const ;
	virtual size_t  GetValueLen() const ;
	virtual const uint8_t * GetValue() const ;

	virtual void SetTag(const uint8_t tag);
	virtual void SetValue(const uint8_t *inData, const size_t datSize);

private:
	uint8_t  m_Tag;
	size_t   m_ElemLen;
	size_t   m_ElemOffset;
	size_t   m_ValueLen;
	size_t   m_ValueOffset;
	uint8_t *m_Value;
};

#endif
