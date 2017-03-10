/**
 * \file
 *      SMVBaseDlg.h
 *
 * \brief
 *      smvҳ�����ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/21
 */

#pragma once

#include "src/main/dlg/basedlg.h"

#include "src/service/datalayer/dsmdatalayer.h"

#define DSM_SMVBASEDLG      CSMVBaseDlg

#define DSM_SMV_TIMER_ID                0x81        ///< ��ʱ��ID
#define DSM_SMV_TIMER_INTERVAL          400         ///< SMVҵ��ʱ�����

class CEventProc;
class CPageDataExtra;
class CDsmDataLayer;
class CSmvRecvConfig;
class CBaseDetectItem;


// CSMVBaseDlg �Ի���

class CSMVBaseDlg : public DSM_BASEDLG
{
	DECLARE_DYNAMIC(CSMVBaseDlg)

public:
	CSMVBaseDlg(UINT nIDTemplate, UINT nIDCaption, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSMVBaseDlg();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNaviMenuF1(UINT nID);
    afx_msg void OnNaviMenuF3T6(UINT nID);
#if SU_FEI
    afx_msg void OnNaviMenuF6X(UINT nID);
#endif
    afx_msg void OnDestroy();
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

protected:
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);


protected:
    /**
     *      �޸Ĳ˵�HMENU
     */
    virtual void _modifyMenu();

    /**
     *      ����Ĭ�϶�ʱ��
     */
    virtual void _beginTimer(UINT nTimeout = DSM_SMV_TIMER_INTERVAL);


    /**
     *      ֹͣĬ�϶�ʱ��
     */
    virtual void _stopTimer();

    /**
     *      ���±���
     */
    virtual void _updateCaption();

    /**
     *      ���ݲ˵�ID�л���ָ���Ŀ��ƿ�
     *
     *  \note
     *      ����ѡ����ƿ���Ҫ���ظ÷�����һЩ�������
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);

    /**
     *      ���ؿ��ƿ鵽F2�˵���
     */
    virtual void _loadCtrlBlk();

    /**
     *      ����ѡ�еĿ��ƿ�˵�
     *
     *  \param[in]
     *      pSelCtrlBlk         ��Ҫ�����Ŀ��ƿ����
     */
    void _highlightCtrlBlk(CBaseDetectItem* pSelCtrlBlk);

    /**
     *      ��ʼ����ǰ���ƿ�
     */
    virtual void _initCurrentCtrlBlk();


private:

    /**
     *      ������ǰҵ��Ĳ˵�
     */
    void _highlightMenu();

    /**
     *      �����˵�ID����ƿ飨��ͻ���ǣ�
     *
     *  \param[in]
     *      nCmdID          �˵�����ID
     *  \param[in]
     *      pCtrlBlk        �˵�ID��Ӧ�Ŀ��ƿ�
     */
    void _setMapping(UINT nCmdID, CBaseDetectItem* pCtrlBlk);

    /**
     *      ����ָ�������Ŀ��ƿ�˵�
     *
     *  \param[in]
     *      nIndex          �����Ŀ��ƿ�˵�������
     */
    inline void _highlightCtrlBlk(int nIndex);

    /**
     *      ���õ�ǰҵ��ģʽ
     */
    void _setMode();



protected:

    typedef std::map<UINT, CBaseDetectItem*>        MapCtrlBlk;

    UINT                    m_nCaptionID;           ///< ����
    UINT                    m_nTemplateID;          ///< SMVҳ��ģ��ID

    UINT_PTR                m_nTimerID;             ///< ˢ�����ݶ�ʱ��ID

    CEventProc*             m_pEventProc;           ///< ҳ�������ָ��
    CDsmDataLayer*          m_pDataLayer;           ///< ���ݲ�ָ��
    CSmvRecvConfig*         m_pSMVRecvConfig;       ///< smv��������ָ��
    CBaseDetectItem*        m_pCurrentCtrlBlk;      ///< ��ǰ�������ƿ�

    int                     m_nHighLightCtrlBlk;    ///< ��ǰ�������ƿ�˵�ID��beginID�Ĳ�ֵ
    UINT                    m_nCtrlBlkLastMenuID;   ///< ���һ�����ƿ�˵�ID
    MapCtrlBlk              m_mapCtrlBlk;           ///< �˵�����IDӳ����ƿ��
    CPageDataExtra*         m_pPageDataExtra;       ///< ҳ��״̬����

    bool                    m_bChannelChange;       ///< ָ�����ƿ�ͨ���������仯
    bool                    m_bEnableTimer;         ///< �Ƿ����ö�ʱ��
};
