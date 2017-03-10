/// @file
///     SCDGOOSEAnalysis.h
///
/// @brief
///     SCD�ļ���GOOSE������Ϣ����
///
/// @note
///     ����Mac��AppID��ȡ���ƿ鼰ͨ������Ϣ��Ϣ�Ľӿ�ֻ������AppID���ı�����
///     ���ݵ�ַ������ȡ���ƿ鼰ͨ������Ϣ��Ϣ�Ľӿ�ֻ������iedName��ldInst��cbName��dataSetName���ı�����
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.2.4
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.02.18     |1.0.3.218  |���GOOSE����������ߵ���Ϣ��GOOSE INPUT���Ľ���          |
///    |2013.05.27     |1.0.4.527  |������\ж�ؽӿ�����ISCLCacheManager��                     |
///    |2013.06.01     |1.0.4.601  |GOOSE���ƿ���Ϣ��������������ݼ��ĸ�Ҫ��Ϣ               |
///    |2013.06.01     |1.0.4.601  |�������ͨ������ͨ����Ϣ�Ļ�ȡ�ӿ�                        |
///    |2013.06.20     |1.0.4.620  |��Ӱ���ַ������ȡ���ƿ顢ͨ������ͨ����Ϣ�Ľӿ�          |
///    |2013.06.20     |1.0.4.620  |��Ӹ���ͨ����Ϣ�ж�ͨ�����͵Ľӿ�                        |
///    |2013.06.20     |1.0.4.620  |����޸Ŀ��ƿ顢ͨ������ͨ����Ϣ�Ľӿ�                    |
///    |2013.07.25     |1.0.5.725  |�����Ƴ�ָ��ͨ��������ͨ��ʱ����ͨ��֮���ͨ��˳���û�и��µ�Bug|
///    |2013.07.25     |1.0.5.725  |������ΪMap��Key�Ĺؼ���Ϣ�����ı�ʱ��ֻ���¶�Ӧ��valueֵȴû����Keyֵ��Bug|
///    |2013.08.07     |1.0.5.807  |��ȡGOOSEͨ����Ϣʱ����Ӷ�ͨ����DOI��Ϣ������ʱ�Ĵ���    |
///    |2013.08.08     |1.0.5.808  |�޸�ĳ�߼��ڵ��Ƿ�ΪLLN0���ж�����                        |
///    |2013.08.13     |1.0.5.813  |������AppID��MacΪ������ȡȫ�����ƿ���Ϣ�Ҳ���AppID��Ӧ�Ŀ��ƿ�û�ж���ʱ��Ϣ��ȡʧ�ܵ�Bug|
///    |2013.08.15     |1.0.5.815  |�޸�Gooseͨ������ͨ����Ϣ�Ĳ�������ȡ�����á��Ƴ���ʵ�֣�ʹ��֧��ͨ��ΪDAģʽ��Ϊ����Ϊ�ṹ������|
///    |2013.08.16     |1.0.5.816  |�޸�Goose��ͨ����Ϣ�Ŀ������Ƴ�����ʹ���Զ������ͨ������ֹ֮�������Ϣ����Ӱ������ͨ������Ϣ|
///    |2013.08.21     |1.0.5.821  |���Gooseͨ����Ϣ����ӽӿڣ���������Ϣ��Ч��������Ϣ������Ч|
///
#ifndef _SCD_GOOSE_ANALYSIS_H_
#define _SCD_GOOSE_ANALYSIS_H_

#include <string>
#include "sclanalysis.h"

///
/// @brief
///     �Զ�������(BOOL)
///
typedef int GOOSE_BOOL;

#define GOOSE_TRUE  GOOSE_BOOL(1)

#define GOOSE_FALSE GOOSE_BOOL(0) 

///
/// @brief
///     SCD�����ļ���MAC��ַ��С(�ֽ�)
///
#define GOOSEANALYSIS_MAC_SIZE  (6)

///
/// @brief
///    ����GOOSEͨ��������Ϣ
///
typedef enum{
	CT_GOOSE_UNKNOW = 0,           ///< ͨ������δ֪
	CT_GOOSE_STRUCT,               ///< �ṹ��
	CT_GOOSE_POINT,                ///< ����
	CT_GOOSE_DOUBLE_POINT,         ///< ˫��
	CT_GOOSE_QUALITY,              ///< Ʒ��
	CT_GOOSE_TIME_STAMP,           ///< ʱ���ǩ
	CT_GOOSE_ENTRY_TIME,           ///< ��Ŀʱ��
	CT_GOOSE_INT8,                 ///< ���ͣ�INT8��
	CT_GOOSE_INT16,                ///< ���ͣ�INT16��
	CT_GOOSE_INT24,                ///< ���ͣ�INT24��
	CT_GOOSE_INT32,                ///< ���ͣ�INT32��
	CT_GOOSE_INT128,               ///< ���ͣ�INT128��
	CT_GOOSE_UINT8,                ///< ���ͣ�UINT8��
	CT_GOOSE_UINT16,               ///< ���ͣ�UINT16��
	CT_GOOSE_UINT24,               ///< ���ͣ�UINT24��
	CT_GOOSE_UINT32,               ///< ���ͣ�UINT32��
	CT_GOOSE_FLOAT32,              ///< ������
	CT_GOOSE_FLOAT64,              ///< ˫����
	CT_GOOSE_OCTET,                ///< ��λλ���ַ���
	CT_GOOSE_VISSTRING,            ///< �����ַ���
	CT_GOOSE_UNICODE               ///< UNICODE�ַ���
}CHANNEL_TYPE_GOOSE;

#pragma region // �������Ͷ���

///
/// @brief
///     IED��GOOSE���ƿ���Ϣ(ʹ��gooseMAC��gooseAppID�ɱ�ʶ����Ϣ��Ωһ��)
///
typedef struct  
{
	std::wstring    iedName;                          ///< GOOSE���ƿ����ڵ�IED������
	std::wstring    iedDesc;                          ///< IED����

	std::wstring    iedAPName;                        ///< GOOSE���ƿ����ڵ�AccessPoint���� 
	std::wstring    iedAPDesc;                        ///< IED��AccessPoint����

	std::wstring    iedLdInst;                        ///< GOOSE���ƿ����ڵ��߼��豸��ʵ����
	std::wstring    iedLdDesc;                        ///< IED��LDevice����

	std::wstring    dataSetName;                      ///< GOOSE���ƿ����������ݼ�������
	std::wstring    dataSetDesc;                      ///< GOOSE���ƿ����������ݼ�������
	int             channelNum;                       ///< GOOSE���ƿ����������ݼ���ͨ����

	unsigned char   gooseMAC[GOOSEANALYSIS_MAC_SIZE]; ///< GOOSEͨ���鲥MAC��ַ
	unsigned short  gooseAppID;                       ///< GOOSEͨ��APPID(��׼Ҫ��ȫվΩһ)
	unsigned short  gooseVlanID;                      ///< GOOSEͨ��VLAN-ID
	unsigned char   gooseVlanPri;                     ///< GOOSEͨ��VLAN-PRIORITY
	unsigned int    gooseMinTime;                     ///< GOOSE���ݱ�λ������������ʱ��������msΪ��λ
	unsigned int    gooseMaxTime;                     ///< ����ʱ��T0,��msΪ��λ
	std::wstring    cbName;                           ///< ���ƿ������
	std::wstring    cbDesc;                           ///< ���ƿ����������
	unsigned int    confRev;                          ///< ���ƿ�����ư汾��
	std::wstring    gooseID;                          ///< ���ƿ�ID
	std::wstring    dataSet;                          ///< Ӧ��GOOSE���ķ��͵����ݼ�������
	std::wstring    type;                             ///< ���ƿ����ͣ��С�GOOSE���͡�GSSE������
}SCDGOOSEANALYSIS_IEDGOOSE_INFO;

///
/// @brief
///     IED��GOOSE���ݼ���Ϣ(����ͨ��)
///
typedef struct  
{
	int           idxSeq;                             ///< ���ݼ���Ϣ��DataSet�ж����˳���
	std::wstring  dataSetName;                        ///< �������ݼ�������
	std::wstring  dataSetDesc;                        ///< �������ݼ�������
	std::wstring  ldInst;                             ///< �߼��豸ʵ��
	std::wstring  lnPrefix;                           ///< �߼��ڵ�ǰ׺
	std::wstring  lnClass;                            ///< �߼��ڵ�����
	std::wstring  lnInst;                             ///< �߼��ڵ�ʵ��
	std::wstring  lnDesc;                             ///< �߼��ڵ�����
	std::wstring  doName;                             ///< ���ݶ�������
	std::wstring  doDesc;                             ///< ���ݶ�������
	std::wstring  dUsAddr;                            ///< ���ݶ�������������DU�Ķ̵�ַ��Ϣ
	std::wstring  dUVal;                              ///< ���ݶ�������������DU��Ĭ��ֵ
	std::wstring  daName;                             ///< ������������
	std::wstring  daFc;                               ///< �������Թ���Լ��
	std::wstring  daBType;                            ///< �������ԵĻ�������
}SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO;

///
/// @brief
///     IED��GOOSE�������֮�������ⲿ�źŵ���Ϣ
///
typedef struct
{
	int           idxSeq_Out;                         ///< �ⲿ�ź���Inputs�е�˳��ţ���0��ʼ
	std::wstring  iedName_Out;                        ///< �ⲿ�ź����ڵ�IED������
	std::wstring  ldInst_Out;                         ///< �ⲿ�ź������߼��豸��ʵ����
	std::wstring  lnName_Out;                         ///< �ⲿ�ź������߼��ڵ������
	std::wstring  doName_Out;                         ///< �ⲿ�ź�����ʶ��DO������
	std::wstring  daName_Out;                         ///< �ⲿ�ź�����ʶ��DA�����ơ���ֵΪ��ʱ����ʾ�ⲿ�ź�ΪDO�������������ԡ�
	std::wstring  lnDesc_Out;                         ///< �ⲿ�ź����ڵ�LN������
	std::wstring  doDesc_Out;                         ///< �ⲿ�ź����ڵ�DO������
	std::wstring  dUDesc_Out;                         ///< �ⲿ�ź����ڵ�DO��dU�������Ե�Val
	int           idxSeq_In;                          ///< �ⲿ�ź���Inputs�е�˳��ţ���0��ʼ
	std::wstring  iedName_In;                         ///< �ⲿ�ź����󶨵��ڲ���ַ��IED������
	std::wstring  ldInst_In;                          ///< �ⲿ�ź����󶨵��ڲ���ַ���߼��豸��ʵ����
	std::wstring  lnName_In;                          ///< �ⲿ�ź����󶨵��ڲ���ַ���߼��ڵ������
	std::wstring  doName_In;                          ///< �ⲿ�ź����󶨵��ڲ���ַ����ʶ��DO������
	std::wstring  daName_In;                          ///< �ⲿ�ź����󶨵��ڲ���ַ����ʶ��DA�����ơ���ֵΪ��ʱ����ʾ�ⲿ�ź�ΪDO�������������ԡ�
	std::wstring  lnDesc_In;                          ///< �ⲿ�ź����󶨵��ڲ���ַ��LN������
	std::wstring  doDesc_In;                          ///< �ⲿ�ź����󶨵��ڲ���ַ��DO������
	std::wstring  dUDesc_In;                          ///< �ⲿ�ź����󶨵��ڲ���ַ��DO��dU�������Ե�Val
}SCDGOOSEANALYSIS_IEDGOOSE_INPUTS_INFO;

#pragma endregion

///
/// @brief
///     SCD�ļ�GOOSE���ý����ӿ�
///
class ISCDGOOSEAnalysis
{
public:

	ISCDGOOSEAnalysis();
	virtual ~ISCDGOOSEAnalysis();

	///
	/// @brief
	///     ��ȡ�����ļ�������IED��������ͨ�Ų�����GOOSE���ƿ������
	///
	/// @param[in]
	///     strFile - SCD�����ļ���(��·��)
	///
	/// @return
	///     GOOSE���ƿ������ - �����ɹ��� -1 - ����ʧ��
	///
	virtual int GetIEDGOOSENum( void ) = 0;

	///
	/// @brief
	///     ��ȡ�����ļ�������������ͨ�Ų�����GOOSE���ƿ���Ϣ
	///
	/// @note
	///     SCL�ļ���ĳ���ƿ���Ϣ���������ÿ��ƿ���Ϣ���践��
	///
	/// @param[in]
	///     iedGOOSEInfoArr - ����GOOSE���ƿ���Ϣ������
	/// @param[in]
	///     arrSize         - ���ݵ��������
	///
	/// @return
	///     �����б�����Ϣ������ - �����ɹ��� -1 - ����ʧ��
	///
	virtual int GetAllIEDGOOSEInfo( 
		SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfoArr,
		const int arrSize ) = 0;

	///
	/// @brief
	///     ��ȡָ��MAC��APPID��GOOSE���ƿ����Ϣ
	///
	/// @note
	///     ���ڶ����ͬAppID�Ŀ��ƿ�ʱ��Ĭ��ѡ���һ��
	///
	/// @param[in]
	///     gooseMAC     - GOOSEͨ���鲥MAC��ַ
	/// @param[in]
	///     gooseAPPID   - GOOSEͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
	/// @param[in]
	///     iedGOOSEInfo - ����GOOSE���ƿ���Ϣ�Ľṹ��
	///
	/// @return
	///     0 - �����ɹ��� -1 - ����ʧ��
	///
	virtual int GetOneIEDGOOSEInfo( 
		const unsigned char *gooseMAC, 
		const unsigned short gooseAppID,
		SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfo ) = 0;

	///
	/// @brief
	///     ��ȡָ��MAC��APPID��ص�GOOSE���ƿ������Ƶ�DataSet��ͨ����FCDA��������
	///
	/// @note
	///     ���ڶ����ͬAppID�Ŀ��ƿ�ʱ��Ĭ��ѡ���һ��
	///
	/// @param[in]
	///     gooseMAC     - GOOSEͨ���鲥MAC��ַ
	/// @param[in]
	///     gooseAPPID   - GOOSEͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
	///
	/// @return
	///     DataSet��FCDA������ - �����ɹ��� -1 - ����ʧ��
	///
	virtual int GetIEDGOOSEDataSetNum( 
		const unsigned char *gooseMAC, 
		const unsigned short gooseAppID )= 0;

	///
	/// @brief
	///     ��ȡָ��MAC��APPID��ص�GOOSE���ƿ������Ƶ�DataSet������ͨ����FCDA����Ϣ
	///
	/// @note
	///     ���ڶ����ͬAppID�Ŀ��ƿ�ʱ��Ĭ��ѡ���һ��
	///     SCL�ļ���ĳͨ����Ϣ����������ͨ����Ϣ���践��
	///
	/// @param[in]
	///     gooseMAC       - GOOSEͨ���鲥MAC��ַ
	/// @param[in]
	///     gooseAPPID     - GOOSEͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
	// @param[in]
	///     dataSetInfoArr - ����GOOSE��ͨ����Ϣ������
	// @param[in]
	///     arrSize        - ������������
	///
	/// @return
	///     ���鱣���ͨ����FCDA����Ϣ������ - �����ɹ��� -1 - ����ʧ��
	///
	virtual int GetAllIEDGOOSEDataSetInfo(
		const unsigned char *gooseMAC, 
		const unsigned short gooseAppID,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfoArr,
		const int arrSize ) = 0;

	///
	/// @brief
	///     ��ȡָ��MAC��APPID��ص�GOOSE���ƿ������Ƶ�DataSet��ĳһ˳��ŵ�ͨ����FCDA����Ϣ
	///
	/// @note
	///     ���ڶ����ͬAppID�Ŀ��ƿ�ʱ��Ĭ��ѡ���һ��
	///
	/// @param[in]
	///     gooseMAC       - GOOSEͨ���鲥MAC��ַ
	/// @param[in]
	///     gooseAPPID     - GOOSEͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
	// @param[in]
	///     idxSeq         - ͨ����˳���(��0��ʼ)
	// @param[in]
	///     dataSetInfo    - ����ͨ����Ϣ�Ľṹ��
	///
	/// @return
	///     0 - �����ɹ��� -1 - ����ʧ��
	///
	virtual int GetOneIEDGOOSEDataSetInfo(
		const unsigned char *gooseMAC, 
		const unsigned short gooseAppID,
		const int            idxSeq,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo ) = 0;

	///
	/// @brief
	///     ��ȡָ��MAC��APPID��ص�GOOSE���ƿ������Ƶ�DataSet��ĳһͨ����FCDA����˳���
	///
	/// @note
	///     ���ڶ����ͬAppID�Ŀ��ƿ�ʱ��Ĭ��ѡ���һ��
	///
	/// @param[in]
	///     gooseMAC       - GOOSEͨ���鲥MAC��ַ
	/// @param[in]
	///     gooseAPPID     - GOOSEͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
	// @param[in]
	///     dataSetInfo    - ����ͨ����Ϣ�Ľṹ��
	///
	/// @return
	///     ͨ����FCDA����˳��� - �����ɹ��� -1 - ����ʧ��
	///
	virtual int GetOneIEDGOOSEDataSetIdxSeq(
		const unsigned char *gooseMAC, 
		const unsigned short gooseAppID,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo ) = 0;

	///
	/// @brief
	///     ��ȡָ��MAC��APPID��GOOSE���ƿ�Ŀ��Ƶ�ĳһ��ͨ������ͨ������Ŀ
	///
	/// @note
	///     ���ڶ����ͬAppID�Ŀ��ƿ�ʱ��Ĭ��ѡ���һ��
	///
	/// @param[in]
	///     gooseMAC       - GOOSEͨ���鲥MAC��ַ
	/// @param[in]
	///     gooseAPPID     - GOOSEͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
	// @param[in]
	///     dataSetInfo    - ��ͨ����Ϣ
	///
	/// @return
	///     ��ͨ����Ŀ - �����ɹ��� -1 - ����ʧ��
	///
	virtual int GetIEDGOOSESubChannelNum(
		const unsigned char *gooseMAC, 
		const unsigned short gooseAppID,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo) = 0;

	///
	/// @brief
	///     ��ȡָ��MAC��APPID��GOOSE���ƿ�Ŀ��Ƶ�ĳһ��ͨ����ȫ����ͨ������Ϣ
	///
	/// @note
	///     ���ڶ����ͬAppID�Ŀ��ƿ�ʱ��Ĭ��ѡ���һ��
	///     SCL�ļ���ĳͨ����Ϣ����������ͨ����Ϣ���践��
	///
	/// @param[in]
	///     gooseMAC          - GOOSEͨ���鲥MAC��ַ
	/// @param[in]
	///     gooseAPPID        - GOOSEͨ�Ŷ�Ӧ��APPID(��׼Ҫ��ȫվΩһ)
	// @param[in]
	///     dataSetInfo       - ��ͨ����Ϣ
	// @param[in]
	///     subChannelInfoArr - ������ͨ����Ϣ������
	// @param[in]
	///     arrSize           - �����С
	///
	/// @return
	///     ʵ�ʵ���ͨ���� - �����ɹ��� -1 - ����ʧ��
	///
	virtual int GetAllIEDGOOSESubChannelInfo(
		const unsigned char *gooseMAC, 
		const unsigned short gooseAppID,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *subChannelInfoArr,
		const int arrSize ) = 0;

	///
	/// @brief
	///     ��ȡĳһIED�����󶨵��ⲿ�źţ�ExtRef����GOOSE�����źţ�������
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
	virtual int GetOneIEDGOINExtRef(
		const std::wstring &iedName,
		SCDGOOSEANALYSIS_IEDGOOSE_INPUTS_INFO *inputsInfo,
		const int arrSize) = 0;

	///
	/// @brief
	///     ��ȡ����ĳһ�ⲿ�ź������GOOSE���ƿ����Ϣ
	///
	/// @param[in]
	///     inputsInfo   - �ⲿ�źŵ�������Ϣ
	/// @param[in]
	///     iedGOOSEInfo - �ⲿ�ź���ص�GOOSE���ƿ����Ϣ
	///
	/// @return
	///     0 - �����ɹ��� -1 - ����ʧ��
	///
	virtual int GetOutAddrGOOSEInfo(
		const SCDGOOSEANALYSIS_IEDGOOSE_INPUTS_INFO &inputsInfo,
		SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfo) = 0;

	///
	/// @brief
	///     ��ȡȫ����GOOSE���ƿ����Ŀ������δ����ͨ�Ų����Ŀ��ƿ飩
	///
	/// @return
	///     ���ƿ����Ŀ�������ɹ���  -1��ʧ��
	///
	virtual int GetAllIEDGOOSENumByCtrlRef( void ) = 0;

	///
	/// @brief
	///     ��ȡȫ����GOOSE���ƿ����Ϣ������δ����ͨ�Ų����Ŀ��ƿ飩��ͨ�����ƿ��������б�������
	///
	/// @note
	///     SCL�ļ���ĳ���ƿ���Ϣ���������ÿ��ƿ���Ϣ���践��
	///     δ���ò��ֽ���Ĭ������
	///
	/// @param[in]
	///     iedGOOSEInfoArr - ����SMV���ƿ���Ϣ������
	/// @param[in]
	///     arrSize         - �����С
	///
	/// @return
	///     �����б���Ŀ��ƿ�������������ɹ���  -1��ʧ��
	///
	virtual int GetAllIEDGOOSEInfoByCtrlRef(
		SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfoArr, 
		const int arrSize ) = 0;

	///
	/// @brief
	///     ����һ��GOOSE���ƿ飬ͨ�����ƿ��������б�������
	///
	/// @param[in]
	///     iedGOOSEInfo - GOOSE���ƿ���Ϣ
	///
	/// @return
	///     0�������ɹ���  -1��ʧ��
	///
	virtual int SetOneIEDGOOSEInfoByCtrlRef(const SCDGOOSEANALYSIS_IEDGOOSE_INFO &iedGOOSEInfo) = 0;

	///
	/// @brief
	///     ��ȡָ��������ַ��GOOSE���ƿ������Ƶ�DataSet��ͨ����FCDA��������
	///
	/// @param[in]
	///     iedName        - GOOSE���ƿ����ڵ�IED������
	/// @param[in]
	///     ldInst         - GOOSE���ƿ����ڵ�LDevice��Inst��
	/// @param[in]
	///     cbName         - GOOSE���ƿ������
	///
	/// @return
	///     DataSet��FCDA�������������ɹ���  -1��ʧ��
	///
	virtual int GetIEDGOOSEDataSetNum(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName) = 0;

	///
	/// @brief
	///     ��ȡָ��������ַ��GOOSE���ƿ������Ƶ�DataSet������ͨ����FCDA����Ϣ
	///
	/// @note
	///     SCL�ļ���ĳͨ����Ϣ����������ͨ����Ϣ���践��
	///
	/// @param[in]
	///     iedName        - GOOSE���ƿ����ڵ�IED������
	/// @param[in]
	///     ldInst         - GOOSE���ƿ����ڵ�LDevice��Inst��
	/// @param[in]
	///     cbName         - GOOSE���ƿ������
	/// @param[in]
	///     dataSetInfoArr - ����GOOSE��ͨ����Ϣ������
	/// @param[in]
	///     arrSize        - ������������
	///
	/// @return
	///     ���鱣��ͨ����Ϣ�������������ɹ���  -1��ʧ��
	///
	virtual int GetAllIEDGOOSEDataSetInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfoArr,
		const int arrSize ) = 0;

	///
	/// @brief
	///     ��ȡָ��������ַ��GOOSE���ƿ������Ƶ�DataSet��ĳһ˳��ŵ�ͨ����FCDA����Ϣ
	///
	/// @param[in]
	///     iedName        - GOOSE���ƿ����ڵ�IED������
	/// @param[in]
	///     ldInst         - GOOSE���ƿ����ڵ�LDevice��Inst��
	/// @param[in]
	///     cbName         - GOOSE���ƿ������
	/// @param[in]
	///     idxSeq         - GOOSEͨ����˳���
	/// @param[in]
	///     dataSetInfo    - ָ�򱣴�GOOSE��ͨ����Ϣ�Ľṹ���ָ��
	///
	/// @return
	///     0�������ɹ���  -1��ʧ��
	///
	virtual int GetOneIEDGOOSEDataSetInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName,
		const int idxSeq,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo) = 0;

	///
	/// @brief
	///     ����Goose���ƿ����������ݼ��е�һ��ͨ����FCDA��
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
	///     eChannelType - ��ͨ����Ҫ���õ�ͨ������
	///
	/// @return
	///     0�������ɹ���  -1��ʧ��
	///
	virtual int SetOneIEDGOOSEDataSetInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		CHANNEL_TYPE_GOOSE eChannelType) = 0;

	///
	/// @brief
	///     ��Goose���ƿ����������ݼ�ĩβ���һ��ͨ����FCDA��
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
	virtual int AddOneIEDGOOSEDataSetInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		CHANNEL_TYPE_GOOSE eChannelType ) = 0;

	///
	/// @brief
	///     ��GOOSEͨ������ĩβ������ͨ������ĳһͨ������Ϣ����n��
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
	virtual int CopyIEDGOOSEDataSetInfoToSetEnd(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &dataSetName,
		int idxSeq,
		int iCopyCount) = 0;

	///
	/// @brief
	///     �Ƴ�Goose���ƿ����������ݼ��е�һ��ͨ����FCDA��
	///
	/// @param[in]
	///     iedName    - ��ͨ�����ڵ�IED������
	/// @param[in]
	///     ldInst     - ��ͨ�����ڵ�LDevice��Inst��
	/// @param[in]
	///     datSetName - ��ͨ�����ڵ�DataSet������
	/// @param[in]
	///     idxSeq     - ��ͨ����˳���
	///
	/// @return
	///     0�������ɹ���  -1��ʧ��
	///
	virtual int RemoveOneIEDGOOSEDataSetInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &datSetName,
		const int idxSeq) = 0;

	///
	/// @brief
	///     ��ȡָ��������ַ��GOOSE���ƿ�Ŀ��Ƶ�ĳһ��ͨ������ͨ������Ŀ
	///
	/// @param[in]
	///     iedName        - ����ͨ�����ڵ�IED������
	/// @param[in]
	///     ldInst         - ����ͨ�����ڵ�LDevice��Inst��
	// @param[in]
	///     dataSetInfo    - ��ͨ����Ϣ
	///
	/// @return
	///     ��ͨ����Ŀ - �����ɹ��� -1 - ����ʧ��
	///
	virtual int GetIEDGOOSESubChannelNum(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo) = 0;

	///
	/// @brief
	///     ��ȡָ��������ַ��GOOSE���ƿ�Ŀ��Ƶ�ĳһ��ͨ����ȫ����ͨ������Ϣ
	///
	/// @note
	///     SCL�ļ���ĳͨ����Ϣ����������ͨ����Ϣ���践��
	///
	/// @param[in]
	///     iedName           - ����ͨ�����ڵ�IED������
	/// @param[in]
	///     ldInst            - ����ͨ�����ڵ�LDevice��Inst��
	// @param[in]
	///     dataSetInfo       - ��ͨ����Ϣ
	// @param[in]
	///     subChannelInfoArr - ������ͨ����Ϣ������
	// @param[in]
	///     arrSize           - �����С
	///
	/// @return
	///     ʵ�ʵ���ͨ���� - �����ɹ��� -1 - ����ʧ��
	///
	virtual int GetAllIEDGOOSESubChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *subChannelInfoArr,
		const int arrSize ) = 0;

	///
	/// @brief
	///     ��ȡָ��������ַ��GOOSE���ƿ�Ŀ��Ƶ�ĳһ��ͨ����ĳһ��ͨ������Ϣ
	///
	/// @param[in]
	///     iedName           - ����ͨ�����ڵ�IED������
	/// @param[in]
	///     ldInst            - ����ͨ�����ڵ�LDevice��Inst��
	// @param[in]
	///     dataSetInfo       - ��ͨ����Ϣ
	// @param[in]
	///     idxSeq            - ��ͨ����˳��ţ���0��ʼ��
	// @param[in]
	///     subChannelInfo    - ������ͨ����Ϣ������
	///
	/// @return
	///     0 - �����ɹ��� -1 - ����ʧ��
	///
	virtual int GetOneIEDGOOSESubChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		int idxSeq,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &subChannelInfo ) = 0;

	///
	/// @brief
	///     ����Goose���ƿ����������ݼ��е�ĳһͨ���е�һ����ͨ��
	///
	/// @note
	///     ��ͨ��Ϊ���Զ���ͨ��ʱ���Զ�ת��Ϊ�Զ���ͨ��
	///
	/// @param[in]
	///     iedName           - ����ͨ�����ڵ�IED������
	/// @param[in]
	///     ldInst            - ����ͨ�����ڵ�LDevice��Inst��
	/// @param[in]
	///     channelInfo       - ����ͨ������Ӧ������ͨ����Ϣ�������ƿ����������ݼ��е�ĳһͨ����
	/// @param[in]
	///     subChannelIdxList - �Ӷ���ͨ���µĵ�һ����ͨ����ʼ����ǰ��ͨ��������ͨ����˳����б�
	/// @param[in]
	///     listSize          - ˳����б�Ĵ�С
	/// @param[in]
	///     eChannelType      - ����ͨ����Ҫ���õ�ͨ�����ͣ����㡢˫��...��
	///
	/// @return
	///     0�������ɹ���  -1��ʧ��
	///
	virtual int SetOneIEDGOOSESubChannelInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
		const int *subChannelIdxList,
		int listSize,
		CHANNEL_TYPE_GOOSE eChannelType) = 0;

	///
	/// @brief
	///     ��Gooseͨ����ĳһ��ͨ����ĩβ��������ͨ�����е�һ����ͨ������n��
	///
	/// @param[in]
	///     iedName           - ����ͨ�����ڵ�IED������
	/// @param[in]
	///     ldInst            - ����ͨ�����ڵ�LDevice��Inst��
	/// @param[in]
	///     channelInfo       - ����ͨ������Ӧ������ͨ����Ϣ�������ƿ����������ݼ��е�ĳһͨ����
	/// @param[in]
	///     subChannelIdxList - �Ӷ���ͨ���µĵ�һ����ͨ����ʼ����ǰ��ͨ��������ͨ����˳����б�
	/// @param[in]
	///     listSize          - ˳����б�Ĵ�С
	/// @param[in]
	///     iCopyCount        - ����������
	///
	/// @return
	///     0�������ɹ���  -1��ʧ��
	///
	virtual int CopyIEDGooseSubChannelToEnd(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
		const int *subChannelIdxList,
		int listSize,
		int iCopyCount) = 0;

	///
	/// @brief
	///     �Ƴ�Goose���ƿ����������ݼ��е�ĳһͨ���е�һ����ͨ��
	///
	/// @param[in]
	///     iedName           - ����ͨ�����ڵ�IED������
	/// @param[in]
	///     ldInst            - ����ͨ�����ڵ�LDevice��Inst��
	/// @param[in]
	///     channelInfo       - ����ͨ������Ӧ������ͨ����Ϣ�������ƿ����������ݼ��е�ĳһͨ����
	/// @param[in]
	///     subChannelIdxList - �Ӷ���ͨ���µĵ�һ����ͨ����ʼ����ǰ��ͨ��������ͨ����˳����б�
	/// @param[in]
	///     listSize          - ˳����б�Ĵ�С
	///
	/// @return
	///     0�������ɹ���  -1��ʧ��
	///
	virtual int RemoveOneIEDGOOSESubChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
		const int *subChannelIdxList,
		int listSize) = 0;

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
	///     channelInfo - goose���ݼ��е�����ͨ����Ϣ
	///
	/// @return
	///     ���ض�Ӧ��ͨ��������Ϣ ����/˫��/ʱ��...
	///
	virtual CHANNEL_TYPE_GOOSE GetChannelType(const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO& channelInfo) = 0;

	///
	/// @brief
	///     ��ȡ�Զ�����������ģ���е�DaName��Ϣ
	///
	/// @param[in]
	///     eChannelType - gooseͨ������
	/// @param[in]
	///     daName       - �����Զ�����������ģ���е�DaName��Ϣ
	///
	/// @return
	///     
	///
	virtual void GetDefDataTemplate_DaName(CHANNEL_TYPE_GOOSE eChannelType, std::wstring &daName) = 0;
};


class ISCLCacheManager;

///
/// @brief
///     SCD�ļ�GOOSE���ý����ӿڽӿڶ���ʵ��
///
/// @param[in]
///     pSclCacheMgr - ָ��Scl�����������ָ��
///
/// @return
///     SCD�ļ�GOOSE���ý����ӿ�
///
SCLANALYSISDLL_API ISCDGOOSEAnalysis *CreateSCDGOOSEAnalysisImpl( ISCLCacheManager *pSclCacheMgr );

///
/// @brief
///     �ͷ�SCD�ļ�goose92���ý����ӿڶ���ʵ��
///
/// @param[in]
///      objPtr - ָ��ӿڶ���ʵ����ָ��
///
/// @return
///     
///
SCLANALYSISDLL_API void  RelaseSCDGOOSEAnalysisImpl(ISCDGOOSEAnalysis *objPtr);

#endif
