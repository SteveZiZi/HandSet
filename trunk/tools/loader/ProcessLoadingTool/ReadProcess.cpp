#include "stdafx.h"
#include "ReadProcess.h"
#include <dbgapi.h>

//存放当前的进程名
const CString CUR_PROCESS_NAME = L"ProcessLoadingTool.exe";

CReadProcess::CReadProcess(void)
{
}

CReadProcess::CReadProcess( CString strFileName )
{
    m_strFileName = strFileName;
}

CReadProcess::~CReadProcess(void)
{
}

///
/// @brief
///     从XML文件中读取进程路径列表
/// @param[vector<CString> &]
///     vecProcessInfo - 用来保存读取到的进程路径列表 
///
/// @return
///     如果读取成功则返回TRUE，否则返回FALSE
///
BOOL CReadProcess::GetProcessList( std::vector<_PROCESS_INFO> &vecProcessInfo )
{
    //如果加载XML文件失败
    if(!m_markup.Load(m_strFileName))
    {
        RETAILMSG(TRUE, (CUR_PROCESS_NAME + 
            TEXT(" Error: this is not a xml file or file not exist\n"), NULL));
        return FALSE;
    }

    //重新设置指针
    m_markup.ResetMainPos();

    //如果找到根节点，则进入子节点
    if(m_markup.FindElem(_T("ROOT")))
    {
        m_markup.IntoElem();
    }
    else
    {
        RETAILMSG(TRUE, (CUR_PROCESS_NAME +
            TEXT(" Error: can't find the root node\n"), NULL));
        return FALSE;
    }
    
    //查找Logo节点
    if(m_markup.FindElem(_T("Logo")))
    {
        m_strLogoName = m_markup.GetData();
    }
    else
    {
        RETAILMSG(TRUE, (CUR_PROCESS_NAME +
            TEXT(" Error: can't find the logo name\n"), NULL));
    }
    //查找ProcessInfo节点
    while(m_markup.FindElem(_T("ProcessInfo")))
    {
        m_markup.IntoElem();
        _PROCESS_INFO process_info;

        //结构体的初始化
        process_info.strProcessName     =   _T("");
        process_info.strProcessParam    =   _T("");
        process_info.uDelayTime         =   0;
        process_info.isProtected        =   0;
        process_info.bAbsolutePath      =   0;

        CString strTemp, strTagName;
        
        //标记有没有找到进程名标签
        bool bProcessName = FALSE;

        //循环读取进程信息
        while(m_markup.FindElem())
        {
            //读取XML文件中节点的标签名
            strTagName = m_markup.GetTagName();

            //找到的是进程名节点
            if(_T("ProcessName") == strTagName)
            {
                bProcessName = TRUE;
                process_info.strProcessName = m_markup.GetData();

                //判断是否是绝对路径
                process_info.bAbsolutePath  = JudgeFilePath(process_info.strProcessName);

            }

            //如果是延迟时间
            else if(_T("DelayTime") == strTagName)
            {
                process_info.uDelayTime = _ttoi(m_markup.GetData());
            }

            //如果是保护标记
            else if(_T("Protected") == strTagName)
            {
                process_info.isProtected = (1 == _ttoi(m_markup.GetData()));
            }

            //如果有进程参数
            else if(_T("ProcessParam") == strTagName)
            {
                process_info.strProcessParam = m_markup.GetData();
            }
            else
            {
                RETAILMSG(TRUE, (CUR_PROCESS_NAME +
                    TEXT(" Error: illegal xml node\n"), NULL));
                return FALSE;
            }
        } 

        //如果不存在进程名节点
        if(!bProcessName)
        {
            RETAILMSG(TRUE, (CUR_PROCESS_NAME +
                TEXT(" Error: lack of xml node\n"),NULL));
            return FALSE;
        }
        
        //把读取到的进程信息添加到向量表中
        vecProcessInfo.push_back(process_info); 

        m_markup.OutOfElem();
    }

    return TRUE;
}

///
/// @brief
///     跟据路径名判断是相对路径还是绝对路径
/// @param[CString]
///     strFilePath - 文件路径名
/// @return
///     返回TRUE表示是绝对路径，返回FALSE表示是相对路径
///
bool CReadProcess::JudgeFilePath( CString strFilePath )
{
    for(int i = 0; i < strFilePath.GetLength(); i++)
    {
        //如果找到目录分隔标记则返回
        if(strFilePath[i] == '\\')
        {
            return TRUE;
        }
    }
    return FALSE;
}

///
/// @brief
///     获取从XMl文件中读取的logo图片路径
///
/// @return
///     返回logo图片的路径
///
CString CReadProcess::GetLogoName()
{
    return m_strLogoName;
}