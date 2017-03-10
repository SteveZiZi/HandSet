/**
 * \file
 *      MenuEx.h
 *
 * \brief
 *      �Զ�����ർ���˵�ͷ�ļ�
 *
 *  \note
 *      ��Ҫ��ʵ��һЩ�ض��Ĳ����߼�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/16
 */


#pragma once
#include "src/utils/reportcontrol/listboxel/listboxel.h"


/**
 *      ��ർ���˵��ؼ�
 */
class CMenuEx : public CListBoxT_3dLine
{
public:
    CMenuEx(void);
    ~CMenuEx(void);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public:
    /**
     *      ͳһ����ITEM�߶�
     *
     *  \note
     *      ��SetHeight֮��������µ�Ԫ�أ�����Ҫ���µ���һ��
     */
    void SetHeight(UINT itemHeight);
};
