#ifndef SCL_MANAGER_H_

#define SCL_MANAGER_H_

#include "src\utils\SclAnalysis\include\sclanalysis.h"
#include "src\utils\SclAnalysis\include\ISCLCacheManager.h"
#include "src\utils\SclAnalysis\include\SCDGOOSEAnalysis.h"
#include "src\utils\SclAnalysis\include\SCDSMV92Analysis.h"

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
	int Load(const std::wstring strFileName);

	///
	/// @brief
	///    ж��SCL�ļ�
	///
	void UnLoad();

	///
	/// @brief
	///    ���滺����scl�ļ�
	///
	int Save(const std::wstring strFileName);

	///
	/// @brief
	///     ��ȡȫ����SMV���ƿ����Ŀ������δ����ͨ�Ų����Ŀ��ƿ飩
	///
	/// @return
	///     ���ƿ����Ŀ�������ɹ���  -1��ʧ��
	///
	int GetAllIEDSMVNumByCtrlRef( void );

	///
	/// @brief
	///     ��ȡȫ����SMV���ƿ����Ϣ������δ����ͨ�Ų����Ŀ��ƿ飩��ͨ�����ƿ��������б�������
	///     ���У�δ���ò��ֽ���Ĭ������
	///
	/// @param[in]
	///     iedSMVInfoArr - ����SMV���ƿ���Ϣ������
	/// @param[in]
	///     arrSize       - �����С
	///
	/// @return
	///     �����б���Ŀ��ƿ�������������ɹ���  -1��ʧ��
	///
	int GetAllIEDSMVInfoByCtrlRef(
		SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfoArr, 
		const int arrSize );

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
	int SetOneIEDSMVInfoByCtrlRef(const SCDSMV92ANALYSIS_IEDSMV_INFO &iedSMVInfo);

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
	int GetIEDSMVChannelNum(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName);

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
	int GetAllIEDSMVChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfoArr,
		const int arrSize );

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
	int GetOneIEDSMVDataSetInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName,
		const int idxSeq,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo);

	///
	/// @brief
	///     ����SMV���ƿ����������ݼ��е�һ��ͨ����FCDA��
	///     ע�⣬��ͨ����Ϣ�е�lnPrefix��lnClass��lnInst��doName��daName��daFc��daBType��dUsAddr���Ե�������Ч
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
	int SetOneIEDSMVChannelInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &dataSetInfo,
		CHANNEL_TYPE_SMV eChannelType );

	///
	/// @brief
	///     ��SMV���ƿ����������ݼ�ĩβ���һ��ͨ����FCDA��
	///     ע�⣬��ͨ����Ϣ�е�lnPrefix��lnClass��lnInst��doName��daName��daFc��daBType��dUsAddr���Ե�������Ч
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
	int AddOneIEDSMVChannelInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &dataSetInfo,
		CHANNEL_TYPE_SMV eChannelType );

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
	int CopyIEDSMVChannelInfoToSetEnd(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &dataSetName,
		int idxSeq, int iCopyCount );

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
	int RemoveOneIEDSMVChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &datSetName,
		const int idxSeq);

	///
	/// @brief
	///     �ж�ĳAppID��SMV���ƿ��Ƿ����
	///
	/// @param[in]
	///     appID - ���ƿ��AppIDֵ
	///
	/// @return
	///     0 - ���ڣ� -1 - ������
	///
	int IsSmvAppIDExisted(const unsigned short smvAppID);

	///
	/// @brief
	///     ��ȡȫ����GOOSE���ƿ����Ŀ������δ����ͨ�Ų����Ŀ��ƿ飩
	///
	/// @return
	///     ���ƿ����Ŀ�������ɹ���  -1��ʧ��
	///
	int GetAllIEDGOOSENumByCtrlRef( void );

	///
	/// @brief
	///     ��ȡȫ����GOOSE���ƿ����Ϣ������δ����ͨ�Ų����Ŀ��ƿ飩��ͨ�����ƿ��������б�������
	///     ���У�δ���ò��ֽ���Ĭ������
	///
	/// @param[in]
	///     iedGOOSEInfoArr - ����SMV���ƿ���Ϣ������
	/// @param[in]
	///     arrSize         - �����С
	///
	/// @return
	///     �����б���Ŀ��ƿ�������������ɹ���  -1��ʧ��
	///
	int GetAllIEDGOOSEInfoByCtrlRef(
		SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfoArr, 
		const int arrSize );

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
	int SetOneIEDGOOSEInfoByCtrlRef(const SCDGOOSEANALYSIS_IEDGOOSE_INFO &iedGOOSEInfo);

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
	int GetIEDGOOSEChannelNum(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName);

	///
	/// @brief
	///     ��ȡָ��������ַ��GOOSE���ƿ������Ƶ�DataSet������ͨ����FCDA����Ϣ
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
	int GetAllIEDGOOSEChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfoArr,
		const int arrSize );

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
	int GetOneIEDGOOSEDataSetInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &cbName,
		const int idxSeq,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo);

	///
	/// @brief
	///     ����Goose���ƿ����������ݼ��е�һ��ͨ����FCDA��
	///     ע�⣬��ͨ����Ϣ�е�lnPrefix��lnClass��lnInst��doName��daName��daFc��daBType��dUsAddr���Ե�������Ч
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
	int SetOneIEDGOOSEChannelInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		CHANNEL_TYPE_GOOSE eChannelType );

	///
	/// @brief
	///     ��Goose���ƿ����������ݼ�ĩβ���һ��ͨ����FCDA��
	///     ע�⣬��ͨ����Ϣ�е�lnPrefix��lnClass��lnInst��doName��daName��daFc��daBType��dUsAddr���Ե�������Ч
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
	int AddOneIEDGOOSEDataSetInfo_Def(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		CHANNEL_TYPE_GOOSE eChannelType );

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
	int CopyIEDGOOSEDataSetInfoToSetEnd(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &dataSetName,
		int idxSeq,
		int iCopyCount);

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
	int RemoveOneIEDGOOSEChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const std::wstring &datSetName,
		const int idxSeq);

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
	int GetIEDGOOSESubChannelNum(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo);

	///
	/// @brief
	///     ��ȡָ��������ַ��GOOSE���ƿ�Ŀ��Ƶ�ĳһ��ͨ����ȫ����ͨ������Ϣ
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
	int GetAllIEDGOOSESubChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *subChannelInfoArr,
		const int arrSize );

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
	int GetOneIEDGOOSESubChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
		int idxSeq,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &subChannelInfo );

	///
	/// @brief
	///     ����Goose���ƿ����������ݼ��е�ĳһͨ���е�һ����ͨ��
	///     ע�⣬��ͨ��Ϊ���Զ���ͨ��ʱ���Զ�ת��Ϊ�Զ���ͨ��
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
	int SetOneIEDGOOSESubChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
		const int *subChannelIdxList,
		int listSize,
		CHANNEL_TYPE_GOOSE eChannelType);

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
	int CopyIEDGooseSubChannelToEnd(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
		const int *subChannelIdxList,
		int listSize,
		int iCopyCount);

	///
	/// @brief
	///     �Ƴ�Goose���ƿ����������ݼ��е�ĳһͨ���е�һ����ͨ��
	///
	/// @param[in]
	///     iedName    - ����ͨ�����ڵ�IED������
	/// @param[in]
	///     ldInst     - ����ͨ�����ڵ�LDevice��Inst��
	/// @param[in]
	///     lnName     - ����ͨ�����ڵ�lNode������
	/// @param[in]
	///     doName     - ����ͨ�����ڵ�DOI������
	/// @param[in]
	///     idxSeq     - ����ͨ����˳���
	///
	/// @return
	///     0�������ɹ���  -1��ʧ��
	///
	int RemoveOneIEDGOOSESubChannelInfo(
		const std::wstring &iedName,
		const std::wstring &ldInst,
		SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
		const int *subChannelIdxList,
		int listSize);

	///
	/// @brief
	///     �ж�ĳAppID��GOOSE���ƿ��Ƿ����
	///
	/// @param[in]
	///     appID - ���ƿ��AppIDֵ
	///
	/// @return
	///     0 - ���ڣ� -1 - ������
	///
	int IsGOOSEAppIDExisted(const unsigned short appID);

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
	CHANNEL_TYPE_SMV GetChannelType(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo);

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
	CHANNEL_PHASE_SMV GetChannelPhase(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo, CHANNEL_TYPE_SMV eChannelType);

	///
	/// @brief
	///     ��ȡͨ����������Ϣ
	///
	/// @param[in]
	///     stGooseChannelInfo - goose���ݼ��е�����ͨ����Ϣ
	///
	/// @return
	///     ���ض�Ӧ��ͨ��������Ϣ ����/˫��/ʱ��...
	///
	CHANNEL_TYPE_GOOSE GetChannelType(const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO& stGooseChannelInfo);

	///
	/// @brief
	///     ��ȡGOOSE�Զ�����������ģ���е�DaName��Ϣ
	///
	/// @param[in]
	///     eChannelType - gooseͨ������
	/// @param[in]
	///     daName       - �����Զ�����������ģ���е�DaName��Ϣ
	///
	/// @return
	///     
	///
	void GetDefDataTemplate_DaName(CHANNEL_TYPE_GOOSE eChannelType, std::wstring &daName);

protected:

private:
	ISCLCacheManager  *m_pSclCache;
	ISCDSMV92Analysis *m_pSmv92Analysis;
	ISCDGOOSEAnalysis *m_pGooseAnalysis;
};

#endif
