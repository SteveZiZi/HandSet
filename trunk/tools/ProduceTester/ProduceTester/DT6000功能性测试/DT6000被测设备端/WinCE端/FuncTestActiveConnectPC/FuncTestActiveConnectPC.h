#ifndef         __FUNCTEST_ACTIVE_CONNECT_PC_H__
#define         __FUNCTEST_ACTIVE_CONNECT_PC_H__


#ifdef          __cplusplus
extern "C" 
{
#endif



//PC��Զ�̵��õĺ���,������ֵ��ʾ�ɹ�,����1��ʾ������ʱ������-1��ʾ���ӵ��ͻ���ʧ��
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