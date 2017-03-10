#include "stdafx.h"
#include "ReadProcess.h"
#include <dbgapi.h>

//��ŵ�ǰ�Ľ�����
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
///     ��XML�ļ��ж�ȡ����·���б�
/// @param[vector<CString> &]
///     vecProcessInfo - ���������ȡ���Ľ���·���б� 
///
/// @return
///     �����ȡ�ɹ��򷵻�TRUE�����򷵻�FALSE
///
BOOL CReadProcess::GetProcessList( std::vector<_PROCESS_INFO> &vecProcessInfo )
{
    //�������XML�ļ�ʧ��
    if(!m_markup.Load(m_strFileName))
    {
        RETAILMSG(TRUE, (CUR_PROCESS_NAME + 
            TEXT(" Error: this is not a xml file or file not exist\n"), NULL));
        return FALSE;
    }

    //��������ָ��
    m_markup.ResetMainPos();

    //����ҵ����ڵ㣬������ӽڵ�
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
    
    //����Logo�ڵ�
    if(m_markup.FindElem(_T("Logo")))
    {
        m_strLogoName = m_markup.GetData();
    }
    else
    {
        RETAILMSG(TRUE, (CUR_PROCESS_NAME +
            TEXT(" Error: can't find the logo name\n"), NULL));
    }
    //����ProcessInfo�ڵ�
    while(m_markup.FindElem(_T("ProcessInfo")))
    {
        m_markup.IntoElem();
        _PROCESS_INFO process_info;

        //�ṹ��ĳ�ʼ��
        process_info.strProcessName     =   _T("");
        process_info.strProcessParam    =   _T("");
        process_info.uDelayTime         =   0;
        process_info.isProtected        =   0;
        process_info.bAbsolutePath      =   0;

        CString strTemp, strTagName;
        
        //�����û���ҵ���������ǩ
        bool bProcessName = FALSE;

        //ѭ����ȡ������Ϣ
        while(m_markup.FindElem())
        {
            //��ȡXML�ļ��нڵ�ı�ǩ��
            strTagName = m_markup.GetTagName();

            //�ҵ����ǽ������ڵ�
            if(_T("ProcessName") == strTagName)
            {
                bProcessName = TRUE;
                process_info.strProcessName = m_markup.GetData();

                //�ж��Ƿ��Ǿ���·��
                process_info.bAbsolutePath  = JudgeFilePath(process_info.strProcessName);

            }

            //������ӳ�ʱ��
            else if(_T("DelayTime") == strTagName)
            {
                process_info.uDelayTime = _ttoi(m_markup.GetData());
            }

            //����Ǳ������
            else if(_T("Protected") == strTagName)
            {
                process_info.isProtected = (1 == _ttoi(m_markup.GetData()));
            }

            //����н��̲���
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

        //��������ڽ������ڵ�
        if(!bProcessName)
        {
            RETAILMSG(TRUE, (CUR_PROCESS_NAME +
                TEXT(" Error: lack of xml node\n"),NULL));
            return FALSE;
        }
        
        //�Ѷ�ȡ���Ľ�����Ϣ��ӵ���������
        vecProcessInfo.push_back(process_info); 

        m_markup.OutOfElem();
    }

    return TRUE;
}

///
/// @brief
///     ����·�����ж������·�����Ǿ���·��
/// @param[CString]
///     strFilePath - �ļ�·����
/// @return
///     ����TRUE��ʾ�Ǿ���·��������FALSE��ʾ�����·��
///
bool CReadProcess::JudgeFilePath( CString strFilePath )
{
    for(int i = 0; i < strFilePath.GetLength(); i++)
    {
        //����ҵ�Ŀ¼�ָ�����򷵻�
        if(strFilePath[i] == '\\')
        {
            return TRUE;
        }
    }
    return FALSE;
}

///
/// @brief
///     ��ȡ��XMl�ļ��ж�ȡ��logoͼƬ·��
///
/// @return
///     ����logoͼƬ��·��
///
CString CReadProcess::GetLogoName()
{
    return m_strLogoName;
}