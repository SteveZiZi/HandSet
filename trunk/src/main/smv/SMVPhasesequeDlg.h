/**
 * \file
 *      SMVPhasesequeDlg.h
 *
 * \brief
 *      ����ҳ��ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/21
 */

#pragma once

#include "src/main/smv/smvbasedlg.h"

#include "src/utils/reportcontrol/listctrlel/listctrlel.h"
#include "src/utils/electriccontrol/phasorgramctrl/phasorgramctrl.h"

class CPageDataExtra;
class CCtrlBlkDataPhasaSeq;



// CSMVPhasesequeDlg �Ի���

class CSMVPhasesequeDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSMVPhasesequeDlg)

public:
	CSMVPhasesequeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSMVPhasesequeDlg();

// �Ի�������
	enum { IDD = IDD_SMV_PHASESEQU_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnMenuF4();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnInplaceButtonDown(NMHDR* pNotifyStruct, LRESULT* pResult);


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
     *      nCmdID          ����˵�ID���������һ��ӳ��Ĺ�ϵ
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);


private:

    /**
     *      ������λ����
     */
    void _createReport();

    /**
     *      ��������ͼ
     */
    void _createPhaseGram();

    /**
     *      ���豨����ͨ������
     */
    void _resetReport();

    /**
     *      ˢ�½�������
     */
    void _updatePhasae();

    /**
     *      Ѱ��3�����ʵ�Ĭ����ʾͨ����
     *
     *  \note
     *      ѡ�е�����Ĭ��ͨ�������������ͨ��������ҳ�����ݶ�����
     */
    void _adjustChannel();

    /**
     *      Ѱ�Һ���ͨ��ֵ
     *
     */
    void _calcChannel(CCtrlBlkDataPhasaSeq* pData);

    /**
     *      ʹ��ҳ����������ͨ��
     */
    bool _restorePage();

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
     *      ������ѡͨ���ĵ�λ
     */
    void _updateUnit();

    /**
     *      ���ͨ���Ƿ����仯
     */
    bool _checkChannel();


private:

    CDataXuliang                m_data;             ///< ����ҵ������
    CReportVM                   m_reportPhase;      ///< ͨ������
    CPhasorGramCtrl             m_ctrlPhasor;       ///< �����ؼ�

    DWORD                       m_dwMode;           ///< F3����ģʽ

    CString                     m_csUnit[4];        ///< ��ʾ��ֵ��λ
    int                         m_nTimes;           ///< ���ű���
    float                       m_fScaleInterval;   ///< ����ͼ�̶�ֵ

    // �Ż�Ƶ����������ʱ�����������ͷ�
    // �������򡢸���
    float                       m_fAngel[3];        ///< ��λ��
    float                       m_fValue[3];        ///< ��Чֵ
    CString                     m_strDesc[3];       ///< ͨ������

    enumSmvValueType            m_eDispType;        ///< ͨ��ֵ����ʾ����
    enumSmvValueType            m_eFrameType;       ///< �ͨ�����͵���ʾ����
};
