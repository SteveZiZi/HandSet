/**
 * \file
 *      dsm_bmpmgr.cpp
 *
 * \brief
 *      图片资源管理器实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/3/15
 */

#include "stdafx.h"
#include "dsm_bmpmgr.h"

#include "src/utils/utility/dsm_utility.h"
#include "src/utils/cmarkup/markup.h"


#define DEFAULT_BMP_FOLDER      L"bmp\\"            ///< 图片资源路径
#define DEFAULT_BMP_CFGFILE     L"res.xml"          ///< 图片资源配置文件
#define DEFAULT_BMP_LOADERR     \
        L"failed to load bmp file: %s\n"            ///< 加载图片错误信息模板

#define DEFAULT_BMP_USE_MUTLILAN        1           ///< 使用多国语言


dsm_bmpmgr::dsm_bmpmgr()
: m_bPreload(true)
{

}

dsm_bmpmgr::~dsm_bmpmgr()
{
    dsm_bmpmgr::clearRes();
}

/**
 *      加载指定标识的资源
 *
 *  \param[in]
 *      nResID              指定语言标识
 *  \return
 *      bool                加载成功返回true，失败返回false
 *
 *  \note
 *      可根据配置决定是否使用图片预加载机制
 */
bool dsm_bmpmgr::Load( UINT nResID )
{
    dsm_bmpmgr::clearRes();

    CString csBMPFolder;
    csBMPFolder.Format(L"%s%s", DSM_GetAppDir(), DEFAULT_BMP_FOLDER);

    CString csXML;
    csXML.Format(L"%s%s", csBMPFolder, DEFAULT_BMP_CFGFILE);

    CMarkup xml;
    if(!xml.Load((LPCTSTR)csXML))
    {
        ::OutputDebugString(L"failed to load xml file\n");
        ASSERT(FALSE);
        return false;
    }

    // 是否允许预先加载图片资源
    if(xml.FindChildElem(L"preload"))
    {
        xml.IntoElem();
        CString csPreload = xml.GetAttrib(L"val").c_str();
        getInstance()->m_bPreload = !csPreload.CompareNoCase(L"true");
        xml.OutOfElem();
    }

    UINT nID, nLan;
    CString csBMPFile, csStrVal;
    typedef std::pair<UINT, CString>    pairStrBmp;

    while(xml.FindChildElem(L"group"))
    {
        xml.IntoElem();

        // 先填充图片文件路径的映射表
        while(xml.FindChildElem(L"bmp"))
        {
            xml.IntoElem();

            nID         = _wtoi(xml.GetAttrib(L"id").c_str());
            csStrVal    = xml.GetAttrib(L"val").c_str();
            nLan        = _wtoi(xml.GetAttrib(L"lan").c_str());

            csBMPFile.Format(L"%s%s", csBMPFolder, csStrVal);

            if(DEFAULT_BMP_USE_MUTLILAN == nLan)
            {
                csBMPFile.Format(csBMPFile, nResID);
            }

            getInstance()->m_mapStrBmp.insert(pairStrBmp(nID, csBMPFile));
            xml.OutOfElem();
        }

        xml.OutOfElem();
    }

    dsm_bmpmgr::loadBMP();

    return true;
}

/**
 *      获取指定标识的图片句柄
 *
 *  \param[in]
 *      指定图片资源的ID
 */
HBITMAP dsm_bmpmgr::Get( UINT nResID )
{
    dsm_bmpmgr* pMgr = getInstance();
    HBITMAP hBitmap = NULL;

    if(pMgr->m_bPreload)
    {
        maphbmp::const_iterator iter = pMgr->m_mapBitmap.find(nResID);
        if(pMgr->m_mapBitmap.end() == iter)
        {
            return NULL;
        }

        hBitmap = iter->second;
    }
    else
    {
        mapcsbmp::const_iterator iter = pMgr->m_mapStrBmp.find(nResID);
        if(pMgr->m_mapStrBmp.end() == iter)
        {
            return NULL;
        }

        hBitmap = SHLoadDIBitmap(iter->second);
        if(!hBitmap)
        {
            CString csMsg;
            csMsg.Format(DEFAULT_BMP_LOADERR, iter->second);
            ::OutputDebugString(csMsg);
        }

        pMgr->m_mapBitmap.insert(std::pair<UINT, HBITMAP>(nResID, hBitmap));
    }

    return hBitmap;
}

CBitmap* dsm_bmpmgr::GetCB( UINT nResID )
{
    CBitmap* pBitmap = NULL;
    dsm_bmpmgr* pMgr = getInstance();

    mapcbmp::iterator iter = pMgr->m_mapCBitmap.find(nResID);

    if(pMgr->m_mapCBitmap.end() == iter)
    {
        pBitmap = new CBitmap;
        pBitmap->Attach(dsm_bmpmgr::Get(nResID));
        pMgr->m_mapCBitmap.insert(std::pair<UINT, CBitmap*>(nResID, pBitmap));
    }
    else
    {
        pBitmap = iter->second;
    }

    ASSERT(pBitmap);

    return pBitmap;
}



/**
 *      获取静态实例对象
 *
 *  \return
 *      dsm_bmpmgr*         返回图片管理器指针
 */
dsm_bmpmgr* dsm_bmpmgr::getInstance()
{
    static dsm_bmpmgr s_bmpMgr;

    return &s_bmpMgr;
}

/**
 *      根据图片路径映射表加载资源
 */
void dsm_bmpmgr::loadBMP()
{
    dsm_bmpmgr* pMgr = getInstance();

    mapcsbmp::const_iterator iter = pMgr->m_mapStrBmp.begin();
    mapcsbmp::const_iterator iterEnd = pMgr->m_mapStrBmp.end();
    typedef std::pair<UINT, HBITMAP>    paribitmap;

    CString csMsg;
    HBITMAP hBitmap = NULL;
    for(; iter != iterEnd; iter++)
    {
        hBitmap = SHLoadDIBitmap(iter->second);
        if(hBitmap)
        {
            pMgr->m_mapBitmap.insert(paribitmap(iter->first, hBitmap));
        }
        else
        {
            csMsg.Format(DEFAULT_BMP_LOADERR, iter->second);
            ::OutputDebugString(csMsg);
        }
    }
}

/**
 *      清理资源
 */
void dsm_bmpmgr::clearRes()
{
    dsm_bmpmgr* pMgr = getInstance();

    mapcbmp::iterator iterc = pMgr->m_mapCBitmap.begin();
    mapcbmp::const_iterator itercEnd = pMgr->m_mapCBitmap.end();

    // 清理CBitmap对象
    for(; iterc != itercEnd; iterc++)
    {
        iterc->second->Detach();
        delete iterc->second;
    }

    maphbmp::iterator iter = pMgr->m_mapBitmap.begin();
    maphbmp::const_iterator iterEnd = pMgr->m_mapBitmap.end();

    // 清理HBitmap
    for(; iter != iterEnd; iter++)
    {
        ::DeleteObject(iter->second);
    }

    pMgr->m_mapCBitmap.clear();
    pMgr->m_mapBitmap.clear();
    pMgr->m_mapStrBmp.clear();
}

CString dsm_bmpmgr::GetBmpPath( UINT nResID )
{
    mapcsbmp::const_iterator iter = getInstance()->m_mapStrBmp.find(nResID);

    if(getInstance()->m_mapStrBmp.end() == iter)
    {
        return L"";
    }

    return iter->second;
}

