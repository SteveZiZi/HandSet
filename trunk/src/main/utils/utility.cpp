/**
 * \file
 *      utility.cpp
 *
 * \brief
 *      辅助小函数实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/7/11
 */

#include "stdafx.h"
#include "utility.h"
#include "src/main/resourcestr.h"
#include "src/main/mgr/statusmgr.h"
#include "src/service/datalayer/dsmdatalayer.h"


/**
 *      根据通道类型、数值显示类型及额定通道类型获取单位
 *
 * \param[in]
 *      eChannType          通道类型
 *  \param[in]
 *      eDispType           显示类型
 *  \param[in]
 *      eRateType           额定通道类型
 *  \return
 *      CString             返回单位字符串
 */
extern CString DM_GetUnitFromType( enumChannelType eChannType, enumSmvValueType eDispType, enumSmvValueType eRateType )
{
    CString csUnit(L"");

    // 显示一次值 或 显示原始值并且额定通道类型是一次值
    if(SMV_VALUE_TYPE_PRIMARY == eDispType || (eRateType == SMV_VALUE_TYPE_ORIGINANL && SMV_VALUE_TYPE_ORIGINANL == eDispType))
    {
        if(eChannType == CHANNEL_TYPE_SMV_CURRENT)
        {
            csUnit = L"A";
        }
        else if(eChannType == CHANNEL_TYPE_SMV_VOLTAGE)
        {
            csUnit = L"kV";
        }
        else if(eChannType == CHANNEL_TYPE_SMV_TIME)
        {
            csUnit = L"μs";
        }
    }
    // 显示二次值 或 显示原始值并且额定通道类型是二次值
    else if(SMV_VALUE_TYPE_SECONDARY == eDispType || (eRateType == SMV_VALUE_TYPE_SECONDARY && SMV_VALUE_TYPE_ORIGINANL == eDispType))
    {
        if(eChannType == CHANNEL_TYPE_SMV_CURRENT)
        {
            csUnit = L"A";
        }
        else if(eChannType == CHANNEL_TYPE_SMV_VOLTAGE)
        {
            csUnit = L"V";
        }
        else if(eChannType == CHANNEL_TYPE_SMV_TIME)
        {
            csUnit = L"μs";
        }
    }

    return csUnit;
}

/**
 *      显示录波异常状态的信息
 *
 *  \param[in]
 *      nErrorID            错误ID，由数据层返回
 */
extern void DM_ShowReordErrorInfo( DWORD nErrorID )
{
    DWORD nInfo = DSM_STR_INFO_RECORD_UNKNOWN_ERROR;

    switch(nErrorID)
    {
    case DATA_LAYER_FILE_OPEN_FAILED:
    case DATA_LAYER_FILE_TOO_MANY_OPEN_FILES:
        nInfo = DSM_STR_INFO_RECORD_OPEN_FILE_FAILED;
        break;

    case DATA_LAYER_FILE_NOT_FOUND:
        nInfo = DSM_STR_INFO_RECORD_FILE_NOT_FOUND;
        break;

    case DATA_LAYER_FILE_BAD_PATH:
    case DATA_LAYER_FILE_REMOVE_CURRENT_DIR:
        nInfo = DSM_STR_INFO_RECORD_DIRECTORY_ERROR;
        break;

    case DATA_LAYER_FILE_DIR_FULL:
        nInfo = DSM_STR_INFO_RECORD_DIRECTORY_FULL;
        break;

    case DATA_LAYER_FILE_ACCESS_DENIED:
        nInfo = DSM_STR_INFO_RECORD_ACCESS_ERROR;
        break;

    case DATA_LAYER_FILE_INVALID_FILE:
        nInfo = DSM_STR_INFO_RECORD_FILE_HANDLE_ERROR;
        break;

    case DATA_LAYER_FILE_HARD_IO:
        nInfo = DSM_STR_INFO_RECORD_HARD_IO_ERROR;
        break;

    case DATA_LAYER_FILE_DISK_FULL:
        nInfo = DSM_STR_INFO_RECORD_CARD_FULL;
        break;

        // SD卡被拔出
    case 201:
        nInfo = DSM_STR_INFO_RECORD_SD_DISCONNECT;
        break;

        // 线路故障
    case 202:
        nInfo = DSM_STR_INFO_RECORD_LINK_DIS;
        break;

    case DATA_LAYER_FILE_GEN_EXCEPTION:
    case DATA_LAYER_FILE_BAD_SEEK:
    case DATA_LAYER_FILE_SHARE_VIOLATION:
    case DATA_LAYER_FILE_LOCK_VIOLATION:
    case DATA_LAYER_UNKOWN_EXCEPTION:
    case DATA_LAYER_FILE_END_OF_FILE:
    default:
        nInfo = DSM_STR_INFO_RECORD_UNKNOWN_ERROR;
        break;
    }

    CStatusMgr::SetTips(dsm_stringMgr::Get(nInfo));
}

/**
 *      获取当前UTC时间的秒数
 *
 *  \return
 *      UTC_TIME_t          返回UTC时间的秒数信息
 */
extern UTC_TIME_t DM_GetUTC()
{
    __time64_t tt64;
    _time64(&tt64);

    UTC_TIME_t utt = {uint32_t(tt64), 0};

    return utt;
}

/**
 *      从UTC时间获取本地时间
 *
 *  \param[in]
 *      t                   UTC时间秒数，从1970年午夜开始
 *  return
 *      SYSTEMTIME          返回本地时间
 */
extern SYSTEMTIME DM_GetLocalTimeFromUTC( UTC_TIME_t t )
{
    SYSTEMTIME  sysTime;
    CTime       stTm1((__time64_t)t.uSecondSinceEpoch);
    tm          stTm2;

    stTm1.GetLocalTm(&stTm2);
    sysTime.wYear           = (WORD)stTm2.tm_year + 1900;
    sysTime.wMonth          = (WORD)stTm2.tm_mon + 1;
    sysTime.wDay            = (WORD)stTm2.tm_mday;
    sysTime.wHour           = (WORD)stTm2.tm_hour;
    sysTime.wMinute         = (WORD)stTm2.tm_min;
    sysTime.wSecond         = (WORD)stTm2.tm_sec;
    sysTime.wDayOfWeek      = (WORD)stTm2.tm_wday;
    
    sysTime.wMilliseconds   = (WORD)(t.uFractionOfSecond/1000);

    return sysTime;
}

/**
 *      从UTC获取本地时间的字符串描述
 *
 *  \param[in]
 *      t                   UTC时间秒数，从1970年午夜开始
 *  return
 *      CString             返回本地时间的字符串描述
 */
extern CString DM_GetLocalTimeStrFromUTC( UTC_TIME_t t )
{
    SYSTEMTIME st = DM_GetLocalTimeFromUTC(t);

    CString csTime;
    csTime.Format(L"%04d-%02d-%02d %02d:%02d:%02d.%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    return csTime;
}

extern SYSTEMTIME DM_GetSystemTimeFromUTC( int64_t seconds, int32_t fraction )
{
    SYSTEMTIME  sysTime;
    CTime       stTm1(seconds);
    tm          stTm2;

    stTm1.GetGmtTm(&stTm2);
    sysTime.wYear           = (WORD)stTm2.tm_year + 1900;
    sysTime.wMonth          = (WORD)stTm2.tm_mon + 1;
    sysTime.wDay            = (WORD)stTm2.tm_mday;
    sysTime.wHour           = (WORD)stTm2.tm_hour;
    sysTime.wMinute         = (WORD)stTm2.tm_min;
    sysTime.wSecond         = (WORD)stTm2.tm_sec;
    sysTime.wDayOfWeek      = (WORD)stTm2.tm_wday;

    sysTime.wMilliseconds   = (WORD)(fraction/1000);

    return sysTime;
}


extern CString DM_GetSystemTimeStrFromUTC( UTC_TIME_t t)
{
    SYSTEMTIME st = DM_GetSystemTimeFromUTC(t.uSecondSinceEpoch, t.uFractionOfSecond);

    CString csTime;
    csTime.Format(L"%04d-%02d-%02d %02d:%02d:%02d.%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    return csTime;
}

extern UTC_TIME_t DM_GetUTCFromSystemTime( LPSYSTEMTIME lpSystemTime)
{
    FILETIME ftTime;
    SystemTimeToFileTime(lpSystemTime, &ftTime);

    UTC_TIME_t utt;
    utt.uSecondSinceEpoch = ftTime.dwHighDateTime;
    utt.uFractionOfSecond = ftTime.dwLowDateTime;

    return utt;
}

extern BOOL DM_SystemTimeToTzSpecificLocalTime(LPSYSTEMTIME lpUniversalTime,LPSYSTEMTIME lpLocalTime)
{
    if(lpUniversalTime == NULL || lpLocalTime == NULL)
    {
        //如果指针为空，则没有必要进行任何计算
        return FALSE;
    }
    
    TIME_ZONE_INFORMATION tzInfo;
    ::ZeroMemory(&tzInfo, sizeof(tzInfo));
    DWORD dstFlag = GetTimeZoneInformation(&tzInfo);
    (void)dstFlag;

    //将UTC时间由SYSTEMTIME转换为FILETIME格式
    FILETIME ftUniversalTime = {0};
    SystemTimeToFileTime(lpUniversalTime,&ftUniversalTime);

    //将FILETIME格式时间的数值存储到一个DWORD64变量中
    DWORD64 ddwUniversalTime = ftUniversalTime.dwHighDateTime;
    ddwUniversalTime = ddwUniversalTime << 32;
    ddwUniversalTime += ftUniversalTime.dwLowDateTime;


    //因为FILETIME的时间单位是100个亿分之一秒，然后TIME_ZONE_INFORMATION的时间单位是分，所以这里需要乘以600000000 
    DWORD64 ddwBias = ((DWORD64)-tzInfo.Bias) * 600000000; 

    //转换公式为：LOCAL_TIME = UTC - BIAS
    DWORD64 ddwLocalTime = 0;
    ddwLocalTime = ddwUniversalTime + ddwBias;

    //将DWORD64数值转换为FILETIME格式
    FILETIME ftLocalTime = {0};
    ftLocalTime.dwLowDateTime = static_cast<DWORD>(ddwLocalTime);
    ftLocalTime.dwHighDateTime = static_cast<DWORD>(ddwLocalTime >> 32);

    //将FILETIME数值转换为SYSTEMTIME格式并返回
    return FileTimeToSystemTime(&ftLocalTime,lpLocalTime);
}

extern BOOL DM_TzSpecificLocalTimeToSystemTime(LPSYSTEMTIME lpLocalTime, LPSYSTEMTIME lpUniversalTime)
{
    if(lpLocalTime == NULL || lpUniversalTime == NULL)
    {
        //如果指针为空，则没有必要进行任何计算
        return FALSE;
    }

    TIME_ZONE_INFORMATION tzInfo;
    ::ZeroMemory(&tzInfo, sizeof(tzInfo));
    GetTimeZoneInformation(&tzInfo);

    //将UTC时间由SYSTEMTIME转换为FILETIME格式
    FILETIME ftLocalTime = {0};
    SystemTimeToFileTime(lpLocalTime,&ftLocalTime);

    //将FILETIME格式时间的数值存储到一个DWORD64变量中
    DWORD64 ddwLocalTime = ftLocalTime.dwHighDateTime;
    ddwLocalTime = ddwLocalTime << 32;
    ddwLocalTime += ftLocalTime.dwLowDateTime;


    //因为FILETIME的时间单位是100个亿分之一秒，然后TIME_ZONE_INFORMATION的时间单位是分，所以这里需要乘以600000000 
    DWORD64 ddwBias = ((DWORD64)-tzInfo.Bias) * 600000000; 

    //转换公式为：LOCAL_TIME = UTC - BIAS
    DWORD64 ddwUniversalTime = 0;
    ddwUniversalTime = ddwLocalTime - ddwBias;

    //将DWORD64数值转换为FILETIME格式
    FILETIME ftUniversalTime = {0};
    ftUniversalTime.dwLowDateTime = static_cast<DWORD>(ddwUniversalTime);
    ftUniversalTime.dwHighDateTime = static_cast<DWORD>(ddwUniversalTime >> 32);

    //将FILETIME数值转换为SYSTEMTIME格式并返回
    return FileTimeToSystemTime(&ftUniversalTime,lpUniversalTime);
}


extern DWORD WINAPI _LoadSCLTask(void* arg)
{
    _SCLTaskArgs* pArgs = (_SCLTaskArgs*)arg;

    ASSERT(pArgs);

    // 加载scl文件
    CSCLManager* pSCLMgr = CDsmDataLayer::getInstance()->GetSclManager();

    bool bRet = pSCLMgr->Load((LPCTSTR)pArgs->csFile);

    if(pArgs->m_pWnd && pArgs->m_pWnd->GetSafeHwnd())
    {
        if(bRet)
        {
            ((CDialog*)pArgs->m_pWnd)->EndDialog(IDOK);
        }
        else
        {
            ((CDialog*)pArgs->m_pWnd)->EndDialog(IDCANCEL);
        }
    }

    return 1;
}
