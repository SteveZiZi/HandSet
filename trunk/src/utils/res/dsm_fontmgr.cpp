/**
 * \file
 *      dsm_fontmgr.cpp
 *
 * \brief
 *      ������Դ������ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/6/14
 */

#include "stdafx.h"

#include "dsm_fontmgr.h"

dsm_fontmgr::dsm_fontmgr()
{

}

dsm_fontmgr::~dsm_fontmgr()
{
    Clear();
}


void dsm_fontmgr::SetStyle( int nWidth, int nEscapement, int nOrientation, int nWeight, BYTE bItalic, 
                               BYTE bUnderline, BYTE cStrikeOut, BYTE nCharSet, BYTE nOutPrecision, BYTE nClipPrecision,
                               BYTE nQuality, BYTE nPitchAndFamily, LPCTSTR lpszFacename )
{
    m_nWidth                = nWidth;
    m_nEscapement           = nEscapement;
    m_nOrientation          = nOrientation;
    m_nWeight               = nWeight;
    m_bItalic               = bItalic;
    m_bUnderline            = bUnderline;
    m_cStrikeOut            = cStrikeOut;
    m_nCharSet              = nCharSet;
    m_nOutPrecision         = nOutPrecision;
    m_nClipPrecision        = nClipPrecision;
    m_nQuality              = nQuality;
    m_nPitchAndFamily       = nPitchAndFamily;
    m_csFacename            = lpszFacename;

    // ��վ���Դ

    Clear();
}

CFont* dsm_fontmgr::GetFont( int nHeight )
{
    CFont* pFont = NULL;

    // ���һ���
    if(FALSE == m_mapFont.Lookup(nHeight, pFont) || !pFont->GetSafeHandle())
    {
        // �Ҳ����������壬���½������뻺���

        if(!pFont)
        {
            pFont = new CFont;
        }

        pFont->CreateFont(
                    nHeight, 
                    m_nWidth, 
                    m_nEscapement, 
                    m_nOrientation, 
                    m_nWeight, 
                    m_bItalic, 
                    m_bUnderline, 
                    m_cStrikeOut, 
                    m_nCharSet, 
                    m_nOutPrecision, 
                    m_nClipPrecision, 
                    m_nQuality, 
                    m_nPitchAndFamily, 
                    m_csFacename);

        ASSERT(pFont->GetSafeHandle());

        CString cs;
        cs.Format(L"dsm: dsm_fontmgr: create object %d -- 0x%08X\n", nHeight, pFont->m_hObject);
        TRACE(cs);

        m_mapFont.SetAt(nHeight, pFont);
    }

    ASSERT(pFont->GetSafeHandle());

    return pFont;
}


dsm_fontmgr* dsm_fontmgr::GetInstance()
{
    static dsm_fontmgr s_fontMgr;

    return &s_fontMgr;
}


void dsm_fontmgr::Clear()
{
    int nKey;
    CFont* pValue = NULL;
    POSITION pos = m_mapFont.GetStartPosition();

    while(pos)
    {
        m_mapFont.GetNextAssoc(pos, nKey, pValue);

        if(pValue)
        {
            CString cs;
            cs.Format(L"dsm: dsm_fontmgr: delete object %d -- 0x%08X\n", nKey, pValue->m_hObject);
            TRACE(cs);

            ASSERT(pValue->GetSafeHandle());
            HGDIOBJ obj = pValue->Detach();
            ::DeleteObject(obj);

            cs.Format(L"dsm: dsm_fontmgr: delete object %d -- 0x%08X\n", nKey, pValue->m_hObject);
            TRACE(cs);

            delete pValue;

            pValue = NULL;
        }
    }

    m_mapFont.RemoveAll();
}

