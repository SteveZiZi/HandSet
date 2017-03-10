/**
 * \file
 *      common_pop.h
 *
 * \brief
 *      smv������Դ�����ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/7/2
 */

#ifndef COMMON_POP_H__
#define COMMON_POP_H__


//////////////////////////////////////////////////////////////////////////
// popup����
#if SU_FEI
#define DSM_BASEPOP_FTSIZE              -17
#else
#define DSM_BASEPOP_FTSIZE              -15
#endif
#define DSM_BASEPOP_CRBK                RGB(143, 169, 188)
#define DSM_BASEPOP_CRTEXT              RGB(76, 77, 94)//RGB(0xFF, 0xFF, 0xFF)
#define DSM_BASEPOP_CRSERIFA            RGB(0xff, 0xff, 0xff)//RGB(0, 0, 0)
#define DSM_BASEPOP_CRSERIFB            RGB(0xff, 0xff, 0xff)//RGB(0x3A, 0x66, 0xA0)


//////////////////////////////////////////////////////////////////////////
// home ҳ��
#define DSM_HOMECFG_RC                  CRect(0, 414, 640, 480)     ///< homeҳ������
#define DSM_HOMECFG_RCMENU              CRect(0, 1, 106, 66)        ///< homeҳ���һ���˵����� 106, 65
#define DSM_HOMECFG_RCLINE              CRect(106, 0, 107, 66)      ///< �������

#define DSM_HOMECFG_COLOR_MARGIN        RGB(0x42, 0x63, 0x94)       ///< �������ɫ
#define DSM_HOMECFG_COLOR_MENUBAR       RGB(0x13, 0x3C, 0x79)       ///< �ղ˵���ɫ

//////////////////////////////////////////////////////////////////////////
// msg ҳ��
#if SU_FEI
#define DSM_MSGDLG_RC                   CRect(0, 333, 640, 441)     ///< ҳ������
#define DSM_MSGDLG_RCBITMAP_TIP         CRect(196, 15, 276, 95)     ///< icon����
#define DSM_MSGDLG_RCTEXT_TIP           CRect(296, 15, 620, 95)     ///< ��������
#define DSM_MSGDLG_RCCOUNTDOWN_TIP      CRect(340, 75, 620, 100)    ///< ����ʱ
#else
#if SHENG_ZE
#define DSM_MSGDLG_RC                   CRect(107 + 110, 139, 430 + 110, 320)     ///< ҳ������
#define DSM_MSGDLG_RCBITMAP_TIP         CRect(120, 20, 200, 100)     ///< icon����
#define DSM_MSGDLG_RCTEXT_TIP           CRect(40, 120, 260, 200)     ///< ��������
#else
#define DSM_MSGDLG_RC                   CRect(159, 139, 481, 320)     ///< ҳ������
#define DSM_MSGDLG_RCBITMAP_TIP         CRect(53, 29, 113, 89)     ///< icon����
#define DSM_MSGDLG_RCTEXT_TIP           CRect(151, 51, 314, 71)     ///< ��������
#define DSM_MSGDLG_RCBITMAP_TIP_Z       CRect(120, 20, 200, 100)     ///< icon����
#endif
#define DSM_MSGDLG_RCCOUNTDOWN_TIP      CRect(340, 75, 620, 100)    ///< ����ʱ
#endif

//////////////////////////////////////////////////////////////////////////
// lock ҳ��
#if ZENG_YUN
#define DSM_LOCKDLG_RC                  CRect(0, 333, 640, 441)     ///< ҳ������
#define DSM_LOCK_RCBITMAP_TIP           CRect(80, 15, 160, 95)
#define DSM_LOCK_RCTEXT_TIP             CRect(180, 15, 620, 95)
#else
#define DSM_LOCKDLG_RC                  CRect(160, 139, 490, 319)     ///< ҳ������
#define DSM_LOCK_RCBITMAP_TIP           CRect(140, 20, 220, 100)
#define DSM_LOCK_RCTEXT_TIP             CRect(30, 96, 220, 160)
#endif

//////////////////////////////////////////////////////////////////////////
// password ҳ��
#define DSM_PASSWORD_RC                 CRect(0, 333, 640, 441)     ///< ҳ������
#define DSM_PASSWORD_RC_EDIT            CRect(260, 34, 380, 74)     ///< EDIT

//////////////////////////////////////////////////////////////////////////
// ����ģ̬ҳ��
#define DSM_PROGRESS_BAR_RC             CRect(0, 333, 640, 441)
#define DSM_PROGRESS_BAR_RCBITMAP       CRect(146, 15, 226, 95)     ///< icon����
#define DSM_PROGRESS_BAR_RCTEXT         CRect(246, 15, 620, 95)



#endif //COMMON_POP_H__