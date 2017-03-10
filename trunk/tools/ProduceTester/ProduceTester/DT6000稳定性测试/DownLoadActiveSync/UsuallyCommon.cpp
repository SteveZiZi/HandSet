#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <string>
#include <assert.h>

using namespace std;

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
	int idotNum=0;									//С�������Ŀ

	UINT32 iFloat=0;								//С�����ֵ�����
	UINT32 icount=0;

	while(pos <len)
	{
		if ( __IsNumber(strDouble[pos]) )
		{
			if(idotNum >0)
			{
				//�����Ҫ˵��һ��С�����ֵ��㷨
				//iFloat����ÿ���¼��ÿ8��ʮ����λ�γɵ���������0.1234���γ�1234
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
** Descriptions:            ��һ��˫���ȸ���������Ҫ�ľ�����������ת��Ϊ�ַ���
**
** input parameters:        dValue			ָ����˫���ȸ�����
                            pBuf            ���ڴ洢�ַ����Ļ���
                            iBufLen	        �������ĳ���
							iPrecision      ת���ľ��ȣ����Ϊ0������Ϊ�Զ�����

** Returned value:          ����ֵ������д�뵽���������ַ�����������β��NUL�ַ�
                            ���Ϊ0������������Ļ�����̫С
** Remarks                  ��Ȼ�ģ����0.999999��5λ����ִ�д˺���,�򷵻��ַ���"1.00000"
                            �Զ����Ȼ�Ѿ����趨Ϊ10�������Զ�ȥ��ĩβ��0��ͬ���ģ��Զ����ȵĻ�
                            ��0.999999����0.999999
*********************************************************************************************************/
int DoubleToStr(double dValue,WCHAR *pBuf,int iBufLen,int  iPrecision=0)
{
    assert(iBufLen>0);
    assert(iPrecision <=20);
    assert(iPrecision>=0);
    WCHAR strInt[256];           //�洢�������ֵ��ַ���
    WCHAR strFloat[256];         //�洢С�����ֵ��ַ���

    int     iIntLen=0;
    int     iFloatLen=0;
    BOOL    bNegative=dValue<0?TRUE:FALSE;
    int     t;   

    //�������������
    if (bNegative)
    {
        dValue=0-dValue;
    }
    //ȷ������
    int precision=iPrecision;
    if (iPrecision ==0)
    {
        precision=10;
    }

    //�����������ֺ�С������
    int iValue=(int)dValue;
    dValue=dValue-iValue;

    //��С������ת��Ϊ�ַ���
    while(precision >0)
    {
        precision --;
        dValue*=10;
        t=(int)dValue;
        dValue-=t;
        
        strFloat[iFloatLen++]=L'0'+t;
    }

    //�ж���������Ľ�λ��ϵ
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
    //��λ�Ƿ�Ӱ�쵽����
    if(bCF)
    {
        iValue++;  
    }
 
    //������Զ����ȣ���ȥ��С������ĩβ��0
    if (iPrecision==0 )
    {
        while(strFloat[iFloatLen-1]==L'0' && iFloatLen >0)
        {
            iFloatLen--;
        }
    }

    //����������ת��Ϊ�ַ��������ǵ���ģ�����λ��ǰ��
    do 
    {
        t=iValue%10;
        iValue/=10;
        strInt[iIntLen++]=L'0'+t;

    } while (iValue>0);

    //����洢��Щ�ַ�����Ҫ�ĳ���
    int len=0;
    if (bNegative)
    {
        len++;
    }
    len+=iIntLen;
    len+=iFloatLen;
    if (iFloatLen>0)
    {
        len++;              //��Ҫ����С����
    }
    len++;                  //���Ͻ�β��NUL�ַ�

    if (len>iBufLen)
    {
        return 0;
    }

    //�����ɵ��ַ������������
    int pos=0;
    if (bNegative)
    {
        pBuf[pos++]=L'-';
    }
    //������������
    for (i=0;i<iIntLen;i++)
    {
        pBuf[pos++]=strInt[iIntLen-1-i];
    }
    if(iFloatLen>0)
    {
        pBuf[pos++]=L'.';
    }
    //����С������
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

    //��ȡ��ִ���ļ����ڵ�·��
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