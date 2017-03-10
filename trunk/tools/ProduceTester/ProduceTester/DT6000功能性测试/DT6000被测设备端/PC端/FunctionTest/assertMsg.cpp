#include <windows.h>
#include <stdio.h>
#ifndef NDEBUG


static int __intToStr(WCHAR *pStrValue,int len,int iValue)
{
    BOOL    bNegative=FALSE;
    WCHAR   str[100];
    int     pos=0;
    
    if (iValue <0)
    {
        iValue =0-iValue;
        bNegative=TRUE;
    }
    
    do 
    {
        str[pos++]=iValue%10+L'0';
        iValue /=10;
    } while (iValue >0);
    
    int iNeedLen=pos;       //��洢���ַ����ĳ��ȣ���������βNUL
    if (bNegative)
    {
        iNeedLen++;
    }
    
    if (len <iNeedLen+1)
    {
        return 0;
    }
    pStrValue[iNeedLen]=0;
    
    int i=0;
    pos--;
    if (bNegative)
    {
        pStrValue[0]=L'-';
        while(i<=pos)
        {
            pStrValue[1+i]=str[pos-i];
            i++;
        }
    }
    else
    {
        while(i<=pos)
        {
            pStrValue[i]=str[pos-i];
            i++;
        }
    }
    
    return iNeedLen;
}

extern "C" void __assert(const WCHAR *con, const WCHAR *strFileName,int iLine)
{
    int     iconLen=(int)wcslen(con);
    int     iFileNamelen=(int)wcslen(strFileName);
    WCHAR   strLine[16];
    WCHAR   strModuleName[MAX_PATH *2+1];


    int iModuleNameLen=GetModuleFileNameW(NULL,strModuleName,MAX_PATH*2);
   
    __intToStr(strLine,16,iLine);
    WCHAR pStrMessage[MAX_PATH*10];
    wcscpy(pStrMessage,L"���Զ���ʧ��!!!\r\n\r\n\r\n\r\n��������:");
    wcscat(pStrMessage,strModuleName);

    wcscat(pStrMessage,L"\r\n\r\nԴ�ļ���:");
    wcscat(pStrMessage,strFileName);    
    wcscat(pStrMessage,L"\r\n\r\nʧ������:");
    wcscat(pStrMessage,strLine);
    wcscat(pStrMessage,L"\r\n\r\nʧ������:");
    wcscat(pStrMessage,con);
    
    wcscat(pStrMessage,L"\r\n\r\n���ֶ���ʧ�ܵ�ԭ������Ϊassert��ʹ�ñ��ʽ��Ϊ��!\r\n");
    wcscat(pStrMessage,L"Ҫ���ø������Ϣ����ο���������ָ���Դ����");
    wcscat(pStrMessage,L"\r\n\r\n\r\n���� \"��ֹ\" ��ֹ��ǰӦ�ó���,���� \"����\" ����ִ��");
    
    int nRet;
    nRet=MessageBoxW(NULL,pStrMessage,L"����ʧ��",MB_ABORTRETRYIGNORE |MB_ICONERROR);
    while(nRet==IDRETRY)
    {
        nRet=MessageBoxW(NULL,pStrMessage,L"����ʧ��",MB_ABORTRETRYIGNORE |MB_ICONERROR); 
    }
    if( nRet==IDABORT)
    {
        ExitProcess(100);
    }
   
}

extern "C" void __assertMsg(const WCHAR *con, const WCHAR *strFileName,int iLine,const WCHAR *strMsg)
{
    int     iconLen=(int)wcslen(con);
    int     iFileNamelen=(int)wcslen(strFileName);
    int     iMsgLen=0;
    WCHAR   strLine[16];
    WCHAR   strModuleName[MAX_PATH *2+1];
    
    
    int iModuleNameLen=GetModuleFileNameW(NULL,strModuleName,MAX_PATH*2);    
    __intToStr(strLine,16,iLine);
    if (strMsg !=NULL)
    {
        iMsgLen=(int)wcslen(strMsg);
    }
    
    WCHAR pStrMessage[MAX_PATH*20];
    wcscpy(pStrMessage,L"���Զ���ʧ��!!!\r\n\r\n\r\n\r\n��������:");
    wcscat(pStrMessage,strModuleName);
    
    wcscat(pStrMessage,L"\r\n\r\nԴ�ļ���:");
    wcscat(pStrMessage,strFileName);    
    wcscat(pStrMessage,L"\r\n\r\nʧ������:");
    wcscat(pStrMessage,strLine);
    wcscat(pStrMessage,L"\r\n\r\nʧ������:");
    wcscat(pStrMessage,con);

    wcscat(pStrMessage,L"\r\n\r\nʧ����Ϣ:");
    if(strMsg !=NULL)
        wcscat(pStrMessage,strMsg);
    
    wcscat(pStrMessage,L"\r\n\r\n���ֶ���ʧ�ܵ�ԭ������Ϊassert��ʹ�ñ��ʽ��Ϊ��!\r\n");
    wcscat(pStrMessage,L"Ҫ���ø������Ϣ����ο���������ָ���Դ����");
    wcscat(pStrMessage,L"\r\n\r\n\r\n���� \"��ֹ\" ��ֹ��ǰӦ�ó���,���� \"����\" ����ִ��");
    
    int nRet;
    nRet=MessageBoxW(NULL,pStrMessage,L"����ʧ��",MB_ABORTRETRYIGNORE |MB_ICONERROR);
    while(nRet==IDRETRY)
    {
        nRet=MessageBoxW(NULL,pStrMessage,L"����ʧ��",MB_ABORTRETRYIGNORE |MB_ICONERROR); 
    }
    if( nRet==IDABORT)
    {
        ExitProcess(100);
    }
}

#endif