/**
 * \file
 *      common.h
 *
 * \brief
 *      公共资源定义文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
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
#define DSM_KEY_SELECT                  VK_F11          ///< menu键
#define DSM_KEY_POWER                   VK_F10          ///< 电源键
#define DSM_KEY_SNAPSHOT                VK_SNAPSHOT        ///< 截屏

#define DSM_CFG_SCL_FILE_FOLDER         L"\\"      ///< SCL目录
#define DSM_CFG_SCL_FILE_EXTENSION      L"*.scd|*.icd|*.cid|*.ssd|*.scx|"          ///< SCL扩展名

#define DSM_PCAP_SMV_FILE_FOLDER        L"\\Record\\SV\\"        ///< 
#define DSM_PCAP_GOOSE_FILE_FOLDER      L"\\Record\\GOOSE\\"      ///< 
#if SHENG_ZE
#define DSM_SNAPSHOT_FOLDER             L"\\snapshot\\"         ///< 截图目录
#endif
#define DSM_PCAP_FILE_EXTENSION         L"pcap"                 ///< pcap文件扩展名


#define DSM_DEFAULT_FONT                L"微软雅黑"

const CString g_csBlankValue = L"--";

#define WM_DSM_TIME_TICK                PRIVATE_WND_USERID + 471        ///< 时间tick
#define WM_DSM_SYNC_NOW                 PRIVATE_WND_USERID + 470        ///< 立即同步
#define DSM_SYNC_LOCAL_TO_FPGA          0           ///< 本地时间同步到FPGA时间
#define DSM_SYNC_FPGA_TO_LOCAL          1           ///< FPGA时间同步到本地时间



#define DSM_FLOAT_VOLT_1_INT            5           ///< 电压一次值整数部分
#define DSM_FLOAT_VOLT_1_DECI           3           ///< 电压一次值小数部分
#define DSM_FLOAT_VOLT_2_INT            5           ///< 电压二次值整数部分
#define DSM_FLOAT_VOLT_2_DECI           3           ///< 电压二次值小数部分

#define DSM_FLOAT_AMPER_1_INT           5           ///< 电流一次值整数部分
#define DSM_FLOAT_AMPER_1_DECI          3           ///< 电流一次值小数部分
#define DSM_FLOAT_AMPER_2_INT           5           ///< 电流二次值整数部分
#define DSM_FLOAT_AMPER_2_DECI          3           ///< 电流二次值小数部分

#define FONT_DEFAULT_STYLE                      0, 0, 0, \
                                                FW_NORMAL, FALSE, FALSE, 0, \
                                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, \
                                                CLEARTYPE_QUALITY, FF_ROMAN, \
                                                DSM_DEFAULT_FONT


#define DSM_GOOSE_SCAN_TIMER_INTERVAL   400         ///< GOOSE探测goose控制块的时间间隔
#define DSM_GOOSE_TIMER_INTERVAL        400         ///< GOOSE业务刷新数据的时间间隔

//////////////////////////////////////////////////////////////////////////
// 美化控件
#define CReportH CListCtrlT_HLine
#define CReportV CListCtrlT_VLine
#define CReportVM CListCtrlT_VLineMidH
#define CReportAbout CListCtrlT_AboutCtrl

#define CTreeV CTreeBoxT_VLine


//////////////////////////////////////////////////////////////////////////
// 对话框页面区域分布
#if CHUN_HUA
#define DSM_RCTOP_STATUBAR              CRect(0, 0, 640, 33)        ///< topbar区域           33
#define DSM_RCTOP_SERIF_LINE            CRect(0, 32, 640, 33)       ///< topbar下方线条区域   1
#define DSM_RCBODY                      CRect(0, 33, 640, 414)      ///< 业务页面区域       381
#define DSM_RCBOTTOM_STATUBAR           CRect(0, 414, 640, 442)     ///< bottombar区域         26
#define DSM_RCNAVIGATIONBAR             CRect(0, 442, 640, 480)     ///< 导航栏区域         38

#else

#define DSM_RCTOP_STATUBAR              CRect(0, 0, 640, 37)        ///< topbar区域           33
#define DSM_RCBODY                      CRect(110, 37, 640, 444)      ///< 业务页面区域       381
#define DSM_RCBOTTOM_STATUBAR           CRect(0, 444, 640, 480)     ///< bottombar区域         26
#define DSM_RCNAVIGATIONBAR             CRect(0, 37, 110, 444)     ///< 导航栏区域         38
#endif


//////////////////////////////////////////////////////////////////////////
// 该数据与上面同步

// body
#define DSM_P_BODY_LEFT                 110
#define DSM_P_BODY_TOP                  37
#define DSM_P_BODY_RIGHT                640
#define DSM_P_BODY_BOTTOM               444

// bottom 上方线
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
#define DSM_RCTOP_CAPTION               CRect(10, 0, 440, 36)       ///< topbar标题区域
#define DSM_RCTOP_BATTERY               CRect(604, 0, 636, 36)      ///< topbar电池区域
#define DSM_RCTOP_RIGHTICON             CRect(440, 0, 604, 36)      ///< topbar除电池外最右区域
#define DSM_RCTOP_ICONRECT              CRect(440, 0, 640, 36)      ///< topbar图标全部区域
#else

#define DSM_RCTOP_CAPTION               CRect(10, 0, 440, 32)       ///< topbar标题区域
#define DSM_RCTOP_BATTERY               CRect(604, 0, 636, 32)      ///< topbar电池区域
#define DSM_RCTOP_RIGHTICON             CRect(440, 0, 604, 32)      ///< topbar除电池外最右区域
#define DSM_RCTOP_ICONRECT              CRect(440, 0, 640, 32)      ///< topbar图标全部区域
#define DSM_RCBOTTOM_TIME               CRect(500, 414, 630, 442)   ///< bottombar时间区域
#define DSM_RCBOTTOM_TIPS               CRect(10, 414, 490, 442)    ///< bottombar提示tips区域

#endif

// bottombar 区域分配
#define DSM_RCBOTTOM_RC                 CRect(0, 0, 640, 28)
#if SU_FEI
#define DSM_RCBOTTOM_TIME_RC            CRect(470, 0, 630, 28)      ///< 时间
#define DSM_RCBOTTOM_TIPS_RC            CRect(10, 0, 460, 28)       ///< tips
#else
#define DSM_RCBOTTOM_TIME_RC            CRect(470, 0, 630, 33)      ///< 时间
#define DSM_RCBOTTOM_TIPS_RC            CRect(41, 0, 460, 33)       ///< tips
#define DSM_RCBOTTOM_ICO_RC            CRect(10, 8, 40, 33)       ///< tips
#endif
#define DSM_RCBOTTOM_SERIF_UP_RC        CRect(0, 0, 640, 1)         ///< 上方线条
#define DSM_RCBOTTOM_SERIF_UP1_RC       CRect(0, 1, 640, 2)         ///< 间隔线条
#define DSM_RCBOTTOM_SERIF_DOWN_RC      CRect(0, 27, 640, 28)       ///< 下方线条


//#define DSM_RCNAVI_BMP                  CRect(2, 444, 108, 480)     ///< 导航栏第一张图区域

#if SU_FEI
#define DSM_FTSIZE_TOPBAR               -17                         ///< topbar字体大小
#define DSM_FTSIZE_BOTTOMBAR            -13                         ///< bottombar字体大小
#define DSM_FTSIZE_NAVIGATION           -16                         ///< 导航栏字体大小
#else
#define DSM_FTSIZE_TOPBAR               -18                         ///< topbar字体大小
#define DSM_FTSIZE_BOTTOMBAR            -16                         ///< bottombar字体大小
#define DSM_FTSIZE_NAVIGATION           -18                         ///< 导航栏字体大小
#endif


// 基本设置

#define DSM_BASICCFG_RCLEFT             CRect(0, DSM_P_BODY_TOP, 110, DSM_P_BODY_BOTTOM)      ///< 左边菜单区域
#define DSM_BASICCFG_RCRIGHT            CRect(110, DSM_P_BODY_TOP, 640, DSM_P_BODY_BOTTOM)    ///< 右边区域
#define DSM_BASICCFG_RCLINE             CRect(110, DSM_P_BODY_TOP, 110, DSM_P_BODY_BOTTOM)    ///< 线



//////////////////////////////////////////////////////////////////////////
// 业务页面内间距
#define DSM_MARGIN_IN                   0              ///< 业务页面的内间距
#define DSM_MARGIN_IN_RECT              CRect(DSM_MARGIN_IN, DSM_MARGIN_IN, DSM_MARGIN_IN, DSM_MARGIN_IN)



//////////////////////////////////////////////////////////////////////////
// 报表默认样式

#define DSM_REPORT_FONT_SIZE                ELT_T1_TEXTLFHEIGHT         ///< 表体的字体大小
#define DSM_REPORT_ROW_HEIGHT               ELT_T1_ROWHEIGHT            ///< 表体的行高
#define DSM_REPORT_HEAD_HEIGHT              ELT_T1_HDRHEIGHT            ///< 标题行高
#define DSM_REPORT_FONT_HEAD_SIZE           ELT_T1_TEXTLFHEIGHT         ///< 标题字体大小

#define DSM_REPORT_WIDTH_MARGIN             0           ///< 边距
#define DSM_REPORT_WIDTH_CAPTION            230         ///< 标题宽度
#define DSM_REPORT_WIDTH_CONTENT            299         ///< 内容宽度

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


#define DSM_WINSTYLE_REPORT_DEFAULT     (WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL)   ///< 默认报表样式
#define DSM_WINSTYLE_TREE_DEFAULT       (WS_CHILD | WS_VISIBLE | WS_TABSTOP)                    ///< 默认树控件样式

#endif //COMMON_H__

