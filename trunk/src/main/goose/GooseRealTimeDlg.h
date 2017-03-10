/**
 * \file
 *      GooseRealTimeDlg.h
 *
 * \brief
 *      gooseʵʱֵҳ��ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/5/9
 */

#pragma once

#include "src/main/goose/goosebasedlg.h"

#include "src/utils/reportcontrol/listctrlel/listctrlel.h"


class CPageDataExtra;

// CGooseRealTimeDlg �Ի���

class CGooseRealTimeDlg : public DSM_GOOSEASEDLG
{
	DECLARE_DYNAMIC(CGooseRealTimeDlg)

public:
	CGooseRealTimeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CGooseRealTimeDlg();

// �Ի�������
	enum { IDD = IDD_GOOSE_REALTIME_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
	afx_msg void OnMenuF3();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL PreTranslateMessage(MSG* pMsg);


public:
    /**
     *      ����ҳ��
     */
    virtual void _SavePage();

protected:

	 /**
     *      �޸Ĳ˵�HMENU
     */
    virtual void _modifyMenu();

    /**
     *      ���ݲ˵�ID�л���ָ���Ŀ��ƿ�
     *
     *  \param[in]
     *      nCmdID          �˵�����ID
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);


private:
    /**
     *      ����ʵʱֵ����
     */
    void _createReport();

    /**
     *      ����ʵʱֵ��������
     */
    void _updateData();

    /**
     *      ˢ�±���ײ���stNum,sqNum,TTL״̬
     */
    void _updateStatus(bool bNull = false);

    /**
     *      �������·�ҳ�ڵ�һ����ʾ��ͨ����ź���ͨ�����
     *
     *  \return
     *      bool            ������ҳ����true�����򷵻�false
     */
    bool _calcDown();

    /**
     *      �������Ϸ�ҳ�ڵ�һ����ʾ��ͨ����ź���ͨ�����
     *
     *  \return
     *      bool            ������ҳ����true�����򷵻�false
     */
    bool _calcUp();

    /**
     *      ͨ����ҳ - ��һҳ
     */
    void _channelUp();

    /**
     *      ͨ����ҳ - ��һҳ
     */
    void _channelDown();

    /**
     *      ��ԭҳ������
     */
    bool _restorePage();

    /**
     *      ����һ��ͨ����¼
     *
     *  \param[in]
     *      pChannel        ͨ������
     *  \param[in]
     *      nChannel        goose����ͨ����
     *  \param[in]
     *      nSubChannel     goose������ͨ����
     *  \param[in]
     *      nRow            �����к�
     */
    void _insertRecord(CGooseChannel* pChannel, int nChannel, int nSubChannel, int nRow);

    /**
     *      ���gooseͨ�������������Ƿ�仯
     *
     *  \param[in]
     *      bool            ͨ�����������ͱ仯����true��û�仯����false
     */
    bool _isChannelChange();

    /**
     *      ����ҳ���ʼ״̬
     *
     *  \note
     *      ������ʱ����ͨ�������������ͷ����仯��
     *      ҳ���״̬�����ص�һ���µĳ�ʼ״̬(��һҳ��
     */
    void _resetPage();


private:

    int                     m_nRows;            ///< ÿҳ�ɼ�����
    int                     m_nChann;           ///< ͨ����������0����
    int                     m_nSubChann;        ///< ��ͨ����������0����
    int                     m_nPage;            ///< ��ǰҳ��

    CRect                   m_rcStatus;         ///< stNum״̬������
    CBitmap                 m_panel;            ///< ״̬�����򻭲�

    CReportV                m_reportGoose;      ///< ʵʱֵ����
    CGooseInfo              m_data;             ///< gooseʵʱֵ����

    bool                            m_bInitChannelData;         ///< �Ƿ��ʼ��ͨ������
    std::vector<CGooseChannel*>     m_vecChannels;              ///< ���ƿ��Ӧ��ͨ����Ϣ
	bool					m_bIsShowDetail;
};
