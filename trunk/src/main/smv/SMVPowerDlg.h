/**
 * \file
 *      SMVPowerDlg.h
 *
 * \brief
 *      ����ҳ��ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/22
 */

#pragma once

#include "src/main/smv/smvbasedlg.h"
#include "src/main/pagedata/pagedata.h"


class CCtrlBlkDataPower;

// CSMVPowerDlg �Ի���

class CSMVPowerDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSMVPowerDlg)

public:
	CSMVPowerDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSMVPowerDlg();

// �Ի�������
	enum { IDD = IDD_SMV_POWER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()


public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnItemSelectChange( NMHDR* pNotifyStruct, LRESULT *pResult);
    afx_msg void OnInplaceButtonDown( NMHDR* pNotifyStruct, LRESULT *pResult);

public:

    /**
     *      ���浱ǰ���ƿ������ҳ��״̬����
     */
    virtual void _SavePage();

	virtual void _modifyMenu();

protected:

    /**
     *      ���ݲ˵�ID�л���ָ���Ŀ��ƿ�
     *
     *  /param[in]
     *      nCmdID      ���ƿ�����Ĳ˵�����ID
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);

private:

    /**
     *      ��������
     */
    void _createReport();

    /**
     *      ��ʼ������
     */
    void _initReport();

    /**
     *      ��Ӧ��ʱ����Ϣ�����±�������
     */
    void _updateReport();

    /**
     *      ���ݿ��ƿ������ҳ��״̬�������������ѹͨ����Ϣ
     */
    void _resetChannel();

    /**
     *      Ϊָ���кż���ָ��ͨ�����͵�ͨ����Ϣ
     *
     *  \param[in]
     *      nRow        ָ�����к�
     *  \param[in]
     *      eType       ָ����ͨ������
     */
    void _loadItemChannel(int nRow, enumChannelType eType);

    /**
     *      ΪABC����ͨ��ѡ�к��ʵ�Ĭ��ͨ��
     */
    void _adjustChannel();

    /**
     *      ��ʾ���ʵ�Ĭ����ʾͨ����
     *
     *  \param[in]
     *      eType       ͨ������
     *  \param[in]
     *      pData       ��ǰ���ƿ����������
     */
    void _calcChannel(enumChannelType eType, CCtrlBlkDataPower* pData);

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
     *      ʹ�ÿ��ƿ��ҳ��״̬�������ñ���
     */
    bool _restorePage();

    /**
     *      ���ͨ���Ƿ����仯
     */
    bool _checkChannel();

    /**
     *      ��ȡ��׼��λֵ
     */
    float _getBasePhaseValue();


private:

    CDataPower              m_data;             ///< ��������
    CReportV                m_reportPower;      ///< ������Ϣ��ʾ����

    // ��ֹ����Ƶ�����µ�Ӱ��
    float                   m_fVolt[3];         ///< ��ѹ��Чֵ
    float                   m_fVoltAngle[3];    ///< ��ѹ��λ��
    float                   m_fAmper[3];        ///< ������Чֵ
    float                   m_fAmperAngle[3];   ///< ������λ��
    float                   m_fActive[4];       ///< �й�����
    float                   m_fReactive[4];     ///< �޹�����
    float                   m_fApparent[4];     ///< ���ڹ���
    float                   m_fFactor[4];       ///< ��������

    enumSmvValueType        m_eDispType;        ///< ͨ��ֵ����ʾ����
    enumSmvValueType        m_eFrameType;       ///< �ͨ�����͵���ʾ����

    CCtrlBlkDataPower*      m_pCtrlBlkData;     ///< ��ǰ���ƿ��ҳ������
};
