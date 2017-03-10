#include "stdafx.h"
#include "SclManager.h"

CSCLManager::CSCLManager()
{
	m_pSclCache      = CreateSCLCacheManagerImpl();
	m_pSmv92Analysis = CreateSCDSMV92AnalysisImpl(m_pSclCache);
	m_pGooseAnalysis = CreateSCDGOOSEAnalysisImpl(m_pSclCache);
	m_pSclCache->EnableSmvAnalysis();
	m_pSclCache->EnableGseAnalysis();
}

CSCLManager::~CSCLManager()
{
	RelaseSCDGOOSEAnalysisImpl(m_pGooseAnalysis);
	RelaseSCDSMV92AnalysisImpl(m_pSmv92Analysis);
	RelaseSCLCacheManagerImpl(m_pSclCache);
}

///
/// @brief
///    ��ȡ��̬ʵ����ȫ��ʹ��һ��������
///
CSCLManager* CSCLManager::getInstance()
{
	static CSCLManager s_SclManager;
	return &s_SclManager;
}

///
/// @brief
///    ����scl�ļ�
///
int CSCLManager::Load(const std::wstring strFileName)
{
	return m_pSclCache->LoadSCL(strFileName);
}

///
/// @brief
///    ж��SCL�ļ�
///
void CSCLManager::UnLoad()
{
	m_pSclCache->UnloadSCL();
}

///
/// @brief
///    ���滺����scl�ļ�
///
int CSCLManager::Save(const std::wstring strFileName)
{
	return m_pSclCache->SaveSCL(strFileName);
}

///
/// @brief
///     ��ȡȫ����SMV���ƿ����Ŀ������δ����ͨ�Ų����Ŀ��ƿ飩
///
/// @return
///     ���ƿ����Ŀ�������ɹ���  -1��ʧ��
///
int CSCLManager::GetAllIEDSMVNumByCtrlRef( void )
{
	return m_pSmv92Analysis->GetAllIEDSMVNumByCtrlRef();
}

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
int CSCLManager::GetAllIEDSMVInfoByCtrlRef(SCDSMV92ANALYSIS_IEDSMV_INFO *iedSMVInfoArr, const int arrSize )
{
	return m_pSmv92Analysis->GetAllIEDSMVInfoByCtrlRef(iedSMVInfoArr,arrSize);
}

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
int CSCLManager::SetOneIEDSMVInfoByCtrlRef(const SCDSMV92ANALYSIS_IEDSMV_INFO &iedSMVInfo)
{
	return m_pSmv92Analysis->SetOneIEDSMVInfoByCtrlRef(iedSMVInfo);
}

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
int CSCLManager::GetIEDSMVChannelNum(const std::wstring &iedName, const std::wstring &ldInst, const std::wstring &cbName)
{
	return m_pSmv92Analysis->GetIEDSMVDataSetNum(iedName,ldInst,cbName);
}

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
int CSCLManager::GetAllIEDSMVChannelInfo( const std::wstring &iedName, const std::wstring &ldInst,
										  const std::wstring &cbName, SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfoArr,
										  const int arrSize )
{
	return m_pSmv92Analysis->GetAllIEDSMVDataSetInfo(iedName,ldInst,cbName,dataSetInfoArr,arrSize);
}

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
int CSCLManager::GetOneIEDSMVDataSetInfo(const std::wstring &iedName, const std::wstring &ldInst,
							             const std::wstring &cbName, const int idxSeq,
										 SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo)
{
	return m_pSmv92Analysis->GetOneIEDSMVDataSetInfo(iedName,ldInst,cbName,idxSeq,dataSetInfo);
}

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
int CSCLManager::SetOneIEDSMVChannelInfo_Def( const std::wstring &iedName, const std::wstring &ldInst,
											  SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &dataSetInfo,
											  CHANNEL_TYPE_SMV eChannelType )
{
	return m_pSmv92Analysis->SetOneIEDSMVDataSetInfo_Def(iedName,ldInst,dataSetInfo,eChannelType);
}

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
int CSCLManager::AddOneIEDSMVChannelInfo_Def( const std::wstring &iedName, const std::wstring &ldInst,
											  SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO &dataSetInfo,
											  CHANNEL_TYPE_SMV eChannelType )
{
	return m_pSmv92Analysis->AddOneIEDSMVDataSetInfo_Def(iedName,ldInst,dataSetInfo,eChannelType);
}

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
int CSCLManager::CopyIEDSMVChannelInfoToSetEnd( const std::wstring &iedName, const std::wstring &ldInst,
											    const std::wstring &dataSetName, int idxSeq, int iCopyCount )
{
	return m_pSmv92Analysis->CopyIEDSMVDataSetInfoToSetEnd(iedName,ldInst,dataSetName,idxSeq,iCopyCount);
}

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
int CSCLManager::RemoveOneIEDSMVChannelInfo(const std::wstring &iedName, const std::wstring &ldInst,
											const std::wstring &datSetName, const int idxSeq)
{
	return m_pSmv92Analysis->RemoveOneIEDSMVDataSetInfo(iedName,ldInst,datSetName,idxSeq);
}

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
int CSCLManager::IsSmvAppIDExisted(const unsigned short smvAppID)
{
	return m_pSmv92Analysis->IsAppIDExisted(smvAppID);
}

///
/// @brief
///     ��ȡȫ����GOOSE���ƿ����Ŀ������δ����ͨ�Ų����Ŀ��ƿ飩
///
/// @return
///     ���ƿ����Ŀ�������ɹ���  -1��ʧ��
///
int CSCLManager::GetAllIEDGOOSENumByCtrlRef( void )
{
	return m_pGooseAnalysis->GetAllIEDGOOSENumByCtrlRef();
}

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
int CSCLManager::GetAllIEDGOOSEInfoByCtrlRef(SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedGOOSEInfoArr, const int arrSize )
{
	return m_pGooseAnalysis->GetAllIEDGOOSEInfoByCtrlRef(iedGOOSEInfoArr,arrSize);
}

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
int CSCLManager::SetOneIEDGOOSEInfoByCtrlRef(const SCDGOOSEANALYSIS_IEDGOOSE_INFO &iedGOOSEInfo)
{
	return m_pGooseAnalysis->SetOneIEDGOOSEInfoByCtrlRef(iedGOOSEInfo);
}

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
int CSCLManager::GetIEDGOOSEChannelNum(const std::wstring &iedName, const std::wstring &ldInst, const std::wstring &cbName)
{
	return m_pGooseAnalysis->GetIEDGOOSEDataSetNum(iedName,ldInst,cbName);
}

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
int CSCLManager::GetAllIEDGOOSEChannelInfo( const std::wstring &iedName, const std::wstring &ldInst,
										    const std::wstring &cbName, SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfoArr,
											const int arrSize )
{
	return m_pGooseAnalysis->GetAllIEDGOOSEDataSetInfo(iedName,ldInst,cbName,dataSetInfoArr,arrSize);
}

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
int CSCLManager::GetOneIEDGOOSEDataSetInfo(const std::wstring &iedName, const std::wstring &ldInst,
							               const std::wstring &cbName, const int idxSeq,
										   SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo)
{
	return m_pGooseAnalysis->GetOneIEDGOOSEDataSetInfo(iedName,ldInst,cbName,idxSeq,dataSetInfo);
}

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
int CSCLManager::SetOneIEDGOOSEChannelInfo_Def(const std::wstring &iedName, const std::wstring &ldInst,
							               SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
										   CHANNEL_TYPE_GOOSE eChannelType)
{
	return m_pGooseAnalysis->SetOneIEDGOOSEDataSetInfo_Def(iedName,ldInst,channelInfo,eChannelType);
}

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
int CSCLManager::AddOneIEDGOOSEDataSetInfo_Def(const std::wstring &iedName, const std::wstring &ldInst,
											   SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
											   CHANNEL_TYPE_GOOSE eChannelType )
{
	return m_pGooseAnalysis->AddOneIEDGOOSEDataSetInfo_Def(iedName,ldInst,dataSetInfo,eChannelType);
}

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
int CSCLManager::CopyIEDGOOSEDataSetInfoToSetEnd(const std::wstring &iedName, const std::wstring &ldInst,
												 const std::wstring &dataSetName, int idxSeq,
												 int iCopyCount)
{
	return m_pGooseAnalysis->CopyIEDGOOSEDataSetInfoToSetEnd(iedName,ldInst,dataSetName,idxSeq,iCopyCount);
}

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
int CSCLManager::RemoveOneIEDGOOSEChannelInfo(const std::wstring &iedName, const std::wstring &ldInst,
	                                          const std::wstring &datSetName, const int idxSeq)
{
	return m_pGooseAnalysis->RemoveOneIEDGOOSEDataSetInfo(iedName,ldInst,datSetName,idxSeq);
}

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
int CSCLManager::GetIEDGOOSESubChannelNum(const std::wstring &iedName, const std::wstring &ldInst,
										  const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo)
{
	return m_pGooseAnalysis->GetIEDGOOSESubChannelNum(iedName,ldInst,dataSetInfo);
}

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
///     0 - �����ɹ��� -1 - ����ʧ��
///
int CSCLManager::GetAllIEDGOOSESubChannelInfo(const std::wstring &iedName, const std::wstring &ldInst,
	                                          const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
	                                          SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *subChannelInfoArr,
	                                          const int arrSize )
{
	return m_pGooseAnalysis->GetAllIEDGOOSESubChannelInfo(iedName,ldInst,dataSetInfo,subChannelInfoArr,arrSize);
}

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
int CSCLManager::GetOneIEDGOOSESubChannelInfo( const std::wstring &iedName, const std::wstring &ldInst,
											   const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
											   int idxSeq,
											   SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &subChannelInfo )
{
	return m_pGooseAnalysis->GetOneIEDGOOSESubChannelInfo(iedName,ldInst,dataSetInfo,idxSeq,subChannelInfo);
}

///
/// @brief
///     ����Goose���ƿ����������ݼ��е�ĳһͨ���е�һ����ͨ��
///     ע�⣬��ͨ��Ϊ���Զ���ͨ��ʱ���Զ�ת��Ϊ�Զ���ͨ��
///
/// @param[in]
///     iedName      - ����ͨ�����ڵ�IED������
/// @param[in]
///     ldInst       - ����ͨ�����ڵ�LDevice��Inst��
/// @param[in]
///     lnName       - ����ͨ�����ڵ�lNode������
/// @param[in]
///     doName       - ����ͨ�����ڵ�DOI������
/// @param[in]
///     idxSeq       - ����ͨ����˳���
/// @param[in]
///     eChannelType - ����ͨ�������ͣ����㡢˫��...��
///
/// @return
///     0�������ɹ���  -1��ʧ��
///
int CSCLManager::SetOneIEDGOOSESubChannelInfo(const std::wstring &iedName, const std::wstring &ldInst,
											  SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
											  const int *subChannelIdxList, int listSize,
											  CHANNEL_TYPE_GOOSE eChannelType)
{
	return m_pGooseAnalysis->SetOneIEDGOOSESubChannelInfo_Def(iedName,ldInst,channelInfo,subChannelIdxList,listSize,eChannelType);
}

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
int CSCLManager::CopyIEDGooseSubChannelToEnd(const std::wstring &iedName, const std::wstring &ldInst,
											 SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
											 const int *subChannelIdxList, int listSize, int iCopyCount)
{
	return m_pGooseAnalysis->CopyIEDGooseSubChannelToEnd(iedName,ldInst,channelInfo,subChannelIdxList,
		listSize,iCopyCount);
}

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
int CSCLManager::RemoveOneIEDGOOSESubChannelInfo(const std::wstring &iedName, const std::wstring &ldInst,
												 SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &channelInfo,
												 const int *subChannelIdxList, int listSize)
{
	return m_pGooseAnalysis->RemoveOneIEDGOOSESubChannelInfo(iedName,ldInst,channelInfo,subChannelIdxList,listSize);
}

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
int CSCLManager::IsGOOSEAppIDExisted(const unsigned short appID)
{
	return m_pGooseAnalysis->IsAppIDExisted(appID);
}

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
CHANNEL_PHASE_SMV CSCLManager::GetChannelPhase(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo, CHANNEL_TYPE_SMV eChannelType)
{
	return m_pSmv92Analysis->GetChannelPhase(stSmvChannelInfo,eChannelType);
}

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
CHANNEL_TYPE_SMV CSCLManager::GetChannelType(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo )
{
	return m_pSmv92Analysis->GetChannelType(stSmvChannelInfo);
}

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
CHANNEL_TYPE_GOOSE CSCLManager::GetChannelType(const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO& stGooseChannelInfo )
{
	return m_pGooseAnalysis->GetChannelType(stGooseChannelInfo);
}

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
void CSCLManager::GetDefDataTemplate_DaName(CHANNEL_TYPE_GOOSE eChannelType, std::wstring &daName)
{
	m_pGooseAnalysis->GetDefDataTemplate_DaName(eChannelType,daName);
}
