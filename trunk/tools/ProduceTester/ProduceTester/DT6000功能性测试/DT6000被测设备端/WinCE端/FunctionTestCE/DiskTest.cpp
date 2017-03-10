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
功能测试软件，磁盘测试协议说明
上位机会依次发送初始化，写文件，读文件，删除测试文件命令
如果初始化失败，则写文件，读文件，删除测试文件不会被发送，
写文件和读文件返回的测试信息中的附加字段的前四个字节要存储读写文件的
校验和
//这里的磁盘测试，指的是每一个要测试的磁盘卡座都已经插入了磁盘，例如
//工控板上有两路SD卡，如果将sd卡归为磁盘测试类，那么两个SD卡座都必须要先插上
//SD卡
***********************************************************************/

//这里定义一个磁盘测试管理类，用于表征某个磁盘所挂载的文件夹是否已经被
//某个测试项所标记为它测试
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


//初始化所有磁盘测试公用的常量
//static const  int CDiskTest::__DISK_TEST_BUF_SIZE=1024;
//定义所支持的磁盘类型的序号
#define     DISK_TYPE_FLASH     0       //FLASH磁盘
#define     DISK_TYPE_SD        1       //SD卡
#define     DISK_TYPE_CF        2       //CF卡
#define     DISK_TYPE_USBDISK   3       //USB磁盘，例如U盘
#define     DISK_TYPE_MAX       3       //支持的磁盘类型的序号的最大值

//对应磁盘类型序号在STOREINFO结构中dwDeviceType字段的掩码
const DWORD g_dwTpyeMask[]=
{
    0,                                  //Flash
    0,                                  //SD卡在dwDeviceType字段中没有掩码,需要依靠StoreName来识别
    0,                                  //CF卡
    0,                                  //USB磁盘
};



BOOL CDiskTest::Init(const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager)
{
    WCHAR strSection[]=L"DiskTest_\0\0";
    strSection[9]=L'1'+index;

    //读取测试项的名字
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"磁盘1",
        m_strTestName,31,strIniFile);

    //读取要测试磁盘类型（磁盘测试专有）
    m_iDiskType=GetPrivateProfileInt(strSection,L"DiskType",0,strIniFile);
    if (m_iDiskType <0 ||m_iDiskType >DISK_TYPE_MAX)
    {
        MessageBox(NULL,L"不支持的磁盘类型",L"磁盘测试",MB_OK|MB_ICONERROR);
        return FALSE;
    }
    //加载StoreName
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
    //确定用于查找磁盘文件夹的名字
    if (m_strTestFolder[0]  !=0)
    {
        //如果要测试的文件夹不为空，则确认这个文件夹是否依然存在            
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
    //开始遍历系统的所有磁盘，并尝试为这个测试项分配一个磁盘

    STOREINFO   si;
    si.cbSize=sizeof(si);
    hSearch=FindFirstStore(&si);
    if (hSearch !=INVALID_HANDLE_VALUE)
    {
        BOOL  bFind=FALSE;
        do 
        {
            //首先看这个存储设备是否属于指定的存储类型
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
                //对每一个找到的磁盘，尝试获取其分区信息
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
                                //尝试删除这个磁盘上可能的测试文件
                                for (int i=0;i<3;i++)
                                {
                                    strTmp.Format(_T("%s%d"),(LPCTSTR)str,i);
                                    DeleteFile(strTmp);
                                }
                                //确认这个磁盘分区上有足够的剩余空间
                                ULARGE_INTEGER   liFree,liTotal;
                                GetDiskFreeSpaceEx(str,&liFree,&liTotal,NULL);
                                if (liFree.QuadPart >3*1024*1024)
                                {
                                    //拥有足够的空间,是否别的测试项已经占有了该磁盘
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

    m_ti.uiErrorLen=swprintf(m_ti.strErr,L"请确认%s已插入，或者插入的磁盘有足够的空间",m_strTestName );
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
                //测试成功，更新测试信息
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
                //测试成功，更新测试信息
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

    //确定要写入的文件名
    m_uiTestNum=(m_uiTestNum+1)%3;
    wcscpy(str,m_strTestFolder);
    str[m_iTestFolderLen+1]=0;
    str[m_iTestFolderLen] =L'0'+m_uiTestNum;


    HANDLE      hTestFile=INVALID_HANDLE_VALUE;
    //创建测试文件
    hTestFile=CreateFile(str,GENERIC_WRITE,0,
        NULL,CREATE_ALWAYS,FILE_FLAG_WRITE_THROUGH,INVALID_HANDLE_VALUE);
    if (hTestFile==INVALID_HANDLE_VALUE)
    {
        m_ti.bError=1;
        m_ti.uiErrorLen=swprintf(m_ti.strErr,L"创建测试文件失败");       
        return FALSE;
    }

    //写入文件，并计算出写入数据的校验和
    int     j;
    UINT32 *pui=(UINT32 *)buf;
    BOOL    bSucess =TRUE;
    DWORD  dwReturn =0;

    //生成测试文件数据
    for(j=0;j<sizeof(buf);j++)
    {
        buf[j]=rand();
    }
    //计算校验和
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
            m_ti.uiErrorLen=swprintf(m_ti.strErr,L"写入测试文件时发现磁盘已满");
            return FALSE;
        }
        else
        {
            //写入成功
            *uiVerifySumWrite=uiSum;
            CloseHandle(hTestFile);
            return TRUE;
        }
    }  
  
    CloseHandle(hTestFile);
    m_ti.bError=1;
    m_ti.uiErrorLen=swprintf(m_ti.strErr,L"写入测试文件出错");
    return FALSE;
}

BOOL CDiskTest::ReadTestFile(UINT32 *uiVerifySumRead)
{

    register UINT32 uiSum=0;

    WCHAR   str[64];

    //确定要读出的文件名
    wcscpy(str,m_strTestFolder);
    str[m_iTestFolderLen+1]=0;
    str[m_iTestFolderLen] =L'0'+m_uiTestNum;

    HANDLE      hTestFile=INVALID_HANDLE_VALUE;
    //创建测试文件
    hTestFile=CreateFile(str,GENERIC_READ,0,
        NULL,OPEN_EXISTING,0,INVALID_HANDLE_VALUE);
    if (hTestFile==INVALID_HANDLE_VALUE)
    {
        m_ti.bError=1;
        m_ti.uiErrorLen=swprintf(m_ti.strErr,L"打开测试文件失败");       
        return FALSE;
    }

    //写入文件，并计算出写入数据的校验和
    int     j;
    UINT32 *pui=(UINT32 *)buf;
    BOOL    bSucess =TRUE;
    DWORD   dwReturn =0;
    
    bSucess=ReadFile(hTestFile,buf,sizeof(buf),&dwReturn,NULL);
    if (bSucess)
    {       
        //计算校验和
        for (j=0;j<__DISK_TEST_BUF_UINT32_NUM;j++)
        {
            uiSum^=pui[j];
        }
        //写入成功
        *uiVerifySumRead=uiSum;
        CloseHandle(hTestFile);
        return TRUE;        
    }  

    CloseHandle(hTestFile);
    m_ti.bError=1;
    m_ti.uiErrorLen=swprintf(m_ti.strErr,L"读测试文件出错");
    return FALSE;
}