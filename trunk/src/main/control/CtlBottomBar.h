/**
 * \file
 *      CtlBottomBar.h
 *
 * \brief
 *      �·�״̬���ؼ�ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/7/6
 */
#pragma once


// CCtlBottomBar

class CCtlBottomBar : public CStatic
{
	DECLARE_DYNAMIC(CCtlBottomBar)

public:
	CCtlBottomBar();
	virtual ~CCtlBottomBar();

protected:
	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL DestroyWindow();
    afx_msg void OnPaint();

public:

    /**
     *      ������ʱ��
     */
    void _StartTimer()
    {
        if(UINT_MAX == m_nTimer)
        {
            m_nTimer = SetTimer(1, 1000, NULL);
        }
    }

private:

    UINT                m_nTimer;           ///< ˢ��ʱ��Ķ�ʱ��ID
    CBitmap             m_panel;            ///< ����
    SYSTEMTIME          m_systime;          ///< ʱ����Ϣ�ṹ
    CString             m_csTime;           ///< ʱ����ʾ

	CBitmap				m_bmpIcon;			///< ��ʾͼ��

    CFont*              m_pFont;            ///< ��ʾ��Ϣ���������

    CRect               m_rcSerifTopA;      ///< ������������
    CRect               m_rcSerifTopB;      ///< ���������·���������
    CRect               m_rcSerifBottom;    ///< �ײ���������
};


