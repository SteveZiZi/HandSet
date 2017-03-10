/**
 * \file
 *      utility.h
 *
 * \brief
 *      ����С����ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/7/11
 */

#pragma once

/**
 *      ����ͨ�����͡���ֵ��ʾ���ͼ��ͨ�����ͻ�ȡ��λ
 *
 * \param[in]
 *      eChannType          ͨ������
 *  \param[in]
 *      eDispType           ��ʾ����
 *  \param[in]
 *      eRateType           �ͨ������
 */
extern CString DM_GetUnitFromType(enumChannelType eChannType, enumSmvValueType eDispType, enumSmvValueType eRateType);

/**
 *      ��ʾ¼���쳣״̬����Ϣ
 *
 *  \param[in]
 *      nErrorID            ����ID�������ݲ㷵��
 */
extern void DM_ShowReordErrorInfo(DWORD nErrorID);


/**
 *      ��ȡ��ǰUTCʱ�������
 *
 *  \return
 *      UTC_TIME_t          ����UTCʱ���������Ϣ
 */
extern UTC_TIME_t DM_GetUTC();

/**
 *      ��UTCʱ���ȡ����ʱ��
 *
 *  \param[in]
 *      t                   UTCʱ����������1970����ҹ��ʼ
 *  return
 *      SYSTEMTIME          ���ر���ʱ��
 */
extern SYSTEMTIME DM_GetLocalTimeFromUTC(UTC_TIME_t t);

/**
 *      ��UTC��ȡ����ʱ����ַ�������
 *
 *  \param[in]
 *      t                   UTCʱ����������1970����ҹ��ʼ
 *  return
 *      CString             ���ر���ʱ����ַ�������
 */
extern CString DM_GetLocalTimeStrFromUTC(UTC_TIME_t t);


/**
 *      ��ȡUTCʱ��
 *
 *  \param[in]
 *      seconds         ��
 *  \param[in]
 *      fraction        ΢��
 */
extern SYSTEMTIME DM_GetSystemTimeFromUTC(int64_t seconds, int32_t fraction);

/**
 *      ��ȡUTCʱ����ַ�������
 */
extern CString DM_GetSystemTimeStrFromUTC(UTC_TIME_t t);


/**
 *      ϵͳʱ��ת��ΪUTCʱ��
 */
extern UTC_TIME_t DM_GetUTCFromSystemTime( LPSYSTEMTIME lpSystemTime);

/**
 *      UTCʱ��ת��Ϊ����ʱ��
 */
extern BOOL DM_SystemTimeToTzSpecificLocalTime(LPSYSTEMTIME lpUniversalTime, LPSYSTEMTIME lpLocalTime);

/**
 *      ����ʱ��ת��ΪUTCʱ��
 */
extern BOOL DM_TzSpecificLocalTimeToSystemTime(LPSYSTEMTIME lpLocalTime, LPSYSTEMTIME lpUniversalTime);


/**
 *      ����SCL�ص���������
 */
struct _SCLTaskArgs
{
    CWnd*   m_pWnd;
    CString csFile;
};


/**
 *      ����SCL�����ļ�
 */
extern DWORD WINAPI _LoadSCLTask(void* arg);


