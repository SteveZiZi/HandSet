#pragma once

#include "../ListCtrlEL/ListCtrlEL.h"
#include "../ListBoxEL/TreeBoxEL.h"
#include "ListBoxExt.h"

//
//=============================================================================
// һ�ű��ģ��, �����ڴ󲿷ֵ�ʹ��report�������
//=============================================================================
//
#define ELT_T1_TEXTLFHEIGHT         -16                                 // �Ƽ������С
#define ELT_T1_HDRHEIGHT            38                                 // ��ͷ�߶�
#if SHENG_ZE
#define ELT_T1_ROWHEIGHT            41                                 // �����и�
#else
#define ELT_T1_ROWHEIGHT            37                                 // �����и�
#endif
#define ELT_T1_HDRGRIDCOLOR         RGB(49,  61,  81)               // ��ͷgrid����ɫ
#define ELT_T1_HDRBKCOLOR           RGB(60,  74,  98)                   // ��ͷ������ɫ
#define ELT_T1_HDRTEXTCOLOR         RGB(197, 201, 208)                  // ��ͷ������ɫ

#define ELT_T1_BKCOLOR              RGB(58,  82,  100)                   // ���屳����ɫ
#define ELT_T1_TEXTCOLOR            RGB(255, 255, 255)                  // �����ı���ɫ
#define ELT_T1_HLCOLOR              RGB(239, 145, 16)                   // ������ɫ
#define ELT_T1_HLTEXTCOLOR          RGB(0,   0,   0)                    // �ı�������ɫ
#define ELT_T1_LLCOLOR              RGB(157, 160, 168)                  // ������ɫ

#define ELT_T1_SUBHLCOLOR           ELT_T1_HLCOLOR/*RGB(247, 219, 91)*/                   // ����(��������)������ɫ
#define ELT_T1_EDITBKCOLOR          LBT_FB2_BKCOLOR                     // Inplace-edit�ı�����ɫ
#define ELT_T1_EDITTEXTCOLOR        ELT_T1_TEXTCOLOR                    // Inplace-edit��������ɫ
#define ELT_T1_EDITBORDERCOLOR      LBT_FB_BORDERCOLOR                  // inplace-edit�ı߿���ɫ
#define ELT_T1_LISTBKCOLOR          LBT_FB_BKCOLOR                     // Inplace-list�ı�����ɫ
#define ELT_T1_LISTTEXTCOLOR        LBT_FB_TEXTCOLOR                    // Inplace-list��������ɫ
#define ELT_T1_LISTHLCOLOR          LBT_FB_HLCOLOR                      // Inplace-list�ĸ�����ɫ
#define ELT_T1_LISTHLTEXTCOLOR      LBT_FB_HLTEXTCOLOR                  // Inplace-list�����ָ�����ɫ



//
//=============================================================================
// CListCtrlT_VLine   ����ͷ��Report�ؼ���չģ�� (��ֱGrid��), ͨ���������ݲɼ�չʾ����
//=============================================================================
//

class CListCtrlT_VLine: public CListCtrlEL
{
public:
    CListCtrlT_VLine ();
    virtual ~CListCtrlT_VLine ();

    void  PostInitTemplateStyle ();
};



//
//=============================================================================
// CListCtrlT_HLine   ������ͷ��Report�ؼ���չģ�� (ˮƽGrid��), ͨ������ϵͳ�������ý���
//=============================================================================
//

class CListCtrlT_HLine: public CListCtrlEL
{
public:
    CListCtrlT_HLine ();
    virtual ~CListCtrlT_HLine ();

    void  PostInitTemplateStyle ();
};


//
//=============================================================================
// CListCtrlT_VLineMidH    ��ֱ�ָ�����ˮƽ���ߵĻ�ϵ�����ListCtrl, ������������, �����ҳ��
//=============================================================================
//

class CListCtrlT_VLineMidH: public CListCtrlT_VLine
{
public:
    CListCtrlT_VLineMidH ();
    virtual ~CListCtrlT_VLineMidH ();

    void  PostInitTemplateStyle ();

protected:
    virtual void  FillGridline (CDC* pDC);
};


//
//=============================================================================
// CTreeBoxT_VLine    ��ֱ�ָ��ߵ�����TreeBox
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// CTreeBoxT_VLine
//-----------------------------------------------------------------------------
//
class CTreeBoxT_VLine: public CTreeBoxEL
{
public:
    //
    //=============================================================================
    // �ؼ���������ʽ
    //=============================================================================
    //
    CTreeBoxT_VLine ();
    virtual ~CTreeBoxT_VLine ();

    void  PostInitTemplateStyle ();

protected:
    //
    //=============================================================================
    // ��Ϣ����
    //=============================================================================
    //

protected:
    //
    //=============================================================================
    // ����
    //=============================================================================
    //

protected:
    //
    //=============================================================================
    // �ڲ�״̬���Ա���
    //=============================================================================
    //

};


//
//=============================================================================
// CListCtrlT_AboutCtrl  ����ҳ��, �����������
//=============================================================================
//

#define ELT_ABT_TEXTLFHEIGHT        -15                                 // �Ƽ������С
#define ELT_ABT_COLTITLE_MARGIN     20                                  // �����е���߾�
#define ELT_ABT_COLTITLE_WIDTH      105                                 // �����еĿ��
#define ELT_ABT_COLTITLE_COLOR      RGB(0xFF, 0xFF, 0xFF)               // �����е��ı���ɫ

#define ELT_ABT_COLSUBTITLE_WIDTH   80                                  // �ӱ����еĿ��


class CListCtrlT_AboutCtrl: public CListCtrlT_HLine
{
public:
    // �ж���
    enum AboutColumnIndex
    {
        COL_TITLE       = 0,  // ��������
        COL_SUBTITLE    = 1,  // ��������
        COL_DESC        = 2   // ������Ϣ��
    };

    // �ж���
    enum AboutRowIndex
    {
        ROW_TOP_BLANK       = 0,    // �հ���, ����ռλ
        ROW_PRODUCT_NAME    = 1,    // ��Ʒ����
        ROW_HW_VER          = 2,    // Ӳ���汾
        ROW_SYS_VER         = 3,    // ����ϵͳ�汾
        ROW_SW_VER          = 4,    // ����汾
        ROW_COMPANY_NAME    = 5,    // ��˾����
        ROW_COMPANY_ADDR    = 6,    // ��˾��ַ
        ROW_SUPPORT         = 7,    // ����֧��
        ROW_BOT_BLANK       = 8     // �ײ��հ���
    };

public:
    CListCtrlT_AboutCtrl ();
    virtual ~CListCtrlT_AboutCtrl ();

    void  PostInitTemplateStyle ();


public:
    //
    // ������������
    //
    void  SetAboutItemText (AboutRowIndex rowIndex, AboutColumnIndex colIndex, const CString& str);


protected:
    virtual void  FillGridline (CDC* pDC);
};

inline void  CListCtrlT_AboutCtrl::SetAboutItemText (AboutRowIndex rowIndex, AboutColumnIndex colIndex, const CString& str)
{
    SetRowItemText(rowIndex, colIndex, str);
}