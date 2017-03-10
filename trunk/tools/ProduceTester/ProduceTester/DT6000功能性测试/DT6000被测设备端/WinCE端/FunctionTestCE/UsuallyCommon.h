#ifndef __USUALLY_COMMMON_H__
#define __USUALLY_COMMMON_H__

double strToDuble(WCHAR *strDouble,int len);
int StrToInt(LPCWSTR str,int strlen);
/*********************************************************************************************************
** Function name:           DoubleToStr
**
** Descriptions:            ��һ��˫���ȸ���������Ҫ�ľ�����������ת��Ϊ�ַ���
**
** input parameters:        dValue              ָ����˫���ȸ�����
                            pBuf                ���ڴ洢�ַ����Ļ���
                            iBufLen             �������ĳ���
                            iPrecision          ת���ľ��ȣ����Ϊ0������Ϊ�Զ�����

** Returned value:          ����ֵ������д�뵽���������ַ�����������β��NUL�ַ�
                            ���Ϊ0������������Ļ�����̫С
** Remarks                  ��Ȼ�ģ����0.999999��5λ����ִ�д˺���,�򷵻��ַ���"1.00000"
                            �Զ����Ȼ�Ѿ����趨Ϊ10�������Զ�ȥ��ĩβ��0��ͬ���ģ��Զ����ȵĻ�
                            ��0.999999����0.999999
*********************************************************************************************************/
int DoubleToStr(double dValue,WCHAR *pBuf,int iBufLen,int  iPrecision=0);
int intToStr(WCHAR *pStrValue,int len,int iValue);

//ָ�����ļ��Ƿ����
BOOL IsFileExist(const WCHAR *strFilePath);

/*********************************************************************************************************
** Function name:           GetExeDirectory
**
** Descriptions:            ��ȡ���г���Ŀ�ִ���ļ����ڵ�·��
**
** input parameters:        strPath  �洢���صı�ʾ��·���ַ����Ļ�������
                            len      strPath��ָ��Ļ������ĳ���
** Returned value:          ����ʵ�ʸ��Ƶ�strPath���������ַ�������������β��NUL
                            ����0����ʾ��������С
*********************************************************************************************************/
int  GetExeDirectory(WCHAR *strPath, int len);

//��ȡָ���ļ����¼������ļ����µ������ļ��ĸ���
int  GetDirFileCountAll(const WCHAR *strDirectory);

#endif
 
