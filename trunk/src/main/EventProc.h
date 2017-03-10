/**
 * \file
 *      EventProc.h
 *
 * \brief
 *      ҳ����Ϣ������
 *          ����ҳ�����ת
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/3/21
 *
 *  \note
 *      ���ฺ��������Ϣ���л����ڣ����͸���״̬����
 */


#pragma once

#include "common.h"
#include "src/main/dlg/BaseDlg.h"
#include "src/main/pagedata/ipagedata.h"

#define DSM_HOME_RETVAL_MAIN        0xF001          ///< ����������
#define DSM_HOME_RETVAL_SNAP        0xF002          ///< ��ͼ
#define DSM_HOME_RETVAL_NONE        0xF003          ///< �ղ���


#define DSM_KEYEVT_LOCK                 0       ///< ����
#define DSM_KEYEVT_POWEROFF             1       ///< �ػ�

class CBaseDlg;

class CEventProc
{
public:

//////////////////////////////////////////////////////////////////////////
// ��������Ϣ

    /**
     *      ����ESC��Ϣ
     */
    void OnKeyEsc();

    /**
     *      ����ػ�����
     *
     *  \param[in]
     *      dwStatu         DSM_KEYEVT_LOCK:        ����
     *                      DSM_KEYEVT_POWEROFF:    �ػ�
     */
    void OnKeyPower(DWORD dwStatu);

    /**
     *      ����home��
     */
    UINT OnKeyHome(bool bModal = false);

    /**
     *      ��ͼ����
     */
    void SnapShot();



//////////////////////////////////////////////////////////////////////////
// 
public:

    /**
     *      ���ص�ǰҳ����
     */
    HWND GetCurrentPageHwnd();

    /**
     *      ��ʾָ��ҳ��
     *
     *  \param[in]
     *      nPageID         ҳ��ID��ʹ�öԻ���ģ��ID
     *  \note
     *      ҳ������˳�Ӧ����һ����Ϣ
     *      �Ա�������ҳ����Ի�֪
     */
    void ShowPage(UINT nPageID);

    /**
     *      ��ʾMAIN����
     */
    void ShowMain();

    /**
     *      ��ʾ�򵼻���
     */
    void ShowWizard();

    /**
     *      �ָ���������
     */
    void ResetFactory();

    /**
     *      �˳�����ǰ�رմ����ͷ���Դ
     */
    void Exit();

    /**
     *      �رճ���
     */
    void Close();

    /**
     *      ��ʾ�����
     */
    bool ShowPassword();

    /**
     *      ���������ļ�
     */
    void SaveConfig();


    /**
     *      ��ȡ��̬����ָ��
     */
    static CEventProc* GetInstance()
    {
        static CEventProc s_Instance;
        return &s_Instance;
    }


//////////////////////////////////////////////////////////////////////////
// ����ҳ������

    /**
     *      ��ȡ��ǰҳ������
     */
    CPageData* GetCurrentPageData();

    /**
     *      ��ȡǰһҳ������
     */
    CPageData* GetPreviousPageData();


//////////////////////////////////////////////////////////////////////////


private:
    CEventProc(void);
    ~CEventProc(void);


private:

    /**
     *      ǰ����ҳ��
     */
    void pageForward(UINT nPageID);

    /**
     *      ���ش�ҳ��
     */
    void pageBack();

    /**
     *      ��ҳ��
     */
    void pageShow(UINT nPageID, DsmPageAction eAction = PageEnter);

    /**
     *      ����ҳ��ջ
     */
    void updatePageStack(UINT nPageID, DsmPageAction eAction);

    /**
     *      �ص���ҳ��
     */
    void processPageMain();

    /**
     *      ������һ��ҳ��
     */
    void processPageBack();

    /**
     *      ��ѯָ��ҳ�������
     */
    CPageData* lookupPageDataByID(UINT nPageID);

    /**
     *      ����ҳ��
     */
    CBaseDlg* pageCreate(UINT nPageID);

    /**
     *      �ж�ҳ���Ƿ���smv����gooseҵ��ҳ��
     */
    bool isBusinessPage(UINT nPageID);

    /**
     *      ���浱ǰ���ڽ���
     */
    void saveFocusHwnd(HWND hWnd);

    /**
     *      �ָ���ǰ���ڽ���
     */
    void restoreFocusHwnd();

    /**
     *      ���ҳ��ջ����
     */
    void clearPageStack();

    /**
     *      ����ҵ��ҳ�����ݵ�ӳ���
     */
    void clearMapBussData();

    /**
     *      ������Դ
     */
    void clearSource();


private:
    typedef CList<HWND, HWND>                   CStackHWND;
    typedef CList<CPageData*>                   CStackData;

    typedef CMap<UINT, UINT, CPageData*, CPageData*>    CMapBussData;

    CBaseDlg*           m_pCurPage;             ///< ��ǰ��ҳ����
    CStackHWND          m_stackAncestor;        ///< ��ǰ���㴰�ڵ�����
    CStackData          m_stackData;            ///< ҳ���������
    CMapBussData        m_mapBussData;          ///< ����ҵ��ҳ�����ݵ�ӳ���
};
