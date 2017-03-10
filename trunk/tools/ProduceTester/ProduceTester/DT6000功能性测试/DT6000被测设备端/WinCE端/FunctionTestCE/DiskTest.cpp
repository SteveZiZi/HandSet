#include "stdafx.h"
#include <vector>
#include <Storemgr.h>
#include "testCommon.h"
#include "connect.h"
#include "testBase.h"
#include "DiskTest.h"
#include "iniFile.h"
#include "assertMsg.h"

using namespace std;
/**********************************************************************
���ܲ�����������̲���Э��˵��
��λ�������η��ͳ�ʼ����д�ļ������ļ���ɾ�������ļ�����
�����ʼ��ʧ�ܣ���д�ļ������ļ���ɾ�������ļ����ᱻ���ͣ�
д�ļ��Ͷ��ļ����صĲ�����Ϣ�еĸ����ֶε�ǰ�ĸ��ֽ�Ҫ�洢��д�ļ���
У���
//����Ĵ��̲��ԣ�ָ����ÿһ��Ҫ���ԵĴ��̿������Ѿ������˴��̣�����
//���ذ�������·SD���������sd����Ϊ���̲����࣬��ô����SD����������Ҫ�Ȳ���
//SD��
***********************************************************************/

//���ﶨ��һ�����̲��Թ����࣬���ڱ���ĳ�����������ص��ļ����Ƿ��Ѿ���
//ĳ�������������Ϊ������
#pragma comment(lib,"Storeapi.lib")

class CDiskTestManager
{
public:
    CDiskTestManager();
    ~CDiskTestManager();

    BOOL InsertMakedDiskTestFolder(LPCWSTR strFolder);
    BOOL IsFolderMarked(LPCWSTR strFolder);
    void DeleteMarkedFolder(LPCWSTR strFolder);

private:
    CRITICAL_SECTION    m_cs;
    vector<CString *>   m_vecFoler;
};


CDiskTestManager::CDiskTestManager()
{
    InitializeCriticalSection(&m_cs);
};

CDiskTestManager::~CDiskTestManager()
{
    EnterCriticalSection(&m_cs);
    
    long len=(long)m_vecFoler.size();
    for(long i=0;i<len;i++)
    {
        delete m_vecFoler[i];
    }
    m_vecFoler.clear();
    LeaveCriticalSection(&m_cs);
    DeleteCriticalSection(&m_cs);
}

BOOL CDiskTestManager::InsertMakedDiskTestFolder(LPCWSTR strFolder)
{
    BOOL bRet=FALSE;
    EnterCriticalSection(&m_cs);
    if (! IsFolderMarked( strFolder) )
    {
        CString *pStr=new CString(strFolder);
        m_vecFoler.insert(m_vecFoler.end(),pStr);
        bRet=TRUE;
    }   
    LeaveCriticalSection(&m_cs);

    return bRet;
}

BOOL CDiskTestManager::IsFolderMarked(LPCWSTR strFolder)
{
    BOOL        bRet=FALSE;

    EnterCriticalSection(&m_cs);
    long      len=(long)m_vecFoler.size();
    for (long i=0;i<len;i++)
    {
        if ( (*m_vecFoler[i])==strFolder )
        {
            bRet=TRUE;
            break;
        }
    }
    LeaveCriticalSection(&m_cs);

    return bRet;
}

void CDiskTestManager::DeleteMarkedFolder(LPCWSTR strFolder)
{
    CString         *pStr;
    vector<CString *>::iterator it;
    EnterCriticalSection(&m_cs);
    for (it=m_vecFoler.begin(); it!=m_vecFoler.end();it++)
    {
        if ( *(*it)==strFolder )
        {
            pStr=*it;
            m_vecFoler.erase(it);
            delete  pStr;
            break;
        }        
    }
    LeaveCriticalSection(&m_cs);
}



static CDiskTestManager g_diskManager;

CDiskTest::CDiskTest():
m_uiTestNum(0)
{
    m_strTestFolder[0]=0;
    m_strStoreName[0]=0;
}

CDiskTest::~CDiskTest()
{
}


//��ʼ�����д��̲��Թ��õĳ���
//static const  int CDiskTest::__DISK_TEST_BUF_SIZE=1024;
//������֧�ֵĴ������͵����
#define     DISK_TYPE_FLASH     0       //FLASH����
#define     DISK_TYPE_SD        1       //SD��
#define     DISK_TYPE_CF        2       //CF��
#define     DISK_TYPE_USBDISK   3       //USB���̣�����U��
#define     DISK_TYPE_MAX       3       //֧�ֵĴ������͵���ŵ����ֵ

//��Ӧ�������������STOREINFO�ṹ��dwDeviceType�ֶε�����
const DWORD g_dwTpyeMask[]=
{
    0,                                  //Flash
    0,                                  //SD����dwDeviceType�ֶ���û������,��Ҫ����StoreName��ʶ��
    0,                                  //CF��
    0,                                  //USB����
};



BOOL CDiskTest::Init(const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager)
{
    WCHAR strSection[]=L"DiskTest_\0\0";
    strSection[9]=L'1'+index;

    //��ȡ�����������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"����1",
        m_strTestName,31,strIniFile);

    //��ȡҪ���Դ������ͣ����̲���ר�У�
    m_iDiskType=GetPrivateProfileInt(strSection,L"DiskType",0,strIniFile);
    if (m_iDiskType <0 ||m_iDiskType >DISK_TYPE_MAX)
    {
        MessageBox(NULL,L"��֧�ֵĴ�������",L"���̲���",MB_OK|MB_ICONERROR);
        return FALSE;
    }
    //����StoreName
    GetPrivateProfileString(strSection,L"StoreName",L"",m_strStoreName,64,
        strIniFile);

    m_uiCmd=CMD_TEST_DISK1+index;
    srand(GetTickCount());
    *uipCmdIndex=m_uiCmd;
    return TRUE;
}

BOOL CDiskTest::InitDiskTest()
{
    WCHAR       strTmp[64];
    HANDLE      hSearch=INVALID_HANDLE_VALUE;
    m_uiTestNum=0;
    //ȷ�����ڲ��Ҵ����ļ��е�����
    if (m_strTestFolder[0]  !=0)
    {
        //���Ҫ���Ե��ļ��в�Ϊ�գ���ȷ������ļ����Ƿ���Ȼ����            
        WIN32_FIND_DATA wfd;
        HANDLE hFind;

        wcsncpy(strTmp,m_strTestFolder,64);
        hFind=FindFirstFile(strTmp,&wfd);
        if (hFind !=INVALID_HANDLE_VALUE)
        {
            FindClose(hFind);            
            goto Find_Store;
        }
        g_diskManager.DeleteMarkedFolder(m_strTestFolder);
        m_strTestFolder[0]=0;
    }
    //��ʼ����ϵͳ�����д��̣�������Ϊ������������һ������

    STOREINFO   si;
    si.cbSize=sizeof(si);
    hSearch=FindFirstStore(&si);
    if (hSearch !=INVALID_HANDLE_VALUE)
    {
        BOOL  bFind=FALSE;
        do 
        {
            //���ȿ�����洢�豸�Ƿ�����ָ���Ĵ洢����
            BOOL    bCorrectType=FALSE;
            if (g_dwTpyeMask[m_iDiskType] !=0)
            {
                if (si.dwDeviceType &g_dwTpyeMask[m_iDiskType] )
                {
                    bCorrectType=TRUE;
                }
            }
            else
            {
                CString strStoreName=m_strStoreName;
                if (strStoreName ==si.szStoreName)
                {
                    bCorrectType=TRUE;
                }
            }

            if (bCorrectType)
            {
                //��ÿһ���ҵ��Ĵ��̣����Ի�ȡ�������Ϣ
                HANDLE      hStore=OpenStore(si.szDeviceName);
                HANDLE      hFindPartition=INVALID_HANDLE_VALUE;
                BOOL        bFindPartition=FALSE;
                if (hStore !=INVALID_HANDLE_VALUE)
                {
                    PARTINFO        pi;
                    pi.cbSize       =sizeof(pi);
                    hFindPartition  =FindFirstPartition(hStore,&pi);
                    if (hFindPartition !=INVALID_HANDLE_VALUE)
                    {
                        do 
                        {
                            if (pi.szVolumeName[0]!=0)
                            {

                                CString str;
                                if (pi.szVolumeName[0]!=L'\\')
                                {
                                    str=L"\\";
                                }
                                
                                str=str+pi.szVolumeName+L"\\";
                                CString strTmp=str;
                                //����ɾ����������Ͽ��ܵĲ����ļ�
                                for (int i=0;i<3;i++)
                                {
                                    strTmp.Format(_T("%s%d"),(LPCTSTR)str,i);
                                    DeleteFile(strTmp);
                                }
                                //ȷ��������̷��������㹻��ʣ��ռ�
                                ULARGE_INTEGER   liFree,liTotal;
                                GetDiskFreeSpaceEx(str,&liFree,&liTotal,NULL);
                                if (liFree.QuadPart >3*1024*1024)
                                {
                                    //ӵ���㹻�Ŀռ�,�Ƿ��Ĳ������Ѿ�ռ���˸ô���
                                    if (g_diskManager.InsertMakedDiskTestFolder(str))
                                    {
                                        FindCloseStore(hSearch);
                                        CloseHandle(hStore);
                                        FindClosePartition(hFindPartition);
                                        wcsncpy(m_strTestFolder,str,32);
                                        m_iTestFolderLen=wcslen(m_strTestFolder);
                                        goto Find_Store;
                                    }

                                }
                            }                         

                            bFindPartition=FindNextPartition(hFindPartition,&pi);
                        } while (bFindPartition);
                        FindClosePartition(hFindPartition);                                        
                    }

                    CloseHandle(hStore);
                }
            } 
           
            bFind=FindNextStore(hSearch,&si);
        } while (bFind);

        FindCloseStore(hSearch);
    }



    wcsncpy(strTmp,m_strTestFolder,32);

    m_ti.uiErrorLen=swprintf(m_ti.strErr,L"��ȷ��%s�Ѳ��룬���߲���Ĵ������㹻�Ŀռ�",m_strTestName );
    m_ti.bError=1;
    return FALSE;
Find_Store:
    m_ti.uiErrorLen=0;
    m_ti.bError=0;
    return TRUE;
}

TEST_STATUS CDiskTest::ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, 
                                        const BYTE *pData,CConnect *pConnect)
{
    TEST_STATUS statusRet=TEST_NORMAL;
    switch(param)
    {
    case DISK_TEST_CMD_WRITE:
        {
            UINT32 uiVerifySumWrite=0;
            if (WriteTestFile(&uiVerifySumWrite))
            {
                statusRet=TEST_NORMAL;
                //���Գɹ������²�����Ϣ
                m_ti.bError=0;
                m_ti.uiErrorLen=0;
                memcpy(m_ti.bAppend,&uiVerifySumWrite,sizeof(uiVerifySumWrite) );
            }
            else
            {
                statusRet=TEST_ERROR;
            }
        }
        break;
    case DISK_TEST_CMD_READ:
        {
            UINT32 uiVerifySumRead=0;
            if (ReadTestFile(&uiVerifySumRead))
            {
                statusRet=TEST_NORMAL;
                //���Գɹ������²�����Ϣ
                m_ti.bError=0;
                m_ti.uiErrorLen=0;
                memcpy(m_ti.bAppend,&uiVerifySumRead,sizeof(uiVerifySumRead) );
            }
            else
            {
                statusRet=TEST_ERROR;
            }
        }
    case DISK_TEST_CMD_DELETE:
        DeleteTestFiles();
        m_ti.bError=0;
        m_ti.uiErrorLen=0;
        break;

    case  PARAM_GET_INIT_INFO:
        InitDiskTest();
        break;
    default:
        m_ti.bError=0;
        m_ti.uiErrorLen=0;
        break;
    }

    int nRet;
    nRet=pConnect->SendAPacket(m_uiCmd,param,sizeof(m_ti),(BYTE *)&m_ti );
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }
    
    return statusRet;
}

BOOL CDiskTest::DeleteTestFiles()
{
    WCHAR  str[64];
    wcscpy(str,m_strTestFolder);

    int len=wcslen(str);
    str[len+1]=0;

    for (int i=0;i<3;i++)
    {
        str[len]=L'0'+i;
        DeleteFile(str);
    }
    m_ti.bError=0;
    m_ti.uiErrorLen=0;
    
    return TRUE;
}

BOOL CDiskTest::WriteTestFile(UINT32 *uiVerifySumWrite)
{
    register UINT32 uiSum=0;
    WCHAR   str[64];

    //ȷ��Ҫд����ļ���
    m_uiTestNum=(m_uiTestNum+1)%3;
    wcscpy(str,m_strTestFolder);
    str[m_iTestFolderLen+1]=0;
    str[m_iTestFolderLen] =L'0'+m_uiTestNum;


    HANDLE      hTestFile=INVALID_HANDLE_VALUE;
    //���������ļ�
    hTestFile=CreateFile(str,GENERIC_WRITE,0,
        NULL,CREATE_ALWAYS,FILE_FLAG_WRITE_THROUGH,INVALID_HANDLE_VALUE);
    if (hTestFile==INVALID_HANDLE_VALUE)
    {
        m_ti.bError=1;
        m_ti.uiErrorLen=swprintf(m_ti.strErr,L"���������ļ�ʧ��");       
        return FALSE;
    }

    //д���ļ����������д�����ݵ�У���
    int     j;
    UINT32 *pui=(UINT32 *)buf;
    BOOL    bSucess =TRUE;
    DWORD  dwReturn =0;

    //���ɲ����ļ�����
    for(j=0;j<sizeof(buf);j++)
    {
        buf[j]=rand();
    }
    //����У���
    for (j=0;j<__DISK_TEST_BUF_UINT32_NUM;j++)
    {
        uiSum^=pui[j];
    }
    bSucess=WriteFile(hTestFile,buf,sizeof(buf),&dwReturn,NULL);
    if (bSucess)
    {
        if (dwReturn<sizeof(buf))
        {
            CloseHandle(hTestFile);
            m_ti.bError=1;
            m_ti.uiErrorLen=swprintf(m_ti.strErr,L"д������ļ�ʱ���ִ�������");
            return FALSE;
        }
        else
        {
            //д��ɹ�
            *uiVerifySumWrite=uiSum;
            CloseHandle(hTestFile);
            return TRUE;
        }
    }  
  
    CloseHandle(hTestFile);
    m_ti.bError=1;
    m_ti.uiErrorLen=swprintf(m_ti.strErr,L"д������ļ�����");
    return FALSE;
}

BOOL CDiskTest::ReadTestFile(UINT32 *uiVerifySumRead)
{

    register UINT32 uiSum=0;

    WCHAR   str[64];

    //ȷ��Ҫ�������ļ���
    wcscpy(str,m_strTestFolder);
    str[m_iTestFolderLen+1]=0;
    str[m_iTestFolderLen] =L'0'+m_uiTestNum;

    HANDLE      hTestFile=INVALID_HANDLE_VALUE;
    //���������ļ�
    hTestFile=CreateFile(str,GENERIC_READ,0,
        NULL,OPEN_EXISTING,0,INVALID_HANDLE_VALUE);
    if (hTestFile==INVALID_HANDLE_VALUE)
    {
        m_ti.bError=1;
        m_ti.uiErrorLen=swprintf(m_ti.strErr,L"�򿪲����ļ�ʧ��");       
        return FALSE;
    }

    //д���ļ����������д�����ݵ�У���
    int     j;
    UINT32 *pui=(UINT32 *)buf;
    BOOL    bSucess =TRUE;
    DWORD   dwReturn =0;
    
    bSucess=ReadFile(hTestFile,buf,sizeof(buf),&dwReturn,NULL);
    if (bSucess)
    {       
        //����У���
        for (j=0;j<__DISK_TEST_BUF_UINT32_NUM;j++)
        {
            uiSum^=pui[j];
        }
        //д��ɹ�
        *uiVerifySumRead=uiSum;
        CloseHandle(hTestFile);
        return TRUE;        
    }  

    CloseHandle(hTestFile);
    m_ti.bError=1;
    m_ti.uiErrorLen=swprintf(m_ti.strErr,L"�������ļ�����");
    return FALSE;
}