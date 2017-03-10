/**
 * \file
 *      StatusDisp.h
 *
 * \brief
 *      ͼƬ��ʾС�ؼ�ͷ�ļ�
 *
 *  \note
 *      ͼƬ�ؼ��������ö��״̬��ÿ��״̬��ʾ��ͬ��ͼƬ��Ϣ
 *      ��ĳ��״̬�����˶��ͼƬʱ����������ʾÿ��ͼƬ
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/6/27
 */

#pragma once

#include <vector>

// CStatusDisp

class CStatusDisp : public CStatic
{
	DECLARE_DYNAMIC(CStatusDisp)

public:
	CStatusDisp();
	virtual ~CStatusDisp();

protected:
	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnPaint();


public:


    /**
     *      ����һ��״̬
     */
    void _InsertStatus(int nStatus, std::vector<HBITMAP>& vbitmap);

    /**
     *      ���ö�ʱ��ʱ��
     */
    void _SetTimeInterval(UINT interval)
    {
        m_nInterval = interval;
    }

    /**
     *      ���õ�ǰ״̬
     */
    void _SetStatus(int nStatus);

    /**
     *      ��ȡ��ǰ״̬
     */
    inline int _GetStatus()
    {
        return m_nStatus;
    }


private:

    typedef std::vector<HBITMAP>            CBValue;
    typedef std::map<int, CBValue*>         CMapStatus;
    

    int                 m_nStatus;          ///< ��ǰ״̬

    UINT                m_nInterval;        ///< Ĭ�϶�ʱ�����
    UINT                m_nTimerID;         ///< ��ʱ��ID

    int                 m_nCount;           ///< ������

    CBValue*            m_pValue;           ///< ��ǰ״̬��ͼƬ����
    CMapStatus          m_mapStatus;        ///< ״̬ͼƬӳ��

};


