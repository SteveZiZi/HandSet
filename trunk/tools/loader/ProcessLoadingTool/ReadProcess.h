/// @file
///
/// @brief
///     ����XML�ļ�����ȡ���õ�����
///
/// @note
///     ��XML�ļ��л�ȡ������Ϣ�б�
///
/// Copyright (c) 2013 IUnknownBase Inc.
///
/// ����:
///
///     chao 2013.7.1
///
/// �޸���ʷ: V1.1
///
/// �޸�ʱ��: 20113.8.2
///
#pragma once
#include "Markup.h"
#include <vector>

///
/// @brief
///     ������Ϣ��Ԫ
///
typedef struct 
{
    CString     strProcessName;     ///<�������̵�·�������������·�������·��
    CString     strProcessParam;    ///<�������̵Ĳ���
    int         uDelayTime;         ///<�����һ������Ҫ�ӳ�������ʱ��
    bool        isProtected;        ///<���Ϊ�Ƿ���Ҫ���м�ر���
    bool        bAbsolutePath;      ///<��ǽ���·���������·�����Ǿ���·��
}_PROCESS_INFO;

class CReadProcess
{
public:
    CReadProcess(void);
    ~CReadProcess(void);
    CReadProcess(CString strFileName);
    BOOL GetProcessList(std::vector<_PROCESS_INFO> &vecProcessInfo);
    CString GetLogoName();
private:
    //CMarkup���һ��������������XML�ļ�
    CMarkup m_markup;

    //Ҫ������XML�ļ���
    CString m_strFileName;

    //Ҫ������logoͼƬ·��
    CString m_strLogoName;

    bool JudgeFilePath(CString strFilePath);
};
