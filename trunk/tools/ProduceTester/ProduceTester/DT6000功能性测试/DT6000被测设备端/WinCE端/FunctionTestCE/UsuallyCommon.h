#ifndef __USUALLY_COMMMON_H__
#define __USUALLY_COMMMON_H__

double strToDuble(WCHAR *strDouble,int len);
int StrToInt(LPCWSTR str,int strlen);
/*********************************************************************************************************
** Function name:           DoubleToStr
**
** Descriptions:            将一个双精度浮点数按需要的精度四舍五入转化为字符串
**
** input parameters:        dValue              指定的双精度浮点数
                            pBuf                用于存储字符串的缓冲
                            iBufLen             缓冲区的长度
                            iPrecision          转换的精度，如果为0，则视为自动精度

** Returned value:          返回值表明了写入到缓冲区的字符数，包括结尾的NUL字符
                            如果为0，则表明给定的缓冲区太小
** Remarks                  显然的，如果0.999999以5位精度执行此函数,则返回字符串"1.00000"
                            自动精度会把精度设定为10，但会自动去掉末尾的0，同样的，自动精度的话
                            ，0.999999返回0.999999
*********************************************************************************************************/
int DoubleToStr(double dValue,WCHAR *pBuf,int iBufLen,int  iPrecision=0);
int intToStr(WCHAR *pStrValue,int len,int iValue);

//指定的文件是否存在
BOOL IsFileExist(const WCHAR *strFilePath);

/*********************************************************************************************************
** Function name:           GetExeDirectory
**
** Descriptions:            获取运行程序的可执行文件所在的路径
**
** input parameters:        strPath  存储返回的表示的路径字符串的缓冲区，
                            len      strPath所指向的缓冲区的长度
** Returned value:          返回实际复制到strPath缓冲区的字符数，不包括结尾的NUL
                            返回0，表示缓冲区过小
*********************************************************************************************************/
int  GetExeDirectory(WCHAR *strPath, int len);

//获取指定文件夹下及其子文件夹下的所有文件的个数
int  GetDirFileCountAll(const WCHAR *strDirectory);

#endif
 
