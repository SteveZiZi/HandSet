/**
 * \file
 *      GooseBaseDlg.h
 *
 * \brief
 *      Gooseҳ�����ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/5/8
 */

#pragma once

#include "src/main/dlg/basedlg.h"

#include "src/service/datalayer/dsmdatalayer.h"


#define DSM_GOOSEASEDLG      CGooseBaseDlg

#define DSM_GOOSE_TIMER_ID              0x82        ///< ��ʱ��ID


class CEventProc;
class CPageDataExtra;


// CGooseBaseDlg �Ի���

class CGooseBaseDlg : public DSM_BASEDLG
{
	DECLARE_DYNAMIC(CGooseBaseDlg)

public:
	CGooseBaseDlg(UINT nIDTemplate, UINT nIDCaption, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CGooseBaseDlg();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNaviMenuF1(UINT nID);
    afx_msg void OnNaviMenuF1T6(UINT nID);
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
    virtual void _beginTimer(UINT nTimeout = DSM_GOOSE_TIMER_INTERVAL);


    /**
     *      ֹͣĬ�϶�ʱ��
     */
    virtual void _stopTimer();

    /**
     *      ���±���
     */
    virtual void _updateCaption();

    /**
     *      ���ؿ��ƿ鵽F1�˵���
     */
    virtual void _loadCtrlBlk();

    /**
     *      ���ݲ˵�ID�л���ָ���Ŀ��ƿ�
     *
     *  \note
     *      ����ѡ����ƿ���Ҫ���ظ÷�����һЩ�������
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);


private:

    /**
     *      �����˵�
     */
    void _highlightMenu();

    /**
     *      �����˵�ID����ƿ飨��ͻ���ǣ�
     */
    void _setMapping(UINT nCmdID, CBaseDetectItem* pCtrlBlk);

    /**
     *      ��ȡָ�����Ƶ�������Ϣ
     */
    CString _getDesc(CNetDetectItem* pCtrlBlk);

protected:

    typedef std::map<UINT, CBaseDetectItem*>        MapCtrlBlk;

    UINT                    m_nCaptionID;           ///< ����
    UINT                    m_nTemplateID;          ///< GOOSEҳ��ģ��ID
    UINT_PTR                m_nTimerID;             ///< ˢ�����ݶ�ʱ��ID

    CEventProc*             m_pEventProc;           ///< ҳ�������ָ��

    CDsmDataLayer*          m_pDataLayer;           ///< ���ݲ�ָ��
    CBaseDetectItem*        m_pCurrentCtrlBlk;      ///< ��ǰ�������ƿ�

    UINT                    m_nHighLightCtrlBlk;    ///< ��ǰ�������ƿ�˵�ID
    UINT                    m_nCtrlBlkLastMenuID;   ///< ���һ�����ƿ�˵�ID
    MapCtrlBlk              m_mapCtrlBlk;           ///< �˵�����IDӳ����ƿ��
    CPageDataExtra*         m_pPageDataExtra;       ///< ҳ��״̬����
};
