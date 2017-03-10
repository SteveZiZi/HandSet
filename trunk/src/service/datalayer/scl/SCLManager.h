/// @file
///
/// @brief
///     SCL�����ļ�����ʹ��src\utils\SclAnalysisģ��
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///     chao        2013.4.7
///
/// �޸���ʷ��
///     Modified by:    chao
///     Modified date:  2013.6.1
///     Version:        1.0.1
///     Descriptions:   ��ȡ��SCL�ļ��п��ƿ����ϸ����Ϣ
///

#ifndef __SCL_MANAGER_H__
#define __SCL_MANAGER_H__

#include <map>
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\include\datatypes.h"
#include "src\utils\SclAnalysis\include\sclanalysis.h"
#include "src\utils\SclAnalysis\include\SCDGOOSEAnalysis.h"
#include "src\utils\SclAnalysis\include\SCDSMV92Analysis.h"
#include "src\utils\SclAnalysis\include\ISCLCacheManager.h"
#include "src\service\datalayer\config\GooseCb.h"
#include "src\service\datalayer\config\IEC9_2Cb.h"

class CSCLManager
{
private:
    CSCLManager(void);
    ~CSCLManager(void);

public:
    ///
    /// @brief
    ///    ��ȡ��̬ʵ����ȫ��ʹ��һ��������
    ///
    static CSCLManager* getInstance();

    ///
    /// @brief
    ///    ����scl�ļ�
    ///
    bool Load(std::wstring strFileName);

    ///
    /// @brief
    ///    ж��SCL�ļ�
    ///
    void UnLoad();

    ///
    /// @brief
    ///    ָʾSCL�������Ƿ��ѳɹ�������SCL�ļ�
    ///
    /// @return
    ///     true ��ʾSCL�������Ѿ��ɹ�������SCL�ļ�
    ///
    bool IsSclLoaded();

    ///
    /// @brief
    ///    ö�ٵ�һ��GOOSE���ƿ�
    ///
    /// @return
    ///     CGooseCb*    �ɹ�ʱ����GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CGooseCb* FirstGooseCb();

    ///
    /// @brief
    ///    ö����һ��GOOSE���ƿ�
    ///
    /// @param[in]  pCurGocb ��ǰλ��GOOSE���ƿ��ָ��
    ///
    /// @return
    ///     CGooseCb*    �ɹ�ʱ����GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CGooseCb* NextGooseCb(CGooseCb* pCurGocb);

    ///
    /// @brief
    ///     ���ָ��APPID��GOOSE���ƿ�
    ///
    /// @param[in]  
    ///     iAPPID  -   ���ƿ��APPID
    ///
    /// @return
    ///     CGooseCb*   �ɹ�ʱ���ؿ��ƿ��ָ�룬ʧ��ʱ����NULL
    ///
    CGooseCb* GetGooseCb(int iAPPID);

    ///
    /// @brief
    ///    ���GOOSE���ƿ����
    ///
    /// @return
    ///     int    ���ƿ����
    ///
    int GetGooseCbCount();

    ///
    /// @brief
    ///    ѡ��ָ��APPID��GOOSE���ƿ�
    ///
    void SelectGooseCb(int iAPPID);

    ///
    /// @brief
    ///    �������GOOSE���ƿ��ѡ��״̬
    ///
    void UnSelectAllGooseCb();

    ///
    /// @brief
    ///    ��ñ�ѡ�е�GOOSE���ƿ����
    ///
    /// @return
    ///     int    ��ѡ�еĿ��ƿ����
    ///
    int GetSelectedGooseCbCount();

    ///
    /// @brief
    ///     ö�ٵ�һ��IEC81850-9-2���ƿ�
    ///
    /// @return
    ///     CIEC9_2Cb*    �ɹ�ʱ���ص�һ�����ƿ��ָ�룬ʧ��ʱ����NULL
    ///
    CIEC9_2Cb* FirstIEC9_2Cb();

    ///
    /// @brief
    ///     ö����һ��IEC81850-9-2���ƿ�
    ///
    /// @param[in]  
    ///     pCurIEC9_2Cb    -   ��ǰλ��IEC81850-9-2���ƿ��ָ��
    ///
    /// @return
    ///     CIEC9_2Cb*  �ɹ�ʱ������һ�����ƿ��ָ�룬ʧ��ʱ����NULL
    ///
    CIEC9_2Cb* NextIEC9_2Cb(CIEC9_2Cb* pCurIEC9_2Cb);

    ///
    /// @brief
    ///     ���ָ��APPID��IEC81850-9-2���ƿ�
    ///
    /// @param[in]  
    ///     iAPPID    -   ���ƿ��APPID
    ///
    /// @return
    ///     CIEC9_2Cb*  �ɹ�ʱ���ؿ��ƿ��ָ�룬ʧ��ʱ����NULL
    ///
    CIEC9_2Cb* GetIEC9_2Cb(int iAPPID);

    ///
    /// @brief
    ///    ���IEC81850-9-2���ƿ����
    ///
    /// @return
    ///     int    ���ƿ����
    ///
    int GetIEC9_2CbCount();

    ///
    /// @brief
    ///    ѡ��ָ��APPID��IEC81850-9-2���ƿ�
    ///
    void SelectIEC9_2Cb(int iAPPID);

    ///
    /// @brief
    ///    �������IEC81850-9-2���ƿ��ѡ��״̬
    ///
    void UnSelectAllIEC9_2Cb();

    ///
    /// @brief
    ///    ��ñ�ѡ�е�IEC81850-9-2���ƿ����
    ///
    /// @return
    ///     int    ��ѡ�еĿ��ƿ����
    ///
    int GetSelectedIEC9_2CbCount();

private:
    ///
    /// @brief
    ///    ��scl�ļ����з�������ȡscl����Ҫ����Ϣ
    ///
    /// @param[in]  strFileName     Ҫ������scl�ļ���
    ///
    /// @return
    ///     true ��ʶscl��Ϣ������ȡ�ɹ�
    ///
    bool __Analysis(std::wstring strFileName);

    ///
    /// @brief
    ///    ����GOOSE���ƿ�
    ///
    /// @param[in]  pSclCacheMgr scl����������ڼ���ж��scl�ļ�
    ///
    /// @return
    ///     bool    true��ʾ���سɹ�
    ///
    bool __LoadGooseCb(ISCLCacheManager* pSclCacheMgr);

    ///
    /// @brief
    ///    ����GOOSE���ƿ��ͨ��
    ///
    /// @param[in]  pGooseAnalysis ָ��GOOSE���ý����ӿڶ����ָ��
    /// @param[in]  pGooseInfo ָ��GOOSE���ƿ���Ϣ�ṹ���ָ��
    /// @param[in]  pGooseCb ָ��GOOSE���ƿ�����ָ��
    ///
    /// @return
    ///     bool    true��ʾ���سɹ�
    ///
    bool __LoadGooseChannel(ISCDGOOSEAnalysis* pGooseAnalysis, SCDGOOSEANALYSIS_IEDGOOSE_INFO* pGooseInfo, CGooseCb* pGooseCb);

    ///
    /// @brief
    ///     ����GOOSE���ƿ����ͨ��
    ///
    /// @param[in]
    ///     pGooseAnalysis  -   ָ��GOOSE���ý����ӿڶ����ָ��
    /// @param[in]
    ///     pGooseInfo      -   ָ��GOOSE���ƿ���Ϣ�ṹ���ָ��
    /// @param[in]  
    ///     pGooseCb        -   ָ��GOOSE���ƿ�����ָ��
    /// @param[in]  
    ///     stChannelInfo   -   IED��GOOSE���ݼ���Ϣ(����ͨ��)
    /// @param[in]  
    ///     pChannel        -   ָ��GOOSEͨ����ָ��
    ///
    /// @return
    ///     bool    true��ʾ���سɹ�
    ///
    bool __LoadGooseSubChannel(
        ISCDGOOSEAnalysis*                          pGooseAnalysis, 
        SCDGOOSEANALYSIS_IEDGOOSE_INFO*             pGooseInfo, 
        CGooseCb*                                   pGooseCb, 
        SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO&     stChannelInfo,
        CChannel*                                   pChannel
        );

    ///
    /// @brief
    ///    ����IEC61850-9-2���ƿ�
    ///
    /// @param[in]  pSclCacheMgr scl����������ڼ���ж��scl�ļ�
    ///
    /// @return
    ///     bool    true��ʾ���سɹ�
    ///
    bool __LoadIEC9_2Cb(ISCLCacheManager* pSclCacheMgr);

    ///
    /// @brief
    ///    ����IEC61850-9-2���ƿ��ͨ��
    ///
    /// @param[in]  pSmvAnalysis ָ��SCD�ļ�SMV92���ý����ӿڵ�ָ��
    /// @param[in]  pSmvInfo ָ��SMV92���ƿ���Ϣ�Ľṹ���ָ��
    /// @param[in]  pIEC9_2Cb ָ��IEC61850-9-2���ƿ�����ָ��
    ///
    /// @return
    ///     bool    true��ʾ���سɹ�
    ///
    bool __LoadIEC9_2Channel(ISCDSMV92Analysis* pSmvAnalysis, SCDSMV92ANALYSIS_IEDSMV_INFO* pSmvInfo, CIEC9_2Cb* pIEC9_2Cb);

    ///
    /// @brief
    ///    ���map�������е���Ϣ
    ///
    void __ClearAllMap();

    ///
    /// @brief
    ///    ��ȡͨ���������Ϣ
    ///
    /// @param[in] stSmvChannelInfo ����ֵ���ݼ��е�����ͨ����Ϣ
    ///
    /// @return
    ///     ���ض�Ӧ��ͨ����� A/B/C��
    ///
    ChannelPhase __GetChannelPhase(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo);

    ///
    /// @brief
    ///    ��ȡͨ����������Ϣ
    ///
    /// @param[in] stSmvChannelInfo ����ֵ���ݼ��е�����ͨ����Ϣ
    ///
    /// @return
    ///     ���ض�Ӧ��ͨ��������Ϣ ʱ��/��ѹ/����...
    ///
    enumChannelType __GetChannelType(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo);

    ///
    /// @brief
    ///    ��ȡͨ����������Ϣ
    ///
    /// @param[in] stGooseChannelInfo goose���ݼ��е�����ͨ����Ϣ
    ///
    /// @return
    ///     ���ض�Ӧ��ͨ��������Ϣ ����/˫��/ʱ��...
    ///
    enumChannelType __GetChannelType(const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO& stGooseChannelInfo);

    ///
    /// @brief
    ///    ����ͨ���������ж�ͨ���Ƿ��ǲ�������
    ///
    /// @param[in]  eChannelType    ͨ������
    /// @param[in]  strDesc    ͨ������
    ///
    /// @return
    ///     bool    true��ʾ�ǲ�������
    ///
    bool __GetMeasure(enumChannelType eChannelType, std::wstring strDesc);

private:
    bool                                    m_bSclLoaded;               ///< �Ƿ��ѳɹ�������һ��SCD�ļ�

    std::map<uint32_t, CIEC9_2Cb*>          m_mapIEC9_2CbGroup;         ///< ����smv���ƿ���Ϣ��map
    std::map<uint32_t, CGooseCb*>           m_mapGooseCbGroup;          ///< ����goose���ƿ���Ϣ��map
};

#endif //__SCL_MANAGER_H__