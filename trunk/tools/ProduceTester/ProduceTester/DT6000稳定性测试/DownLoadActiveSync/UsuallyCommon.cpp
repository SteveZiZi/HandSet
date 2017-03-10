#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <string>
#include <assert.h>

using namespace std;

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

int StrToInt(LPCWSTR str,int strlen)
{
    int		t=0;
    BOOL	bMinusFlag=FALSE;
    int		i=__SkipSpaceTab(str,strlen,0);
    
    if(str[i] ==L'-')
    {
        bMinusFlag=TRUE;
        i=__SkipSpaceTab(str,strlen,i+1);
    }
    
    while(i<strlen)
    {
        if (__IsNumber(str[i]))
        {
            t=t*10+( str[i]-L'0');
        }
        else
            break;
        i++;
    }
    if (bMinusFlag)
    {
        t=0-t;
    }
    
    return t;
}


double strToDuble(WCHAR *strDouble,int len)
{
	double	t=0;
	BOOL	bNegative=FALSE;

	int pos=__SkipSpaceTab(strDouble,len,0);
	if (strDouble[pos]==L'-')
	{
		pos++;
		bNegative=TRUE;
	}
	int idotNum=0;									//小数点的数目

	UINT32 iFloat=0;								//小数部分的整数
	UINT32 icount=0;

	while(pos <len)
	{
		if ( __IsNumber(strDouble[pos]) )
		{
			if(idotNum >0)
			{
				//这里简要说明一下小数部分的算法
				//iFloat数组每项记录下每8个十进制位形成的整数，如0.1234，形成1234
				iFloat*=10;
				iFloat+=(strDouble[pos] -L'0');
				icount++;
				if(icount>=9)
					break;
			}
			else
			{
				t*=10;
				t+=strDouble[pos]-L'0';
			}
		}
		else if (strDouble[pos] ==L'.')
		{
			idotNum++;
			if(idotNum>1)
				break;
		}
		else
		{
			break;
		}
		pos ++;
	}
	t+=(double)iFloat/pow(10.0,(int)icount);



	return t;
}

/*********************************************************************************************************
** Function name:           DoubleToStr
**
** Descriptions:            将一个双精度浮点数按需要的精度四舍五入转化为字符串
**
** input parameters:        dValue			指定的双精度浮点数
                            pBuf            用于存储字符串的缓冲
                            iBufLen	        缓冲区的长度
							iPrecision      转换的精度，如果为0，则视为自动精度

** Returned value:          返回值表明了写入到缓冲区的字符数，包括结尾的NUL字符
                            如果为0，则表明给定的缓冲区太小
** Remarks                  显然的，如果0.999999以5位精度执行此函数,则返回字符串"1.00000"
                            自动精度会把精度设定为10，但会自动去掉末尾的0，同样的，自动精度的话
                            ，0.999999返回0.999999
*********************************************************************************************************/
int DoubleToStr(double dValue,WCHAR *pBuf,int iBufLen,int  iPrecision=0)
{
    assert(iBufLen>0);
    assert(iPrecision <=20);
    assert(iPrecision>=0);
    WCHAR strInt[256];           //存储整数部分的字符串
    WCHAR strFloat[256];         //存储小数部分的字符串

    int     iIntLen=0;
    int     iFloatLen=0;
    BOOL    bNegative=dValue<0?TRUE:FALSE;
    int     t;   

    //将负数变成正数
    if (bNegative)
    {
        dValue=0-dValue;
    }
    //确定精度
    int precision=iPrecision;
    if (iPrecision ==0)
    {
        precision=10;
    }

    //分离整数部分和小数部分
    int iValue=(int)dValue;
    dValue=dValue-iValue;

    //将小数部分转化为字符串
    while(precision >0)
    {
        precision --;
        dValue*=10;
        t=(int)dValue;
        dValue-=t;
        
        strFloat[iFloatLen++]=L'0'+t;
    }

    //判断四舍五入的进位关系
    dValue*=10;
    t=(int)dValue;
    BOOL bCF=FALSE;
    int i=iFloatLen-1;

    if (t>=5)
    {
        bCF=TRUE;
    }
    while (bCF && i>=0)
    {
        if (strFloat[i]<L'9')
        {
            bCF=FALSE;
            strFloat[i]++;
        }
        else
        {
            strFloat[i]=L'0';
        }
        i--;
    }
    //进位是否影响到整数
    if(bCF)
    {
        iValue++;  
    }
 
    //如果是自动精度，则去掉小数部分末尾的0
    if (iPrecision==0 )
    {
        while(strFloat[iFloatLen-1]==L'0' && iFloatLen >0)
        {
            iFloatLen--;
        }
    }

    //将整数部分转化为字符串，这是倒序的，即个位在前面
    do 
    {
        t=iValue%10;
        iValue/=10;
        strInt[iIntLen++]=L'0'+t;

    } while (iValue>0);

    //计算存储这些字符串需要的长度
    int len=0;
    if (bNegative)
    {
        len++;
    }
    len+=iIntLen;
    len+=iFloatLen;
    if (iFloatLen>0)
    {
        len++;              //还要加上小数点
    }
    len++;                  //加上结尾的NUL字符

    if (len>iBufLen)
    {
        return 0;
    }

    //将生成的字符串填充至缓冲
    int pos=0;
    if (bNegative)
    {
        pBuf[pos++]=L'-';
    }
    //复制整数部分
    for (i=0;i<iIntLen;i++)
    {
        pBuf[pos++]=strInt[iIntLen-1-i];
    }
    if(iFloatLen>0)
    {
        pBuf[pos++]=L'.';
    }
    //复制小数部分
    for (i=0;i<iFloatLen;i++)
    {
        pBuf[pos++]=strFloat[i];
    }
    pBuf[pos]=0;
    
    return len;
}

int intToStr(WCHAR *pStrValue,int len,int iValue)
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

    int iNeedLen=pos;       //需存储的字符串的长度，不包括结尾NUL
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

BOOL IsFileExist(const WCHAR *strFilePath)
{
    FILE *fp;
    assert(strFilePath!=NULL);

    fp=_wfopen(strFilePath,L"rb");
    if (fp==NULL)
    {
        return FALSE;
    }

    fclose(fp);
    return TRUE;
}

int GetExeDirectory(WCHAR *strPath, int len)
{
    int         i;
    WCHAR       strFilePath[MAX_PATH*2];

    //获取可执行文件所在的路径
    i=GetModuleFileName(NULL,strFilePath,MAX_PATH);
    while(i>0 && strFilePath[i]!=L'\\' )
    {
        i--;
    }
    if (i<=0)
        return FALSE;
    i++;
    strFilePath[i]=0;
    
    if (len <i+1)
    {
        return 0;
    }

    memcpy(strPath,strFilePath,(i+i)*sizeof(WCHAR));    
    return i;
}

int  GetDirFileCountAll(const WCHAR *strDirectory)
{
    wstring     strFindPath=strDirectory;
    wstring     strFind,strFindTemp;
    HANDLE      hFind;
    BOOL        bFind=FALSE;
    WIN32_FIND_DATA wfd;

    int         count=0;
    strFind=strFindPath+L"*.*";
    hFind=FindFirstFile(strFind.c_str(),&wfd);

    if (hFind !=INVALID_HANDLE_VALUE)
    {
        do 
        {
            if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY  )
            {
                if (wfd.cFileName[0]!=L'.' )
                {
                    strFindTemp=strFindPath+wfd.cFileName;
                    strFindTemp+=L"\\";
                    count+=GetDirFileCountAll(strFindTemp.c_str());
                }
            }
            else
            {
                count++;
            }

            bFind=FindNextFile(hFind,&wfd);
        } while (bFind);
        FindClose(hFind);
        hFind=INVALID_HANDLE_VALUE;
    }

    return count;
}