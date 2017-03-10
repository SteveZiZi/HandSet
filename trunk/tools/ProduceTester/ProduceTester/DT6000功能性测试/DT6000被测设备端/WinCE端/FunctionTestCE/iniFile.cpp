/*********************************************************************************************************
** File name:				iniFile.cpp
**
** Descriptions:            ʵ����Win32�²���ini�ļ�һ�麯������Win32ͬ�����ܵĺ���ӵ����ͬ�������ͱ��֣�
							������WinCE��ϵͳ��ʹ�ã�Ҳ�����Ժ�΢����Win CE���ṩ��һ�麯��֮�󣬿��Ժܷ�
							��ʹ��΢���ʵ�֡�
							������ʵ�ֵĺ�����������Win32������ʹ�ã���ע�����΢���ʵ�ֳ���������ͻ
*********************************************************************************************************/


#include "stdafx.h"
#include <assert.h>
#include "iniFile.h"
#include "UsuallyCommon.h"

enum	__CODEPAGE{CODEPAGE_ANSI,CODEPAGE_UTF16_LITTLE,CODEPAGE_UTF16_BIG,CODEPAGE_UTF8};

static int __MemCmp(const void *pBuf1,const void *pBuf2,int size)
{
    const BYTE *p1=(const BYTE *)pBuf1;
    const BYTE *p2=(const BYTE *)pBuf2;
    for (int i=0;i<size;i++)
    {
        if (p1[i] >p2[i])
            return 1;
        if (p1[i] <p2[i])
            return -1;     
    }
    return 0;
}

/*********************************************************************************************************
** Function name:           __SkipSpaceTabForward
**
** Descriptions:            ��һ���ַ�����������posƫ������ʼ������pos����ǰ�����ո��tab���Ʒ���
                            ����pos��ʼ��һ����Ϊ�ո���ַ���ƫ����
**
** input parameters:        strBuf          �ַ���������                        
** output parameters:       iBufLen         �ַ����������ĳ���
                            pos             ��posƫ�ƿ�ʼ����                           
** Returned value:          ����pos��ʼ��һ����Ϊ�ո���ַ���ƫ����

** Remarks:                 �����ƫ����pos��ʼ��ǰ���ַ���ȫΪ�ո��򷵻�ֵΪ-1
*********************************************************************************************************/
static int __SkipSpaceTabForward(LPCWSTR strBuf,int iBufLen,int pos)
{
    assert(pos<iBufLen);                        //��ֹƫ���������ַ�������

    int i=pos;
    while(i>=0 && ( strBuf[i] == L' ' || strBuf[i] == L'\t'))
    {
        i--;
    }   
    return i;
}

/*********************************************************************************************************
** Function name:           __SkipSpaceTab
**
** Descriptions:            ��һ���ַ�����������posƫ������ʼ������pos�������ո��tab���Ʒ���
                            ����pos��ʼ��һ����Ϊ�ո���ַ���ƫ����
**
** input parameters:        strBuf          �ַ���������                        
** output parameters:       iBufLen         �ַ����������ĳ���
                            pos             ��posƫ�ƿ�ʼ����
** Returned value:          ����pos��ʼ��һ����Ϊ�ո���ַ���ƫ����

** Remarks:                 �����ƫ����pos��ʼ���ַ���ȫΪ�ո��򷵻�ֵΪiBufLen
*********************************************************************************************************/
static int __SkipSpaceTab(LPCWSTR strBuf,int iBufLen,int pos)
{   

    int i=pos;
    while(i<iBufLen && ( strBuf[i] == L' ' || strBuf[i] == L'\t'))
    {
        i++;
    }

	return i;
}

static BOOL inline __IsNumber(WCHAR wc)
{
	if (wc >=L'0' && wc<=L'9')
	{
		return TRUE;
	}
	return FALSE;
}

/*********************************************************************************************************
** Function name:           __IgnoreSpaceTabStrcmp
**
** Descriptions:            ���Ե������ַ���ͷ����β���Ŀո�tab���Ʒ� �ٽ����ַ����Ƚ�
**                          ���͵�"people" ����"   people       "
** input parameters:        pStr1			�ַ���1
                            iStr1Len		�ַ���1�ĳ���
                            pStr2			�ַ���2
                            iStr2Len		�ַ���2�ĳ���
��
** Returned value:          ����ַ���1�����ַ���2������ֵ����0
                            ����ַ���1С���ַ���2������ֵС��0
                            ����򷵻�0
*********************************************************************************************************/
static int __IgnoreSpaceTabStrcmp(LPCWSTR pStr1,int iStr1Len,LPCWSTR pStr2,int iStr2Len)
{
    int iStart1=0,iStart2=0;
    int iStop1=iStr1Len-1;
    int iStop2=iStr2Len-1;
    int len,len1,len2;                  //��¼��Ҫ�Ƚϵ��ַ������ȣ����Կո���ַ���1���ַ���2�ĳ���
    
    assert(iStr1Len>=0);
    assert(iStr2Len>=0);

    //���Թ��˿ո��tab���Ʒ�
    while((pStr1[iStart1] ==L' ' || pStr1[iStart1] ==L'\t') && (iStart1 <iStr1Len) )
    {
        iStart1++;
    }
    while( (pStr1[iStop1] ==L' ' || pStr1[iStop1] ==L'\t') && (iStop1 >iStart1))
    {
        iStop1--;
    }
    while((pStr2[iStart2] ==L' ' || pStr2[iStart2] ==L'\t') && (iStart2 <iStr2Len) )
    {
        iStart2++;
    }
    while( (pStr2[iStop2] ==L' ' || pStr2[iStop2] ==L'\t') && (iStop2 >iStart2))
    {
        iStop2--;
    }

    //������Կո��tab����ַ����ĳ��Ⱥ���Ҫ�Ƚϵĳ���
    len1=iStop1-iStart1+1;
    len2=iStop2-iStart2+1;
    len= len1>len2?len2:len1;

    int i;
    for (i=0;i<len;i++)
    {
        if (pStr1[iStart1+i] > pStr2[iStart2+i])
            return 1;
        else if(pStr1[iStart1+i] <pStr2[iStart2+i])
            return -1;
    }

    if(len1 <len2)
        return -1;
    else if (len1 >len2)
        return 1;
    else
        return 0;
}

/*********************************************************************************************************
** Function name:           __iniFindCharInLine
**
** Descriptions:            ��ƫ����pos��ʼ ��ini�ļ����ַ�����������ƫ����pos�����в���ָ���ַ����˺����Զ�
							����������ע�͵�����
**
** input parameters:        strBuf          �ַ�������
                            iBufLen         �ַ�������ĳ���
                            wc              ָ���ַ�
                            pos             ָ����ƫ����
							��
** Returned value:          ����ֵ�����˲��ҵ��ĵ�һ��ָ���ַ���ƫ���������û�ҵ��򷵻�-1
                            �������������ini�ļ���ע�ͷ�';',Ҳ���ǲ����ע�͵�����
*********************************************************************************************************/
static int __iniFindCharInLine(LPCWSTR strBuf,int iBufLen,WCHAR wc,int pos)
{
    assert(pos<iBufLen);
    int i=pos;

    while(i<iBufLen && strBuf[i] != L'\n' && strBuf[i] != wc)
    {
        if (strBuf[i] == L';')
        {
            return -1;
        }
        i++;
    }

    if (i== iBufLen || strBuf[i] ==L'\n')
    {
        return -1;
    }
    return i;
}


/*********************************************************************************************************
** Function name:           __iniFindEndOfLine
**
** Descriptions:            �ҳ�ini�ļ����ַ�����������ƫ����pos�����������Ч�ַ���ƫ�������˺����Զ�
							����������ע�͵����ݡ�������ƫ����pos��ʼ�����ҡ�
**
** input parameters:        strBuf          �ַ�������
                            iBufLen         �ַ�������ĳ���                           
                            pos             ָ����ƫ����
							
** Returned value:          ����ֵ������ƫ����pos�����������Ч�ַ���ƫ����
							����-1��ʾ����ʧ�ܣ�ʧ�ܵ�ԭ����posΪ�ַ���ĩβ��ƫ����pos��Ϊ�����еĽ�β
*********************************************************************************************************/
static int __iniFindEndOfLine(LPCWSTR strBuf,int iBufLen,int pos)
{
    assert(pos<iBufLen);

	if(strBuf[pos] ==L'\n' || strBuf[pos] ==L'\r')
		return -1;

	while(pos< iBufLen)
	{
		if(strBuf[pos] ==L';' ||strBuf[pos] ==L'\n' || strBuf[pos] ==L'\r')
			break;
		pos++;
	}

	return pos-1;		
}

/*********************************************************************************************************
** Function name:           __SkipLine
**
** Descriptions:            ��һ���ַ�����������posƫ������ʼ������pos������һ�У�
                            ������һ�е�ƫ����
**
** input parameters:        strBuf          �ַ���������                        
** output parameters:       iBufLen         �ַ����������ĳ���
                            pos             ��posƫ�ƿ�ʼ����
                            pCodePage       �����ı��ļ�ԭ�����ַ�����
** Returned value:          ������һ�е�ƫ����

** Remarks:                 ����Ѵﵽ����ĩβ���򷵻�iBufLen

*********************************************************************************************************/
static int __SkipLine(LPCWSTR strBuf,int iBufLen,int pos)
{
    assert(pos<iBufLen);                        //��ֹƫ���������ַ�������

    int i=pos;
    while(i<iBufLen && strBuf[i] != L'\n')
    {
        i++;
    }

    if (i==iBufLen)
        return iBufLen;
    else
        return i+1;
}


static void __utf16BigTolittle(wchar_t *strBig,int iBigLen,wchar_t *strLittle=NULL,int iLittleLen=0)
{
    assert(strBig);
    if (strLittle)
    {
        assert(iLittleLen >iBigLen);
        assert(strLittle!=strBig);
        
        char *pBig=(char *)strBig;
        char *pLittle=(char *)strLittle;
        
        int i=0;
        while(i<iBigLen)
        {
            pLittle[2*i]=pBig[2*i+1];
            pLittle[2*i+1]=pBig[2*i];
            i++;
        }
        //strLittle[iBigLen]=0;
    }
    else
    {
        char *pBig=(char *)strBig;
        char *pLittle=(char *)strBig;

        char t;        
        int i=0;
        while(i<iBigLen)
        {
            t=pBig[2*i];
            pLittle[2*i]=pBig[2*i+1];
            pLittle[2*i+1]=t;
            i++;
        }
        //strLittle[iBigLen]=0;
    }
}


/*********************************************************************************************************
** Function name:           __utf16OpenTextFile
**
** Descriptions:            �Զ����Ʒ�ʽ��һ���ı��ļ�������������ת��ΪС��utf16���룬����ڻ�������
**
** input parameters:        strFileName     ����·����Ϣ���ı��ļ�����
**                          
** output parameters:       ppContext       ָ��һ��wchar_t�͵�ָ�룬���ڷ����ı����ַ���
                            pLen            ָ��һ�����������ز�������β��'\0'���ַ��ı��ĳ���
                            pCodePage       �����ı��ļ�ԭ�����ַ�����
** Returned value:          0               ��������ִ�гɹ�
                            ��0             ����ִ��ʧ��

** Remarks:                 ��֧��2GB��С���ϵ��ı��ļ�
                            ���ö����Ʒ�ʽ���ļ����ʻس��ܿ�����Ϊ"\r\n"
*********************************************************************************************************/
static int __utf16OpenTextFile(LPCWSTR strFileName,wchar_t **ppContext,DWORD *pLen,__CODEPAGE *pCodePage)
{
    HANDLE hFile;


    //���Դ��ļ�
    
    hFile=CreateFile(strFileName,GENERIC_READ,FILE_SHARE_READ,NULL,
        OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    
    if (hFile ==INVALID_HANDLE_VALUE)
    {
        return 1;
    }

    DWORD dwFileSize=0;

    //�����ж��ַ���
    unsigned char charSet[3];
    DWORD dwReadNum;
    __CODEPAGE codepage;
    if( !ReadFile(hFile,charSet,3,&dwReadNum,NULL) )
    {
        return 1;
    }

    if (dwReadNum==1)
    {
        codepage=CODEPAGE_ANSI;
    }
    else if (dwReadNum ==2)
    {
        BYTE t1[2]={0xFF,0xFE};
        BYTE t2[2]={0xFE,0xFF};
        //ΪС�˸�ʽ��UTF16?
        if (__MemCmp(t1,charSet,2)==0)
        {
            codepage=CODEPAGE_UTF16_LITTLE;
        }
        else if (__MemCmp(t2,charSet,2)==0)                          //Ϊ��˸�ʽ��utf16?
        {
            codepage=CODEPAGE_UTF16_BIG;
        }
        else
        {
            codepage=CODEPAGE_ANSI;
        }
    }
    else
    {
        BYTE t1[2]={0xFF,0xFE};
        BYTE t2[2]={0xFE,0xFF};
        BYTE t3[3]={0xEF,0xBB,0xBF};
        //ΪС�˸�ʽ��UTF16?
        if (__MemCmp(t1,charSet,2)==0)
        {
            codepage=CODEPAGE_UTF16_LITTLE;
        }
        else if (__MemCmp(t2,charSet,2)==0)                          //Ϊ��˸�ʽ��utf16?
        {
            codepage=CODEPAGE_UTF16_BIG;
        }
        else if(__MemCmp(t3,charSet,3)==0)
        {
            codepage=CODEPAGE_UTF8;
        }
        else
        {
            codepage=CODEPAGE_ANSI;
        }
    }

    //�����ַ��������Ӧ�����ļ��Ĵ�С ����λ�ļ�ָ�����ı��ļ���ʼ��
    dwFileSize=GetFileSize(hFile,NULL);
    switch (codepage)
    {
    case CODEPAGE_UTF16_BIG:
    case CODEPAGE_UTF16_LITTLE:
        dwFileSize-=2;
        SetFilePointer(hFile,2,0,FILE_BEGIN);
        break;
    case CODEPAGE_UTF8:
        dwFileSize-=3;
        SetFilePointer(hFile,3,0,FILE_BEGIN);
        break;
    default:
        SetFilePointer(hFile,0,0,FILE_BEGIN);
        break;
    }
	*pCodePage=codepage;

	//����ı��ļ�����Ϊ0���򷵻�ʧ��
	if (dwFileSize <=0)
	{
		CloseHandle(hFile);
		return 1;
	}

    //�����ı�����
    BYTE *pBuf=NULL;
    pBuf=(BYTE *)LocalAlloc(LMEM_FIXED,dwFileSize);
    if (pBuf==NULL)
    {
        goto OPEN_ERROR;
    }
	
    if(!ReadFile(hFile,pBuf,dwFileSize,&dwReadNum,NULL)  || (dwReadNum !=dwFileSize))
    {      
        goto OPEN_ERROR;
    }
    CloseHandle(hFile);

    switch (codepage)
    {
        case CODEPAGE_UTF16_LITTLE:
            *ppContext=(wchar_t *)pBuf;
            *pLen=dwFileSize/2;
            break;
        case CODEPAGE_UTF16_BIG:
            __utf16BigTolittle((wchar_t *)pBuf,dwFileSize/2);
            *ppContext=(wchar_t *)pBuf;
            *pLen=dwFileSize/2;
            break;
        default:
            {
                UINT cp = (codepage ==CODEPAGE_UTF8)?CP_UTF8:CP_ACP;
              

                int len=MultiByteToWideChar(cp,0,(char *)pBuf,dwFileSize,NULL,0);
                if (len==0)
                {
                    goto OPEN_ERROR;
                }
				
                wchar_t *pWideBuf=NULL;
                pWideBuf=(wchar_t *)LocalAlloc(LMEM_FIXED,len*sizeof(wchar_t));
                if (pWideBuf ==NULL)
                {
                    goto OPEN_ERROR;
                }
                MultiByteToWideChar(cp,0,(char *)pBuf,dwFileSize,(wchar_t *)pWideBuf,len);
                LocalFree(pBuf);
				pBuf=NULL;
                *ppContext=(wchar_t *)pWideBuf;
                *pLen=len;                
            }
            
            break;
    }   

    return 0;
OPEN_ERROR:
    CloseHandle(hFile);
    if (pBuf !=NULL)
    {
        LocalFree(pBuf);
    }
    return 1;
}

/*********************************************************************************************************
** Function name:           iniFindNextSection
**
** Descriptions:            ��һ����ini�ļ��ж������ַ�����������(���͵Ŀ���__utf16OpenTextFile�����õ�)��
                            ƫ����pos��ʼ������һ��SECTION�����еĿ�ͷ��ƫ����
**
** input parameters:        strINIBuf       ini�ļ����ַ�������
                            iBufLen         �ַ�������ĳ���                            
                            pos             ָ����ƫ����
��
** Returned value:          ����ֵ��������һ��SECTION�Ŀ�ʼ��λ�ã���'['��ʶ��λ�ã����ַ����е�ƫ����,
                            ����Ѵﵽ������ĩβ�򷵻�iBufLen
*********************************************************************************************************/
static int __iniFindNextSection(LPCWSTR strINIBuf,int iBufLen,int pos)
{
    int i=pos;

    while (i<iBufLen)
    {
        i=__SkipSpaceTab(strINIBuf,iBufLen,i);
        if (strINIBuf[i] != L'[')
        {
            i=__SkipLine(strINIBuf,iBufLen,i);
        }
        else
        {
            if(__iniFindCharInLine(strINIBuf,iBufLen,L']',i) != -1)
            {
                break;
            }
            else
            {
                i=__SkipLine(strINIBuf,iBufLen,i);
            }            
        }
    }
    return i;
}

/*********************************************************************************************************
** Function name:           __iniFindSection
**
** Descriptions:            ��һ����ini�ļ��ж������ַ�����������(���͵Ŀ���__utf16OpenTextFile�����õ�)��
                            ƫ����pos��ʼ������ΪstrSection��SECTION
**
** input parameters:        strINIBuf       ini�ļ����ַ�������
                            iBufLen         �ַ�������ĳ��ȣ���WCHAR�ĸ������㡣
                            strSection      ���ҵ�SECTION����(�ַ�����,������NUL��β��Ҳ���Բ�
							iSectionLen		SECTION���ֵĳ��ȣ���WCHAR�ĸ������㣬��Ӧ������β��NUL�ַ���
                            pos             ָ����ƫ����
��
** Returned value:          ����ֵ������ָ��SECTION�Ŀ�ʼ��λ�ã���'['��ʶ��λ�ã����ַ����е�ƫ����
                            ���û���ҵ����SECTION�򷵻�-1
*********************************************************************************************************/
static int __iniFindSection(LPCWSTR strINIBuf,int iBufLen,LPCWSTR strSection,int iSectionLen,int pos)
{
    assert(pos<iBufLen);

    int iSectionNameStop;    
    int iSectionNameStart2=0;
    int i=pos;   

    i=__iniFindNextSection(strINIBuf,iBufLen,i);
    while (i<iBufLen)
    {        
        iSectionNameStop=__iniFindCharInLine(strINIBuf,iBufLen,L']',i);
		
		if (__IgnoreSpaceTabStrcmp(strINIBuf+i+1, iSectionNameStop-i-1,strSection,iSectionLen) ==0)
		{
			break;
		}	

		//�������ȣ����������
		i=__SkipLine(strINIBuf,iBufLen,i);
		i=__iniFindNextSection(strINIBuf,iBufLen,i);        
    }

    if (i<iBufLen)
    {        
        return i;
    }
    return -1;
}

/*********************************************************************************************************
** Function name:           __iniFindKey
**
** Descriptions:            ��һ����ini�ļ��ж������ַ�����������(���͵Ŀ���__utf16OpenTextFile�����õ�)��
							����ΪstrSection��SECTION�в�����ΪstrKey��KEY
**
** input parameters:        strINIBuf       ini�ļ����ַ�������
                            iBufLen         �ַ�������ĳ���
							strSection		SECTION������
							iSectionLen		strSection�ĳ��ȣ���WCHAR�ĸ������㣬������ĩβ��'\0'
                            strKey          ���ҵ�Key��
							iKeyLen			Key���ĳ���


** output parameters:       pPos1			��ָ��ĵ�Ԫ���ڴ洢���ҵ�key�����ݵ���ʼƫ��
							pPos2			��ָ��ĵ�Ԫ���ڴ洢���ҵ�key�����ݵĽ���ƫ��

							�������key�ɹ���key������������Ϊ0����pPos2��ָ��ĵ�Ԫֵ��pPos1С1;
** Returned value:          ����ֵ�����˲����Ƿ�ɹ�
*********************************************************************************************************/
static BOOL __iniFindKey(LPCWSTR strINIBuf,int iBufLen,LPCWSTR strSection,int iSectionLen,LPCWSTR strKey,
				 int iKeyLen,int *pPos1,int *pPos2)
{
	assert(iBufLen>0);

	int iSectionStart=__iniFindSection(strINIBuf,iBufLen,strSection,iSectionLen,0);
	if (iSectionStart <0)
	{
		return 0;
	}

	int iSecKeyStart=__SkipLine(strINIBuf,iBufLen,iSectionStart);
	int iNextSectionStart=__iniFindNextSection(strINIBuf,iBufLen,iSectionStart+1);

	//����key�����п�ʼ�Ŀո��tab
	iSecKeyStart=__SkipSpaceTab(strINIBuf,iBufLen,iSecKeyStart);

	while(iSecKeyStart <iNextSectionStart)
	{		
		int t=__iniFindCharInLine(strINIBuf,iBufLen,L'=',iSecKeyStart);		//����=�ŵ�λ��



		if (t> 0)
		{
			if(__IgnoreSpaceTabStrcmp(strINIBuf+iSecKeyStart,t-iSecKeyStart,strKey,iKeyLen) 
				==0)															//���ҵ���Ӧ��key
			{
				*pPos1		=	__SkipSpaceTab(strINIBuf,iBufLen,t+1);
				int iPosTmp	=	__iniFindEndOfLine(strINIBuf,iBufLen,t+1);
				if (iPosTmp == -1)
				{
					*pPos2=*pPos1-1;
				}
				else
				{
					iPosTmp	=	__SkipSpaceTabForward(strINIBuf,iBufLen,iPosTmp);
					*pPos2=iPosTmp;
				}

				return TRUE;
			}
		}
		else																	//û�в��ҵ��Ⱥ�
		{
			
		}
		iSecKeyStart=__SkipLine(strINIBuf,iBufLen,iSecKeyStart);
		iSecKeyStart=__SkipSpaceTab(strINIBuf,iBufLen,iSecKeyStart);
	}	

    return FALSE;
}

DWORD WINAPI GetPrivateProfileString( LPCWSTR lpAppName, LPCWSTR lpKeyName,   LPCWSTR lpDefault,
                                        LPWSTR lpReturnedString,  DWORD nSize, LPCWSTR lpFileName )
{
	WCHAR		*pBuf;
	DWORD		iBufLen;
	__CODEPAGE	codePage;

	DWORD			nRet=0;	

	assert(nSize>0);
	assert(lpAppName);
	assert(lpKeyName);
	assert(lpDefault);
	assert(lpFileName);
	if (__utf16OpenTextFile(lpFileName,&pBuf,&iBufLen, &codePage) !=0)
		return 0;

	int pos1,pos2;
	if (__iniFindKey(pBuf,iBufLen,lpAppName ,(int)wcslen(lpAppName),
			lpKeyName,(int)wcslen(lpKeyName),&pos1,&pos2) )
	{		
		DWORD len=pos2-pos1+1;
		nRet=  (len+1)>nSize? nSize-1:len;
		memcpy(lpReturnedString,pBuf+pos1,sizeof(WCHAR)*nRet);
		lpReturnedString[nRet]=0;
	}
	else
	{
		nRet=(DWORD)wcslen(lpDefault);
		memcpy(lpReturnedString,lpDefault,sizeof(WCHAR)*nRet);
		lpReturnedString[nRet]=0;
	}

	LocalFree(pBuf);
	return nRet;
}

UINT WINAPI GetPrivateProfileInt(LPCTSTR lpSectionName,LPCTSTR lpKeyName,  INT nDefault,LPCTSTR lpFileName)
{
	UINT nRet;
	WCHAR *pNumBuf=NULL;

	WCHAR		*pBuf;
	DWORD		iBufLen;
	__CODEPAGE	codePage;	
	

	if (__utf16OpenTextFile(lpFileName,&pBuf,&iBufLen, &codePage) !=0)
		return 0;
	
	int pos1,pos2;
	if (__iniFindKey(pBuf,iBufLen,lpSectionName ,(int)wcslen(lpSectionName),
		lpKeyName,(int)wcslen(lpKeyName),&pos1,&pos2) )
	{
		nRet = StrToInt(&pBuf[pos1],pos2-pos1+1);
	}
	else
	{
		nRet=nDefault;
	}

	LocalFree(pBuf);
	return nRet;
}

/*********************************************************************************************************
** Function name:           __WriteProfileInEndOfFile
**
** Descriptions:            ��һ���ļ��Ľ�βֱ�Ӱ�ini�ļ��ı�׼д��ָ����SECTION��Key
**
** input parameters:        hFile			ָ�����ļ����
                            lpSectionName   SECTION������
                            lpKeyName		Key������
							lpString		Key��ֵ
                            codePage        ָ���ı��룬
** Returned value:          ����ֵ������д���Ƿ�ɹ�
*********************************************************************************************************/
static BOOL __WriteProfileKey(HANDLE hFile,LPCWSTR lpKeyName,LPCWSTR lpString,
									  __CODEPAGE codePage)
{
	DWORD dwWriteByte;
	DWORD iNeedWrite;	
	
	iNeedWrite=sizeof(WCHAR)*(DWORD)wcslen(lpKeyName);
	if (!WriteFile(hFile,lpKeyName,iNeedWrite,&dwWriteByte,NULL) || (dwWriteByte <iNeedWrite))
	{
		goto WRITE_ERR1;
	}
	iNeedWrite=sizeof(WCHAR)*1;
	if (!WriteFile(hFile,L"=",iNeedWrite,&dwWriteByte,NULL) || (dwWriteByte <iNeedWrite))
	{
		goto WRITE_ERR1;
	}
	iNeedWrite=sizeof(WCHAR)*(DWORD)wcslen(lpString);
	assert(iNeedWrite);
	if (!WriteFile(hFile,lpString,iNeedWrite,&dwWriteByte,NULL) || (dwWriteByte <iNeedWrite))
	{
		goto WRITE_ERR1;
	}
	iNeedWrite=sizeof(WCHAR)*2;
	if( (!WriteFile(hFile,L"\r\n",iNeedWrite,&dwWriteByte,NULL)) || (dwWriteByte <iNeedWrite))		//д��س�����
	{
		goto WRITE_ERR1;			
	}
	
	return TRUE;	
WRITE_ERR1:
	return FALSE;	
} 




/*********************************************************************************************************
** Function name:           __WriteProfileInEndOfFile
**
** Descriptions:            ��һ���ļ��Ľ�βֱ�Ӱ�ini�ļ��ı�׼д��ָ����SECTION��Key
**
** input parameters:        hFile			ָ�����ļ����
                            lpSectionName   SECTION������
                            lpKeyName		Key������
							lpString		Key��ֵ
                            codePage        ָ���ı��룬
** Returned value:          ����ֵ������д���Ƿ�ɹ�
*********************************************************************************************************/
static BOOL __WriteProfileInEndOfFile(HANDLE hFile,LPCWSTR lpSectionName,LPCWSTR lpKeyName,LPCWSTR lpString,
									  __CODEPAGE codePage)
{
	DWORD dwWriteByte;
	DWORD iNeedWrite;
	
	SetFilePointer(hFile,0L,NULL,FILE_END);
	iNeedWrite=sizeof(WCHAR)*1;
	if (!WriteFile(hFile,L"[",iNeedWrite,&dwWriteByte,NULL) || (dwWriteByte <iNeedWrite) )
	{
		goto WRITE_ERR1;
	}
	iNeedWrite=sizeof(WCHAR)*(DWORD)wcslen(lpSectionName);
	if (!WriteFile(hFile,lpSectionName,iNeedWrite,&dwWriteByte,NULL) || (dwWriteByte <iNeedWrite) )
	{
		goto WRITE_ERR1;
	}
	iNeedWrite=sizeof(WCHAR)*1;
	if (!WriteFile(hFile,L"]",iNeedWrite,&dwWriteByte,NULL) || (dwWriteByte <iNeedWrite) )
	{
		goto WRITE_ERR1;
	}
	iNeedWrite=sizeof(WCHAR)*2;
	if( (!WriteFile(hFile,L"\r\n",iNeedWrite,&dwWriteByte,NULL)) || (dwWriteByte <iNeedWrite))		//д��س�����
	{
		goto WRITE_ERR1;			
	}
	
	return __WriteProfileKey(hFile,lpKeyName,lpString,codePage);
	
WRITE_ERR1:
	return FALSE;	
} 




BOOL WINAPI WritePrivateProfileString(LPCWSTR lpSectionName,LPCWSTR lpKeyName,LPCWSTR lpString,LPCWSTR lpFileName)
{
	WCHAR		*pBuf;
	DWORD		iBufLen;
	__CODEPAGE	codePage;

	int iRet=__utf16OpenTextFile(lpFileName,&pBuf,&iBufLen, &codePage);

	HANDLE hFile;
	hFile=CreateFileW(lpFileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
	if (hFile==INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD dwWriteByte;
	BYTE t[2]={0xFF,0xFE};															//С��UNICODE�ı��ļ� �ļ�ͷ	
	
	if( (!WriteFile(hFile,t,2,&dwWriteByte,NULL)) || (dwWriteByte <2))				//д���ļ�ͷ
	{
		goto WRITE_ERR1;			
	}


	if(iRet !=0)																	//�����ini�ļ�ʧ�ܣ���ֱ��д��������Ϣ
	{		
		BOOL bRet=__WriteProfileInEndOfFile(hFile,lpSectionName,lpKeyName,lpString,CODEPAGE_UTF16_LITTLE);
		CloseHandle(hFile);
		return bRet;
		
	}
	else
	{
	
		int pos1,pos2;
		int secPos;
		DWORD dwNeedWrite;

		if ( __iniFindKey(pBuf,iBufLen,lpSectionName,(int)wcslen(lpSectionName),lpKeyName,(int)wcslen(lpKeyName),&pos1,&pos2) )
		{
			dwNeedWrite=sizeof(WCHAR)*pos1;			
			if (!WriteFile(hFile,pBuf,dwNeedWrite,&dwWriteByte,NULL) || (dwWriteByte <dwNeedWrite) )
			{
				goto WRITE_ERR1;
			}
			dwNeedWrite=sizeof(WCHAR)*(DWORD)wcslen(lpString);
			if (!WriteFile(hFile,lpString,dwNeedWrite,&dwWriteByte,NULL) || (dwWriteByte <dwNeedWrite))
			{
				goto WRITE_ERR1;
			}
			dwNeedWrite=(iBufLen-pos2-1)*sizeof(WCHAR);
			if (!WriteFile(hFile,pBuf+pos2+1,dwNeedWrite,&dwWriteByte,NULL) || (dwWriteByte <dwNeedWrite) )
			{
				goto WRITE_ERR1;
			}
		}

		else if( (secPos= __iniFindSection(pBuf,iBufLen,lpSectionName,(int)wcslen(lpSectionName),0)) !=-1 ) 
		{
			int t;
			t=__SkipLine(pBuf,iBufLen,secPos);			
			dwNeedWrite=sizeof(WCHAR)*t;

			if (!WriteFile(hFile,pBuf,dwNeedWrite,&dwWriteByte,NULL) || (dwWriteByte <dwNeedWrite) )
			{
				goto WRITE_ERR1;
			}
			if (!__WriteProfileKey(hFile,lpKeyName,lpString,codePage))
			{
				goto WRITE_ERR1;
			}
			dwNeedWrite=(iBufLen-t)*sizeof(WCHAR);
			if (!WriteFile(hFile,pBuf+t,dwNeedWrite,&dwWriteByte,NULL) || (dwWriteByte <dwNeedWrite) )
			{
				goto WRITE_ERR1;
			}
		}

		else
		{
			dwNeedWrite	=sizeof(WCHAR) *iBufLen;
			if (!WriteFile(hFile,pBuf,dwNeedWrite,&dwWriteByte,NULL) || (dwWriteByte <dwNeedWrite) )
			{
				goto WRITE_ERR1;
			}
			if (!__WriteProfileInEndOfFile(hFile,lpSectionName,lpKeyName,lpString,codePage))
			{
				goto WRITE_ERR1;
			}
		}

        LocalFree(pBuf);            
		CloseHandle(hFile);
		return TRUE;
WRITE_ERR1:
		CloseHandle(hFile);
        LocalFree(pBuf);
		return FALSE;	
	}
}

BOOL WINAPI WritePrivateProfileInt(LPCWSTR lpSectionName,LPCWSTR lpKeyName,int iValue,LPCWSTR lpFileName)
{
	WCHAR strValue[50];

	intToStr(strValue,50,iValue);
	return WritePrivateProfileString(lpSectionName,lpKeyName,strValue,lpFileName);
}

double WINAPI GetPrivateProfileDouble(LPCTSTR lpSectionName,LPCTSTR lpKeyName, double dDefault,LPCTSTR lpFileName)
{
	WCHAR strValue[50];
	double t;
	GetPrivateProfileString(lpSectionName,lpKeyName,L"",strValue,50,lpFileName);

	if(strValue[0]!=0)
	{
		t=strToDuble(strValue,50);
		return t;
	}
	else
		return dDefault;		
}



BOOL WINAPI WritePrivateProfileDouble(LPCWSTR lpSectionName,LPCWSTR lpKeyName,double dValue,LPCWSTR lpFileName)
{
	WCHAR strValue[50];	
    DoubleToStr(dValue,strValue,50);
	return WritePrivateProfileString(lpSectionName,lpKeyName,strValue,lpFileName);
}