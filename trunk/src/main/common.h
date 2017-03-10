/**
 * \file
 *      common.h
 *
 * \brief
 *      ������Դ�����ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/3/21
 */

#ifndef COMMON_H__
#define COMMON_H__

#ifdef _DEBUG
#define DSM_DEBUG NKDbgPrintfW
#else
#define DSM_DEBUG(...)
#endif

#define DSM_WORK_HERTZ_50               50
#define DSM_BATTERY_LOW_WARNNING        10
#define DSM_CHANNEL_MAX                 30

#define IDBACKTOMAIN                    IDHELP + 100


#define DSM_COLOR_BK                    ELT_T1_BKCOLOR


#define DSM_KEY_ESC                     VK_ESCAPE       ///< esc
#define DSM_KEY_HOME                    VK_F12          ///< Home
#define DSM_KEY_SELECT                  VK_F11          ///< menu��
#define DSM_KEY_POWER                   VK_F10          ///< ��Դ��
#define DSM_KEY_SNAPSHOT                VK_SNAPSHOT        ///< ����

#define DSM_CFG_SCL_FILE_FOLDER         L"\\"      ///< SCLĿ¼
#define DSM_CFG_SCL_FILE_EXTENSION      L"*.scd|*.icd|*.cid|*.ssd|*.scx|"          ///< SCL��չ��

#define DSM_PCAP_SMV_FILE_FOLDER        L"\\Record\\SV\\"        ///< 
#define DSM_PCAP_GOOSE_FILE_FOLDER      L"\\Record\\GOOSE\\"      ///< 
#if SHENG_ZE
#define DSM_SNAPSHOT_FOLDER             L"\\snapshot\\"         ///< ��ͼĿ¼
#endif
#define DSM_PCAP_FILE_EXTENSION         L"pcap"                 ///< pcap�ļ���չ��


#define DSM_DEFAULT_FONT                L"΢���ź�"

const CString g_csBlankValue = L"--";

#define WM_DSM_TIME_TICK                PRIVATE_WND_USERID + 471        ///< ʱ��tick
#define WM_DSM_SYNC_NOW                 PRIVATE_WND_USERID + 470        ///< ����ͬ��
#define DSM_SYNC_LOCAL_TO_FPGA          0           ///< ����ʱ��ͬ����FPGAʱ��
#define DSM_SYNC_FPGA_TO_LOCAL          1           ///< FPGAʱ��ͬ��������ʱ��



#define DSM_FLOAT_VOLT_1_INT            5           ///< ��ѹһ��ֵ��������
#define DSM_FLOAT_VOLT_1_DECI           3           ///< ��ѹһ��ֵС������
#define DSM_FLOAT_VOLT_2_INT            5           ///< ��ѹ����ֵ��������
#define DSM_FLOAT_VOLT_2_DECI           3           ///< ��ѹ����ֵС������

#define DSM_FLOAT_AMPER_1_INT           5           ///< ����һ��ֵ��������
#define DSM_FLOAT_AMPER_1_DECI          3           ///< ����һ��ֵС������
#define DSM_FLOAT_AMPER_2_INT           5           ///< ��������ֵ��������
#define DSM_FLOAT_AMPER_2_DECI          3           ///< ��������ֵС������

#define FONT_DEFAULT_STYLE                      0, 0, 0, \
                                                FW_NORMAL, FALSE, FALSE, 0, \
                                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, \
                                                CLEARTYPE_QUALITY, FF_ROMAN, \
                                                DSM_DEFAULT_FONT


#define DSM_GOOSE_SCAN_TIMER_INTERVAL   400         ///< GOOSE̽��goose���ƿ��ʱ����
#define DSM_GOOSE_TIMER_INTERVAL        400         ///< GOOSEҵ��ˢ�����ݵ�ʱ����

//////////////////////////////////////////////////////////////////////////
// �����ؼ�
#define CReportH CListCtrlT_HLine
#define CReportV CListCtrlT_VLine
#define CReportVM CListCtrlT_VLineMidH
#define CReportAbout CListCtrlT_AboutCtrl

#define CTreeV CTreeBoxT_VLine


//////////////////////////////////////////////////////////////////////////
// �Ի���ҳ������ֲ�
#if CHUN_HUA
#define DSM_RCTOP_STATUBAR              CRect(0, 0, 640, 33)        ///< topbar����           33
#define DSM_RCTOP_SERIF_LINE            CRect(0, 32, 640, 33)       ///< topbar�·���������   1
#define DSM_RCBODY                      CRect(0, 33, 640, 414)      ///< ҵ��ҳ������       381
#define DSM_RCBOTTOM_STATUBAR           CRect(0, 414, 640, 442)     ///< bottombar����         26
#define DSM_RCNAVIGATIONBAR             CRect(0, 442, 640, 480)     ///< ����������         38

#else

#define DSM_RCTOP_STATUBAR              CRect(0, 0, 640, 37)        ///< topbar����           33
#define DSM_RCBODY                      CRect(110, 37, 640, 444)      ///< ҵ��ҳ������       381
#define DSM_RCBOTTOM_STATUBAR           CRect(0, 444, 640, 480)     ///< bottombar����         26
#define DSM_RCNAVIGATIONBAR             CRect(0, 37, 110, 444)     ///< ����������         38
#endif


//////////////////////////////////////////////////////////////////////////
// ������������ͬ��

// body
#define DSM_P_BODY_LEFT                 110
#define DSM_P_BODY_TOP                  37
#define DSM_P_BODY_RIGHT                640
#define DSM_P_BODY_BOTTOM               444

// bottom �Ϸ���
#define DSM_P_LINE_BOT_UP_LEFT          0
#define DSM_P_LINE_BOT_UP_TOP           444
#define DSM_P_LINE_BOT_UP_RIGHT         640
#define DSM_P_LINE_BOT_UP_BOTTOM        445

// bottombar
#define DSM_P_BOTTOM_LEFT               0
#define DSM_P_BOTTOM_TOP                445
#define DSM_P_BOTTOM_RIGHT              640
#define DSM_P_BOTTOM_BOTTOM             480

//////////////////////////////////////////////////////////////////////////

#define DSM_LINE_COLOR                  RGB(115, 113, 123)
#define DSM_LINE_MARGIN_COLOR           RGB(0x2D, 0x46, 0x6E)

#define DSM_COLOR_TEXT                  RGB(0xFF, 0xFF, 0xFF)

#if !SU_FEI
#define DSM_RCTOP_CAPTION               CRect(10, 0, 440, 36)       ///< topbar��������
#define DSM_RCTOP_BATTERY               CRect(604, 0, 636, 36)      ///< topbar�������
#define DSM_RCTOP_RIGHTICON             CRect(440, 0, 604, 36)      ///< topbar���������������
#define DSM_RCTOP_ICONRECT              CRect(440, 0, 640, 36)      ///< topbarͼ��ȫ������
#else

#define DSM_RCTOP_CAPTION               CRect(10, 0, 440, 32)       ///< topbar��������
#define DSM_RCTOP_BATTERY               CRect(604, 0, 636, 32)      ///< topbar�������
#define DSM_RCTOP_RIGHTICON             CRect(440, 0, 604, 32)      ///< topbar���������������
#define DSM_RCTOP_ICONRECT              CRect(440, 0, 640, 32)      ///< topbarͼ��ȫ������
#define DSM_RCBOTTOM_TIME               CRect(500, 414, 630, 442)   ///< bottombarʱ������
#define DSM_RCBOTTOM_TIPS               CRect(10, 414, 490, 442)    ///< bottombar��ʾtips����

#endif

// bottombar �������
#define DSM_RCBOTTOM_RC                 CRect(0, 0, 640, 28)
#if SU_FEI
#define DSM_RCBOTTOM_TIME_RC            CRect(470, 0, 630, 28)      ///< ʱ��
#define DSM_RCBOTTOM_TIPS_RC            CRect(10, 0, 460, 28)       ///< tips
#else
#define DSM_RCBOTTOM_TIME_RC            CRect(470, 0, 630, 33)      ///< ʱ��
#define DSM_RCBOTTOM_TIPS_RC            CRect(41, 0, 460, 33)       ///< tips
#define DSM_RCBOTTOM_ICO_RC            CRect(10, 8, 40, 33)       ///< tips
#endif
#define DSM_RCBOTTOM_SERIF_UP_RC        CRect(0, 0, 640, 1)         ///< �Ϸ�����
#define DSM_RCBOTTOM_SERIF_UP1_RC       CRect(0, 1, 640, 2)         ///< �������
#define DSM_RCBOTTOM_SERIF_DOWN_RC      CRect(0, 27, 640, 28)       ///< �·�����


//#define DSM_RCNAVI_BMP                  CRect(2, 444, 108, 480)     ///< ��������һ��ͼ����

#if SU_FEI
#define DSM_FTSIZE_TOPBAR               -17                         ///< topbar�����С
#define DSM_FTSIZE_BOTTOMBAR            -13                         ///< bottombar�����С
#define DSM_FTSIZE_NAVIGATION           -16                         ///< �����������С
#else
#define DSM_FTSIZE_TOPBAR               -18                         ///< topbar�����С
#define DSM_FTSIZE_BOTTOMBAR            -16                         ///< bottombar�����С
#define DSM_FTSIZE_NAVIGATION           -18                         ///< �����������С
#endif


// ��������

#define DSM_BASICCFG_RCLEFT             CRect(0, DSM_P_BODY_TOP, 110, DSM_P_BODY_BOTTOM)      ///< ��߲˵�����
#define DSM_BASICCFG_RCRIGHT            CRect(110, DSM_P_BODY_TOP, 640, DSM_P_BODY_BOTTOM)    ///< �ұ�����
#define DSM_BASICCFG_RCLINE             CRect(110, DSM_P_BODY_TOP, 110, DSM_P_BODY_BOTTOM)    ///< ��



//////////////////////////////////////////////////////////////////////////
// ҵ��ҳ���ڼ��
#define DSM_MARGIN_IN                   0              ///< ҵ��ҳ����ڼ��
#define DSM_MARGIN_IN_RECT              CRect(DSM_MARGIN_IN, DSM_MARGIN_IN, DSM_MARGIN_IN, DSM_MARGIN_IN)



//////////////////////////////////////////////////////////////////////////
// ����Ĭ����ʽ

#define DSM_REPORT_FONT_SIZE                ELT_T1_TEXTLFHEIGHT         ///< ����������С
#define DSM_REPORT_ROW_HEIGHT               ELT_T1_ROWHEIGHT            ///< ������и�
#define DSM_REPORT_HEAD_HEIGHT              ELT_T1_HDRHEIGHT            ///< �����и�
#define DSM_REPORT_FONT_HEAD_SIZE           ELT_T1_TEXTLFHEIGHT         ///< ���������С

#define DSM_REPORT_WIDTH_MARGIN             0           ///< �߾�
#define DSM_REPORT_WIDTH_CAPTION            230         ///< ������
#define DSM_REPORT_WIDTH_CONTENT            299         ///< ���ݿ��

#define DSM_REPORT_COLOR_TEXT               ELT_T1_TEXTCOLOR
#define DSM_REPORT_COLOR_BK                 ELT_T1_BKCOLOR
#define DSM_REPORT_COLOR_BK_HIGH            ELT_T1_HLCOLOR
#define DSM_REPORT_COLOR_BK_ITEM_HIGH       ELT_T1_SUBHLCOLOR
#define DSM_REPORT_COLOR_BK_LOW             ELT_T1_LLCOLOR
#define DSM_REPORT_COLOR_LINE               ELT_T1_HDRGRIDCOLOR

#define DSM_REPORT_COLOR_HEAD_BK            ELT_T1_HDRBKCOLOR
#define DSM_REPORT_COLOR_HEAD_TEXT          ELT_T1_HDRTEXTCOLOR
#define DSM_REPORT_COLOR_HEAD_LINE          ELT_T1_HDRGRIDCOLOR


#define DSM_REPORT_COLOR_CAPTION_BK         ELT_T1_HDRBKCOLOR


#define DSM_WINSTYLE_REPORT_DEFAULT     (WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL)   ///< Ĭ�ϱ�����ʽ
#define DSM_WINSTYLE_TREE_DEFAULT       (WS_CHILD | WS_VISIBLE | WS_TABSTOP)                    ///< Ĭ�����ؼ���ʽ

#endif //COMMON_H__

