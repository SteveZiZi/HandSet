#ifndef         __FUNCTEST_ACTIVE_CONNECT_CE_H__
#define         __FUNCTEST_ACTIVE_CONNECT_CE_H__

#ifdef      __cplusplus
extern "C"
{
#endif

    //CE��Ӧ�ó�����ô����ӱ���
    BOOL    CEConnect();


    //CE��Ӧ�ó���Ͽ�����
    void    CEDisConnect();
    BOOL    CEWrite(void *pBuf,unsigned long ulLen,DWORD dwTimeOut);
    BOOL    CERead(void *pBuf,unsigned long ulLen,DWORD *pdwReaded,DWORD dwTimeOut );

#ifdef      __cplusplus
};
#endif
#endif