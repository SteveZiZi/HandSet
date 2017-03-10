#ifndef         __FUNCTEST_ACTIVE_CONNECT_PC_H__
#define         __FUNCTEST_ACTIVE_CONNECT_PC_H__


#ifdef          __cplusplus
extern "C" 
{
#endif



//PC端远程调用的函数,返回零值表示成功,返回1表示操作超时，返回-1表示连接到客户端失败
int     PCWrite(DWORD cbInput,BYTE *Input,DWORD *pvbOutput,
                  BYTE **ppOutput,void *pStream);
int     PCRead(DWORD cbInput,BYTE *Input,DWORD *pvbOutput,
                BYTE **ppOutput,void *pStream);
int     PCConnect(DWORD cbInput,BYTE *Input,DWORD *pvbOutput,
               BYTE **ppOutput,void *pStream);

#ifdef          __cplusplus
};
#endif

#endif