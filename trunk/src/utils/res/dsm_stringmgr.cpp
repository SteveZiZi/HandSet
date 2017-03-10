/**
 * \file
 *      dsm_stringmgr.cpp
 *
 * \brief
 *      字符串资源管理器实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/3/14
 */

#include "stdafx.h"
#include "dsm_stringmgr.h"

#include "src/utils/utility/dsm_utility.h"
#include "src/utils/cmarkup/markup.h"

#define DEFAULT_LAN_FOLDER          L"lan\\"        ///< 多国语言路径
#define DEFAULT_LAN_TYPE            L"xml"          ///< 资源文件类型


/**
 *      加载指定资源文件
 *
 *  \param[in]
 *      指定的语言资源标识
 *  \return
 *      bool            加载资源成功返回true，失败返回false
 */
bool dsm_stringMgr::Load( UINT uResID )
{
    getInstance()->m_mapString.clear();

    CString csXML;
    csXML.Format(L"%s%s%d.%s", 
        DSM_GetAppDir(), DEFAULT_LAN_FOLDER, uResID, DEFAULT_LAN_TYPE);

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
const CString& dsm_stringMgr::Get( UINT uResID )
{
    static const CString csBlank(L"");
    dsm_stringMgr* pMgr = getInstance();
    std::map<UINT, CString>::const_iterator itFind = pMgr->m_mapString.find(uResID);

    if(pMgr->m_mapString.end() == itFind)
    {
        return csBlank;
    }

    return itFind->second;
}

