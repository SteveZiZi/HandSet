#ifndef __MY_ASSERT_H_
#define __MY_ASSERT_H_

#ifdef assert
#undef assert
#endif


//����C��C++�Ĳ�ͬ����const
#ifndef __cplusplus
#define const  
#endif

#ifdef __cplusplus
extern "C" {
#endif

//���������ַ����ĺ�
#define __MAKE_STR2(x)          #x
#define __MAKE_STR(x)           __MAKE_STR2(x)

//������UNICODE�ַ����ĺ�
#define __MAKE_WIDE_STR2(x)     L ## x
#define __MAKE_WIDE_STR(x)      __MAKE_WIDE_STR2(x)

//���ݵ��԰汾�ͷ��а汾�Ĳ�ͬ������assert��assertMsg
#ifndef NDEBUG
void __assert(const WCHAR *con, const WCHAR *strFIleName,int iLine);
void __assertMsg(const WCHAR *con, const WCHAR *strFIleName,int iLine,const WCHAR *strMsg);

#define  assert(con) do{  \
        if(!(con)) \
        __assert( __MAKE_WIDE_STR(__MAKE_STR(con) ),__MAKE_WIDE_STR(__FILE__),__LINE__);  \
        } while(0);
#define  assertMsg(con,MSG) do{  \
    if(!(con) ) \
    __assertMsg( __MAKE_WIDE_STR(__MAKE_STR(con) ),__MAKE_WIDE_STR(__FILE__),__LINE__,MSG);  \
        } while(0);
#else
#define  assert(con) 
#define  assertMsg(con,MSG)
#endif



#ifdef __cplusplus
}
#endif


#endif