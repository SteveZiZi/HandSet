#include "stdafx.h"
#include "epcAuthorization.h"

static LPCSTR s_pszPassword = "639547";
BOOL __inline IsAuthorized()
{
    char EncodeData[__BLOCK_DATA_SIZE] = {0};
    BOOL bRet = FALSE;
    DWORD dwEncodeDataLen = __BLOCK_DATA_SIZE;							/* �������Ĵ�С					*/

    bRet = epcAuthorizeInit();
    if (bRet) {
        bRet = epcAuthorizeEncode(s_pszPassword, EncodeData, &dwEncodeDataLen);
        /*	����Ȩ���������Ȩ���ܡ�ע	*/
        /* ����øú���ǰӦ�����һ����	*/
        /* ���뻺�����Ĵ�С�����øú���	*/
        /* �����һ�������������ܽ�� */
        /* ����							*/
    }
    if (bRet) {
        char *pBuf = new char[dwEncodeDataLen];
        if (pBuf) {
            DWORD dwReadLen = epcReadBlockData(pBuf, dwEncodeDataLen);	/* ��ȡ�������ݿ��е�ƽ̨��Ȩ��	*/
            /*	������						*/
            if (dwReadLen == dwEncodeDataLen) {							/* �ж϶������Ƿ�ɹ�			*/
                bRet = (memcmp(pBuf, EncodeData, dwEncodeDataLen) == 0);
                /* �Ƚ��ɴ����������ܺ������	*/
                /* ��ӱ������ݿ��ж�ȡ��������	*/
                /* ����ͬ						*/
            } else {
                bRet = FALSE;
            }
            delete[] pBuf;
        } else {
            bRet = FALSE;
        }
    }
    epcAuthorizeDeinit();

    return bRet;
}