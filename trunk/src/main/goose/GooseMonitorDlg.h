/**
 * \file
 *      GooseMonitorDlg.h
 *
 * \brief
 *      goose���ļ��ҳ��ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/5/10
 */

#pragma once

#include "src/main/goose/goosebasedlg.h"

// CGooseMonitorDlg �Ի���

class CMsgDlg;
class CRNaviMenu;

class CGooseMonitorDlg : public DSM_GOOSEASEDLG
{
	DECLARE_DYNAMIC(CGooseMonitorDlg)

public:
	CGooseMonitorDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CGooseMonitorDlg();

// �Ի�������
	enum { IDD = IDD_GOOSE_MONITOR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    //afx_msg void OnMenuF3();
    afx_msg void OnMenuF4();
    afx_msg void OnMenuF3();
    virtual BOOL DestroyWindow();
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg LRESULT OnHandleRecordException(WPARAM wParam, LPARAM lParam);

protected:
    /**
     *      �޸Ĳ˵�HMENU
     */
    virtual void _modifyMenu();

    /**
     *      ���ݲ˵�ID�л���ָ���Ŀ��ƿ�
     *
     *  \note
     *      ����ѡ����ƿ���Ҫ���ظ÷�����һЩ�������
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);

private:

    /**
     *      �������ؼ�
     */
    void _createTree();

    /**
     *      ��ʾsmv֡����
     */
    void _showFrame();

    /**
     *      ����֡����
     *
     *  \param[in]
     *      type            ֡����
     *  \param[in]
     *      pBuff           ֡���ݻ�����
     *  \param[in]
     *      bufflen         ֡���ݻ���������
     */
    void _updateFrame(enumFrameType type, const uint8_t* pBuff, int bufflen);

    /**
     *      goose¼��
     */
    bool _recordGoose();

    /**
     *      goose¼������
     */
    bool _recordGooseEx();

    /**
     *      ¼��ǰ�����·��SD״̬
     */
    bool _checkforRecordGoose();

    /**
     *      ����¼���ļ�·��
     */
    void _genRecordFileName();

    /**
     *      ����¼������������
     */
    bool _handleRecord();

    /**
     *      ��ʾ¼��״̬��
     */
    INT_PTR _showRecordDlg();

private:

    CTreeV                  m_treeFrame;        ///< ����֡���ؼ�
    CString                 m_csPcapFiles;      ///< ��ǰ������pcap�ļ�
    CDataRealTimeMsgData    m_data;             ///< ʵʱ��������
    GooseMsg*               m_pGooseMsg;        ///< goose�������
    int                     m_nMaxRows;         ///< һҳ������ʾ����
    UINT                    m_nRealTimer;       ///< ��һ�λ�ȡʵʱ���ĵĶ�ʱ�������һ�����ݺ�رն�ʱ��
    UINT                    m_nDetectTimer;     ///< ¼��ʱ���SD��������״̬�Ķ�ʱ��
    CMsgDlg*                m_pMsgDlg;          ///< ¼��״̬��
    CRNaviMenu*             m_pRNaviMenu;       ///< �˵���Դ�������

#if !CHUN_HUA
	CString					m_csRecordPath;
#endif
};
