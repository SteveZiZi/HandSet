/**
 * \file
 *      BaseMoDlg.h
 *
 * \brief
 *      ģ̬�Ի���Ļ���ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2014/1/2
 */


#pragma once

class CRNaviMenu;

#define DSM_BASEMODLG   CBaseMoDlg

class CBaseMoDlg : public CDialog
{
    DECLARE_DYNAMIC(CBaseMoDlg)

public:
    CBaseMoDlg(UINT nIDTemplate, CString csCaption, CWnd* pParent = NULL);   // ��׼���캯��
    virtual ~CBaseMoDlg();


protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

public:

    /**
     *      �����ڹ�������Դ
     */
    virtual void _ClearResource();

    /**
     *      �رմ��ں���
     */
    virtual void _EndDialog(int nResult);

protected:

    UINT                m_nIDMenu;              ///< �˵�ID
    CString             m_csCaption;            ///< �����ַ���ID
    CRNaviMenu*         m_pRNaviMenu;           ///< �˵���Դ�������
};
