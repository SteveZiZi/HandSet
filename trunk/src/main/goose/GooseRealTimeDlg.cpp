/**
 * \file
 *      GooseRealTimeDlg.cpp
 *
 * \brief
 *      goose实时值页面实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/5/9
 */

// GooseRealTimeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GooseRealTimeDlg.h"

#include "src/main/common.h"
#include "src/main/pagedata/pagedata.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/main/goose/utility_goose.h"

//////////////////////////////////////////////////////////////////////////
// goose - 实时值
//
#define DSM_GOOSE_RT_HEAD_INDEX             50          ///< 索引
#define DSM_GOOSE_RT_HEAD_TYPE              105         ///< 类型
#define DSM_GOOSE_RT_HEAD_VALUE             240         ///< 值
#define DSM_GOOSE_RT_HEAD_DESC              134         ///< 描述信息

#define DSM_GOOSE_RT_RC_TOP                 CRect(0, 0, 640-115, 24)    ///< stNum状态栏区域



/**
 *      goose接收 - 实时值记录
 *
 *  \note
 *      都是文本值，4列，都只显示
 */
class CDsmGooseRTRecord : public CELRecord
{
public:
    CDsmGooseRTRecord()
    {
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
    }

    virtual ~CDsmGooseRTRecord() {}
};


/**
 *      goose实时值页面控制块管理的数据类
 */
class CCtrlBlkDataGooseRT : public ICtrlBlkData
{
public:
    CCtrlBlkDataGooseRT()
    : nChannel(0)
    , nSubChannel(-1)
    , nPage(0)
    {

    }

    virtual ~CCtrlBlkDataGooseRT(){}

public:
    int             nChannel;               ///< 通道编号
    int             nSubChannel;            ///< 子通道编号
    int             nPage;                  ///< 当前页数
};



// CGooseRealTimeDlg 对话框

IMPLEMENT_DYNAMIC(CGooseRealTimeDlg, DSM_GOOSEASEDLG)

CGooseRealTimeDlg::CGooseRealTimeDlg(CWnd* pParent /*=NULL*/)
	: DSM_GOOSEASEDLG(CGooseRealTimeDlg::IDD, DSM_STR_GOOSE_RT_CAPTION, pParent)
    , m_nRows(0)
    , m_nChann(0)
    , m_nSubChann(-1)
    , m_rcStatus(DSM_GOOSE_RT_RC_TOP)
    , m_bInitChannelData(false)
	, m_bIsShowDetail(true)
    , m_nPage(0)
{

}

CGooseRealTimeDlg::~CGooseRealTimeDlg()
{
    m_panel.DeleteObject();
}

void CGooseRealTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_GOOSEASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGooseRealTimeDlg, DSM_GOOSEASEDLG)
	ON_COMMAND(ID_MENU_GOOSE_F3, &CGooseRealTimeDlg::OnMenuF3)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CGooseRealTimeDlg 消息处理程序



BOOL CGooseRealTimeDlg::OnInitDialog()
{
    CGooseBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    _createReport();

    _restorePage();

    if(m_pCurrentCtrlBlk)
    {
        m_pDataLayer->SetCurrentMode(DSM_MODE_GOOSE_RECV_REALTIME_VAL);

        _beginTimer();
    }

    return FALSE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

/**
 *      创建实时值报表
 */
void CGooseRealTimeDlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    // 计算报表行数与报表区域
    rcReport.DeflateRect(DSM_MARGIN_IN, DSM_MARGIN_IN, DSM_MARGIN_IN, m_rcStatus.Height());
    m_nRows         = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + m_nRows * ELT_T1_ROWHEIGHT;

    // 计算底部状态区域
    m_rcStatus.DeflateRect(DSM_MARGIN_IN, 0, DSM_MARGIN_IN, 0);
    m_rcStatus.bottom   = rcReport.bottom + m_rcStatus.Height();
    m_rcStatus.top      = rcReport.bottom ;

    // 创建并设置报表样式
    m_reportGoose.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, IDC_GOOSE_RT_REPORT);
    m_reportGoose.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportGoose.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportGoose.PostInitTemplateStyle();

    // 插入列元素
    m_reportGoose.InsertColumn(0, L"", EL_CENTER, DSM_GOOSE_RT_HEAD_INDEX);                                           // 索引
    m_reportGoose.InsertColumn(1, L"    " + dsm_stringMgr::Get(DSM_STR_GOOSE_RT_TYPE), EL_CENTER, DSM_GOOSE_RT_HEAD_TYPE);    // 类型
    m_reportGoose.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_GOOSE_RT_VALUE), EL_LEFT, DSM_GOOSE_RT_HEAD_VALUE);    // 值
    m_reportGoose.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_GOOSE_RT_DESC), EL_LEFT, DSM_GOOSE_RT_HEAD_DESC);      // 描述

    // 设置选择焦点样式
    m_reportGoose.SetRowSelectionVisible(FALSE);
    m_reportGoose.SetRowFocusEnable(FALSE);

    // 创建报表区域所能容纳的行数
    for(int i = 0; i < m_nRows; i++)
    {
        m_reportGoose.InsertRecordRow(i, new CDsmGooseRTRecord);
    }

	m_menuMain.SetMenuItemCaption(2, L"隐藏结构");
	m_pNaviBar->UpdateNaviMenu();

    m_reportGoose.SetFocus();
}


void CGooseRealTimeDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if(nIDEvent == m_nTimerID)
    {
        _updateData();
    }

    CGooseBaseDlg::OnTimer(nIDEvent);
}


/**
 *      更新实时值报表数据
 */
void CGooseRealTimeDlg::_updateData()
{
    ASSERT(m_pDataLayer);
    ASSERT(m_pCurrentCtrlBlk);

    if(!m_pDataLayer->GetGooseRealTimeValue(&m_data))
    {
        // 获取不到数据时，界面显示 -- 值，说明数据一直在更新

		
		for(int i = 0; i < m_nRows; i++)
		{
			m_reportGoose.SetRowItemText(i, 0, g_csBlankValue);
			m_reportGoose.SetRowItemText(i, 1, g_csBlankValue);
			m_reportGoose.SetRowItemText(i, 2, g_csBlankValue);
			m_reportGoose.SetRowItemText(i, 3, L"");
		}	
		

        // 清空底部stNum等状态
        _updateStatus(true);

        return;
    }

    // 检查运行时通道数据是否变化
    if(_isChannelChange())
    {
        _resetPage();       // 重设页面回到初始状态
    }

    CString csBlank;
    int nCounts             = m_data.GetChannelCounts();
    int nChann              = m_nChann;         // 第一行记录的通道序号
    int nSubChann           = m_nSubChann;      // 第一行记录通道序号的子通道序号
    GOOSE_DATA* pGsData     = NULL;
    CGooseChannel* pChann   = NULL;

	

    for(int i = 0; i < m_nRows; nChann++, nSubChann = -1)
    {
        pChann = m_data.GetChanneData(nChann);

        if(!pChann)
        {
            for(int k = i; k < m_nRows; k++)
            {
                if(nChann >= nCounts)
                {
                    csBlank = L"";
                }
                else
                {
                    csBlank = g_csBlankValue;
                }

                m_reportGoose.SetRowItemText(k, 0, csBlank);
				m_reportGoose.SetRowItemText(k, 1, csBlank);
				m_reportGoose.SetRowItemText(k, 2, csBlank);
				m_reportGoose.SetRowItemText(k, 3, L"");
            }

            break;
        }

        pGsData = pChann->GetChannelData();

#ifdef ZY_PRJ

        // 如果子通道序号为-1，则插入父通道（结构类型）
        if(-1 == nSubChann)
        {
            _insertRecord(pChann, nChann, nSubChann, i++);
        }

        // 若为父通道（结构类型），则枚举子通道
        if(pGsData && GSEDT_STRUCT == pChann->GetChannelData()->type && i < m_nRows)
        {
            CGooseChannel* pSubChan = NULL;
            uint32_t nCounts        = pChann->GetSubChannelCount();         // 子通道个数
            nSubChann               = (nSubChann == -1) ? 0 : nSubChann;    // 计算枚举子通道的位置

            ASSERT(((GOOSE_STRUCT*)pGsData)->member.size() == nCounts);

            // 枚举插入子通道
            for(uint32_t j = nSubChann; j < nCounts && i < m_nRows; j++)
            {
                pSubChan = pChann->GetSubChannelData(j);
                ASSERT(pSubChan);

                _insertRecord(pSubChan, nChann, j, i++);
            }
        }
#else
		if (m_bIsShowDetail)
		{
		
			// 如果子通道序号为-1，则插入父通道（结构类型）
			if(-1 == nSubChann)
			{
				_insertRecord(pChann, nChann, nSubChann, i++);
			}

			// 若为父通道（结构类型），则枚举子通道
			if(pGsData && GSEDT_STRUCT == pChann->GetChannelData()->type && i < m_nRows)
			{
				CGooseChannel* pSubChan = NULL;
				uint32_t nCounts        = pChann->GetSubChannelCount();         // 子通道个数
				nSubChann               = (nSubChann == -1) ? 0 : nSubChann;    // 计算枚举子通道的位置

				ASSERT(((GOOSE_STRUCT*)pGsData)->member.size() == nCounts);

				// 枚举插入子通道
				for(uint32_t j = nSubChann; j < nCounts && i < m_nRows; j++)
				{
					pSubChan = pChann->GetSubChannelData(j);
					ASSERT(pSubChan);

					_insertRecord(pSubChan, nChann, j, i++);
				}
			}
		}
		else
		{
			// 如果子通道序号为-1，则插入父通道（结构类型）
			if(-1 == nSubChann)
			{
				_insertRecord(pChann, nChann, nSubChann, i++);
			}

			// 若为父通道（结构类型），则枚举子通道
			if(pGsData && GSEDT_STRUCT == pChann->GetChannelData()->type && i < m_nRows)
			{
				CGooseChannel* pSubChan = NULL;
				uint32_t nCounts        = pChann->GetSubChannelCount();         // 子通道个数
				nSubChann               = (nSubChann == -1) ? 0 : nSubChann;    // 计算枚举子通道的位置

				ASSERT(((GOOSE_STRUCT*)pGsData)->member.size() == nCounts);

				// 枚举插入子通道
				for(uint32_t  j = nSubChann; j < nCounts && i < m_nRows; j++)
				{
					pSubChan = pChann->GetSubChannelData(j);
					ASSERT(pSubChan);

					_insertRecord(pSubChan, nChann, j, i);
				}
				i++;
			}
		}
#endif
    }

    // 更新stNum等状态
    _updateStatus();
}


/**
 *      刷新报表底部的stNum,sqNum,TTL状态
 */
void CGooseRealTimeDlg::_updateStatus( bool bNull /* = false */ )
{
    CDC* pDC = GetDC();

    CString csStatus;

    if(bNull)
    {
        csStatus = L"stNum: --  sqNum: --d  TTL: --";
    }
    else
    {
        csStatus.Format(L"stNum: %4d  sqNum: %4d  TTL: %4d", m_data.GetGooseStNum(), m_data.GetGooseSqNum(), m_data.GetGooseTTL());
    }

    if(!m_panel.GetSafeHandle())
    {
        m_panel.CreateCompatibleBitmap(pDC, m_rcStatus.Width(), m_rcStatus.Height());
    }

    CDC dcMem;
    dcMem.CreateCompatibleDC(pDC);

    int nPre = dcMem.SaveDC();
    dcMem.SelectObject(m_panel);

    dcMem.SetBkMode(TRANSPARENT);
    dcMem.SelectObject(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    dcMem.SetTextColor(ELT_T1_TEXTCOLOR);
    dcMem.FillSolidRect(0, 0, m_rcStatus.Width(), m_rcStatus.Height(), ELT_T1_BKCOLOR);
    dcMem.DrawText(csStatus, CRect(0, 0, m_rcStatus.Width(), m_rcStatus.Height()), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

    pDC->BitBlt(m_rcStatus.left, m_rcStatus.top, m_rcStatus.Width(), m_rcStatus.Height(), &dcMem, 0, 0, SRCCOPY);
    dcMem.RestoreDC(nPre);
    dcMem.DeleteDC();
    pDC->DeleteDC();
}



BOOL CGooseRealTimeDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if(WM_KEYDOWN == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case VK_UP:
            _channelUp();
            break;

        case  VK_DOWN:
            _channelDown();
            break;

        default:
            break;
        }
    }

    return CGooseBaseDlg::PreTranslateMessage(pMsg);
}


/**
 *      计算向下翻页在第一行显示的通道序号和子通道序号
 *
 *  \return
 *      bool            发生翻页返回true，否则返回false
 */
bool CGooseRealTimeDlg::_calcDown()
{
    uint32_t nCount = m_data.GetChannelCounts();

    if(nCount == 0)
    {
        return false;
    }

    int nRow                    = 0;                // 累计显示的行数
    int nChan                   = m_nChann;         // 当前计算的通道索引
    int nSubChan                = m_nSubChann;      // 当前计算通道的子通道索引
    GOOSE_DATA* pGooseData      = NULL;
    GOOSE_STRUCT* pGooseSt      = NULL;
    CGooseChannel* pChannel     = m_data.GetChanneData(nChan);
    CCtrlBlkDataGooseRT* pData = dynamic_cast<CCtrlBlkDataGooseRT*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    ASSERT(pData);
    ASSERT(pChannel);

#ifdef ZY_PRJ
    while(pChannel)
    {
        pGooseData = pChannel->GetChannelData();
        ASSERT(pGooseData);

        // 当前通道是结构体类型（即父通道类型）
        if(GSEDT_STRUCT == pGooseData->type)
        {
            pGooseSt        = (GOOSE_STRUCT*)pGooseData;
            size_t nSize    = pGooseSt->member.size();

            // 算出该通道已显示的行数
            nRow = nRow + nSize - nSubChan;

            // 判断最后一页的情况
            if(nRow == m_nRows && !m_data.GetChanneData(nChan + 1))
            {
                return false;
            }

            // 如果该通道在本页面显示不完，则下一页继续显示该通道
            if(nRow >= m_nRows)
            {
                m_nPage++;

                m_nChann            = nRow > m_nRows ? nChan : nChan + 1;
                m_nSubChann         = nRow > m_nRows ? nSize - (nRow - m_nRows) : -1;
                pData->nChannel     = m_nChann;
                pData->nSubChannel  = m_nSubChann;
                pData->nPage        = m_nPage;

                return true;
            }
        }
        else
        {
            nRow++;
            if(nRow > m_nRows)
            {
                m_nPage++;

                m_nChann            = nChan;
                m_nSubChann         = -1;
                pData->nChannel     = m_nChann;
                pData->nSubChannel  = m_nSubChann;
                pData->nPage        = m_nPage;

                return true;
            }
        }

        // 该通道在本页能被完全显示，则枚举下一通道，并恢复子通道索引为-1
        nSubChan = -1;
        pChannel = m_data.GetChanneData(++nChan);
    }
#else
	while(pChannel)
	{
		pGooseData = pChannel->GetChannelData();
		ASSERT(pGooseData);

		// 当前通道是结构体类型（即父通道类型）
		if(GSEDT_STRUCT == pGooseData->type)
		{
			pGooseSt        = (GOOSE_STRUCT*)pGooseData;
			size_t nSize    = pGooseSt->member.size();

			if (!m_bIsShowDetail)
			{
				
				// 算出该通道已显示的行数
				nRow += 2;
			}
			else
			{
				// 算出该通道已显示的行数
				nRow = nRow + nSize - nSubChan;
			}
			
			
			

			// 判断最后一页的情况
			if(nRow == m_nRows && !m_data.GetChanneData(nChan + 1))
			{
				return false;
			}

			// 如果该通道在本页面显示不完，则下一页继续显示该通道
			if(nRow >= m_nRows)
			{
				m_nPage++;

				m_nChann            = nRow > m_nRows ? nChan : nChan + 1;
				if (!m_bIsShowDetail)
				{
					m_nSubChann         = -1;
				}
				else
					m_nSubChann         = nRow > m_nRows ? nSize - (nRow - m_nRows) : -1;
				pData->nChannel     = m_nChann;
				pData->nSubChannel  = m_nSubChann;
				pData->nPage        = m_nPage;

				return true;
			}
		}
		else
		{
			nRow++;
			if(nRow > m_nRows)
			{
				m_nPage++;

				m_nChann            = nChan;
				m_nSubChann         = -1;
				pData->nChannel     = m_nChann;
				pData->nSubChannel  = m_nSubChann;
				pData->nPage        = m_nPage;

				return true;
			}
		}

		// 该通道在本页能被完全显示，则枚举下一通道，并恢复子通道索引为-1
		nSubChan = -1;
		pChannel = m_data.GetChanneData(++nChan);
	}
#endif
    return false;
}


/**
 *      计算向上翻页在第一行显示的通道序号和子通道序号
 *
 *  \return
 *      bool            发生翻页返回true，否则返回false
 */
bool CGooseRealTimeDlg::_calcUp()
{
    uint32_t nCount = m_data.GetChannelCounts();

    if(nCount == 0)
    {
        return false;
    }

    int nRow                    = 0;                // 累计显示的行数
    int nChan                   = m_nChann;         // 当前显示通道索引
    GOOSE_DATA* pGooseData      = NULL;
    GOOSE_STRUCT* pGooseSt      = NULL;
    CGooseChannel* pChannel     = m_data.GetChanneData(nChan);
    CCtrlBlkDataGooseRT* pData = dynamic_cast<CCtrlBlkDataGooseRT*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    ASSERT(pData);
    ASSERT(pChannel);

#ifdef ZY_PRJ
    while(pChannel)
    {
        pGooseData = pChannel->GetChannelData();
        ASSERT(pGooseData);

        if(GSEDT_STRUCT == pGooseData->type)
        {
            pGooseSt        = (GOOSE_STRUCT*)pGooseData;
            size_t nSize    = pGooseSt->member.size();

            if(nChan == m_nChann)
            {
                // m_nSubChann为未显示的子通道
                // 算出m_nSubChann之前该通道未显示的行数
                nRow = nRow + m_nSubChann + 1;
            }
            else
            {
                // 该通道全部未显示nsize是子通道+1是该通道本身
                nRow = nRow + nSize + 1;
            }

            // 已显示的通道数大于每页最大显示通道数
            // 需要计算某个（子）通道索引，让其显示为第一行
            if(nRow >= m_nRows)
            {
                m_nPage--;

                m_nChann            = nChan;                    // 某通道索引
                m_nSubChann         = nRow - m_nRows - 1;       // 其子通道索引，算出首行索引
                pData->nChannel     = m_nChann;
                pData->nSubChannel  = m_nSubChann;
                pData->nPage        = m_nPage;

                return true;
            }
        }
        else
        {
            if(nChan != m_nChann)
            {
                nRow++;
            }

            // 已显示满通道
            if(nRow == m_nRows)
            {
                m_nPage--;

                m_nChann            = nChan;
                m_nSubChann         = -1;
                pData->nChannel     = m_nChann;
                pData->nSubChannel  = m_nSubChann;
                pData->nPage        = m_nPage;

                return true;
            }
        }

        // 枚举上一个通道
        pChannel = m_data.GetChanneData(--nChan);
    }
#else

	while(pChannel)
	{
		pGooseData = pChannel->GetChannelData();
		ASSERT(pGooseData);

		if(GSEDT_STRUCT == pGooseData->type)
		{
			pGooseSt        = (GOOSE_STRUCT*)pGooseData;
			size_t nSize    = pGooseSt->member.size();

			if (!m_bIsShowDetail)
			{
				/*if(nChan<11)
				{
					m_nChann  =0;
					m_nSubChann =-1;
					return true;
				}*/
			}
			if(nChan == m_nChann)
			{
				// m_nSubChann为未显示的子通道
				// 算出m_nSubChann之前该通道未显示的行数
				nRow = nRow + m_nSubChann + 1;
			}
			else
			{
				if (!m_bIsShowDetail)
				{
					// 算出该通道已显示的行数
					nRow += 2;
				}
				else{
					nRow = nRow + nSize + 1;
				}
				
				
				
			}

			// 已显示的通道数大于每页最大显示通道数
			// 需要计算某个（子）通道索引，让其显示为第一行
			if(nRow >= m_nRows || nChan == 0)
			{
				m_nPage--;
				if (!m_bIsShowDetail)
				{
					m_nChann            = nChan;                    // 某通道索引
				}
				else
					m_nChann            = nChan;
				m_nSubChann         = nRow - m_nRows - 1;       // 其子通道索引，算出首行索引
				pData->nChannel     = m_nChann;
				if (nChan ==0)
				{
					m_nSubChann = -1;
				}
				pData->nSubChannel  = m_nSubChann;
				pData->nPage        = m_nPage;

				return true;
			}
		}
		else
		{
			if(nChan != m_nChann)
			{
				nRow++;
			}

			// 已显示满通道
			if(nRow == m_nRows)
			{
				m_nPage--;

				m_nChann            = nChan;
				m_nSubChann         = -1;
				pData->nChannel     = m_nChann;
				pData->nSubChannel  = m_nSubChann;
				pData->nPage        = m_nPage;

				return true;
			}
		}

		// 枚举上一个通道
		pChannel = m_data.GetChanneData(--nChan);
	}

#endif
    return false;
}

/**
 *      通道翻页 - 上一页
 */
void CGooseRealTimeDlg::_channelUp()
{
    if(_calcUp())
    {
        _updateData();
    }
}

/**
 *      通道翻页 - 下一页
 */
void CGooseRealTimeDlg::_channelDown()
{
    if(_calcDown())
    {
        _updateData();
    }
}


/**
 *      保存页面
 */
void CGooseRealTimeDlg::_SavePage()
{
}

/**
 *      还原页面数据
 */
bool CGooseRealTimeDlg::_restorePage()
{
    ASSERT(m_pPageDataExtra);

    CCtrlBlkDataGooseRT* pData = dynamic_cast<CCtrlBlkDataGooseRT*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    if(!pData)
    {
        pData = new CCtrlBlkDataGooseRT;
        m_pPageDataExtra->Set(m_pCurrentCtrlBlk->nID, pData);
    }

    m_nChann    = pData->nChannel;
    m_nSubChann = pData->nSubChannel;
    m_nPage     = pData->nPage;

    return true;
}


/**
 *      插入一条通道记录
 *
 *  \param[in]
 *      pChannel        通道对象
 *  \param[in]
 *      nChannel        goose数据通道号
 *  \param[in]
 *      nSubChannel     goose数据子通道号
 *  \param[in]
 *      nRow            报表行号
 */
void CGooseRealTimeDlg::_insertRecord( CGooseChannel* pChannel, int nChannel, int nSubChannel, int nRow )
{
    if(!pChannel || !pChannel->GetChannelData())
    {
        ASSERT(FALSE);
        return;
    }

    CString csIndex(L""), csType(L""), csValue(L""), csDesc(pChannel->GetChannelDesc().c_str());

    // 判断父子通道
    if(pChannel->GetParentChannel())
    {
        csIndex.Format(L"%d.%d", nChannel + 1, nSubChannel + 1);
    }
    else
    {
        csIndex.Format(L"%d", nChannel + 1);
    }

    DSM_GetGooseDataInfo(pChannel, csType, csValue);
#ifdef ZY_PRJ
    m_reportGoose.SetRowItemText(nRow, 0, csIndex);
    m_reportGoose.SetRowItemText(nRow, 1, csType);
    m_reportGoose.SetRowItemText(nRow, 2, csValue);
    m_reportGoose.SetRowItemText(nRow, 3, csDesc);
#else
	
	if (!m_bIsShowDetail )
	{
		if (csType.Compare(L"BOOLEAN")==0 || nSubChannel == -1)
		{
			m_reportGoose.SetRowItemText(nRow, 0, csIndex);
			m_reportGoose.SetRowItemText(nRow, 1, csType);
			m_reportGoose.SetRowItemText(nRow, 2, csValue);
			m_reportGoose.SetRowItemText(nRow, 3, csDesc);
		}		
	}
	else
	{
		m_reportGoose.SetRowItemText(nRow, 0, csIndex);
		m_reportGoose.SetRowItemText(nRow, 1, csType);
		m_reportGoose.SetRowItemText(nRow, 2, csValue);
		m_reportGoose.SetRowItemText(nRow, 3, csDesc);
	}
	
#endif
}

/**
 *      根据菜单ID切换到指定的控制块
 *
 *  \param[in]
 *      nCmdID          菜单命令ID
 */
bool CGooseRealTimeDlg::_switchCtrlBlk( UINT nCmdID )
{
    CBaseDetectItem* pPreCtrlBlk = m_pCurrentCtrlBlk;

    if(__super::_switchCtrlBlk(nCmdID))
    {
        CCtrlBlkDataGooseRT* pData = dynamic_cast<CCtrlBlkDataGooseRT*>(m_pPageDataExtra->Get(pPreCtrlBlk->nID));

        if(!pData)
        {
            pData = new CCtrlBlkDataGooseRT;
            m_pPageDataExtra->Set(pPreCtrlBlk->nID, pData);
        }

        pData->nChannel     = m_nChann;
        pData->nSubChannel  = m_nSubChann;
        pData->nPage        = m_nPage;

        _restorePage();

        return true;
    }

    return false;
}


/**
 *      检查goose通道数量与类型是否变化
 *
 *  \param[in]
 *      bool            通道数量或类型变化返回true，没变化返回false
 */
bool CGooseRealTimeDlg::_isChannelChange()
{
    // 已初始化过m_vecChannels
    if(m_bInitChannelData)
    {
        // 通道数量发生变化，将返回到页面的初始状态
        if(m_vecChannels.size() != m_data.GetChannelCounts())
        {
            return true;
        }

        CGooseChannel* pChannelNew  = NULL;
        CGooseChannel* pChannelOld  = NULL;

        uint32_t nCounts            = m_data.GetChannelCounts();

        // 检查每个通道类型是否变化，变化则返回页面初始状态
        for(uint32_t i = 0; i < nCounts; i++)
        {
            pChannelNew = m_data.GetChanneData(i);
            pChannelOld = m_vecChannels.at(i);
            ASSERT(pChannelOld && pChannelNew);

            // 数据类型不同
            if(pChannelNew->GetChannelData()->type != pChannelOld->GetChannelData()->type)
            {
                return true;
            }

            // 结构类型通道还需要判断子通道的类型
            if(pChannelNew->GetChannelData()->type == GSEDT_STRUCT)
            {
                // 子通道数量不相等
                if(pChannelNew->GetSubChannelCount() != pChannelOld->GetSubChannelCount())
                {
                    return true;
                }

                uint32_t nSubCounts = pChannelNew->GetSubChannelCount();

                // 检查子通道的类型
                for(uint32_t k = 0; k < nSubCounts; k++)
                {
                    if(pChannelNew->GetSubChannelData(k)->GetChannelData()->type != pChannelOld->GetSubChannelData(k)->GetChannelData()->type)
                    {
                        return true;
                    }
                } // for 子通道
            } // if 结构类型通道
        } // for 父通道
    }
    else
    {
        CGooseChannel* pChannel = NULL;
        m_bInitChannelData      = true;         // 初始化完成
        uint32_t nCounts        = m_data.GetChannelCounts();

        // 保存当前通道的信息
        for(uint32_t i = 0; i < nCounts; i++)
        {
            pChannel = m_data.GetChanneData(i);
            ASSERT(pChannel);

            CGooseChannel* pNewChannel = new CGooseChannel();
            pNewChannel->SetChannelID(pChannel->GetChannelID());
            pNewChannel->SetChannelDesc(pChannel->GetChannelDesc());
            pNewChannel->SetChannelType(pChannel->GetChannelType());
            pNewChannel->SetParentChannel(NULL);
            pNewChannel->SetChannelData(pChannel->GetChannelData());

            m_vecChannels.push_back(pNewChannel);
        }
    }

    return false;
}

void CGooseRealTimeDlg::_resetPage()
{
    // 释放旧通道数据
    uint32_t nCounts = m_vecChannels.size();
    for(uint32_t i = 0; i < nCounts; i++)
    {
        delete m_vecChannels.at(i);
    }

    m_vecChannels.clear();
    CGooseChannel* pChannel = NULL;
    nCounts                 = m_data.GetChannelCounts();

    // 保存当前通道的信息
    for(uint32_t i = 0; i < nCounts; i++)
    {
        pChannel = m_data.GetChanneData(i);
        ASSERT(pChannel);

        CGooseChannel* pNewChannel = new CGooseChannel();
        pNewChannel->SetChannelID(pChannel->GetChannelID());
        pNewChannel->SetChannelDesc(pChannel->GetChannelDesc());
        pNewChannel->SetChannelType(pChannel->GetChannelType());
        pNewChannel->SetParentChannel(NULL);
        pNewChannel->SetChannelData(pChannel->GetChannelData());

        m_vecChannels.push_back(pNewChannel);
    }


    ASSERT(m_pPageDataExtra);
    ASSERT(m_pCurrentCtrlBlk);

    CCtrlBlkDataGooseRT* pData = dynamic_cast<CCtrlBlkDataGooseRT*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    int nOldPage        = pData->nPage;
    m_nChann            = 0;
    m_nSubChann         = -1;
    m_nPage             = 0;

    pData->nChannel     = m_nChann;
    pData->nSubChannel  = m_nSubChann;

    // 定位到合适的页面
    while(nOldPage != m_nPage)
    {
        if(!_calcDown())
        {
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void CGooseRealTimeDlg::OnMenuF3()
{
	m_bIsShowDetail = !m_bIsShowDetail;
	if (m_bIsShowDetail)
	{
		m_menuMain.SetMenuItemCaption(2,L"隐藏结构");
	}
	else
	{
		m_menuMain.SetMenuItemCaption(2,L"显示结构");//dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_FILTER));
	}
	m_pNaviBar->UpdateNaviMenu();
	_updateData();
}

/**
 *      修改菜单HMENU
 */
void CGooseRealTimeDlg::_modifyMenu()
{
    __super::_modifyMenu();
	

 //   // 修改F3字符串为『隐藏结构』
	//if (m_bIsShowDetail)
	//{
	//	m_menuMain.SetMenuItemCaption(2,L"隐藏结构");
	//}
	//else
	//{
	//	m_menuMain.SetMenuItemCaption(2,L"显示结构");//dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_FILTER));
	//}
}