/**
  \file SCDSMV92Analysis.h

  \brief SCD�ļ���IEC61850-9-2������Ϣ����

  \note
      ����Mac��AppID��ȡ���ƿ鼰ͨ������Ϣ��Ϣ�Ľӿ�ֻ������AppID���ı�����
      ���ݵ�ַ������ȡ���ƿ鼰ͨ������Ϣ��Ϣ�Ľӿ�ֻ������iedName��ldInst��cbName��dataSetName���ı�����

  \copyright 

  \author  chao

  \version 1.0.x.x

  \note ���ļ�¼
      �� ʱ��         : �汾      :  ��ϸ��Ϣ    
      :-------------- :-----------:----------------------------------------------------------
      |2013.01.18     |1.0.1.118  |���SMV�������ƿ���Ϣ                                    |
	  |2013.01.20     |1.0.1.120  |���ַ���������string����Ϊwstring �ṩ�Զ����Ե�֧��     |
	  |2013.01.28     |1.0.2.128  |����IED��SMV��Ϣ�ṹ�����ͽӿ����ע��                   |
      |2013.02.18     |1.0.3.218  |��Ӳ���ֵ����������ߵ���Ϣ��SMV INPUT���Ľ���          |
	  |2013.05.27     |1.0.4.527  |������\ж�ؽӿ�����ISCLCacheManager��                    |
	  |2013.06.01     |1.0.4.620  |SMV���ƿ���Ϣ��������������ݼ��ĸ�Ҫ��Ϣ                |
	  |2013.06.20     |1.0.4.620  |��Ӱ���ַ������ȡ���ƿ顢ͨ����Ϣ�Ľӿ�                 |
	  |2013.06.20     |1.0.4.620  |��Ӹ���ͨ����Ϣ�ж�ͨ�����͡����Ľӿ�                 |
	  |2013.06.20     |1.0.4.620  |����޸Ŀ��ƿ顢ͨ����Ϣ�Ľӿ�                           |
	  |2013.07.25     |1.0.5.725  |�����Ƴ�ָ��ͨ��ʱ����ͨ��֮���ͨ��˳���û�и��µ�Bug  |
	  |2013.07.25     |1.0.5.725  |������ΪMap��Key�Ĺؼ���Ϣ�����ı�ʱ��ֻ���¶�Ӧ��valueֵȴû����Keyֵ��Bug|
	  |2013.08.21     |1.0.5.821  |�޸�SMVͨ����Ϣ�����ýӿڣ�˳��ų�����Χ���Զ���ӣ���ͨ����Ϣ�н�������Ϣ��Ч��������Ϣ������Ч|
	  |2013.08.21     |1.0.5.821  |���SMVͨ����Ϣ����ӽӿڣ���������Ϣ��Ч��������Ϣ������Ч|
 */
#ifndef _SCDSMV92ANALYSIS_H_
#define _SCDSMV92ANALYSIS_H_

#include <string>
#include "sclanalysis.h"

/**
  \brief �Զ�������(BOOL)
 */
typedef int SMV92_BOOLEAN;

#define SMV92_TRUE  SMV92_BOOLEAN(1)

#define SMV92_FALSE SMV92_BOOLEAN(0) 

/**
  \brief SCD�����ļ���MAC��ַ��С(�ֽ�)
 */
#define SCDSMV92ANALYSIS_MAC_SIZE  (6)

///
/// @brief
///    ����SMVͨ��������Ϣ
///
typedef enum{
	CT_SMV_UNKNOW = 0,             ///< ͨ������δ֪
	CT_SMV_TIME,                   ///< ͨ������Ϊʱ��
	CT_SMV_CURRENT,                ///< ͨ������Ϊ����
	CT_SMV_VOLTAGE                 ///< ͨ������Ϊ��ѹ
}CHANNEL_TYPE_SMV;

///
/// @brief
///    ����ͨ�������Ϣ
///
typedef enum{
	CP_SMV_PHASE_UNKNOW = 0,       ///< ͨ�����δ֪
	CP_SMV_PHASE_A,                ///< ͨ�����ΪA��
	CP_SMV_PHASE_B,                ///< ͨ�����ΪB��
	CP_SMV_PHASE_C,                ///< ͨ�����ΪC��
	CP_SMV_PHASE_N,                ///< ͨ�����ΪN�࣬���Ե�/����
	CP_SMV_PHASE_X                 ///< ͨ�����ΪX�࣬ͬ��/ĸ��/��ȡ
}CHANNEL_PHASE_SMV;

#pragma region // �������Ͷ���

/**
  \brief IED��SMV���ƿ���Ϣ(ʹ��smvMAC��smvAppID�ɱ�ʶ����Ϣ��Ωһ��)
 */
typedef struct  
{
    std::wstring    iedName;                           /**< IED����(����SMV������IED����) */
    std::wstring    iedDesc;                           /**< IED���� */
    std::wstring    iedAPName;                         /**< IED��AccessPoint���� 
                                                           (����SMV������IED��AccessPoint)*/
    std::wstring    iedAPDesc;                         /**< IED��AccessPoint���� */
    std::wstring    iedLdInst;                         /**< IED�߼��豸ʵ������ 
                                                           (����SMV������IED��LDevie) */
    std::wstring    iedLdDesc;                         /**< IED��LDevice���� */

	std::wstring    dataSetName;                       /**< SMV���ƿ����������ݼ������� */
	std::wstring    dataSetDesc;                       /**< SMV���ƿ����������ݼ������� */
	int             channelNum;                        /**< SMV���ƿ����������ݼ���ͨ���� */

    unsigned char   smvMAC[SCDSMV92ANALYSIS_MAC_SIZE]; /**< SMVͨ���鲥MAC��ַ */
    unsigned short  smvAppID;                          /**< SMVͨ��APPID(��׼Ҫ��ȫվΩһ) */
    unsigned short  smvVlanID;                         /**< SMVͨ��VLAN-ID */
    unsigned char   smvVlanPri;                        /**< SMVͨ��VLAN-PRIORITY */
    std::wstring    smvDesc;                           /**< SMVͨ������ */
    std::wstring    smvCbName;                         /**< SMV�������ƿ����� */
    unsigned int    smvCbConfRev;                      /**< SMV�������ƿ����ð汾 */
    SMV92_BOOLEAN   smvCbMulticast;                    /**< SMV����ֵ�Ƿ�㲥 */
    std::wstring    smvCbDataSet;                      /**< SMV�������ݼ�����(�������ݼ�����) */
    unsigned int    smvCbNofASDU;                      /**< SMV�������ݰ���ASDU�ĸ��� */
    unsigned int    smvCbSmpRate;                      /**< SMV������ */
    std::wstring    smvCbSmvID;                        /**< SMV����ID */
    SMV92_BOOLEAN   smvCbOptRefreshTime;               /**< SMV����ˢ��ʱ���ѡ������ */
    SMV92_BOOLEAN   smvCbOptSmpRate;                   /**< SMV�����ʿ�ѡ������ */
	SMV92_BOOLEAN   smvCbOptDataRef;                   /**< SMV���ݼ�������ѡ������ */
    SMV92_BOOLEAN   smvCbOptSmpSync;                   /**< SMV����ͬ����ѡ������ */
    SMV92_BOOLEAN   smvCbOptSecurity;                  /**< SMV������ȫ��ѡ������ */

}SCDSMV92ANALYSIS_IEDSMV_INFO;

/**
  \brief IED��SMV���ݼ���Ϣ������ͨ����
 */
typedef struct  
{
    int idxSeq;                /**< ���ݼ���Ϣ��DataSet�ж����˳��� */
    std::wstring  dataSetName; /**< �������ݼ������� */
    std::wstring  dataSetDesc; /**< �������ݼ������� */
    std::wstring  ldInst;      /**< �߼��豸ʵ�� */
    std::wstring  lnPrefix;    /**< �߼��ڵ�ǰ׺ */
    std::wstring  lnClass;     /**< �߼��ڵ����� */
    std::wstring  lnInst;      /**< �߼��ڵ�ʵ�� */
	std::wstring  lnDesc;      /**< �߼��ڵ����� */
    std::wstring  doName;      /**< ���ݶ������� */
    std::wstring  doDesc;      /**< ���ݶ������� */
    std::wstring  daFc;        /**< �������Թ���Լ�� */
    std::wstring  daName;      /**< ������������ */
    std::wstring  dasAddr;     /**< �������Ե�ַ��Ϣ */
    std::wstring  daVal;       /**< ��������Ĭ��ֵ */

}SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO;

/**
\brief IED��SMV�������֮�������ⲿ�źŵ���Ϣ
*/
typedef struct
{
	int           idxSeq_Out;                          /**< �ⲿ�ź���Inputs�е�˳��ţ���0��ʼ */
	std::wstring  iedName_Out;                         /**< �ⲿ�ź����ڵ�IED������ */
	std::wstring  ldInst_Out;                          /**< �ⲿ�ź������߼��豸��ʵ���� */
	std::wstring  lnName_Out;                          /**< �ⲿ�ź������߼��ڵ������ */
	std::wstring  doName_Out;                          /**< �ⲿ�ź�����ʶ��DO������ */
	std::wstring  daName_Out;                          /**< �ⲿ�ź�����ʶ��DA�����ơ���ֵΪ��ʱ����ʾ�ⲿ�ź�ΪDO�������������ԡ� */
	std::wstring  lnDesc_Out;                          /**< �ⲿ�ź����ڵ�LN������ */
	std::wstring  doDesc_Out;                          /**< �ⲿ�ź����ڵ�DO������ */
	std::wstring  dUDesc_Out;                          /**< �ⲿ�ź����ڵ�DO��dU�������Ե�Val */
	int           idxSeq_In;                           /**< �ⲿ�ź���Inputs�е�˳��ţ���0��ʼ */
	std::wstring  iedName_In;                          /**< �ⲿ�ź����󶨵��ڲ���ַ��IED������ */
	std::wstring  ldInst_In;                           /**< �ⲿ�ź����󶨵��ڲ���ַ���߼��豸��ʵ���� */
	std::wstring  lnName_In;                           /**< �ⲿ�ź����󶨵��ڲ���ַ���߼��ڵ������ */
	std::wstring  doName_In;                           /**< �ⲿ�ź����󶨵��ڲ���ַ����ʶ��DO������ */
	std::wstring  daName_In;                           /**< �ⲿ�ź����󶨵��ڲ���ַ����ʶ��DA�����ơ���ֵΪ��ʱ����ʾ�ⲿ�ź�ΪDO�������������ԡ� */
	std::wstring  lnDesc_In;                           /**< �ⲿ�ź����󶨵��ڲ���ַ��LN������ */
	std::wstring  doDesc_In;                           /**< �ⲿ�ź����󶨵��ڲ���ַ��DO������ */
	std::wstring  dUDesc_In;                           /**< �ⲿ�ź����󶨵��ڲ���ַ��DO��dU�������Ե�Val */
}SCDSMV92ANALYSIS_IEDSMV_INPUTS_INFO;

#pragma endregion

/**
  \brief SCD�ļ�SMV92���ý����ӿ�
 */
class ISCDSMV92Analysis
{
public:
    
	ISCDSMV92Analysis();
    virtual ~ISCDSMV92Analysis();

	///
	/// @brief
	///     ��ȡ�����ļ������е�SMV���ƿ��������������δ����ͨ�Ų����Ŀ��ƿ飩
	///
	/// @return
	///     SMV���ƿ������ - �����ɹ��� -1 - ����ʧ��
	///
    virtual int GetIEDSMVNum( void ) = 0;

	///
	/// @brief
	///     ��ȡ�����ļ������е�SMV���ƿ���Ϣ��������δ����ͨ�Ų����Ŀ��ƿ飩
	///
	/// @note
	///     SCL�ļ���ĳ���ƿ���Ϣ���������ÿ��ƿ���Ϣ���践��
	///
	/// @param[in]
	///     iedSMVInfoArr - ����SMV���ƿ���Ϣ������
	/// @param[in]
	///     arrSize       - ���ݵ��������
	///
	/// @return
	///     �����б�����Ϣ�������������ɹ���  -1������ʧ��
	///
    virtual int GetAllIEDSMVInfo( 
                 SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfoArr, 
                 const int arrSize ) = 0;

	///
	/// @brief
	///     ��ȡָ��MAC��APPID��SMV���ƿ���Ϣ
	///
	/// @note
	///     ���ڶ����ͬAppID�Ŀ��ƿ�ʱ��Ĭ��ѡ���һ��
	///
	/// @param[in]
	///     smvMAC     - SMVͨ���鲥MAC��ַ
	/// @param[in]
	///     smvAPPID   - SMVͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
	/// @param[in]
	///     iedSMVInfo - ����SMV���ƿ���Ϣ�Ľṹ��
	///
	/// @return
	///     0�������ɹ���  -1��ʧ��
	///
    virtual int GetOneIEDSMVInfo( 
                const unsigned char *smvMAC, 
                const unsigned short smvAppID,
                SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfo ) = 0;

	///
	/// @brief
	///     ��ȡָ��MAC��APPID��ص�SMV���ƿ������Ƶ�DataSet��ͨ����FCDA��������
	///
	/// @note
	///     ���ڶ����ͬAppID�Ŀ��ƿ�ʱ��Ĭ��ѡ���һ��
	///
	/// @param[in]
	///     smvMAC     - SMVͨ���鲥MAC��ַ
	/// @param[in]
	///     smvAPPID   - SMVͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
	///
	/// @return
	///     DataSet��FCDA�������������ɹ���  -1��ʧ��
	///
    virtual int GetIEDSMVDataSetNum( 
                    const unsigned char *smvMAC, 
                    const unsigned short smvAppID )= 0;

	///
	/// @brief
	///     ��ȡָ��MAC��APPID��ص�SMV���ƿ������Ƶ�DataSet������ͨ����FCDA����Ϣ
	///
	/// @note
	///     ���ڶ����ͬAppID�Ŀ��ƿ�ʱ��Ĭ��ѡ���һ��
	///     SCL�ļ���ĳͨ����Ϣ����������ͨ����Ϣ���践��
	///
	/// @param[in]
	///     smvMAC         - SMVͨ���鲥MAC��ַ
	/// @param[in]
	///     smvAPPID       - SMVͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
	/// @param[in]
	///     dataSetInfoArr - ����SMV��ͨ����Ϣ������
	/// @param[in]
	///     arrSize        - ������������
	///
	/// @return
	///     ���鱣��ͨ����Ϣ�������������ɹ���  -1��ʧ��
	///
    virtual int GetAllIEDSMVDataSetInfo(
                    const unsigned char *smvMAC, 
                    const unsigned short smvAppID,
                    SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfoArr,
                    const int arrSize ) = 0;

	///
	/// @brief
	///     ��ȡָ��MAC��APPID��ص�SMV���ƿ������Ƶ�DataSet��ĳһ˳��ŵ�ͨ����FCDA����Ϣ
	///
	/// @note
	///     ���ڶ����ͬAppID�Ŀ��ƿ�ʱ��Ĭ��ѡ���һ��
	///
	/// @param[in]
	///     smvMAC      - SMVͨ���鲥MAC��ַ
	/// @param[in]
	///     smvAPPID    - SMVͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
	/// @param[in]
	///     idxSeq      - ͨ����˳���(��0��ʼ)
	/// @param[in]
	///     dataSetInfo - ͨ����Ϣ
	///
	/// @return
	///     0�������ɹ���  -1��ʧ��
	///
    virtual int GetOneIEDSMVDataSetInfo(
                    const unsigned char *smvMAC, 
                    const unsigned short smvAppID,
                    const int            idxSeq,
                    SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo ) = 0;

	///
	/// @brief
	///     ��ȡָ��MAC��APPID��ص�SMV���ƿ������Ƶ�DataSet��ĳһͨ����FCDA����˳���
	///
	/// @note
	///     ���ڶ����ͬAppID�Ŀ��ƿ�ʱ��Ĭ��ѡ���һ��
	///
	/// @param[in]
	///     smvMAC      - SMVͨ���鲥MAC��ַ
	/// @param[in]
	///     smvAPPID    - SMVͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
	/// @param[in]
	///     dataSetInfo - ����ͨ����Ϣ�Ľṹ��
	///
	/// @return
	///     ͨ����˳��ţ������ɹ���  -1��ʧ��
	///
    virtual int GetOneIEDSMVDataSetIdxSeq(
                    const unsigned char *smvMAC, 
                    const unsigned short smvAppID,
                    const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo ) = 0;

	///
	/// @brief
	///     ��ȡĳһIED�����󶨵��ⲿ�źţ�ExtRef����SMV�����źţ�������
	///
	/// @note
	///     ÿ��װ�õ�LLN0���ֺ��и�װ�õ�GOOSE�Ͳ���ֵ�����������Ϣ
	///
	/// @param[in]
	///     iedName      - IED������
	///
	/// @return
	///     ExtRef���������ɹ���  -1��ʧ��
	///
	virtual int GetOneIEDInputsExtRefNum(
		const std::wstring &iedName) = 0;

	///
	/// @brief
	///     ��ȡָ��IED�����󶨵�ȫ���ⲿ�źŵ�������Ϣ
	///
	/// @param[in]
	///     iedName    - IED����
	/// @param[in]
	///     inputsInfo - �����ⲿ�źŵ�������Ϣ������
	/// @param[in]
	///     arrSize    - �����ⲿ�źŵ�������Ϣ������Ĵ�С
	///
	/// @return
	///     �����б������Ϣ������ - �����ɹ��� -1 - ����ʧ��
	///
	virtual int GetOneIEDSVINExtRef(
		const std::wstring &iedName,
		SCDSMV92ANALYSIS_IEDSMV_INPUTS_INFO *inputsInfo,
		const int arrSize) = 0;

	///
	/// @brief
	///     ��ȡ����ĳһ�ⲿ�ź������SMV���ƿ����Ϣ
	///
	/// @param[in]
	///     inputsInfo - �ⲿ�źŵ�������Ϣ
	/// @param[in]
	///     iedSMVInfo - ���Ƹ��ⲿ�ź������SMV���ƿ����Ϣ
	///
	/// @return
	///     0 - �����ɹ��� -1 - ����ʧ��
	///
	virtual int GetOutAddrSMVInfo(
		const SCDSMV92ANALYSIS_IEDSMV_INPUTS_INFO &inputsInfo,
		SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfo) = 0;

	///
	/// @brief
	///     ��ȡȫ����SMV���ƿ����Ŀ������δ����ͨ�Ų����Ŀ��ƿ飩
	///
	/// @return
	///     ���ƿ����Ŀ�������ɹ���  -1��ʧ��
	///
	virtual int GetAllIEDSMVNumByCtrlRef( void ) = 0;

	///
	/// @brief
	///     ��ȡȫ����SMV���ƿ����Ϣ������δ����ͨ�Ų����Ŀ��ƿ飩��ͨ�����ƿ��������б�������
	///
	/// @note
	///     SCL�ļ���ĳ���ƿ���Ϣ���������ÿ��ƿ���Ϣ���践��
	///     δ���ò��ֽ���Ĭ������
	///
	/// @param[in]
	///     iedSMVInfoArr - ����SMV���ƿ���Ϣ������
	/// @param[in]
	///     arrSize       - �����С
	///
	/// @return
	///     �����б���Ŀ��ƿ�������������ɹ���  -1��ʧ��
	///
	virtual int GetAllIEDSMVInfoByCtrlRef(
		SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfoArr, 
		const int arrSize ) = 0;

	///
	/// @brief
	///     ����һ��SMV���ƿ飬ͨ�����ƿ��������б�������
	///
	/// @param[in]
	///     iedSMVInfo - SMV���ƿ���Ϣ
	///
	/// @return
	///     0�������ɹ���  -1��ʧ��
	///
	virtual int SetOneIEDSMVInfoByCtrlRef(const SCDSMV92ANALYSIS_IEDSMV_INFO &iedSMVInfo) = 0;

	///
	/// @brief
	///     ��ȡָ��������ַ��SMV���ƿ������Ƶ�DataSet��ͨ����FCDA��������
	///
	/// @param[in]
	///     iedName        - SMV���ƿ����ڵ�IED������
	/// @param[in]
	///     ldInst         - SMV���ƿ����ڵ�LDevice��Inst��
	/// @param[in]
	///     cbName         - SMV���ƿ������
	///
	/// @return
	///     DataSet��FCDA�������������ɹ���  -1��ʧ��
	///
	virtual int GetIEDSMVDataSetNum(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName) = 0;

	///
	/// @brief
	///     ��ȡָ��������ַ��SMV���ƿ������Ƶ�DataSet������ͨ����FCDA����Ϣ
	///
	/// @param[in]
	///     iedName        - SMV���ƿ����ڵ�IED������
	/// @param[in]
	///     ldInst         - SMV���ƿ����ڵ�LDevice��Inst��
	/// @param[in]
	///     cbName         - SMV���ƿ������
	/// @param[in]
	///     dataSetInfoArr - ����SMV��ͨ����Ϣ������
	/// @param[in]
	///     arrSize        - ������������
	///
	/// @return
	///     ���鱣��ͨ����Ϣ�������������ɹ���  -1��ʧ��
	///
	virtual int GetAllIEDSMVDataSetInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfoArr,
		const int arrSize ) = 0;

	///
	/// @brief
	///     ��ȡָ��������ַ��SMV���ƿ������Ƶ�DataSet��ĳһ˳��ŵ�ͨ����FCDA����Ϣ
	///
	/// @param[in]
	///     iedName        - SMV���ƿ����ڵ�IED������
	/// @param[in]
	///     ldInst         - SMV���ƿ����ڵ�LDevice��Inst��
	/// @param[in]
	///     cbName         - SMV���ƿ������
	/// @param[in]
	///     idxSeq         - SMVͨ����˳���
	/// @param[in]
	///     dataSetInfo    - ָ�򱣴�SMV��ͨ����Ϣ�Ľṹ���ָ��
	///
	/// @return
	///     0�������ɹ���  -1��ʧ��
	///
	virtual int GetOneIEDSMVDataSetInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName,
		const int idxSeq,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo) = 0;

	///
	/// @brief
	///     ����SMV���ƿ����������ݼ��е�һ��ͨ����FCDA��
	///
	/// @note
	///     ��ͨ����Ϣ�е�lnPrefix��lnClass��lnInst��doName��daName��daFc��daBType��dUsAddr���Ե�������Ч
	///
	/// @param[in]
	///     iedName      - ��ͨ�����ڵ�IED������
	/// @param[in]
	///     ldInst       - ��ͨ�����ڵ�LDevice��Inst��
	/// @param[in]
	///     dataSetInfo  - ��ͨ����������������Ϣ
	/// @param[in]
	///     eChannelType - ��ͨ��������
	///
	/// @return
	///     0�������ɹ���  -1��ʧ��
	///
	virtual int SetOneIEDSMVDataSetInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &dataSetInfo,
		CHANNEL_TYPE_SMV eChannelType ) = 0;

	///
	/// @brief
	///     ��SMV���ƿ����������ݼ�ĩβ���һ��ͨ����FCDA��
	///
	/// @note
	///     ��ͨ����Ϣ�е�lnPrefix��lnClass��lnInst��doName��daName��daFc��daBType��dUsAddr���Ե�������Ч
	///
	/// @param[in]
	///     iedName      - ��ͨ�����ڵ�IED������
	/// @param[in]
	///     ldInst       - ��ͨ�����ڵ�LDevice��Inst��
	/// @param[in]
	///     dataSetInfo  - ��ͨ����������������Ϣ
	/// @param[in]
	///     eChannelType - ��ͨ��������
	///
	/// @return
	///     0�������ɹ���  -1��ʧ��
	///
	virtual int AddOneIEDSMVDataSetInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &dataSetInfo,
		CHANNEL_TYPE_SMV eChannelType ) = 0;

	///
	/// @brief
	///     ��SMVͨ������ĩβ������ͨ������ĳһͨ������Ϣ����n��
	///
	/// @param[in]
	///     iedName      - ͨ�����ڵ�IED������
	/// @param[in]
	///     ldInst       - ͨ�����ڵ�LDevice��Inst��
	/// @param[in]
	///     dataSetName  - ͨ�����ڵ����ݼ�������
	/// @param[in]
	///     idxSeq       - Ҫ������ͨ����˳��ţ���0��ʼ��
	/// @param[in]
	///     iCopyCount   - ��������Ŀ
	///
	/// @return
	///     0�������ɹ���  -1��ʧ��
	///
	virtual int CopyIEDSMVDataSetInfoToSetEnd(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &dataSetName,
		int idxSeq, int iCopyCount ) = 0;

	///
	/// @brief
	///     �Ƴ�SMV���ƿ����������ݼ��е�һ��ͨ��
	///
	/// @param[in]
	///     iedName    - ��ͨ����FCDA���Ķ�����Ϣ���ڵ�IED������
	/// @param[in]
	///     ldInst     - ��ͨ����FCDA���Ķ�����Ϣ���ڵ�LDevice��Inst��
	/// @param[in]
	///     datSetName - ��ͨ����FCDA���Ķ�����Ϣ���ڵ�DataSet������
	/// @param[in]
	///     idxSeq     - ��ͨ����FCDA����˳���
	///
	/// @return
	///     0�������ɹ���  -1��ʧ��
	///
	virtual int RemoveOneIEDSMVDataSetInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &datSetName,
		const int idxSeq) = 0;

	///
	/// @brief
	///     �ж�ĳAppID�Ŀ��ƿ��Ƿ����
	///
	/// @param[in]
	///     appID - ���ƿ��AppIDֵ
	///
	/// @return
	///     0 - ���ڣ� -1 - ������
	///
	virtual int IsAppIDExisted(const unsigned short appID) = 0;
	
	///
	/// @brief
	///     ��ȡͨ����������Ϣ
	///
	/// @param[in]
	///     stSmvChannelInfo - ����ֵ���ݼ��е�����ͨ����Ϣ
	///
	/// @return
	///     ���ض�Ӧ��ͨ��������Ϣ ʱ��/��ѹ/����...
	///
	virtual CHANNEL_TYPE_SMV GetChannelType(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo) = 0;

	///
	/// @brief
	///     ��ȡͨ���������Ϣ
	///
	/// @param[in]
	///     stSmvChannelInfo - ����ֵ���ݼ��е�����ͨ����Ϣ
	/// @param[in]
	///     eChannelType     - ͨ������
	///
	/// @return
	///     ���ض�Ӧ��ͨ����� A/B/C��
	///
	virtual CHANNEL_PHASE_SMV GetChannelPhase(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo, CHANNEL_TYPE_SMV eChannelType) = 0;

};

class ISCLCacheManager;

///
/// @brief
///     SCD�ļ�SMV92���ý����ӿڽӿڶ���ʵ��
///
/// @param[in]
///     pSclCacheMgr - ָ��Scl�����������ָ��
///
/// @return
///     SCD�ļ�SMV92���ý����ӿ�
///
SCLANALYSISDLL_API ISCDSMV92Analysis *CreateSCDSMV92AnalysisImpl( ISCLCacheManager *pSclCacheMgr );

///
/// @brief
///     �ͷ�SCD�ļ�SMV92���ý����ӿڶ���ʵ��
///
/// @param[in]
///     objPtr - ָ��ӿڶ���ʵ����ָ��
///
SCLANALYSISDLL_API void  RelaseSCDSMV92AnalysisImpl(ISCDSMV92Analysis *objPtr);

#endif
