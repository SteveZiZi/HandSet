/// @file
///
/// @brief
///     SMV��������
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.24
///
/// �޸���ʷ��
///

#ifndef __PARAM_CONFIG_SMV_SEND_H__
#define __PARAM_CONFIG_SMV_SEND_H__
#include <map>
#include <vector>
#include "FT3Cb.h"
#include "IEC9_1Cb.h"
#include "IEC9_2Cb.h"
#include "ParamConfigBase.h"
#include "src/utils/cmarkup/markup.h"
#include "src/service/datalayer/include/datatypes.h"
#include "src/service/datalayer/recv/FrameRecvStruct.h"

class CParamConfigSMVSend : public CParamConfigBase
{
private:
    CParamConfigSMVSend(void);
    ~CParamConfigSMVSend(void);

public:
    ///
    /// @brief
    ///     ��ȡ����ʵ���ӿ�
    ///
    static CParamConfigSMVSend* getInstance();

    ///
    /// @brief
    ///    ���������ļ�
    ///
    bool LoadConfig( std::wstring strConfigFile );

    ///
    /// @brief
    ///    ���������ļ�
    ///
    /// @return
    ///     bool    true��ʾ����ɹ�
    ///
    bool SaveConfig();

    ///
    /// @brief
    ///    ���SMV����
    ///
    /// @return
    ///     enumFrameType    ��õ�SMV����
    ///
    enumFrameType GetSMVType();

    ///
    /// @brief
    ///    ����SMV����
    ///
    /// @param[in]  eNewSMVType SMV����
    ///
    void SetSMVType(enumFrameType eNewSMVType);

    ///
    /// @brief
    ///    ��ò���ֵ��ʾ��ʽ
    ///
    /// @return
    ///     enumSmvValueType    ����ֵ��ʾ��ʽ
    ///
    enumSmvValueType GetSVDisplay();

    ///
    /// @brief
    ///    ���ò���ֵ��ʾ��ʽ
    ///
    /// @param[in]  eNewSVDisplay ����ֵ��ʾ��ʽ
    ///
    void SetSVDisplay(enumSmvValueType eNewSVDisplay);

    ///
    /// @brief
    ///    ��ò���Ƶ��
    ///
    /// @return
    ///     FT3_SAMP_RATE    ����Ƶ��
    ///
    FT3_SAMP_RATE GetFrequency();

    ///
    /// @brief
    ///    ���ò���Ƶ��
    ///
    /// @param[in]  eNewFrequency ����Ƶ��
    ///
    void SetFrequency(FT3_SAMP_RATE eNewFrequency);

    ///
    /// @brief
    ///    ��ò�����
    ///
    /// @return
    ///     FT3_BAUD_RATE    ������
    ///
    FT3_BAUD_RATE GetBaudRate();

    ///
    /// @brief
    ///    ���ò�����
    ///
    /// @param[in]  eBaudRate ������
    ///
    void SetBaudRate(FT3_BAUD_RATE eNewBaudRate);

    ///
    /// @brief
    ///    ���ASDU��Ŀ
    ///
    /// @return
    ///     int    ASDU��Ŀ������-1��ʾ��ѯʧ��
    ///
    int GetASDUCount();

    ///
    /// @brief
    ///    ����ASDU��Ŀ
    ///
    /// @param[in]  iNewASDUCount ASDU��Ŀ
    ///
    /// @return
    ///     bool    true��ʾ����ɹ�
    ///
    bool SetASDUCount(int iNewASDUCount);

    ///
    /// @brief
    ///    ���ָ������ͨ���Ľ�ֱ������
    ///
    /// @param[in]  strChannelName ����ͨ����
    ///
    /// @return
    ///     eCurrentType    ����ͨ���Ľ�ֱ������
    ///
    eCurrentType GetCurrentType(std::wstring strChannelName);

    ///
    /// @brief
    ///    ����ָ������ͨ���Ľ�ֱ������
    ///
    /// @param[in]  strChannelName ����ͨ����
    /// @param[in]  eChannelCurType ��ֱ������
    ///
    /// @return
    ///     bool    true��ʾ���óɹ�
    ///
    bool SetCurrentType(std::wstring strChannelName, eCurrentType eChannelCurType);

    ///
    /// @brief
    ///    ö�ٵ�һ��IEC61850-9-1���ƿ�
    ///
    /// @return
    ///     CIEC9_1Cb*    �ɹ�ʱ����IEC61850-9-1���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CIEC9_1Cb* FirstIEC9_1Cb();

    ///
    /// @brief
    ///    �����һ��IEC61850-9-1���ƿ�
    ///
    /// @param[in]  pCurIEC9_1Cb ��ǰIEC61850-9-1���ƿ��ָ��
    ///
    /// @return
    ///     CIEC9_1Cb*    �ɹ�ʱ����IEC61850-9-1���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CIEC9_1Cb* NextIEC9_1Cb(CIEC9_1Cb* pCurIEC9_1Cb);

    ///
    /// @brief
    ///    �����һ��IEC61850-9-1���ƿ�
    ///
    /// @param[in]  pCurIEC9_1Cb ��ǰIEC61850-9-1���ƿ��ָ��
    ///
    /// @return
    ///     CIEC9_1Cb*    �ɹ�ʱ����IEC61850-9-1���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CIEC9_1Cb* PrevIEC9_1Cb(CIEC9_1Cb* pCurIEC9_1Cb);

    ///
    /// @brief
    ///    ������һ��IEC61850-9-1���ƿ�
    ///
    /// @return
    ///     CIEC9_1Cb*    �ɹ�ʱ����IEC61850-9-1���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CIEC9_1Cb* LastIEC9_1Cb();

    ///
    /// @brief
    ///    ��ñ�ѡ�е�IEC61850-9-1���ƿ����
    ///
    /// @return
    ///     int    ��ѡ�еĿ��ƿ����
    ///
    int GetSelectedIEC9_1CbCount();

    ///
    /// @brief
    ///    ���IEC61850-9-1���ƿ�
    ///
    /// @return
    ///     CIEC9_1Cb*    ����ӵ�IEC61850-9-1���ƿ�����ָ��
    ///
    CIEC9_1Cb* AddIEC9_1Cb();

    ///
    /// @brief
    ///     ���IEC61850-9-1���ƿ飨�˺�������ȿ���һ���β���ָ����Ȼ����ӵ��ڲ������У�
    ///
    /// @param[in]
    ///     pNetCb   -   ָ����ƿ��ָ��
    ///
    /// @return
    ///     CIEC9_1Cb*   �ɹ�ʱ��������ӵ�IEC61850-9-1���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CIEC9_1Cb* AddIEC9_1Cb(CNetCb* pNetCb);

    ///
    /// @brief
    ///     ���ɨ���б��е�SMV���ƿ鵽�����У��˺�������ȿ���һ���β���ָ����Ȼ����ӵ��ڲ������У�
    ///
    /// @param[in]
    ///     pDetectedIEC9_1Item    -   ɨ���б��е�SMV���ƿ�ָ��
    ///
    /// @return
    ///     CIEC9_1Cb*    �ɹ�ʱ��������ӵ�IEC61850-9-1���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CIEC9_1Cb* AddIEC9_1Cb(CDetectItemFrame* pDetectedIEC9_1Item);

    ///
    /// @brief
    ///    ɾ��IEC61850-9-1���ƿ�
    ///
    /// @param[in]  pCurIEC9_1Cb IEC61850-9-1���ƿ�ָ��
    ///
    /// @return
    ///     bool    true��ʾɾ���ɹ�
    ///
    bool DeleteIEC9_1Cb(CIEC9_1Cb* pCurIEC9_1Cb);

    ///
    /// @brief
    ///    ɾ��ָ��APPID��IEC61850-9-1���ƿ�
    ///
    /// @param[in]  iAPPID ��ɾ���Ŀ��ƿ�APPID
    ///
    /// @return
    ///     bool    true��ʾɾ���ɹ�
    ///
    bool DeleteIEC9_1Cb(int iAPPID);

    ///
    /// @brief
    ///    �������IEC61850-9-1���ƿ�
    ///
    void ClearAllIEC9_1Cb();

    ///
    /// @brief
    ///    ö�ٵ�һ��IEC61850-9-2���ƿ�
    ///
    /// @return
    ///     CIEC9_2Cb*    �ɹ�ʱ����IEC61850-9-2���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CIEC9_2Cb* FirstIEC9_2Cb();

    ///
    /// @brief
    ///    �����һ��IEC61850-9-2���ƿ�
    ///
    /// @param[in]  pCurIEC9_2Cb ��ǰIEC61850-9-2���ƿ��ָ��
    ///
    /// @return
    ///     CIEC9_2Cb*    �ɹ�ʱ����IEC61850-9-2���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CIEC9_2Cb* NextIEC9_2Cb(CIEC9_2Cb* pCurIEC9_2Cb);

    ///
    /// @brief
    ///    �����һ��IEC61850-9-2���ƿ�
    ///
    /// @param[in]  pCurIEC9_2Cb ��ǰIEC61850-9-2���ƿ��ָ��
    ///
    /// @return
    ///     CIEC9_2Cb*    �ɹ�ʱ����IEC61850-9-2���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CIEC9_2Cb* PrevIEC9_2Cb(CIEC9_2Cb* pCurIEC9_2Cb);

    ///
    /// @brief
    ///    ������һ��IEC61850-9-2���ƿ�
    ///
    /// @return
    ///     CIEC9_2Cb*    �ɹ�ʱ����IEC61850-9-2���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CIEC9_2Cb* LastIEC9_2Cb();

    ///
    /// @brief
    ///    ��ñ�ѡ�е�IEC61850-9-2���ƿ����
    ///
    /// @return
    ///     int    ��ѡ�еĿ��ƿ����
    ///
    int GetSelectedIEC9_2CbCount();

    ///
    /// @brief
    ///    ���IEC61850-9-2���ƿ�
    ///
    /// @return
    ///     CIEC9_2Cb*    ����ӵ�IEC61850-9-2���ƿ�����ָ��
    ///
    CIEC9_2Cb* AddIEC9_2Cb();

    ///
    /// @brief
    ///     ���IEC61850-9-2���ƿ飨�˺�������ȿ���һ���β���ָ����Ȼ����ӵ��ڲ������У�
    ///
    /// @param[in]
    ///     pIEC9_2Cb   -   IEC61850-9-2���ƿ�ָ��
    ///
    /// @return
    ///     CIEC9_2Cb*   �ɹ�ʱ��������ӵ�IEC61850-9-2���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CIEC9_2Cb* AddIEC9_2Cb(CIEC9_2Cb* pIEC9_2Cb);

    ///
    /// @brief
    ///     ���ɨ���б��е�SMV���ƿ鵽�����У��˺�������ȿ���һ���β���ָ����Ȼ����ӵ��ڲ������У�
    ///
    /// @param[in]
    ///     pDetectedIEC9_2Item   -   ɨ���б��е�SMV���ƿ�ָ��
    ///
    /// @return
    ///     CIEC9_2Cb*    �ɹ�ʱ��������ӵ�IEC61850-9-2���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CIEC9_2Cb* AddIEC9_2Cb(CDetectItemFrame* pDetectedIEC9_2Item);

    ///
    /// @brief
    ///    ɾ��IEC61850-9-2���ƿ�
    ///
    /// @param[in]  pCurIEC9_2Cb IEC61850-9-2���ƿ�ָ��
    ///
    /// @return
    ///     bool    true��ʾɾ���ɹ�
    ///
    bool DeleteIEC9_2Cb(CIEC9_2Cb* pCurIEC9_2Cb);

    ///
    /// @brief
    ///    ɾ��ָ��APPID��IEC61850-9-2���ƿ�
    ///
    /// @param[in]  iAPPID ��ɾ���Ŀ��ƿ�APPID
    ///
    /// @return
    ///     bool    true��ʾɾ���ɹ�
    ///
    bool DeleteIEC9_2Cb(int iAPPID);

    ///
    /// @brief
    ///    �������IEC61850-9-2���ƿ�
    ///
    void ClearAllIEC9_2Cb();

    ///
    /// @brief
    ///    ��õ�һ��FT3���ƿ�
    ///
    /// @return
    ///     CFT3Cb*    �ɹ�ʱ����CFT3���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CFT3Cb* FirstFt3Cb();

    ///
    /// @brief
    ///    �����һ��FT3���ƿ�
    ///
    /// @param[in]  pCurFt3Cb ��ǰFT3���ƿ��ָ��
    ///
    /// @return
    ///     CFT3Cb*    �ɹ�ʱ����FT3���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CFT3Cb* NextFt3Cb(CFT3Cb* pCurFt3Cb);

    ///
    /// @brief
    ///    �����һ��FT3���ƿ�
    ///
    /// @param[in]  pCurFt3Cb ��ǰFT3���ƿ��ָ��
    ///
    /// @return
    ///     CFT3Cb*    �ɹ�ʱ����FT3���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CFT3Cb* PrevFt3Cb(CFT3Cb* pCurFt3Cb);

    ///
    /// @brief
    ///    ������һ��FT3���ƿ�
    ///
    /// @return
    ///     CFT3Cb*    �ɹ�ʱ����FT3���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CFT3Cb* LastFt3Cb();

    ///
    /// @brief
    ///    ��ñ�ѡ�е�FT3���ƿ����
    ///
    /// @return
    ///     int    ��ѡ�еĿ��ƿ����
    ///
    int GetSelectedFt3CbCount();

    ///
    /// @brief
    ///    ���FT3���ƿ�
    ///
    /// @return
    ///     CFT3Cb*    ����ӵ�FT3���ƿ�����ָ��
    ///
    CFT3Cb* AddFt3Cb();

    ///
    /// @brief
    ///     ���FT3���ƿ飨�˺�������ȿ���һ���β���ָ����Ȼ����ӵ��ڲ������У�
    ///
    /// @param[in]
    ///     pBaseCb   -   ���ƿ�ָ��
    ///
    /// @return
    ///     CFT3Cb*   �ɹ�ʱ��������ӵ�FT3���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CFT3Cb* AddFt3Cb(CBaseCb* pBaseCb);

    ///
    /// @brief
    ///     ���ɨ���б��е�SMV���ƿ鵽�����У��˺�������ȿ���һ���β���ָ����Ȼ����ӵ��ڲ������У�
    ///
    /// @param[in]
    ///     pDetectedFt3Item   -   ɨ���б��е�SMV���ƿ�ָ��
    ///
    /// @return
    ///     CFT3Cb*    �ɹ�ʱ��������ӵ�FT3���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CFT3Cb* AddFt3Cb(CDetectItemFrame* pDetectedFt3Item);

    ///
    /// @brief
    ///    ɾ��FT3���ƿ�
    ///
    /// @param[in]  pCurFt3Cb FT3���ƿ�ָ��
    ///
    /// @return
    ///     bool    true��ʾɾ���ɹ�
    ///
    bool DeleteFt3Cb(CFT3Cb* pCurFt3Cb);

    ///
    /// @brief
    ///    ɾ��ָ��APPID����LDName����FT3���ƿ�
    ///
    /// @param[in]  iAPPID ��ɾ���Ŀ��ƿ�APPID����LDName��
    ///
    /// @return
    ///     bool    true��ʾɾ���ɹ�
    ///
    bool DeleteFt3Cb(int iAPPID);

    ///
    /// @brief
    ///    �������FT3���ƿ�
    ///
    void ClearAllFt3Cb();

    ///
    /// @brief
    ///    ��õ�һ��FT3��չ���ƿ�
    ///
    /// @return
    ///     CFT3ExpandCb*    �ɹ�ʱ����CFT3��չ���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CFT3ExpandCb* FirstFt3ExpandCb();

    ///
    /// @brief
    ///    �����һ��FT3��չ���ƿ�
    ///
    /// @param[in]  pCurFt3ExpandCb ��ǰFT3��չ���ƿ��ָ��
    ///
    /// @return
    ///     CFT3ExpandCb*    �ɹ�ʱ����FT3��չ���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CFT3ExpandCb* NextFt3ExpandCb(CFT3ExpandCb* pCurFt3ExpandCb);

    ///
    /// @brief
    ///    �����һ��FT3��չ���ƿ�
    ///
    /// @param[in]  pCurFt3ExpandCb ��ǰFT3��չ���ƿ��ָ��
    ///
    /// @return
    ///     CFT3ExpandCb*    �ɹ�ʱ����FT3��չ���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CFT3ExpandCb* PrevFt3ExpandCb(CFT3ExpandCb* pCurFt3ExpandCb);

    ///
    /// @brief
    ///    ������һ��FT3��չ���ƿ�
    ///
    /// @return
    ///     CFT3ExpandCb*    �ɹ�ʱ����FT3��չ���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CFT3ExpandCb* LastFt3ExpandCb();

    ///
    /// @brief
    ///    ��ñ�ѡ�е�FT3��չ���ƿ����
    ///
    /// @return
    ///     int    ��ѡ�еĿ��ƿ����
    ///
    int GetSelectedFt3ExpandCbCount();

    ///
    /// @brief
    ///    ���FT3��չ���ƿ�
    ///
    /// @return
    ///     CFT3ExpandCb*    ����ӵ�FT3��չ���ƿ�����ָ��
    ///
    CFT3ExpandCb* AddFt3ExpandCb();

    ///
    /// @brief
    ///     ���FT3��չ���ƿ飨�˺�������ȿ���һ���β���ָ����Ȼ����ӵ��ڲ������У�
    ///
    /// @param[in]
    ///     pBaseCb   -   ���ƿ�ָ��
    ///
    /// @return
    ///     CFT3ExpandCb*   �ɹ�ʱ��������ӵ�FT3��չ���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CFT3ExpandCb* AddFt3ExpandCb(CBaseCb* pBaseCb);

    ///
    /// @brief
    ///     ���ɨ���б��е�SMV���ƿ鵽�����У��˺�������ȿ���һ���β���ָ����Ȼ����ӵ��ڲ������У�
    ///
    /// @param[in]
    ///     pDetectedFt3ExpandItem   -   ɨ���б��е�SMV���ƿ�ָ��
    ///
    /// @return
    ///     CFT3ExpandCb*    �ɹ�ʱ��������ӵ�FT3��չ���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CFT3ExpandCb* AddFt3ExpandCb(CDetectItemFrame* pDetectedFt3ExpandItem);

    ///
    /// @brief
    ///    ɾ��FT3��չ���ƿ�
    ///
    /// @param[in]  pCurFt3ExpandCb FT3��չ���ƿ�ָ��
    ///
    /// @return
    ///     bool    true��ʾɾ���ɹ�
    ///
    bool DeleteFt3ExpandCb(CFT3ExpandCb* pCurFt3ExpandCb);

    ///
    /// @brief
    ///    ɾ��ָ��APPID����LDName����FT3��չ���ƿ�
    ///
    /// @param[in]  iAPPID ��ɾ���Ŀ��ƿ�APPID����LDName��
    ///
    /// @return
    ///     bool    true��ʾɾ���ɹ�
    ///
    bool DeleteFt3ExpandCb(int iAPPID);

    ///
    /// @brief
    ///    �������FT3��չ���ƿ�
    ///
    void ClearAllFt3ExpandCb();

    ///
    /// @brief
    ///    �������п��ƿ��е�ѹͨ����һ�ζֵ
    ///
    /// @param[in]  dNewValue    �µĵ�ѹһ�ζֵ��V��
    ///
    void UpdateRatedPrimaryVoltage(double dNewValue);

    ///
    /// @brief
    ///    �������п��ƿ��е�ѹͨ���Ķ��ζֵ
    ///
    /// @param[in]  dNewValue    �µĵ�ѹ���ζֵ��V��
    ///
    void UpdateRatedSecondaryVoltage(double dNewValue);

    ///
    /// @brief
    ///    �������п��ƿ��е���ͨ����һ�ζֵ
    ///
    /// @param[in]  dNewValue    �µĵ���һ�ζֵ��A��
    ///
    void UpdateRatedPrimaryCurrent(double dNewValue);

    ///
    /// @brief
    ///    �������п��ƿ��е���ͨ���Ķ��ζֵ
    ///
    /// @param[in]  dNewValue    �µĵ������ζֵ��A��
    ///
    void UpdateRatedSecondaryCurrent(double dNewValue);

    ///
    /// @brief
    ///    �������п��ƿ���ʱ��ͨ���Ķֵ
    ///
    /// @param[in]  dNewValue    �µ�MU���ʱ��us��
    ///
    void UpdateMuRatedDelay(double dNewValue);

    ///
    /// @brief
    ///     ���Թ�ѡָ����IEC61850-9-1���ƿ�
    ///
    /// @param[in]  pIec9_1Cb   ָ����ƿ�����ָ��
    ///
    /// @return
    ///     bool    ��ѡ�ɹ�����true
    ///
    bool TryToSelectIec9_1Cb(CIEC9_1Cb* pIec9_1Cb);

    ///
    /// @brief
    ///     ���Թ�ѡָ����IEC61850-9-2���ƿ�
    ///
    /// @param[in]  pIec9_2Cb   ָ����ƿ�����ָ��
    ///
    /// @return
    ///     bool    ��ѡ�ɹ�����true
    ///
    bool TryToSelectIec9_2Cb(CIEC9_2Cb* pIec9_2Cb);

    ///
    /// @brief
    ///     �����������Ϳ��ƿ�Ĺ�ѡ��
    ///
    void ReviseSelectedCbCount();

private:
    ///
    /// @brief
    ///    ���IEC61850-9-1���ƿ�
    ///
    /// @param[in]  pNewCb ָ��IEC61850-9-1���ƿ�����ָ��
    ///
    /// @return
    ///     bool    true��ʾ��ӳɹ�
    ///
    bool __AddIEC9_1Cb(CIEC9_1Cb* pNewCb);

    ///
    /// @brief
    ///    ���IEC61850-9-2���ƿ�
    ///
    /// @param[in]  pNewCb ָ��IEC61850-9-2���ƿ�����ָ��
    ///
    /// @return
    ///     bool    true��ʾ��ӳɹ�
    ///
    bool __AddIEC9_2Cb(CIEC9_2Cb* pNewCb);

    ///
    /// @brief
    ///    ���FT3���ƿ�
    ///
    /// @param[in]  pNewCb ָ��FT3���ƿ�����ָ��
    ///
    /// @return
    ///     bool    true��ʾ��ӳɹ�
    ///
    bool __AddFt3Cb(CFT3Cb* pNewCb);

    ///
    /// @brief
    ///    ���FT3��չ���ƿ�
    ///
    /// @param[in]  pNewCb ָ��FT3��չ���ƿ�����ָ��
    ///
    /// @return
    ///     bool    true��ʾ��ӳɹ�
    ///
    bool __AddFt3ExpandCb(CFT3ExpandCb* pNewCb);

    ///
    /// @brief
    ///     ���ָ��APPID��IEC61850-9-1���ƿ飨ע����ͬAPPID�Ŀ��ƿ�����ж�����˴�ֻ��õ�һ��ƥ��Ŀ��ƿ飩
    ///
    /// @param[in]  
    ///     iAPPID  -   ���ƿ��APPID
    ///
    /// @return
    ///     CGooseCb*   �ɹ�ʱ���ؿ��ƿ��ָ�룬ʧ��ʱ����NULL
    ///
    CIEC9_1Cb* __GetIEC9_1Cb(int iAPPID);

    ///
    /// @brief
    ///     ���ָ��APPID��IEC61850-9-2���ƿ飨ע����ͬAPPID�Ŀ��ƿ�����ж�����˴�ֻ��õ�һ��ƥ��Ŀ��ƿ飩
    ///
    /// @param[in]  
    ///     iAPPID  -   ���ƿ��APPID
    ///
    /// @return
    ///     CGooseCb*   �ɹ�ʱ���ؿ��ƿ��ָ�룬ʧ��ʱ����NULL
    ///
    CIEC9_2Cb* __GetIEC9_2Cb(int iAPPID);

    ///
    /// @brief
    ///     ���ָ��APPID��FT3���ƿ飨ע����ͬAPPID�Ŀ��ƿ�����ж�����˴�ֻ��õ�һ��ƥ��Ŀ��ƿ飩
    ///
    /// @param[in]  
    ///     iAPPID  -   ���ƿ��APPID
    ///
    /// @return
    ///     CFT3Cb*     �ɹ�ʱ���ؿ��ƿ��ָ�룬ʧ��ʱ����NULL
    ///
    CFT3Cb* __GetFt3Cb(int iAPPID);

    ///
    /// @brief
    ///     ���ָ��APPID��FT3��չ���ƿ飨ע����ͬAPPID�Ŀ��ƿ�����ж�����˴�ֻ��õ�һ��ƥ��Ŀ��ƿ飩
    ///
    /// @param[in]  
    ///     iAPPID  -       ���ƿ��APPID
    ///
    /// @return
    ///     CFT3ExpandCb*   �ɹ�ʱ���ؿ��ƿ��ָ�룬ʧ��ʱ����NULL
    ///
    CFT3ExpandCb* __GetFt3ExpandCb(int iAPPID);

private:
    enumFrameType                           m_eSMVType;                 ///< SMV����
    enumSmvValueType                        m_eSVDisplay;               ///< ����ֵ��ʾ
    FT3_SAMP_RATE                           m_eFrequency;               ///< ����Ƶ��
    FT3_BAUD_RATE                           m_eBuadRate;                ///< ������
    unsigned int                            m_uiASDUCount;              ///< ASDU��Ŀ

    std::map<std::wstring , eCurrentType>   m_mapCurrentType;           ///< ��ֱ������
    std::list<CIEC9_1Cb *>                  m_listIEC9_1CbGroup;        ///< IEC61850-9-1���ƿ�����
    std::list<CIEC9_2Cb *>                  m_listIEC9_2CbGroup;        ///< IEC61850-9-2���ƿ�����
    std::list<CFT3Cb *>                     m_listFt3CbGroup;           ///< FT3���ƿ�����
    std::list<CFT3ExpandCb *>               m_listFt3ExpandCbGroup;     ///< FT3��չ���ƿ�����
};
#endif
