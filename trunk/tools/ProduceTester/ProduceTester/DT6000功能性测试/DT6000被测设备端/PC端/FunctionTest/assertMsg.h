#ifndef __MY_ASSERT_H_
#define __MY_ASSERT_H_

#ifdef assert
#undef assert
#endif


//根据C和C++的不同定义const
#ifndef __cplusplus
#define const  
#endif

#ifdef __cplusplus
extern "C" {
#endif

//定义生成字符串的宏
#define __MAKE_STR2(x)          #x
#define __MAKE_STR(x)           __MAKE_STR2(x)

//定生成UNICODE字符串的宏
#define __MAKE_WIDE_STR2(x)     L ## x
#define __MAKE_WIDE_STR(x)      __MAKE_WIDE_STR2(x)

//根据调试版本和发行版本的不同来定义assert和assertMsg
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