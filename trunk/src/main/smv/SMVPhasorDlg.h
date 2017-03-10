/**
 * \file
 *      SMVPhasorDlg.h
 *
 * \brief
 *      ����
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/20
 */

#pragma once

#include "src/main/smv/smvbasedlg.h"
#include "src/utils/reportcontrol/listctrlel/listctrlel.h"
#include "src/utils/electriccontrol/phasorgramctrl/phasorgramctrl.h"



class CCtrlBlkDataPhasor;


// CSMVPhasorDlg �Ի���

class CSMVPhasorDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSMVPhasorDlg)

public:
	CSMVPhasorDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSMVPhasorDlg();

// �Ի�������
	enum { IDD = IDD_SMV_PHASOR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnMenuF4();
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnItemSelectChange( NMHDR* pNotifyStruct, LRESULT *pResult);


public:

    /**
     *      ����ҳ��״̬��Ϣ
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
     *      nCmdID          ���ƿ�ӳ�������˵�ID
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);


private:

    /**
     *      ������������
     */
    void _createReport();

    /**
     *      ���豨����ͨ������
     */
    void _resetReport();

    /**
     *      ��������ͼ
     */
    void _createPhasorGram();

    /**
     *      ˢ�½����ֵ
     */
    void _updatePhasor();

    /**
     *      ������ʼѡ�е�3��ͨ��������ѡ�к��ʵ�ABC����
     */
    void _adjustChannel();

    /**
     *      Ѱ��3�����ʵ�Ĭ����ʾͨ����
     *
     *  \note
     *      ѡ�е�����Ĭ��ͨ�������������ͨ��������ҳ�����ݶ�����
     */
    void _calcChannel();

    /**
     *      ���ط���������ͨ����
     *
     *  \param[in]
     *      eType           ָ����ͨ������
     *  \param[in]
     *      ePhase          ָ����ͨ�����
     *  \param[in]
     *      nStart          ͨ���Ĳ�ѯ��ʼλ��
     *  \return
     *      uint32_t        ���ط���������ͨ���ţ��Ҳ����򷵻� UINT_MAX
     */
    uint32_t _findChannel(enumChannelType eType, ChannelPhase ePhase, int nStart);

    /**
     *      ʹ��ҳ����������ͨ��
     */
    bool _restorePage();

    /**
     *      ������ѡͨ���ĵ�λ
     */
    void _updateUnit();

    /**
     *      ���ͨ���Ƿ�仯
     *
     *  \return
     *      bool        �����仯����true��û�仯����false
     */
    bool _checkChannel();



private:

    DWORD                       m_dwMode;           ///< F3����ģʽ

    CReportVM                   m_reportChannel;    ///< ͨ������
    CPhasorGramCtrl             m_ctrlPhasor;       ///< �����ؼ�
    CDataPhasor                 m_data;             ///< ����ҵ������

    CString                     m_csUnit[4];        ///< ��ʾ��ֵ��λ
    float                       m_fScaleInterval;   ///< ����ͼ�̶�ֵ
    int                         m_nTimes;           ///< ���ű���

    CCtrlBlkDataPhasor*         m_pCurCtrlBlkData;  ///< ��ǰ���ƿ��������

    // �Ż���ʱ����������ʱ���ݶ���Ĵ���

    float                       m_fLen[4];          ///< ��Чֵ
    float                       m_fValue[4];        ///< ��λ
    CString                     m_strDesc[4];       ///< ͨ������

    enumSmvValueType            m_eDispType;        ///< ͨ��ֵ����ʾ����
    enumSmvValueType            m_eFrameType;       ///< �ͨ�����͵���ʾ����
};
