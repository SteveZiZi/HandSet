// .\dlg\ScanSmvDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ScanSmvDlg.h"


#include "src/main/eventproc.h"
#include "src/main/resourcestr.h"
#include "src/main/resourcebmp.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/mgr/navibarmgr.h"
#include "src/utils/res/dsm_stringmgr.h"
#include "src/utils/utility/dsm_utility.h"


//////////////////////////////////////////////////////////////////////////
#if SU_FEI
// 9X探测报表
#define DSM_SCAN9X_WIDTH_CHEBOX         48
#define DSM_SCAN9X_WIDTH_TYPE           91
#define DSM_SCAN9X_WIDTH_APPID          110
#define DSM_SCAN9X_WIDTH_DESC           260
#define DSM_SCAN9X_WIDTH_PORT           91

// FT3探测报表
#define DSM_SCANFT3_WIDTH_CHEBOX        48
#define DSM_SCANFT3_WIDTH_SAMPLE        91
#define DSM_SCANFT3_WIDTH_BAUD          91
#define DSM_SCANFT3_WIDTH_CHANS         91
#define DSM_SCANFT3_WIDTH_LDNAME        188
#define DSM_SCANFT3_WIDTH_PORT          91

#else
// 9X探测报表
#define DSM_SCAN9X_WIDTH_CHEBOX         48
#define DSM_SCAN9X_WIDTH_TYPE           81
#define DSM_SCAN9X_WIDTH_APPID          103
#define DSM_SCAN9X_WIDTH_DESC           206
#define DSM_SCAN9X_WIDTH_PORT           91

// FT3探测报表
#define DSM_SCANFT3_WIDTH_CHEBOX        48
#define DSM_SCANFT3_WIDTH_SAMPLE        81
#define DSM_SCANFT3_WIDTH_BAUD          81
#define DSM_SCANFT3_WIDTH_CHANS         81
#define DSM_SCANFT3_WIDTH_LDNAME        147
#define DSM_SCANFT3_WIDTH_PORT          91

// goose - 探测
//
#define DSM_GOOSE_SCAN_HEAD_CHECKBOX        48          ///< 选择控制块栏
#define DSM_GOOSE_SCAN_HEAD_INDEX           81        ///< 控制块索引
#define DSM_GOOSE_SCAN_HEAD_APPID           103         ///< 控制块AppID
#define DSM_GOOSE_SCAN_HEAD_DESC            206         ///< 控制块描述，没有描述信息则显示MAC地址
#define DSM_GOOSE_SCAN_HEAD_PORT            91          ///< 端口名

#endif


#define DSM_SCANSMV_SCAN_INTERVAL           300         ///< 获取探测数据的时间间隔
#define DSM_SCANSMV_SCAN_TIMER_ID           1           ///< 探测定时器iD

#define DSM_SCANSMV_9X_CTRLBLK_MAX_NUMS     2           ///< 9x允许选控制块

#define DSM_GOOSE_CTRLBLK_MAX_NUM           10          ///< GOOSE最多可选控制块个数


/**
 *      SMV9X探测记录项
 *
 *  \note
 *      checkbox、索引、帧类型、AppID、描述、端口类型
 */
class CDsmScan9xSmvRecord : public CELRecord
{
public:
    /**
     *      SMV9X探测记录项
     *
     *  \param[in]
     *      pInfo           控制块信息
     */
    CDsmScan9xSmvRecord(CNetDetectItem* pInfo);
    virtual ~CDsmScan9xSmvRecord()
    {
    }

public:
    /**
     *      获取记录关联的控制块信息
     */
    inline CNetDetectItem* _GetInfo()
    {
        return m_pInfo;
    }

private:
    CNetDetectItem*         m_pInfo;        ///< 记录关联的控制块信息
};


/**
 *      FT3探测记录项
 *
 *  \note
 *      端口、采样值、波特率、通道数、LDNAME
 */
class CDsmScanFT3Record : public CELRecord
{
public:

    /**
     *      FT3探测记录项
     *
     *  \param[in]
     *      pInfo           控制块信息
     */
    CDsmScanFT3Record(CFt3DetectItem* pInfo);
    virtual ~CDsmScanFT3Record()
    {
    }

public:
    /**
     *      获取记录关联的控制块信息
     */
    inline CFt3DetectItem* _GetInfo()
    {
        return m_pInfo;
    }

    /**
     *      更新记录信息
     */
    void _Update();

private:
    CFt3DetectItem*         m_pInfo;        ///< 记录关联的控制块信息
};


CDsmScan9xSmvRecord::CDsmScan9xSmvRecord(CNetDetectItem* pInfo )
: m_pInfo(pInfo)
{
    ASSERT(m_pInfo);

    CString csType(L"*");
    CString csID;

    // 加载协议类型描述
    switch(m_pInfo->enFrameType)
    {
    case FRAME_TYPE_NET_9_1:
        csType = dsm_stringMgr::Get(DSM_STR_TYPE_91);
        break;
    case FRAME_TYPE_NET_9_2:
        csType = dsm_stringMgr::Get(DSM_STR_TYPE_92);
        break;
    default:
        csType = dsm_stringMgr::Get(DSM_STR_TYPE_UNKNOWN);
        ASSERT(FALSE);
        break;
    }

    csID.Format(L"0x%04X", m_pInfo->appId);

    CString csDesc = DSM_GetNetCtrlBlkDesc(m_pInfo);

    
    AddItem(new CDsmRecordItem(csType));
    AddItem(new CDsmRecordItem(csID));
    AddItem(new CDsmRecordItem(csDesc));
    AddItem(new CDsmRecordItem(DSM_GetPortDesc(m_pInfo->enPortName)));
	CELRecordItem* pItem = AddItem(new CDsmRecordItem(L""));

    pItem->SetEditable(TRUE);
    pItem->HasCheckbox(TRUE);
}


CDsmScanFT3Record::CDsmScanFT3Record( CFt3DetectItem* pInfo )
: m_pInfo(pInfo)
{
    ASSERT(m_pInfo);

    CString csPort, csSample, csBaud, csChannel, csLDName;

    csLDName.Format(L"0x%04X", m_pInfo->nLDName);
    csChannel.Format(L"%u", m_pInfo->nChannelCount);

    // 加载端口类型描述
    switch(m_pInfo->enPortName)
    {
    case FT3_PORT_LIGHT_A:
        csPort = dsm_stringMgr::Get(DSM_STR_PORT_LIGHTA);
        break;
    case FT3_PORT_LIGHT_B:
        csPort = dsm_stringMgr::Get(DSM_STR_PORT_LIGHTB);
        break;
    case FT3_PORT_LIGHT_C:
        csPort = dsm_stringMgr::Get(DSM_STR_PORT_LIGHTC);
        break;
    default:
        csPort = dsm_stringMgr::Get(DSM_STR_PORT_UNKNOWN);
        ASSERT(FALSE);
        break;
    }

    // 加载采样率描述
    switch(m_pInfo->nSampleRate)
    {
    case 4000:
        csSample = dsm_stringMgr::Get(DSM_STR_SAMPLE_4K);
        break;
    case 5000:
        csSample = dsm_stringMgr::Get(DSM_STR_SAMPLE_5K);
        break;
    case 8000:
        csSample = dsm_stringMgr::Get(DSM_STR_SAMPLE_8K);
        break;
    case 10000:
        csSample = dsm_stringMgr::Get(DSM_STR_SAMPLE_10K);
        break;
    case 12800:
        csSample = dsm_stringMgr::Get(DSM_STR_SAMPLE_12P8K);
        break;
    default:
        csSample = dsm_stringMgr::Get(DSM_STR_SAMPLE_UNKNOWN);
        ASSERT(FALSE);
        break;
    }

    // 加载波特率描述
    switch(m_pInfo->enBaudRate)
    {
    case FT3_BAUD_RATE_2P5M:
        csBaud = dsm_stringMgr::Get(DSM_STR_BAUD_2P5M);
        break;
    case FT3_BAUD_RATE_5M:
        csBaud = dsm_stringMgr::Get(DSM_STR_BAUD_5M);
        break;
    case FT3_BAUD_RATE_10M:
        csBaud = dsm_stringMgr::Get(DSM_STR_BAUD_10M);
        break;
    default:
        csBaud = dsm_stringMgr::Get(DSM_STR_BAUD_UNKNOWN);
        ASSERT(FALSE);
        break;
    }

    
    AddItem(new CDsmRecordItem(csSample));
    AddItem(new CDsmRecordItem(csBaud));
    AddItem(new CDsmRecordItem(csChannel));
    AddItem(new CDsmRecordItem(csLDName));
    AddItem(new CDsmRecordItem(csPort));

	CELRecordItem* pItem = AddItem(new CDsmRecordItem(L""));

    pItem->SetEditable(TRUE);
    pItem->HasCheckbox(TRUE);
}

void CDsmScanFT3Record::_Update()
{
    ASSERT(m_pInfo);

    CString csPort, csSample, csBaud, csChannel, csLDName;

    csLDName.Format(L"0x%04X", m_pInfo->nLDName);
    csChannel.Format(L"%u", m_pInfo->nChannelCount);

    // 加载端口类型描述
    switch(m_pInfo->enPortName)
    {
    case FT3_PORT_LIGHT_A:
        csPort = dsm_stringMgr::Get(DSM_STR_PORT_LIGHTA);
        break;
    case FT3_PORT_LIGHT_B:
        csPort = dsm_stringMgr::Get(DSM_STR_PORT_LIGHTB);
        break;
    case FT3_PORT_LIGHT_C:
        csPort = dsm_stringMgr::Get(DSM_STR_PORT_LIGHTC);
        break;
    default:
        csPort = dsm_stringMgr::Get(DSM_STR_PORT_UNKNOWN);
        ASSERT(FALSE);
        break;
    }

    // 加载采样率描述
    switch(m_pInfo->nSampleRate)
    {
    case 4000:
        csSample = dsm_stringMgr::Get(DSM_STR_SAMPLE_4K);
        break;
    case 5000:
        csSample = dsm_stringMgr::Get(DSM_STR_SAMPLE_5K);
        break;
    case 8000:
        csSample = dsm_stringMgr::Get(DSM_STR_SAMPLE_8K);
        break;
    case 10000:
        csSample = dsm_stringMgr::Get(DSM_STR_SAMPLE_10K);
        break;
    case 12800:
        csSample = dsm_stringMgr::Get(DSM_STR_SAMPLE_12P8K);
        break;
    default:
        csSample = dsm_stringMgr::Get(DSM_STR_SAMPLE_UNKNOWN);
        ASSERT(FALSE);
        break;
    }

    // 加载波特率描述
    switch(m_pInfo->enBaudRate)
    {
    case FT3_BAUD_RATE_2P5M:
        csBaud = dsm_stringMgr::Get(DSM_STR_BAUD_2P5M);
        break;
    case FT3_BAUD_RATE_5M:
        csBaud = dsm_stringMgr::Get(DSM_STR_BAUD_5M);
        break;
    case FT3_BAUD_RATE_10M:
        csBaud = dsm_stringMgr::Get(DSM_STR_BAUD_10M);
        break;
    default:
        csBaud = dsm_stringMgr::Get(DSM_STR_BAUD_UNKNOWN);
        ASSERT(FALSE);
        break;
    }
#if SU_FEI
    GetItem(1)->SetCaption(csSample);
    GetItem(2)->SetCaption(csBaud);
    GetItem(3)->SetCaption(csChannel);
    GetItem(4)->SetCaption(csLDName);
    GetItem(5)->SetCaption(csPort);
#else

	GetItem(0)->SetCaption(csSample);
	GetItem(1)->SetCaption(csBaud);
	GetItem(2)->SetCaption(csChannel);
	GetItem(3)->SetCaption(csLDName);
	GetItem(4)->SetCaption(csPort);
#endif
}


#if !SU_FEI
/**
 *      GOOSE探测数据记录项
 *
 *  \note
 *      checkbox、序号、AppID、描述、端口
 */
class CDsmScanGooseRecord_second : public CELRecord
{
public:
    /**
     *      GOOSE探测记录项
     *
     *  \param[in]
     *      nIndex          索引
     *  \param[in]
     *      pInfo           控制块信息
     */
    CDsmScanGooseRecord_second(int nIndex, CNetDetectItem* pInfo);
    virtual ~CDsmScanGooseRecord_second() {}

public:
    /**
     *      获取goose记录返回的控制块信息
     */
    inline CNetDetectItem* _GetInfo()
    {
        return m_pInfo;
    }

private:
    CNetDetectItem*         m_pInfo;            ///< 记录关联的控制块信息
};


CDsmScanGooseRecord_second::CDsmScanGooseRecord_second( int nIndex, CNetDetectItem* pInfo)
: m_pInfo(pInfo)
{
    ASSERT(m_pInfo);

    CString csAppID;
    csAppID.Format(L"0x%04X", m_pInfo->appId);

   
    AddItem(new CDsmRecordItem(nIndex));
    AddItem(new CDsmRecordItem(csAppID));
    AddItem(new CDsmRecordItem(DSM_GetNetCtrlBlkDesc(m_pInfo)));
    AddItem(new CDsmRecordItem(DSM_GetPortDesc(m_pInfo->enPortName)));

	CELRecordItem* pItem = AddItem(new CDsmRecordItem(L""));

    pItem->SetEditable(TRUE);
    pItem->HasCheckbox(TRUE);
}

#endif


// CScanSmvDlg 对话框

IMPLEMENT_DYNAMIC(CScanSmvDlg, CBaseDlg)

CScanSmvDlg::CScanSmvDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CScanSmvDlg::IDD, DSM_STR_SCANSMV_CAPTION, pParent)
    , m_nScanTimerID(0)
    , m_pDataLayer(CDsmDataLayer::getInstance())
   , m_eDetectType(SMVSCAN::PROTO_TYPE_9X)
  //  , m_eDetectType(SMVSCAN::PROTO_TYPE_FT3)
{
    m_nMenuID = IDR_MENU_SMV_SCAN;
}

CScanSmvDlg::~CScanSmvDlg()
{
    _clearRes();
}

void CScanSmvDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CScanSmvDlg, CBaseDlg)
#if SU_FEI
    ON_COMMAND_RANGE(ID_MENU_SCANSMV_SWITCH_9X, ID_MENU_SCANSMV_ENTER, &CScanSmvDlg::OnNaviMenuFn)
    ON_COMMAND_RANGE(ID_MENU_SCANSMV_AUTO, ID_MENU_SCANSMV_12P8K, &CScanSmvDlg::OnMenuSample)
#else
	ON_COMMAND_RANGE(ID_MENU_SCANSMV_SWITCH_9X, ID_MENU_SCANSMV_WAVE, &CScanSmvDlg::OnNaviMenuFn)
#endif
    ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_SCANSMV_9X_RET, &CScanSmvDlg::OnItemChecked9X)
    ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_SCANSMV_FT3_RET, &CScanSmvDlg::OnItemCheckedFT3)
	ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_GOOSE_SCAN_REPORT, &CScanSmvDlg::OnItemCheckGoose)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CScanSmvDlg 消息处理程序

BOOL CScanSmvDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    // 重置采样率为自动探测模式
    CSmvRecvConfigPublic* pCfg = CDsmDataLayer::getInstance()->GetSmvConfigPublic();
    pCfg->SetSmv92SampleRate(SMV92_SAMPLE_RATE_AUTO_DETECT);

    m_cs9XCaption.Format(L"%s->%s", 
        dsm_stringMgr::Get(DSM_STR_SCANSMV_CAPTION), 
        dsm_stringMgr::Get(DSM_STR_SCANSMV_CAPTION_9X));

    m_csFT3Caption.Format(L"%s->%s", 
        dsm_stringMgr::Get(DSM_STR_SCANSMV_CAPTION), 
        dsm_stringMgr::Get(DSM_STR_SCANSMV_CAPTION_FT3));

#if !SU_FEI
	//设置goose探测标题
	m_csGooseCaption.Format(L"%s->%s", 
		dsm_stringMgr::Get(DSM_STR_SCANSMV_CAPTION),
		dsm_stringMgr::Get(DSM_STR_GOOSE_SCAN_CAPTION));
#endif

    m_nScanTimerID = SetTimer(DSM_SCANSMV_SCAN_TIMER_ID, 
        DSM_SCANSMV_SCAN_INTERVAL, NULL);

    CPageData* pPD = CEventProc::GetInstance()->GetCurrentPageData();
    if(pPD && pPD->m_eAction == PageBack)
    {
        if(IDC_SCANSMV_9X_RET == pPD->m_nCtrlID)
        {
            m_eDetectType = SMVSCAN::PROTO_TYPE_9X;
        }
        else if(IDC_SCANSMV_FT3_RET == pPD->m_nCtrlID)
        {
            m_eDetectType = SMVSCAN::PROTO_TYPE_FT3;
        }
#if !SU_FEI
		else if (IDC_GOOSE_SCAN_REPORT == pPD->m_nCtrlID)
		{
			m_eDetectType = SMVSCAN::PROTO_TYPE_GOOSE;
		}
#endif
    }

    if( SMVSCAN::PROTO_TYPE_9X == m_eDetectType)
    {
        _scan9X();
    }
    else if(SMVSCAN::PROTO_TYPE_FT3 == m_eDetectType)
    {
        _scanFT3();
    }
#if !SU_FEI
	else if (SMVSCAN::PROTO_TYPE_GOOSE == m_eDetectType)
	{
		_scanGoose();
	}
#endif
    else
    {
        ASSERT(FALSE);
    }

    return FALSE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CScanSmvDlg::OnNaviMenuFn( UINT nID )
{
    // 处理FT3/9X探测、重新探测、清空选择

    switch(nID)
    {
        // 切换探测9X
    case ID_MENU_SCANSMV_SWITCH_9X:

        if(m_eDetectType != SMVSCAN::PROTO_TYPE_9X)
        {
#if !SU_FEI
			//m_ctl9X.SetChecked(FALSE);
			
#endif
            _scan9X();
        }

        break;

        // 切换探测FT3
    case ID_MENU_SCANSMV_SWITCH_FT3:

        if(m_eDetectType != SMVSCAN::PROTO_TYPE_FT3)
        {
#if !SU_FEI
			//m_ctlFT3.SetChecked(FALSE);		

#else
            m_pNaviBar->SetCheck(1, TRUE);
#endif
            _scanFT3();
        }

        break;

    case ID_MENU_SCANSMV_RESCAN:

        // 重新探测9X
        if(SMVSCAN::PROTO_TYPE_9X == m_eDetectType)
        {
            _scan9X();
        }
        // 重新探测FT3
        else if(SMVSCAN::PROTO_TYPE_FT3 == m_eDetectType)
        {
            _scanFT3();
        }
#if  !SU_FEI
		else if (SMVSCAN::PROTO_TYPE_GOOSE == m_eDetectType)
		{
			_scanGoose();
		}
#endif
        break;

#if SU_FEI
        // 取消所选
    case ID_MENU_SCANSMV_CLRSEL:

        if(SMVSCAN::PROTO_TYPE_9X == m_eDetectType)
        {
            _clearSelect(&m_ctl9X);
        }
        else if(SMVSCAN::PROTO_TYPE_FT3 == m_eDetectType)
        {
            _clearSelect(&m_ctlFT3);
        }
        break;

#else

		//录波
	case ID_MENU_SCANSMV_WAVE:
/*
		if(SMVSCAN::PROTO_TYPE_9X == m_eDetectType)
		{
			_clearSelect(&m_ctl9X);
		}
		else if(SMVSCAN::PROTO_TYPE_FT3 == m_eDetectType)
		{
			_clearSelect(&m_ctlFT3);
		}
		else if (SMVSCAN::PROTO_TYPE_GOOSE == m_eDetectType)
		{
			_clearSelect(&m_ctlGOOSE);
		}
*/
		{
			
		}

		break;
	case ID_MENU_SCANSMV_SWITCH_GOOSE:
		if(m_eDetectType != SMVSCAN::PROTO_TYPE_GOOSE)
		{
		//	m_ctlGOOSE.SetChecked(FALSE);
			_scanGoose();
		}

		break;
		
#endif

        // 确定所选控制块
    case ID_MENU_SCANSMV_ENTER:

#if SU_FEI
        if(_setSelected())
        {
            CEventProc::GetInstance()->ShowPage(IDD_SMV_EFFECTIVE_VALUE_DLG);
        }
#else
		if (_setSelected())
		{
			if (m_eDetectType == SMVSCAN::PROTO_TYPE_9X || m_eDetectType == SMVSCAN::PROTO_TYPE_FT3)
			{
				
				CEventProc::GetInstance()->ShowPage(IDD_SMV_EFFECTIVE_VALUE_DLG);
			}
			else if (m_eDetectType == SMVSCAN::PROTO_TYPE_GOOSE)
			{
				CEventProc::GetInstance()->ShowPage(IDD_GOOSE_REALTIME_DLG);
			}
		}


#endif
        break;

    default:
        break;
    }
}


void CScanSmvDlg::_insertColumn9x()
{
    int nIndex = 0;

    // 插入列元素
    
    m_ctl9X.InsertColumn(nIndex++, L"    "  + dsm_stringMgr::Get(DSM_STR_SCANSMV_9XRPT_TYPE), EL_CENTER, DSM_SCAN9X_WIDTH_TYPE);       // 类型
    m_ctl9X.InsertColumn(nIndex++, L"   "  + dsm_stringMgr::Get(DSM_STR_SCANSMV_9XRPT_APPID), EL_CENTER, DSM_SCAN9X_WIDTH_APPID);     // APPID
    m_ctl9X.InsertColumn(nIndex++, dsm_stringMgr::Get(DSM_STR_SCANSMV_9XRPT_DESC), EL_LEFT, DSM_SCAN9X_WIDTH_DESC);         // 描述
    m_ctl9X.InsertColumn(nIndex++, dsm_stringMgr::Get(DSM_STR_SCANSMV_9XRPT_PORT), EL_LEFT, DSM_SCAN9X_WIDTH_PORT);         // 端口
	m_ctl9X.InsertColumn(nIndex++, L"", EL_CENTER, DSM_SCAN9X_WIDTH_CHEBOX);   // checkbox
}


void CScanSmvDlg::_insertColumnFT3()
{
    int nIndex = 0;

    // 插入列元素
    
    m_ctlFT3.InsertColumn(nIndex++, dsm_stringMgr::Get(DSM_STR_SCANSMV_FT3RPT_SAMPLE), EL_CENTER, DSM_SCANFT3_WIDTH_SAMPLE);    // 采样率
    m_ctlFT3.InsertColumn(nIndex++, dsm_stringMgr::Get(DSM_STR_SCANSMV_FT3RPT_BAUD), EL_CENTER, DSM_SCANFT3_WIDTH_BAUD);        // 波特率
    m_ctlFT3.InsertColumn(nIndex++, dsm_stringMgr::Get(DSM_STR_SCANSMV_FT3RPT_CHANS), EL_CENTER, DSM_SCANFT3_WIDTH_CHANS);      // 通道数目
    m_ctlFT3.InsertColumn(nIndex++, L"      "  + dsm_stringMgr::Get(DSM_STR_SCANSMV_FT3RPT_LDNAME), EL_CENTER, DSM_SCANFT3_WIDTH_LDNAME);    // LDNAME
    m_ctlFT3.InsertColumn(nIndex++, dsm_stringMgr::Get(DSM_STR_SCANSMV_FT3RPT_PORT), EL_LEFT, DSM_SCANFT3_WIDTH_PORT);          // 端口名
	m_ctlFT3.InsertColumn(nIndex++, L"", EL_CENTER, DSM_SCANFT3_WIDTH_CHEBOX); // checkbox
}

#if !SU_FEI
void CScanSmvDlg::_insertColumnGoose()
{
	 int nIndex = 0;
	m_ctlGOOSE.InsertColumn(nIndex++, L"    "  + dsm_stringMgr::Get(DSM_STR_GOOSE_SCAN_HEAD_INDEX), EL_CENTER, DSM_GOOSE_SCAN_HEAD_INDEX);     // 序号
	m_ctlGOOSE.InsertColumn(nIndex++, L"   "  + dsm_stringMgr::Get(DSM_STR_GOOSE_SCAN_HEAD_APPID), EL_CENTER, DSM_GOOSE_SCAN_HEAD_APPID);     // APPID
	m_ctlGOOSE.InsertColumn(nIndex++, dsm_stringMgr::Get(DSM_STR_GOOSE_SCAN_HEAD_DESC), EL_LEFT, DSM_GOOSE_SCAN_HEAD_DESC);         // 描述
	m_ctlGOOSE.InsertColumn(nIndex++, dsm_stringMgr::Get(DSM_STR_GOOSE_SCAN_HEAD_PORT), EL_LEFT, DSM_GOOSE_SCAN_HEAD_PORT);       // 端口
	m_ctlGOOSE.InsertColumn(nIndex++, L"", EL_CENTER, DSM_GOOSE_SCAN_HEAD_CHECKBOX);   // checkbox

}
#endif

void CScanSmvDlg::_createReport(CReportV* pReport, UINT nID)
{
    CRect rcReport;
    GetClientRect(rcReport);
 //   rcReport.DeflateRect(DSM_MARGIN_IN_RECT);

    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    pReport->Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, nID);

    // 设置字体
    pReport->SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    pReport->SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // 设置样式
    pReport->PostInitTemplateStyle();
#if SU_FEI
	pReport->SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL));
#else
	

	pReport->SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
								dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));
	
	
#endif
    // 插入9X列
    if(IDC_SCANSMV_9X_RET == nID)
    {
        _insertColumn9x();
    }
    // 插入FT列
    else if(IDC_SCANSMV_FT3_RET == nID)
    {
        _insertColumnFT3();
    }
#if !SU_FEI
	//插入goose列
	else if (IDC_GOOSE_SCAN_REPORT == nID)
	{
		_insertColumnGoose();
	}
#endif

    // 设置选择焦点样式
    pReport->SetRowSelectionVisible(TRUE);
    pReport->SetRowFocusEnable(TRUE);
    pReport->SetSubItemFocusEnable(TRUE);
    pReport->SetFreezeSelectionVisible(TRUE);
    pReport->SetFocusedColIndex(4);
}


void CScanSmvDlg::_scan9X()
{
    _updateMenu(SMVSCAN::PROTO_TYPE_9X);

    if(m_ctlFT3.GetSafeHwnd())
    {
        m_ctlFT3.ShowWindow(SW_HIDE);
    }
#if !SU_FEI

	if (m_ctlGOOSE.GetSafeHwnd())
	{
		m_ctlGOOSE.ShowWindow(SW_HIDE);
	}
#endif


    if(!m_ctl9X.GetSafeHwnd())
    {
        _createReport(&m_ctl9X, IDC_SCANSMV_9X_RET);
    }
    else
    {
        m_ctl9X.ShowWindow(SW_SHOW);
    }

    _clearRes();

    // 探测9X控制块
    m_pDataLayer->BeginDetect(NET_RECV_TYPE_SMV);

    m_eDetectType = SMVSCAN::PROTO_TYPE_9X;

    m_ctl9X.SetFocus();
}

void CScanSmvDlg::_scanFT3()
{
    _updateMenu(SMVSCAN::PROTO_TYPE_FT3);

    if(m_ctl9X.GetSafeHwnd())
    {
        m_ctl9X.ShowWindow(SW_HIDE);
    }
#if !SU_FEI

	if (m_ctlGOOSE.GetSafeHwnd())
	{
		m_ctlGOOSE.ShowWindow(SW_HIDE);
	}
#endif
    if(!m_ctlFT3.GetSafeHwnd())
    {
        _createReport(&m_ctlFT3, IDC_SCANSMV_FT3_RET);
    }
    else
    {
        m_ctlFT3.ShowWindow(SW_SHOW);
    }


    _clearRes();

    // 探测FT3
    m_pDataLayer->BeginDetect(FT3_RECV_TYPE_SMV);

    m_eDetectType = SMVSCAN::PROTO_TYPE_FT3;

#if !CHUN_HUA
	m_ctlFT3.SetFocusedColIndex(5);
#endif
    m_ctlFT3.SetFocus();
}

#if !SU_FEI

void CScanSmvDlg::_scanGoose()
{
	_updateMenu(SMVSCAN::PROTO_TYPE_GOOSE);

	if(m_ctl9X.GetSafeHwnd())
	{
		m_ctl9X.ShowWindow(SW_HIDE);
	}

	if(m_ctlFT3.GetSafeHwnd())
	{
		m_ctlFT3.ShowWindow(SW_HIDE);
	}

	if(!m_ctlGOOSE.GetSafeHwnd())
	{
		_createReport(&m_ctlGOOSE, IDC_GOOSE_SCAN_REPORT);
	}
	else
	{
		m_ctlGOOSE.ShowWindow(SW_SHOW);
	}
	_clearRes();

	// 探测GOOSE控制块
	m_pDataLayer->BeginDetect(NET_RECV_TYPE_GOOSE);

	m_eDetectType = SMVSCAN::PROTO_TYPE_GOOSE;

	m_ctlGOOSE.SetFocus();
}

#endif

void CScanSmvDlg::_insertRecordTo9X()
{
    unsigned int nCounts        = m_ctl9X.GetRecordRowCount();
    unsigned int nDetectCounts  = m_pDataLayer->GetNetDetectItemsCount();

    // 若无新的探测结果则返回
    if(nDetectCounts == nCounts || !m_ctl9X.GetSafeHwnd())
    {
        return;
    }

    CNetDetectItem* pInfo= NULL;

    // 插入新的记录
    for(unsigned int i = nCounts; i < nDetectCounts; i++)
    {
        pInfo = m_pDataLayer->GetNetDetectItem(i);

        if(!pInfo)
        {
            ASSERT(pInfo);
            TRACE(L"dsm: _insertRecordTo9X: get 9X ctrlblk error\n");
            continue;
        }

        m_ctl9X.InsertRecordRow(i, new CDsmScan9xSmvRecord(pInfo));

        for(int nColumn = 0; nColumn < 4; nColumn++)
        {
            m_ctl9X.GetRecordRow(i)->GetItem(nColumn)->SetFocusable(FALSE);
        }

        if(0 == i)
        {
            m_ctl9X.SetFocusedSelectionRowIndex(0);
        }

        m_lstRecords.AddTail(pInfo);
    }
}

void CScanSmvDlg::_insertRecordToFT3()
{
    unsigned int nCounts        = m_ctlFT3.GetRecordRowCount();
    unsigned int nDetectCounts  = m_pDataLayer->GetFt3DetectItemsCount();

	/*if(!m_ctlFT3.GetSafeHwnd())
	{
		return;
	}*/

	// 若无新的探测结果则返回
	if(nDetectCounts == nCounts || !m_ctlFT3.GetSafeHwnd())
	{
		return;
	}


   // CDsmScanFT3Record* pRecord = NULL;


    //// 更新旧数据显示
    //for(unsigned int i = 0; i < nCounts; i++)
    //{
    //    pRecord = dynamic_cast<CDsmScanFT3Record*>(m_ctlFT3.GetRecordRow(i));
    //    if(pRecord)
    //    {
    //        pRecord->_Update();
    //    }
    //}
    CFt3DetectItem* pInfo = NULL;

    // 插入新的记录
    for(unsigned int i = nCounts; i < nDetectCounts; i++)
    {
        pInfo = m_pDataLayer->GetFt3DetectItem(i);

        if(!pInfo)
        {
            ASSERT(pInfo);
            TRACE(L"dsm: get FT3 ctrlblk error\n");
            continue;
        }

        m_ctlFT3.InsertRecordRow(i, new CDsmScanFT3Record(pInfo));

        for(int nColumn = 0; nColumn < 5; nColumn++)
        {
            m_ctlFT3.GetRecordRow(i)->GetItem(nColumn)->SetFocusable(FALSE);
        }

        if(0 == i)
        {
            m_ctlFT3.SetFocusedSelectionRowIndex(0);
        }

        m_lstRecords.AddTail(pInfo);
    }

	//m_ctlFT3.Invalidate(TRUE);
}


#if !SU_FEI
void CScanSmvDlg::_insertRecordToGoose()
{
	unsigned int nCounts        = m_ctlGOOSE.GetRecordRowCount();
	unsigned int nDetectCounts  = m_pDataLayer->GetNetDetectItemsCount();

	// 若无新的探测结果则返回
	if(nDetectCounts == nCounts ||
		!m_ctlGOOSE.GetSafeHwnd())
	{
		return;
	}

	CNetDetectItem* pInfo= NULL;

	// 插入新的记录
	for(unsigned int i = nCounts; i < nDetectCounts; i++)
	{
		pInfo = m_pDataLayer->GetNetDetectItem(i);

		if(!pInfo)
		{
			TRACE(L"dsm: get goose ctrlblk error\n");
			continue;
		}

		m_ctlGOOSE.InsertRecordRow(i, new CDsmScanGooseRecord_second(i, pInfo));

		for(int nColumn = 0; nColumn < 4; nColumn++)
		{
			m_ctlGOOSE.GetRecordRow(i)->GetItem(nColumn)->SetFocusable(FALSE);
		}

		if(0 == i)
		{
			m_ctlGOOSE.SetFocusedSelectionRowIndex(0);
		}

		m_lstRecords.AddTail(pInfo);
	}
}


#endif


void CScanSmvDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    if(SMVSCAN::PROTO_TYPE_9X == m_eDetectType)
    {
        _insertRecordTo9X();
    }
    else if(SMVSCAN::PROTO_TYPE_FT3 == m_eDetectType)
    {
        _insertRecordToFT3();
    }
#if !SU_FEI
	else if (SMVSCAN::PROTO_TYPE_GOOSE == m_eDetectType)
	{
		_insertRecordToGoose();
	}

#endif

    CBaseDlg::OnTimer(nIDEvent);
}

void CScanSmvDlg::_clearRes()
{
    if(m_ctl9X.GetSafeHwnd())
    {
        m_ctl9X.DeleteAllRecordRow();
    }

    if(m_ctlFT3.GetSafeHwnd())
    {
        m_ctlFT3.DeleteAllRecordRow();
    }
#if !SU_FEI

	if (m_ctlGOOSE.GetSafeHwnd())
	{
		m_ctlGOOSE.DeleteAllRecordRow();
	}
	
#endif

    m_lstSelect.RemoveAll();
    m_lstRecords.RemoveAll();
}

void CScanSmvDlg::OnItemChecked9X( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    *pResult = TRUE;

    ELNM_RECORDITEM_ARGS* pNS = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
    ASSERT(pNS);

    CDsmScan9xSmvRecord* pRecord = dynamic_cast<CDsmScan9xSmvRecord*>(m_ctl9X.GetRecordRow(pNS->rowIndex));
    ASSERT(pRecord);

    CNetDetectItem* pInfo = pRecord->_GetInfo();
    ASSERT(pInfo);

    // 取消选择，将从m_lst9XSels列表中删除元素
    if(!pNS->pItem->IsChecked())
    {
        CNetDetectItem* pTempInfo = NULL;
        POSITION prepos;
        POSITION pos = m_lstSelect.GetHeadPosition();
        while(pos)
        {
            prepos = pos;
            pTempInfo = dynamic_cast<CNetDetectItem*>(m_lstSelect.GetNext(pos));
            ASSERT(pTempInfo);

            if(pTempInfo && pTempInfo->nID == pInfo->nID)
            {
                TRACE(L"dsm: delete %d\n", pInfo->nID);
                m_lstSelect.RemoveAt(prepos);
                break;
            }
        }

        return;
    }

    // 选中一个元素，判断是否允许加入选中列表m_lst9XSels

    // 控制块超过可选数目，取消选择
    if(m_lstSelect.GetCount() >= DSM_SCANSMV_9X_CTRLBLK_MAX_NUMS)
    {
        CString csTip;
        CString csFmt(dsm_stringMgr::Get(DSM_STR_SCANSMV_NUMMAX));
        csTip.Format(csFmt, DSM_SCANSMV_9X_CTRLBLK_MAX_NUMS);
        CStatusMgr::SetTips(csTip);

        pNS->pItem->SetChecked(FALSE);
        return;
    }

    // 控制块类型不同，取消选择
    if(m_lstSelect.GetCount() > 0)
    {
        CNetDetectItem* pInfo = dynamic_cast<CNetDetectItem*>(m_lstSelect.GetHead());
        ASSERT(pInfo);

        if(pInfo->enFrameType != pRecord->_GetInfo()->enFrameType)
        {
            pNS->pItem->SetChecked(FALSE);
            return;
        }

        // 采样率不同，取消选择
        if(pInfo->nSampleRate != pRecord->_GetInfo()->nSampleRate)
        {
            pNS->pItem->SetChecked(FALSE);
            return;
        }
    }

    TRACE(L"dsm: add %d\n", pRecord->_GetInfo()->nID);
    m_lstSelect.AddTail(pRecord->_GetInfo());
}

void CScanSmvDlg::OnItemCheckedFT3( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    *pResult = TRUE;

    ELNM_RECORDITEM_ARGS* pNS = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
    ASSERT(pNS);

    CDsmScanFT3Record* pRecord = dynamic_cast<CDsmScanFT3Record*>(m_ctlFT3.GetRecordRow(pNS->rowIndex));
    ASSERT(pRecord);

    CFt3DetectItem* pInfo = pRecord->_GetInfo();
    ASSERT(pInfo);

    // 取消元素选中，从m_lstFT3Sels中移除
    if(!pNS->pItem->IsChecked())
    {
        CFt3DetectItem* pTempInfo = NULL;
        POSITION prepos;
        POSITION pos = m_lstSelect.GetHeadPosition();
        while(pos)
        {
            prepos      = pos;
            pTempInfo   = dynamic_cast<CFt3DetectItem*>(m_lstSelect.GetNext(pos));

            if(pTempInfo && pTempInfo->nID == pInfo->nID)
            {
                m_lstSelect.RemoveAt(prepos);
                break;
            }
        }

        return;
    }

	// 控制块超过可选数目，取消选择
	if(m_lstSelect.GetCount() >= DSM_SCANSMV_9X_CTRLBLK_MAX_NUMS)
	{
		CString csTip;
		CString csFmt(dsm_stringMgr::Get(DSM_STR_SCANSMV_NUMMAX));
		csTip.Format(csFmt, DSM_SCANSMV_9X_CTRLBLK_MAX_NUMS);
		CStatusMgr::SetTips(csTip);

		pNS->pItem->SetChecked(FALSE);
		return;
	}
    if(m_lstSelect.GetCount() > 0)
    {
        POSITION pos = m_lstSelect.GetHeadPosition();
        while(pos)
        {
            if(m_lstSelect.GetNext(pos)->enPortName == pInfo->enPortName)
            {
                CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_SCANSMV_NUMMAX_FT3));

                pNS->pItem->SetChecked(FALSE);
                return;
            }
        }
    }

    // 添加选中元素
    m_lstSelect.AddTail(pRecord->_GetInfo());
}

#if !SU_FEI

void CScanSmvDlg::OnItemCheckGoose(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	ELNM_RECORDITEM_ARGS* pNS = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
	ASSERT(pNS);

	CDsmScanGooseRecord_second* pRecord = dynamic_cast<CDsmScanGooseRecord_second*>(m_ctlGOOSE.GetRecordRow(pNS->rowIndex));
	ASSERT(pRecord);

	*pResult = TRUE;
	CNetDetectItem* pInfo = pRecord->_GetInfo();

	// 取消选择，将从m_lstSelects列表中移除已选控制块
	if(!pNS->pItem->IsChecked())
	{
		CNetDetectItem* pTempInfo = NULL;
		POSITION prepos;
		POSITION pos = m_lstSelect.GetHeadPosition();
		while(pos)
		{
			prepos = pos;
			pTempInfo = dynamic_cast<CNetDetectItem*>(m_lstSelect.GetNext(pos));
			ASSERT(pTempInfo);

			if(pTempInfo && pTempInfo->appId == pInfo->appId && pTempInfo->enPortName == pInfo->enPortName)
			{
				m_lstSelect.RemoveAt(prepos);
				break;
			}
		}

		return;
	}

//#pragma message(__FILE__ "(" DSM_STRING(__LINE__) "): 是否需要过滤不同端口过来的相同控制块")

	// 控制块超过可选数目，取消选择
	if(m_lstSelect.GetCount() >= DSM_GOOSE_CTRLBLK_MAX_NUM)
	{
		CString csTip;
		CString csFmt(dsm_stringMgr::Get(DSM_STR_GOOSE_SCAN_NUMMAX));
		csTip.Format(csFmt, DSM_GOOSE_CTRLBLK_MAX_NUM);
		CStatusMgr::SetTips(csTip);

		pNS->pItem->SetChecked(FALSE);
		return;
	}

	m_lstSelect.AddTail(pRecord->_GetInfo());
}

#endif


bool CScanSmvDlg::_setSelected()
{
    CBaseDetectItem* pInfo  = NULL;
    POSITION pos            = m_lstSelect.GetHeadPosition();

    if(m_lstSelect.GetCount() <= 0)
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_SCANSMV_SETSEL_TIPS));
        return false;
    }

    // 增加检查92的通道数限制
    if(!_checkChannels())
    {
        return false;
    }

    while(pos)
    {
        pInfo = m_lstSelect.GetNext(pos);
        if(!m_pDataLayer->AddOneSelectedCtrlBlk(pInfo))
        {
            TRACE(L"设置所选控制块失败");
            ASSERT(FALSE);
            return false;
        }
    }

    m_pDataLayer->AddSelectedCtrlBlkFinished();

    return true;
}


void CScanSmvDlg::_clearSelect(CReportV* pReport)
{
    CELRecordItem* pItem    = NULL;

    int nCounts = pReport->GetRecordRowCount();
    for(int i = 0; i < nCounts; i++)
    {
        pItem = pReport->GetRecordRow(i)->GetItem(0);

        if(pItem->IsChecked())
        {
            pItem->SetChecked(FALSE);
            pReport->RedrawRowItem(i, 0);
        }
    }

    m_lstSelect.RemoveAll();
}

void CScanSmvDlg::_SavePage()
{
    CPageData* pPD = CEventProc::GetInstance()->GetCurrentPageData();

    if(pPD)
    {
        if(SMVSCAN::PROTO_TYPE_9X == m_eDetectType)
        {
            pPD->m_nCtrlID = IDC_SCANSMV_9X_RET;
        }
        else if(SMVSCAN::PROTO_TYPE_FT3 == m_eDetectType)
        {
            pPD->m_nCtrlID = IDC_SCANSMV_FT3_RET;
        }
#if !SU_FEI
		else if (SMVSCAN::PROTO_TYPE_GOOSE == m_eDetectType)
		{
			pPD->m_nCtrlID = IDC_GOOSE_SCAN_REPORT;
		}
#endif
    }
}

void CScanSmvDlg::_modifyMenu()
{
    m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_9X_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_9X_SEL), NULL);
    m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_FT3_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_FT3_SEL), NULL);
#if !SU_FEI
	m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_GOOSE_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_GOOSE_SEL), NULL);
#endif
}

bool CScanSmvDlg::_checkChannels()
{
    CBaseDetectItem* pCtrlBlk   = NULL;
    POSITION pos                = m_lstSelect.GetHeadPosition();

    while(pos)
    {
        pCtrlBlk = m_lstSelect.GetNext(pos);

        // 12.8k采样率，通道数大于30则返回false
        if(pCtrlBlk && 
            pCtrlBlk->nSampleRate == 12800 && 
            pCtrlBlk->nChannelCount > DSM_CHANNEL_MAX)
        {
            CString csTips, csID;

           CNetDetectItem* pNet = dynamic_cast<CNetDetectItem*>(pCtrlBlk);
            if(pNet)
            {
                csID.Format(L"0x%04X", pNet->appId);
            }

            CFt3DetectItem* pFt3 = dynamic_cast<CFt3DetectItem*>(pCtrlBlk);
            if(pFt3)
            {
                csID.Format(L"0x%04X", pFt3->nLDName);
            }

            if(!pNet && !pFt3)
            {
                csTips.Format(dsm_stringMgr::Get(DSM_STR_INFO_CHANNEL_EXCEED_MAX), DSM_CHANNEL_MAX);
            }
            else
            {
                csTips.Format(dsm_stringMgr::Get(DSM_STR_INFO_SCAN_CHANNEL_EXCEED_MAX), csID, DSM_CHANNEL_MAX);
            }

            CStatusMgr::SetTips(csTips);

            return false;
        }
    }

    return true;
}


#if SU_FEI
void CScanSmvDlg::OnMenuSample( UINT nID )
{
    CSmvRecvConfigPublic* pCfg = CDsmDataLayer::getInstance()->GetSmvConfigPublic();

    for(int i = 0; i < 3; i++)
    {
        CNaviBarMgr::GetInstance()->HighlightMenuItem(2, i, FALSE);
    }

    CString csTitle(m_cs9XCaption);

    switch(nID)
    {
    case ID_MENU_SCANSMV_AUTO:
        pCfg->SetSmv92SampleRate(SMV92_SAMPLE_RATE_AUTO_DETECT);
        CNaviBarMgr::GetInstance()->HighlightMenuItem(2, 0);
        csTitle += L" - " + dsm_stringMgr::Get(DSM_STR_SCAN_AUTO);
        break;

    case ID_MENU_SCANSMV_4K:
        pCfg->SetSmv92SampleRate(SMV92_SAMPLE_RATE_4K);
        CNaviBarMgr::GetInstance()->HighlightMenuItem(2, 1);
        csTitle += L" - " + dsm_stringMgr::Get(DSM_STR_SCAN_4K);
        break;

    case ID_MENU_SCANSMV_12P8K:
        pCfg->SetSmv92SampleRate(SMV92_SAMPLE_RATE_12P8K);
        CNaviBarMgr::GetInstance()->HighlightMenuItem(2, 2);
        csTitle += L" - " + dsm_stringMgr::Get(DSM_STR_SCAN_12P8K);
        break;

    default:
        ASSERT(FALSE);
        break;
    }

    CStatusMgr::SetTitle(csTitle);
}

#endif

void CScanSmvDlg::_updateMenu( SMVSCAN::SMVSCAN_PROTO_TYPE eType )
{
    if(eType == SMVSCAN::PROTO_TYPE_9X)
    {
		m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_9X_SEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_FT3_UNSEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_GOOSE_UNSEL), NULL, NULL);

#if SU_FEI
        CMenu menu;
        VERIFY(menu.CreatePopupMenu());

        menu.AppendMenu(MF_STRING, ID_MENU_SCANSMV_AUTO, dsm_stringMgr::Get(DSM_STR_SCAN_AUTO));
        menu.AppendMenu(MF_STRING, ID_MENU_SCANSMV_4K, dsm_stringMgr::Get(DSM_STR_SCAN_4K));
        menu.AppendMenu(MF_STRING, ID_MENU_SCANSMV_12P8K, dsm_stringMgr::Get(DSM_STR_SCAN_12P8K));

        CNaviMenu* pNaviMenu = new CNaviMenu;
        pNaviMenu->LoadMenu(&menu);
        menu.DestroyMenu();

        m_pNaviBar->GetNaviMenu()->SetMenuSubItem(2, pNaviMenu);
        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_SCAN_SAMPLE));

        m_pNaviBar->UpdateNaviMenu();

        CSmvRecvConfigPublic* pCfg = CDsmDataLayer::getInstance()->GetSmvConfigPublic();
        CString csTitle(m_cs9XCaption);

        switch(pCfg->GetSmv92SampleRate())
        {
        case SMV92_SAMPLE_RATE_AUTO_DETECT:
            CNaviBarMgr::GetInstance()->HighlightMenuItem(2, 0);
            csTitle += L" - " + dsm_stringMgr::Get(DSM_STR_SCAN_AUTO);
            break;

        case SMV92_SAMPLE_RATE_4K:
            CNaviBarMgr::GetInstance()->HighlightMenuItem(2, 1);
            csTitle += L" - " + dsm_stringMgr::Get(DSM_STR_SCAN_4K);
            break;

        case SMV92_SAMPLE_RATE_12P8K:
            CNaviBarMgr::GetInstance()->HighlightMenuItem(2, 2);
            csTitle += L" - " + dsm_stringMgr::Get(DSM_STR_SCAN_12P8K);
            break;

        default:
            ASSERT(FALSE);
            break;
        }

        CStatusMgr::SetTitle(csTitle);
#else
		CStatusMgr::SetTitle(m_cs9XCaption);

#endif
    }
    else if(eType == SMVSCAN::PROTO_TYPE_FT3)
    {
        CStatusMgr::SetTitle(m_csFT3Caption);

		m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_9X_UNSEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_FT3_SEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_GOOSE_UNSEL), NULL, NULL);
#if SU_FEI
        m_pNaviBar->GetNaviMenu()->SetMenuSubItem(2, NULL);
        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, L"");
	
#endif
       
    }
#if !SU_FEI
	else if (eType == SMVSCAN::PROTO_TYPE_GOOSE)
	{
		CStatusMgr::SetTitle(m_csGooseCaption);
		m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_9X_UNSEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_FT3_UNSEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_GOOSE_SEL), NULL, NULL);
	}
#endif 
	
	m_pNaviBar->UpdateNaviMenu();
}

