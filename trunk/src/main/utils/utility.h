/**
 * \file
 *      utility.h
 *
 * \brief
 *      辅助小函数头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/7/11
 */

#pragma once

/**
 *      根据通道类型、数值显示类型及额定通道类型获取单位
 *
 * \param[in]
 *      eChannType          通道类型
 *  \param[in]
 *      eDispType           显示类型
 *  \param[in]
 *      eRateType           额定通道类型
 */
extern CString DM_GetUnitFromType(enumChannelType eChannType, enumSmvValueType eDispType, enumSmvValueType eRateType);

/**
 *      显示录波异常状态的信息
 *
 *  \param[in]
 *      nErrorID            错误ID，由数据层返回
 */
extern void DM_ShowReordErrorInfo(DWORD nErrorID);


/**
 *      获取当前UTC时间的秒数
 *
 *  \return
 *      UTC_TIME_t          返回UTC时间的秒数信息
 */
extern UTC_TIME_t DM_GetUTC();

/**
 *      从UTC时间获取本地时间
 *
 *  \param[in]
 *      t                   UTC时间秒数，从1970年午夜开始
 *  return
 *      SYSTEMTIME          返回本地时间
 */
extern SYSTEMTIME DM_GetLocalTimeFromUTC(UTC_TIME_t t);

/**
 *      从UTC获取本地时间的字符串描述
 *
 *  \param[in]
 *      t                   UTC时间秒数，从1970年午夜开始
 *  return
 *      CString             返回本地时间的字符串描述
 */
extern CString DM_GetLocalTimeStrFromUTC(UTC_TIME_t t);


/**
 *      获取UTC时间
 *
 *  \param[in]
 *      seconds         秒
 *  \param[in]
 *      fraction        微秒
 */
extern SYSTEMTIME DM_GetSystemTimeFromUTC(int64_t seconds, int32_t fraction);

/**
 *      获取UTC时间的字符串描述
 */
extern CString DM_GetSystemTimeStrFromUTC(UTC_TIME_t t);


/**
 *      系统时间转化为UTC时间
 */
extern UTC_TIME_t DM_GetUTCFromSystemTime( LPSYSTEMTIME lpSystemTime);

/**
 *      UTC时间转化为本地时间
 */
extern BOOL DM_SystemTimeToTzSpecificLocalTime(LPSYSTEMTIME lpUniversalTime, LPSYSTEMTIME lpLocalTime);

/**
 *      本地时间转化为UTC时间
 */
extern BOOL DM_TzSpecificLocalTimeToSystemTime(LPSYSTEMTIME lpLocalTime, LPSYSTEMTIME lpUniversalTime);


/**
 *      加载SCL回调函数参数
 */
struct _SCLTaskArgs
{
    CWnd*   m_pWnd;
    CString csFile;
};


/**
 *      加载SCL配置文件
 */
extern DWORD WINAPI _LoadSCLTask(void* arg);


