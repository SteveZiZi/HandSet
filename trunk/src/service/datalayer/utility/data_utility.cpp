/// @file
///
/// @brief
///     数据层使用的辅助小函数
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.23
///
/// 修改历史：
///
#include "stdafx.h"
#include <stdio.h>
#include <Windows.h>
#include <atlstr.h>
#include <float.h>
#include "data_utility.h"

#define _CVTBUFSIZE_USER_DEF        349          ///< FLOAT转字符串时使用缓冲区长度，与_CVTBUFSIZE相同

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
///     转换合适的浮点字符串
///
/// @param[in]   strVal           输入字符串
/// @param[out]  rstrResult       输出符合要求的浮点字符串
/// @param[out]  pdResult         如果指定了缓冲区，用double类型保存转换后的数据
///
/// @return
///     bool            转换成功返回true，失败返回false
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
///     转换合适的整型字符串
///
/// @param[in]   strVal           输入字符串
/// @param[out]  rstrResult       输出符合要求的浮点字符串
/// @param[out]  pnResult         如果指定了缓冲区，用uint32_t类型保存转换后的数据
///
/// @return
///     bool            转换成功返回true，失败返回false
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
///    浮点数转字符串
///
/// @param[in]      fVal        要转换的浮点数
/// @param[out]     strResult   保存转换后的结果
///
/// @return
///     bool       转换成功返回true
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
///     将MAC地址由wstring转换为char数组
///
/// @param[in]
///     strMAC  - 要转换的MAC地址字符串
/// @param[in]
///     pucMAC  - 保存转换结果的char数组的指针
/// @param[in]
///     iLen    - 数组大小
///
/// @return
///     bool       转换成功返回true
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
///     将MAC地址由char数组转换为wstring
///
/// @param[in]
///     pucMAC  - 存放MAC地址的char数组指针
/// @param[in]
///     iLen    - 数组大小
///
/// @return
///     wstring   MAC字符串
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
///    将b码utc时间转换为SYSTEMTIME
///
/// @return
///     SYSTEMTIME    SystemTime表示的UTC时间
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
///    获取当前程序运行时配置文件路径
///
/// @note
///     c:\a\b\c.exe 返回 c:\a\e\
///     e为b同级目录，保存运行时配置
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
        if (strAppPath.empty())  //对应于可执行程序直接在盘符根目录下情况 \a.exe
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
///    将字符串中指定的子字符串替换成新的字符串
///
/// @param[in]  str    字符串
/// @param[in]  old_value    老的子字符串
/// @param[in]  new_value    新的子字符串
///
void ReplaceSubStr(std::wstring& str, const std::wstring old_value, const std::wstring new_value)
{
    // 检查输入参数
    if (old_value.size() == 0)
    {
        return ;
    }

    // 新老字符串不能相同
    if (old_value.compare(new_value) == 0)
    {
        return ;
    }

    // 依次替换字符串
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
///    根据通道属性，重新生成通道的描述信息
///
/// @param[in]  strOldDescription   当前通道描述信息
/// @param[in]  eOldChannelType     老的通道类型
/// @param[in]  eNewChannelType     新的通道类型
/// @param[in]  eOldChannelPhase    老的通道相角
/// @param[in]  eNewChannelPhase    新的通道相角
/// @param[in]  bOldIsMeasure       原来通道是否是测量类型
/// @param[in]  bNewIsMeasure       现在通道是否是测量类型
///
/// @return
///     std::wstring    新生成的描述信息
///
std::wstring GenerateChannelDes(std::wstring        strOldDescription, 
                                enumChannelType     eOldChannelType,
                                enumChannelType     eNewChannelType, 
                                ChannelPhase        eOldChannelPhase,
                                ChannelPhase        eNewChannelPhase, 
                                bool                bOldIsMeasure,
                                bool                bNewIsMeasure)
{
    // 修改描述信息中的通道类型部分
    if (eNewChannelType != eOldChannelType)
    {
        switch(eNewChannelType)
        {
        case CHANNEL_TYPE_SMV_CURRENT:                                  // 电流
            {
                ReplaceSubStr(strOldDescription, L"电压", L"电流");
                ReplaceSubStr(strOldDescription, L"时间", L"电流");
            }
            break;

        case CHANNEL_TYPE_SMV_VOLTAGE:                                  // 电压
            {
                ReplaceSubStr(strOldDescription, L"电流", L"电压");
                ReplaceSubStr(strOldDescription, L"时间", L"电压");
            }
            break;

        case CHANNEL_TYPE_SMV_TIME:                                     // 时间
            {
                ReplaceSubStr(strOldDescription, L"电流", L"时间");
                ReplaceSubStr(strOldDescription, L"电压", L"时间");
            }
            break;
        }
    }

    // 修改描述信息中的相角部分
    if (eNewChannelPhase != eOldChannelPhase)
    {
        std::wstring    strNewPhase;


        // 获得新相角对应字符串
        if (CHANNEL_PHASE_A == eNewChannelPhase)
        {
            strNewPhase = L"A相";
        }
        else if (CHANNEL_PHASE_B == eNewChannelPhase)
        {
            strNewPhase = L"B相";
        }
        else if (CHANNEL_PHASE_C == eNewChannelPhase)
        {
            strNewPhase = L"C相";
        }
        else if (CHANNEL_PHASE_X == eNewChannelPhase)
        {
            strNewPhase = L"X相";
        }
        else if (CHANNEL_PHASE_N == eNewChannelPhase)
        {
            strNewPhase = L"N相";
        }

        // 修改通道描述字符串
        if (strNewPhase.size() > 0)
        {
            ReplaceSubStr(strOldDescription, L"A相", strNewPhase);
            ReplaceSubStr(strOldDescription, L"B相", strNewPhase);
            ReplaceSubStr(strOldDescription, L"C相", strNewPhase);
            ReplaceSubStr(strOldDescription, L"X相", strNewPhase);
            ReplaceSubStr(strOldDescription, L"N相", strNewPhase);
        }
    }

    // 修改描述信息中的测量部分
    if (bOldIsMeasure != bNewIsMeasure)
    {
        if (bNewIsMeasure)
        {
            ReplaceSubStr(strOldDescription, L"保护", L"测量");
        }
        else
        {
            ReplaceSubStr(strOldDescription, L"测量", L"保护");
        }
    }

    return strOldDescription;
}
