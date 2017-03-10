/**
 * \file
 *      dsm_stringmgr.cpp
 *
 * \brief
 *      �ַ�����Դ������ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/3/14
 */

#include "stdafx.h"
#include "dsm_stringmgr.h"

#include "src/utils/utility/dsm_utility.h"
#include "src/utils/cmarkup/markup.h"

#define DEFAULT_LAN_FOLDER          L"lan\\"        ///< �������·��
#define DEFAULT_LAN_TYPE            L"xml"          ///< ��Դ�ļ�����


/**
 *      ����ָ����Դ�ļ�
 *
 *  \param[in]
 *      ָ����������Դ��ʶ
 *  \return
 *      bool            ������Դ�ɹ�����true��ʧ�ܷ���false
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
 *      ��ȡָ����ʶ���ַ���
 *
 *  \param[in]
 *      uResID          �ַ�����ԴID
 *  \return
 *      CString         ������ԴID��Ӧ���ַ�������ID�������򷵻ؿ��ַ���
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

