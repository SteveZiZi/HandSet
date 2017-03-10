/**
 * \file
 *      NaviBarMgr.h
 *
 * \brief
 *      ������������
 *
 *  \note
 *      ҵ�񴰿�ͨ���ù����������Լ��ĵ�����
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/13
 */

#pragma once

#include "src/utils/navicontrol/navimenubar/navitoolbar.h"
#include "src/main/resource.h"

/**
 *      ������������
 */
class CNaviBarMgr
{
public:
    /**
     *      ��ȡ��̬ʵ������
     */
    static CNaviBarMgr* GetInstance()
    {
        static CNaviBarMgr m_sInst;

        return &m_sInst;
    }

    /**
     *      �󶨵�����
     *
     *  \param[in]
     *      pNaviBar            �������ĵ�����ָ��
     *  \note
     *      �����ڳ�ʼ��ʱ�Ȱ󶨵���������ʹ��
     */
    void SetNaviBar(CNaviToolBar* pNaviBar);

    /**
     *      ��ȡ������
     *
     *  \return
     *      CNaviToolBar*       ���ص�����
     */
    CNaviToolBar* GetNaviBar();

    /**
     *      �����Ƿ����ĳ��
     *
     *  \param[in]
     *      nSub                �������
     *  \param[in]
     *      nPos                ���������
     *  \param[in]
     *      bHighlight          ������־
     */
    void HighlightMenuItem(int nSub, int nPos, BOOL bHighlight = TRUE);

    /**
     *      ����inpalceedit�༭�˵�
     */
    CWnd* EnterInplaceEdit(CWnd* target, UINT uMenuID = IDR_MENU_INPLACEEDIT);

    /**
     *      �뿪inplceedit�༭�˵�
     */
    void LeaveInplaceEdit(CWnd* target = NULL);

private:
    CNaviBarMgr(void);

    ~CNaviBarMgr(void);

private:
    CNaviToolBar*               m_pNaviBar;             ///< �����ڵĵ������˵�

    CNaviMenu                   m_menuInplaceEdit;      ///< �༭�˵�
    CNaviMenu*                  m_pMenuLast;            ///< ��һ��Ŀ�괰�ڹ����Ĳ˵�(inplace edit)
    CWnd*                       m_pPreEditWnd;          ///< �༭ǰ����
};
