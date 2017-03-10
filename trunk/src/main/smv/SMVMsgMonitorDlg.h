/**
 * \file
 *      SMVMsgMonitorDlg.h
 *
 * \brief
 *      ���ļ�⼰¼��ҳ��ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/5/11
 */

#pragma once

#include "src/main/smv/smvbasedlg.h"

class CMsgDlg;

// CSMVMsgMonitorDlg �Ի���

class CSMVMsgMonitorDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSMVMsgMonitorDlg)

public:
	CSMVMsgMonitorDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSMVMsgMonitorDlg();

// �Ի�������
	enum { IDD = IDD_SMV_MSGMONITOR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnMenuF3();
    afx_msg void OnMenuF5();
    afx_msg void OnMenuF6();
#if	!SU_FEI
	
	afx_msg void OnMenuF4();

#endif
    virtual BOOL DestroyWindow();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg LRESULT OnHandleRecordException(WPARAM wParam, LPARAM lParam);


protected:
    /**
     *      �޸Ĳ˵�HMENU
     */
    virtual void _modifyMenu();

    /**
     *      ��ʼ����ǰ���ƿ�
     */
    virtual void _initCurrentCtrlBlk() {}

    /**
     *      ���ݲ˵�ID�л���ָ���Ŀ��ƿ�
     *
     *  \note
     *      ����ѡ����ƿ���Ҫ���ظ÷�����һЩ�������
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);

private:

    /**
     *      ��ʾsmv֡����
     */
    void _showFrame();

    /**
     *      ����֡����
     */
    void _updateFrame(enumFrameType type, const uint8_t* pBuff, int bufflen);

    /**
     *      �������ؼ�
     */
    void _createTree();

    /**
     *      ˢ��һ֡����
     */
    void _refreshFrame();

    /**
     *      ¼��ǰ�����·��SD״̬
     */
    bool _checkforRecordSMV();

    /**
     *      smv¼��
     */
    bool _recordSmv();

    /**
     *      smv 9x¼��
     */
    bool _recordSmv9x();

    /**
     *      smv ft3¼��
     */
    bool _recordSmvft3();

    /**
     *      ����¼���ļ�·��
     */
    void _genRecordFile();

    /**
     *      ��ʾ¼��״̬��
     */
    INT_PTR _showRecordDlg();

    /**
     *      ����¼������������
     */
    bool _handleRecord();


private:

    CTreeV                  m_treeFrame;        ///< ����֡���ؼ�

    Sav91Msg*               m_pMsg91;           ///< 91Э�������
    Sav92Msg*               m_pMsg92;           ///< 92Э�������
    NanRuiFT3Msg*           m_pMsgNanRui;       ///< FT3����Э�������
    GuoWangFT3Msg*          m_pMsgGuoWang;      ///< FT3����Э�������


    CString                 m_csPcapFiles;      ///< ��ǰ������pcap�ļ�
    CDataRealTimeMsgData    m_data;             ///< ʵʱ��������
    int                     m_nTreeRows;        ///< ���ؼ�һҳ����ʾ����
    CMsgDlg*                m_pMsgDlg;          ///< ¼��״̬��
    UINT                    m_nRealTimer;       ///< ��һ�λ�ȡʵʱ���ĵĶ�ʱ�������һ�����ݺ�رն�ʱ��
    UINT                    m_nRecordTimer;     ///< ¼��ʱ�����·�Լ�SD��״̬��ʱ��

#if !CHUN_HUA
	CString					m_csPcapPath;
#endif
};
