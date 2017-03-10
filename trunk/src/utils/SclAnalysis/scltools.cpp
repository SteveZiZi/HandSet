/// @file
///     scltools.cpp
///
/// @brief
///     SCL文件解析工具
///
/// @note
///     对Markup解析器进行封装，使之更适合SCL文件的解析，
///     主要包括文件加载、复位、定位和获取数据这4类函数
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.1.22
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.02.01     |1.0.1.201  |增加对SCL文件中DataTypeTemplates部分的访问               |
///    |2013.02.06     |1.0.2.206  |增加对SCL文件中外部信号（Inputs及ExtRef）的访问          |
///    |2013.05.22     |1.0.4.522  |添加符合SCL文件格式的各元素的插入接口                    |
///    |2013.06.26     |1.0.4.626  |修改子网元素的添加接口                                   |
///    |2013.08.07     |1.0.5.807  |添加子网元素属性信息的获取接口                           |
///    |2013.08.08     |1.0.5.808  |修正DOI信息获取接口在dU元素不存在时相应的信息没初始化的Bug|
///    |2013.11.04     |1.0.5.1104 |添加SMV/GOOSE控制块信息时增加版本号信息                  |
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
///     加载SCL文件
///
/// @param[in]
///     szFileName - 文件路径
///
/// @return
///     true：加载成功；  false：加载失败
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
///     复位到该文件的开始
///
void SCLTools::ResetPos()
{
	m_SCLFile.ResetPos();
}

///
/// @brief
///     复位到当前元素所在层级的第1个元素处
///
void SCLTools::ResetMainPos()
{
	m_SCLFile.ResetMainPos();
}

///
/// @brief
///     复位到当前元素的父元素处
///
/// @return
///     true：复位成功；  false：已经为文件的顶部
///
bool SCLTools::OutOfElem()
{
	return m_SCLFile.OutOfElem();
}

///
/// @brief
///     进入当前元素内部
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::IntoElem()
{
	// 进入currentElem的下一层级
	if ( !m_SCLFile.IntoElem() )
		return false;

	return true;
}

///
/// @brief
///     在当前层级中搜索某一属性值匹配的特定元素，并定位到该元素
///     若attribName为空串，则定位到下一个elemName元素
///
/// @param[in]
///     node       - 所要定位到的元素，不能为空串
/// @param[in]
///     attribName - 元素elemName的某一属性
/// @param[in]
///     attribVal  - 元素elemName的attribName属性的值
///
/// @return
///     true：定位成功；  false：找不到该元素
///
bool SCLTools::FindElemWithAttrib(const SCL_CSTR elemTag, const SCL_CSTR attribName,
								  const SCL_STR attribVal)
{
	// 如果node为空，则返回false
	if ( elemTag == _T("") )
		return false;

	// attribName为空则定位到下一个elemName元素
	if (attribName == _T("") )
	{
		return m_SCLFile.FindElem(elemTag);
	}

	// 遍历，寻找attribName的属性值为attribVal的Node，并定位到该Node
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
///     进入当前元素内部搜索某一属性值匹配的特定子元素，并定位到该元素
///     若attribName为空串，则定位到下一个subNode元素
///
/// @param[in]
///     parentNode - 当前元素
/// @param[in]
///     subNode    - 所要定位到的子元素
/// @param[in]
///     attribName - 元素subNode的某一属性
/// @param[in]
///     attribVal  - 元素subNode的attribName属性的值
///
/// @return
///     true：定位成功；  false：找不到该元素
///
bool SCLTools::FindChildElemWithAttrib(const SCL_STR parentElem, const SCL_CSTR childElem,
									   const SCL_CSTR attribName, const SCL_STR attribVal)
{
	// 当前元素不是parentNode则返回false
	if ( m_SCLFile.GetTagName() != parentElem )
		return false;

	// 进入parentNode的下一层级
	if ( !m_SCLFile.IntoElem() )
		return false;
	
	return FindElemWithAttrib(childElem, attribName, attribVal);
}

///
/// @brief
///     定位到SCL元素。SCL元素为SCL文件的根元素
///
/// @return
///     true：定位成功；  false：找不到该元素
///
bool SCLTools::FindSCL()
{
	// 回到文件开始处
	m_SCLFile.ResetPos();

	// 定位到SCL元素，一个SCL文件只有一个
	return m_SCLFile.FindElem(_T("SCL"));
}

///
/// @brief
///     在SCL元素内部搜索与属性name值匹配的IED元素，并定位到该元素
///     若iedName为空串，则定位到下一个IED元素
///
/// @param[in]
///     iedName - 目标IED元素的name属性值，默认为空串
///
/// @return
///     true：定位成功；  false：找不到该元素
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
///     在IED元素内部搜索与属性name值匹配的AccessPoint元素，并定位到该元素
///     若apName为空串，则定位到下一个AccessPoint元素
///
/// @param[in]
///     apName - 目标AccessPoint元素的name属性值，默认为空串
///
/// @return
///     true：定位成功；  false：找不到该元素
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
	// 搜索Server，1个AccessPoint只有1个Server
	return m_SCLFile.FindElem(_T("Server"));
}

///
/// @brief
///     在Server元素内部搜索与属性inst值匹配的LDevice元素，并定位到该元素
///     若ldInst为空串，则定位到下一个LDevice元素
///
/// @param[in]
///     ldInst - 目标LDevice元素的inst属性值，默认为空串
///
/// @return
///     true：定位成功；  false：找不到该元素
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
///     在IED元素内部搜索与属性inst值匹配的LDevice元素，并定位到该元素
///     若ldInst为空串，则定位到下一个LDevice元素
///
/// @param[in]
///     ldInst - 目标LDevice元素的inst属性值，默认为空串
///
/// @return
///     true：定位成功；  false：找不到该元素
///
bool SCLTools::FindLDeviceInIED(const SCL_STR &ldInst)
{
	if ( ldInst == _T("") )
		return false;
	while ( m_SCLFile.FindElem(_T("AccessPoint")) )
	{
		if ( !m_SCLFile.IntoElem() )
			continue;
		// 搜索Server，1个AccessPoint只有1个Server
		if ( !m_SCLFile.FindElem(_T("Server")) )
			continue;
		if ( !m_SCLFile.IntoElem() )
			continue;
		if ( FindElemWithAttrib(_T("LDevice"),_T("inst"),ldInst))
			return true;
		// 回到Server
		m_SCLFile.OutOfElem();
		// 回到AccessPoint
		m_SCLFile.OutOfElem();
	}
	return false;
}

///
/// @brief
///     在LDevice元素内部搜索LN0元素，并定位到该元素
///
/// @return
///     true：定位成功；  false：找不到该元素
///
bool SCLTools::FindLN0InLDevice()
{
	return m_SCLFile.FindElem(_T("LN0"));
}

///
/// @brief
///     在LDevice元素内部搜索与属性prefix、lnClass、lnInst值匹配的LN元素，并定位到该元素
///     若lnPrefix、lnClass、lnInst均为空串，则定位到下一个LN元素
///
/// @param[in]
///     lnPrefix - 目标LN元素的prefix属性值
/// @param[in]
///     lnClass  - 目标LN元素的lnClass属性值,为“LLN0”时则为LN0
/// @param[in]
///     lnInst   - 目标LN元素的lnInst属性值
///
/// @return
///     true：定位成功；  false：找不到该元素
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
///     在LDevice元素内部搜索与lnName相同的LN元素，并定位到该元素
///     若lnName均为空串，则定位到下一个LN元素
///
/// @param[in]
///     lnName - 目标LN元素的名称，默认为空串，由prefix+lnClass+lnInst组成
///
/// @return
///     true：定位成功；  false：找不到该元素
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
///     在LN0元素内部搜索与属性name值匹配的DataSet元素，并定位到该元素
///     若dataSetName为空串，则定位到下一个DataSet元素
///
/// @param[in]
///     dataSetName - 目标DataSet元素的name属性值，默认为空串
///
/// @return
///     true：定位成功；  false：找不到该元素
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
///     在LN0元素内部搜索与属性name值匹配的SampledValueControl元素，并定位到该元素
///     若cbName为空串，则定位到下一个SampledValueControl元素
///
/// @param[in]
///     cbName - 目标SampledValueControl元素的name属性值，默认为空串
///
/// @return
///     true：定位成功；  false：找不到该元素
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
///     在LN0元素内部搜索与属性name值匹配的GSEControl元素，并定位到该元素
///     若cbName为空串，则定位到下一个GSEControl元素
///
/// @param[in]
///     cbName - 目标GSEControl元素的name属性值，默认为空串
///
/// @return
///     true：定位成功；  false：找不到该元素
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
///     在LN0或者LN元素内部搜索与属性name值匹配的DOI元素，并定位到该元素
///     若doName为空串，则定位到下一个DOI元素
///
/// @param[in]
///     doName - 目标DOI元素的name属性值，默认为空串
///
/// @return
///     true：定位成功；  false：找不到该元素
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
///     在LN0或者LN元素内部搜索下一个Inputs元素，并定位到该元素
///     第1次搜索到的是第一个Inputs元素
///
/// @return
///     true：定位成功；  false：找不到该元素
///
bool SCLTools::FindInputsInLNorLN0()
{
	return m_SCLFile.FindElem(_T("Inputs"));
}

///
/// @brief
///     在Inputs元素内部搜索下一个ExtRef元素，并定位到该元素
///     第1次搜索到的是第一个ExtRef元素
///
/// @return
///     true：定位成功；  false：找不到该元素
///
bool SCLTools::FindExtRefInInputs()
{
	return m_SCLFile.FindElem(_T("ExtRef"));
}

///
/// @brief
///     在DOI元素内部搜索与属性name值匹配的DAI元素，并定位到该元素
///     若daName为空串，则定位到下一个DAI元素
///
/// @param[in]
///     daName - 目标DAI元素的name属性值，默认为空串
///
/// @return
///     true：定位成功；  false：找不到该元素
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
///     在SCL元素内部搜索Communication元素，并定位到该元素
///     一个SCL元素中只有一个Communicaiton元素
///
/// @return
///     true：定位成功；  false：找不到该元素
///
bool SCLTools::FindCommnInSCL()
{
	return FindElemWithAttrib(_T("Communication"), _T(""), _T(""));
}

///
/// @brief
///     在Communication元素内部搜索与属性name值匹配的SubNetwork元素，并定位到该元素
///     若subNetworkName为空串，则定位到下一个SubNetwork元素
///
/// @param[in]
///     subNetworkName - 目标SubNetwork元素的name属性值，默认为空串
///
/// @return
///     true：定位成功；  false：找不到该元素
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
///     在SubNetwork元素内部搜索与属性apName、iedName值匹配的ConnectedAP元素，并定位到该元素
///     若apName、iedName均为空串，则定位到下一个ConnectedAP元素
///
/// @param[in]
///     apName  - 目标ConnectedAP元素的apName属性值，默认为空串
/// @param[in]
///     iedName - 目标ConnectedAP元素的iedName属性值，默认为空串
///
/// @return
///     true：定位成功；  false：找不到该元素
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
///     在ConnnectedAP元素内部搜索Address元素，并定位到该元素
///
/// @return
///     true：定位成功；  false：找不到该元素
///
bool SCLTools::FindAddressInConnnectedAP()
{
	return FindElemWithAttrib(_T("Address"),_T(""),_T(""));
}

///
/// @brief
///     在ConnectedAP元素内部搜索与属性cbName、ldInst值匹配的SMV元素，并定位到该元素
///     若cbName、ldInst均为空串，则定位到下一个SMV元素
///
/// @param[in]
///     cbName - 目标SMV元素的cbName属性值，默认为空串
/// @param[in]
///     ldInst - 目标SMV元素的ldInst属性值，默认为空串
///
/// @return
///     true：定位成功；  false：找不到该元素
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
///     在ConnectedAP元素内部搜索与属性cbName、ldInst值匹配的GSE元素，并定位到该元素
///     若cbName、ldInst均为空串，则定位到下一个GSEControl元素
///
/// @param[in]
///     cbName - 目标GSE元素的cbName属性值，默认为空串
/// @param[in]
///     ldInst - 目标GSE元素的ldInst属性值，默认为空串
///
/// @return
///     true：定位成功；  false：找不到该元素
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
///     在SCL元素内部搜索DataTypeTemplates元素，并定位到该元素
///     一个SCL元素中只有一个DataTypeTemplates元素
///
/// @return
///     true：定位成功；  false：找不到该元素
///
bool SCLTools::FindDataTypeTmpltInSCL()
{
	return FindElemWithAttrib(_T("DataTypeTemplates"), _T(""), _T(""));
}

///
/// @brief
///     在DataTypeTemplates元素内部搜索与属性id值匹配的LNodeType元素，并定位到该元素
///     若id为空串，则定位到下一个LNodeType元素
///
/// @param[in]
///     id - 目标LNodeType元素的id属性值，默认为空串
///
/// @return
///     true：定位成功；  false：找不到该元素
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
///     在LNodeType元素内部搜索与属性name值匹配的DO元素，并定位到该元素
///     若name为空串，则定位到下一个DO元素
///
/// @param[in]
///     name - 目标DO元素的name属性值，默认为空串
///
/// @return
///     true：定位成功；  false：找不到该元素
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
///     在DataTypeTemplates元素内部搜索与属性id值匹配的DOType元素，并定位到该元素
///     若id为空串，则定位到下一个DOType元素
///
/// @param[in]
///     id - 目标DOType元素的id属性值，默认为空串
///
/// @return
///     true：定位成功；  false：找不到该元素
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
///     在DOType元素内部搜索与属性name值匹配的DA元素，并定位到该元素
///     若name为空串，则定位到下一个DA元素
///
/// @param[in]
///     name - 目标DA元素的name属性值，默认为空串
///
/// @return
///     true：定位成功；  false：找不到该元素
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
///     在DataTypeTemplates元素内部搜索与属性id值匹配的DAType元素，并定位到该元素
///     若id为空串，则定位到下一个DAType元素
///
/// @param[in]
///     id - 目标DAType元素的id属性值，默认为空串
///
/// @return
///     true：定位成功；  false：找不到该元素
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
///     在DAType元素内部搜索与属性name值匹配的BDA元素，并定位到该元素
///     若name为空串，则定位到下一个BDA元素
///
/// @param[in]
///     name - 目标BDA元素的name属性值，默认为空串
///
/// @return
///     true：定位成功；  false：找不到该元素
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
///     在DataTypeTemplates元素内部搜索与属性id值匹配的EnumType元素，并定位到该元素
///     若id为空串，则定位到下一个EnumType元素
///
/// @param[in]
///     id - 目标EnumType元素的id属性值，默认为空串
///
/// @return
///     true：定位成功；  false：找不到该元素
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
///     获取当前元素的标签名
///
/// @return
///     当前元素的标签名
///
SCL_STR SCLTools::GetElemTag()
{
	return m_SCLFile.GetTagName();
}

///
/// @brief
///     获取当前IED元素的属性，需先定位到IED元素
///
/// @param[in]
///     iedAttrib - 保存IED属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetIEDAttrib(SCL_IED_ATTRIB &iedAttrib)
{
	// 当前层级不是IED则返回false
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
///     获取当前AccessPoint元素的属性，需先定位到AccessPoint元素
///
/// @param[in]
///     apAttrib - 保存AccessPoint属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetAccessPointAttrib(SCL_ACCESS_POINT_ATTRIB &apAttrib)
{
	// 当前层级不是AccessPoint则返回false
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
///     获取当前Server元素的属性，需先定位到Server元素
///
/// @param[in]
///     serverAttrib - 保存Server属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetServerAttrib(SCL_SERVER_ATTRIB &serverAttrib)
{
	// 当前层级不是AccessPoint则返回false
	if (m_SCLFile.GetTagName() != _T("AccessPoint"))
		return false;

	serverAttrib.serverDesc = m_SCLFile.GetAttrib(_T("desc"));
	serverAttrib.timeout    = m_SCLFile.GetAttrib(_T("timeout"));

	return true;
}

///
/// @brief
///     获取当前LDevice元素的属性，需先定位到LDevice元素
///
/// @param[in]
///     ldAttrib - 保存LDevice属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetLDeviceAttrib(SCL_LDEVICE_ATTRIB &ldAttrib)
{
	// 当前层级不是LDevice则返回false
	if (m_SCLFile.GetTagName() != _T("LDevice"))
		return false;

	ldAttrib.ldDesc = m_SCLFile.GetAttrib(_T("desc"));
	ldAttrib.ldInst = m_SCLFile.GetAttrib(_T("inst"));

	return true;
}

///
/// @brief
///     获取当前LN0元素的属性，需先定位到LN0元素
///
/// @param[in]
///     ln0Attrib - 保存LN0属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetLN0Attrib(SCL_LN0_ATTRIB &ln0Attrib)
{
	// 当前层级不是LN0则返回false
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
///     获取当前LN元素的属性，需先定位到LN元素
///
/// @param[in]
///     lnAttrib - 保存LN属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetLNAttrib(SCL_LN_ATTRIB &lnAttrib)
{
	// 当前层级不是LN则返回false
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
///     获取当前DataSet元素的属性，需先定位到DataSet元素
///
/// @param[in]
///     dataSetAttrib - 保存DataSet属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetDataSetAttrib(SCL_DATASET_ATTRIB &datSetAttrib)
{
	// 当前层级不是DataSet则返回false
	if (m_SCLFile.GetTagName() != _T("DataSet"))
		return false;

	// 获取DataSet中FCDA的数目
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
///     获取DataSet中某一顺序号的FCDA元素的属性，需先定位到DataSet元素
///
/// @param[in]
///     fcdaAttrib - 保存FCDA属性信息的结构体
/// @param[in]
///     idxSeq     - 该FCDA元素在DataSet元素中的顺序号，从0开始
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetOneFCDAAttrib(SCL_FCDA_ATTRIB &fcdaAttrib, int idxSeq)
{
	// 当前层级不是DataSet则返回false
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
///     获取DOI元素的属性，需先定位到DOI元素
///
/// @param[in]
///     doiAttrib - 保存DOI属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetDOIAttrib(SCL_DOI_ATTRIB &doiAttrib)
{
	// 当前层级不是DOI则返回false
	if (m_SCLFile.GetTagName() != _T("DOI"))
		return false;

	doiAttrib.doName        = m_SCLFile.GetAttrib(_T("name"));
	doiAttrib.doDesc        = m_SCLFile.GetAttrib(_T("desc"));
// 	doiAttrib.accessControl = m_SCLFile.GetAttrib(_T("accessControl"));
// 	doiAttrib.ix            = m_SCLFile.GetAttrib(_T("ix"));

	if ( !m_SCLFile.IntoElem() )
		return false;

	// 获取name=“dU”的DAI的信息
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
///     获取DAI元素的属性，需先定位到DAI元素
///
/// @param[in]
///     daiAttrib - 保存DAI属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetDAIAttrib(SCL_DAI_ATTRIB &daiAttrib)
{
	// 当前层级不是DAI则返回false
	if (m_SCLFile.GetTagName() != _T("DAI"))
		return false;

	daiAttrib.daName = m_SCLFile.GetAttrib(_T("name"));
	daiAttrib.daDesc = m_SCLFile.GetAttrib(_T("desc"));
	daiAttrib.sAddr  = m_SCLFile.GetAttrib(_T("sAddr"));

	return true;
}

///
/// @brief
///     获取当前ExtRef元素的属性，需先定位到ExtRef元素
///
/// @param[in]
///     extRefAttrib - 保存ExtRef属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetExtRefAttrib(SCL_EXTREF_ATTRIB &extRefAttrib)
{
	// 当前层级不是ExtRef则返回false
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
///     获取当前SmapledValueControl元素的属性，需先定位到SmapledValueControl元素
///
/// @param[in]
///     smvCtrlAttrib - 保存SmapledValueControl属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetSMVCtrlAttrib(SCL_SMVCTRL_ATTRIB &smvCtrlAttrib)
{
	// 当前层级不是SampledValueControl则返回false
	if (m_SCLFile.GetTagName() != _T("SampledValueControl"))
		return false;

	smvCtrlAttrib.cbName    = m_SCLFile.GetAttrib(_T("name"));
	smvCtrlAttrib.cbDesc    = m_SCLFile.GetAttrib(_T("desc"));
	smvCtrlAttrib.confRev   = m_SCLFile.GetAttrib(_T("confRev"));
	smvCtrlAttrib.smvID     = m_SCLFile.GetAttrib(_T("smvID"));
	smvCtrlAttrib.dataSet   = m_SCLFile.GetAttrib(_T("datSet"));
	smvCtrlAttrib.smpRate   = m_SCLFile.GetAttrib(_T("smpRate"));
	smvCtrlAttrib.nofASDU   = m_SCLFile.GetAttrib(_T("nofASDU"));

	// multicast，SCL文件中默认为true
	if ( m_SCLFile.GetAttrib(_T("multicast")) == _T("false") )
		smvCtrlAttrib.multicast = false;
	else
		smvCtrlAttrib.multicast = true;

	// SmvOpts，SCL文件中默认为false
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
///     获取当前GSEControl元素的属性，需先定位到GSEControl元素
///
/// @param[in]
///     gseCtrlAttrib - 保存GSEControl属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetGSECtrlAttrib(SCL_GSECTRL_ATTRIB &gseCtrlAttrib)
{
	// 当前层级不是GSEControl则返回false
	if (m_SCLFile.GetTagName() != _T("GSEControl"))
		return false;

	gseCtrlAttrib.cbName  = m_SCLFile.GetAttrib(_T("name"));
	gseCtrlAttrib.cbDesc  = m_SCLFile.GetAttrib(_T("desc"));
	gseCtrlAttrib.confRev = m_SCLFile.GetAttrib(_T("confRev"));
	gseCtrlAttrib.gooseID = m_SCLFile.GetAttrib(_T("appID"));
	gseCtrlAttrib.dataSet = m_SCLFile.GetAttrib(_T("datSet"));
	// type默认为GOOSE
	if ( m_SCLFile.GetAttrib(_T("type")).compare(_T("GSSE")) == 0 )
		gseCtrlAttrib.type = _T("GSSE");
	else
		gseCtrlAttrib.type = _T("GOOSE");

	return true;
}

///
/// @brief
///     获取SubNetwork元素的属性信息，需先定位到SubNetwork元素
///
/// @param[in]
///     subNetAttrib - 保存SubNetwork元素的属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetSubNetworkAttrib(SCL_SUBNETWORK_ATTRIB &subNetAttrib)
{
	// 当前层级不是ConnectedAP则返回false
	if (m_SCLFile.GetTagName() != _T("SubNetwork"))
		return false;

	subNetAttrib.name = m_SCLFile.GetAttrib(_T("name"));
	subNetAttrib.type = m_SCLFile.GetAttrib(_T("type"));

	return true;
}

///
/// @brief
///     获取ConnectedAP元素的属性信息，该信息为在某子网上的访问点引用，需先定位到ConnectedAP元素
///
/// @param[in]
///     connectedAPAttrib - 保存ConnectedAP元素的属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetConnectedAPAttrib(SCL_CONNECTEDAP_ATTRIB &connectedAPAttrib)
{
	// 当前层级不是ConnectedAP则返回false
	if (m_SCLFile.GetTagName() != _T("ConnectedAP"))
		return false;

	connectedAPAttrib.iedName = m_SCLFile.GetAttrib(_T("iedName"));
	connectedAPAttrib.apName  = m_SCLFile.GetAttrib(_T("apName"));

	return true;
}

///
/// @brief
///     获取ConnectedAP元素中Address元素的信息，该信息为客户/服务器传输的地址信息，需先定位到Address元素
///
/// @param[in]
///     addrAtttrib - 保存ConnectedAP元素中Address元素的信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetAddrAttrib(SCL_ADDR_ATTRIB &addrAtttrib)
{
	// 当前层级不是Address则返回false
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
///     获取ConnectedAP元素中SMV元素的信息，为SMV传输服务的通信地址及其控制块引用，需先定位到SMV元素
///
/// @param[in]
///     smvAttrib - 保存ConnectedAP元素中SMV信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetSMVAttrib(SCL_SMV_ATTRIB &smvAttrib)
{
	// 当前层级不是SMV则返回false
	if (m_SCLFile.GetTagName() != _T("SMV"))
		return false;

	smvAttrib.ldInst = m_SCLFile.GetAttrib(_T("ldInst"));
	smvAttrib.cbName = m_SCLFile.GetAttrib(_T("cbName"));

	if ( !m_SCLFile.IntoElem() )
		return false;

	// SMV-->Address，获取规定的地址信息
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
	
	// 回到SMV
	m_SCLFile.OutOfElem();

	return true;
}

///
/// @brief
///     获取ConnectedAP元素中GSE元素的信息，为GOOSE/GSSE传输服务的通信地址及其控制块引用，需先定位到GSE元素
///
/// @param[in]
///     gseAttrib - 保存ConnectedAP元素中GSE信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetGSEAttrib(SCL_GSE_ATTRIB &gseAttrib)
{
	// 当前层级不是GSE则返回false
	if (m_SCLFile.GetTagName() != _T("GSE"))
		return false;

	gseAttrib.ldInst = m_SCLFile.GetAttrib(_T("ldInst"));
	gseAttrib.cbName = m_SCLFile.GetAttrib(_T("cbName"));

	if ( !m_SCLFile.IntoElem() )
		return false;

	// GSE-->Address，获取规定的地址信息
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

	// GSE-->MinTime，获取GOOSE数据变位后立即补发的时间间隔
	m_SCLFile.ResetMainPos();
	if ( m_SCLFile.FindElem(_T("MinTime")) )
	{
		gseAttrib.minTimeVal   = m_SCLFile.GetData();
		gseAttrib.minTimeUnit  = m_SCLFile.GetAttrib(_T("multiplier"));
		gseAttrib.minTimeUnit += m_SCLFile.GetAttrib(_T("unit"));
	}

	// GSE-->MaxTime，获取GOOSE心跳时间
	m_SCLFile.ResetMainPos();
	if ( m_SCLFile.FindElem(_T("MaxTime")) )
	{
		gseAttrib.maxTimeVal   = m_SCLFile.GetData();
		gseAttrib.maxTimeUnit  = m_SCLFile.GetAttrib(_T("multiplier"));
		gseAttrib.maxTimeUnit += m_SCLFile.GetAttrib(_T("unit"));
	}

	// 回到GSE
	m_SCLFile.OutOfElem();

	return true;
}

///
/// @brief
///     获取LNodeType元素的属性信息，需先定位到LNodeType元素
///
/// @param[in]
///     lnTypeAttrib - 保存LNodeType元素的属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetLNTypeAttrib(SCL_LNTYPE_ATTRIB &lnTypeAttrib)
{
	// 当前层级不是LNodeType则返回false
	if (m_SCLFile.GetTagName() != _T("LNodeType"))
		return false;

	lnTypeAttrib.id      = m_SCLFile.GetAttrib(_T("id"));
	lnTypeAttrib.desc    = m_SCLFile.GetAttrib(_T("desc"));
	lnTypeAttrib.lnClass = m_SCLFile.GetAttrib(_T("lnClass"));

	return true;
}

///
/// @brief
///     获取LNodeType中某一DO元素的属性信息，需先定位到DO元素
///
/// @param[in]
///     doAttrib - 保存DO元素的属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetDOAttrib(SCL_DOINLNTYPE_ATTRIB &doAttrib)
{
	// 当前层级不是DO则返回false
	if (m_SCLFile.GetTagName() != _T("DO"))
		return false;

	doAttrib.name = m_SCLFile.GetAttrib(_T("name"));
	doAttrib.type = m_SCLFile.GetAttrib(_T("type"));
	doAttrib.desc = m_SCLFile.GetAttrib(_T("desc"));

	return true;
}

///
/// @brief
///     获取DOType元素的属性信息，需先定位到DOType元素
///
/// @param[in]
///     doTypeAttrib - 保存DOType元素的属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetDOTypeAttrib(SCL_DOTYPE_ATTRIB &doTypeAttrib)
{
	// 当前层级不是DOType则返回false
	if (m_SCLFile.GetTagName() != _T("DOType"))
		return false;

	doTypeAttrib.id  = m_SCLFile.GetAttrib(_T("id"));
	doTypeAttrib.cdc = m_SCLFile.GetAttrib(_T("cdc"));

	return true;
}

///
/// @brief
///     获取DOType中某一DA元素的属性信息，需先定位到DA元素
///
/// @param[in]
///     daAttrib - 保存DA元素的属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetDAAttrib(SCL_DAINDOTYPE_ATTRIB &daAttrib)
{
	// 当前层级不是DA则返回false
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
///     获取DAType的属性信息，需先定位到DAType元素
///
/// @param[in]
///     daTypeAttrib - 保存DAType元素的属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetDATypeAttrib(SCL_DATYPE_ATTRIB &daTypeAttrib)
{
	// 当前层级不是DA则返回false
	if (m_SCLFile.GetTagName() != _T("DAType"))
		return false;

	daTypeAttrib.id  = m_SCLFile.GetAttrib(_T("id"));
	daTypeAttrib.desc  = m_SCLFile.GetAttrib(_T("desc"));

	return true;
}

///
/// @brief
///     获取DAType的属性信息，需先定位到DAType元素
///
/// @param[in]
///     daAttrib - 保存DAType元素的属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetBDAAttrib(SCL_BDAINDATYPE_ATTRIB &bdaAttrib)
{
	// 当前层级不是DA则返回false
	if (m_SCLFile.GetTagName() != _T("BDA"))
		return false;

	bdaAttrib.name  = m_SCLFile.GetAttrib(_T("name"));
	bdaAttrib.bType = m_SCLFile.GetAttrib(_T("bType"));
	bdaAttrib.type  = m_SCLFile.GetAttrib(_T("type"));

	return true;
}

///
/// @brief
///     获取EnumType中某一EnumVal元素的属性信息，需先定位到EnumType元素
///
/// @param[in]
///     enumValAttrib - 保存EnumVal元素的属性信息的结构体
/// @param[in]
///     ord           - EnumVal的标签值
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetEnumValAttrib(SCL_ENUMVAL_ATTRIB &enumValAttrib, SCL_STR ord)
{
	// 当前层级不是EnumType则返回false
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
	// 当前层级不是SCL则返回false
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
	// 当前层级不是IED则返回false
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
	// 当前层级不是AccessPoint则返回false
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
	// 当前层级不是Server则返回false
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
	// 当前层级不是LDevice则返回false
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
	// 当前层级不是LDevice则返回false
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
	// 当前层级不是LN0则返回false
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
	// 当前层级不是DataSet则返回false
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
	// 当前层级不是LN0则返回false
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
	// 当前层级不是LN0则返回false
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
	// 当前层级不是LN0或LN则返回false
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
	// 当前层级不是LN0则返回false
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
	// 当前层级不是Inputs则返回false
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
	// 当前层级不是DOI则返回false
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
	// 当前层级不是SCL则返回false
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
	// 当前层级不是Communication则返回false
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
	// 当前层级不是SubNetwork则返回false
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
	// 当前层级不是ConnectedAP则返回false
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
	// 当前层级不是ConnectedAP则返回false
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
	// 当前层级不是SCL则返回false
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
	// 当前层级不是DataTypeTemplates则返回false
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
	// 当前层级不是LNodeType则返回false
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
	// 当前层级不是DataTypeTemplates则返回false
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
	// 当前层级不是DOType则返回false
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
	// 当前层级不是DataTypeTemplates则返回false
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
	// 当前层级不是DAType则返回false
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
	// 当前层级不是DataTypeTemplates则返回false
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
///     保存SCL文件
///
bool SCLTools::SaveSCL(const SCL_CSTR_FILENAME &szFileName)
{
	m_SCLFile.Save(szFileName);
	return true;
}

