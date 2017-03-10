#ifndef         __FUNCTEST_ACTIVE_CONNECT_CE_H__
#define         __FUNCTEST_ACTIVE_CONNECT_CE_H__

#ifdef      __cplusplus
extern "C"
{
#endif

    //CE端应用程序调用此链接表明
    BOOL    CEConnect();


    //CE端应用程序断开连接
    void    CEDisConnect();
    BOOL    CEWrite(void *pBuf,unsigned long ulLen,DWORD dwTimeOut);
    BOOL    CERead(void *pBuf,unsigned long ulLen,DWORD *pdwReaded,DWORD dwTimeOut );

#ifdef      __cplusplus
};
#endif
#endif