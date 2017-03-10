#include "stdafx.h"
#include "epcAuthorization.h"

static LPCSTR s_pszPassword = "639547";
BOOL __inline IsAuthorized()
{
    char EncodeData[__BLOCK_DATA_SIZE] = {0};
    BOOL bRet = FALSE;
    DWORD dwEncodeDataLen = __BLOCK_DATA_SIZE;							/* 缓冲区的大小					*/

    bRet = epcAuthorizeInit();
    if (bRet) {
        bRet = epcAuthorizeEncode(s_pszPassword, EncodeData, &dwEncodeDataLen);
        /*	对授权密码进行授权加密。注	*/
        /* 意调用该函数前应由最后一参数	*/
        /* 传入缓冲区的大小，调用该函数	*/
        /* 后，最后一参数将传出加密结果 */
        /* 长度							*/
    }
    if (bRet) {
        char *pBuf = new char[dwEncodeDataLen];
        if (pBuf) {
            DWORD dwReadLen = epcReadBlockData(pBuf, dwEncodeDataLen);	/* 读取保留数据块中的平台授权加	*/
            /*	密数据						*/
            if (dwReadLen == dwEncodeDataLen) {							/* 判断读数据是否成功			*/
                bRet = (memcmp(pBuf, EncodeData, dwEncodeDataLen) == 0);
                /* 比较由传入的密码加密后的数据	*/
                /* 与从保留数据块中读取的数据是	*/
                /* 否相同						*/
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