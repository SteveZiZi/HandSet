/**
 * \file
 *      CountDownWnd.h
 *
 * \brief
 *      ����ʱ��Ϣ��ʾ��ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/12/10
 */


#pragma once


// CCountDownWnd

class CCountDownWnd : public CStatic
{
	DECLARE_DYNAMIC(CCountDownWnd)

public:
	CCountDownWnd();
	virtual ~CCountDownWnd();

public:
    afx_msg void OnPaint();
    virtual BOOL DestroyWindow();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

public:

    /**
     *      ���õ���ʱ����
     */
    inline void _SetSeconds(unsigned int nSeconds)
    {
        m_nLeftSeconds  = nSeconds;
        m_nTotalSeconds = nSeconds;
    }

    /**
     *      ������ʾ��Ϣ��ʽ�ַ���
     */
    inline void _SetTipsFmt(const CString csTips)
    {
        m_csTipsFmt = csTips;
    }

    /**
     *      ���ñ���ɫ
     */
    inline void _SetBKColor(COLORREF color)
    {
        m_crBackground = color;
    }

    /**
     *      ��ʼ���ؼ�
     */
    void _Init();


protected:
	DECLARE_MESSAGE_MAP()

private:
    COLORREF            m_crBackground;         ///< ����ɫ
    CBitmap             m_panel;                ///< ����
    UINT                m_nTimer;               ///< ��ʱ��ID
    unsigned int        m_nLeftSeconds;         ///< ʣ��ʱ������
    unsigned int        m_nTotalSeconds;        ///< �ܹ�ʱ��
    CString             m_csTipsFmt;            ///< ��ʾ��Ϣ��ʽ
    CString             m_csTips;               ///< ��ʾ��Ϣ
};


