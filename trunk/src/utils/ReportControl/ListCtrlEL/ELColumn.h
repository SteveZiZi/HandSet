#pragma once


#include "ELDefines.h"


//
//-----------------------------------------------------------------------------
// CELColumn
//
//  ��������һ����(field/col)������, ����, �п��, �ж��뷽ʽ, ��ѡ���
//  ������е���Щ�еĸ��Ӳ������е�����, ���Ըø��ӵ�����Ϊ׼
//
//  ע��: ��Ϊ�����Ǽ򻯵�CListCtrl, �����е�ĳЩ��������ԭ����CHeaderCtrl�ṩ��
//  ���ﲢû����Ӧ������, ����: �е�����, �еĶ��뷽ʽ, �еĿ�ȵ�, 
//  ��Щ���Կ�����CHeaderCtrl::GetItem���
//-----------------------------------------------------------------------------
//
class CELColumn
{
public:
    //
    //=============================================================================
    // �ؼ���������ʽ
    //=============================================================================
    //
    CELColumn ();
    virtual ~CELColumn ();


    CELRecordItemEditOptions*  GetEditOptions ();


private:
    //
    //=============================================================================
    // �ڲ�״̬���Ա���
    //=============================================================================
    //
    BOOL                    m_bEditable;        // Indicates whether an edit box should be added to the items in the column.
    ELAlignment             m_nAlignment;       // ���е�����Ԫ�صĶ��뷽ʽ
    ELAlignment             m_nHeaderAlignment; // Alignment mode for column text: DT_LEFT || DT_RIGHT || DT_CENTER
    COLORREF                m_crBkColor;        // �������ݵı���ɫ, Ĭ����EL_COLOR_DEFAULT
    int                     m_nMarginLeft;      // ����Item������ʱ, ��ߵ���������
    int                     m_nMarginRight;     // ����Item������ʱ, �ұߵ���������
    BOOL                    m_bBoldText;
    CELRecordItemEditOptions*  m_pEditOptions;  // Edit options of the column


    friend class CHeaderCtrlEL;
    friend class CListCtrlEL;
    friend class CELRecordItem;
};

//
//-----------------------------------------------------------------------------
// ��ñ��й����Ŀ�ѡ�����, �ö����Ӱ�챾�е�����item
//  ���itemû��ָ����ѡ��, ��ʹ�ñ��еĿ�ѡ��
//-----------------------------------------------------------------------------
//
inline CELRecordItemEditOptions*  CELColumn::GetEditOptions ()
{
    return m_pEditOptions;
}

