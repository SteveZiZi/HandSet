/**
 * \file
 *      common_smv.h
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

#ifndef COMMON_SMV_H__
#define COMMON_SMV_H__


#define DSM_SMV_BLINK_TIPS_TIME             0x300000    ///< ��ʾ��ʾͨ�����������仯��Ϣ��ʱ��

//////////////////////////////////////////////////////////////////////////
// ���� ���� ���� ����
#if SU_FEI
#define DSM_PHASOR_REPORT_CAPTION           80          ///< �����п��
#define DSM_PHASOR_REPORT_VALUE             180         ///< �����п��
#define DSM_PHASOR_REPORT_EFFECTIVE         160         ///< ��Чֵ���
#define DSM_PHASOR_REPORT_ANGEL             160         ///< ��ǲ���


#define DSM_PHASOR_MARGIN                   CRect(20, 40, 0, 40)     ///< phasorgram���
#define DSM_PHASOR_REPORT_RECT              CRect(0, DSM_P_BODY_TOP, 260, DSM_P_BODY_BOTTOM)        ///< ��౨������
#define DSM_PHASOR_PHASORGRAM_RECT          CRect(260, DSM_P_BODY_TOP, 640, DSM_P_BODY_BOTTOM)      ///< �����ؼ�
#define DSM_ELEC_CTRL_DEFT_FT               ELT_T1_TEXTLFHEIGHT                                     ///< ����ͼ�����������С

#else
//#define DSM_PHASOR_REPORT_CAPTION           65          ///< �����п��
#define DSM_PHASOR_REPORT_CAPTION           75          ///< �����п��

#define DSM_PHASOR_REPORT_VALUE             169         ///< �����п��
//#define DSM_PHASOR_REPORT_EFFECTIVE         157         ///< ��Чֵ���
//#define DSM_PHASOR_REPORT_ANGEL             167         ///< ��ǲ���
#define DSM_PHASOR_REPORT_EFFECTIVE         169         ///< ��Чֵ���
#define DSM_PHASOR_REPORT_ANGEL             169        ///< ��ǲ���


#define DSM_PHASOR_MARGIN                   CRect(0, 0, 0, 0)     ///< phasorgram���
//#define DSM_PHASOR_REPORT_RECT              CRect(110, DSM_P_BODY_TOP, 316, DSM_P_BODY_BOTTOM)        ///< ��౨������
//#define DSM_PHASOR_PHASORGRAM_RECT          CRect(315, DSM_P_BODY_TOP, 640, DSM_P_BODY_BOTTOM)      ///< �����ؼ�
#define DSM_PHASOR_REPORT_RECT              CRect(110, DSM_P_BODY_TOP, 356, DSM_P_BODY_BOTTOM)        ///< ��౨������
#define DSM_PHASOR_PHASORGRAM_RECT          CRect(365, DSM_P_BODY_TOP, 640, DSM_P_BODY_BOTTOM)      ///< �����ؼ�
#define DSM_ELEC_CTRL_DEFT_FT               ELT_T1_TEXTLFHEIGHT 

#endif

//////////////////////////////////////////////////////////////////////////
// ABCNX������ɫ
#define DSM_PHASE_COLOR_A               RGB(0xFF, 0xFF, 0x00)       ///< A����ɫ       ��
#define DSM_PHASE_COLOR_B               RGB(0x32, 0xFF, 0x00)       ///< B����ɫ       ��
#define DSM_PHASE_COLOR_C               RGB(0xFF, 0x00, 0x84)       ///< C����ɫ       ��
#define DSM_PHASE_COLOR_N               RGB(0x55, 0x78, 0xFF)       ///< N����ɫ       ��
#define DSM_PHASE_COLOR_X               RGB(0xFF, 0xA5, 0x00)       ///< X����ɫ       
#define DSM_PHASE_COLOR_UN              RGB(0x00, 0xCC, 0xFF)       ///< δ֪����ɫ
// #define DSM_PHASE_COLOR_A               RGB(0xFF, 0x00, 0x00)       ///< A����ɫ       ��
// #define DSM_PHASE_COLOR_B               RGB(0xFE, 0xFE, 0x00)       ///< B����ɫ       ��
// #define DSM_PHASE_COLOR_C               RGB(0x00, 0xCC, 0xFF)       ///< C����ɫ       ����
// #define DSM_PHASE_COLOR_N               RGB(0x26, 0xC1, 0x97)       ///< N����ɫ       
// #define DSM_PHASE_COLOR_X               RGB(0xCE, 0x9D, 0x1E)       ///< X����ɫ       
// #define DSM_PHASE_COLOR_UN              RGB(0x80, 0x80, 0x80)       ///< δ֪����ɫ    



// ¼��״̬����
#define DSM_SMV_MSGMONITOR_RCRECORD_TIP_ICO     CRect(110, 20, 320, 111)
#define DSM_SMV_MSGMONITOR_RCRECORD_TIP_TEXT	CRect(60, 120, 260, 200)


/**
 *      pcap����֡�б����Ϣ
 */
// struct DSM_PCAP_RITEM
// {
//     int             index;
//     int             length;
//     std::wstring    time;
//     std::wstring    type;
//     std::wstring    desc;
// };

#define DSM_SMV_RECORD_ERR_SD_INJECTED      201         ///< SD�γ�
#define DSM_SMV_RECORD_ERR_LINK             202         ///< ��·����

/**
 *      ����pcap�ļ��ص���������
 */
struct _PcapTaskArgs
{
    CWnd*               pWnd;           ///< �����̵߳Ĵ���
    CString             csFile;         ///< pcap�ļ���
    IPcapFile*          pDecode;        ///< ������
};


#endif //COMMON_SMV_H__


