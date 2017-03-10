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
#ifndef DATA_UTILITY_H__
#define DATA_UTILITY_H__

#include <string>
#include "src\utils\SmvScaler\SmvScaler.h"
#include "src\service\datalayer\config\ParamConfigBasic.h"
#include "src\service\datalayer\config\SmvRecvConfigPublic.h"
#include "src\service\datalayer\include\datatypes.h"


///
/// @brief
///    ��ȡ��ǰģ�����ڵ�·��
///
/// @note
///     c:\a\b\c.exe ���� c:\a\b\
///
const std::wstring GetAppDir();

///
/// @brief
///    ��ȡ��ǰ��������ʱ�����ļ�·��
///
/// @note
///     c:\a\b\c.exe ���� c:\a\e\
///     eΪbͬ��Ŀ¼����������ʱ����
///
const std::wstring GetAppRunTimeCfgDir();

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
bool StrFloatConvert(const std::wstring& strVal, std::wstring* pstrResult, double* pdResult = NULL);

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
bool StrUint32Convert(const std::wstring& strVal, std::wstring* pstrResult, uint32_t* pnResult = NULL);

///
/// @brief
///    ������ת�ַ���
///
/// @param[in]      fVal        Ҫת���ĸ�����
/// @param[out]     strResult   ����ת����Ľ��
/// @param[in]      nPoints     С�����λ��
///
/// @return
///     bool       ת���ɹ�����ͷtrue
///
bool FloatStrConvert(double fVal, std::wstring& strResult, uint32_t nPoints = 3);

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
bool ConvMacWStr2Char(const std::wstring strMAC, unsigned char* pucMAC, int iLen);

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
std::wstring ConvMacChar2WStr(unsigned char* pucMAC, int iLen);

///
/// @brief
///     ��b��utcʱ��ת��ΪSYSTEMTIME
///
/// @return
///     SYSTEMTIME    SystemTime��ʾ��UTCʱ��
///
SYSTEMTIME UtcTime2SystemTime(UTC_TIME_t stUtcTime);

///
/// @brief
///    ���ַ�����ָ�������ַ����滻���µ��ַ���
///
/// @param[in]  str    �ַ���
/// @param[in]  old_value    �ϵ����ַ���
/// @param[in]  new_value    �µ����ַ���
///
void ReplaceSubStr(std::wstring& str, const std::wstring old_value, const std::wstring new_value);

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
                                bool                bNewIsMeasure);

#endif //DATA_UTILITY_H__
