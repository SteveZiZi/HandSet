/**
 * \file
 *      SMVDualphaseDlg.h
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

class CPageDataExtraDualPhase;


/**
 *      ҳ��״̬����������
 */
class CPageDataExtraDualPhase : public IPageDataExtra
{
public:
    CPageDataExtraDualPhase(CPageData* pPageData)
        : pPageData(pPageData)
        , nFoucs(0) 
        , nSmv1(UINT_MAX) 
        , nSmv2(UINT_MAX) 
        , bGram(true)
        , bSame(true)
    {
        for(int i = 0; i < 3; i++)
        {
            nChannelA[i]        = UINT_MAX;
            nChannelB[i]        = UINT_MAX;
            bChannelValidA[i]   = false;
            bChannelValidB[i]   = false;
        }

    }
    virtual ~CPageDataExtraDualPhase() {}

public:

    /**
     *      ��ȡ����������󶨵�����ҳ������ָ��
     */
    virtual CPageData* GetPageData()
    {
        return pPageData;
    }

public:
    int                 nFoucs;                 ///< �����к�

    bool                bGram;                  ///< true ����ͼģʽ��false ���ģʽ
    bool                bSame;                  ///< true ͬ�� false ���

    uint32_t            nSmv1;                  ///< SMV1�Ŀ��ƿ�ID
    uint32_t            nSmv2;                  ///< SMV2�Ŀ��ƿ�ID

    uint32_t            nChannelA[3];           ///< SMV1ѡ��ͨ����
    uint32_t            nChannelB[3];           ///< SMV2ѡ��ͨ����

    bool                bChannelValidA[3];      ///< SMV1ѡ��ͨ����Ч��
    bool                bChannelValidB[3];      ///< SMV2ѡ��ͨ����Ч��

private:
    CPageData*          pPageData;              ///< �������ݹ�����ҳ������ָ��
};

// CSMVDualphaseDlg �Ի���

class CSMVDualphaseDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSMVDualphaseDlg)

public:
	CSMVDualphaseDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSMVDualphaseDlg();

// �Ի�������
	enum { IDD = IDD_SMV_DUALPHASE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnMenuF2();
  //  afx_msg void OnMenuF3();
    afx_msg void OnMenuF4();
    afx_msg void OnMenuF5();
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnItemSelectChange( NMHDR* pNotifyStruct, LRESULT *pResult);
    afx_msg void OnInplaceButtonDown( NMHDR* pNotifyStruct, LRESULT *pResult);


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
     *      ��ֹ���ؿ��ƿ鵽F1�˵���
     */
    void _loadCtrlBlk() {}

    /**
     *      ��ʼ����ǰ�����ҳ������
     */
    virtual void _initCurrentCtrlBlk();


private:
    /**
     *      ����ҵ�������ö��
     */
    enum ePhaseOrder
    {
        ORDER_PHASE_A,          ///< ��һ��
        ORDER_PHASE_B,          ///< �ڶ���
    };


private:

    /**
     *      ��ʼ������ҵ���smv1��smv2���ƿ��Լ���ѡ�е�ABC����ͨ��
     */
    void _initPageStatus();


    /**
     *      ���������౨��
     */
    void _createReport();

    /**
     *      ������Чֵ��
     */
    void _createReportVal();

    /**
     *      ��������ͼ
     */
    void _createDualphaseGram();

    /**
     *      ��ʼ������
     */
    void _initReport();

    /**
     *      ����ҳ��״̬��Ϣ�Զ�����smv���ƿ���ѡ�еı���
     */
    void _adjustChannel();

    /**
     *      ����ָ����Ϣ����һ��ͨ�����������ÿ��ƿ��Լ�ABC����ͨ��
     */
    void _setChannel(ePhaseOrder order);

    /**
     *      Ϊָ���Ŀ��ƿ�Ѱ�Һ��ʵ�ABC����ͨ����ʾ
     *
     *  \param[in]
     *      pCtrlBlk         ָ���Ŀ��ƿ����
     *  \param[in]
     *      order           smv����smv1����smv2
     */
    void _calcChannel(CBaseDetectItem* pCtrlBlk, ePhaseOrder order);

    /**
     *      ���ط���������ͨ����
     *
     *  \param[in]
     *      pCtrlBlk        ָ���Ŀ��ƿ�
     *  \param[in]
     *      eType           ָ����ͨ������
     *  \param[in]
     *      ePhase          ָ����ͨ�����
     *  \param[in]
     *      nStart          ͨ���Ĳ�ѯ��ʼλ��
     *  \return
     *      uint32_t        ���ط���������ͨ���ţ��Ҳ����򷵻� UINT_MAX
     */
    uint32_t _findChannel(CBaseDetectItem* pCtrlBlk, enumChannelType eType, ChannelPhase ePhase, int nStart);

    /**
     *      F4�л����ܣ����������ͼ���л���ʾ
     */
    void _switchF4();

    /**
     *      F5�л����ܣ��ڱ��ģʽ�£��л���ʾͬ����ǲ�����������
     */
    void _switchF5();

    /**
     *      ���²˵�����
     */

    void _updateMenuNavi();

    /**
     *      ����ѡ��ͨ���ĵ�λ
     *
     *  \param[in]
     *      pCtrlBlk        ͨ�������Ŀ��ƿ����
     *  \param[in]
     *      nRow            ͨ�����ڱ����к�
     *  \param[in]
     *      nChannel        ͨ����
     */
    void _updateUnit(CBaseDetectItem* pCtrlBlk, int nRow, uint32_t nChannel);

    /**
     *      ��һ��ѡ��ͨ������A���������Զ�Ϊ2��3ͨ��ѡ����ӵ�BC�����͵�ͨ��
     *
     *  \param[in]
     *      eOrder          smv����smv1��smv2
     *  \param[in]
     *      pCtrlBlk        ѡ�еĿ��ƿ����
     *  \param[in]
     *      nSel            ���ƿ���ѡ�е�A��ͨ������
     */
    void _adjustABC(ePhaseOrder eOrder, CBaseDetectItem* pCtrlBlk, uint32_t nSel);

    /**
     *      ѡ���¿��ƿ�󣬵����ÿ��ƿ���ABCͨ��
     *
     *  \param[in]
     *      eOrder          smv����smv1��smv2
     *  \param[in]
     *      nSel            ���ƿ���������������ABC����ͨ��������ѡ��
     */
    void _adjustCtrlBlk(ePhaseOrder eOrder, uint32_t nSel);

    /**
     *      ˢ��ҳ������
     */
    void _updateData();

    /**
     *      ˢ������ͼ
     */
    void _updateDataGram();

    /**
     *      ˢ�±�������
     */
    void _updateDataReport();

    /**
     *      ��ձ������
     */
    void _clearReport();

    /**
     *      ���ͨ���Ƿ����仯
     */
    bool _checkChannel();


private:

    CReportVM           m_reportPhase;      ///< �����౨��
    CReportVM           m_reportValue;      ///< ��Чֵ������
    CPhasorGramCtrl     m_ctrlPhaseGram;    ///< ����ͼ

    CBaseDetectItem*    m_pCtrlBlkA;        ///< ѡ�еĿ��ƿ�A
    CBaseDetectItem*    m_pCtrlBlkB;        ///< ѡ�еĿ��ƿ�B
    CDataPhaseCheck     m_data;             ///< �������ݽӿ�

    bool                m_bGram;            ///< true ����ͼģʽ false ���ģʽ
    bool                m_bSamePhase;       ///< true ͬ�� false ���
    bool                m_bLastStatus;      ///< ��һ�ε�����״̬
    float               m_fScaleInterval;   ///< ����ͼ�̶�ֵ

    CPageDataExtraDualPhase*    m_pPageDataExtraDual;       ///< ����ҳ���������

    CRect                       m_rcAa;                         ///< ��ǲ���ʾλ��
    CRect                       m_rcBb;                         ///< ��ǲ���ʾλ��
    CRect                       m_rcCc;                         ///< ��ǲ���ʾλ��
    CRect                       m_rcF1;                         ///< smv1 Ƶ����ʾλ��
    CRect                       m_rcF2;                         ///< smv2 Ƶ����ʾλ��

    float                       m_fInPhaseAngleDiff[3];         ///< �ǲ�
    CStructPhasor               m_stPhaseVal1[3];               ///< smv1 ��������
    CStructPhasor               m_stPhaseVal2[3];               ///< smv2 ��������
    CString                     m_csDesc[6];                    ///< ����ͨ��������
    CString                     m_csUnit[6];                    ///< ͨ���ĵ�λ
    CString                     m_csFmt[6];                     ///< ͨ����ʾ���ݵĸ�ʽ���ַ���

    CString                     m_csFmtEffect;                  ///< ��Чֵ��ǵĸ�ʽ���ַ���
    CString                     m_csFmtFreq;                    ///< Ƶ�ʸ�ʽ���ַ���

    CStructPhasor               m_stPhaseData1[3];              ///< smv1 ͬ��/��������
    CStructPhasor               m_stPhaseData2[3];              ///< smv2 ͬ��/��������

    CELRecordItem*              m_arrayItem[14];
};
