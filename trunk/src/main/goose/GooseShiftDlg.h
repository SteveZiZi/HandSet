/**
 * \file
 *      GooseShiftDlg.h
 *
 * \brief
 *      goose��λҳ��ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/5/10
 */

#pragma once

#include "src/main/goose/goosebasedlg.h"


/**
 *      ��λģʽ
 */
enum ModeTime
{
    TIME_REL,           ///< ���
    TIME_ABS            ///< ����
};

/**
 *      ��ϸ��Ϣor�б�ģʽ
 */
enum ModePage
{
    PAGE_LIST,          ///< ��λ�б�
    PAGE_DETAIL         ///< ��ϸ��Ϣ
};

/**
 *      ȫ����Ϣ���λ��Ϣ
 */
enum ModeShift
{
    SHIFT_ALL,          ///< ȫ����Ϣ
    SHIFT_ONLY          ///< ������λ��Ϣ
};


class CPageDataExtra;

// CGooseShiftDlg �Ի���

class CGooseShiftDlg : public DSM_GOOSEASEDLG
{
	DECLARE_DYNAMIC(CGooseShiftDlg)

public:
	CGooseShiftDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CGooseShiftDlg();

// �Ի�������
	enum { IDD = IDD_GOOSE_SHIFT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnMenuF3();
    afx_msg void OnMenuF6();
    afx_msg void OnMenuShiftF6();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnClickOK(NMHDR* pNotifyStruct, LRESULT* pResult);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

protected:
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

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
     *      nCmdID          �˵�������ID
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);


private:

    /**
     *      ������λ��Ϣ����
     */
    void _createReport();

    /**
     *      ������ϸ��Ϣ����
     */
    void _createReportDetail();

    /**
     *      ˢ��goose��λ��Ϣ
     */
    void _updateData();

    /**
     *      ��ԭҳ������
     */
    bool _restorePage();

    /**
     *      ˢ�½�����ʾ�ؼ�Ԫ�أ������ݣ�
     */
    void _switchMenu();

    /**
     *      ����F6ͨ���˵�
     */
    void _resetF6Channel();

    /**
     *      ����ͨ��
     *
     *  \param[in]
     *      nItem       ͨ���Ŷ�Ӧ�Ĳ˵�ID
     */
    void _filter(int nItem);

    /**
     *      �л���ʾ����ʱ�������ʱ��
     */
    void _switchAbsoRel();

    /**
     *      ��ʾ��ϸ��λ��Ϣ
     */
    void _showDetail();

    /**
     *      ������ϸ��λ��Ϣ
     */
    void _updateDetail();

    /**
     *      ����һ����ϸ��λ��¼
     *
     *  \param[in]
     *      pChanBef        ��λǰͨ��
     *  \param[in]
     *      pChannAft       ��λ��ͨ��
     *  \param[in]
     *      nChannel        ͨ����
     *  \param[in]
     *      nSubChannel     ��ͨ����
     *  \param[in]
     *      nRow            �����к�
     */
    bool _insertRecord(CGooseChannel* pChanBef, CGooseChannel* pChannAft, int nChannel, int nSubChannel, int nRow);

    /**
     *      �������Ϸ�ҳ�ڵ�һ����ʾ��ͨ����ź���ͨ�����
     *
     *  \return
     *      bool            ������ҳ����true�����򷵻�false
     */
    bool _calcUp();

    /**
     *      �������·�ҳ�ڵ�һ����ʾ��ͨ����ź���ͨ�����
     *
     *  \return
     *      bool            ������ҳ����true�����򷵻�false
     */
    bool _calcDown();

    /**
     *      ͨ����ҳ - ��һҳ
     */
    void _channelDetailUp();

    /**
     *      ͨ����ҳ - ��һҳ
     */
    void _channelDetailDown();

    /**
     *      ���㵥����λ��Ϣ�Ϸ�ҳ
     *
     *  \return
     *      bool        ������ҳ����true�����򷵻�false
     */
    bool _calcUpOnly();

    /**
     *      ���㵥����λ��Ϣ�·�ҳ
     *
     *  \return
     *      bool        ������ҳ����true�����򷵻�false
     */
    bool _calcDownOnly();

    /**
     *      ��ʾ������λ��Ϣ
     */
    void _updateShiftDetail();

    /**
     *      ���detal����
     *
     *  \param[in]
     *      csContent       ����ַ�������
     */
    inline void _fillDetailReport(CString csContent);

    /**
     *      ��ȡָ�������ı�λ����
     *
     *  \param[in]
     *      nIndex      ��λ��������
     *  \param[out]
     *      change      ��λ����
     *  \return
     *      bool        ��ȡ���ݳɹ�����true��ʧ�ܷ���false
     */
    bool _getChange(uint32_t nIndex, CGooseOneTimeChange& change);


private:

    ModeTime                m_eTime;            ///< ��Ի����ʱ��
    ModePage                m_ePage;            ///< ��ʾ��λ�б�orͨ����ϸ��Ϣ
    ModeShift               m_eShift;           ///< ȫ����Ϣ���ڵ�����λ��Ϣ

    CReportV                m_reportGoose;      ///< ��λ��Ϣ����
    CReportV                m_reportDetail;     ///< ͨ����ϸ��Ϣ����

    CDataGooseChangeList    m_data;             ///< goose��λ����

    int                     m_nChannel;         ///< ѡ�е�item����һ����ȫ����ʾ��������ͨ������
    DWORD                   m_dwLast;           ///< F6�����˵�item��

    uint32_t                m_nDetailIndex;     ///< ��ʾ��ϸ��λ��Ϣ������

    CNaviMenu*              m_pMenuList;        ///< ��λ��Ϣ�б�ҳ��˵�
    CNaviMenu*              m_pMenuDetail;      ///< ��ϸ��λ��Ϣҳ��˵�

    bool                    m_bRecounter;       ///< ���²�ѯ���б�λ��Ϣ��¼

    int                     m_nRows;            ///< ÿҳ�ɼ�����
    int                     m_nParentChann;     ///< ͨ����������0����
    int                     m_nSubChann;        ///< ��ͨ����������0����

    int                     m_nLastFocus;       ///< �л���ϸ��λ��Ϣʱ�����λ�б�Ľ�����

    uint32_t                m_nShiftIndex;      ///< ������λ��Ϣ�ı�λͨ������
    int                     m_nShiftSubIndex;   ///< ������λ��Ϣ�ı�λͨ������ͨ������
};
