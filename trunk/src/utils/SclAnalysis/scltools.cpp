/// @file
///     scltools.cpp
///
/// @brief
///     SCL�ļ���������
///
/// @note
///     ��Markup���������з�װ��ʹ֮���ʺ�SCL�ļ��Ľ�����
///     ��Ҫ�����ļ����ء���λ����λ�ͻ�ȡ������4�ຯ��
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.1.22
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.02.01     |1.0.1.201  |���Ӷ�SCL�ļ���DataTypeTemplates���ֵķ���               |
///    |2013.02.06     |1.0.2.206  |���Ӷ�SCL�ļ����ⲿ�źţ�Inputs��ExtRef���ķ���          |
///    |2013.05.22     |1.0.4.522  |��ӷ���SCL�ļ���ʽ�ĸ�Ԫ�صĲ���ӿ�                    |
///    |2013.06.26     |1.0.4.626  |�޸�����Ԫ�ص���ӽӿ�                                   |
///    |2013.08.07     |1.0.5.807  |�������Ԫ��������Ϣ�Ļ�ȡ�ӿ�                           |
///    |2013.08.08     |1.0.5.808  |����DOI��Ϣ��ȡ�ӿ���dUԪ�ز�����ʱ��Ӧ����Ϣû��ʼ����Bug|
///    |2013.11.04     |1.0.5.1104 |���SMV/GOOSE���ƿ���Ϣʱ���Ӱ汾����Ϣ                  |
///
#include "stdafx.h"
#include "scltools.h"

#define SCL_MARKUP_NODE_FLAG CMarkup::MNF_WITHNOLINES

SCLTools::SCLTools()
{

}

SCLTools::~SCLTools()
{

}

///
/// @brief
///     ����SCL�ļ�
///
/// @param[in]
///     szFileName - �ļ�·��
///
/// @return
///     true�����سɹ���  false������ʧ��
///
bool SCLTools::LoadSCL(const SCL_CSTR_FILENAME &szFileName)
{
	return m_SCLFile.Load(szFileName);
//	bool bLoad = false;
// 	__try
// 	{
//		_set_se_translator(trans_func);
//		bLoad = m_SCLFile.Load(szFileName);
//	}
// 	__except(exception_func(GetExceptionInformation()),EXCEPTION_EXECUTE_HANDLER)
// 	{
//		bLoad = false;
// 		TRACE((_T("Load SCL FAIL.\r\n")));
// 	}
//	set_new_handler(NULL);
//	return bLoad;
}

///
/// @brief
///     ��λ�����ļ��Ŀ�ʼ
///
void SCLTools::ResetPos()
{
	m_SCLFile.ResetPos();
}

///
/// @brief
///     ��λ����ǰԪ�����ڲ㼶�ĵ�1��Ԫ�ش�
///
void SCLTools::ResetMainPos()
{
	m_SCLFile.ResetMainPos();
}

///
/// @brief
///     ��λ����ǰԪ�صĸ�Ԫ�ش�
///
/// @return
///     true����λ�ɹ���  false���Ѿ�Ϊ�ļ��Ķ���
///
bool SCLTools::OutOfElem()
{
	return m_SCLFile.OutOfElem();
}

///
/// @brief
///     ���뵱ǰԪ���ڲ�
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::IntoElem()
{
	// ����currentElem����һ�㼶
	if ( !m_SCLFile.IntoElem() )
		return false;

	return true;
}

///
/// @brief
///     �ڵ�ǰ�㼶������ĳһ����ֵƥ����ض�Ԫ�أ�����λ����Ԫ��
///     ��attribNameΪ�մ�����λ����һ��elemNameԪ��
///
/// @param[in]
///     node       - ��Ҫ��λ����Ԫ�أ�����Ϊ�մ�
/// @param[in]
///     attribName - Ԫ��elemName��ĳһ����
/// @param[in]
///     attribVal  - Ԫ��elemName��attribName���Ե�ֵ
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindElemWithAttrib(const SCL_CSTR elemTag, const SCL_CSTR attribName,
								  const SCL_STR attribVal)
{
	// ���nodeΪ�գ��򷵻�false
	if ( elemTag == _T("") )
		return false;

	// attribNameΪ����λ����һ��elemNameԪ��
	if (attribName == _T("") )
	{
		return m_SCLFile.FindElem(elemTag);
	}

	// ������Ѱ��attribName������ֵΪattribVal��Node������λ����Node
	while (1)
	{
		if ( !m_SCLFile.FindElem(elemTag) )
			return false;
		if ( m_SCLFile.GetAttrib(attribName) == attribVal )
			return true;
	}

	return false;
}

///
/// @brief
///     ���뵱ǰԪ���ڲ�����ĳһ����ֵƥ����ض���Ԫ�أ�����λ����Ԫ��
///     ��attribNameΪ�մ�����λ����һ��subNodeԪ��
///
/// @param[in]
///     parentNode - ��ǰԪ��
/// @param[in]
///     subNode    - ��Ҫ��λ������Ԫ��
/// @param[in]
///     attribName - Ԫ��subNode��ĳһ����
/// @param[in]
///     attribVal  - Ԫ��subNode��attribName���Ե�ֵ
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindChildElemWithAttrib(const SCL_STR parentElem, const SCL_CSTR childElem,
									   const SCL_CSTR attribName, const SCL_STR attribVal)
{
	// ��ǰԪ�ز���parentNode�򷵻�false
	if ( m_SCLFile.GetTagName() != parentElem )
		return false;

	// ����parentNode����һ�㼶
	if ( !m_SCLFile.IntoElem() )
		return false;
	
	return FindElemWithAttrib(childElem, attribName, attribVal);
}

///
/// @brief
///     ��λ��SCLԪ�ء�SCLԪ��ΪSCL�ļ��ĸ�Ԫ��
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindSCL()
{
	// �ص��ļ���ʼ��
	m_SCLFile.ResetPos();

	// ��λ��SCLԪ�أ�һ��SCL�ļ�ֻ��һ��
	return m_SCLFile.FindElem(_T("SCL"));
}

///
/// @brief
///     ��SCLԪ���ڲ�����������nameֵƥ���IEDԪ�أ�����λ����Ԫ��
///     ��iedNameΪ�մ�����λ����һ��IEDԪ��
///
/// @param[in]
///     iedName - Ŀ��IEDԪ�ص�name����ֵ��Ĭ��Ϊ�մ�
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindIEDInSCL(SCL_STR iedName /* = _T */)
{
	SCL_CSTR attribName;

	if ( iedName == _T("") )
		attribName = _T("");
	else
		attribName = _T("name");

	return FindElemWithAttrib(_T("IED"), attribName, iedName);
}

///
/// @brief
///     ��IEDԪ���ڲ�����������nameֵƥ���AccessPointԪ�أ�����λ����Ԫ��
///     ��apNameΪ�մ�����λ����һ��AccessPointԪ��
///
/// @param[in]
///     apName - Ŀ��AccessPointԪ�ص�name����ֵ��Ĭ��Ϊ�մ�
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindAccessPointInIED(SCL_STR apName /* = _T */)
{
	SCL_CSTR attribName;

	if ( apName == _T("") )
		attribName = _T("");
	else
		attribName = _T("name");

	return FindElemWithAttrib(_T("AccessPoint"), attribName, apName);
}

bool SCLTools::FindServerInAccessPoint()
{
	// ����Server��1��AccessPointֻ��1��Server
	return m_SCLFile.FindElem(_T("Server"));
}

///
/// @brief
///     ��ServerԪ���ڲ�����������instֵƥ���LDeviceԪ�أ�����λ����Ԫ��
///     ��ldInstΪ�մ�����λ����һ��LDeviceԪ��
///
/// @param[in]
///     ldInst - Ŀ��LDeviceԪ�ص�inst����ֵ��Ĭ��Ϊ�մ�
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindLDeviceInServer(SCL_STR ldInst /* = _T */)
{
	SCL_CSTR attribName;

	if ( ldInst == _T("") )
		attribName = _T("");
	else
		attribName = _T("inst");

	return FindElemWithAttrib(_T("LDevice"), attribName, ldInst);
}

///
/// @brief
///     ��IEDԪ���ڲ�����������instֵƥ���LDeviceԪ�أ�����λ����Ԫ��
///     ��ldInstΪ�մ�����λ����һ��LDeviceԪ��
///
/// @param[in]
///     ldInst - Ŀ��LDeviceԪ�ص�inst����ֵ��Ĭ��Ϊ�մ�
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindLDeviceInIED(const SCL_STR &ldInst)
{
	if ( ldInst == _T("") )
		return false;
	while ( m_SCLFile.FindElem(_T("AccessPoint")) )
	{
		if ( !m_SCLFile.IntoElem() )
			continue;
		// ����Server��1��AccessPointֻ��1��Server
		if ( !m_SCLFile.FindElem(_T("Server")) )
			continue;
		if ( !m_SCLFile.IntoElem() )
			continue;
		if ( FindElemWithAttrib(_T("LDevice"),_T("inst"),ldInst))
			return true;
		// �ص�Server
		m_SCLFile.OutOfElem();
		// �ص�AccessPoint
		m_SCLFile.OutOfElem();
	}
	return false;
}

///
/// @brief
///     ��LDeviceԪ���ڲ�����LN0Ԫ�أ�����λ����Ԫ��
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindLN0InLDevice()
{
	return m_SCLFile.FindElem(_T("LN0"));
}

///
/// @brief
///     ��LDeviceԪ���ڲ�����������prefix��lnClass��lnInstֵƥ���LNԪ�أ�����λ����Ԫ��
///     ��lnPrefix��lnClass��lnInst��Ϊ�մ�����λ����һ��LNԪ��
///
/// @param[in]
///     lnPrefix - Ŀ��LNԪ�ص�prefix����ֵ
/// @param[in]
///     lnClass  - Ŀ��LNԪ�ص�lnClass����ֵ,Ϊ��LLN0��ʱ��ΪLN0
/// @param[in]
///     lnInst   - Ŀ��LNԪ�ص�lnInst����ֵ
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindLNInLDevice(const SCL_STR &lnPrefix, const SCL_STR &lnClass, const SCL_STR &lnInst)
{
	if ( lnClass == _T("LLN0") )
	{
		return m_SCLFile.FindElem(_T("LN0"));
	}

	if ( lnPrefix == _T("") && lnClass == _T("") && lnInst == _T("") )
	{
		return m_SCLFile.FindElem(_T("LN"));
	}

	while (1)
	{
		if ( !m_SCLFile.FindElem(_T("LN")) )
			return false;
		if ( lnPrefix == m_SCLFile.GetAttrib(_T("prefix")) &&
		     lnClass  == m_SCLFile.GetAttrib(_T("lnClass")) &&
		     lnInst   == m_SCLFile.GetAttrib(_T("inst")) )
			return true;
	}

	return false;
}

///
/// @brief
///     ��LDeviceԪ���ڲ�������lnName��ͬ��LNԪ�أ�����λ����Ԫ��
///     ��lnName��Ϊ�մ�����λ����һ��LNԪ��
///
/// @param[in]
///     lnName - Ŀ��LNԪ�ص����ƣ�Ĭ��Ϊ�մ�����prefix+lnClass+lnInst���
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindLNInLDevice(SCL_STR lnName /* = _T */)
{
	if ( lnName == _T("LLN0") )
	{
		return m_SCLFile.FindElem(_T("LN0"));
	}

	if ( lnName == _T("") )
	{
		return m_SCLFile.FindElem(_T("LN"));
	}

	while (1)
	{
		if ( !m_SCLFile.FindElem(_T("LN")) )
			return false;
		SCL_STR name = m_SCLFile.GetAttrib(_T("prefix")) + m_SCLFile.GetAttrib(_T("lnClass")) + m_SCLFile.GetAttrib(_T("inst"));
		if ( lnName == name )
			return true;
	}

	return false;
}

///
/// @brief
///     ��LN0Ԫ���ڲ�����������nameֵƥ���DataSetԪ�أ�����λ����Ԫ��
///     ��dataSetNameΪ�մ�����λ����һ��DataSetԪ��
///
/// @param[in]
///     dataSetName - Ŀ��DataSetԪ�ص�name����ֵ��Ĭ��Ϊ�մ�
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindDataSetInLN0(SCL_STR dataSetName /* = _T */)
{
	SCL_CSTR attribName;

	if ( dataSetName == _T("") )
		attribName = _T("");
	else
		attribName = _T("name");

	return FindElemWithAttrib(_T("DataSet"), attribName, dataSetName);
}

///
/// @brief
///     ��LN0Ԫ���ڲ�����������nameֵƥ���SampledValueControlԪ�أ�����λ����Ԫ��
///     ��cbNameΪ�մ�����λ����һ��SampledValueControlԪ��
///
/// @param[in]
///     cbName - Ŀ��SampledValueControlԪ�ص�name����ֵ��Ĭ��Ϊ�մ�
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindSMVCtrlInLN0(SCL_STR cbName /* = _T */)
{
	SCL_CSTR attribName;

	if ( cbName == _T("") )
		attribName = _T("");
	else
		attribName = _T("name");

	return FindElemWithAttrib(_T("SampledValueControl"), attribName, cbName);
}

///
/// @brief
///     ��LN0Ԫ���ڲ�����������nameֵƥ���GSEControlԪ�أ�����λ����Ԫ��
///     ��cbNameΪ�մ�����λ����һ��GSEControlԪ��
///
/// @param[in]
///     cbName - Ŀ��GSEControlԪ�ص�name����ֵ��Ĭ��Ϊ�մ�
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindGSECtrlInLN0(SCL_STR cbName /* = _T */)
{
	SCL_CSTR attribName;

	if ( cbName == _T("") )
		attribName = _T("");
	else
		attribName = _T("name");

	return FindElemWithAttrib(_T("GSEControl"), attribName, cbName);
}

///
/// @brief
///     ��LN0����LNԪ���ڲ�����������nameֵƥ���DOIԪ�أ�����λ����Ԫ��
///     ��doNameΪ�մ�����λ����һ��DOIԪ��
///
/// @param[in]
///     doName - Ŀ��DOIԪ�ص�name����ֵ��Ĭ��Ϊ�մ�
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindDOIInLNorLN0(SCL_STR doName /* = _T */)
{
	SCL_CSTR attribName;

	if ( doName == _T("") )
		attribName = _T("");
	else
		attribName = _T("name");

	return FindElemWithAttrib(_T("DOI"), attribName, doName);
}

///
/// @brief
///     ��LN0����LNԪ���ڲ�������һ��InputsԪ�أ�����λ����Ԫ��
///     ��1�����������ǵ�һ��InputsԪ��
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindInputsInLNorLN0()
{
	return m_SCLFile.FindElem(_T("Inputs"));
}

///
/// @brief
///     ��InputsԪ���ڲ�������һ��ExtRefԪ�أ�����λ����Ԫ��
///     ��1�����������ǵ�һ��ExtRefԪ��
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindExtRefInInputs()
{
	return m_SCLFile.FindElem(_T("ExtRef"));
}

///
/// @brief
///     ��DOIԪ���ڲ�����������nameֵƥ���DAIԪ�أ�����λ����Ԫ��
///     ��daNameΪ�մ�����λ����һ��DAIԪ��
///
/// @param[in]
///     daName - Ŀ��DAIԪ�ص�name����ֵ��Ĭ��Ϊ�մ�
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindDAIInDOI(SCL_STR daName /* = _T */)
{
	SCL_CSTR attribName;

	if ( daName == _T("") )
		attribName = _T("");
	else
		attribName = _T("name");

	return FindElemWithAttrib(_T("DAI"), attribName, daName);
}

///
/// @brief
///     ��SCLԪ���ڲ�����CommunicationԪ�أ�����λ����Ԫ��
///     һ��SCLԪ����ֻ��һ��CommunicaitonԪ��
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindCommnInSCL()
{
	return FindElemWithAttrib(_T("Communication"), _T(""), _T(""));
}

///
/// @brief
///     ��CommunicationԪ���ڲ�����������nameֵƥ���SubNetworkԪ�أ�����λ����Ԫ��
///     ��subNetworkNameΪ�մ�����λ����һ��SubNetworkԪ��
///
/// @param[in]
///     subNetworkName - Ŀ��SubNetworkԪ�ص�name����ֵ��Ĭ��Ϊ�մ�
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindSubNetworkInCommn(SCL_STR subNetworkName /* = _T */)
{
	SCL_CSTR attribName;

	if ( subNetworkName == _T("") )
		attribName = _T("");
	else
		attribName = _T("name");

	return FindElemWithAttrib(_T("SubNetwork"),attribName,subNetworkName);
}

///
/// @brief
///     ��SubNetworkԪ���ڲ�����������apName��iedNameֵƥ���ConnectedAPԪ�أ�����λ����Ԫ��
///     ��apName��iedName��Ϊ�մ�����λ����һ��ConnectedAPԪ��
///
/// @param[in]
///     apName  - Ŀ��ConnectedAPԪ�ص�apName����ֵ��Ĭ��Ϊ�մ�
/// @param[in]
///     iedName - Ŀ��ConnectedAPԪ�ص�iedName����ֵ��Ĭ��Ϊ�մ�
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindConnectedAPInSubNetwork(SCL_STR apName /* = _T */, SCL_STR iedName /* = _T */)
{
	if ( apName == _T("") && iedName == _T("") )
	{
		return m_SCLFile.FindElem(_T("ConnectedAP"));
	}

	while (1)
	{
		if ( !m_SCLFile.FindElem(_T("ConnectedAP")) )
			return false;
		if ( apName == m_SCLFile.GetAttrib(_T("apName")) &&
		     iedName  == m_SCLFile.GetAttrib(_T("iedName")) )
			return true;
	}

	return false;
}

///
/// @brief
///     ��ConnnectedAPԪ���ڲ�����AddressԪ�أ�����λ����Ԫ��
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindAddressInConnnectedAP()
{
	return FindElemWithAttrib(_T("Address"),_T(""),_T(""));
}

///
/// @brief
///     ��ConnectedAPԪ���ڲ�����������cbName��ldInstֵƥ���SMVԪ�أ�����λ����Ԫ��
///     ��cbName��ldInst��Ϊ�մ�����λ����һ��SMVԪ��
///
/// @param[in]
///     cbName - Ŀ��SMVԪ�ص�cbName����ֵ��Ĭ��Ϊ�մ�
/// @param[in]
///     ldInst - Ŀ��SMVԪ�ص�ldInst����ֵ��Ĭ��Ϊ�մ�
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindSMVInConnectedAP(SCL_STR cbName /* = _T */, SCL_STR ldInst /* = _T */)
{
	if ( cbName == _T("") && ldInst == _T("") )
	{
		return m_SCLFile.FindElem(_T("SMV"));
	}

	while (1)
	{
		if ( !m_SCLFile.FindElem(_T("SMV")) )
			return false;
		if ( cbName == m_SCLFile.GetAttrib(_T("cbName")) &&
		     ldInst  == m_SCLFile.GetAttrib(_T("ldInst")) )
			return true;
	}

	return false;
}

///
/// @brief
///     ��ConnectedAPԪ���ڲ�����������cbName��ldInstֵƥ���GSEԪ�أ�����λ����Ԫ��
///     ��cbName��ldInst��Ϊ�մ�����λ����һ��GSEControlԪ��
///
/// @param[in]
///     cbName - Ŀ��GSEԪ�ص�cbName����ֵ��Ĭ��Ϊ�մ�
/// @param[in]
///     ldInst - Ŀ��GSEԪ�ص�ldInst����ֵ��Ĭ��Ϊ�մ�
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindGSEInConnectedAP(SCL_STR cbName /* = _T */, SCL_STR ldInst /* = _T */)
{
	if ( cbName == _T("") && ldInst == _T("") )
	{
		return m_SCLFile.FindElem(_T("GSE"));
	}

	while (1)
	{
		if ( !m_SCLFile.FindElem(_T("GSE")) )
			return false;
		if ( cbName == m_SCLFile.GetAttrib(_T("cbName")) &&
			ldInst  == m_SCLFile.GetAttrib(_T("ldInst")) )
			return true;
	}

	return false;
}

///
/// @brief
///     ��SCLԪ���ڲ�����DataTypeTemplatesԪ�أ�����λ����Ԫ��
///     һ��SCLԪ����ֻ��һ��DataTypeTemplatesԪ��
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindDataTypeTmpltInSCL()
{
	return FindElemWithAttrib(_T("DataTypeTemplates"), _T(""), _T(""));
}

///
/// @brief
///     ��DataTypeTemplatesԪ���ڲ�����������idֵƥ���LNodeTypeԪ�أ�����λ����Ԫ��
///     ��idΪ�մ�����λ����һ��LNodeTypeԪ��
///
/// @param[in]
///     id - Ŀ��LNodeTypeԪ�ص�id����ֵ��Ĭ��Ϊ�մ�
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindLNTypeInDataTypeTmplt(SCL_STR id /* = _T */)
{
	SCL_CSTR attribName;

	if ( id == _T("") )
		attribName = _T("");
	else
		attribName = _T("id");

	return FindElemWithAttrib(_T("LNodeType"),attribName,id);
}

///
/// @brief
///     ��LNodeTypeԪ���ڲ�����������nameֵƥ���DOԪ�أ�����λ����Ԫ��
///     ��nameΪ�մ�����λ����һ��DOԪ��
///
/// @param[in]
///     name - Ŀ��DOԪ�ص�name����ֵ��Ĭ��Ϊ�մ�
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindDOInLNType(SCL_STR doName /* = _T */)
{
	SCL_CSTR attribName;

	if ( doName == _T("") )
		attribName = _T("");
	else
		attribName = _T("name");

	return FindElemWithAttrib(_T("DO"),attribName,doName);
}

///
/// @brief
///     ��DataTypeTemplatesԪ���ڲ�����������idֵƥ���DOTypeԪ�أ�����λ����Ԫ��
///     ��idΪ�մ�����λ����һ��DOTypeԪ��
///
/// @param[in]
///     id - Ŀ��DOTypeԪ�ص�id����ֵ��Ĭ��Ϊ�մ�
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindDOTypeInDataTypeTmplt(SCL_STR id /* = _T */)
{
	SCL_CSTR attribName;

	if ( id == _T("") )
		attribName = _T("");
	else
		attribName = _T("id");

	return FindElemWithAttrib(_T("DOType"),attribName,id);
}

///
/// @brief
///     ��DOTypeԪ���ڲ�����������nameֵƥ���DAԪ�أ�����λ����Ԫ��
///     ��nameΪ�մ�����λ����һ��DAԪ��
///
/// @param[in]
///     name - Ŀ��DAԪ�ص�name����ֵ��Ĭ��Ϊ�մ�
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindDAInDOType(SCL_STR daName /* = _T */)
{
	SCL_CSTR attribName;

	if ( daName == _T("") )
		attribName = _T("");
	else
		attribName = _T("name");

	return FindElemWithAttrib(_T("DA"),attribName,daName);
}

///
/// @brief
///     ��DataTypeTemplatesԪ���ڲ�����������idֵƥ���DATypeԪ�أ�����λ����Ԫ��
///     ��idΪ�մ�����λ����һ��DATypeԪ��
///
/// @param[in]
///     id - Ŀ��DATypeԪ�ص�id����ֵ��Ĭ��Ϊ�մ�
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindDATypeInDataTypeTmplt(SCL_STR id /* = _T */)
{
	SCL_CSTR attribName;

	if ( id == _T("") )
		attribName = _T("");
	else
		attribName = _T("id");

	return FindElemWithAttrib(_T("DAType"),attribName,id);
}

///
/// @brief
///     ��DATypeԪ���ڲ�����������nameֵƥ���BDAԪ�أ�����λ����Ԫ��
///     ��nameΪ�մ�����λ����һ��BDAԪ��
///
/// @param[in]
///     name - Ŀ��BDAԪ�ص�name����ֵ��Ĭ��Ϊ�մ�
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindBDAInDAType(SCL_STR name /* = _T */)
{
	SCL_CSTR attribName;

	if ( name == _T("") )
		attribName = _T("");
	else
		attribName = _T("name");

	return FindElemWithAttrib(_T("BDA"),attribName,name);
}

///
/// @brief
///     ��DataTypeTemplatesԪ���ڲ�����������idֵƥ���EnumTypeԪ�أ�����λ����Ԫ��
///     ��idΪ�մ�����λ����һ��EnumTypeԪ��
///
/// @param[in]
///     id - Ŀ��EnumTypeԪ�ص�id����ֵ��Ĭ��Ϊ�մ�
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ��
///
bool SCLTools::FindEnumTypeInDataTypeTmplt(SCL_STR id /* = _T */)
{
	SCL_CSTR attribName;

	if ( id == _T("") )
		attribName = _T("");
	else
		attribName = _T("id");

	return FindElemWithAttrib(_T("EnumType"),attribName,id);
}

///
/// @brief
///     ��ȡ��ǰԪ�صı�ǩ��
///
/// @return
///     ��ǰԪ�صı�ǩ��
///
SCL_STR SCLTools::GetElemTag()
{
	return m_SCLFile.GetTagName();
}

///
/// @brief
///     ��ȡ��ǰIEDԪ�ص����ԣ����ȶ�λ��IEDԪ��
///
/// @param[in]
///     iedAttrib - ����IED������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetIEDAttrib(SCL_IED_ATTRIB &iedAttrib)
{
	// ��ǰ�㼶����IED�򷵻�false
	if (m_SCLFile.GetTagName() != _T("IED"))
		return false;
	
	iedAttrib.iedName = m_SCLFile.GetAttrib(_T("name"));
	iedAttrib.iedDesc = m_SCLFile.GetAttrib(_T("desc"));
	iedAttrib.manufactactuer = m_SCLFile.GetAttrib(_T("manufacturer"));
	iedAttrib.configVersion = m_SCLFile.GetAttrib(_T("configVersion"));

	return true;
}

///
/// @brief
///     ��ȡ��ǰAccessPointԪ�ص����ԣ����ȶ�λ��AccessPointԪ��
///
/// @param[in]
///     apAttrib - ����AccessPoint������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetAccessPointAttrib(SCL_ACCESS_POINT_ATTRIB &apAttrib)
{
	// ��ǰ�㼶����AccessPoint�򷵻�false
	if (m_SCLFile.GetTagName() != _T("AccessPoint"))
		return false;

	apAttrib.apDesc = m_SCLFile.GetAttrib(_T("desc"));
	apAttrib.apName = m_SCLFile.GetAttrib(_T("name"));

	if ( m_SCLFile.GetAttrib(_T("clock")) == _T("true") )
		apAttrib.clock = true;
	else
		apAttrib.clock = false;
	if ( m_SCLFile.GetAttrib(_T("router")) == _T("true") )
		apAttrib.router = true;
	else
		apAttrib.router = false;

	return true;
}

///
/// @brief
///     ��ȡ��ǰServerԪ�ص����ԣ����ȶ�λ��ServerԪ��
///
/// @param[in]
///     serverAttrib - ����Server������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetServerAttrib(SCL_SERVER_ATTRIB &serverAttrib)
{
	// ��ǰ�㼶����AccessPoint�򷵻�false
	if (m_SCLFile.GetTagName() != _T("AccessPoint"))
		return false;

	serverAttrib.serverDesc = m_SCLFile.GetAttrib(_T("desc"));
	serverAttrib.timeout    = m_SCLFile.GetAttrib(_T("timeout"));

	return true;
}

///
/// @brief
///     ��ȡ��ǰLDeviceԪ�ص����ԣ����ȶ�λ��LDeviceԪ��
///
/// @param[in]
///     ldAttrib - ����LDevice������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetLDeviceAttrib(SCL_LDEVICE_ATTRIB &ldAttrib)
{
	// ��ǰ�㼶����LDevice�򷵻�false
	if (m_SCLFile.GetTagName() != _T("LDevice"))
		return false;

	ldAttrib.ldDesc = m_SCLFile.GetAttrib(_T("desc"));
	ldAttrib.ldInst = m_SCLFile.GetAttrib(_T("inst"));

	return true;
}

///
/// @brief
///     ��ȡ��ǰLN0Ԫ�ص����ԣ����ȶ�λ��LN0Ԫ��
///
/// @param[in]
///     ln0Attrib - ����LN0������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetLN0Attrib(SCL_LN0_ATTRIB &ln0Attrib)
{
	// ��ǰ�㼶����LN0�򷵻�false
	if (m_SCLFile.GetTagName() != _T("LN0"))
		return false;

	ln0Attrib.lnClass = m_SCLFile.GetAttrib(_T("lnClass"));
	ln0Attrib.lnInst  = m_SCLFile.GetAttrib(_T("inst"));
	ln0Attrib.lnType  = m_SCLFile.GetAttrib(_T("lnType"));
	ln0Attrib.lnDesc  = m_SCLFile.GetAttrib(_T("desc"));

	return true;
}

///
/// @brief
///     ��ȡ��ǰLNԪ�ص����ԣ����ȶ�λ��LNԪ��
///
/// @param[in]
///     lnAttrib - ����LN������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetLNAttrib(SCL_LN_ATTRIB &lnAttrib)
{
	// ��ǰ�㼶����LN�򷵻�false
	if (m_SCLFile.GetTagName() != _T("LN"))
		return false;

	lnAttrib.lnPrefix = m_SCLFile.GetAttrib(_T("prefix"));
	lnAttrib.lnClass  = m_SCLFile.GetAttrib(_T("lnClass"));
	lnAttrib.lnInst   = m_SCLFile.GetAttrib(_T("inst"));
	lnAttrib.lnType   = m_SCLFile.GetAttrib(_T("lnType"));
	lnAttrib.lnDesc   = m_SCLFile.GetAttrib(_T("desc"));

	return true;
}

///
/// @brief
///     ��ȡ��ǰDataSetԪ�ص����ԣ����ȶ�λ��DataSetԪ��
///
/// @param[in]
///     dataSetAttrib - ����DataSet������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetDataSetAttrib(SCL_DATASET_ATTRIB &datSetAttrib)
{
	// ��ǰ�㼶����DataSet�򷵻�false
	if (m_SCLFile.GetTagName() != _T("DataSet"))
		return false;

	// ��ȡDataSet��FCDA����Ŀ
	int i = 0;
	while(m_SCLFile.FindChildElem(_T("FCDA")))
		i++;

	datSetAttrib.dataSetName = m_SCLFile.GetAttrib(_T("name"));
	datSetAttrib.dataSetDesc = m_SCLFile.GetAttrib(_T("desc"));
	datSetAttrib.fcdaNum     = i;

	m_SCLFile.ResetChildPos();

	return true;
}

///
/// @brief
///     ��ȡDataSet��ĳһ˳��ŵ�FCDAԪ�ص����ԣ����ȶ�λ��DataSetԪ��
///
/// @param[in]
///     fcdaAttrib - ����FCDA������Ϣ�Ľṹ��
/// @param[in]
///     idxSeq     - ��FCDAԪ����DataSetԪ���е�˳��ţ���0��ʼ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetOneFCDAAttrib(SCL_FCDA_ATTRIB &fcdaAttrib, int idxSeq)
{
	// ��ǰ�㼶����DataSet�򷵻�false
	if (m_SCLFile.GetTagName() != _T("DataSet"))
		return false;
	
	m_SCLFile.ResetChildPos();

	int i = 0;

	while( m_SCLFile.FindChildElem(_T("FCDA")) )
	{
		if ( i == idxSeq )
		{
			fcdaAttrib.idxSeq   = idxSeq;
			fcdaAttrib.ldInst   = m_SCLFile.GetChildAttrib(_T("ldInst"));
			fcdaAttrib.lnPrefix = m_SCLFile.GetChildAttrib(_T("prefix"));
			fcdaAttrib.lnClass  = m_SCLFile.GetChildAttrib(_T("lnClass"));
			fcdaAttrib.lnInst   = m_SCLFile.GetChildAttrib(_T("lnInst"));
			fcdaAttrib.doName   = m_SCLFile.GetChildAttrib(_T("doName"));
			fcdaAttrib.daName   = m_SCLFile.GetChildAttrib(_T("daName"));
			fcdaAttrib.fc       = m_SCLFile.GetChildAttrib(_T("fc"));
			return true;
		}
		i++;
	}

	return false;
}

///
/// @brief
///     ��ȡDOIԪ�ص����ԣ����ȶ�λ��DOIԪ��
///
/// @param[in]
///     doiAttrib - ����DOI������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetDOIAttrib(SCL_DOI_ATTRIB &doiAttrib)
{
	// ��ǰ�㼶����DOI�򷵻�false
	if (m_SCLFile.GetTagName() != _T("DOI"))
		return false;

	doiAttrib.doName        = m_SCLFile.GetAttrib(_T("name"));
	doiAttrib.doDesc        = m_SCLFile.GetAttrib(_T("desc"));
// 	doiAttrib.accessControl = m_SCLFile.GetAttrib(_T("accessControl"));
// 	doiAttrib.ix            = m_SCLFile.GetAttrib(_T("ix"));

	if ( !m_SCLFile.IntoElem() )
		return false;

	// ��ȡname=��dU����DAI����Ϣ
	if ( FindElemWithAttrib(_T("DAI"),_T("name"),_T("dU")) )
	{
		doiAttrib.dUSAddr = m_SCLFile.GetAttrib(_T("sAddr"));
		if ( m_SCLFile.FindChildElem(_T("Val")) )
		{
			doiAttrib.dUVal = m_SCLFile.GetChildData();
		}
	}
	else
	{
		doiAttrib.dUSAddr = _T("");
		doiAttrib.dUVal   = _T("");
	}

	m_SCLFile.OutOfElem();

	return true;
}

///
/// @brief
///     ��ȡDAIԪ�ص����ԣ����ȶ�λ��DAIԪ��
///
/// @param[in]
///     daiAttrib - ����DAI������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetDAIAttrib(SCL_DAI_ATTRIB &daiAttrib)
{
	// ��ǰ�㼶����DAI�򷵻�false
	if (m_SCLFile.GetTagName() != _T("DAI"))
		return false;

	daiAttrib.daName = m_SCLFile.GetAttrib(_T("name"));
	daiAttrib.daDesc = m_SCLFile.GetAttrib(_T("desc"));
	daiAttrib.sAddr  = m_SCLFile.GetAttrib(_T("sAddr"));

	return true;
}

///
/// @brief
///     ��ȡ��ǰExtRefԪ�ص����ԣ����ȶ�λ��ExtRefԪ��
///
/// @param[in]
///     extRefAttrib - ����ExtRef������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetExtRefAttrib(SCL_EXTREF_ATTRIB &extRefAttrib)
{
	// ��ǰ�㼶����ExtRef�򷵻�false
	if (m_SCLFile.GetTagName() != _T("ExtRef"))
		return false;

	extRefAttrib.desc     = m_SCLFile.GetAttrib(_T("desc"));
	extRefAttrib.iedName  = m_SCLFile.GetAttrib(_T("iedName"));
	extRefAttrib.ldInst   = m_SCLFile.GetAttrib(_T("ldInst"));
	extRefAttrib.lnPrefix = m_SCLFile.GetAttrib(_T("prefix"));
	extRefAttrib.lnClass  = m_SCLFile.GetAttrib(_T("lnClass"));
	extRefAttrib.lnInst   = m_SCLFile.GetAttrib(_T("lnInst"));
	extRefAttrib.doName   = m_SCLFile.GetAttrib(_T("doName"));
	extRefAttrib.daName   = m_SCLFile.GetAttrib(_T("daName"));
	extRefAttrib.intAddr  = m_SCLFile.GetAttrib(_T("intAddr"));

	return true;
}

///
/// @brief
///     ��ȡ��ǰSmapledValueControlԪ�ص����ԣ����ȶ�λ��SmapledValueControlԪ��
///
/// @param[in]
///     smvCtrlAttrib - ����SmapledValueControl������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetSMVCtrlAttrib(SCL_SMVCTRL_ATTRIB &smvCtrlAttrib)
{
	// ��ǰ�㼶����SampledValueControl�򷵻�false
	if (m_SCLFile.GetTagName() != _T("SampledValueControl"))
		return false;

	smvCtrlAttrib.cbName    = m_SCLFile.GetAttrib(_T("name"));
	smvCtrlAttrib.cbDesc    = m_SCLFile.GetAttrib(_T("desc"));
	smvCtrlAttrib.confRev   = m_SCLFile.GetAttrib(_T("confRev"));
	smvCtrlAttrib.smvID     = m_SCLFile.GetAttrib(_T("smvID"));
	smvCtrlAttrib.dataSet   = m_SCLFile.GetAttrib(_T("datSet"));
	smvCtrlAttrib.smpRate   = m_SCLFile.GetAttrib(_T("smpRate"));
	smvCtrlAttrib.nofASDU   = m_SCLFile.GetAttrib(_T("nofASDU"));

	// multicast��SCL�ļ���Ĭ��Ϊtrue
	if ( m_SCLFile.GetAttrib(_T("multicast")) == _T("false") )
		smvCtrlAttrib.multicast = false;
	else
		smvCtrlAttrib.multicast = true;

	// SmvOpts��SCL�ļ���Ĭ��Ϊfalse
	if ( m_SCLFile.FindChildElem(_T("SmvOpts")) )
	{
		if ( m_SCLFile.GetChildAttrib(_T("refreshTime")) == _T("true") )
			smvCtrlAttrib.smvOptReftTm = true;
		else
			smvCtrlAttrib.smvOptReftTm = false;

		if ( m_SCLFile.GetChildAttrib(_T("sampleRate")) == _T("true") )
			smvCtrlAttrib.smvOptSmpRate = true;
		else
			smvCtrlAttrib.smvOptSmpRate = false;

		if ( m_SCLFile.GetChildAttrib(_T("sampleSynchronized")) == _T("true") )
			smvCtrlAttrib.smvOptSmpSync = true;
		else
			smvCtrlAttrib.smvOptSmpSync = false;

		if ( m_SCLFile.GetChildAttrib(_T("security")) == _T("true") )
			smvCtrlAttrib.smvOptSecurity = true;
		else
			smvCtrlAttrib.smvOptSecurity = false;

		if ( m_SCLFile.GetChildAttrib(_T("dataRef")) ==_T("true") )
			smvCtrlAttrib.smvOptDataRef = true;
		else
			smvCtrlAttrib.smvOptDataRef = false;
	}
	else
	{
		smvCtrlAttrib.smvOptReftTm   = false;
		smvCtrlAttrib.smvOptSmpRate  = false;
		smvCtrlAttrib.smvOptSmpSync  = false;
		smvCtrlAttrib.smvOptSecurity = false;
		smvCtrlAttrib.smvOptDataRef  = false;
	}
	
	return true;
}

///
/// @brief
///     ��ȡ��ǰGSEControlԪ�ص����ԣ����ȶ�λ��GSEControlԪ��
///
/// @param[in]
///     gseCtrlAttrib - ����GSEControl������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetGSECtrlAttrib(SCL_GSECTRL_ATTRIB &gseCtrlAttrib)
{
	// ��ǰ�㼶����GSEControl�򷵻�false
	if (m_SCLFile.GetTagName() != _T("GSEControl"))
		return false;

	gseCtrlAttrib.cbName  = m_SCLFile.GetAttrib(_T("name"));
	gseCtrlAttrib.cbDesc  = m_SCLFile.GetAttrib(_T("desc"));
	gseCtrlAttrib.confRev = m_SCLFile.GetAttrib(_T("confRev"));
	gseCtrlAttrib.gooseID = m_SCLFile.GetAttrib(_T("appID"));
	gseCtrlAttrib.dataSet = m_SCLFile.GetAttrib(_T("datSet"));
	// typeĬ��ΪGOOSE
	if ( m_SCLFile.GetAttrib(_T("type")).compare(_T("GSSE")) == 0 )
		gseCtrlAttrib.type = _T("GSSE");
	else
		gseCtrlAttrib.type = _T("GOOSE");

	return true;
}

///
/// @brief
///     ��ȡSubNetworkԪ�ص�������Ϣ�����ȶ�λ��SubNetworkԪ��
///
/// @param[in]
///     subNetAttrib - ����SubNetworkԪ�ص�������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetSubNetworkAttrib(SCL_SUBNETWORK_ATTRIB &subNetAttrib)
{
	// ��ǰ�㼶����ConnectedAP�򷵻�false
	if (m_SCLFile.GetTagName() != _T("SubNetwork"))
		return false;

	subNetAttrib.name = m_SCLFile.GetAttrib(_T("name"));
	subNetAttrib.type = m_SCLFile.GetAttrib(_T("type"));

	return true;
}

///
/// @brief
///     ��ȡConnectedAPԪ�ص�������Ϣ������ϢΪ��ĳ�����ϵķ��ʵ����ã����ȶ�λ��ConnectedAPԪ��
///
/// @param[in]
///     connectedAPAttrib - ����ConnectedAPԪ�ص�������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetConnectedAPAttrib(SCL_CONNECTEDAP_ATTRIB &connectedAPAttrib)
{
	// ��ǰ�㼶����ConnectedAP�򷵻�false
	if (m_SCLFile.GetTagName() != _T("ConnectedAP"))
		return false;

	connectedAPAttrib.iedName = m_SCLFile.GetAttrib(_T("iedName"));
	connectedAPAttrib.apName  = m_SCLFile.GetAttrib(_T("apName"));

	return true;
}

///
/// @brief
///     ��ȡConnectedAPԪ����AddressԪ�ص���Ϣ������ϢΪ�ͻ�/����������ĵ�ַ��Ϣ�����ȶ�λ��AddressԪ��
///
/// @param[in]
///     addrAtttrib - ����ConnectedAPԪ����AddressԪ�ص���Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetAddrAttrib(SCL_ADDR_ATTRIB &addrAtttrib)
{
	// ��ǰ�㼶����Address�򷵻�false
	if (m_SCLFile.GetTagName() != _T("Address"))
		return false;

	while( m_SCLFile.FindChildElem(_T("P")) )
	{
		SCL_STR typeVal = m_SCLFile.GetChildAttrib(_T("type"));
		SCL_STR pData   = m_SCLFile.GetChildData();
		if ( typeVal == _T("IP") )
			addrAtttrib.ip = pData;
		else if ( typeVal == _T("IP-SUBNET") )
			addrAtttrib.ipSubnet = pData;
		else if ( typeVal == _T("IP-GATEWAY") )
			addrAtttrib.ipGateway = pData;
		else if ( typeVal == _T("OSI-NSAP") )
			addrAtttrib.osiNsap = pData;
		else if ( typeVal == _T("OSI-TSEL") )
			addrAtttrib.osiTsel = pData;
		else if ( typeVal == _T("OSI-SSEL") )
			addrAtttrib.osiSsel = pData;
		else if ( typeVal == _T("OSI-PSEL") )
			addrAtttrib.osiPsel = pData;
		else if ( typeVal == _T("OSI-AP-Title"))
			addrAtttrib.osiApTitle = pData;
		else if ( typeVal == _T("OSI-AP-Invoke"))
			addrAtttrib.osiApInvoke = pData;
		else if ( typeVal == _T("OSI-AE-Qualifier"))
			addrAtttrib.osiAeQualifier = pData;
		else if ( typeVal == _T("OSI-AE-Invoke"))
			addrAtttrib.osiAeInvoke = pData;
	}

	return true;
}

///
/// @brief
///     ��ȡConnectedAPԪ����SMVԪ�ص���Ϣ��ΪSMV��������ͨ�ŵ�ַ������ƿ����ã����ȶ�λ��SMVԪ��
///
/// @param[in]
///     smvAttrib - ����ConnectedAPԪ����SMV��Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetSMVAttrib(SCL_SMV_ATTRIB &smvAttrib)
{
	// ��ǰ�㼶����SMV�򷵻�false
	if (m_SCLFile.GetTagName() != _T("SMV"))
		return false;

	smvAttrib.ldInst = m_SCLFile.GetAttrib(_T("ldInst"));
	smvAttrib.cbName = m_SCLFile.GetAttrib(_T("cbName"));

	if ( !m_SCLFile.IntoElem() )
		return false;

	// SMV-->Address����ȡ�涨�ĵ�ַ��Ϣ
	if ( !m_SCLFile.FindElem(_T("Address")) )
		return false;
	while( m_SCLFile.FindChildElem(_T("P")) )
	{
		SCL_STR typeVal = m_SCLFile.GetChildAttrib(_T("type"));
		SCL_STR pData   = m_SCLFile.GetChildData();
		if ( typeVal == _T("MAC-Address") )
			smvAttrib.macAddr = pData;
		else if ( typeVal == _T("APPID") )
			smvAttrib.appID = pData;
		else if ( typeVal == _T("VLAN-ID") )
			smvAttrib.vlanID = pData;
		else if ( typeVal == _T("VLAN-PRIORITY") )
			smvAttrib.vlanPriority = pData;
	}
	
	// �ص�SMV
	m_SCLFile.OutOfElem();

	return true;
}

///
/// @brief
///     ��ȡConnectedAPԪ����GSEԪ�ص���Ϣ��ΪGOOSE/GSSE��������ͨ�ŵ�ַ������ƿ����ã����ȶ�λ��GSEԪ��
///
/// @param[in]
///     gseAttrib - ����ConnectedAPԪ����GSE��Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetGSEAttrib(SCL_GSE_ATTRIB &gseAttrib)
{
	// ��ǰ�㼶����GSE�򷵻�false
	if (m_SCLFile.GetTagName() != _T("GSE"))
		return false;

	gseAttrib.ldInst = m_SCLFile.GetAttrib(_T("ldInst"));
	gseAttrib.cbName = m_SCLFile.GetAttrib(_T("cbName"));

	if ( !m_SCLFile.IntoElem() )
		return false;

	// GSE-->Address����ȡ�涨�ĵ�ַ��Ϣ
	if ( !m_SCLFile.FindElem(_T("Address")) )
		return false;
	while( m_SCLFile.FindChildElem(_T("P")) )
	{
		SCL_STR typeVal = m_SCLFile.GetChildAttrib(_T("type"));
		SCL_STR pData   = m_SCLFile.GetChildData();
		if ( typeVal == _T("MAC-Address") )
			gseAttrib.macAddr = pData;
		else if ( typeVal == _T("APPID") )
			gseAttrib.appID = pData;
		else if ( typeVal == _T("VLAN-ID") )
			gseAttrib.vlanID = pData;
		else if ( typeVal == _T("VLAN-PRIORITY") )
			gseAttrib.vlanPriority = pData;
	}

	// GSE-->MinTime����ȡGOOSE���ݱ�λ������������ʱ����
	m_SCLFile.ResetMainPos();
	if ( m_SCLFile.FindElem(_T("MinTime")) )
	{
		gseAttrib.minTimeVal   = m_SCLFile.GetData();
		gseAttrib.minTimeUnit  = m_SCLFile.GetAttrib(_T("multiplier"));
		gseAttrib.minTimeUnit += m_SCLFile.GetAttrib(_T("unit"));
	}

	// GSE-->MaxTime����ȡGOOSE����ʱ��
	m_SCLFile.ResetMainPos();
	if ( m_SCLFile.FindElem(_T("MaxTime")) )
	{
		gseAttrib.maxTimeVal   = m_SCLFile.GetData();
		gseAttrib.maxTimeUnit  = m_SCLFile.GetAttrib(_T("multiplier"));
		gseAttrib.maxTimeUnit += m_SCLFile.GetAttrib(_T("unit"));
	}

	// �ص�GSE
	m_SCLFile.OutOfElem();

	return true;
}

///
/// @brief
///     ��ȡLNodeTypeԪ�ص�������Ϣ�����ȶ�λ��LNodeTypeԪ��
///
/// @param[in]
///     lnTypeAttrib - ����LNodeTypeԪ�ص�������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetLNTypeAttrib(SCL_LNTYPE_ATTRIB &lnTypeAttrib)
{
	// ��ǰ�㼶����LNodeType�򷵻�false
	if (m_SCLFile.GetTagName() != _T("LNodeType"))
		return false;

	lnTypeAttrib.id      = m_SCLFile.GetAttrib(_T("id"));
	lnTypeAttrib.desc    = m_SCLFile.GetAttrib(_T("desc"));
	lnTypeAttrib.lnClass = m_SCLFile.GetAttrib(_T("lnClass"));

	return true;
}

///
/// @brief
///     ��ȡLNodeType��ĳһDOԪ�ص�������Ϣ�����ȶ�λ��DOԪ��
///
/// @param[in]
///     doAttrib - ����DOԪ�ص�������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetDOAttrib(SCL_DOINLNTYPE_ATTRIB &doAttrib)
{
	// ��ǰ�㼶����DO�򷵻�false
	if (m_SCLFile.GetTagName() != _T("DO"))
		return false;

	doAttrib.name = m_SCLFile.GetAttrib(_T("name"));
	doAttrib.type = m_SCLFile.GetAttrib(_T("type"));
	doAttrib.desc = m_SCLFile.GetAttrib(_T("desc"));

	return true;
}

///
/// @brief
///     ��ȡDOTypeԪ�ص�������Ϣ�����ȶ�λ��DOTypeԪ��
///
/// @param[in]
///     doTypeAttrib - ����DOTypeԪ�ص�������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetDOTypeAttrib(SCL_DOTYPE_ATTRIB &doTypeAttrib)
{
	// ��ǰ�㼶����DOType�򷵻�false
	if (m_SCLFile.GetTagName() != _T("DOType"))
		return false;

	doTypeAttrib.id  = m_SCLFile.GetAttrib(_T("id"));
	doTypeAttrib.cdc = m_SCLFile.GetAttrib(_T("cdc"));

	return true;
}

///
/// @brief
///     ��ȡDOType��ĳһDAԪ�ص�������Ϣ�����ȶ�λ��DAԪ��
///
/// @param[in]
///     daAttrib - ����DAԪ�ص�������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetDAAttrib(SCL_DAINDOTYPE_ATTRIB &daAttrib)
{
	// ��ǰ�㼶����DA�򷵻�false
	if (m_SCLFile.GetTagName() != _T("DA"))
		return false;

	daAttrib.name  = m_SCLFile.GetAttrib(_T("name"));
	daAttrib.desc  = m_SCLFile.GetAttrib(_T("desc"));
	daAttrib.fc    = m_SCLFile.GetAttrib(_T("fc"));
	daAttrib.bType = m_SCLFile.GetAttrib(_T("bType"));
	daAttrib.type  = m_SCLFile.GetAttrib(_T("type"));

	return true;
}

///
/// @brief
///     ��ȡDAType��������Ϣ�����ȶ�λ��DATypeԪ��
///
/// @param[in]
///     daTypeAttrib - ����DATypeԪ�ص�������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetDATypeAttrib(SCL_DATYPE_ATTRIB &daTypeAttrib)
{
	// ��ǰ�㼶����DA�򷵻�false
	if (m_SCLFile.GetTagName() != _T("DAType"))
		return false;

	daTypeAttrib.id  = m_SCLFile.GetAttrib(_T("id"));
	daTypeAttrib.desc  = m_SCLFile.GetAttrib(_T("desc"));

	return true;
}

///
/// @brief
///     ��ȡDAType��������Ϣ�����ȶ�λ��DATypeԪ��
///
/// @param[in]
///     daAttrib - ����DATypeԪ�ص�������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetBDAAttrib(SCL_BDAINDATYPE_ATTRIB &bdaAttrib)
{
	// ��ǰ�㼶����DA�򷵻�false
	if (m_SCLFile.GetTagName() != _T("BDA"))
		return false;

	bdaAttrib.name  = m_SCLFile.GetAttrib(_T("name"));
	bdaAttrib.bType = m_SCLFile.GetAttrib(_T("bType"));
	bdaAttrib.type  = m_SCLFile.GetAttrib(_T("type"));

	return true;
}

///
/// @brief
///     ��ȡEnumType��ĳһEnumValԪ�ص�������Ϣ�����ȶ�λ��EnumTypeԪ��
///
/// @param[in]
///     enumValAttrib - ����EnumValԪ�ص�������Ϣ�Ľṹ��
/// @param[in]
///     ord           - EnumVal�ı�ǩֵ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetEnumValAttrib(SCL_ENUMVAL_ATTRIB &enumValAttrib, SCL_STR ord)
{
	// ��ǰ�㼶����EnumType�򷵻�false
	if (m_SCLFile.GetTagName() != _T("EnumType"))
		return false;

	enumValAttrib.id  = m_SCLFile.GetAttrib(_T("id"));
	enumValAttrib.desc  = m_SCLFile.GetAttrib(_T("desc"));

	if (!m_SCLFile.IntoElem())
		return false;

	if (!FindElemWithAttrib(_T("EnumVal"),_T("ord"),ord))
		return false;

	enumValAttrib.ord    = m_SCLFile.GetAttrib(_T("ord"));
	enumValAttrib.enumVal = m_SCLFile.GetData();

	if (!m_SCLFile.OutOfElem())
		return false;

	return true;
}

bool SCLTools::AddSCL()
{
	if ( !FindSCL() )
	{
		if ( !m_SCLFile.AddNode( CMarkup::MNT_PROCESSING_INSTRUCTION, _T("xml") ) )
			return false;
		m_SCLFile.SetAttrib(_T("version"), _T("1.0") );
		m_SCLFile.SetAttrib(_T("encoding"), _T("UTF-8") );
		m_SCLFile.SetAttrib(_T("standalone"), _T("no") );
		m_SCLFile.ResetPos();
		if ( !m_SCLFile.AddElem(_T("SCL"),_T(""),SCL_MARKUP_NODE_FLAG) )
			return false;
// 		m_SCLFile.SetAttrib(_T("xmlns"), _T("http://www.iec.ch/61850/2003/SCL") );
// 		m_SCLFile.SetAttrib(_T("xmlns:xsi"), _T("http://www.w3.org/2001/XMLSchema-instance") );
// 		m_SCLFile.SetAttrib(_T("xsi:schemaLocation"), _T("http://www.iec.ch/61850/2003/SCL SCL.xsd") );
	}
	return true;
}

bool SCLTools::AddIEDIntoSCL(const SCL_IED_ATTRIB &attrib)
{
	// ��ǰ�㼶����SCL�򷵻�false
	if (m_SCLFile.GetTagName() != _T("SCL"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("IED"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("name"), attrib.iedName );
	m_SCLFile.SetAttrib(_T("desc"), attrib.iedDesc );
//	m_SCLFile.SetAttrib(_T("configVersion"), attrib.configVersion );
//	m_SCLFile.SetAttrib(_T("manufacturer"), attrib.manufactactuer );
	return true;
}

bool SCLTools::AddAccessPointIntoIED(const SCL_ACCESS_POINT_ATTRIB &attrib)
{
	// ��ǰ�㼶����IED�򷵻�false
	if (m_SCLFile.GetTagName() != _T("IED"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("AccessPoint"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("name"), attrib.apName );
//	m_SCLFile.SetAttrib(_T("desc"), attrib.apDesc );
// 	if ( attrib.clock )
// 		m_SCLFile.SetAttrib(_T("clock"), _T("true") );
// 	else
// 		m_SCLFile.SetAttrib(_T("clock"), _T("false") );
// 	if ( attrib.router )
// 		m_SCLFile.SetAttrib(_T("router"), _T("true") );
// 	else
// 		m_SCLFile.SetAttrib(_T("router"), _T("false") );
	return true;
}

bool SCLTools::AddServerIntoAccessPoint()
{
	// ��ǰ�㼶����AccessPoint�򷵻�false
	if (m_SCLFile.GetTagName() != _T("AccessPoint"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("Server"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	return true;
}

bool SCLTools::AddLDeviceIntoServer(const SCL_LDEVICE_ATTRIB &attrib)
{
	// ��ǰ�㼶����Server�򷵻�false
	if (m_SCLFile.GetTagName() != _T("Server"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("LDevice"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("inst"), attrib.ldInst );
//	m_SCLFile.SetAttrib(_T("desc"), attrib.ldDesc );
	return true;
}

bool SCLTools::AddLN0IntoLDevice(const SCL_LN0_ATTRIB &attrib)
{
	// ��ǰ�㼶����LDevice�򷵻�false
	if (m_SCLFile.GetTagName() != _T("LDevice"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("LN0"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("desc"), attrib.lnDesc );
	m_SCLFile.SetAttrib(_T("inst"), attrib.lnInst );
	m_SCLFile.SetAttrib(_T("lnClass"), attrib.lnClass );
	m_SCLFile.SetAttrib(_T("lnType"), attrib.lnType );
	return true;
}

bool SCLTools::AddLNIntoLDevice(const SCL_LN_ATTRIB &attrib)
{
	// ��ǰ�㼶����LDevice�򷵻�false
	if (m_SCLFile.GetTagName() != _T("LDevice"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("LN"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("desc"), attrib.lnDesc );
	m_SCLFile.SetAttrib(_T("inst"), attrib.lnInst );
	m_SCLFile.SetAttrib(_T("lnClass"), attrib.lnClass );
	m_SCLFile.SetAttrib(_T("lnType"), attrib.lnType );
	m_SCLFile.SetAttrib(_T("prefix"), attrib.lnPrefix );
	return true;
}

bool SCLTools::AddDataSetIntoLN0(const SCL_DATASET_ATTRIB &attrib)
{
	// ��ǰ�㼶����LN0�򷵻�false
	if (m_SCLFile.GetTagName() != _T("LN0"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("DataSet"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("desc"), attrib.dataSetDesc );
	m_SCLFile.SetAttrib(_T("name"), attrib.dataSetName );
	return true;
}

bool SCLTools::AddFCDAIntoDataSet(const SCL_FCDA_ATTRIB &attrib)
{
	// ��ǰ�㼶����DataSet�򷵻�false
	if (m_SCLFile.GetTagName() != _T("DataSet"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("FCDA"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	if ( !attrib.daName.empty() )
		m_SCLFile.SetAttrib(_T("daName"), attrib.daName );
	m_SCLFile.SetAttrib(_T("doName"), attrib.doName );
	m_SCLFile.SetAttrib(_T("fc"), attrib.fc );
	m_SCLFile.SetAttrib(_T("ldInst"), attrib.ldInst );
	m_SCLFile.SetAttrib(_T("lnClass"), attrib.lnClass );
	m_SCLFile.SetAttrib(_T("lnInst"), attrib.lnInst );
	m_SCLFile.SetAttrib(_T("prefix"), attrib.lnPrefix );
	return true;
}

bool SCLTools::AddSMVCtrlIntoLN0(const SCL_SMVCTRL_ATTRIB &attrib)
{
	// ��ǰ�㼶����LN0�򷵻�false
	if (m_SCLFile.GetTagName() != _T("LN0"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("SampledValueControl"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;

	m_SCLFile.SetAttrib(_T("name"), attrib.cbName );
//	m_SCLFile.SetAttrib(_T("desc"), attrib.cbDesc );
	m_SCLFile.SetAttrib(_T("confRev"), attrib.confRev );
	m_SCLFile.SetAttrib(_T("datSet"), attrib.dataSet );
	m_SCLFile.SetAttrib(_T("smvID"), attrib.smvID );
	m_SCLFile.SetAttrib(_T("smpRate"), attrib.smpRate );
	m_SCLFile.SetAttrib(_T("nofASDU"), attrib.nofASDU );
	if ( !attrib.multicast )
		m_SCLFile.SetAttrib(_T("multicast"), _T("false") );

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("SmvOpts"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	if ( attrib.smvOptReftTm )
		m_SCLFile.SetAttrib(_T("refreshTime"), _T("true") );
	if ( attrib.smvOptSmpRate )
		m_SCLFile.SetAttrib(_T("sampleRate"), _T("true") );
	if ( attrib.smvOptSmpSync )
		m_SCLFile.SetAttrib(_T("sampleSynchronized"), _T("true") );
	if ( attrib.smvOptDataRef )
		m_SCLFile.SetAttrib(_T("dataRef"), _T("true") );
	if ( attrib.smvOptSecurity )
		m_SCLFile.SetAttrib(_T("security"), _T("true") );
	m_SCLFile.OutOfElem();

	return true;
}

bool SCLTools::AddGSECtrlIntoLN0(const SCL_GSECTRL_ATTRIB &attrib)
{
	// ��ǰ�㼶����LN0�򷵻�false
	if (m_SCLFile.GetTagName() != _T("LN0"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("GSEControl"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("name"), attrib.cbName );
//	m_SCLFile.SetAttrib(_T("desc"), attrib.cbDesc );
	m_SCLFile.SetAttrib(_T("confRev"), attrib.confRev );
	m_SCLFile.SetAttrib(_T("appID"), attrib.gooseID );
	m_SCLFile.SetAttrib(_T("datSet"), attrib.dataSet );
	m_SCLFile.SetAttrib(_T("type"), attrib.type );
	return true;
}

bool SCLTools::AddDOIIntoLNorLN0(const SCL_DOI_ATTRIB &attrib)
{
	// ��ǰ�㼶����LN0��LN�򷵻�false
	if (m_SCLFile.GetTagName() != _T("LN0") && m_SCLFile.GetTagName() != _T("LN"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("DOI"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("name"), attrib.doName );
	m_SCLFile.SetAttrib(_T("desc"), attrib.doDesc );

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("DAI"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("name"), _T("dU") );
	if ( !attrib.dUSAddr.empty() )
		m_SCLFile.SetAttrib(_T("sAddr"), attrib.dUSAddr );

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("Val"),attrib.dUVal,SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.OutOfElem();

	m_SCLFile.OutOfElem();
	return true;
}

bool SCLTools::AddInputsIntoLNorLN0()
{
	// ��ǰ�㼶����LN0�򷵻�false
	if (m_SCLFile.GetTagName() != _T("LN0") && m_SCLFile.GetTagName() != _T("LN"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("Inputs"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	return true;
}

bool SCLTools::AddExtRefIntoInputs(const SCL_EXTREF_ATTRIB &attrib)
{
	// ��ǰ�㼶����Inputs�򷵻�false
	if (m_SCLFile.GetTagName() != _T("Inputs"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("ExtRef"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	if ( !attrib.desc.empty() )
		m_SCLFile.SetAttrib(_T("desc"), attrib.desc );
	if ( !attrib.daName.empty() )
		m_SCLFile.SetAttrib(_T("daName"), attrib.daName );
	m_SCLFile.SetAttrib(_T("doName"), attrib.doName );
	m_SCLFile.SetAttrib(_T("iedName"), attrib.iedName );
	m_SCLFile.SetAttrib(_T("ldInst"), attrib.ldInst );
	m_SCLFile.SetAttrib(_T("lnClass"), attrib.lnClass );
	m_SCLFile.SetAttrib(_T("lnInst"), attrib.lnInst );
	m_SCLFile.SetAttrib(_T("prefix"), attrib.lnPrefix );
	m_SCLFile.SetAttrib(_T("intAddr"), attrib.intAddr );
	return true;
}

bool SCLTools::AddDAIIntoDOI(const SCL_DAI_ATTRIB &attrib)
{
	// ��ǰ�㼶����DOI�򷵻�false
	if (m_SCLFile.GetTagName() != _T("DOI"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("DAI"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("name"), attrib.daName );
//	m_SCLFile.SetAttrib(_T("desc"), attrib.daDesc );
	m_SCLFile.SetAttrib(_T("sAddr"), attrib.sAddr );
	return true;
}

bool SCLTools::AddCommnIntoSCL()
{
	// ��ǰ�㼶����SCL�򷵻�false
	if (m_SCLFile.GetTagName() != _T("SCL"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("Communication"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	return true;
}

bool SCLTools::AddSubNetworkIntoCommn(const SCL_SUBNETWORK_ATTRIB &attrib)
{
	// ��ǰ�㼶����Communication�򷵻�false
	if (m_SCLFile.GetTagName() != _T("Communication"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("SubNetwork"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("name"), attrib.name );
	m_SCLFile.SetAttrib(_T("type"), attrib.type );
	return true;
}

bool SCLTools::AddConnectedAPIntoSubNetwork(const SCL_CONNECTEDAP_ATTRIB &attrib)
{
	// ��ǰ�㼶����SubNetwork�򷵻�false
	if (m_SCLFile.GetTagName() != _T("SubNetwork"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("ConnectedAP"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("iedName"), attrib.iedName );
	m_SCLFile.SetAttrib(_T("apName"), attrib.apName );
	return true;
}

bool SCLTools::AddSMVIntoConnectedAP(const SCL_SMV_ATTRIB &attrib)
{
	// ��ǰ�㼶����ConnectedAP�򷵻�false
	if (m_SCLFile.GetTagName() != _T("ConnectedAP"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("SMV"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("cbName"), attrib.cbName );
	m_SCLFile.SetAttrib(_T("ldInst"), attrib.ldInst );

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("Address"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	if ( !m_SCLFile.AddChildElem(_T("P"),attrib.macAddr,SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetChildAttrib(_T("type"),_T("MAC-Address"));
	if ( !m_SCLFile.AddChildElem(_T("P"),attrib.appID,SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetChildAttrib(_T("type"),_T("APPID"));
	if ( !m_SCLFile.AddChildElem(_T("P"),attrib.vlanID,SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetChildAttrib(_T("type"),_T("VLAN-ID"));
	if ( !m_SCLFile.AddChildElem(_T("P"),attrib.vlanPriority,SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetChildAttrib(_T("type"),_T("VLAN-PRIORITY"));
	m_SCLFile.OutOfElem();

	return true;
}

bool SCLTools::AddGSEIntoConnectedAP(const SCL_GSE_ATTRIB &attrib)
{
	// ��ǰ�㼶����ConnectedAP�򷵻�false
	if (m_SCLFile.GetTagName() != _T("ConnectedAP"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("GSE"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("cbName"), attrib.cbName );
	m_SCLFile.SetAttrib(_T("ldInst"), attrib.ldInst );

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("Address"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;

	if ( !m_SCLFile.AddChildElem(_T("P"),attrib.macAddr,SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetChildAttrib(_T("type"),_T("MAC-Address"));

	if ( !m_SCLFile.AddChildElem(_T("P"),attrib.appID,SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetChildAttrib(_T("type"),_T("APPID"));

	if ( !m_SCLFile.AddChildElem(_T("P"),attrib.vlanID,SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetChildAttrib(_T("type"),_T("VLAN-ID"));

	if ( !m_SCLFile.AddChildElem(_T("P"),attrib.vlanPriority,SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetChildAttrib(_T("type"),_T("VLAN-PRIORITY"));

	if ( !m_SCLFile.AddElem(_T("MinTime"),attrib.minTimeVal,SCL_MARKUP_NODE_FLAG) )
		return false;
	size_t len = attrib.minTimeUnit.length();
	if ( len > 1 )
		m_SCLFile.SetAttrib(_T("multiplier"),attrib.minTimeUnit.substr(0,len-1));
	if ( len > 0 )
		m_SCLFile.SetAttrib(_T("unit"),attrib.minTimeUnit.substr(len-1,1));

	if ( !m_SCLFile.AddElem(_T("MaxTime"),attrib.maxTimeVal,SCL_MARKUP_NODE_FLAG) )
		return false;
	len = attrib.maxTimeUnit.length();
	if ( len > 1 )
		m_SCLFile.SetAttrib(_T("multiplier"),attrib.maxTimeUnit.substr(0,len-1));
	if ( len > 0 )
		m_SCLFile.SetAttrib(_T("unit"),attrib.maxTimeUnit.substr(len-1,1));
	m_SCLFile.OutOfElem();

	return true;
}

bool SCLTools::AddDataTypeTmpltIntoSCL()
{
	// ��ǰ�㼶����SCL�򷵻�false
	if (m_SCLFile.GetTagName() != _T("SCL"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("DataTypeTemplates"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	return true;
}

bool SCLTools::AddLNTypeIntoDataTypeTmplt(const SCL_LNTYPE_ATTRIB &attrib)
{
	// ��ǰ�㼶����DataTypeTemplates�򷵻�false
	if (m_SCLFile.GetTagName() != _T("DataTypeTemplates"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("LNodeType"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("id"), attrib.id );
//	m_SCLFile.SetAttrib(_T("desc"), attrib.desc );
	m_SCLFile.SetAttrib(_T("lnClass"), attrib.lnClass );
	return true;
}

bool SCLTools::AddDOIntoLNType(const SCL_DOINLNTYPE_ATTRIB &attrib)
{
	// ��ǰ�㼶����LNodeType�򷵻�false
	if (m_SCLFile.GetTagName() != _T("LNodeType"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("DO"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("name"), attrib.name );
//	m_SCLFile.SetAttrib(_T("desc"), attrib.desc );
	m_SCLFile.SetAttrib(_T("type"), attrib.type );
	return true;
}

bool SCLTools::AddDOTypeIntoDataTypeTmplt(const SCL_DOTYPE_ATTRIB &attrib)
{
	// ��ǰ�㼶����DataTypeTemplates�򷵻�false
	if (m_SCLFile.GetTagName() != _T("DataTypeTemplates"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("DOType"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("id"), attrib.id );
	m_SCLFile.SetAttrib(_T("cdc"), attrib.cdc );

	return true;
}

bool SCLTools::AddDAIntoDOType(const SCL_DAINDOTYPE_ATTRIB &attrib)
{
	// ��ǰ�㼶����DOType�򷵻�false
	if (m_SCLFile.GetTagName() != _T("DOType"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("DA"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("name"), attrib.name );
//	m_SCLFile.SetAttrib(_T("desc"), attrib.desc );
	m_SCLFile.SetAttrib(_T("fc"), attrib.fc );
	m_SCLFile.SetAttrib(_T("bType"), attrib.bType );
	m_SCLFile.SetAttrib(_T("type"), attrib.type );
	return true;
}

bool SCLTools::AddDATypeIntoDataTypeTmplt(const SCL_DATYPE_ATTRIB &attrib)
{
	// ��ǰ�㼶����DataTypeTemplates�򷵻�false
	if (m_SCLFile.GetTagName() != _T("DataTypeTemplates"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("DAType"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("id"), attrib.id );
//	m_SCLFile.SetAttrib(_T("desc"), attrib.desc );
	return true;
}

bool SCLTools::AddBDAIntoDAType(const SCL_BDAINDATYPE_ATTRIB &attrib)
{
	// ��ǰ�㼶����DAType�򷵻�false
	if (m_SCLFile.GetTagName() != _T("DAType"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("BDA"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("name"), attrib.name );
	m_SCLFile.SetAttrib(_T("bType"), attrib.bType );
	m_SCLFile.SetAttrib(_T("type"), attrib.type );
	return true;
}

bool SCLTools::AddEnumTypeIntoDataTypeTmplt(const SCL_ENUMVAL_ATTRIB &attrib)
{
	// ��ǰ�㼶����DataTypeTemplates�򷵻�false
	if (m_SCLFile.GetTagName() != _T("DataTypeTemplates"))
		return false;

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("EnumType"),_T(""),SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("id"), attrib.id );
	//	m_SCLFile.SetAttrib(_T("desc"), attrib.desc );

	if ( !m_SCLFile.IntoElem() )
		return false;
	if ( !m_SCLFile.AddElem(_T("EnumVal"),attrib.enumVal,SCL_MARKUP_NODE_FLAG) )
		return false;
	m_SCLFile.SetAttrib(_T("ord"), attrib.ord );
	m_SCLFile.OutOfElem();

	return true;
}

///
/// @brief
///     ����SCL�ļ�
///
bool SCLTools::SaveSCL(const SCL_CSTR_FILENAME &szFileName)
{
	m_SCLFile.Save(szFileName);
	return true;
}

