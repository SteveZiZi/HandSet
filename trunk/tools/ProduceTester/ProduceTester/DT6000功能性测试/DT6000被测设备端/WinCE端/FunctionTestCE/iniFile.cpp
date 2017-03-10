/*********************************************************************************************************
** File name:				iniFile.cpp
**
** Descriptions:            实现与Win32下操纵ini文件一组函数，与Win32同样功能的函数拥有相同的声明和表现，
							方便在WinCE上系统上使用，也便于以后微软在Win CE上提供这一组函数之后，可以很方
							地使用微软的实现。
							这里所实现的函数都可以在Win32环境下使用，但注意别与微软的实现出现命名冲突
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
** Descriptions:            从一个字符串缓冲区中pos偏移量开始（包括pos）向前跳过空格和tab控制符，
                            返回pos开始第一个不为空格的字符的偏移量
**
** input parameters:        strBuf          字符串缓冲区                        
** output parameters:       iBufLen         字符串缓冲区的长度
                            pos             从pos偏移开始跳过                           
** Returned value:          返回pos开始第一个不为空格的字符的偏移量

** Remarks:                 如果从偏移量pos开始向前，字符串全为空格，则返回值为-1
*********************************************************************************************************/
static int __SkipSpaceTabForward(LPCWSTR strBuf,int iBufLen,int pos)
{
    assert(pos<iBufLen);                        //禁止偏移量大于字符串长度

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
** Descriptions:            从一个字符串缓冲区中pos偏移量开始（包括pos）跳过空格和tab控制符，
                            返回pos开始第一个不为空格的字符的偏移量
**
** input parameters:        strBuf          字符串缓冲区                        
** output parameters:       iBufLen         字符串缓冲区的长度
                            pos             从pos偏移开始跳过
** Returned value:          返回pos开始第一个不为空格的字符的偏移量

** Remarks:                 如果从偏移量pos开始，字符串全为空格，则返回值为iBufLen
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
** Descriptions:            忽略掉两个字符串头部和尾部的空格及tab控制符 再进行字符串比较
**                          典型的"people" 等于"   people       "
** input parameters:        pStr1			字符串1
                            iStr1Len		字符串1的长度
                            pStr2			字符串2
                            iStr2Len		字符串2的长度
，
** Returned value:          如果字符串1大于字符串2，返回值大于0
                            如果字符串1小于字符串2，返回值小于0
                            相等则返回0
*********************************************************************************************************/
static int __IgnoreSpaceTabStrcmp(LPCWSTR pStr1,int iStr1Len,LPCWSTR pStr2,int iStr2Len)
{
    int iStart1=0,iStart2=0;
    int iStop1=iStr1Len-1;
    int iStop2=iStr2Len-1;
    int len,len1,len2;                  //记录需要比较的字符串长度，忽略空格后字符串1和字符串2的长度
    
    assert(iStr1Len>=0);
    assert(iStr2Len>=0);

    //尝试过滤空格和tab控制符
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

    //计算忽略空格和tab后各字符串的长度和需要比较的长度
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
** Descriptions:            从偏移量pos开始 从ini文件的字符串缓冲区中偏移量pos所在行查找指定字符，此函数自动
							忽略所在行注释的内容
**
** input parameters:        strBuf          字符串缓冲
                            iBufLen         字符串缓冲的长度
                            wc              指定字符
                            pos             指定的偏移量
							，
** Returned value:          返回值表明了查找到的第一个指定字符的偏移量，如果没找到则返回-1
                            这个函数回跳过ini文件的注释符';',也就是不检查注释的内容
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
** Descriptions:            找出ini文件的字符串缓冲区中偏移量pos所在行最后有效字符的偏移量，此函数自动
							忽略所在行注释的内容。函数从偏移量pos开始向后查找。
**
** input parameters:        strBuf          字符串缓冲
                            iBufLen         字符串缓冲的长度                           
                            pos             指定的偏移量
							
** Returned value:          返回值表明了偏移量pos所在行最后有效字符的偏移量
							返回-1表示查找失败，失败的原因是pos为字符串末尾或偏移量pos即为所在行的结尾
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
** Descriptions:            从一个字符串缓冲区中pos偏移量开始（包括pos）跳过一行，
                            返回下一行的偏移量
**
** input parameters:        strBuf          字符串缓冲区                        
** output parameters:       iBufLen         字符串缓冲区的长度
                            pos             从pos偏移开始跳过
                            pCodePage       返回文本文件原本的字符集，
** Returned value:          返回下一行的偏移量

** Remarks:                 如果已达到缓冲末尾，则返回iBufLen

*********************************************************************************************************/
static int __SkipLine(LPCWSTR strBuf,int iBufLen,int pos)
{
    assert(pos<iBufLen);                        //禁止偏移量大于字符串长度

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
** Descriptions:            以二进制方式打开一个文本文件，并将其内容转换为小端utf16编码，存放于缓冲区中
**
** input parameters:        strFileName     包含路径信息的文本文件名称
**                          
** output parameters:       ppContext       指向一个wchar_t型的指针，用于返回文本的字符串
                            pLen            指向一个整数，返回不包含结尾的'\0'宽字符文本的长度
                            pCodePage       返回文本文件原本的字符集，
** Returned value:          0               表明函数执行成功
                            非0             函数执行失败

** Remarks:                 不支持2GB大小以上的文本文件
                            采用二进制方式打开文件，故回车很可能仍为"\r\n"
*********************************************************************************************************/
static int __utf16OpenTextFile(LPCWSTR strFileName,wchar_t **ppContext,DWORD *pLen,__CODEPAGE *pCodePage)
{
    HANDLE hFile;


    //尝试打开文件
    
    hFile=CreateFile(strFileName,GENERIC_READ,FILE_SHARE_READ,NULL,
        OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    
    if (hFile ==INVALID_HANDLE_VALUE)
    {
        return 1;
    }

    DWORD dwFileSize=0;

    //尝试判断字符集
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
        //为小端格式的UTF16?
        if (__MemCmp(t1,charSet,2)==0)
        {
            codepage=CODEPAGE_UTF16_LITTLE;
        }
        else if (__MemCmp(t2,charSet,2)==0)                          //为大端格式的utf16?
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
        //为小端格式的UTF16?
        if (__MemCmp(t1,charSet,2)==0)
        {
            codepage=CODEPAGE_UTF16_LITTLE;
        }
        else if (__MemCmp(t2,charSet,2)==0)                          //为大端格式的utf16?
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

    //根据字符集计算出应读出文件的大小 并定位文件指针至文本文件开始处
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

	//如果文本文件长度为0，则返回失败
	if (dwFileSize <=0)
	{
		CloseHandle(hFile);
		return 1;
	}

    //读入文本内容
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
** Descriptions:            从一个由ini文件中读出的字符串缓冲区中(典型的可由__utf16OpenTextFile函数得到)从
                            偏移量pos开始查找下一个SECTION所在行的开头的偏移量
**
** input parameters:        strINIBuf       ini文件的字符串缓冲
                            iBufLen         字符串缓冲的长度                            
                            pos             指定的偏移量
，
** Returned value:          返回值表明了下一个SECTION的开始的位置，即'['标识的位置，在字符串中的偏移量,
                            如果已达到缓冲区末尾则返回iBufLen
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
** Descriptions:            从一个由ini文件中读出的字符串缓冲区中(典型的可由__utf16OpenTextFile函数得到)从
                            偏移量pos开始查找名为strSection的SECTION
**
** input parameters:        strINIBuf       ini文件的字符串缓冲
                            iBufLen         字符串缓冲的长度，以WCHAR的个数计算。
                            strSection      查找的SECTION名字(字符串）,可以以NUL结尾，也可以不
							iSectionLen		SECTION名字的长度，以WCHAR的个数计算，不应包含结尾的NUL字符。
                            pos             指定的偏移量
，
** Returned value:          返回值表明了指定SECTION的开始的位置，即'['标识的位置，在字符串中的偏移量
                            如果没有找到这个SECTION则返回-1
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

		//如果不相等，则继续查找
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
** Descriptions:            从一个由ini文件中读出的字符串缓冲区中(典型的可由__utf16OpenTextFile函数得到)查
							找名为strSection的SECTION中查找名为strKey的KEY
**
** input parameters:        strINIBuf       ini文件的字符串缓冲
                            iBufLen         字符串缓冲的长度
							strSection		SECTION的名字
							iSectionLen		strSection的长度，以WCHAR的个数计算，不包含末尾的'\0'
                            strKey          查找的Key名
							iKeyLen			Key名的长度


** output parameters:       pPos1			所指向的单元用于存储查找的key的数据的起始偏移
							pPos2			所指向的单元用于存储查找的key的数据的结束偏移

							如果查找key成功但key的数据区长度为0，则pPos2所指向的单元值比pPos1小1;
** Returned value:          返回值表明了查找是否成功
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

	//跳过key所在行开始的空格和tab
	iSecKeyStart=__SkipSpaceTab(strINIBuf,iBufLen,iSecKeyStart);

	while(iSecKeyStart <iNextSectionStart)
	{		
		int t=__iniFindCharInLine(strINIBuf,iBufLen,L'=',iSecKeyStart);		//查找=号的位置



		if (t> 0)
		{
			if(__IgnoreSpaceTabStrcmp(strINIBuf+iSecKeyStart,t-iSecKeyStart,strKey,iKeyLen) 
				==0)															//查找到相应的key
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
		else																	//没有查找到等号
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
** Descriptions:            在一个文件的结尾直接按ini文件的标准写入指定的SECTION和Key
**
** input parameters:        hFile			指定的文件句柄
                            lpSectionName   SECTION的名字
                            lpKeyName		Key的名字
							lpString		Key的值
                            codePage        指定的编码，
** Returned value:          返回值表明了写入是否成功
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
	if( (!WriteFile(hFile,L"\r\n",iNeedWrite,&dwWriteByte,NULL)) || (dwWriteByte <iNeedWrite))		//写入回车符号
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
** Descriptions:            在一个文件的结尾直接按ini文件的标准写入指定的SECTION和Key
**
** input parameters:        hFile			指定的文件句柄
                            lpSectionName   SECTION的名字
                            lpKeyName		Key的名字
							lpString		Key的值
                            codePage        指定的编码，
** Returned value:          返回值表明了写入是否成功
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
	if( (!WriteFile(hFile,L"\r\n",iNeedWrite,&dwWriteByte,NULL)) || (dwWriteByte <iNeedWrite))		//写入回车符号
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
	BYTE t[2]={0xFF,0xFE};															//小端UNICODE文本文件 文件头	
	
	if( (!WriteFile(hFile,t,2,&dwWriteByte,NULL)) || (dwWriteByte <2))				//写入文件头
	{
		goto WRITE_ERR1;			
	}


	if(iRet !=0)																	//如果打开ini文件失败，则直接写入配置信息
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