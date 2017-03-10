/**
 * \file
 *      GooseRealTimeDlg.cpp
 *
 * \brief
 *      gooseʵʱֵҳ��ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/5/9
 */

// GooseRealTimeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GooseRealTimeDlg.h"

#include "src/main/common.h"
#include "src/main/pagedata/pagedata.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/main/goose/utility_goose.h"

//////////////////////////////////////////////////////////////////////////
// goose - ʵʱֵ
//
#define DSM_GOOSE_RT_HEAD_INDEX             50          ///< ����
#define DSM_GOOSE_RT_HEAD_TYPE              105         ///< ����
#define DSM_GOOSE_RT_HEAD_VALUE             240         ///< ֵ
#define DSM_GOOSE_RT_HEAD_DESC              134         ///< ������Ϣ

#define DSM_GOOSE_RT_RC_TOP                 CRect(0, 0, 640-115, 24)    ///< stNum״̬������



/**
 *      goose���� - ʵʱֵ��¼
 *
 *  \note
 *      �����ı�ֵ��4�У���ֻ��ʾ
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
 *      gooseʵʱֵҳ����ƿ�����������
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
    int             nChannel;               ///< ͨ�����
    int             nSubChannel;            ///< ��ͨ�����
    int             nPage;                  ///< ��ǰҳ��
};



// CGooseRealTimeDlg �Ի���

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


// CGooseRealTimeDlg ��Ϣ�������



BOOL CGooseRealTimeDlg::OnInitDialog()
{
    CGooseBaseDlg::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    _createReport();

    _restorePage();

    if(m_pCurrentCtrlBlk)
    {
        m_pDataLayer->SetCurrentMode(DSM_MODE_GOOSE_RECV_REALTIME_VAL);

        _beginTimer();
    }

    return FALSE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

/**
 *      ����ʵʱֵ����
 */
void CGooseRealTimeDlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    // ���㱨�������뱨������
    rcReport.DeflateRect(DSM_MARGIN_IN, DSM_MARGIN_IN, DSM_MARGIN_IN, m_rcStatus.Height());
    m_nRows         = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + m_nRows * ELT_T1_ROWHEIGHT;

    // ����ײ�״̬����
    m_rcStatus.DeflateRect(DSM_MARGIN_IN, 0, DSM_MARGIN_IN, 0);
    m_rcStatus.bottom   = rcReport.bottom + m_rcStatus.Height();
    m_rcStatus.top      = rcReport.bottom ;

    // ���������ñ�����ʽ
    m_reportGoose.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, IDC_GOOSE_RT_REPORT);
    m_reportGoose.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportGoose.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportGoose.PostInitTemplateStyle();

    // ������Ԫ��
    m_reportGoose.InsertColumn(0, L"", EL_CENTER, DSM_GOOSE_RT_HEAD_INDEX);                                           // ����
    m_reportGoose.InsertColumn(1, L"    " + dsm_stringMgr::Get(DSM_STR_GOOSE_RT_TYPE), EL_CENTER, DSM_GOOSE_RT_HEAD_TYPE);    // ����
    m_reportGoose.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_GOOSE_RT_VALUE), EL_LEFT, DSM_GOOSE_RT_HEAD_VALUE);    // ֵ
    m_reportGoose.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_GOOSE_RT_DESC), EL_LEFT, DSM_GOOSE_RT_HEAD_DESC);      // ����

    // ����ѡ�񽹵���ʽ
    m_reportGoose.SetRowSelectionVisible(FALSE);
    m_reportGoose.SetRowFocusEnable(FALSE);

    // �������������������ɵ�����
    for(int i = 0; i < m_nRows; i++)
    {
        m_reportGoose.InsertRecordRow(i, new CDsmGooseRTRecord);
    }

	m_menuMain.SetMenuItemCaption(2, L"���ؽṹ");
	m_pNaviBar->UpdateNaviMenu();

    m_reportGoose.SetFocus();
}


void CGooseRealTimeDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    if(nIDEvent == m_nTimerID)
    {
        _updateData();
    }

    CGooseBaseDlg::OnTimer(nIDEvent);
}


/**
 *      ����ʵʱֵ��������
 */
void CGooseRealTimeDlg::_updateData()
{
    ASSERT(m_pDataLayer);
    ASSERT(m_pCurrentCtrlBlk);

    if(!m_pDataLayer->GetGooseRealTimeValue(&m_data))
    {
        // ��ȡ��������ʱ��������ʾ -- ֵ��˵������һֱ�ڸ���

		
		for(int i = 0; i < m_nRows; i++)
		{
			m_reportGoose.SetRowItemText(i, 0, g_csBlankValue);
			m_reportGoose.SetRowItemText(i, 1, g_csBlankValue);
			m_reportGoose.SetRowItemText(i, 2, g_csBlankValue);
			m_reportGoose.SetRowItemText(i, 3, L"");
		}	
		

        // ��յײ�stNum��״̬
        _updateStatus(true);

        return;
    }

    // �������ʱͨ�������Ƿ�仯
    if(_isChannelChange())
    {
        _resetPage();       // ����ҳ��ص���ʼ״̬
    }

    CString csBlank;
    int nCounts             = m_data.GetChannelCounts();
    int nChann              = m_nChann;         // ��һ�м�¼��ͨ�����
    int nSubChann           = m_nSubChann;      // ��һ�м�¼ͨ����ŵ���ͨ�����
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

        // �����ͨ�����Ϊ-1������븸ͨ�����ṹ���ͣ�
        if(-1 == nSubChann)
        {
            _insertRecord(pChann, nChann, nSubChann, i++);
        }

        // ��Ϊ��ͨ�����ṹ���ͣ�����ö����ͨ��
        if(pGsData && GSEDT_STRUCT == pChann->GetChannelData()->type && i < m_nRows)
        {
            CGooseChannel* pSubChan = NULL;
            uint32_t nCounts        = pChann->GetSubChannelCount();         // ��ͨ������
            nSubChann               = (nSubChann == -1) ? 0 : nSubChann;    // ����ö����ͨ����λ��

            ASSERT(((GOOSE_STRUCT*)pGsData)->member.size() == nCounts);

            // ö�ٲ�����ͨ��
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
		
			// �����ͨ�����Ϊ-1������븸ͨ�����ṹ���ͣ�
			if(-1 == nSubChann)
			{
				_insertRecord(pChann, nChann, nSubChann, i++);
			}

			// ��Ϊ��ͨ�����ṹ���ͣ�����ö����ͨ��
			if(pGsData && GSEDT_STRUCT == pChann->GetChannelData()->type && i < m_nRows)
			{
				CGooseChannel* pSubChan = NULL;
				uint32_t nCounts        = pChann->GetSubChannelCount();         // ��ͨ������
				nSubChann               = (nSubChann == -1) ? 0 : nSubChann;    // ����ö����ͨ����λ��

				ASSERT(((GOOSE_STRUCT*)pGsData)->member.size() == nCounts);

				// ö�ٲ�����ͨ��
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
			// �����ͨ�����Ϊ-1������븸ͨ�����ṹ���ͣ�
			if(-1 == nSubChann)
			{
				_insertRecord(pChann, nChann, nSubChann, i++);
			}

			// ��Ϊ��ͨ�����ṹ���ͣ�����ö����ͨ��
			if(pGsData && GSEDT_STRUCT == pChann->GetChannelData()->type && i < m_nRows)
			{
				CGooseChannel* pSubChan = NULL;
				uint32_t nCounts        = pChann->GetSubChannelCount();         // ��ͨ������
				nSubChann               = (nSubChann == -1) ? 0 : nSubChann;    // ����ö����ͨ����λ��

				ASSERT(((GOOSE_STRUCT*)pGsData)->member.size() == nCounts);

				// ö�ٲ�����ͨ��
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

    // ����stNum��״̬
    _updateStatus();
}


/**
 *      ˢ�±���ײ���stNum,sqNum,TTL״̬
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
    // TODO: �ڴ����ר�ô����/����û���
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
 *      �������·�ҳ�ڵ�һ����ʾ��ͨ����ź���ͨ�����
 *
 *  \return
 *      bool            ������ҳ����true�����򷵻�false
 */
bool CGooseRealTimeDlg::_calcDown()
{
    uint32_t nCount = m_data.GetChannelCounts();

    if(nCount == 0)
    {
        return false;
    }

    int nRow                    = 0;                // �ۼ���ʾ������
    int nChan                   = m_nChann;         // ��ǰ�����ͨ������
    int nSubChan                = m_nSubChann;      // ��ǰ����ͨ������ͨ������
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

        // ��ǰͨ���ǽṹ�����ͣ�����ͨ�����ͣ�
        if(GSEDT_STRUCT == pGooseData->type)
        {
            pGooseSt        = (GOOSE_STRUCT*)pGooseData;
            size_t nSize    = pGooseSt->member.size();

            // �����ͨ������ʾ������
            nRow = nRow + nSize - nSubChan;

            // �ж����һҳ�����
            if(nRow == m_nRows && !m_data.GetChanneData(nChan + 1))
            {
                return false;
            }

            // �����ͨ���ڱ�ҳ����ʾ���꣬����һҳ������ʾ��ͨ��
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

        // ��ͨ���ڱ�ҳ�ܱ���ȫ��ʾ����ö����һͨ�������ָ���ͨ������Ϊ-1
        nSubChan = -1;
        pChannel = m_data.GetChanneData(++nChan);
    }
#else
	while(pChannel)
	{
		pGooseData = pChannel->GetChannelData();
		ASSERT(pGooseData);

		// ��ǰͨ���ǽṹ�����ͣ�����ͨ�����ͣ�
		if(GSEDT_STRUCT == pGooseData->type)
		{
			pGooseSt        = (GOOSE_STRUCT*)pGooseData;
			size_t nSize    = pGooseSt->member.size();

			if (!m_bIsShowDetail)
			{
				
				// �����ͨ������ʾ������
				nRow += 2;
			}
			else
			{
				// �����ͨ������ʾ������
				nRow = nRow + nSize - nSubChan;
			}
			
			
			

			// �ж����һҳ�����
			if(nRow == m_nRows && !m_data.GetChanneData(nChan + 1))
			{
				return false;
			}

			// �����ͨ���ڱ�ҳ����ʾ���꣬����һҳ������ʾ��ͨ��
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

		// ��ͨ���ڱ�ҳ�ܱ���ȫ��ʾ����ö����һͨ�������ָ���ͨ������Ϊ-1
		nSubChan = -1;
		pChannel = m_data.GetChanneData(++nChan);
	}
#endif
    return false;
}


/**
 *      �������Ϸ�ҳ�ڵ�һ����ʾ��ͨ����ź���ͨ�����
 *
 *  \return
 *      bool            ������ҳ����true�����򷵻�false
 */
bool CGooseRealTimeDlg::_calcUp()
{
    uint32_t nCount = m_data.GetChannelCounts();

    if(nCount == 0)
    {
        return false;
    }

    int nRow                    = 0;                // �ۼ���ʾ������
    int nChan                   = m_nChann;         // ��ǰ��ʾͨ������
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
                // m_nSubChannΪδ��ʾ����ͨ��
                // ���m_nSubChann֮ǰ��ͨ��δ��ʾ������
                nRow = nRow + m_nSubChann + 1;
            }
            else
            {
                // ��ͨ��ȫ��δ��ʾnsize����ͨ��+1�Ǹ�ͨ������
                nRow = nRow + nSize + 1;
            }

            // ����ʾ��ͨ��������ÿҳ�����ʾͨ����
            // ��Ҫ����ĳ�����ӣ�ͨ��������������ʾΪ��һ��
            if(nRow >= m_nRows)
            {
                m_nPage--;

                m_nChann            = nChan;                    // ĳͨ������
                m_nSubChann         = nRow - m_nRows - 1;       // ����ͨ�������������������
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

            // ����ʾ��ͨ��
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

        // ö����һ��ͨ��
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
				// m_nSubChannΪδ��ʾ����ͨ��
				// ���m_nSubChann֮ǰ��ͨ��δ��ʾ������
				nRow = nRow + m_nSubChann + 1;
			}
			else
			{
				if (!m_bIsShowDetail)
				{
					// �����ͨ������ʾ������
					nRow += 2;
				}
				else{
					nRow = nRow + nSize + 1;
				}
				
				
				
			}

			// ����ʾ��ͨ��������ÿҳ�����ʾͨ����
			// ��Ҫ����ĳ�����ӣ�ͨ��������������ʾΪ��һ��
			if(nRow >= m_nRows || nChan == 0)
			{
				m_nPage--;
				if (!m_bIsShowDetail)
				{
					m_nChann            = nChan;                    // ĳͨ������
				}
				else
					m_nChann            = nChan;
				m_nSubChann         = nRow - m_nRows - 1;       // ����ͨ�������������������
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

			// ����ʾ��ͨ��
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

		// ö����һ��ͨ��
		pChannel = m_data.GetChanneData(--nChan);
	}

#endif
    return false;
}

/**
 *      ͨ����ҳ - ��һҳ
 */
void CGooseRealTimeDlg::_channelUp()
{
    if(_calcUp())
    {
        _updateData();
    }
}

/**
 *      ͨ����ҳ - ��һҳ
 */
void CGooseRealTimeDlg::_channelDown()
{
    if(_calcDown())
    {
        _updateData();
    }
}


/**
 *      ����ҳ��
 */
void CGooseRealTimeDlg::_SavePage()
{
}

/**
 *      ��ԭҳ������
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
 *      ����һ��ͨ����¼
 *
 *  \param[in]
 *      pChannel        ͨ������
 *  \param[in]
 *      nChannel        goose����ͨ����
 *  \param[in]
 *      nSubChannel     goose������ͨ����
 *  \param[in]
 *      nRow            �����к�
 */
void CGooseRealTimeDlg::_insertRecord( CGooseChannel* pChannel, int nChannel, int nSubChannel, int nRow )
{
    if(!pChannel || !pChannel->GetChannelData())
    {
        ASSERT(FALSE);
        return;
    }

    CString csIndex(L""), csType(L""), csValue(L""), csDesc(pChannel->GetChannelDesc().c_str());

    // �жϸ���ͨ��
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
 *      ���ݲ˵�ID�л���ָ���Ŀ��ƿ�
 *
 *  \param[in]
 *      nCmdID          �˵�����ID
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
 *      ���gooseͨ�������������Ƿ�仯
 *
 *  \param[in]
 *      bool            ͨ�����������ͱ仯����true��û�仯����false
 */
bool CGooseRealTimeDlg::_isChannelChange()
{
    // �ѳ�ʼ����m_vecChannels
    if(m_bInitChannelData)
    {
        // ͨ�����������仯�������ص�ҳ��ĳ�ʼ״̬
        if(m_vecChannels.size() != m_data.GetChannelCounts())
        {
            return true;
        }

        CGooseChannel* pChannelNew  = NULL;
        CGooseChannel* pChannelOld  = NULL;

        uint32_t nCounts            = m_data.GetChannelCounts();

        // ���ÿ��ͨ�������Ƿ�仯���仯�򷵻�ҳ���ʼ״̬
        for(uint32_t i = 0; i < nCounts; i++)
        {
            pChannelNew = m_data.GetChanneData(i);
            pChannelOld = m_vecChannels.at(i);
            ASSERT(pChannelOld && pChannelNew);

            // �������Ͳ�ͬ
            if(pChannelNew->GetChannelData()->type != pChannelOld->GetChannelData()->type)
            {
                return true;
            }

            // �ṹ����ͨ������Ҫ�ж���ͨ��������
            if(pChannelNew->GetChannelData()->type == GSEDT_STRUCT)
            {
                // ��ͨ�����������
                if(pChannelNew->GetSubChannelCount() != pChannelOld->GetSubChannelCount())
                {
                    return true;
                }

                uint32_t nSubCounts = pChannelNew->GetSubChannelCount();

                // �����ͨ��������
                for(uint32_t k = 0; k < nSubCounts; k++)
                {
                    if(pChannelNew->GetSubChannelData(k)->GetChannelData()->type != pChannelOld->GetSubChannelData(k)->GetChannelData()->type)
                    {
                        return true;
                    }
                } // for ��ͨ��
            } // if �ṹ����ͨ��
        } // for ��ͨ��
    }
    else
    {
        CGooseChannel* pChannel = NULL;
        m_bInitChannelData      = true;         // ��ʼ�����
        uint32_t nCounts        = m_data.GetChannelCounts();

        // ���浱ǰͨ������Ϣ
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
    // �ͷž�ͨ������
    uint32_t nCounts = m_vecChannels.size();
    for(uint32_t i = 0; i < nCounts; i++)
    {
        delete m_vecChannels.at(i);
    }

    m_vecChannels.clear();
    CGooseChannel* pChannel = NULL;
    nCounts                 = m_data.GetChannelCounts();

    // ���浱ǰͨ������Ϣ
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

    // ��λ�����ʵ�ҳ��
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
		m_menuMain.SetMenuItemCaption(2,L"���ؽṹ");
	}
	else
	{
		m_menuMain.SetMenuItemCaption(2,L"��ʾ�ṹ");//dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_FILTER));
	}
	m_pNaviBar->UpdateNaviMenu();
	_updateData();
}

/**
 *      �޸Ĳ˵�HMENU
 */
void CGooseRealTimeDlg::_modifyMenu()
{
    __super::_modifyMenu();
	

 //   // �޸�F3�ַ���Ϊ�����ؽṹ��
	//if (m_bIsShowDetail)
	//{
	//	m_menuMain.SetMenuItemCaption(2,L"���ؽṹ");
	//}
	//else
	//{
	//	m_menuMain.SetMenuItemCaption(2,L"��ʾ�ṹ");//dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_FILTER));
	//}
}