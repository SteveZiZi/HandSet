/// @file
///
/// @brief
///     ���ݲ�ʹ�õĸ���С����
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.23
///
/// �޸���ʷ��
///
#include "stdafx.h"
#include <stdio.h>
#include <Windows.h>
#include <atlstr.h>
#include <float.h>
#include "data_utility.h"

#define _CVTBUFSIZE_USER_DEF        349          ///< FLOATת�ַ���ʱʹ�û��������ȣ���_CVTBUFSIZE��ͬ

const std::wstring GetAppDir()
{
    wchar_t AppPath[MAX_PATH + 1] = {0};
    ::GetModuleFileName(NULL, AppPath, MAX_PATH);
    std::wstring strAppPath(AppPath);
    std::wstring::size_type nPos = strAppPath.rfind(L"\\");
    if (nPos != std::wstring::npos)
    {
        strAppPath.resize(nPos+1);
        return strAppPath;
    }
    return L"";
}

///
/// @brief
///     ת�����ʵĸ����ַ���
///
/// @param[in]   strVal           �����ַ���
/// @param[out]  rstrResult       �������Ҫ��ĸ����ַ���
/// @param[out]  pdResult         ���ָ���˻���������double���ͱ���ת���������
///
/// @return
///     bool            ת���ɹ�����true��ʧ�ܷ���false
///
bool StrFloatConvert(const std::wstring& strVal, std::wstring* pstrResult, double* pdResult/* = NULL*/)
{
    CString csVal(strVal.c_str()), csResult;
    csVal.Trim();

    double fVolt    = 0.0;
    bool bDot       = false;
    int nLen        = csVal.GetLength();

    for(int i = 0; i < nLen; i++)
    {
        if(0 == _istdigit(csVal.GetAt(i)))
        {
            if(L'.' == csVal.GetAt(i) && !bDot)
            {
                bDot = true;
                continue;
            }

            return false;
        }
    }

    if(EOF == swscanf(csVal, L"%lf", &fVolt) || 0 == _finite(fVolt))
    {
        return false;
    }

    csResult.Format(L"%.03lf", fVolt);
    if (pstrResult)
    {
        *pstrResult = csResult.GetString();
    }
    if (pdResult)
    {
        *pdResult = fVolt;
    }

    return true; 
}

///
/// @brief
///     ת�����ʵ������ַ���
///
/// @param[in]   strVal           �����ַ���
/// @param[out]  rstrResult       �������Ҫ��ĸ����ַ���
/// @param[out]  pnResult         ���ָ���˻���������uint32_t���ͱ���ת���������
///
/// @return
///     bool            ת���ɹ�����true��ʧ�ܷ���false
///
bool StrUint32Convert(const std::wstring& strVal, std::wstring* pstrResult, uint32_t* pnResult/* = NULL*/)
{
    CString csVal(strVal.c_str()), csResult;
    int nLen            = csVal.GetLength();
    uint32_t nVal   = 0;

    if(nLen > 10)
    {
        return false;
    }

    for (int i = 0; i < nLen; i++)
    {
        if(_istdigit(csVal.GetAt(i)))
        {
            continue;
        }

        return false;
    }

    if(1 != swscanf(csVal, L"%u", &nVal))
    {
        return false;
    }

    csResult.Format(L"%u", nVal);
    if (pstrResult)
    {
        *pstrResult = csResult.GetString();
    }
    if (pnResult)
    {
        *pnResult = nVal;
    }

    return true;
}

///
/// @brief
///    ������ת�ַ���
///
/// @param[in]      fVal        Ҫת���ĸ�����
/// @param[out]     strResult   ����ת����Ľ��
///
/// @return
///     bool       ת���ɹ�����true
///
bool FloatStrConvert( double fVal, std::wstring& strResult, uint32_t nPoints/* = 3 */)
{
    wchar_t buf[_CVTBUFSIZE_USER_DEF] = {0};
    CString csFormatStr(L"%0.3f");
    if (nPoints < 10)
    {
        csFormatStr.Format(L"%%0.%df", nPoints);
    }
    if (swprintf_s(buf, _CVTBUFSIZE_USER_DEF, csFormatStr.GetString(), fVal) > 0)
    {
        std::wstring strtemp(buf);
        strResult = strtemp;
        return true;
    }
    return false;
}

///
/// @brief
///     ��MAC��ַ��wstringת��Ϊchar����
///
/// @param[in]
///     strMAC  - Ҫת����MAC��ַ�ַ���
/// @param[in]
///     pucMAC  - ����ת�������char�����ָ��
/// @param[in]
///     iLen    - �����С
///
/// @return
///     bool       ת���ɹ�����true
///
bool ConvMacWStr2Char(const std::wstring strMAC, unsigned char* pucMAC, int iLen)
{
    int          j      = 0;
    int          iValue = 0;
    wchar_t      ch[3]  = {0};


    if (((strMAC.length() < 17) || iLen < ETHERNET_MAC_LEN))
    {
        return false;
    }

    for ( int i=0 ; i<17 ;i = i+3)
    {
        ch[0] = strMAC[i];
        ch[1] = strMAC[i+1];
        swscanf(ch, L"%x", &iValue);
        pucMAC[j] = (unsigned char)iValue;
        j++;
    }

    return true;
}

///
/// @brief
///     ��MAC��ַ��char����ת��Ϊwstring
///
/// @param[in]
///     pucMAC  - ���MAC��ַ��char����ָ��
/// @param[in]
///     iLen    - �����С
///
/// @return
///     wstring   MAC�ַ���
///
std::wstring ConvMacChar2WStr(unsigned char* pucMAC, int iLen)
{
    wchar_t         cBuffer[18] = {0};


    if (iLen < ETHERNET_MAC_LEN)
    {
        return L"";
    }

    swprintf_s( cBuffer, sizeof(cBuffer) / sizeof(cBuffer[0]),
                L"%X%X-%X%X-%X%X-%X%X-%X%X-%X%X", 
                pucMAC[0]/16, pucMAC[0]%16,
                pucMAC[1]/16, pucMAC[1]%16,
                pucMAC[2]/16, pucMAC[2]%16,
                pucMAC[3]/16, pucMAC[3]%16,
                pucMAC[4]/16, pucMAC[4]%16,
                pucMAC[5]/16, pucMAC[5]%16
                );

    return cBuffer;
}

///
/// @brief
///    ��b��utcʱ��ת��ΪSYSTEMTIME
///
/// @return
///     SYSTEMTIME    SystemTime��ʾ��UTCʱ��
///
SYSTEMTIME UtcTime2SystemTime(UTC_TIME_t stUtcTime)
{
    SYSTEMTIME  sysTime;
    CTime       stTm1((__time64_t)stUtcTime.uSecondSinceEpoch);
    tm          stTm2;


    stTm1.GetGmtTm(&stTm2);
    sysTime.wYear           = (WORD)stTm2.tm_year + 1900;
    sysTime.wMonth          = (WORD)stTm2.tm_mon + 1;
    sysTime.wDay            = (WORD)stTm2.tm_mday;
    sysTime.wHour           = (WORD)stTm2.tm_hour;
    sysTime.wMinute         = (WORD)stTm2.tm_min;
    sysTime.wSecond         = (WORD)stTm2.tm_sec;
    sysTime.wDayOfWeek      = (WORD)stTm2.tm_wday;
    sysTime.wMilliseconds   = (WORD)(stUtcTime.uFractionOfSecond/1000);

    return sysTime;
}

///
/// @brief
///    ��ȡ��ǰ��������ʱ�����ļ�·��
///
/// @note
///     c:\a\b\c.exe ���� c:\a\e\
///     eΪbͬ��Ŀ¼����������ʱ����
///
const std::wstring GetAppRunTimeCfgDir()
{
    wchar_t AppPath[MAX_PATH + 1] = {0};
    ::GetModuleFileName(NULL, AppPath, MAX_PATH);
    std::wstring strAppPath(AppPath);
    std::wstring::size_type nPos = strAppPath.rfind(L"\\");
    if (nPos != std::wstring::npos)
    {
        strAppPath.resize(nPos);
        if (strAppPath.empty())  //��Ӧ�ڿ�ִ�г���ֱ�����̷���Ŀ¼����� \a.exe
        {
            return strAppPath + L"\\" + DEFAULT_CFG_RUN;
        }
        else
        {
            nPos = strAppPath.rfind(L"\\");
            if (nPos != std::wstring::npos)
            {
                strAppPath.resize(nPos + 1);
                strAppPath = strAppPath + DEFAULT_CFG_RUN;
                return strAppPath;
            }
        }
    }
    return L"\\";
}

///
/// @brief
///    ���ַ�����ָ�������ַ����滻���µ��ַ���
///
/// @param[in]  str    �ַ���
/// @param[in]  old_value    �ϵ����ַ���
/// @param[in]  new_value    �µ����ַ���
///
void ReplaceSubStr(std::wstring& str, const std::wstring old_value, const std::wstring new_value)
{
    // ����������
    if (old_value.size() == 0)
    {
        return ;
    }

    // �����ַ���������ͬ
    if (old_value.compare(new_value) == 0)
    {
        return ;
    }

    // �����滻�ַ���
    while(true)
    {
        std::wstring::size_type   pos(0);
        if((pos = str.find(old_value)) != std::wstring::npos)
        {
            str.replace(pos, old_value.length(), new_value);
        }
        else
        {
            break;
        }
    }
}

///
/// @brief
///    ����ͨ�����ԣ���������ͨ����������Ϣ
///
/// @param[in]  strOldDescription   ��ǰͨ��������Ϣ
/// @param[in]  eOldChannelType     �ϵ�ͨ������
/// @param[in]  eNewChannelType     �µ�ͨ������
/// @param[in]  eOldChannelPhase    �ϵ�ͨ�����
/// @param[in]  eNewChannelPhase    �µ�ͨ�����
/// @param[in]  bOldIsMeasure       ԭ��ͨ���Ƿ��ǲ�������
/// @param[in]  bNewIsMeasure       ����ͨ���Ƿ��ǲ�������
///
/// @return
///     std::wstring    �����ɵ�������Ϣ
///
std::wstring GenerateChannelDes(std::wstring        strOldDescription, 
                                enumChannelType     eOldChannelType,
                                enumChannelType     eNewChannelType, 
                                ChannelPhase        eOldChannelPhase,
                                ChannelPhase        eNewChannelPhase, 
                                bool                bOldIsMeasure,
                                bool                bNewIsMeasure)
{
    // �޸�������Ϣ�е�ͨ�����Ͳ���
    if (eNewChannelType != eOldChannelType)
    {
        switch(eNewChannelType)
        {
        case CHANNEL_TYPE_SMV_CURRENT:                                  // ����
            {
                ReplaceSubStr(strOldDescription, L"��ѹ", L"����");
                ReplaceSubStr(strOldDescription, L"ʱ��", L"����");
            }
            break;

        case CHANNEL_TYPE_SMV_VOLTAGE:                                  // ��ѹ
            {
                ReplaceSubStr(strOldDescription, L"����", L"��ѹ");
                ReplaceSubStr(strOldDescription, L"ʱ��", L"��ѹ");
            }
            break;

        case CHANNEL_TYPE_SMV_TIME:                                     // ʱ��
            {
                ReplaceSubStr(strOldDescription, L"����", L"ʱ��");
                ReplaceSubStr(strOldDescription, L"��ѹ", L"ʱ��");
            }
            break;
        }
    }

    // �޸�������Ϣ�е���ǲ���
    if (eNewChannelPhase != eOldChannelPhase)
    {
        std::wstring    strNewPhase;


        // �������Ƕ�Ӧ�ַ���
        if (CHANNEL_PHASE_A == eNewChannelPhase)
        {
            strNewPhase = L"A��";
        }
        else if (CHANNEL_PHASE_B == eNewChannelPhase)
        {
            strNewPhase = L"B��";
        }
        else if (CHANNEL_PHASE_C == eNewChannelPhase)
        {
            strNewPhase = L"C��";
        }
        else if (CHANNEL_PHASE_X == eNewChannelPhase)
        {
            strNewPhase = L"X��";
        }
        else if (CHANNEL_PHASE_N == eNewChannelPhase)
        {
            strNewPhase = L"N��";
        }

        // �޸�ͨ�������ַ���
        if (strNewPhase.size() > 0)
        {
            ReplaceSubStr(strOldDescription, L"A��", strNewPhase);
            ReplaceSubStr(strOldDescription, L"B��", strNewPhase);
            ReplaceSubStr(strOldDescription, L"C��", strNewPhase);
            ReplaceSubStr(strOldDescription, L"X��", strNewPhase);
            ReplaceSubStr(strOldDescription, L"N��", strNewPhase);
        }
    }

    // �޸�������Ϣ�еĲ�������
    if (bOldIsMeasure != bNewIsMeasure)
    {
        if (bNewIsMeasure)
        {
            ReplaceSubStr(strOldDescription, L"����", L"����");
        }
        else
        {
            ReplaceSubStr(strOldDescription, L"����", L"����");
        }
    }

    return strOldDescription;
}
