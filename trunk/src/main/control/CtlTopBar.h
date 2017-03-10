/**
 * \file
 *      CtlTopBar.h
 *
 * \brief
 *      ����״̬����Ϣͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/7/6
 */

#pragma once

#include "src/include/p_filemapping_def.h"

// CCtlTopBar

class CCtlTopBar : public CStatic
{
	DECLARE_DYNAMIC(CCtlTopBar)

public:
	CCtlTopBar();
	virtual ~CCtlTopBar();

protected:
	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnPaint();


public:

    /**
     *      ���ñ���
     */
    inline void _SetTitle(const CString& csTitle)
    {
        m_csCaption = csTitle;
        InvalidateRect(m_rcCaption, TRUE);
        UpdateWindow();
    }

    /**
     *      ����״̬��ICON������Ч
     */
    inline void _InvalidateICON()
    {
        InvalidateRect(m_rcICONStatusAll, TRUE);
    }

    // topbar״̬��Ϣ
private:

    CBitmap             m_panel;                    ///< ��������

    CString             m_csCaption;                ///< �����ַ���

    CRect               m_rcCaption;                ///< ����������
    CRect               m_rcICONStatusAll;          ///< icon����
	bool                m_bSD;                      ///< SD����״̬
#if !SHENG_ZE
	bool                m_bUCard;                   ///< U�̽���״̬
#endif
};


