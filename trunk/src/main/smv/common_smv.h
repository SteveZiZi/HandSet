/**
 * \file
 *      common_smv.h
 *
 * \brief
 *      smv公共资源定义文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/7/2
 */

#ifndef COMMON_SMV_H__
#define COMMON_SMV_H__


#define DSM_SMV_BLINK_TIPS_TIME             0x300000    ///< 显示提示通道数量发生变化信息的时长

//////////////////////////////////////////////////////////////////////////
// 相量 序量 核相 共用
#if SU_FEI
#define DSM_PHASOR_REPORT_CAPTION           80          ///< 标题列宽度
#define DSM_PHASOR_REPORT_VALUE             180         ///< 内容列宽度
#define DSM_PHASOR_REPORT_EFFECTIVE         160         ///< 有效值宽度
#define DSM_PHASOR_REPORT_ANGEL             160         ///< 相角差宽度


#define DSM_PHASOR_MARGIN                   CRect(20, 40, 0, 40)     ///< phasorgram间距
#define DSM_PHASOR_REPORT_RECT              CRect(0, DSM_P_BODY_TOP, 260, DSM_P_BODY_BOTTOM)        ///< 左侧报表区域
#define DSM_PHASOR_PHASORGRAM_RECT          CRect(260, DSM_P_BODY_TOP, 640, DSM_P_BODY_BOTTOM)      ///< 相量控件
#define DSM_ELEC_CTRL_DEFT_FT               ELT_T1_TEXTLFHEIGHT                                     ///< 相量图内描述字体大小

#else
//#define DSM_PHASOR_REPORT_CAPTION           65          ///< 标题列宽度
#define DSM_PHASOR_REPORT_CAPTION           75          ///< 标题列宽度

#define DSM_PHASOR_REPORT_VALUE             169         ///< 内容列宽度
//#define DSM_PHASOR_REPORT_EFFECTIVE         157         ///< 有效值宽度
//#define DSM_PHASOR_REPORT_ANGEL             167         ///< 相角差宽度
#define DSM_PHASOR_REPORT_EFFECTIVE         169         ///< 有效值宽度
#define DSM_PHASOR_REPORT_ANGEL             169        ///< 相角差宽度


#define DSM_PHASOR_MARGIN                   CRect(0, 0, 0, 0)     ///< phasorgram间距
//#define DSM_PHASOR_REPORT_RECT              CRect(110, DSM_P_BODY_TOP, 316, DSM_P_BODY_BOTTOM)        ///< 左侧报表区域
//#define DSM_PHASOR_PHASORGRAM_RECT          CRect(315, DSM_P_BODY_TOP, 640, DSM_P_BODY_BOTTOM)      ///< 相量控件
#define DSM_PHASOR_REPORT_RECT              CRect(110, DSM_P_BODY_TOP, 356, DSM_P_BODY_BOTTOM)        ///< 左侧报表区域
#define DSM_PHASOR_PHASORGRAM_RECT          CRect(365, DSM_P_BODY_TOP, 640, DSM_P_BODY_BOTTOM)      ///< 相量控件
#define DSM_ELEC_CTRL_DEFT_FT               ELT_T1_TEXTLFHEIGHT 

#endif

//////////////////////////////////////////////////////////////////////////
// ABCNX三相颜色
#define DSM_PHASE_COLOR_A               RGB(0xFF, 0xFF, 0x00)       ///< A相颜色       黄
#define DSM_PHASE_COLOR_B               RGB(0x32, 0xFF, 0x00)       ///< B相颜色       绿
#define DSM_PHASE_COLOR_C               RGB(0xFF, 0x00, 0x84)       ///< C相颜色       红
#define DSM_PHASE_COLOR_N               RGB(0x55, 0x78, 0xFF)       ///< N相颜色       蓝
#define DSM_PHASE_COLOR_X               RGB(0xFF, 0xA5, 0x00)       ///< X相颜色       
#define DSM_PHASE_COLOR_UN              RGB(0x00, 0xCC, 0xFF)       ///< 未知相颜色
// #define DSM_PHASE_COLOR_A               RGB(0xFF, 0x00, 0x00)       ///< A相颜色       红
// #define DSM_PHASE_COLOR_B               RGB(0xFE, 0xFE, 0x00)       ///< B相颜色       黄
// #define DSM_PHASE_COLOR_C               RGB(0x00, 0xCC, 0xFF)       ///< C相颜色       蓝绿
// #define DSM_PHASE_COLOR_N               RGB(0x26, 0xC1, 0x97)       ///< N相颜色       
// #define DSM_PHASE_COLOR_X               RGB(0xCE, 0x9D, 0x1E)       ///< X相颜色       
// #define DSM_PHASE_COLOR_UN              RGB(0x80, 0x80, 0x80)       ///< 未知相颜色    



// 录波状态区域
#define DSM_SMV_MSGMONITOR_RCRECORD_TIP_ICO     CRect(110, 20, 320, 111)
#define DSM_SMV_MSGMONITOR_RCRECORD_TIP_TEXT	CRect(60, 120, 260, 200)


/**
 *      pcap报文帧列表的信息
 */
// struct DSM_PCAP_RITEM
// {
//     int             index;
//     int             length;
//     std::wstring    time;
//     std::wstring    type;
//     std::wstring    desc;
// };

#define DSM_SMV_RECORD_ERR_SD_INJECTED      201         ///< SD拔出
#define DSM_SMV_RECORD_ERR_LINK             202         ///< 线路故障

/**
 *      加载pcap文件回调函数参数
 */
struct _PcapTaskArgs
{
    CWnd*               pWnd;           ///< 启用线程的窗口
    CString             csFile;         ///< pcap文件名
    IPcapFile*          pDecode;        ///< 解码器
};


#endif //COMMON_SMV_H__


