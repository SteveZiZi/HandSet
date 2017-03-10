#pragma once
#include "testCommon.h"

// CListCtrlTestInfo

class CListCtrlTestInfo : public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlTestInfo)

public:
	CListCtrlTestInfo();
	virtual ~CListCtrlTestInfo();

    BOOL SetCheck(int nItem, BOOL fCheck = TRUE);
    void SetItemColor(int nItem,COLORREF cr);
    void ResetAllItemsColor();

private:
	int		m_iLastSelectItem;							//记录最后一次被选中的项
    COLORREF m_crTextColor[MAX_TEST_OBJ];               //记录显示每一个测试项的颜色
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};


