#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "testCommon.h"

BOOL TestInfoLocalToTransfer(TEST_INFO_TRANSFER *pTestInfoTrans,const TEST_INFO *pTestInfoLocal)
{
    int len;
    pTestInfoTrans->bError=pTestInfoLocal->bError;
    memcpy(pTestInfoTrans->bAppend,pTestInfoLocal->bAppend,sizeof(pTestInfoTrans->bAppend) );

    if (pTestInfoLocal->bError)
    {
        len=WideCharToMultiByte(CP_UTF8,0,pTestInfoLocal->strErr,pTestInfoLocal->uiErrorLen,
            pTestInfoTrans->strErr,sizeof(pTestInfoTrans->strErr )-1,NULL,FALSE);     
        pTestInfoTrans->strErr[len]=0;
        pTestInfoTrans->uiErrorLen=(UINT16)len; 
    }
    return TRUE;
}

BOOL TestInfoTransferToLocal(TEST_INFO *pTestInfoLocal,const TEST_INFO_TRANSFER*pTestInfoTrans)
{
    int len;
    pTestInfoLocal->bError=pTestInfoTrans->bError;
    memcpy(pTestInfoLocal->bAppend,pTestInfoTrans->bAppend,sizeof(pTestInfoLocal->bAppend ));

    if (pTestInfoLocal->bError)
    {
        len=MultiByteToWideChar(CP_UTF8,0,pTestInfoTrans->strErr,pTestInfoTrans->uiErrorLen,
            pTestInfoLocal->strErr,sizeof(pTestInfoLocal->strErr)/sizeof(wchar_t)-1  );   
        pTestInfoLocal->strErr[len]=0;    
        pTestInfoLocal->uiErrorLen=(UINT16 )len;
    }
    
    
    return TRUE;
}

BOOL DeviceInfoLocalToTransfer(DEVICE_INFO_TRANSFER *pDevInfoTrans,const DEVICE_INFO *pDevInfoLocal)
{
    int     len;
    size_t  istrLen=wcslen(pDevInfoLocal->strDeviceID);
    size_t  iStrBuferLen=sizeof(pDevInfoTrans->strDeviceID)-1;

    len=WideCharToMultiByte(CP_UTF8,0,pDevInfoLocal->strDeviceID,(int)istrLen,
        pDevInfoTrans->strDeviceID,(int)iStrBuferLen,NULL,FALSE); 
    if (len <=0 && istrLen !=0)
    {
        return FALSE;
    }
    pDevInfoTrans->strDeviceID[len]=0;
    
    istrLen=wcslen(pDevInfoLocal->strDeviceType);
    iStrBuferLen=sizeof(pDevInfoTrans->strDeviceType )-1;

    len=WideCharToMultiByte(CP_UTF8,0,pDevInfoLocal->strDeviceType,(int)istrLen,
        pDevInfoTrans->strDeviceType,int(iStrBuferLen),NULL,FALSE );
    if (len<=0 && istrLen !=0)
    {
        return FALSE;
    }
    pDevInfoTrans->strDeviceType[len]=0;

    istrLen=wcslen(pDevInfoLocal->strDeviceVer);
    iStrBuferLen=sizeof(pDevInfoTrans->strDeviceVer )-1;

    len=WideCharToMultiByte(CP_UTF8,0,pDevInfoLocal->strDeviceVer,(int)istrLen,
        pDevInfoTrans->strDeviceVer,int(iStrBuferLen),NULL,FALSE );
    if (len<=0 && istrLen !=0)
    {
        return FALSE;
    }
    pDevInfoTrans->strDeviceVer[len]=0;

    return TRUE;
}

BOOL DeviceInfoTransferToLocal(DEVICE_INFO *pDevInfoLocal,const DEVICE_INFO_TRANSFER *pDevInfoTrans)
{
    int len;
    size_t istrLen=strlen(pDevInfoTrans->strDeviceID);
    size_t istrBufLenLocal=sizeof(pDevInfoLocal->strDeviceID)/sizeof(WCHAR )-1;
    len=MultiByteToWideChar(CP_UTF8,0,pDevInfoTrans->strDeviceID,(int)istrLen,
        pDevInfoLocal->strDeviceID,(int)istrBufLenLocal);
    if (len<=0 && istrLen !=0)
    {
        return FALSE;
    }
    pDevInfoLocal->strDeviceID[len]=0;

    istrLen=strlen(pDevInfoTrans->strDeviceType);
    istrBufLenLocal=sizeof(pDevInfoLocal->strDeviceType)/sizeof(WCHAR)-1;

    len=MultiByteToWideChar(CP_UTF8,0,pDevInfoTrans->strDeviceType,(int)istrLen,
        pDevInfoLocal->strDeviceType,(int)istrBufLenLocal );
    if (len<=0 && istrLen !=0)
    {
        return FALSE;
    }
    pDevInfoLocal->strDeviceType[len]=0;

    istrLen=strlen(pDevInfoTrans->strDeviceVer);
    istrBufLenLocal=sizeof(pDevInfoLocal->strDeviceVer)/sizeof(WCHAR)-1;

    len=MultiByteToWideChar(CP_UTF8,0,pDevInfoTrans->strDeviceVer,(int)istrLen,
        pDevInfoLocal->strDeviceVer,(int)istrBufLenLocal );
    if (len<=0 && istrLen !=0)
    {
        return FALSE;
    }
    pDevInfoLocal->strDeviceVer[len]=0;
    
    return FALSE;
}