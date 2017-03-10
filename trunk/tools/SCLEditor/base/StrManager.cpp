#include "stdafx.h"
#include "StrManager.h"

#include "src/utils/cmarkup/markup.h"

#define DEFAULT_LAN_FOLDER          L"lan\\"        ///< �������·��
#define DEFAULT_LAN_FILE_PREFIX     L"SCLEditor_"   ///< ��������ļ�����ǰ׺
#define DEFAULT_LAN_TYPE            L"xml"          ///< ��Դ�ļ�����

/**
*      ��ȡģ��·���ַ���
*
* \return
*      CString     ģ���·���ַ���
* \note
*      ����x:\aaaaa\bbbb\ccc.exe�򷵻�x:\aaaaa\bbbb\
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
*      ����ָ����Դ�ļ�
*
*  \param[in]
*      ָ����������Դ��ʶ
*  \return
*      bool            ������Դ�ɹ�����true��ʧ�ܷ���false
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
*      ��ȡָ����ʶ���ַ���
*
*  \param[in]
*      uResID          �ַ�����ԴID
*  \return
*      CString         ������ԴID��Ӧ���ַ�������ID�������򷵻ؿ��ַ���
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