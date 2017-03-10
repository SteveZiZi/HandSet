/**
 * \file
 *      MenuEx.h
 *
 * \brief
 *      自定义左侧导航菜单头文件
 *
 *  \note
 *      主要是实现一些特定的操作逻辑
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/16
 */


#pragma once
#include "src/utils/reportcontrol/listboxel/listboxel.h"


/**
 *      左侧导航菜单控件
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
     *      统一设置ITEM高度
     *
     *  \note
     *      若SetHeight之后又添加新的元素，则需要重新调用一次
     */
    void SetHeight(UINT itemHeight);
};
