#include "stdafx.h"
#include "StrManager.h"

#include "src/utils/cmarkup/markup.h"

#define DEFAULT_LAN_FOLDER          L"lan\\"        ///< 多国语言路径
#define DEFAULT_LAN_FILE_PREFIX     L"SCLEditor_"   ///< 多国语言文件名称前缀
#define DEFAULT_LAN_TYPE            L"xml"          ///< 资源文件类型

/**
*      获取模块路径字符串
*
* \return
*      CString     模块的路径字符串
* \note
*      比如x:\aaaaa\bbbb\ccc.exe则返回x:\aaaaa\bbbb\
*/
const CString _GetAppDir()
{
	static CString csAppDir(L"");

	if(csAppDir.IsEmpty())
	{
		::GetModuleFileName(NULL, csAppDir.GetBuffer(MAX_PATH + 1), MAX_PATH);
		csAppDir.ReleaseBuffer();

		csAppDir = csAppDir.Left(csAppDir.ReverseFind(L'\\') + 1);
	}

	return csAppDir;
}

/**
*      加载指定资源文件
*
*  \param[in]
*      指定的语言资源标识
*  \return
*      bool            加载资源成功返回true，失败返回false
*/
bool CStrManager::Load( UINT uResID )
{
	getInstance()->m_mapString.clear();

	CString csXML;
	csXML.Format(L"%s%s%s%d.%s", 
		_GetAppDir(), DEFAULT_LAN_FOLDER, DEFAULT_LAN_FILE_PREFIX, uResID, DEFAULT_LAN_TYPE);

	CMarkup xml;
	if(!xml.Load((LPCTSTR)csXML))
	{
		::OutputDebugString(L"failed to load xml file\n");
		ASSERT(FALSE);
		return false;
	}

	UINT nID;
	CString csStrVal;
	typedef std::pair<UINT, CString>     stringpair;

	while(xml.FindChildElem(L"page"))
	{
		xml.IntoElem();
		while(xml.FindChildElem(L"str"))
		{
			xml.IntoElem();
			nID = _wtoi(xml.GetAttrib(L"id").c_str());
			csStrVal = xml.GetAttrib(L"val").c_str();

			if(nID > 0)
			{
				getInstance()->m_mapString.insert(stringpair(nID, csStrVal));
			}

			xml.OutOfElem();
		}
		xml.OutOfElem();
	}

	return true;
}

/**
*      获取指定标识的字符串
*
*  \param[in]
*      uResID          字符串资源ID
*  \return
*      CString         返回资源ID对应的字符串，若ID不存在则返回空字符串
*/
CString CStrManager::Get( UINT uResID )
{
	CStrManager* pMgr = getInstance();
	std::map<UINT, CString>::const_iterator itFind = pMgr->m_mapString.find(uResID);

	if(pMgr->m_mapString.end() == itFind)
	{
		return L"";
	}

	return itFind->second;
}