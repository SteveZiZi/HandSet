// WinCETestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WinCETest.h"
#include "WinCETestDlg.h"
#include "src/utils/MessageCodec/include/msg_rapid_decoder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MSG_CODEC_COUNT 3

#define DECODE_TIMER    1000

#define PI 3.14159265

static uint8_t k_Sav91[] = {
	0x01,0x0C,0xCD,0x04,0x09,0x06,\
	0x00,0x30,0x18,0xA1,0x5B,0xED,\
	0x81,0x00,\
	0x80,0x00,0x88,0xBA,\
	0x09,0x06,0x00,0x3A,0x00,0x00,0x00,0x00,\
	0x80,0x5E,0x00,0x02,\
	0x00,0x2C,\
	0x02,0x01,0x00,0x00,0x00,0x05,0x00,0x05,0x00,0x64,0x00,0x64,0x00,0x50,0x06,0x8F,\
	0xF5,0x3A,0x01,0x40,0x01,0x90,0x13,0xAD,0xE6,0xDC,0x03,0x20,0x24,0xEC,0xFB,0xEA,\
	0x00,0x5B,0x00,0x5B,0x00,0x57,0x00,0x4D,0x34,0x31,0x50,0x00,\
	0x00,0x2C,\
	0x02,0x01,0x00,0x00,0x00,0x05,0x00,0x05,0x00,0x64,0x00,0x64,0x00,0x50,0x06,0x8F,\
	0xF5,0x3A,0x01,0x40,0x01,0x90,0x13,0xAD,0xE6,0xDC,0x03,0x20,0x24,0xEC,0xFB,0xEA,\
	0x00,0x5B,0x00,0x5B,0x00,0x57,0x00,0x4D,0x34,0x31,0x50,0x00
};

static uint16_t k_NanRuiFT3[NANRUI_FT3_MSG_WORD] = {
	0x0564,0x002c,0x0201,0x0001,0x0005,0x0005,0x0039,0x0000,
	0x0000,0xe04c,0x003a,0xff7d,0x0047,0xfffe,0x05ab,0xf333,
	0x06f0,0x0256,0xdd64,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x039d,0x5001,0xb409
};

static uint16_t k_GuoWangFT3[GUOWANG_FT3_MSG_WORD] = {
	0x0564,0x003E,0x02FE,0x4042,0x0BB8,0x0BB8,0x044C,0x02EE,
	0x0D1D,0x834C,0x027E,0xFF3F,0xFE44,0x027E,0x3E68,0xED13,
	0xD485,0x3E68,0x88B6,0xED13,0xD485,0x3E68,0x3E68,0x3E68,
	0x3E68,0x3E68,0x3E68,0x2B0A,0x3E68,0x3E68,0x3E68,0x3E68,
	0x3E68,0x3E68,0x0000,0x0000,0x3C2E
};

static uint16_t k_1PhaseCtvtFt3[ONEPHASE_CTORVT_FT3_MSG_WORD] = {
	0x0564,0x0110,0x0201,0x0001,0x0002,0x0003,0x0004,0x0005,0x0000,0x1234,
	0x0000,0x0001,0x1234
};

static uint16_t k_3PhaseCtFt3[TRIPHASE_CT_FT3_MSG_WORD] = {
	0x0564,0x0210,0x0201,0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x1234,
	0x0007,0x0008,0x0009,0x0000,0x0000,0x1111,0x1234
};

static uint16_t k_3PhaseVt10Ft3[TRIPHASE_VT_FT3_2010_MSG_WORD] = {
	0x0564,0x0310,0x0201,0x0001,0x0002,0x0003,0x0004,0x0005,0x4D1B,
	0x0006,0x0000,0x0000,0x1111,0xD803
};

static uint16_t k_3PhaseVt11Ft3[TRIPHASE_VT_FT3_2011_MSG_WORD] = {
	0x0564,0x0310,0x0201,0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x00EE,
	0x0000,0x0000,0x1111,0x8E4B
};

static uint16_t k_3PhaseCtvtFt3[TRIPHASE_CTORVT_FT3_MSG_WORD] = {
	0x0564,0x0410,0x0201,0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x1234,
	0x0007,0x0008,0x0009,0x000A,0x000B,0x000C,0x000D,0x000E,0x1234,
	0x000F,0x0000,0x0000,0x1111,0x1234
};

static uint16_t k_Std52Ft3[STD52_FT3_MSG_WORD] = {
	0x0564,0x002A,0x0201,0x0001,0x1111,0x2222,0x3333,0x4444,0x1234,
	0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x0007,0x0008,0x1234,
	0x0009,0x000A,0x000B,0x000C,0x0000,0x0000,0x1111,0x1234
};

static uint8_t k_Sav91Ex[] = {
	0x01,0x0C,0xCD,0x04,0x09,0x06,\
	0x00,0x30,0x18,0xA1,0x5B,0xED,\
	0x81,0x00,\
	0x80,0x00,0x88,0xBA,\
	0x09,0x06,0x00,0x3A,0x00,0x00,0x00,0x00,\
	0x80,0x5E,0x00,0x02,\
	0x00,0x24,\
	0x02,0xFE,0x00,0x00,0x00,0x01,0x00,0x02,0x00,0x03,0x00,0x04,0x00,0x05,0x00,0x06,\
	0x00,0x07,0x00,0x08,0x00,0x09,0x00,0x0A,0x00,0x0B,0x00,0x0C,\
	0x11,0x22,0x33,0x44,0x00,0x5B,0x50,0x57,\
	0x00,0x24,\
	0x02,0xFE,0x00,0x00,0x00,0x01,0x00,0x02,0x00,0x03,0x00,0x04,0x00,0x05,0x00,0x06,\
	0x00,0x07,0x00,0x08,0x00,0x09,0x00,0x0A,0x00,0x0B,0x00,0x0C,\
	0x11,0x22,0x33,0x00,0x44,0x5C,0x50,0x57
};

// CWinCETestDlg 对话框

CWinCETestDlg::CWinCETestDlg(CWnd* pParent /*=NULL*/)
: CDialog(CWinCETestDlg::IDD, pParent)
, m_PcapFile(CreatePcapFileImpl())
, m_TestData(_T(""))
, m_CodeTime(0)
, m_DecTime(0)
, m_pSav91Msg(CreateSav91MsgImpl())
, m_pSav91ExMsg(CreateSav91ExMsgImpl())
, m_pSav92Msg(CreateSav92MsgImpl())
, m_pGooseMsg(CreateGooseMsgImpl())
, m_pNanRuiFT3Msg(CreateNanRuiFT3MsgImpl())
, m_pGuoWangFT3Msg(CreateGuoWangFT3MsgImpl())
, m_pStd52FT3Msg(CreateStd52FT3MsgImpl())
, m_p1PhaseCtvtFt3(CreateCTorVTFT3MsgImpl(CFT_ONEPHASE_CTORVT))
, m_p3PhaseCtFt3(CreateCTorVTFT3MsgImpl(CFT_TRIPHASE_CT))
, m_p3PhaseVt10Ft3(CreateCTorVTFT3MsgImpl(CFT_TRIPHASE_VT_2010))
, m_p3PhaseVt11Ft3(CreateCTorVTFT3MsgImpl(CFT_TRIPHASE_VT_2011))
, m_p3PhaseCtvtFt3(CreateCTorVTFT3MsgImpl(CFT_TRIPHASE_CTORVT))
, m_pIeee1588Msg(CreateIeee1588MsgImpl())
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_vSav92Msg.resize(4);
	for ( size_t i=0; i<m_vSav92Msg.size(); i++ )
	{
		m_vSav92Msg.at(i).pSav92Msg = CreateSav92MsgImpl();
	}
}

CWinCETestDlg::~CWinCETestDlg()
{
	ReleasePcapFileImpl(m_PcapFile);
	ReleaseSav91MsgImpl(m_pSav91Msg);
	ReleaseSav91ExMsgImpl(m_pSav91ExMsg);
	ReleaseSav92MsgImpl(m_pSav92Msg);
	ReleaseGooseMsgImpl(m_pGooseMsg);
	ReleaseNanRuiFT3MsgImpl(m_pNanRuiFT3Msg);
	ReleaseGuoWangFT3MsgImpl(m_pGuoWangFT3Msg);
	ReleaseStd52FT3MsgImpl(m_pStd52FT3Msg);
	ReleaseCTorVTFT3MsgImpl(m_p1PhaseCtvtFt3);
	ReleaseCTorVTFT3MsgImpl(m_p3PhaseCtFt3);
	ReleaseCTorVTFT3MsgImpl(m_p3PhaseVt10Ft3);
	ReleaseCTorVTFT3MsgImpl(m_p3PhaseVt11Ft3);
	ReleaseCTorVTFT3MsgImpl(m_p3PhaseCtvtFt3);
	ReleaseIeee1588MsgImpl(m_pIeee1588Msg);
	for ( size_t i=0; i<m_vSav92Msg.size(); i++ )
	{
		ReleaseSav92MsgImpl(m_vSav92Msg.at(i).pSav92Msg);
	}
}

void CWinCETestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TEST_DATA, m_TestData);
	DDX_Text(pDX, IDC_READ_TIME, m_CodeTime);
	DDX_Text(pDX, IDC_WRITE_TIME, m_DecTime);
}

BEGIN_MESSAGE_MAP(CWinCETestDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN1, &CWinCETestDlg::OnBnClickedSav91Code)
	ON_BN_CLICKED(IDC_BTN2, &CWinCETestDlg::OnBnClickedSav91Dec)
	ON_BN_CLICKED(IDC_BTN3, &CWinCETestDlg::OnBnClickedSav92Code)
	ON_BN_CLICKED(IDC_BTN4, &CWinCETestDlg::OnBnClickedSav92Dec)
	ON_BN_CLICKED(IDC_BTN5, &CWinCETestDlg::OnBnClickedGooseCode)
	ON_BN_CLICKED(IDC_BTN6, &CWinCETestDlg::OnBnClickedGooseDec)
	ON_BN_CLICKED(IDC_BTN7, &CWinCETestDlg::OnBnClickedNanRuiCode)
	ON_BN_CLICKED(IDC_BTN8, &CWinCETestDlg::OnBnClickedNanRuiDec)
	ON_BN_CLICKED(IDC_BTN9, &CWinCETestDlg::OnBnClickedGuoWangCode)
	ON_BN_CLICKED(IDC_BTN10, &CWinCETestDlg::OnBnClickedGuoWangDec)
	ON_BN_CLICKED(IDC_BTN11, &CWinCETestDlg::OnBnClickedRapidDecode)
	ON_BN_CLICKED(IDC_BTN12, &CWinCETestDlg::OnBnClickedBtn12)
	ON_BN_CLICKED(IDC_SAV92_4APPID_TEST, &CWinCETestDlg::OnBnClickedSav92MulDec)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_1PHASE_CTVT_FT3_C, &CWinCETestDlg::OnBnClicked1phaseCtvtFt3C)
	ON_BN_CLICKED(IDC_1PHASE_CTVT_FT3_D, &CWinCETestDlg::OnBnClicked1phaseCtvtFt3D)
	ON_BN_CLICKED(IDC_3PHASE_CT_FT3_C, &CWinCETestDlg::OnBnClicked3phaseCtFt3C)
	ON_BN_CLICKED(IDC_3PHASE_CT_FT3_D, &CWinCETestDlg::OnBnClicked3phaseCtFt3D)
	ON_BN_CLICKED(IDC_3PHASE_VT10_FT3_C, &CWinCETestDlg::OnBnClicked3phaseVt10Ft3C)
	ON_BN_CLICKED(IDC_3PHASE_VT10_FT3_D, &CWinCETestDlg::OnBnClicked3phaseVt10Ft3D)
	ON_BN_CLICKED(IDC_3PHASE_VT11_FT3_C, &CWinCETestDlg::OnBnClicked3phaseVt11Ft3C)
	ON_BN_CLICKED(IDC_3PHASE_VT11_FT3_D, &CWinCETestDlg::OnBnClicked3phaseVt11Ft3D)
	ON_BN_CLICKED(IDC_3PHASE_CTVT_FT3_C, &CWinCETestDlg::OnBnClicked3phaseCtvtFt3C)
	ON_BN_CLICKED(IDC_3PHASE_CTVT_FT3_D, &CWinCETestDlg::OnBnClicked3phaseCtvtFt3D)
	ON_BN_CLICKED(IDC_STD52_FT3_C, &CWinCETestDlg::OnBnClickedStd52Ft3C)
	ON_BN_CLICKED(IDC_STD52_FT3_D, &CWinCETestDlg::OnBnClickedStd52Ft3D)
	ON_BN_CLICKED(IDC_SAV91_EX_C, &CWinCETestDlg::OnBnClickedSav91ExC)
	ON_BN_CLICKED(IDC_SAV91_EX_D, &CWinCETestDlg::OnBnClickedSav91ExD)
	ON_BN_CLICKED(IDC_STATUS_WORD_C, &CWinCETestDlg::OnBnClickedStatusWordC)
	ON_BN_CLICKED(IDC_STATUS_WORD_D, &CWinCETestDlg::OnBnClickedStatusWordD)
	ON_BN_CLICKED(IDC_TIME_QUALITY_C, &CWinCETestDlg::OnBnClickedTimeQualityC)
	ON_BN_CLICKED(IDC_TIME_QUALITY_D, &CWinCETestDlg::OnBnClickedTimeQualityD)
	ON_BN_CLICKED(IDC_QUALITY_C, &CWinCETestDlg::OnBnClickedQualityC)
	ON_BN_CLICKED(IDC_QUALITY_D, &CWinCETestDlg::OnBnClickedQualityD)
	ON_BN_CLICKED(IDC_IEEE1588_D, &CWinCETestDlg::OnBnClickedIeee1588D)
END_MESSAGE_MAP()


// CWinCETestDlg 消息处理程序

BOOL CWinCETestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CWinCETestDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_WINCETEST_DIALOG_WIDE) : 
		MAKEINTRESOURCE(IDD_WINCETEST_DIALOG));
	}
}
#endif

void CWinCETestDlg::DisplayEthHdr(ETHER_II_HEADER &ethhdr)
{
	CString str;
	str.Format(_T("DesMac:%02x-%02x-%02x-%02x-%02x-%02x\r\n"),ethhdr.desMac[0],ethhdr.desMac[1],
		ethhdr.desMac[2],ethhdr.desMac[3],ethhdr.desMac[4],ethhdr.desMac[5]);
	m_TestData += str;
	str.Format(_T("SrcMac:%02x-%02x-%02x-%02x-%02x-%02x\r\n"),ethhdr.srcMac[0],ethhdr.srcMac[1],
		ethhdr.srcMac[2],ethhdr.srcMac[3],ethhdr.srcMac[4],ethhdr.srcMac[5]);
	m_TestData += str;
	str.Format(_T("EtherType:%04x\r\n\r\n"),ethhdr.etherType);
	m_TestData += str;
}

void CWinCETestDlg::DisplayEthHdrOff(ETHER_II_HEADER_OFFSET &offset)
{
	CString str;
	str.Format(_T("DesMacOff:%d\r\n"),offset.desMacOff);
	m_TestData += str;
	str.Format(_T("SrcMacOff:%d\r\n"),offset.srcMacOff);
	m_TestData += str;
	str.Format(_T("EtherTypeOff:%d\r\n\r\n"),offset.etherTypeOff);
	m_TestData += str;
}

void CWinCETestDlg::DisplayCodeData(uint8_t *inData, size_t datLen)
{
	CString str;
	str.Format(_T("Code Data:\r\n"));
	m_TestData += str;
	for ( size_t i=0; i<datLen; i++)
	{
		str.Format(_T("%02x  "),(uint32_t)inData[i]);
		m_TestData += str;
	}
	str.Format(_T("\r\n\r\n"));
	m_TestData += str;
}

void CWinCETestDlg::DisplayVlanInfo(IEEE802_1Q_VLAN_INFO &info)
{
	CString str;
	str.Format(_T("userPriority:%01x\r\n"),(uint32_t)info.tci.userPriority);
	m_TestData += str;
	str.Format(_T("cfi:%01x\r\n"),(uint32_t)info.tci.cfi);
	m_TestData += str;
	str.Format(_T("vlanID:%03x\r\n"),(uint32_t)info.tci.vlanID);
	m_TestData += str;
	str.Format(_T("etherType:%04x\r\n\r\n"),(uint32_t)info.etherType);
	m_TestData += str;
}

void CWinCETestDlg::DisplayVlanInfoOff(IEEE802_1Q_VLAN_INFO_OFFSET &offset)
{
	CString str;
	str.Format(_T("TciOff:%d\r\n"),offset.tciOff);
	m_TestData += str;
	str.Format(_T("etherTypeOff:%d\r\n\r\n"),offset.etherTypeOff);
	m_TestData += str;
}

void CWinCETestDlg::DisplayIEC88023Pdu(IEC8802_3_PDU_PUBLIC &info)
{
	CString str;
	str.Format(_T("APPID:%04x\r\n"),info.appID);
	m_TestData += str;
	str.Format(_T("len:%d\r\n"),info.len);
	m_TestData += str;
	str.Format(_T("reserved1:%04x\r\n"),info.reserved1);
	m_TestData += str;
	str.Format(_T("reserved2:%04x\r\n\r\n"),info.reserved2);
	m_TestData += str;
}

void CWinCETestDlg::DisplayIEC88023PduOff(IEC8802_3_PDU_PUBLIC_OFFSET &offset)
{
	CString str;
	str.Format(_T("APPIDOff:%d\r\n"),offset.appIDOff);
	m_TestData += str;
	str.Format(_T("lenOff:%d\r\n"),offset.lenOff);
	m_TestData += str;
	str.Format(_T("reserved1Off:%d\r\n"),offset.reserved1Off);
	m_TestData += str;
	str.Format(_T("reserved2Off:%d\r\n\r\n"),offset.reserved2Off);
	m_TestData += str;
}

void CWinCETestDlg::DisplayChannelOff(size_t *channelOff,size_t channelNum)
{
	CString str;
	for ( size_t i = 0; i < channelNum; i++ )
	{
		str.Format(_T("Channel %d Off:%d\r\n"),i,channelOff[i]);
		m_TestData += str;
	}
	str.Format(_T("\r\n\r\n"));
	m_TestData += str;
}

void CWinCETestDlg::DisplaySav91AsduPbl(SAV91_ASDU_PUBLIC &asduPbl)
{
	CString str;
	str.Format(_T("asduLen:%d\r\n"),asduPbl.asduLen);
	m_TestData += str;
	str.Format(_T("lnName:%02x\r\n"),asduPbl.lnName);
	m_TestData += str;
	str.Format(_T("dataSetName:%02x\r\n"),asduPbl.dataSetName);
	m_TestData += str;
	str.Format(_T("ldName:%04x\r\n"),asduPbl.ldName);
	m_TestData += str;
	str.Format(_T("phsArtg:%d\r\n"),asduPbl.phsArtg);
	m_TestData += str;
	str.Format(_T("neutArtg:%d\r\n"),asduPbl.neutArtg);
	m_TestData += str;
	str.Format(_T("phsVrtg:%d\r\n"),asduPbl.phsVrtg);
	m_TestData += str;
	str.Format(_T("delayTime:%d\r\n"),asduPbl.delayTime);
	m_TestData += str;
	str.Format(_T("statusWord1:%04x\r\n"),asduPbl.statusWord1);
	m_TestData += str;
	str.Format(_T("statusWord2:%04x\r\n"),asduPbl.statusWord2);
	m_TestData += str;
	str.Format(_T("smpCnt:%d\r\n"),asduPbl.smpCnt);
	m_TestData += str;
	str.Format(_T("smpRate:%d\r\n"),asduPbl.smpRate);
	m_TestData += str;
	str.Format(_T("confRev:%d\r\n\r\n"),asduPbl.confRev);
	m_TestData += str;
}

void CWinCETestDlg::DisplaySav91AsduPblOff(SAV91_ASDU_PUBLIC_OFFSET &offset)
{
	CString str;
	str.Format(_T("asduLenOff:%d\r\n"),offset.asduLenOff);
	m_TestData += str;
	str.Format(_T("lnNameOff:%d\r\n"),offset.lnNameOff);
	m_TestData += str;
	str.Format(_T("dataSetNameOff:%d\r\n"),offset.dataSetNameOff);
	m_TestData += str;
	str.Format(_T("ldNameOff:%d\r\n"),offset.ldNameOff);
	m_TestData += str;
	str.Format(_T("phsArtgOff:%d\r\n"),offset.phsArtgOff);
	m_TestData += str;
	str.Format(_T("neutArtgOff:%d\r\n"),offset.neutArtgOff);
	m_TestData += str;
	str.Format(_T("phsVrtgOff:%d\r\n"),offset.phsVrtgOff);
	m_TestData += str;
	str.Format(_T("delayTimeOff:%d\r\n"),offset.delayTimeOff);
	m_TestData += str;
	str.Format(_T("statusWord1Off:%d\r\n"),offset.statusWord1Off);
	m_TestData += str;
	str.Format(_T("statusWord2Off:%d\r\n"),offset.statusWord2Off);
	m_TestData += str;
	str.Format(_T("smpCntOff:%d\r\n"),offset.smpCntOff);
	m_TestData += str;
	str.Format(_T("smpRateOff:%d\r\n"),offset.smpRateOff);
	m_TestData += str;
	str.Format(_T("confRevOff:%d\r\n\r\n"),offset.confRevOff);
	m_TestData += str;
}

void CWinCETestDlg::DisplayUint16Smps(uint16_t *smpVal, size_t smpsNum)
{
	CString str;
	for ( size_t i = 0; i < smpsNum; i++ )
	{
		str.Format(_T("Channel %d:%d\r\n"),i,(int16_t)smpVal[i]);
		m_TestData += str;
	}
	str.Format(_T("\r\n\r\n"));
	m_TestData += str;
}

void CWinCETestDlg::DisplaySav91ExAsduPbl(SAV91_EX_ASDU_PUBLIC &asduPbl)
{
	CString str;
	str.Format(_T("asduLen:%d\r\n"),asduPbl.asduLen);
	m_TestData += str;
	str.Format(_T("lnName:%02x\r\n"),asduPbl.lnName);
	m_TestData += str;
	str.Format(_T("dataSetName:%02x\r\n"),asduPbl.dataSetName);
	m_TestData += str;
	str.Format(_T("ldName:%04x\r\n"),asduPbl.ldName);
	m_TestData += str;
	str.Format(_T("statusWord1:%04x\r\n"),asduPbl.statusWord1);
	m_TestData += str;
	str.Format(_T("statusWord2:%04x\r\n"),asduPbl.statusWord2);
	m_TestData += str;
	str.Format(_T("smpCnt:%d\r\n"),asduPbl.smpCnt);
	m_TestData += str;
	str.Format(_T("smpRate:%d\r\n"),asduPbl.smpRate);
	m_TestData += str;
	str.Format(_T("confRev:%d\r\n\r\n"),asduPbl.confRev);
	m_TestData += str;
}

void CWinCETestDlg::DisplaySav91ExAsduPblOff(SAV91_EX_ASDU_PUBLIC_OFFSET &offset)
{
	CString str;
	str.Format(_T("asduLenOff:%d\r\n"),offset.asduLenOff);
	m_TestData += str;
	str.Format(_T("lnNameOff:%d\r\n"),offset.lnNameOff);
	m_TestData += str;
	str.Format(_T("dataSetNameOff:%d\r\n"),offset.dataSetNameOff);
	m_TestData += str;
	str.Format(_T("ldNameOff:%d\r\n"),offset.ldNameOff);
	m_TestData += str;
	str.Format(_T("statusWord1Off:%d\r\n"),offset.statusWord1Off);
	m_TestData += str;
	str.Format(_T("statusWord2Off:%d\r\n"),offset.statusWord2Off);
	m_TestData += str;
	str.Format(_T("smpCntOff:%d\r\n"),offset.smpCntOff);
	m_TestData += str;
	str.Format(_T("smpRateOff:%d\r\n"),offset.smpRateOff);
	m_TestData += str;
	str.Format(_T("confRevOff:%d\r\n\r\n"),offset.confRevOff);
	m_TestData += str;
}

void CWinCETestDlg::DisplaySav92AsduPbl(SAV92_ASDU_PUBLIC &asduPbl)
{
	CString str;
	m_TestData += _T("svID:");
	m_TestData += asduPbl.svID.c_str();
	m_TestData += _T("\r\n");
	m_TestData += _T("datSet:");
	m_TestData += asduPbl.datSet.c_str();
	m_TestData += _T("\r\n");
	str.Format(_T("smpCnt:%d\r\n"),asduPbl.smpCnt);
	m_TestData += str;
	str.Format(_T("confRev:%d\r\n"),asduPbl.confRev);
	m_TestData += str;
	CTime t((time_t)asduPbl.refrTm.ts_s);
	str.Format(_T("Refresh Time:%04d-%02d-%02d %02d:%02d:%02d.%09u  Time Quality:%02x\r\n"),
		t.GetYear(),t.GetMonth(),t.GetDay(),t.GetHour(),t.GetMinute(),t.GetSecond(),
		asduPbl.refrTm.ts_ns,asduPbl.refrTm.ts_q);
	m_TestData += str;
	str.Format(_T("smpSynch:%d\r\n"),asduPbl.smpSynch);
	m_TestData += str;
	str.Format(_T("smpRate:%d\r\n"),asduPbl.smpRate);
	m_TestData += str;
}

void CWinCETestDlg::DisplaySav92AsduPblOff(SAV92_ASDU_PUBLIC_OFFSET &offset)
{
	CString str;
	str.Format(_T("svIDOff:%d\r\n"),offset.svIDOff);
	m_TestData += str;
	str.Format(_T("datSetOff:%d\r\n"),offset.datSetOff);
	m_TestData += str;
	str.Format(_T("smpCntOff:%d\r\n"),offset.smpCntOff);
	m_TestData += str;
	str.Format(_T("confRevOff:%d\r\n"),offset.confRevOff);
	m_TestData += str;
	str.Format(_T("refrTmOff:%d\r\n"),offset.refrTmOff);
	m_TestData += str;
	str.Format(_T("smpSynchOff:%d\r\n"),offset.smpSynchOff);
	m_TestData += str;
	str.Format(_T("smpRateOff:%d\r\n"),offset.smpRateOff);
	m_TestData += str;
}

void CWinCETestDlg::DisplaySav92Smps(uint32_t *smpVal, uint32_t *smpQlt, size_t smpsNum)
{
	CString str;
	for ( size_t i = 0; i < smpsNum; i++ )
	{
		str.Format(_T("Channel %d:\r\n    nvalue:%d;  quality:%02x;\r\n"),i,smpVal[i],smpQlt[i]);
		m_TestData += str;
	}
	str.Format(_T("\r\n"));
	m_TestData += str;
}

void CWinCETestDlg::DisplayGoosePduPbl(GOOSE_PDU_PUBLIC &pduPbl)
{
	CString str;
	m_TestData += _T("gocbRef:");
	m_TestData += pduPbl.gocbRef.c_str();
	m_TestData += _T("\r\n");
	str.Format(_T("timeAllowedtoLive:%u\r\n"),pduPbl.timeAllowedtoLive);
	m_TestData += str;
	m_TestData += _T("datSet:");
	m_TestData += pduPbl.datSet.c_str();
	m_TestData += _T("\r\n");
	m_TestData += _T("goID:");
	m_TestData += pduPbl.goID.c_str();
	m_TestData += _T("\r\n");
	CTime t((time_t)pduPbl.eventTm.ts_s);
	//	time_t  t_s = (time_t)pduPbl.eventTm.ts_s;
	//	gmtime_s(&t_ymd,&t_s);
	str.Format(_T("Event Time:%04d-%02d-%02d %02d:%02d:%02d.%09u  Time Quality:%02x\r\n"),
		t.GetYear(),t.GetMonth(),t.GetDay(),t.GetHour(),t.GetMinute(),t.GetSecond(),
		pduPbl.eventTm.ts_ns,pduPbl.eventTm.ts_q);
	m_TestData += str;
	str.Format(_T("stNum:%u\r\n"),pduPbl.stNum);
	m_TestData += str;
	str.Format(_T("sqNum:%u\r\n"),pduPbl.sqNum);
	m_TestData += str;
	str.Format(_T("test:%d\r\n"),pduPbl.test);
	m_TestData += str;
	str.Format(_T("confRev:%u\r\n"),pduPbl.confRev);
	m_TestData += str;
	str.Format(_T("ndsCom:%d\r\n"),pduPbl.ndsCom);
	m_TestData += str;
	str.Format(_T("numDatSetEntries:%u\r\n"),pduPbl.numDatSetEntries);
	m_TestData += str;
	m_TestData += _T("security:");
	m_TestData += pduPbl.security.c_str();
	m_TestData += _T("\r\n\r\n");
}

void CWinCETestDlg::DisplayGoosePduPblOff(GOOSE_PDU_PUBLIC_OFFSET &offset)
{
	CString str;
	str.Format(_T("gocbRefOff:%d\r\n"),offset.gocbRefOff);
	m_TestData += str;
	str.Format(_T("timeAllowedtoLiveOff:%d\r\n"),offset.timeAllowedtoLiveOff);
	m_TestData += str;
	str.Format(_T("datSetOff:%d\r\n"),offset.datSetOff);
	m_TestData += str;
	str.Format(_T("goIDOff:%d\r\n"),offset.goIDOff);
	m_TestData += str;
	str.Format(_T("eventTmOff:%d\r\n"),offset.eventTmOff);
	m_TestData += str;
	str.Format(_T("stNumOff:%d\r\n"),offset.stNumOff);
	m_TestData += str;
	str.Format(_T("sqNumOff:%d\r\n"),offset.sqNumOff);
	m_TestData += str;
	str.Format(_T("testOff:%d\r\n"),offset.testOff);
	m_TestData += str;
	str.Format(_T("confRevOff:%d\r\n"),offset.confRevOff);
	m_TestData += str;
	str.Format(_T("ndsComOff:%d\r\n"),offset.ndsComOff);
	m_TestData += str;
	str.Format(_T("numDatSetEntriesOff:%d\r\n"),offset.numDatSetEntriesOff);
	m_TestData += str;
	str.Format(_T("securityOff:%d\r\n\r\n"),offset.securityOff);
	m_TestData += str;
}

void CWinCETestDlg::DisplayNanRuiAsduPbl(NANRUI_FT3_ASDU_PUBLIC &asduPbl)
{
	CString str;
	str.Format(_T("asduLen:%d\r\n"),asduPbl.asduLen);
	m_TestData += str;
	str.Format(_T("lnName:%02x\r\n"),asduPbl.lnName);
	m_TestData += str;
	str.Format(_T("dataSetName:%02x\r\n"),asduPbl.dataSetName);
	m_TestData += str;
	str.Format(_T("ldName:%04x\r\n"),asduPbl.ldName);
	m_TestData += str;
	str.Format(_T("phsArtg:%d\r\n"),asduPbl.phsArtg);
	m_TestData += str;
	str.Format(_T("neutArtg:%d\r\n"),asduPbl.neutArtg);
	m_TestData += str;
	str.Format(_T("phsVrtg:%d\r\n"),asduPbl.phsVrtg);
	m_TestData += str;
	str.Format(_T("delayTime:%d\r\n"),asduPbl.delayTime);
	m_TestData += str;
	str.Format(_T("statusWord1:%04x\r\n"),asduPbl.statusWord1);
	m_TestData += str;
	str.Format(_T("statusWord2:%04x\r\n"),asduPbl.statusWord2);
	m_TestData += str;
	str.Format(_T("smpCnt:%d\r\n"),asduPbl.smpCnt);
	m_TestData += str;
	str.Format(_T("smpRate:%d\r\n"),asduPbl.smpRate);
	m_TestData += str;
	str.Format(_T("confRev:%d\r\n"),asduPbl.confRev);
	m_TestData += str;
	str.Format(_T("reserved:%04x\r\n\r\n"),asduPbl.reserved);
	m_TestData += str;
}

void CWinCETestDlg::DisplayNanRuiAsduPblOff(NANRUI_FT3_ASDU_PUBLIC_OFFSET &offset)
{
	CString str;
	str.Format(_T("asduLenOff:%d\r\n"),offset.asduLenOff);
	m_TestData += str;
	str.Format(_T("lnNameOff:%d\r\n"),offset.lnNameOff);
	m_TestData += str;
	str.Format(_T("dataSetNameOff:%d\r\n"),offset.dataSetNameOff);
	m_TestData += str;
	str.Format(_T("ldNameOff:%d\r\n"),offset.ldNameOff);
	m_TestData += str;
	str.Format(_T("phsArtgOff:%d\r\n"),offset.phsArtgOff);
	m_TestData += str;
	str.Format(_T("neutArtgOff:%d\r\n"),offset.neutArtgOff);
	m_TestData += str;
	str.Format(_T("phsVrtgOff:%d\r\n"),offset.phsVrtgOff);
	m_TestData += str;
	str.Format(_T("delayTimeOff:%d\r\n"),offset.delayTimeOff);
	m_TestData += str;
	str.Format(_T("statusWord1Off:%d\r\n"),offset.statusWord1Off);
	m_TestData += str;
	str.Format(_T("statusWord2Off:%d\r\n"),offset.statusWord2Off);
	m_TestData += str;
	str.Format(_T("smpCntOff:%d\r\n"),offset.smpCntOff);
	m_TestData += str;
	str.Format(_T("smpRateOff:%d\r\n"),offset.smpRateOff);
	m_TestData += str;
	str.Format(_T("confRevOff:%d\r\n"),offset.confRevOff);
	m_TestData += str;
	str.Format(_T("reservedOff:%d\r\n\r\n"),offset.reservedOff);
	m_TestData += str;
}

void CWinCETestDlg::DisplayGuoWangAsduPbl(GUOWANG_FT3_ASDU_PUBLIC &asduPbl)
{
	CString str;
	str.Format(_T("asduLen:%d\r\n"),asduPbl.asduLen);
	m_TestData += str;
	str.Format(_T("lnName:%02x\r\n"),asduPbl.lnName);
	m_TestData += str;
	str.Format(_T("dataSetName:%02x\r\n"),asduPbl.dataSetName);
	m_TestData += str;
	str.Format(_T("ldName:%04x\r\n"),asduPbl.ldName);
	m_TestData += str;
	str.Format(_T("phsArtg:%d\r\n"),asduPbl.phsArtg);
	m_TestData += str;
	str.Format(_T("neutArtg:%d\r\n"),asduPbl.neutArtg);
	m_TestData += str;
	str.Format(_T("phsVrtg:%d\r\n"),asduPbl.phsVrtg);
	m_TestData += str;
	str.Format(_T("delayTime:%d\r\n"),asduPbl.delayTime);
	m_TestData += str;
	str.Format(_T("statusWord1:%04x\r\n"),asduPbl.statusWord1);
	m_TestData += str;
	str.Format(_T("statusWord2:%04x\r\n"),asduPbl.statusWord2);
	m_TestData += str;
	str.Format(_T("smpCnt:%d\r\n"),asduPbl.smpCnt);
	m_TestData += str;
}

void CWinCETestDlg::DisplayGuoWangAsduPblOff(GUOWANG_FT3_ASDU_PUBLIC_OFFSET &offset)
{
	CString str;
	str.Format(_T("asduLenOff:%d\r\n"),offset.asduLenOff);
	m_TestData += str;
	str.Format(_T("lnNameOff:%d\r\n"),offset.lnNameOff);
	m_TestData += str;
	str.Format(_T("dataSetNameOff:%d\r\n"),offset.dataSetNameOff);
	m_TestData += str;
	str.Format(_T("ldNameOff:%d\r\n"),offset.ldNameOff);
	m_TestData += str;
	str.Format(_T("phsArtgOff:%d\r\n"),offset.phsArtgOff);
	m_TestData += str;
	str.Format(_T("neutArtgOff:%d\r\n"),offset.neutArtgOff);
	m_TestData += str;
	str.Format(_T("phsVrtgOff:%d\r\n"),offset.phsVrtgOff);
	m_TestData += str;
	str.Format(_T("delayTimeOff:%d\r\n"),offset.delayTimeOff);
	m_TestData += str;
	str.Format(_T("statusWord1Off:%d\r\n"),offset.statusWord1Off);
	m_TestData += str;
	str.Format(_T("statusWord2Off:%d\r\n"),offset.statusWord2Off);
	m_TestData += str;
	str.Format(_T("smpCntOff:%d\r\n"),offset.smpCntOff);
	m_TestData += str;
}

void CWinCETestDlg::DisplayStd52AsduPbl(STD52_FT3_ASDU_PUBLIC &asduPbl)
{
	CString str;
	str.Format(_T("asduLen:%d\r\n"),asduPbl.asduLen);
	m_TestData += str;
	str.Format(_T("lnName:%02x\r\n"),asduPbl.lnName);
	m_TestData += str;
	str.Format(_T("dataSetName:%02x\r\n"),asduPbl.dataSetName);
	m_TestData += str;
	str.Format(_T("ldName:%04x\r\n"),asduPbl.ldName);
	m_TestData += str;
	str.Format(_T("phsArtg:%d\r\n"),asduPbl.phsArtg);
	m_TestData += str;
	str.Format(_T("neutArtg:%d\r\n"),asduPbl.neutArtg);
	m_TestData += str;
	str.Format(_T("phsVrtg:%d\r\n"),asduPbl.phsVrtg);
	m_TestData += str;
	str.Format(_T("delayTime:%d\r\n"),asduPbl.delayTime);
	m_TestData += str;
	str.Format(_T("statusWord1:%04x\r\n"),asduPbl.statusWord1);
	m_TestData += str;
	str.Format(_T("statusWord2:%04x\r\n"),asduPbl.statusWord2);
	m_TestData += str;
	str.Format(_T("smpCnt:%d\r\n"),asduPbl.smpCnt);
	m_TestData += str;
}

void CWinCETestDlg::DisplayStd52AsduPblOff(STD52_FT3_ASDU_PUBLIC_OFFSET &offset)
{
	CString str;
	str.Format(_T("asduLenOff:%d\r\n"),offset.asduLenOff);
	m_TestData += str;
	str.Format(_T("lnNameOff:%d\r\n"),offset.lnNameOff);
	m_TestData += str;
	str.Format(_T("dataSetNameOff:%d\r\n"),offset.dataSetNameOff);
	m_TestData += str;
	str.Format(_T("ldNameOff:%d\r\n"),offset.ldNameOff);
	m_TestData += str;
	str.Format(_T("phsArtgOff:%d\r\n"),offset.phsArtgOff);
	m_TestData += str;
	str.Format(_T("neutArtgOff:%d\r\n"),offset.neutArtgOff);
	m_TestData += str;
	str.Format(_T("phsVrtgOff:%d\r\n"),offset.phsVrtgOff);
	m_TestData += str;
	str.Format(_T("delayTimeOff:%d\r\n"),offset.delayTimeOff);
	m_TestData += str;
	str.Format(_T("statusWord1Off:%d\r\n"),offset.statusWord1Off);
	m_TestData += str;
	str.Format(_T("statusWord2Off:%d\r\n"),offset.statusWord2Off);
	m_TestData += str;
	str.Format(_T("smpCntOff:%d\r\n"),offset.smpCntOff);
	m_TestData += str;
}

void CWinCETestDlg::DisplayCTorVTAsduPbl(CTORVT_FT3_ASDU_PUBLIC &asduPbl)
{
	CString str;
	str.Format(_T("msg Type:%d\r\n"),asduPbl.msgType);
	m_TestData += str;
	str.Format(_T("temperature:%d\r\n"),asduPbl.temperature);
	m_TestData += str;
	str.Format(_T("delayTime:%d\r\n"),asduPbl.delayTime);
	m_TestData += str;
	str.Format(_T("statusWord1:%04x\r\n"),asduPbl.statusWord1);
	m_TestData += str;
	str.Format(_T("statusWord2:%04x\r\n"),asduPbl.statusWord2);
	m_TestData += str;
	str.Format(_T("smpCnt:%d\r\n"),asduPbl.smpCnt);
	m_TestData += str;
}

void CWinCETestDlg::DisplayCTorVTAsduPblOff(CTORVT_FT3_ASDU_PUBLIC_OFFSET &offset)
{
	CString str;
	str.Format(_T("msgTypeOff:%d\r\n"),offset.msgTypeOff);
	m_TestData += str;
	str.Format(_T("temperatureOff:%d\r\n"),offset.temperatureOff);
	m_TestData += str;
	str.Format(_T("delayTimeOff:%d\r\n"),offset.delayTimeOff);
	m_TestData += str;
	str.Format(_T("statusWord1Off:%d\r\n"),offset.statusWord1Off);
	m_TestData += str;
	str.Format(_T("statusWord2Off:%d\r\n"),offset.statusWord2Off);
	m_TestData += str;
	str.Format(_T("smpCntOff:%d\r\n"),offset.smpCntOff);
	m_TestData += str;
}

void CWinCETestDlg::DisplayCodeData(uint16_t *inData, size_t datLen)
{
	CString str;
	str.Format(_T("Code Data:\r\n"));
	m_TestData += str;
	for ( size_t i=0; i<datLen; i++)
	{
		str.Format(_T("%04x  "),inData[i]);
		m_TestData += str;
	}
	str.Format(_T("\r\n\r\n"));
	m_TestData += str;
}

void CWinCETestDlg::DisplayGooseData(char *str)
{
	m_TestData += str;
	m_TestData += _T("\r\n");
}

int CWinCETestDlg::ChannelDataToStr(const GOOSE_DATA *datVal, char *outStr, const size_t strSize)
{
	int offset = 0;
	if ( datVal == NULL )
	{
		return offset;
	}
	switch (datVal->type)
	{
	case GSEDT_BOOLEAN:
		{
			GOOSE_BOOLEAN *tmp = (GOOSE_BOOLEAN *)datVal;
			if ( tmp->boolVal )
				offset = sprintf_s(outStr,strSize,"boolean:True\r\n");
			else
				offset = sprintf_s(outStr,strSize,"boolean:False\r\n");
		}
		break;
	case GSEDT_BIT_STR:
		{
			offset = sprintf_s(outStr,strSize,"Bit String:");
			GOOSE_BIT_STRING *tmp = (GOOSE_BIT_STRING *)datVal;
			offset += sprintf_s(outStr+offset,strSize-offset,"Padding:%d  ",tmp->padding);
			for ( size_t i=0; i < tmp->strSize; i++ )
			{
				offset += sprintf_s(outStr+offset,strSize-offset,"%02x",tmp->bitStr[i]);
			}
			offset += sprintf_s(outStr+offset,strSize-offset,"\r\n");
		}
		break;
	case GSEDT_INT8:
		{
			GOOSE_INT8 *tmp = (GOOSE_INT8 *)datVal;
			offset = sprintf_s(outStr,strSize,"Int8:%d\r\n",tmp->datVal);
		}
		break;
	case GSEDT_INT16:
		{
			GOOSE_INT16 *tmp = (GOOSE_INT16 *)datVal;
			offset = sprintf_s(outStr,strSize,"Int16:%d\r\n",tmp->datVal);
		}
		break;
	case GSEDT_INT24:
		{
			GOOSE_INT24 *tmp = (GOOSE_INT24 *)datVal;
			offset = sprintf_s(outStr,strSize,"Int24:%d\r\n",tmp->datVal);
		}
		break;
	case GSEDT_INT32:
		{
			GOOSE_INT32 *tmp = (GOOSE_INT32 *)datVal;
			offset = sprintf_s(outStr,strSize,"Int32:%d\r\n",tmp->datVal);
		}
		break;
	case GSEDT_UINT8:
		{
			GOOSE_UINT8 *tmp = (GOOSE_UINT8 *)datVal;
			offset = sprintf_s(outStr,strSize,"Uint8:%u\r\n",tmp->datVal);
		}
		break;
	case GSEDT_UINT16:
		{
			GOOSE_UINT16 *tmp = (GOOSE_UINT16 *)datVal;
			offset = sprintf_s(outStr,strSize,"Uint16:%u\r\n",tmp->datVal);
		}
		break;
	case GSEDT_UINT24:
		{
			GOOSE_UINT24 *tmp = (GOOSE_UINT24 *)datVal;
			offset = sprintf_s(outStr,strSize,"Uint24:%u\r\n",tmp->datVal);
		}
		break;
	case GSEDT_UINT32:
		{
			GOOSE_UINT32 *tmp = (GOOSE_UINT32 *)datVal;
			offset = sprintf_s(outStr,strSize,"Uint32:%u\r\n",tmp->datVal);
		}
		break;
	case GSEDT_FLOAT:
		{
			GOOSE_FLOAT *tmp = (GOOSE_FLOAT *)datVal;
			offset = sprintf_s(outStr,strSize,"Float:%06f\r\n",tmp->datVal);
		}
		break;
	case GSEDT_OCTET_STR:
		{
			GOOSE_OCTET_STRING *tmp = (GOOSE_OCTET_STRING *)datVal;
			offset = sprintf_s(outStr,strSize,"Octet String:");
			for ( size_t i = 0; i < tmp->strSize; i++ )
			{
				offset += sprintf_s(outStr+offset,strSize-offset,"%x",tmp->octetStr[i]);
			}
			offset += sprintf_s(outStr+offset,strSize-offset,"\r\n");
		}
		break;
	case GSEDT_VISIBLE_STR:
		{
			GOOSE_VISIBLE_STRING *tmp = (GOOSE_VISIBLE_STRING *)datVal;
			offset = sprintf_s(outStr,strSize,"Visible String:");
			for ( size_t i = 0; i < tmp->strSize; i++ )
			{
				offset += sprintf_s(outStr+offset,strSize-offset,"%c",tmp->visibleStr[i]);
			}
			offset += sprintf_s(outStr+offset,strSize-offset,"\r\n");
		}
		break;
	case GSEDT_UTC_TIME:
		{
			GOOSE_UTC_TIME *tmp = (GOOSE_UTC_TIME *)datVal;
			CTime t((time_t)tmp->t_s);
			offset = sprintf_s(outStr,strSize,"UTC Time:");
			offset += sprintf_s(outStr+offset,strSize-offset,"%04d-%02d-%02d %02d:%02d:%02d.%09u  ",
				t.GetYear(),t.GetMonth(),t.GetDay(),t.GetHour(),t.GetMinute(),t.GetSecond(),tmp->t_ns);
			offset += sprintf_s(outStr+offset,strSize-offset,"Time Quality:%02x",tmp->t_q);
			offset += sprintf_s(outStr+offset,strSize-offset,"\r\n");
		}
		break;
	case GSEDT_STRUCT:
		{
			GOOSE_STRUCT *tmp = (GOOSE_STRUCT *)datVal;
			std::vector<GOOSE_DATA *>::iterator it;
			offset = sprintf_s(outStr,strSize,"Struct:\r\n");
			for ( it = tmp->member.begin(); it != tmp->member.end(); it++ )
			{
				offset += ChannelDataToStr((*it),&outStr[offset],strSize-offset);
			}
			offset += sprintf_s(outStr+offset,strSize-offset,"\r\n");
		}
		break;
	default:
		offset = sprintf_s(outStr,strSize,"");
	}
	return offset;
}

void CWinCETestDlg::DisplayStatusWord(Ft3FrameStatusWord &statusWord)
{
	CString str;

	str.Format(_T("IsMaintenanceRequired:%d\r\n"),statusWord.IsMaintenanceRequired());
	m_TestData += str;

	str.Format(_T("IsTestMode:%d\r\n"),statusWord.IsTestMode());
	m_TestData += str;

	str.Format(_T("IsMuWakingUp:%d\r\n"),statusWord.IsMuWakingUp());
	m_TestData += str;

	str.Format(_T("MuSyncMethod:%d\r\n"),statusWord.GetMuSyncMethod());
	m_TestData += str;

	str.Format(_T("IsMuNotSync:%d\r\n"),statusWord.IsMuNotSync());
	m_TestData += str;

	str.Format(_T("IsCtorVtFiring:%d\r\n"),statusWord.IsCtorVtFiring());
	m_TestData += str;

	str.Format(_T("CtorVtSyncMethod:%d\r\n"),statusWord.GetCtorVtSyncMethod());
	m_TestData += str;

	str.Format(_T("IsCtorVtNotSync:%d\r\n"),statusWord.IsCtorVtNotSync());
	m_TestData += str;

	str.Format(_T("CtOutputType:%d\r\n"),statusWord.GetCtOutputType());
	m_TestData += str;

	str.Format(_T("RangeFlag:%d\r\n"),statusWord.GetRangeFlag());
	m_TestData += str;

	str.Format(_T("IsCtorVtAdError:%d\r\n"),statusWord.IsCtorVtAdError());
	m_TestData += str;

	str.Format(_T("IsCtorVtPower1stAbnor:%d\r\n"),statusWord.IsCtorVtPower1stAbnor());
	m_TestData += str;

	str.Format(_T("IsCtorVtPower2ndAbnor:%d\r\n"),statusWord.IsCtorVtPower2ndAbnor());
	m_TestData += str;

	str.Format(_T("IsCtorVtHvpsInvalid:%d\r\n"),statusWord.IsCtorVtHvpsInvalid());
	m_TestData += str;

	FT3_CTORVT_LASER_POWER_STATUS eStatus = statusWord.GetCtorVtLaserPowerStatus();
	if ( eStatus == FT3CLPS_MAINTAIN_OUTPUT )
		str.Format(_T("CtorVtLaserPowerStatus:Maintain Output\r\n"));
	else if ( eStatus == FT3CLPS_DOWN_TO_MAINTAIN_POWER )
		str.Format(_T("CtorVtLaserPowerStatus:Down To Maintain Power\r\n"));
	else if ( eStatus == FT3CLPS_DECREASE_OUTPUT_POWER )
		str.Format(_T("CtorVtLaserPowerStatus:Decrease Output Power\r\n"));
	else if ( eStatus == FT3CLPS_INCREASE_OUTPUT_POWER )
		str.Format(_T("CtorVtLaserPowerStatus:Increase Output Power\r\n"));
	else if ( eStatus == FT3CLPS_SPEED_INCREASE_OUTPUT_POWER )
		str.Format(_T("CtorVtLaserPowerStatus:Speed Increase Output Power\r\n"));
	else if ( eStatus == FT3CLPS_FAILURE )
		str.Format(_T("CtorVtLaserPowerStatus:Failure\r\n"));
	else
		str.Format(_T("CtorVtLaserPowerStatus:Unknown\r\n"));
	m_TestData += str;

	for ( int i = 0; i < 22; i++ )
	{
		str.Format(_T("IsChannel%dInvalid:%d\r\n"),i,statusWord.IsChannelInvalid(i));
		m_TestData += str;
	}

	for ( int i = 0; i < 13; i++ )
	{
		str.Format(_T("FutureUsedBitVal%d:%d\r\n"),i,statusWord.GetFutureUsedBitVal(i));
		m_TestData += str;
	}
}

void CWinCETestDlg::DisplayTimeQuality(Iec61850TimeQuality &tq)
{
	CString str;

	str.Format(_T("IsLeapSecondKnown:%d\r\n"),tq.IsLeapSecondKnown());
	m_TestData += str;

	str.Format(_T("IsClockFailure:%d\r\n"),tq.IsClockFailure());
	m_TestData += str;

	str.Format(_T("IsClockNotSync:%d\r\n"),tq.IsClockNotSync());
	m_TestData += str;

	str.Format(_T("SecondAccuracy:%d\r\n"),tq.GetSecondAccuracy());
	m_TestData += str;
}

void CWinCETestDlg::DisplaySmvQuality(Iec61850SmvQuality &sq)
{
	CString str;

	IEC61850_QUALITY_VALIDITY_TYPE eValidity = sq.GetValidity();
	if ( eValidity == IEC61850_QV_GOOD )
		str.Format(_T("Validity:Good\r\n"));
	else if ( eValidity == IEC61850_QV_INVALID )
		str.Format(_T("Validity:Invalid\r\n"));
	else if ( eValidity == IEC61850_QV_RESERVED )
		str.Format(_T("Validity:Reserved\r\n"));
	else
		str.Format(_T("Validity:Questional\r\n"));
	m_TestData += str;

	str.Format(_T("IsOverflow:%d\r\n"),sq.IsOverflow());
	m_TestData += str;

	str.Format(_T("IsOutofRange:%d\r\n"),sq.IsOutofRange());
	m_TestData += str;

	str.Format(_T("IsBadReference:%d\r\n"),sq.IsBadReference());
	m_TestData += str;

	str.Format(_T("IsOscillatory:%d\r\n"),sq.IsOscillatory());
	m_TestData += str;

	str.Format(_T("IsFailure:%d\r\n"),sq.IsFailure());
	m_TestData += str;

	str.Format(_T("IsOldData:%d\r\n"),sq.IsOldData());
	m_TestData += str;

	str.Format(_T("IsInconsistent:%d\r\n"),sq.IsInconsistent());
	m_TestData += str;

	str.Format(_T("IsInaccurate:%d\r\n"),sq.IsInaccurate());
	m_TestData += str;

	IEC61850_QUALITY_SOURCE_TYPE eSourceType = sq.GetSourceType();
	if ( eSourceType == IEC61850_QST_PROCESS )
		str.Format(_T("SourceType:Process\r\n"));
	else
		str.Format(_T("SourceType:Substituted\r\n"));
	m_TestData += str;

	str.Format(_T("IsTest:%d\r\n"),sq.IsTest());
	m_TestData += str;

	str.Format(_T("IsOperatorBlocked:%d\r\n"),sq.IsOperatorBlocked());
	m_TestData += str;

	str.Format(_T("IsDerived:%d\r\n"),sq.IsDerived());
	m_TestData += str;
}

void CWinCETestDlg::DisplayGooseQuality(Iec61850GooseQuality &gq)
{
	CString str;

	IEC61850_QUALITY_VALIDITY_TYPE eValidity = gq.GetValidity();
	if ( eValidity == IEC61850_QV_GOOD )
		str.Format(_T("Validity:Good\r\n"));
	else if ( eValidity == IEC61850_QV_INVALID )
		str.Format(_T("Validity:Invalid\r\n"));
	else if ( eValidity == IEC61850_QV_RESERVED )
		str.Format(_T("Validity:Reserved\r\n"));
	else
		str.Format(_T("Validity:Questional\r\n"));
	m_TestData += str;

	str.Format(_T("IsOverflow:%d\r\n"),gq.IsOverflow());
	m_TestData += str;

	str.Format(_T("IsOutofRange:%d\r\n"),gq.IsOutofRange());
	m_TestData += str;

	str.Format(_T("IsBadReference:%d\r\n"),gq.IsBadReference());
	m_TestData += str;

	str.Format(_T("IsOscillatory:%d\r\n"),gq.IsOscillatory());
	m_TestData += str;

	str.Format(_T("IsFailure:%d\r\n"),gq.IsFailure());
	m_TestData += str;

	str.Format(_T("IsOldData:%d\r\n"),gq.IsOldData());
	m_TestData += str;

	str.Format(_T("IsInconsistent:%d\r\n"),gq.IsInconsistent());
	m_TestData += str;

	str.Format(_T("IsInaccurate:%d\r\n"),gq.IsInaccurate());
	m_TestData += str;

	IEC61850_QUALITY_SOURCE_TYPE eSourceType = gq.GetSourceType();
	if ( eSourceType == IEC61850_QST_PROCESS )
		str.Format(_T("SourceType:Process\r\n"));
	else
		str.Format(_T("SourceType:Substituted\r\n"));
	m_TestData += str;

	str.Format(_T("IsTest:%d\r\n"),gq.IsTest());
	m_TestData += str;

	str.Format(_T("IsOperatorBlocked:%d\r\n"),gq.IsOperatorBlocked());
	m_TestData += str;
}

void CWinCETestDlg::DisplayIeee1588Header(IEEE1588_HEADER &hdr)
{
	CString str;

	str.Format(_T("transportSpecific:0x%1x\r\n"),hdr.type.transportSpecific);
	m_TestData += str;

	IEEE1588_MESSAGE_TYPE eMsgType = hdr.type.GetMessageType();
	switch ( eMsgType )
	{
	case IEEE1588_TYPE_SYNC:
		str.Format(_T("messageType:SYNC(0x%1x)\r\n"),hdr.type.messageType);
		break;
	case IEEE1588_TYPE_DELAY_REQUEST:
		str.Format(_T("messageType:DELAY_REQUEST(0x%1x)\r\n"),hdr.type.messageType);
		break;
	case IEEE1588_TYPE_PDELAY_REQUEST:
		str.Format(_T("messageType:PDELAY_REQUEST(0x%1x)\r\n"),hdr.type.messageType);
		break;
	case IEEE1588_TYPE_PDELAY_RESPONSE:
		str.Format(_T("messageType:PDELAY_RESPONSE(0x%1x)\r\n"),hdr.type.messageType);
		break;
	case IEEE1588_TYPE_FOLLOW_UP:
		str.Format(_T("messageType:FOLLOW_UP(0x%1x)\r\n"),hdr.type.messageType);
		break;
	case IEEE1588_TYPE_DELAY_RESPONSE:
		str.Format(_T("messageType:DELAY_RESPONSE(0x%1x)\r\n"),hdr.type.messageType);
		break;
	case IEEE1588_TYPE_PDELAY_RESPONSE_FOLLOW_UP:
		str.Format(_T("messageType:PDELAY_RESPONSE_FOLLOW_UP(0x%1x)\r\n"),hdr.type.messageType);
		break;
	case IEEE1588_TYPE_ANNOUNCE:
		str.Format(_T("messageType:ANNOUNCE(0x%1x)\r\n"),hdr.type.messageType);
		break;
	case IEEE1588_TYPE_SIGNALING:
		str.Format(_T("messageType:SIGNALING(0x%1x)\r\n"),hdr.type.messageType);
		break;
	case IEEE1588_TYPE_MANAGEMENT:
		str.Format(_T("messageType:MANAGEMENT(0x%1x)\r\n"),hdr.type.messageType);
		break;
	default:
		str.Format(_T("messageType:Reserved(0x%1x)\r\n"),hdr.type.messageType);
	}
	m_TestData += str;

	str.Format(_T("versionPTP:0x%1x\r\n"),hdr.version.versionPTP);
	m_TestData += str;

	str.Format(_T("messageLength:%d\r\n"),hdr.messageLength);
	m_TestData += str;

	str.Format(_T("domainNumber:%d\r\n"),hdr.domainNumber);
	m_TestData += str;

	str.Format(_T("flags:%d\r\n"),hdr.flags);
	m_TestData += str;

	str.Format(_T("correctionField:%06f\r\n"),(double)((int64_t)hdr.correctionField)/(1<<16));
	m_TestData += str;

	str.Format(_T("clockIdentity:0x%08x"),hdr.sourcePortIdentity.clockIdentity>>32);
	m_TestData += str;
	str.Format(_T(" %08x\r\n"),hdr.sourcePortIdentity.clockIdentity);
	m_TestData += str;

	str.Format(_T("sourcePortID:%d\r\n"),hdr.sourcePortIdentity.sourcePortID);
	m_TestData += str;

	str.Format(_T("sequenceId:%d\r\n"),hdr.sequenceId);
	m_TestData += str;

	str.Format(_T("controlField:%d\r\n"),hdr.controlField);
	m_TestData += str;

	str.Format(_T("logMessageInterval:%d\r\n"),hdr.logMessageInterval);
	m_TestData += str;
}

void CWinCETestDlg::DisplayIeee1588HeaderFlags(IEEE1588_HEADER_FLAGS &flags)
{
	CString str;

	str.Format(_T("IsPtpSecurity:%d\r\n"),flags.IsPtpSecurity());
	m_TestData += str;

	str.Format(_T("IsPtpProfileSpecific2:%d\r\n"),flags.IsPtpProfileSpecific2());
	m_TestData += str;

	str.Format(_T("IsPtpProfileSpecific1:%d\r\n"),flags.IsPtpProfileSpecific1());
	m_TestData += str;

	str.Format(_T("IsPtpUnicast:%d\r\n"),flags.IsPtpUnicast());
	m_TestData += str;

	str.Format(_T("IsPtpTwoStep:%d\r\n"),flags.IsPtpTwoStep());
	m_TestData += str;

	str.Format(_T("IsPtpAlternateMaster:%d\r\n"),flags.IsPtpAlternateMaster());
	m_TestData += str;

	str.Format(_T("IsFrequencyTraceable:%d\r\n"),flags.IsFrequencyTraceable());
	m_TestData += str;

	str.Format(_T("IsTimeTraceble:%d\r\n"),flags.IsTimeTraceble());
	m_TestData += str;

	str.Format(_T("IsPtpTimescale:%d\r\n"),flags.IsPtpTimescale());
	m_TestData += str;

	str.Format(_T("IsPtpUtcReasonable:%d\r\n"),flags.IsPtpUtcReasonable());
	m_TestData += str;

	str.Format(_T("IsPtpLI59:%d\r\n"),flags.IsPtpLI59());
	m_TestData += str;

	str.Format(_T("IsPtpLI61:%d\r\n"),flags.IsPtpLI61());
	m_TestData += str;
}

void CWinCETestDlg::OnBnClickedSav91Code()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	uint8_t   desMac[] = {0x01,0x0C,0xCD,0x04,0x09,0x06};
	uint8_t   srcMac[] = {0x00,0x30,0x18,0xA1,0x5B,0xED};
	uint8_t   usrPrio = 4;
	uint16_t  vlanID  = 0;
	uint16_t  appID   = 0x0906;
	SAV91_ASDU_PUBLIC asduPbl = {
		0x002C,0x02,0x01,0x0000,5,5,100,100,
		0x0057,0x004D,13361,80,0x00
	};
	uint16_t smpVal[12] = {1,2,3,4,5,6,7,8,9,10,11,12};
	uint8_t  msgData[ETHER_FRAME_MAX_LEN];

	m_TestData = _T("");

	// 设值
	dwStartTime = GetTickCount();
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		m_pSav91Msg->ClearSavPDU();
		m_pSav91Msg->SetMacAddr(desMac,srcMac,ETHER_MAC_LEN);
		m_pSav91Msg->Set8021QVlanInfo(usrPrio,vlanID);
		m_pSav91Msg->SetAppID(appID);
		for ( int i=0; i<2; i++ )
		{
			if ( m_pSav91Msg->AddAnASDU(asduPbl,smpVal,12) == false )
			{
				str.Format(_T("Add ASDU%d fail\r\n"),i+1);
				m_TestData += str;
				break;
			}
		}
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;


	// 编码
	dwStartTime = GetTickCount();
	size_t cSize;
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		cSize = m_pSav91Msg->Code(msgData,sizeof(msgData));
	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayCodeData(msgData,cSize);
	str.Format(_T("Code Size:%d\r\n\r\n"),cSize);
	m_TestData += str;
	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedSav91Dec()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	ETHER_II_HEADER ethHdr;
	ETHER_II_HEADER_OFFSET ethOff;
	IEEE802_1Q_VLAN_INFO vlanInfo;
	IEEE802_1Q_VLAN_INFO_OFFSET vlanOff;
	IEC8802_3_PDU_PUBLIC iec8802Info;
	IEC8802_3_PDU_PUBLIC_OFFSET iec8802Off;
	const uint16_t maxNoASDU = 4;
	SAV91_ASDU_PUBLIC asduPbl[maxNoASDU];
	SAV91_ASDU_PUBLIC_OFFSET asduPblOff[maxNoASDU];
	uint16_t noASDU;
	size_t smpsNum;
	uint16_t smpVal[maxNoASDU][12];
	size_t   smpValOff[maxNoASDU][12];

	m_TestData = _T("");

	// 解码
	dwStartTime = GetTickCount();
	size_t dSize;
	for ( int i = 0; i<MSG_CODEC_COUNT; i++ )
	{
		dSize= m_pSav91Msg->Decode(k_Sav91,/*sizeof(k_Sav91)*/121212121);
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;

	// 取值
	str.Format(_T("Src Size: %d\r\nDecode Size: %d\r\n\r\n"),sizeof(k_Sav91),dSize);
	m_TestData += str;
	dwStartTime = GetTickCount();
	if ( dSize == 0 )
	{
		m_TestData.Format(_T("Decode Fail!"));
	}
	else
	{
		for ( int i = 0; i<MSG_CODEC_COUNT; i++ )
		{
			m_pSav91Msg->GetEthernetHeader(ethHdr);
			m_pSav91Msg->Get8021QVlanInfo(vlanInfo);
			m_pSav91Msg->GetIEC8802Info(iec8802Info);

			noASDU = m_pSav91Msg->GetNoASDU();

			// 			str.Format(_T("SavPDU Len:%04x\r\n"),m_Sav91Msg->GetSavPduLen());
			// 			m_TestData += str;
			for ( uint16_t i = 0; i < noASDU ; i++ )
			{
				smpsNum = m_pSav91Msg->GetASDUDefaultSmpsNum();
				m_pSav91Msg->GetASDUPublicInfo(i,asduPbl[i]);
				m_pSav91Msg->GetASDUSmps(i,smpVal[i],smpsNum);
			}

			m_pSav91Msg->GetEthernetHeaderOff(ethOff);
			m_pSav91Msg->Get8021QVlanInfoOff(vlanOff);
			m_pSav91Msg->GetIEC8802InfoOff(iec8802Off);

			// 			str.Format(_T("SavPDU Off:%04x\r\n"),m_Sav91Msg->GetSavPduOff());
			// 			m_TestData += str;
			for ( uint16_t i = 0; i < noASDU ; i++ )
			{
				m_pSav91Msg->GetASDUPublicInfoOff(i,asduPblOff[i]);
				m_pSav91Msg->GetASDUSmpsOff(i,smpValOff[i],smpsNum);
			}
		}
	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayEthHdr(ethHdr);
	DisplayVlanInfo(vlanInfo);
	DisplayIEC88023Pdu(iec8802Info);
	for ( uint16_t i = 0; i < noASDU ; i++ )
	{
		DisplaySav91AsduPbl(asduPbl[i]);
		DisplayUint16Smps(smpVal[i],smpsNum);
	}

	DisplayEthHdrOff(ethOff);
	DisplayVlanInfoOff(vlanOff);
	DisplayIEC88023PduOff(iec8802Off);
	for ( uint16_t i = 0; i < noASDU ; i++ )
	{
		DisplaySav91AsduPblOff(asduPblOff[i]);
		DisplayChannelOff(smpValOff[i],smpsNum);
	}

	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedSav92Code()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	uint8_t   desMac[] = {0x01,0x0C,0xCD,0x04,0x00,0x01};
	uint8_t   srcMac[] = {0x00,0xE0,0x4C,0x08,0x2A,0x22};
	uint8_t   usrPrio = 4;
	uint16_t  vlanID  = 10;
	uint16_t  appID   = 0x4001;
	SAV92_ASDU_PUBLIC asduPbl = {
		"ABCDEFGHIJKL","",2709,1,0,0,0,1,0
	};
	uint32_t smpVal[] = {
		1,2,3,4,5,6,7,8,9,10,-1,-2,-3,-4,-5,-6,-7,-8,-9,-10,1,2
	};
	uint32_t smpQlt[] = {
		1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10,1,2
	};
	uint8_t  msgData[ETHER_FRAME_MAX_LEN];

	m_TestData = _T("");

	// 设值
	dwStartTime = GetTickCount();
	for ( int j = 0; j < MSG_CODEC_COUNT; j++ )
	{
		m_pSav92Msg->ClearSavPDU();
		m_pSav92Msg->SetMacAddr(desMac,srcMac,ETHER_MAC_LEN);
		m_pSav92Msg->Set8021QVlanInfo(usrPrio,vlanID);
		m_pSav92Msg->SetAppID(appID);
		for ( int i=0; i<2; i++ )
		{
			if ( m_pSav92Msg->AddAnASDU(asduPbl,smpVal,smpQlt,sizeof(smpVal)/sizeof(uint32_t)) == false )
			{
				str.Format(_T("Add ASDU%d fail\r\n"),i+1);
				m_TestData += str;
				break;
			}
		}
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;

	// 编码
	size_t cSize = 0;
	dwStartTime = GetTickCount();
	for ( int j = 0; j < MSG_CODEC_COUNT; j++ )
	{
		cSize = m_pSav92Msg->Code(msgData,sizeof(msgData));
	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayCodeData(msgData,cSize);
	str.Format(_T("Code Size:%d\r\n\r\n"),cSize);
	m_TestData += str;
	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedSav92Dec()
{
	Sav92DecTest();
// 	SetTimer(1,DECODE_TIMER,NULL);
// 	KillTimer(2);
}

void CWinCETestDlg::Sav92DecTest()
{
	// TODO: 在此添加控件通知处理程序代码
#ifdef CHSINT_SDK
	//	std::wstring fileName(_T("\\Storage Card\\数字化变电站分析仪资料\\其他资料\\pcap\\报文\\sav92.pcap"));
	std::wstring fileName(_T("\\Storage Card\\数字化变电站分析仪资料\\其他资料\\pcap\\报文\\Filter92.pcap"));
#else
	std::wstring fileName(_T("\\FlashDisk\\Filter92_MUL_1ASDU_32.pcap"));
//	std::wstring fileName(_T("\\FlashDisk2\\Filter92_MUL_8ASDU.pcap"));
#endif
	PCAP_PACKET_HEADER pckHeader;
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	uint8_t *pckData = NULL;
	m_TestData = _T("");

	const int maxNoASDU = 8;
	ETHER_II_HEADER ethHdr;
	ETHER_II_HEADER_OFFSET ethOff;
	IEEE802_1Q_VLAN_INFO vlanInfo;
	IEEE802_1Q_VLAN_INFO_OFFSET vlanOff;
	IEC8802_3_PDU_PUBLIC iec8802Info;
	IEC8802_3_PDU_PUBLIC_OFFSET iec8802Off;
	SAV92_ASDU_PUBLIC asduPbl[maxNoASDU];
	SAV92_ASDU_PUBLIC_OFFSET asduPblOff[maxNoASDU];
	uint16_t noASDU;
	uint32_t smpVal[maxNoASDU][30];
	uint32_t smpQlt[maxNoASDU][30];
	size_t smpsNum;
	uint32_t smpValOff[maxNoASDU][30];
	uint32_t smpQltOff[maxNoASDU][30];
	size_t   seqDataOff[maxNoASDU];

	if ( m_PcapFile->Open(fileName,PFM_READ) == PCAP_FILE_ERROR )
	{
		str.Format(_T("Open Write Error!\r\n\r\n"));
		m_TestData += str;
	}

	int pckArrSize = 0;
	static int pckOff = 0;
	pckOff = (++pckOff)%4;
	for ( int i = 0; i < 4; i++ )
	{
		if ( m_PcapFile->SeekPckToRead(pckOff+i) == PCAP_FILE_ERROR )
		{
			str.Format(_T("Seek Pck Error!\r\n\r\n"));
			m_TestData += str;
		}
		if ( m_PcapFile->ReadOnePckHeader(pckHeader) == PCAP_FILE_ERROR )
		{
			str.Format(_T("ReadOnePckHeader ERROR!!\r\n\r\n"));
			m_TestData += str;
		}
		if ( pckData == NULL )
		{
			pckArrSize = (pckHeader.capLen+1)/2*2;
			pckData = new uint8_t[pckArrSize*4];
		}
		if ( m_PcapFile->ReadOnePckData(&pckData[i*pckArrSize],pckHeader.capLen) == PCAP_FILE_ERROR )
		{
			str.Format(_T("ReadOnePckData ERROR!!\r\n\r\n"));
			m_TestData += str;
		}
	}

	size_t dSize;
	int ctrlNum = 1;
	m_DecTime = 0;
	m_CodeTime = 0;
	dwStartTime = GetTickCount();

	for ( int i = 0; i < MSG_CODEC_COUNT*ctrlNum; i++ )
	{
		// 解码
		int idx = i%ctrlNum;
//		dwStartTime = GetTickCount();
		dSize =  m_pSav92Msg->Decode(&pckData[idx*pckArrSize],pckHeader.capLen);
// 		dwFinishTime = GetTickCount();
// 		m_DecTime += dwFinishTime-dwStartTime;

		// 取值
//		dwStartTime = GetTickCount();
		if ( dSize != 0 )
		{
			m_pSav92Msg->GetEthernetHeader(ethHdr);
			m_pSav92Msg->Get8021QVlanInfo(vlanInfo);
			m_pSav92Msg->GetIEC8802Info(iec8802Info);

			noASDU = m_pSav92Msg->GetNoASDU();
			for ( uint16_t j = 0; j < noASDU ; j++ )
			{
				smpsNum = m_pSav92Msg->GetASDUSmpsNum(j);
				m_pSav92Msg->GetASDUPublicInfo(j,asduPbl[j]);
				m_pSav92Msg->GetASDUSmps(j,smpVal[j],smpQlt[j],smpsNum);
			}

			m_pSav92Msg->GetEthernetHeaderOff(ethOff);
			m_pSav92Msg->Get8021QVlanInfoOff(vlanOff);
			m_pSav92Msg->GetIEC8802InfoOff(iec8802Off);

			for ( uint16_t j = 0; j < noASDU ; j++ )
			{
				smpsNum = m_pSav92Msg->GetASDUSmpsNum(j);
				m_pSav92Msg->GetASDUPublicInfoOff(j,asduPblOff[j]);
				m_pSav92Msg->GetASDUSmpsOff(j,seqDataOff[j],smpValOff[j],smpQltOff[j],smpsNum);
			}
		}
// 		dwFinishTime = GetTickCount();
// 		m_CodeTime += dwFinishTime-dwStartTime;
	}
	dwFinishTime = GetTickCount();
	m_CodeTime   = dwFinishTime-dwStartTime;

	RETAILMSG(1,(_T("Decode Time:%d\r\nGetVal Time:%d\r\n\r\n"),m_DecTime,m_CodeTime));

	str.Format(_T("Src Size: %d\r\nDecode Size: %d\r\n\r\n"),pckHeader.capLen,dSize);
	m_TestData += str;

	// 显示
	dwStartTime = GetTickCount();
	for ( int i = 0; i < 3; i++ )
	{
		dSize =  m_pSav92Msg->Decode(&pckData[i*pckArrSize],pckHeader.capLen-10);

		if ( dSize != 0 )
		{
			m_pSav92Msg->GetEthernetHeader(ethHdr);
			m_pSav92Msg->Get8021QVlanInfo(vlanInfo);
			m_pSav92Msg->GetIEC8802Info(iec8802Info);

			noASDU = m_pSav92Msg->GetNoASDU();
			for ( uint16_t j = 0; j < noASDU ; j++ )
			{
				smpsNum = m_pSav92Msg->GetASDUSmpsNum(j);
				m_pSav92Msg->GetASDUPublicInfo(j,asduPbl[j]);
				m_pSav92Msg->GetASDUSmps(j,smpVal[j],smpQlt[j],smpsNum);
			}

			m_pSav92Msg->GetEthernetHeaderOff(ethOff);
			m_pSav92Msg->Get8021QVlanInfoOff(vlanOff);
			m_pSav92Msg->GetIEC8802InfoOff(iec8802Off);

			for ( uint16_t j = 0; j < noASDU ; j++ )
			{
				smpsNum = m_pSav92Msg->GetASDUSmpsNum(j);
				m_pSav92Msg->GetASDUPublicInfoOff(j,asduPblOff[j]);
				m_pSav92Msg->GetASDUSmpsOff(j,seqDataOff[j],smpValOff[j],smpQltOff[j],smpsNum);
			}

			DisplayEthHdr(ethHdr);
			DisplayVlanInfo(vlanInfo);
			DisplayIEC88023Pdu(iec8802Info);
			for ( uint16_t j = 0; j < noASDU ; j++ )
			{
				DisplaySav92AsduPbl(asduPbl[j]);
				DisplaySav92Smps(smpVal[j],smpQlt[j],smpsNum);
			}

			DisplayEthHdrOff(ethOff);
			DisplayVlanInfoOff(vlanOff);
			DisplayIEC88023PduOff(iec8802Off);
			for ( uint16_t j = 0; j < noASDU ; j++ )
			{
				DisplaySav92AsduPblOff(asduPblOff[j]);
				DisplayChannelOff(smpValOff[j],smpsNum);
			}
		}
	}

	delete[] pckData;
	dwFinishTime = GetTickCount();
	str.Format(_T("DisplayTime: %d\r\n\r\n"),dwFinishTime-dwStartTime);
	m_TestData += str;

	str.Format(_T("Close File: %d\r\n\r\n"),m_PcapFile->Close());
	m_TestData += str;
	
	UpdateData(FALSE);
}

void CWinCETestDlg::Sav92GetChannelDataTest()
{
#ifdef CHSINT_SDK
	//	std::wstring fileName(_T("\\Storage Card\\数字化变电站分析仪资料\\其他资料\\pcap\\报文\\sav92.pcap"));
	std::wstring fileName(_T("\\Storage Card\\数字化变电站分析仪资料\\其他资料\\pcap\\报文\\Filter92.pcap"));
#else
	std::wstring fileName(_T("\\FlashDisk\\Filter92_MUL_1ASDU_32.pcap"));
	//	std::wstring fileName(_T("\\FlashDisk2\\Filter92_MUL_8ASDU.pcap"));
#endif
	CString str;
	m_TestData = _T("");

	// 获取连续报文帧的指定通道值
	const uint32_t k_channelNum = 80;
	uint32_t channelData[k_channelNum];
	GetSav92ChannelData(fileName,0,k_channelNum,0,channelData,k_channelNum);

	// 采样值转换
	CSmvScaler scaler;
	scaler.SetChannelMsgValType(SMV_VALUE_TYPE_PRIMARY);
	scaler.SetChannelNtrValType(SMV_VALUE_TYPE_SECONDARY);
	float ratedVal1 = 3000.0f;
	float ratedVal2 = 5.0f;
	for ( int i = 0; i<k_channelNum; i++ )
	{
		str.Format(_T("Frame%d: %04x     "),i,channelData[i]);
		m_TestData += str;
		float ntrVal = scaler.ChannelMsgVal2NtrVal_Sav92(channelData[i],ratedVal1,ratedVal2,CHANNEL_TYPE_SMV_CURRENT);
		str.Format(_T("%06f    "),ntrVal/sqrt(2.0));
		m_TestData += str;
		uint32_t msgVal = scaler.ChannelNtrVal2MsgVal_Sav92(ntrVal,ratedVal1,ratedVal2,CHANNEL_TYPE_SMV_CURRENT);
		str.Format(_T("%04x\r\n"),msgVal);
		m_TestData += str;
	}

	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedGooseCode()
{
	// TODO: 在此添加控件通知处理程序代码
#ifdef CHSINT_SDK
	std::wstring fileName(_T("\\Storage Card\\数字化变电站分析仪资料\\其他资料\\pcap\\报文\\goose_test.pcap"));
#else
	std::wstring fileName(_T("\\FlashDisk\\goose_do.pcap"));
	//	std::wstring fileName(_T("\\FlashDisk\\goose3.pcap"));
#endif

	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	uint8_t   desMac[] = {0x01,0x0C,0xCD,0x01,0x00,0x01};
	uint8_t   srcMac[] = {0x00,0xE0,0x4C,0x08,0x2A,0x22};
	uint8_t   usrPrio = 4;
	uint16_t  vlanID  = 10;
	uint16_t  appID   = 0x1001;
	GOOSE_PDU_PUBLIC pduPbl = {
		"gocbRef0",20,"gocbRef0","goId0",{0x5146B339,983999967,0},1,2,0,0x1122,1,32,""
	};
	uint8_t datBuf1[2] = {0xE3,0x48};
	uint8_t datBuf2[4] = {0x01,0x02,0x03,0x04};
	//	uint8_t datBuf2[8] = {0};
	GOOSE_BOOLEAN    b      = {GSEDT_BOOLEAN,0,true};
	GOOSE_BIT_STRING bitStr = {GSEDT_BIT_STR,1,3,2,datBuf1};
	GOOSE_UTC_TIME   utcTime = {GSEDT_UTC_TIME,2,1,1826345,3};
	GOOSE_INT8    integer = {GSEDT_INT8,3,0x12};
	GOOSE_FLOAT   f = {GSEDT_FLOAT,4,987.123474f};
	// 	GOOSE_DATA channelDat[2] = {
	// 		{GSEDT_BIT_STR,3,datBuf1},{GSEDT_UTC_TIME,8,datBuf2}
	// 	};
	// 	memcpy(channelDat[0].dat,datBuf1,sizeof(datBuf1));
	// 	memcpy(channelDat[1].dat,datBuf2,sizeof(datBuf2));
	GOOSE_STRUCT structVal;
	structVal.id.type = GSEDT_STRUCT;
	structVal.member.push_back((GOOSE_DATA *)&bitStr);
	structVal.member.push_back((GOOSE_DATA *)&utcTime);
	structVal.member.push_back((GOOSE_DATA *)&b);
	uint8_t  msgData[ETHER_FRAME_MAX_LEN];
	PGOOSE_DATA pGse[] = {(GOOSE_DATA *)&b,(GOOSE_DATA *)&bitStr,(GOOSE_DATA *)&utcTime};

	m_TestData = _T("");

// 	GOOSE_STRUCT *newStruct = (GOOSE_STRUCT *)GooseDataCopy((GOOSE_DATA *)&structVal);
// 	GOOSE_BIT_STRING *newBitStr = (GOOSE_BIT_STRING *)newStruct->member.at(0);
// 	str.Format(_T("%d,%d;%x%x\r\n"),newBitStr->id.offset,newBitStr->padding,newBitStr->bitStr[0],newBitStr->bitStr[1]);
// 	m_TestData += str;
// 	GOOSE_UTC_TIME *newUtc = (GOOSE_UTC_TIME *)newStruct->member.at(1);
// 	str.Format(_T("%d;%d;%d\r\n"),newUtc->t_s,newUtc->t_us,newUtc->t_q);
// 	m_TestData += str;

	// 设值
	dwStartTime = GetTickCount();
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		m_pGooseMsg->ClearGoosePDU();
		m_pGooseMsg->SetMacAddr(desMac,srcMac,ETHER_MAC_LEN);
		m_pGooseMsg->Set8021QVlanInfo(usrPrio,vlanID);
		m_pGooseMsg->SetAppID(appID);
		m_pGooseMsg->SetPduPublicInfo(pduPbl);
		for ( int i=0; i<32; i++ )
		{
			// 添加结构体
			if ( m_pGooseMsg->AddAChannel((GOOSE_DATA *)&structVal) == false )
			{
				str.Format(_T("Add Channel %d fail\r\n"),i+1);
				m_TestData += str;
				break;
			}
			// 添加单通道数据
// 			if ( m_GooseMsg->AddAChannel((GOOSE_DATA *)&b) == false )
// 			{
// 				str.Format(_T("Add Channel %d fail\r\n"),i+1);
// 				m_TestData += str;
// 				break;
// 			}
// 			if ( m_GooseMsg->AddAChannel((GOOSE_DATA *)&bitStr) == false )
// 			{
// 				str.Format(_T("Add Channel %d fail\r\n"),i+1);
// 				m_TestData += str;
// 				break;
// 			}
// 			if ( m_GooseMsg->AddAChannel((GOOSE_DATA *)&utcTime) == false )
// 			{
// 				str.Format(_T("Add Channel %d fail\r\n"),i+1);
// 				m_TestData += str;
// 				break;
// 			}
		}
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;

	// 编码
	dwStartTime = GetTickCount();
	size_t cSize;
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		cSize = m_pGooseMsg->Code(msgData,sizeof(msgData));
	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayCodeData(msgData,cSize);
	str.Format(_T("Code Size:%d\r\n\r\n"),cSize);
	m_TestData += str;

	if ( m_PcapFile->Open(fileName,PFM_WRITE) == PCAP_FILE_ERROR )
	{
		str.Format(_T("Open Write Error!\r\n\r\n"));
		m_TestData += str;
	}
	PCAP_FILE_TIME_VAL tm = {0,0};
	if ( m_PcapFile->WriteNextPck(tm,msgData,cSize) == PCAP_FILE_ERROR )
	{
		str.Format(_T("ReadOnePckHeader ERROR!!\r\n\r\n"));
		m_TestData += str;
	}
	// 保存写入数据
	if ( m_PcapFile->Save() == PCAP_FILE_ERROR )
	{
		str.Format(_T("WriteNextPck ERROR!!!\r\n\r\n"));
		m_TestData += str;
	}

	m_PcapFile->Close();

	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedGooseDec()
{
	// TODO: 在此添加控件通知处理程序代码
#ifdef CHSINT_SDK
	std::wstring fileName(_T("\\Storage Card\\数字化变电站分析仪资料\\其他资料\\pcap\\报文\\goose_kaimo_all_type.pcap"));
#else
	std::wstring fileName(_T("\\FlashDisk\\goose_do.pcap"));
//	std::wstring fileName(_T("\\FlashDisk\\goose3.pcap"));
#endif
	PCAP_PACKET_HEADER pckHeader;
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	uint8_t *pckData = NULL;
	m_TestData = _T("");
	ETHER_II_HEADER ethHdr;
	ETHER_II_HEADER_OFFSET ethOff;
	IEEE802_1Q_VLAN_INFO vlanInfo;
	IEEE802_1Q_VLAN_INFO_OFFSET vlanOff;
	IEC8802_3_PDU_PUBLIC iec8802Info;
	IEC8802_3_PDU_PUBLIC_OFFSET iec8802Off;
	GOOSE_PDU_PUBLIC pduPbl;
	GOOSE_PDU_PUBLIC_OFFSET pduPblOff;
	size_t channelSize;
	GOOSE_DATA *gseChannel;

	if ( m_PcapFile->Open(fileName,PFM_READ) == PCAP_FILE_ERROR )
	{
		str.Format(_T("Open Write Error!\r\n\r\n"));
		m_TestData += str;
	}
	if ( m_PcapFile->SeekPckToRead(5) == PCAP_FILE_ERROR )
	{
		str.Format(_T("Seek Pck Error!\r\n\r\n"));
		m_TestData += str;
	}
	if ( m_PcapFile->ReadOnePckHeader(pckHeader) == PCAP_FILE_ERROR )
	{
		str.Format(_T("ReadOnePckHeader ERROR!!\r\n\r\n"));
		m_TestData += str;
	}
	pckData = new uint8_t[pckHeader.capLen];
	if ( m_PcapFile->ReadOnePckData(pckData,pckHeader.capLen) == PCAP_FILE_ERROR )
	{
		str.Format(_T("ReadOnePckData ERROR!!\r\n\r\n"));
		m_TestData += str;
	}

	// 解码
	size_t dSize = 0;
	dwStartTime = GetTickCount();
	for ( int i=0; i<MSG_CODEC_COUNT; i++ )
	{
		dSize =  m_pGooseMsg->Decode(pckData,pckHeader.capLen);
	}	
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;
	str.Format(_T("Src Size: %d\r\nDecode Size: %d\r\n\r\n"),pckHeader.capLen,dSize);
	m_TestData += str;
	str.Format(_T("ChannelNum: %d\r\n\r\n"),m_pGooseMsg->GetChannelNum());
	m_TestData += str;

	delete[] pckData;

	// 取值
	dwStartTime = GetTickCount();
	if ( dSize != 0 )
	{
		for ( int i=0; i<MSG_CODEC_COUNT; i++ )
		{
			m_pGooseMsg->GetEthernetHeader(ethHdr);
			m_pGooseMsg->Get8021QVlanInfo(vlanInfo);
			m_pGooseMsg->GetIEC8802Info(iec8802Info);

			m_pGooseMsg->GetPduPublicInfo(pduPbl);
			for ( size_t i = 0; i < m_pGooseMsg->GetChannelNum(); i++ )
			{
				gseChannel = m_pGooseMsg->GetAChannel(i);
			}

			m_pGooseMsg->GetEthernetHeaderOff(ethOff);
			m_pGooseMsg->Get8021QVlanInfoOff(vlanOff);
			m_pGooseMsg->GetIEC8802InfoOff(iec8802Off);

			m_pGooseMsg->GetPduPublicInfoOff(pduPblOff);
		}
	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	for ( int i = 0; i < 2; i++ )
	{
		if ( m_PcapFile->SeekPckToRead(5+16*i) == PCAP_FILE_ERROR )
		{
			str.Format(_T("Seek Pck Error!\r\n\r\n"));
			m_TestData += str;
		}
		if ( m_PcapFile->ReadOnePckHeader(pckHeader) == PCAP_FILE_ERROR )
		{
			str.Format(_T("ReadOnePckHeader ERROR!!\r\n\r\n"));
			m_TestData += str;
		}
		pckData = new uint8_t[pckHeader.capLen];
		if ( m_PcapFile->ReadOnePckData(pckData,pckHeader.capLen) == PCAP_FILE_ERROR )
		{
			str.Format(_T("ReadOnePckData ERROR!!\r\n\r\n"));
			m_TestData += str;
		}

		m_pGooseMsg->Decode(pckData,pckHeader.capLen);

		delete[] pckData;

		m_pGooseMsg->GetEthernetHeader(ethHdr);
		m_pGooseMsg->Get8021QVlanInfo(vlanInfo);
		m_pGooseMsg->GetIEC8802Info(iec8802Info);

// 			str.Format(_T("GooosePDU Len:%04x\r\n"),m_GooseMsg->GetGoosePduLen());
// 			m_TestData += str;
		m_pGooseMsg->GetPduPublicInfo(pduPbl);
// 			str.Format(_T("allData Len:%04x\r\n"),m_GooseMsg->GetGooseDataLen());
// 			m_TestData += str;
// 			str.Format(_T("allData:%d items\r\n"),m_GooseMsg->GetChannelNum());
// 			m_TestData += str;

		m_pGooseMsg->GetEthernetHeaderOff(ethOff);
		m_pGooseMsg->Get8021QVlanInfoOff(vlanOff);
		m_pGooseMsg->GetIEC8802InfoOff(iec8802Off);

// 			str.Format(_T("GooosePDU Off:%d\r\n"),m_GooseMsg->GetGoosePduOff());
// 			m_TestData += str;
		m_pGooseMsg->GetPduPublicInfoOff(pduPblOff);
// 			str.Format(_T("allData Off:%d\r\n"),m_GooseMsg->GetGooseDataOff());
// 			m_TestData += str;

		DisplayEthHdr(ethHdr);
		DisplayVlanInfo(vlanInfo);
		DisplayIEC88023Pdu(iec8802Info);
		DisplayGoosePduPbl(pduPbl);
		for ( size_t i = 0; i < m_pGooseMsg->GetChannelNum(); i++ )
		{
			gseChannel = m_pGooseMsg->GetAChannel(i);
			char datStr[256];
			ChannelDataToStr(gseChannel,datStr,256);
			DisplayGooseData(datStr);
		}

		DisplayEthHdrOff(ethOff);
		DisplayVlanInfoOff(vlanOff);
		DisplayIEC88023PduOff(iec8802Off);
		DisplayGoosePduPblOff(pduPblOff);
	}

	str.Format(_T("Close File: %d\r\n\r\n"),m_PcapFile->Close());
	m_TestData += str;
	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedNanRuiCode()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	NANRUI_FT3_ASDU_PUBLIC asduPbl = {
		44,0x02,0x04,0x0001,5,5,57,0,
		0,0,0,925,80,1
	};
	uint16_t smpVal[12] = {1,2,3,4,5,6,7,8,9,10,11,12};
	uint16_t msgData[NANRUI_FT3_MSG_WORD];

	m_TestData = _T("");

	// 设值
	dwStartTime = GetTickCount();
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		m_pNanRuiFT3Msg->SetASDU(asduPbl,smpVal,12);
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;


	// 编码
	dwStartTime = GetTickCount();
	size_t cSize;
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		cSize = m_pNanRuiFT3Msg->Code(msgData,NANRUI_FT3_MSG_WORD);
	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayCodeData(msgData,cSize);
	str.Format(_T("Code Size:%d Words\r\n\r\n"),cSize);
	m_TestData += str;
	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedNanRuiDec()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	NANRUI_FT3_ASDU_PUBLIC asduPbl;
	NANRUI_FT3_ASDU_PUBLIC_OFFSET asduPblOff;
	size_t smpsNum = m_pNanRuiFT3Msg->GetASDUSmpsNum();
	uint16_t smpVal[12];
	size_t   smpValOff[12];

	m_TestData = _T("");

	// 解码
	dwStartTime = GetTickCount();
	size_t dSize;
	for ( int i = 0; i<MSG_CODEC_COUNT; i++ )
	{
		dSize =  m_pNanRuiFT3Msg->Decode(k_NanRuiFT3,NANRUI_FT3_MSG_WORD);
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;
	str.Format(_T("Src Size: %d\r\nDecode Size: %d\r\n\r\n"),NANRUI_FT3_MSG_WORD,dSize);
	m_TestData += str;

	// 取值
	dwStartTime = GetTickCount();
	if ( dSize == 0 )
	{
		m_TestData.Format(_T("Decode Fail!"));
	}
	else
	{
		str.Format(_T("ASDU Len:%d\r\n"),m_pNanRuiFT3Msg->GetASDULen());
		m_TestData += str;
		str.Format(_T("ASDU Off:%d\r\n"),m_pNanRuiFT3Msg->GetASDUOff());
		m_TestData += str;

		for ( int i=0; i<MSG_CODEC_COUNT;i++ )
		{
			m_pNanRuiFT3Msg->GetASDUPublicInfo(asduPbl);
			m_pNanRuiFT3Msg->GetASDUSmps(smpVal,smpsNum);

			m_pNanRuiFT3Msg->GetASDUPublicInfoOff(asduPblOff);
			m_pNanRuiFT3Msg->GetASDUSmpsOff(smpValOff,smpsNum);
		}

	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayNanRuiAsduPbl(asduPbl);
	DisplayUint16Smps(smpVal,smpsNum);

	DisplayNanRuiAsduPblOff(asduPblOff);
	DisplayChannelOff(smpValOff,smpsNum);

	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedGuoWangCode()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	GUOWANG_FT3_ASDU_PUBLIC asduPbl = {
		0x003E,0x02,0xFE,0x4042,3000,3000,1100,750,
		0x0000,0x0000,3357
	};
	uint16_t smpVal[22] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22};
	uint16_t msgData[GUOWANG_FT3_MSG_WORD];

	m_TestData = _T("");

	// 设值
	dwStartTime = GetTickCount();
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		m_pGuoWangFT3Msg->SetASDU(asduPbl,smpVal,22);
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;

	// 编码
	dwStartTime = GetTickCount();
	size_t cSize;
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		cSize = m_pGuoWangFT3Msg->Code(msgData,GUOWANG_FT3_MSG_WORD);
	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayCodeData(msgData,cSize);
	str.Format(_T("Code Size:%d Words\r\n\r\n"),cSize);
	m_TestData += str;
	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedGuoWangDec()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	GUOWANG_FT3_ASDU_PUBLIC asduPbl;
	GUOWANG_FT3_ASDU_PUBLIC_OFFSET asduPblOff;
	size_t smpsNum = m_pGuoWangFT3Msg->GetASDUSmpsNum();
	uint16_t smpVal[22];
	size_t   smpValOff[22];

	m_TestData = _T("");

	// 解码
	dwStartTime = GetTickCount();
	size_t dSize;
	for ( int i=0; i<MSG_CODEC_COUNT; i++ )
	{
		dSize = m_pGuoWangFT3Msg->Decode(k_GuoWangFT3,GUOWANG_FT3_MSG_WORD);
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;
	str.Format(_T("Src Size: %d\r\nDecode Size: %d\r\n\r\n"),GUOWANG_FT3_MSG_WORD,dSize);
	m_TestData += str;

	// 取值
	dwStartTime = GetTickCount();
	if ( dSize == 0 )
	{
		m_TestData.Format(_T("Decode Fail!"));
	}
	else
	{
		str.Format(_T("ASDU Len:%d\r\n"),m_pGuoWangFT3Msg->GetASDULen());
		m_TestData += str;
		str.Format(_T("ASDU Off:%d\r\n"),m_pGuoWangFT3Msg->GetASDUOff());
		m_TestData += str;

		for ( int i=0; i<MSG_CODEC_COUNT; i++ )
		{
			m_pGuoWangFT3Msg->GetASDUPublicInfo(asduPbl);
			m_pGuoWangFT3Msg->GetASDUSmps(smpVal,smpsNum);

			m_pGuoWangFT3Msg->GetASDUPublicInfoOff(asduPblOff);
			m_pGuoWangFT3Msg->GetASDUSmpsOff(smpValOff,smpsNum);
		}

	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayGuoWangAsduPbl(asduPbl);
	DisplayUint16Smps(smpVal,smpsNum);

	DisplayGuoWangAsduPblOff(asduPblOff);
	DisplayChannelOff(smpValOff,smpsNum);

	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedRapidDecode()
{
	// TODO: 在此添加控件通知处理程序代码
	m_TestData = _T("");
#ifdef CHSINT_SDK
	std::wstring fileName(_T("\\Storage Card\\数字化变电站分析仪资料\\其他资料\\pcap\\报文\\Filter92.pcap"));
#else
	std::wstring fileName(_T("\\FlashDisk\\Filter92.pcap"));
#endif
	PCAP_PACKET_HEADER pckHeader;
	uint8_t *pckData = NULL;
	int count = 4000;

	DWORD dwStartTime;
	DWORD dwFinishTime;
	uint8_t mac[6];
	uint16_t etherType;
	uint16_t appId;
	size_t channelNum;
	uint16_t lnName;
	uint16_t smpRate;
	uint16_t noASDU;
	uint16_t pduLen;
	CString str;
	MsgRapidDecoder rapidDecoder;

	// Sav91
	dwStartTime = GetTickCount();
	if ( rapidDecoder.GetMessageType(k_Sav91) == FRAME_TYPE_NET_9_1 )
	{
		str.Format(_T("Sav9-1:\r\n"));
		m_TestData += str;
	}
	for ( int i = 0; i < count; i++ )
	{
		rapidDecoder.GetDstMac(k_Sav91,mac);
		rapidDecoder.GetEtherType(k_Sav91,etherType);
		rapidDecoder.GetAppID(k_Sav91,appId);
		rapidDecoder.GetChannelNum(k_Sav91,FRAME_TYPE_NET_9_1,channelNum);
		rapidDecoder.GetLNName(k_Sav91,FRAME_TYPE_NET_9_1,lnName);
		rapidDecoder.GetNoASDU(k_Sav91,FRAME_TYPE_NET_9_1,noASDU);
		rapidDecoder.GetSmpRate(k_Sav91,FRAME_TYPE_NET_9_1,smpRate);
		rapidDecoder.GetPDULen(k_Sav91,pduLen);
	}
	dwFinishTime = GetTickCount();

	str.Format(_T("Mac:%02x-%02x-%02x-%02x-%02x-%02x;\r\n"),mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	m_TestData += str;
	str.Format(_T("EtherType:0x%04x;\r\n"),etherType);
	m_TestData += str;
	str.Format(_T("AppID:0x%04x;\r\n"),appId);
	m_TestData += str;
	str.Format(_T("LnName:0x%02x;\r\n"),lnName);
	m_TestData += str;
	str.Format(_T("NoASDU:%d;\r\n"),noASDU);
	m_TestData += str;
	str.Format(_T("SmpRate:%d;\r\n"),smpRate);
	m_TestData += str;
	str.Format(_T("ChannelNum:%d;\r\n"),channelNum);
	m_TestData += str;
	str.Format(_T("PduLen:%d;\r\n"),pduLen);
	m_TestData += str;
	str.Format(_T("Time:%d;\r\n\r\n"),dwFinishTime-dwStartTime);
	m_TestData += str;

	// Sav91Ex
	dwStartTime = GetTickCount();
	if ( rapidDecoder.GetMessageType(k_Sav91Ex) == FRAME_TYPE_NET_9_1_EX )
	{
		str.Format(_T("Sav9-1Ex:\r\n"));
		m_TestData += str;
	}
	for ( int i = 0; i < count; i++ )
	{
		rapidDecoder.GetDstMac(k_Sav91Ex,mac);
		rapidDecoder.GetEtherType(k_Sav91Ex,etherType);
		rapidDecoder.GetAppID(k_Sav91Ex,appId);
		rapidDecoder.GetChannelNum(k_Sav91Ex,FRAME_TYPE_NET_9_1_EX,channelNum);
		rapidDecoder.GetLNName(k_Sav91Ex,FRAME_TYPE_NET_9_1_EX,lnName);
		rapidDecoder.GetNoASDU(k_Sav91Ex,FRAME_TYPE_NET_9_1_EX,noASDU);
		rapidDecoder.GetSmpRate(k_Sav91Ex,FRAME_TYPE_NET_9_1_EX,smpRate);
		rapidDecoder.GetPDULen(k_Sav91Ex,pduLen);
	}
	dwFinishTime = GetTickCount();

	str.Format(_T("Mac:%02x-%02x-%02x-%02x-%02x-%02x;\r\n"),mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	m_TestData += str;
	str.Format(_T("EtherType:0x%04x;\r\n"),etherType);
	m_TestData += str;
	str.Format(_T("AppID:0x%04x;\r\n"),appId);
	m_TestData += str;
	str.Format(_T("LnName:0x%02x;\r\n"),lnName);
	m_TestData += str;
	str.Format(_T("NoASDU:%d;\r\n"),noASDU);
	m_TestData += str;
	str.Format(_T("SmpRate:%d;\r\n"),smpRate);
	m_TestData += str;
	str.Format(_T("ChannelNum:%d;\r\n"),channelNum);
	m_TestData += str;
	str.Format(_T("PduLen:%d;\r\n"),pduLen);
	m_TestData += str;
	str.Format(_T("Time:%d;\r\n\r\n"),dwFinishTime-dwStartTime);
	m_TestData += str;

	// Sav92
	if ( m_PcapFile->Open(fileName,PFM_READ) == PCAP_FILE_ERROR )
	{
		str.Format(_T("Open Write Error!\r\n\r\n"));
		m_TestData += str;
	}
	if ( m_PcapFile->SeekPckToRead(0) == PCAP_FILE_ERROR )
	{
		str.Format(_T("Seek Pck Error!\r\n\r\n"));
		m_TestData += str;
	}
	if ( m_PcapFile->ReadOnePckHeader(pckHeader) == PCAP_FILE_ERROR )
	{
		str.Format(_T("ReadOnePckHeader ERROR!!\r\n\r\n"));
		m_TestData += str;
	}
	pckData = new uint8_t[pckHeader.capLen];
	if ( m_PcapFile->ReadOnePckData(pckData,pckHeader.capLen) == PCAP_FILE_ERROR )
	{
		str.Format(_T("ReadOnePckData ERROR!!\r\n\r\n"));
		m_TestData += str;
	}
	m_PcapFile->Close();

	dwStartTime = GetTickCount();
	if ( rapidDecoder.GetMessageType(pckData) == FRAME_TYPE_NET_9_2 )
	{
		str.Format(_T("Sav9-2:\r\n"));
		m_TestData += str;
	}
	for ( int i = 0; i < count; i++ )
	{
		rapidDecoder.GetDstMac(pckData,mac);
		rapidDecoder.GetEtherType(pckData,etherType);
		rapidDecoder.GetAppID(pckData,appId);
		rapidDecoder.GetChannelNum(pckData,FRAME_TYPE_NET_9_2,channelNum);
		rapidDecoder.GetLNName(pckData,FRAME_TYPE_NET_9_2,lnName);
		rapidDecoder.GetNoASDU(pckData,FRAME_TYPE_NET_9_2,noASDU);
		rapidDecoder.GetSmpRate(pckData,FRAME_TYPE_NET_9_2,smpRate);
		rapidDecoder.GetPDULen(pckData,pduLen);
	}
	dwFinishTime = GetTickCount();

	delete[] pckData;

	str.Format(_T("Mac:%02x-%02x-%02x-%02x-%02x-%02x;\r\n"),mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	m_TestData += str;
	str.Format(_T("EtherType:0x%04x;\r\n"),etherType);
	m_TestData += str;
	str.Format(_T("AppID:0x%04x;\r\n"),appId);
	m_TestData += str;
	str.Format(_T("LnName:0x%02x;\r\n"),lnName);
	m_TestData += str;
	str.Format(_T("NoASDU:%d;\r\n"),noASDU);
	m_TestData += str;
	str.Format(_T("SmpRate:%d;\r\n"),smpRate);
	m_TestData += str;
	str.Format(_T("ChannelNum:%d;\r\n"),channelNum);
	m_TestData += str;
	str.Format(_T("PduLen:%d;\r\n"),pduLen);
	m_TestData += str;
	str.Format(_T("Time:%d;\r\n\r\n"),dwFinishTime-dwStartTime);
	m_TestData += str;

	// GOOSE
#ifdef CHSINT_SDK
	fileName = _T("\\Storage Card\\数字化变电站分析仪资料\\其他资料\\pcap\\报文\\goose3.pcap");
#else
	fileName = _T("\\FlashDisk\\goose3.pcap");
#endif

	if ( m_PcapFile->Open(fileName,PFM_READ) == PCAP_FILE_ERROR )
	{
		str.Format(_T("Open Write Error!\r\n\r\n"));
		m_TestData += str;
	}
	if ( m_PcapFile->SeekPckToRead(0) == PCAP_FILE_ERROR )
	{
		str.Format(_T("Seek Pck Error!\r\n\r\n"));
		m_TestData += str;
	}
	if ( m_PcapFile->ReadOnePckHeader(pckHeader) == PCAP_FILE_ERROR )
	{
		str.Format(_T("ReadOnePckHeader ERROR!!\r\n\r\n"));
		m_TestData += str;
	}
	pckData = new uint8_t[pckHeader.capLen];
	if ( m_PcapFile->ReadOnePckData(pckData,pckHeader.capLen) == PCAP_FILE_ERROR )
	{
		str.Format(_T("ReadOnePckData ERROR!!\r\n\r\n"));
		m_TestData += str;
	}
	m_PcapFile->Close();

	dwStartTime = GetTickCount();
	if ( rapidDecoder.GetMessageType(pckData) == FRAME_TYPE_NET_GOOSE )
	{
		str.Format(_T("Goose:\r\n"));
		m_TestData += str;
	}
	for ( int i = 0; i < count; i++ )
	{
		rapidDecoder.GetDstMac(pckData,mac);
		rapidDecoder.GetEtherType(pckData,etherType);
		rapidDecoder.GetAppID(pckData,appId);
		rapidDecoder.GetChannelNum(pckData,FRAME_TYPE_NET_GOOSE,channelNum);
		rapidDecoder.GetLNName(pckData,FRAME_TYPE_NET_GOOSE,lnName);
		rapidDecoder.GetSmpRate(pckData,FRAME_TYPE_NET_GOOSE,smpRate);
		rapidDecoder.GetPDULen(pckData,pduLen);
	}
	dwFinishTime = GetTickCount();

	delete[] pckData;

	str.Format(_T("Mac:%02x-%02x-%02x-%02x-%02x-%02x;\r\n"),mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	m_TestData += str;
	str.Format(_T("EtherType:0x%04x;\r\n"),etherType);
	m_TestData += str;
	str.Format(_T("AppID:0x%04x;\r\n"),appId);
	m_TestData += str;
	str.Format(_T("LnName:0x%02x;\r\n"),lnName);
	m_TestData += str;
	str.Format(_T("SmpRate:%d;\r\n"),smpRate);
	m_TestData += str;
	str.Format(_T("ChannelNum:%d;\r\n"),channelNum);
	m_TestData += str;
	str.Format(_T("PduLen:%d;\r\n"),pduLen);
	m_TestData += str;
	str.Format(_T("Time:%d;\r\n\r\n"),dwFinishTime-dwStartTime);
	m_TestData += str;

	// NanRuiFT3
	dwStartTime = GetTickCount();
	if ( rapidDecoder.GetMessageType((uint8_t *)k_NanRuiFT3) == FRAME_TYPE_FT3_NANRUI )
	{
		str.Format(_T("NanRuiFT3:\r\n"));
		m_TestData += str;
	}
	for ( int i = 0; i < count; i++ )
	{
		rapidDecoder.GetChannelNum((uint8_t *)k_NanRuiFT3,FRAME_TYPE_FT3_NANRUI,channelNum);
		rapidDecoder.GetLNName((uint8_t *)k_NanRuiFT3,FRAME_TYPE_FT3_NANRUI,lnName);
		rapidDecoder.GetSmpRate((uint8_t *)k_NanRuiFT3,FRAME_TYPE_FT3_NANRUI,smpRate);
		rapidDecoder.GetPDULen((uint8_t *)k_NanRuiFT3,pduLen);
	}
	dwFinishTime = GetTickCount();

	str.Format(_T("LnName:0x%02x;\r\n"),lnName);
	m_TestData += str;
	str.Format(_T("SmpRate:%d;\r\n"),smpRate);
	m_TestData += str;
	str.Format(_T("ChannelNum:%d;\r\n"),channelNum);
	m_TestData += str;
	str.Format(_T("PduLen:%d;\r\n"),pduLen);
	m_TestData += str;
	str.Format(_T("Time:%d;\r\n\r\n"),dwFinishTime-dwStartTime);
	m_TestData += str;

	// GuoWangFT3
	dwStartTime = GetTickCount();
	if ( rapidDecoder.GetMessageType((uint8_t *)k_GuoWangFT3) == FRAME_TYPE_FT3_GUOWANG )
	{
		str.Format(_T("GuoWangFT3:\r\n"));
		m_TestData += str;
	}
	for ( int i = 0; i < count; i++ )
	{
		rapidDecoder.GetChannelNum((uint8_t *)k_GuoWangFT3,FRAME_TYPE_FT3_GUOWANG,channelNum);
		rapidDecoder.GetLNName((uint8_t *)k_GuoWangFT3,FRAME_TYPE_FT3_GUOWANG,lnName);
		rapidDecoder.GetSmpRate((uint8_t *)k_GuoWangFT3,FRAME_TYPE_FT3_GUOWANG,smpRate);
		rapidDecoder.GetPDULen((uint8_t *)k_GuoWangFT3,pduLen);
	}
	dwFinishTime = GetTickCount();

	str.Format(_T("LnName:0x%02x;\r\n"),lnName);
	m_TestData += str;
	str.Format(_T("SmpRate:%d;\r\n"),smpRate);
	m_TestData += str;
	str.Format(_T("ChannelNum:%d;\r\n"),channelNum);
	m_TestData += str;
	str.Format(_T("PduLen:%d;\r\n"),pduLen);
	m_TestData += str;
	str.Format(_T("Time:%d;\r\n\r\n"),dwFinishTime-dwStartTime);
	m_TestData += str;

	// Std52FT3
	dwStartTime = GetTickCount();
	if ( rapidDecoder.GetMessageType((uint8_t *)k_Std52Ft3) == FRAME_TYPE_FT3_STD52 )
	{
		str.Format(_T("Std52FT3:\r\n"));
		m_TestData += str;
	}
	for ( int i = 0; i < count; i++ )
	{
		rapidDecoder.GetChannelNum((uint8_t *)k_Std52Ft3,FRAME_TYPE_FT3_STD52,channelNum);
		rapidDecoder.GetLNName((uint8_t *)k_Std52Ft3,FRAME_TYPE_FT3_STD52,lnName);
		rapidDecoder.GetSmpRate((uint8_t *)k_Std52Ft3,FRAME_TYPE_FT3_STD52,smpRate);
		rapidDecoder.GetPDULen((uint8_t *)k_Std52Ft3,pduLen);
	}
	dwFinishTime = GetTickCount();

	str.Format(_T("LnName:0x%02x;\r\n"),lnName);
	m_TestData += str;
	str.Format(_T("SmpRate:%d;\r\n"),smpRate);
	m_TestData += str;
	str.Format(_T("ChannelNum:%d;\r\n"),channelNum);
	m_TestData += str;
	str.Format(_T("PduLen:%d;\r\n"),pduLen);
	m_TestData += str;
	str.Format(_T("Time:%d;\r\n\r\n"),dwFinishTime-dwStartTime);
	m_TestData += str;

	// OnePhaseCtorVtFT3
	dwStartTime = GetTickCount();
	if ( rapidDecoder.GetMessageType((uint8_t *)k_1PhaseCtvtFt3) == FRAME_TYPE_FT3_ONEPHASE_CTORVT )
	{
		str.Format(_T("OnePhaseCtorVtFT3:\r\n"));
		m_TestData += str;
	}
	for ( int i = 0; i < count; i++ )
	{
		rapidDecoder.GetChannelNum((uint8_t *)k_1PhaseCtvtFt3,FRAME_TYPE_FT3_ONEPHASE_CTORVT,channelNum);
		rapidDecoder.GetLNName((uint8_t *)k_1PhaseCtvtFt3,FRAME_TYPE_FT3_ONEPHASE_CTORVT,lnName);
		rapidDecoder.GetSmpRate((uint8_t *)k_1PhaseCtvtFt3,FRAME_TYPE_FT3_ONEPHASE_CTORVT,smpRate);
		rapidDecoder.GetPDULen((uint8_t *)k_1PhaseCtvtFt3,pduLen);
	}
	dwFinishTime = GetTickCount();

	str.Format(_T("LnName:0x%02x;\r\n"),lnName);
	m_TestData += str;
	str.Format(_T("SmpRate:%d;\r\n"),smpRate);
	m_TestData += str;
	str.Format(_T("ChannelNum:%d;\r\n"),channelNum);
	m_TestData += str;
	str.Format(_T("PduLen:%d;\r\n"),pduLen);
	m_TestData += str;
	str.Format(_T("Time:%d;\r\n\r\n"),dwFinishTime-dwStartTime);
	m_TestData += str;

	// TriPhaseCtFT3
	dwStartTime = GetTickCount();
	if ( rapidDecoder.GetMessageType((uint8_t *)k_3PhaseCtFt3) == FRAME_TYPE_FT3_TRIPHASE_CT )
	{
		str.Format(_T("TriPhaseCtFT3:\r\n"));
		m_TestData += str;
	}
	for ( int i = 0; i < count; i++ )
	{
		rapidDecoder.GetChannelNum((uint8_t *)k_3PhaseCtFt3,FRAME_TYPE_FT3_TRIPHASE_CT,channelNum);
		rapidDecoder.GetLNName((uint8_t *)k_3PhaseCtFt3,FRAME_TYPE_FT3_TRIPHASE_CT,lnName);
		rapidDecoder.GetSmpRate((uint8_t *)k_3PhaseCtFt3,FRAME_TYPE_FT3_TRIPHASE_CT,smpRate);
		rapidDecoder.GetPDULen((uint8_t *)k_3PhaseCtFt3,pduLen);
	}
	dwFinishTime = GetTickCount();

	str.Format(_T("LnName:0x%02x;\r\n"),lnName);
	m_TestData += str;
	str.Format(_T("SmpRate:%d;\r\n"),smpRate);
	m_TestData += str;
	str.Format(_T("ChannelNum:%d;\r\n"),channelNum);
	m_TestData += str;
	str.Format(_T("PduLen:%d;\r\n"),pduLen);
	m_TestData += str;
	str.Format(_T("Time:%d;\r\n\r\n"),dwFinishTime-dwStartTime);
	m_TestData += str;

	// TriPhaseVt2010FT3
	dwStartTime = GetTickCount();
	if ( rapidDecoder.GetMessageType((uint8_t *)k_3PhaseVt10Ft3) == FRAME_TYPE_FT3_TRIPHASE_VT_2010 )
	{
		str.Format(_T("TriPhaseVt2010FT3:\r\n"));
		m_TestData += str;
	}
	for ( int i = 0; i < count; i++ )
	{
		rapidDecoder.GetMessageType((uint8_t *)k_3PhaseVt10Ft3);
		rapidDecoder.GetChannelNum((uint8_t *)k_3PhaseVt10Ft3,FRAME_TYPE_FT3_TRIPHASE_VT_2010,channelNum);
		rapidDecoder.GetLNName((uint8_t *)k_3PhaseVt10Ft3,FRAME_TYPE_FT3_TRIPHASE_VT_2010,lnName);
		rapidDecoder.GetSmpRate((uint8_t *)k_3PhaseVt10Ft3,FRAME_TYPE_FT3_TRIPHASE_VT_2010,smpRate);
		rapidDecoder.GetPDULen((uint8_t *)k_3PhaseVt10Ft3,pduLen);
	}
	dwFinishTime = GetTickCount();

	str.Format(_T("LnName:0x%02x;\r\n"),lnName);
	m_TestData += str;
	str.Format(_T("SmpRate:%d;\r\n"),smpRate);
	m_TestData += str;
	str.Format(_T("ChannelNum:%d;\r\n"),channelNum);
	m_TestData += str;
	str.Format(_T("PduLen:%d;\r\n"),pduLen);
	m_TestData += str;
	str.Format(_T("Time:%d;\r\n\r\n"),dwFinishTime-dwStartTime);
	m_TestData += str;

	// TriPhaseVt2011FT3
	dwStartTime = GetTickCount();
	if ( rapidDecoder.GetMessageType((uint8_t *)k_3PhaseVt11Ft3) == FRAME_TYPE_FT3_TRIPHASE_VT_2011 )
	{
		str.Format(_T("TriPhaseVt2011FT3:\r\n"));
		m_TestData += str;
	}
	for ( int i = 0; i < count; i++ )
	{
		rapidDecoder.GetMessageType((uint8_t *)k_3PhaseVt11Ft3);
		rapidDecoder.GetChannelNum((uint8_t *)k_3PhaseVt11Ft3,FRAME_TYPE_FT3_TRIPHASE_VT_2011,channelNum);
		rapidDecoder.GetLNName((uint8_t *)k_3PhaseVt11Ft3,FRAME_TYPE_FT3_TRIPHASE_VT_2011,lnName);
		rapidDecoder.GetSmpRate((uint8_t *)k_3PhaseVt11Ft3,FRAME_TYPE_FT3_TRIPHASE_VT_2011,smpRate);
		rapidDecoder.GetPDULen((uint8_t *)k_3PhaseVt11Ft3,pduLen);
	}
	dwFinishTime = GetTickCount();

	str.Format(_T("LnName:0x%02x;\r\n"),lnName);
	m_TestData += str;
	str.Format(_T("SmpRate:%d;\r\n"),smpRate);
	m_TestData += str;
	str.Format(_T("ChannelNum:%d;\r\n"),channelNum);
	m_TestData += str;
	str.Format(_T("PduLen:%d;\r\n"),pduLen);
	m_TestData += str;
	str.Format(_T("Time:%d;\r\n\r\n"),dwFinishTime-dwStartTime);
	m_TestData += str;

	// TriPhaseCtorVtFT3
	dwStartTime = GetTickCount();
	if ( rapidDecoder.GetMessageType((uint8_t *)k_3PhaseCtvtFt3) == FRAME_TYPE_FT3_TRIPHASE_CTORVT )
	{
		str.Format(_T("TriPhaseCtorVtFT3:\r\n"));
		m_TestData += str;
	}
	for ( int i = 0; i < count; i++ )
	{
		rapidDecoder.GetChannelNum((uint8_t *)k_3PhaseCtvtFt3,FRAME_TYPE_FT3_TRIPHASE_CTORVT,channelNum);
		rapidDecoder.GetLNName((uint8_t *)k_3PhaseCtvtFt3,FRAME_TYPE_FT3_TRIPHASE_CTORVT,lnName);
		rapidDecoder.GetSmpRate((uint8_t *)k_3PhaseCtvtFt3,FRAME_TYPE_FT3_TRIPHASE_CTORVT,smpRate);
		rapidDecoder.GetPDULen((uint8_t *)k_3PhaseCtvtFt3,pduLen);
	}
	dwFinishTime = GetTickCount();

	str.Format(_T("LnName:0x%02x;\r\n"),lnName);
	m_TestData += str;
	str.Format(_T("SmpRate:%d;\r\n"),smpRate);
	m_TestData += str;
	str.Format(_T("ChannelNum:%d;\r\n"),channelNum);
	m_TestData += str;
	str.Format(_T("PduLen:%d;\r\n"),pduLen);
	m_TestData += str;
	str.Format(_T("Time:%d;\r\n\r\n"),dwFinishTime-dwStartTime);
	m_TestData += str;

	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedBtn12()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	GUOWANG_FT3_ASDU_PUBLIC asduPbl;
	GUOWANG_FT3_ASDU_PUBLIC_OFFSET asduPblOff;
	size_t smpsNum = m_pGuoWangFT3Msg->GetASDUSmpsNum();
	uint16_t smpVal[22];
	size_t   smpValOff[22];
	uint8_t  msgData[GUOWANG_FT3_MSG_WORD*2];
	
// 	CFile file;
// 	file.Open(_T("\\Storage Card\\数字化变电站分析仪资料\\FT3 57.735_57.735_57.735_110_55.pcap"),CFile::modeReadWrite);
// 	m_TestData = _T("");
// 
// 	// 解码
// 	dwStartTime = GetTickCount();
// 	for ( int i=0; i<80; i++ )
// 	{
// 		file.Read(msgData,GUOWANG_FT3_MSG_WORD*2);
// 		size_t dSize;
// 		dSize = m_pGuoWangFT3Msg->Decode((uint16_t *)msgData,GUOWANG_FT3_MSG_WORD);
// 
// 		// 取值
// 		dwStartTime = GetTickCount();
// 		if ( dSize == 0 )
// 		{
// 			m_TestData.Format(_T("Decode Fail!"));
// 		}
// 		else
// 		{
// 			float ratedVal1 = 110000.0f;
// 			float ratedVal2 = 100.0f;
// 			//			m_pGuoWangFT3Msg->GetASDUPublicInfo(asduPbl);
// 			m_pGuoWangFT3Msg->GetASDUSmps(smpVal,smpsNum);
// 
// 			//			m_pGuoWangFT3Msg->GetASDUPublicInfoOff(asduPblOff);
// 			//			m_pGuoWangFT3Msg->GetASDUSmpsOff(smpValOff,smpsNum);
// 			str.Format(_T("Frame%d: %04x     "),i,smpVal[19]);
// 			m_TestData += str;
// 			float ntrVal = scaler.ChannelMsgVal2NtrVal_FT3(smpVal[19],ratedVal1,ratedVal2,SCT_VOLTAGE);
// 			str.Format(_T("%06f    "),ntrVal/sqrt(2.0));
// 			m_TestData += str;
// 			uint16_t msgVal = scaler.ChannelNtrVal2MsgVal_FT3(ntrVal,ratedVal1,ratedVal2,SCT_VOLTAGE);
// 			str.Format(_T("%04x\r\n"),msgVal);
// 			m_TestData += str;
// 		}
// 	}

	// 报文二次值 设置二次值
	CSmvScaler scaler;
	scaler.SetChannelMsgValType(SMV_VALUE_TYPE_SECONDARY);
	scaler.SetChannelNtrValType(SMV_VALUE_TYPE_SECONDARY);

	double rms_vol = 115.469;
	double rms_amp = 10.0;
	double smpRate = 50.0;
	int   smpCount = 80;
	float ratedAmp1 = 3000.0f;
	float ratedAmp2 = 5.0f;
	float ratedVol1 = 110000.0f;
	float ratedVol2 = 100.0f;
	
	for ( int i = 0; i < smpCount; i++ )
	{
		double vol = rms_vol*sqrt(2.0)*sin(2.0*PI*smpRate*(1.0/smpRate/double(smpCount)*double(i))/*+2.0/3.0*PI*/);
		double amp = rms_amp*sqrt(2.0)*sin(2.0*PI*smpRate*(1.0/smpRate/double(smpCount)*double(i))/*+2.0/3.0*PI*/);

		int16_t u = scaler.ChannelNtrVal2MsgVal_FT3(amp,ratedAmp1,ratedAmp2,CHANNEL_TYPE_SMV_CURRENT,CHANNEL_VALUE_TYPE_MEASURE);
		float f = scaler.ChannelMsgVal2NtrVal_FT3(u,ratedAmp1,ratedAmp2,CHANNEL_TYPE_SMV_CURRENT,CHANNEL_VALUE_TYPE_MEASURE);
		str.Format(_T("Amp:%d  %0.3f  "),u,f);
		m_TestData += str;

		u = scaler.ChannelNtrVal2MsgVal_FT3(vol,ratedVol1,ratedVol2,CHANNEL_TYPE_SMV_VOLTAGE,CHANNEL_VALUE_TYPE_UNKNOWN);
		f = scaler.ChannelMsgVal2NtrVal_FT3(u,ratedVol1,ratedVol2,CHANNEL_TYPE_SMV_VOLTAGE,CHANNEL_VALUE_TYPE_UNKNOWN);
		str.Format(_T("Vol:%d  %0.3f\r\n"),u,f);
		m_TestData += str;
	}
	str.Format(_T("INT16MAX %d\r\n"),(int16_t)0x7FFF);
	m_TestData += str;
	str.Format(_T("INT16MAX %d\r\n"),(int16_t)(10.0*sqrt(2.0)/ratedAmp2*11585.0));
	m_TestData += str;
	str.Format(_T("INT16MAX %d\r\n"),(int16_t)(115.469*sqrt(2.0)/(ratedVol2/sqrt(3.0))*11585.0));
	m_TestData += str;
	str.Format(_T("INT16MAX/11585 %0.3f\r\n"),((float)0x7FFF*ratedAmp2/11585.0/sqrt(2.0)));
	m_TestData += str;
	str.Format(_T("INT16MAX/11585 %0.3f\r\n"),((double)0x7FFF*(ratedVol2/sqrt(3.0))/11585.0/sqrt(2.0)));
	m_TestData += str;

	// 	uint32_t u32 = scaler.ChannelNtrVal2MsgVal_Sav92(-75.0f,110.0f,100.0f,CHANNEL_TYPE_SMV_VOLTAGE);
	// 	str.Format(_T("%x\r\n"),u32);
	// 	m_TestData += str;
	// 	uint16_t u = scaler.ChannelNtrVal2MsgVal_Sav91(-75.0f,110.0f,100.0f,CHANNEL_TYPE_SMV_VOLTAGE,CHANNEL_VALUE_TYPE_UNKNOWN);
	// 	str.Format(_T("%x\r\n"),u);
	// 	m_TestData += str;

// 	dwFinishTime = GetTickCount();
// 	m_CodeTime = dwFinishTime-dwStartTime;
//	file.Close();
	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedSav92MulDec()
{
	Sav92MulDecTest();
// 	SetTimer(2,DECODE_TIMER,NULL);
// 	KillTimer(1);
}

void CWinCETestDlg::Sav92MulDecTest()
{
	// TODO: 在此添加控件通知处理程序代码
#ifdef CHSINT_SDK
	//	std::wstring fileName(_T("\\Storage Card\\数字化变电站分析仪资料\\其他资料\\pcap\\报文\\sav92.pcap"));
	std::wstring fileName(_T("\\Storage Card\\数字化变电站分析仪资料\\其他资料\\pcap\\报文\\Filter92.pcap"));
#else
	std::wstring fileName(_T("\\FlashDisk\\Filter92_MUL_1ASDU_32.pcap"));
#endif
	PCAP_PACKET_HEADER pckHeader;
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	uint8_t *pckData = NULL;
	m_TestData = _T("");

	const int maxNoASDU = 8;
	ETHER_II_HEADER ethHdr;
	ETHER_II_HEADER_OFFSET ethOff;
	IEEE802_1Q_VLAN_INFO vlanInfo;
	IEEE802_1Q_VLAN_INFO_OFFSET vlanOff;
	IEC8802_3_PDU_PUBLIC iec8802Info;
	IEC8802_3_PDU_PUBLIC_OFFSET iec8802Off;
	SAV92_ASDU_PUBLIC asduPbl[maxNoASDU];
	SAV92_ASDU_PUBLIC_OFFSET asduPblOff[maxNoASDU];
	uint16_t noASDU;
	uint32_t smpVal[maxNoASDU][30];
	uint32_t smpQlt[maxNoASDU][30];
	size_t smpsNum;
	uint32_t smpValOff[maxNoASDU][30];
	uint32_t smpQltOff[maxNoASDU][30];
	size_t   seqDataOff[maxNoASDU];

	if ( m_PcapFile->Open(fileName,PFM_READ) == PCAP_FILE_ERROR )
	{
		str.Format(_T("Open Write Error!\r\n\r\n"));
		m_TestData += str;
	}

	int pckArrSize = 0;
	static int pckOff = 0;
	pckOff = (++pckOff)%4;
	for ( int i = 0; i < 4; i++ )
	{
		if ( m_PcapFile->SeekPckToRead(pckOff+i) == PCAP_FILE_ERROR )
		{
			str.Format(_T("Seek Pck Error!\r\n\r\n"));
			m_TestData += str;
		}
		if ( m_PcapFile->ReadOnePckHeader(pckHeader) == PCAP_FILE_ERROR )
		{
			str.Format(_T("ReadOnePckHeader ERROR!!\r\n\r\n"));
			m_TestData += str;
		}
		if ( pckData == NULL )
		{
			pckArrSize = (pckHeader.capLen+1)/2*2;
			pckData = new uint8_t[pckArrSize*4];
		}
		if ( m_PcapFile->ReadOnePckData(&pckData[i*pckArrSize],pckHeader.capLen) == PCAP_FILE_ERROR )
		{
			str.Format(_T("ReadOnePckData ERROR!!\r\n\r\n"));
			m_TestData += str;
		}
	}

	size_t dSize;
	m_DecTime = 0;
	m_CodeTime = 0;
	dwStartTime = GetTickCount();
	for ( int i = 0; i < MSG_CODEC_COUNT/**4*/; i++ )
	{
		// 解码
		int idx = i%4;
//		dwStartTime = GetTickCount();
		dSize =  m_vSav92Msg[idx].pSav92Msg->Decode(&pckData[idx*pckArrSize],pckHeader.capLen);
// 		dwFinishTime = GetTickCount();
// 		m_DecTime += dwFinishTime-dwStartTime;

		// 取值
//		dwStartTime = GetTickCount();
		if ( dSize > 0 )
		{
// 			m_vSav92Msg[idx].pSav92Msg->GetEthernetHeader(ethHdr);
// 			m_vSav92Msg[idx].pSav92Msg->Get8021QVlanInfo(vlanInfo);
// 			m_vSav92Msg[idx].pSav92Msg->GetIEC8802Info(iec8802Info);

			noASDU = m_pSav92Msg->GetNoASDU();
			for ( uint16_t j = 0; j < noASDU ; j++ )
			{
				smpsNum = m_vSav92Msg[idx].pSav92Msg->GetASDUSmpsNum(j);
				m_vSav92Msg[idx].pSav92Msg->GetASDUPublicInfo(j,asduPbl[j]);
				m_vSav92Msg[idx].pSav92Msg->GetASDUSmps(j,smpVal[j],smpQlt[j],smpsNum);
			}

// 			m_vSav92Msg[idx].pSav92Msg->GetEthernetHeaderOff(ethOff);
// 			m_vSav92Msg[idx].pSav92Msg->Get8021QVlanInfoOff(vlanOff);
// 			m_vSav92Msg[idx].pSav92Msg->GetIEC8802InfoOff(iec8802Off);
// 
// 			for ( uint16_t j = 0; j < noASDU ; j++ )
// 			{
// 				smpsNum = m_vSav92Msg[idx].pSav92Msg->GetASDUSmpsNum(j);
// 				m_vSav92Msg[idx].pSav92Msg->GetASDUPublicInfoOff(j,asduPblOff[j]);
// 				m_vSav92Msg[idx].pSav92Msg->GetASDUSmpsOff(j,seqDataOff[j],smpValOff[j],smpQltOff[j],smpsNum);
// 			}
		}
// 		dwFinishTime = GetTickCount();
// 		m_CodeTime += dwFinishTime-dwStartTime;
	}
	dwFinishTime = GetTickCount();
	m_DecTime += dwFinishTime-dwStartTime;
	RETAILMSG(1,(_T("Decode Time:%d\r\nGetVal Time:%d\r\n\r\n"),m_DecTime,m_CodeTime));

	str.Format(_T("Src Size: %d\r\nDecode Size: %d\r\n\r\n"),pckHeader.capLen,dSize);
	m_TestData += str;

	// 显示
	dwStartTime = GetTickCount();
	for ( int i = 0; i < 4; i++ )
	{
		int idx = i;
		dSize =  m_vSav92Msg[idx].pSav92Msg->Decode(&pckData[idx*pckArrSize],pckHeader.capLen);

		if ( dSize != 0 )
		{
			m_vSav92Msg[idx].pSav92Msg->GetEthernetHeader(ethHdr);
			m_vSav92Msg[idx].pSav92Msg->Get8021QVlanInfo(vlanInfo);
			m_vSav92Msg[idx].pSav92Msg->GetIEC8802Info(iec8802Info);

			noASDU = m_vSav92Msg[idx].pSav92Msg->GetNoASDU();
			for ( uint16_t j = 0; j < noASDU ; j++ )
			{
				smpsNum = m_vSav92Msg[idx].pSav92Msg->GetASDUSmpsNum(j);
				m_vSav92Msg[idx].pSav92Msg->GetASDUPublicInfo(j,asduPbl[j]);
				m_vSav92Msg[idx].pSav92Msg->GetASDUSmps(j,smpVal[j],smpQlt[j],smpsNum);
			}

			m_vSav92Msg[idx].pSav92Msg->GetEthernetHeaderOff(ethOff);
			m_vSav92Msg[idx].pSav92Msg->Get8021QVlanInfoOff(vlanOff);
			m_vSav92Msg[idx].pSav92Msg->GetIEC8802InfoOff(iec8802Off);

			for ( uint16_t j = 0; j < noASDU ; j++ )
			{
				smpsNum = m_vSav92Msg[idx].pSav92Msg->GetASDUSmpsNum(j);
				m_vSav92Msg[idx].pSav92Msg->GetASDUPublicInfoOff(j,asduPblOff[j]);
				m_vSav92Msg[idx].pSav92Msg->GetASDUSmpsOff(j,seqDataOff[j],smpValOff[j],smpQltOff[j],smpsNum);
			}
		}

		DisplayEthHdr(ethHdr);
		DisplayVlanInfo(vlanInfo);
		DisplayIEC88023Pdu(iec8802Info);
		for ( uint16_t j = 0; j < noASDU ; j++ )
		{
			DisplaySav92AsduPbl(asduPbl[j]);
			DisplaySav92Smps(smpVal[j],smpQlt[j],smpsNum);
		}

// 		DisplayEthHdrOff(ethOff);
// 		DisplayVlanInfoOff(vlanOff);
// 		DisplayIEC88023PduOff(iec8802Off);
// 		for ( uint16_t j = 0; j < noASDU ; j++ )
// 		{
// 			DisplaySav92AsduPblOff(asduPblOff[j]);
// 			DisplayChannelOff(smpValOff[j],smpsNum);
// 		}
	}

	delete[] pckData;
	dwFinishTime = GetTickCount();
	str.Format(_T("DisplayTime: %d\r\n\r\n"),dwFinishTime-dwStartTime);
	m_TestData += str;

	str.Format(_T("Close File: %d\r\n\r\n"),m_PcapFile->Close());
	m_TestData += str;
	UpdateData(FALSE);
}

void CWinCETestDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	switch (nIDEvent)
	{
	case 1:
		{
//			Sav92DecTest();
			Sav92GetChannelDataTest();
		}
		break;
	case 2:
		{
			Sav92MulDecTest();
		}
		break;
	default:
		;
	}
//	CDialog::OnTimer(nIDEvent);
}


int CWinCETestDlg::GetSav92ChannelData(std::wstring &fileName, int beginFrameIdx, int frameNum, int channelIdx, uint32_t *channelDataArr, uint32_t arrSize)
{
	const int k_maxEtherLen = 1536;
	PCAP_PACKET_HEADER pckHeader;
	uint8_t pckData[k_maxEtherLen];

	const int k_maxSmpsNum = 30;
	uint32_t smpVal[k_maxSmpsNum];
	uint32_t smpQlt[k_maxSmpsNum];

	IPcapFile *pPcapFile = CreatePcapFileImpl();
	Sav92Msg  *pSav92Msg = CreateSav92MsgImpl();
	if ( pPcapFile->Open(fileName,PFM_READ) == PCAP_FILE_ERROR )
	{
		RETAILMSG(1,(_T("Open PcapFile Error!\r\n")));
		return 0;
	}

	uint32_t channelDataNum = 0;
	for ( int i = beginFrameIdx; i < frameNum; i++ )
	{
		// 取报文帧
		if ( pPcapFile->SeekPckToRead(i) == PCAP_FILE_ERROR )
		{
			RETAILMSG(1,(_T("Seek Pck Error!\r\n")));
			return 0;
		}
		if ( pPcapFile->ReadOnePckHeader(pckHeader) == PCAP_FILE_ERROR )
		{
			RETAILMSG(1,(_T("ReadOnePckHeader ERROR!!\r\n")));
			return 0;
		}
		if ( pPcapFile->ReadOnePckData(pckData,pckHeader.capLen) == PCAP_FILE_ERROR )
		{
			RETAILMSG(1,(_T("ReadOnePckData ERROR!!\r\n")));
			return 0;
		}
		
		// 解码
		size_t dSize =  pSav92Msg->Decode(pckData,pckHeader.capLen);

		// 取指定通道的值
		if ( dSize != 0 )
		{
			uint16_t noASDU = pSav92Msg->GetNoASDU();
			for ( uint16_t j = 0; j < noASDU ; j++ )
			{
				if ( channelDataNum >= arrSize )
				{
					break;
				}
				size_t smpsNum = pSav92Msg->GetASDUSmpsNum(j);
				if ( channelIdx >= smpsNum )
				{
					RETAILMSG(1,(_T("Channel Num Over!!\r\n")));
					return 0;
				}
				pSav92Msg->GetASDUSmps(j,smpVal,smpQlt,smpsNum);
				channelDataArr[channelDataNum] = smpVal[channelIdx];
				channelDataNum++;
			}
		}
		else
		{
			RETAILMSG(1,(_T("Decode 9-2Msg ERROR!!\r\n")));
			return 0;
		}
	}

	pPcapFile->Close();

	ReleasePcapFileImpl(pPcapFile);
	ReleaseSav92MsgImpl(pSav92Msg);

	return channelDataNum;
}

void CWinCETestDlg::OnBnClicked1phaseCtvtFt3C()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	CTORVT_FT3_ASDU_PUBLIC asduPbl = {
		0x01,0x10,0x1234,0x0000,0x0000,5
	};
	uint16_t smpVal[5] = {1,2,3,4,5};
	uint16_t msgData[ONEPHASE_CTORVT_FT3_MSG_WORD];

	m_TestData = _T("");

	// 设值
	dwStartTime = GetTickCount();
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		m_p1PhaseCtvtFt3->SetASDU(asduPbl,smpVal,5);
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;


	// 编码
	dwStartTime = GetTickCount();
	size_t cSize;
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		cSize = m_p1PhaseCtvtFt3->Code(msgData,ONEPHASE_CTORVT_FT3_MSG_WORD);
	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayCodeData(msgData,cSize);
	str.Format(_T("Code Size:%d Words\r\n\r\n"),cSize);
	m_TestData += str;
	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClicked1phaseCtvtFt3D()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	CTORVT_FT3_ASDU_PUBLIC asduPbl;
	CTORVT_FT3_ASDU_PUBLIC_OFFSET asduPblOff;
	size_t smpsNum = m_p1PhaseCtvtFt3->GetASDUSmpsNum();
	uint16_t smpVal[5];
	size_t   smpValOff[5];

	m_TestData = _T("");

	// 解码
	dwStartTime = GetTickCount();
	size_t dSize;
	for ( int i = 0; i<MSG_CODEC_COUNT; i++ )
	{
		dSize =  m_p1PhaseCtvtFt3->Decode(k_1PhaseCtvtFt3,ONEPHASE_CTORVT_FT3_MSG_WORD);
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;
	str.Format(_T("Src Size: %d\r\nDecode Size: %d\r\n\r\n"),ONEPHASE_CTORVT_FT3_MSG_WORD,dSize);
	m_TestData += str;

	// 取值
	dwStartTime = GetTickCount();
	if ( dSize == 0 )
	{
		m_TestData.Format(_T("Decode Fail!"));
	}
	else
	{
		str.Format(_T("ASDU Len:%d\r\n"),m_p1PhaseCtvtFt3->GetASDULen());
		m_TestData += str;
		str.Format(_T("ASDU Off:%d\r\n"),m_p1PhaseCtvtFt3->GetASDUOff());
		m_TestData += str;

		for ( int i=0; i<MSG_CODEC_COUNT;i++ )
		{
			m_p1PhaseCtvtFt3->GetASDUPublicInfo(asduPbl);
			m_p1PhaseCtvtFt3->GetASDUSmps(smpVal,smpsNum);

			m_p1PhaseCtvtFt3->GetASDUPublicInfoOff(asduPblOff);
			m_p1PhaseCtvtFt3->GetASDUSmpsOff(smpValOff,smpsNum);
		}

	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayCTorVTAsduPbl(asduPbl);
	DisplayUint16Smps(smpVal,smpsNum);

	DisplayCTorVTAsduPblOff(asduPblOff);
	DisplayChannelOff(smpValOff,smpsNum);

	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClicked3phaseCtFt3C()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	CTORVT_FT3_ASDU_PUBLIC asduPbl = {
		0x01,0x10,0x1234,0x0000,0x0000,5
	};
	uint16_t smpVal[9] = {1,2,3,4,5,6,7,8,9};
	uint16_t msgData[TRIPHASE_CT_FT3_MSG_WORD];

	m_TestData = _T("");

	// 设值
	dwStartTime = GetTickCount();
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		m_p3PhaseCtFt3->SetASDU(asduPbl,smpVal,9);
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;


	// 编码
	dwStartTime = GetTickCount();
	size_t cSize;
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		cSize = m_p3PhaseCtFt3->Code(msgData,TRIPHASE_CT_FT3_MSG_WORD);
	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayCodeData(msgData,cSize);
	str.Format(_T("Code Size:%d Words\r\n\r\n"),cSize);
	m_TestData += str;
	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClicked3phaseCtFt3D()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	CTORVT_FT3_ASDU_PUBLIC asduPbl;
	CTORVT_FT3_ASDU_PUBLIC_OFFSET asduPblOff;
	size_t smpsNum = m_p3PhaseCtFt3->GetASDUSmpsNum();
	uint16_t smpVal[9];
	size_t   smpValOff[9];

	m_TestData = _T("");

	// 解码
	dwStartTime = GetTickCount();
	size_t dSize;
	for ( int i = 0; i<MSG_CODEC_COUNT; i++ )
	{
		dSize =  m_p3PhaseCtFt3->Decode(k_3PhaseCtFt3,TRIPHASE_CT_FT3_MSG_WORD);
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;
	str.Format(_T("Src Size: %d\r\nDecode Size: %d\r\n\r\n"),TRIPHASE_CT_FT3_MSG_WORD,dSize);
	m_TestData += str;

	// 取值
	dwStartTime = GetTickCount();
	if ( dSize == 0 )
	{
		m_TestData.Format(_T("Decode Fail!"));
	}
	else
	{
		str.Format(_T("ASDU Len:%d\r\n"),m_p3PhaseCtFt3->GetASDULen());
		m_TestData += str;
		str.Format(_T("ASDU Off:%d\r\n"),m_p3PhaseCtFt3->GetASDUOff());
		m_TestData += str;

		for ( int i=0; i<MSG_CODEC_COUNT;i++ )
		{
			m_p3PhaseCtFt3->GetASDUPublicInfo(asduPbl);
			m_p3PhaseCtFt3->GetASDUSmps(smpVal,smpsNum);

			m_p3PhaseCtFt3->GetASDUPublicInfoOff(asduPblOff);
			m_p3PhaseCtFt3->GetASDUSmpsOff(smpValOff,smpsNum);
		}

	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayCTorVTAsduPbl(asduPbl);
	DisplayUint16Smps(smpVal,smpsNum);

	DisplayCTorVTAsduPblOff(asduPblOff);
	DisplayChannelOff(smpValOff,smpsNum);

	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClicked3phaseVt10Ft3C()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	CTORVT_FT3_ASDU_PUBLIC asduPbl = {
		0x01,0x10,0x1234,0x0000,0x0000,5
	};
	uint16_t smpVal[6] = {1,2,3,4,5,6};
	uint16_t msgData[TRIPHASE_VT_FT3_2010_MSG_WORD];

	m_TestData = _T("");

	// 设值
	dwStartTime = GetTickCount();
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		m_p3PhaseVt10Ft3->SetASDU(asduPbl,smpVal,3);
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;


	// 编码
	dwStartTime = GetTickCount();
	size_t cSize;
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		cSize = m_p3PhaseVt10Ft3->Code(msgData,TRIPHASE_VT_FT3_2010_MSG_WORD);
	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayCodeData(msgData,cSize);
	str.Format(_T("Code Size:%d Words\r\n\r\n"),cSize);
	m_TestData += str;
	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClicked3phaseVt10Ft3D()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	CTORVT_FT3_ASDU_PUBLIC asduPbl;
	CTORVT_FT3_ASDU_PUBLIC_OFFSET asduPblOff;
	size_t smpsNum = m_p3PhaseVt10Ft3->GetASDUSmpsNum();
	uint16_t smpVal[6];
	size_t   smpValOff[6];

	m_TestData = _T("");

	// 解码
	dwStartTime = GetTickCount();
	size_t dSize;
	for ( int i = 0; i<MSG_CODEC_COUNT; i++ )
	{
		dSize =  m_p3PhaseVt10Ft3->Decode(k_3PhaseVt10Ft3,TRIPHASE_VT_FT3_2010_MSG_WORD);
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;
	str.Format(_T("Src Size: %d\r\nDecode Size: %d\r\n\r\n"),TRIPHASE_VT_FT3_2010_MSG_WORD,dSize);
	m_TestData += str;

	// 取值
	dwStartTime = GetTickCount();
	if ( dSize == 0 )
	{
		m_TestData.Format(_T("Decode Fail!"));
	}
	else
	{
		str.Format(_T("ASDU Len:%d\r\n"),m_p3PhaseVt10Ft3->GetASDULen());
		m_TestData += str;
		str.Format(_T("ASDU Off:%d\r\n"),m_p3PhaseVt10Ft3->GetASDUOff());
		m_TestData += str;

		for ( int i=0; i<MSG_CODEC_COUNT;i++ )
		{
			m_p3PhaseVt10Ft3->GetASDUPublicInfo(asduPbl);
			m_p3PhaseVt10Ft3->GetASDUSmps(smpVal,smpsNum);

			m_p3PhaseVt10Ft3->GetASDUPublicInfoOff(asduPblOff);
			m_p3PhaseVt10Ft3->GetASDUSmpsOff(smpValOff,smpsNum);
		}

	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayCTorVTAsduPbl(asduPbl);
	DisplayUint16Smps(smpVal,smpsNum);

	DisplayCTorVTAsduPblOff(asduPblOff);
	DisplayChannelOff(smpValOff,smpsNum);

	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClicked3phaseVt11Ft3C()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	CTORVT_FT3_ASDU_PUBLIC asduPbl = {
		0x01,0x10,0x1234,0x0000,0x0000,5
	};
	uint16_t smpVal[6] = {1,2,3,4,5,6};
	uint16_t msgData[TRIPHASE_VT_FT3_2011_MSG_WORD];

	m_TestData = _T("");

	// 设值
	dwStartTime = GetTickCount();
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		m_p3PhaseVt11Ft3->SetASDU(asduPbl,smpVal,6);
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;


	// 编码
	dwStartTime = GetTickCount();
	size_t cSize;
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		cSize = m_p3PhaseVt11Ft3->Code(msgData,TRIPHASE_VT_FT3_2011_MSG_WORD);
	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayCodeData(msgData,cSize);
	str.Format(_T("Code Size:%d Words\r\n\r\n"),cSize);
	m_TestData += str;
	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClicked3phaseVt11Ft3D()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	CTORVT_FT3_ASDU_PUBLIC asduPbl;
	CTORVT_FT3_ASDU_PUBLIC_OFFSET asduPblOff;
	size_t smpsNum = m_p3PhaseVt11Ft3->GetASDUSmpsNum();
	uint16_t smpVal[6];
	size_t   smpValOff[6];

	m_TestData = _T("");

	// 解码
	dwStartTime = GetTickCount();
	size_t dSize;
	for ( int i = 0; i<MSG_CODEC_COUNT; i++ )
	{
		dSize =  m_p3PhaseVt11Ft3->Decode(k_3PhaseVt11Ft3,TRIPHASE_VT_FT3_2011_MSG_WORD);
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;
	str.Format(_T("Src Size: %d\r\nDecode Size: %d\r\n\r\n"),TRIPHASE_VT_FT3_2011_MSG_WORD,dSize);
	m_TestData += str;

	// 取值
	dwStartTime = GetTickCount();
	if ( dSize == 0 )
	{
		m_TestData.Format(_T("Decode Fail!"));
	}
	else
	{
		str.Format(_T("ASDU Len:%d\r\n"),m_p3PhaseVt11Ft3->GetASDULen());
		m_TestData += str;
		str.Format(_T("ASDU Off:%d\r\n"),m_p3PhaseVt11Ft3->GetASDUOff());
		m_TestData += str;

		for ( int i=0; i<MSG_CODEC_COUNT;i++ )
		{
			m_p3PhaseVt11Ft3->GetASDUPublicInfo(asduPbl);
			m_p3PhaseVt11Ft3->GetASDUSmps(smpVal,smpsNum);

			m_p3PhaseVt11Ft3->GetASDUPublicInfoOff(asduPblOff);
			m_p3PhaseVt11Ft3->GetASDUSmpsOff(smpValOff,smpsNum);
		}

	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayCTorVTAsduPbl(asduPbl);
	DisplayUint16Smps(smpVal,smpsNum);

	DisplayCTorVTAsduPblOff(asduPblOff);
	DisplayChannelOff(smpValOff,smpsNum);

	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClicked3phaseCtvtFt3C()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	CTORVT_FT3_ASDU_PUBLIC asduPbl = {
		0x01,0x10,0x1234,0x0000,0x0000,5
	};
	uint16_t smpVal[15] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	uint16_t msgData[TRIPHASE_CTORVT_FT3_MSG_WORD];

	m_TestData = _T("");

	// 设值
	dwStartTime = GetTickCount();
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		m_p3PhaseCtvtFt3->SetASDU(asduPbl,smpVal,15);
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;


	// 编码
	dwStartTime = GetTickCount();
	size_t cSize;
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		cSize = m_p3PhaseCtvtFt3->Code(msgData,TRIPHASE_CTORVT_FT3_MSG_WORD);
	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayCodeData(msgData,cSize);
	str.Format(_T("Code Size:%d Words\r\n\r\n"),cSize);
	m_TestData += str;
	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClicked3phaseCtvtFt3D()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	CTORVT_FT3_ASDU_PUBLIC asduPbl;
	CTORVT_FT3_ASDU_PUBLIC_OFFSET asduPblOff;
	size_t smpsNum = m_p3PhaseCtvtFt3->GetASDUSmpsNum();
	uint16_t smpVal[15];
	size_t   smpValOff[15];

	m_TestData = _T("");

	// 解码
	dwStartTime = GetTickCount();
	size_t dSize;
	for ( int i = 0; i<MSG_CODEC_COUNT; i++ )
	{
		dSize =  m_p3PhaseCtvtFt3->Decode(k_3PhaseCtvtFt3,TRIPHASE_CTORVT_FT3_MSG_WORD);
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;
	str.Format(_T("Src Size: %d\r\nDecode Size: %d\r\n\r\n"),TRIPHASE_CTORVT_FT3_MSG_WORD,dSize);
	m_TestData += str;

	// 取值
	dwStartTime = GetTickCount();
	if ( dSize == 0 )
	{
		m_TestData.Format(_T("Decode Fail!"));
	}
	else
	{
		str.Format(_T("ASDU Len:%d\r\n"),m_p3PhaseCtvtFt3->GetASDULen());
		m_TestData += str;
		str.Format(_T("ASDU Off:%d\r\n"),m_p3PhaseCtvtFt3->GetASDUOff());
		m_TestData += str;

		for ( int i=0; i<MSG_CODEC_COUNT;i++ )
		{
			m_p3PhaseCtvtFt3->GetASDUPublicInfo(asduPbl);
			m_p3PhaseCtvtFt3->GetASDUSmps(smpVal,smpsNum);

			m_p3PhaseCtvtFt3->GetASDUPublicInfoOff(asduPblOff);
			m_p3PhaseCtvtFt3->GetASDUSmpsOff(smpValOff,smpsNum);
		}

	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayCTorVTAsduPbl(asduPbl);
	DisplayUint16Smps(smpVal,smpsNum);

	DisplayCTorVTAsduPblOff(asduPblOff);
	DisplayChannelOff(smpValOff,smpsNum);

	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedStd52Ft3C()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	STD52_FT3_ASDU_PUBLIC asduPbl = {
		0x003E,0x02,0x01,0x4042,3000,3000,1100,750,
		0x0000,0x0000,3357
	};
	uint16_t smpVal[12] = {1,2,3,4,5,6,7,8,9,10,11,12};
	uint16_t msgData[STD52_FT3_MSG_WORD];

	m_TestData = _T("");

	// 设值
	dwStartTime = GetTickCount();
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		m_pStd52FT3Msg->SetASDU(asduPbl,smpVal,15);
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;


	// 编码
	dwStartTime = GetTickCount();
	size_t cSize;
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		cSize = m_pStd52FT3Msg->Code(msgData,STD52_FT3_MSG_WORD);
	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayCodeData(msgData,cSize);
	str.Format(_T("Code Size:%d Words\r\n\r\n"),cSize);
	m_TestData += str;
	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedStd52Ft3D()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	STD52_FT3_ASDU_PUBLIC asduPbl;
	STD52_FT3_ASDU_PUBLIC_OFFSET asduPblOff;
	size_t smpsNum = m_pStd52FT3Msg->GetASDUSmpsNum();
	uint16_t smpVal[15];
	size_t   smpValOff[15];

	m_TestData = _T("");

	// 解码
	dwStartTime = GetTickCount();
	size_t dSize;
	for ( int i = 0; i<MSG_CODEC_COUNT; i++ )
	{
		dSize =  m_pStd52FT3Msg->Decode(k_Std52Ft3,STD52_FT3_MSG_WORD-6);
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;
	str.Format(_T("Src Size: %d\r\nDecode Size: %d\r\n\r\n"),STD52_FT3_MSG_WORD,dSize);
	m_TestData += str;

	// 取值
	dwStartTime = GetTickCount();
	if ( dSize == 0 )
	{
		m_TestData.Format(_T("Decode Fail!"));
	}
	else
	{
		str.Format(_T("ASDU Len:%d\r\n"),m_pStd52FT3Msg->GetASDULen());
		m_TestData += str;
		str.Format(_T("ASDU Off:%d\r\n"),m_pStd52FT3Msg->GetASDUOff());
		m_TestData += str;

		for ( int i=0; i<MSG_CODEC_COUNT;i++ )
		{
			m_pStd52FT3Msg->GetASDUPublicInfo(asduPbl);
			m_pStd52FT3Msg->GetASDUSmps(smpVal,smpsNum);

			m_pStd52FT3Msg->GetASDUPublicInfoOff(asduPblOff);
			m_pStd52FT3Msg->GetASDUSmpsOff(smpValOff,smpsNum);
		}

	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayStd52AsduPbl(asduPbl);
	DisplayUint16Smps(smpVal,smpsNum);

	DisplayStd52AsduPblOff(asduPblOff);
	DisplayChannelOff(smpValOff,smpsNum);

	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedSav91ExC()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	uint8_t   desMac[] = {0x01,0x0C,0xCD,0x04,0x09,0x06};
	uint8_t   srcMac[] = {0x00,0x30,0x18,0xA1,0x5B,0xED};
	uint8_t   usrPrio = 4;
	uint16_t  vlanID  = 0;
	uint16_t  appID   = 0x0906;
	SAV91_EX_ASDU_PUBLIC asduPbl = {
		0x002C,0x02,0xFE,0x0000,0x0057,0x004D,13361,80,0x57
	};
	uint16_t smpVal[12] = {1,2,3,4,5,6,7,8,9,10,11,12};
	uint8_t  msgData[ETHER_FRAME_MAX_LEN];

	m_TestData = _T("");

	// 设值
	dwStartTime = GetTickCount();
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		m_pSav91ExMsg->ClearSavPDU();
		m_pSav91ExMsg->SetMacAddr(desMac,srcMac,ETHER_MAC_LEN);
		m_pSav91ExMsg->Set8021QVlanInfo(usrPrio,vlanID);
		m_pSav91ExMsg->SetAppID(appID);
		for ( int i=0; i<2; i++ )
		{
			if ( m_pSav91ExMsg->AddAnASDU(asduPbl,smpVal,12) == false )
			{
				str.Format(_T("Add ASDU%d fail\r\n"),i+1);
				m_TestData += str;
				break;
			}
		}
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;


	// 编码
	dwStartTime = GetTickCount();
	size_t cSize;
	for ( int j = 0 ; j<MSG_CODEC_COUNT; j++ )
	{
		cSize = m_pSav91ExMsg->Code(msgData,sizeof(msgData));
	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayCodeData(msgData,cSize);
	str.Format(_T("Code Size:%d\r\n\r\n"),cSize);
	m_TestData += str;
	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedSav91ExD()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	ETHER_II_HEADER ethHdr;
	ETHER_II_HEADER_OFFSET ethOff;
	IEEE802_1Q_VLAN_INFO vlanInfo;
	IEEE802_1Q_VLAN_INFO_OFFSET vlanOff;
	IEC8802_3_PDU_PUBLIC iec8802Info;
	IEC8802_3_PDU_PUBLIC_OFFSET iec8802Off;
	const uint16_t maxNoASDU = 4;
	SAV91_EX_ASDU_PUBLIC asduPbl[maxNoASDU];
	SAV91_EX_ASDU_PUBLIC_OFFSET asduPblOff[maxNoASDU];
	uint16_t noASDU;
	size_t smpsNum;
	uint16_t smpVal[maxNoASDU][12];
	size_t   smpValOff[maxNoASDU][12];

	m_TestData = _T("");

	// 解码
	dwStartTime = GetTickCount();
	size_t dSize;
	for ( int i = 0; i<MSG_CODEC_COUNT; i++ )
	{
		dSize = m_pSav91ExMsg->Decode(k_Sav91Ex,/*sizeof(k_Sav91)*/121212121);
	}
	dwFinishTime = GetTickCount();
	m_DecTime = dwFinishTime-dwStartTime;

	// 取值
	str.Format(_T("Src Size: %d\r\nDecode Size: %d\r\n\r\n"),sizeof(k_Sav91Ex),dSize);
	m_TestData += str;
	dwStartTime = GetTickCount();
	if ( dSize == 0 )
	{
		m_TestData.Format(_T("Decode Fail!"));
	}
	else
	{
		for ( int i = 0; i<MSG_CODEC_COUNT; i++ )
		{
			m_pSav91ExMsg->GetEthernetHeader(ethHdr);
			m_pSav91ExMsg->Get8021QVlanInfo(vlanInfo);
			m_pSav91ExMsg->GetIEC8802Info(iec8802Info);

			noASDU = m_pSav91ExMsg->GetNoASDU();

			for ( uint16_t i = 0; i < noASDU ; i++ )
			{
				smpsNum = m_pSav91ExMsg->GetASDUSmpsNum();
				m_pSav91ExMsg->GetASDUPublicInfo(i,asduPbl[i]);
				m_pSav91ExMsg->GetASDUSmps(i,smpVal[i],smpsNum);
			}

			m_pSav91ExMsg->GetEthernetHeaderOff(ethOff);
			m_pSav91ExMsg->Get8021QVlanInfoOff(vlanOff);
			m_pSav91ExMsg->GetIEC8802InfoOff(iec8802Off);

			for ( uint16_t i = 0; i < noASDU ; i++ )
			{
				m_pSav91ExMsg->GetASDUPublicInfoOff(i,asduPblOff[i]);
				m_pSav91ExMsg->GetASDUSmpsOff(i,smpValOff[i],smpsNum);
			}
		}
	}
	dwFinishTime = GetTickCount();
	m_CodeTime = dwFinishTime-dwStartTime;

	DisplayEthHdr(ethHdr);
	DisplayVlanInfo(vlanInfo);
	DisplayIEC88023Pdu(iec8802Info);
	for ( uint16_t i = 0; i < noASDU ; i++ )
	{
		DisplaySav91ExAsduPbl(asduPbl[i]);
		DisplayUint16Smps(smpVal[i],smpsNum);
	}

	DisplayEthHdrOff(ethOff);
	DisplayVlanInfoOff(vlanOff);
	DisplayIEC88023PduOff(iec8802Off);
	for ( uint16_t i = 0; i < noASDU ; i++ )
	{
		DisplaySav91ExAsduPblOff(asduPblOff[i]);
		DisplayChannelOff(smpValOff[i],smpsNum);
	}

	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedStatusWordC()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	Ft3FrameStatusWord statusWord;
	m_TestData = _T("");

	str.Format(_T("\r\n标准FT3：\r\n"));
	m_TestData += str;
	statusWord.SetFt3FrameType(FT3_FRAME_TYPE_MU_STD);
	statusWord.SetMaintenanceRequired(true);
	statusWord.SetTestMode(true);
	statusWord.SetMuWakingUp(true);
	statusWord.SetMuSyncMethod(false);
	statusWord.SetMuSync(false);
	statusWord.SetCtOutputType(false);
	statusWord.SetRangeFlag(false);
	for ( int i=0; i<22; i++ )
	{
		statusWord.SetChannelValidity(i,false);
	}
	for ( int i=0; i<13; i++ )
	{
		statusWord.SetFutureUsedBitVal(i,true);
	}
	DisplayStatusWord(statusWord);

	str.Format(_T("\r\n扩展FT3：\r\n"));
	m_TestData += str;
	statusWord.SetFt3FrameType(FT3_FRAME_TYPE_MU_EX);
	statusWord.SetMaintenanceRequired(false);
	statusWord.SetTestMode(false);
	statusWord.SetCtorVtFiring(false);
	statusWord.SetCtorVtSyncMethod(true);
	statusWord.SetCtorVtSync(true);
	statusWord.SetCtOutputType(true);
	statusWord.SetRangeFlag(true);
	for ( int i=0; i<22; i++ )
	{
		statusWord.SetChannelValidity(i,true);
	}
	for ( int i=0; i<13; i++ )
	{
		statusWord.SetFutureUsedBitVal(i,false);
	}
	DisplayStatusWord(statusWord);

	str.Format(_T("\r\n互感器FT3：\r\n"));
	m_TestData += str;
	statusWord.SetFt3FrameType(FT3_FRAME_TYPE_CTORVT);
	statusWord.SetMaintenanceRequired(false);
	statusWord.SetTestMode(false);
	statusWord.SetCtorVtFiring(false);
	statusWord.SetCtorVtSyncMethod(true);
	statusWord.SetCtorVtSync(true);
	statusWord.SetMuWakingUp(true);
	statusWord.SetMuSyncMethod(false);
	statusWord.SetMuSync(false);
	statusWord.SetCtOutputType(true);
	statusWord.SetRangeFlag(true);
	statusWord.SetCtorVtAdError(false);
	statusWord.SetCtorVtPower1stAbnor(false);
	statusWord.SetCtorVtPower2ndAbnor(false);
	statusWord.SetCtorVtHvpsValidity(false);
//	statusWord.SetCtorVtLaserPowerStatus(FT3CLPS_MAINTAIN_OUTPUT);
// 	statusWord.SetCtorVtLaserPowerStatus(FT3CLPS_DOWN_TO_MAINTAIN_POWER);
// 	statusWord.SetCtorVtLaserPowerStatus(FT3CLPS_DECREASE_OUTPUT_POWER);
// 	statusWord.SetCtorVtLaserPowerStatus(FT3CLPS_INCREASE_OUTPUT_POWER);
 	statusWord.SetCtorVtLaserPowerStatus(FT3CLPS_SPEED_INCREASE_OUTPUT_POWER);
//	statusWord.SetCtorVtLaserPowerStatus(FT3CLPS_FAILURE);
	for ( int i=0; i<22; i++ )
	{
		statusWord.SetChannelValidity(i,false);
	}
	for ( int i=0; i<13; i++ )
	{
		statusWord.SetFutureUsedBitVal(i,true);
	}
	DisplayStatusWord(statusWord);

	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedStatusWordD()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	Ft3FrameStatusWord statusWord;
	m_TestData = _T("");

	str.Format(_T("\r\n标准FT3：\r\n"));
	m_TestData += str;
	statusWord.SetFt3FrameType(FT3_FRAME_TYPE_MU_STD);
	DisplayStatusWord(statusWord);

	str.Format(_T("\r\n扩展FT3：\r\n"));
	m_TestData += str;
	statusWord.SetFt3FrameType(FT3_FRAME_TYPE_MU_EX);
	DisplayStatusWord(statusWord);

	str.Format(_T("\r\n互感器FT3：\r\n"));
	m_TestData += str;
	statusWord.SetFt3FrameType(FT3_FRAME_TYPE_CTORVT);
	DisplayStatusWord(statusWord);

	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedTimeQualityC()
{
	// TODO: 在此添加控件通知处理程序代码
	m_TestData = _T("");

	CString str;
	Iec61850TimeQuality tq;
	tq.SetLeapSecondKnown(true);
	tq.SetClockFailure(true);
	tq.SetClockSync(true);
	tq.SetSecondAccuracy(5);
	DisplayTimeQuality(tq);
	uint8_t nTq = tq.GetTimeQuality();
	str.Format(_T("\r\Iec61850TimeQuality：%x\r\n"),nTq);
	m_TestData += str;

	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedTimeQualityD()
{
	// TODO: 在此添加控件通知处理程序代码
	m_TestData = _T("");

	
	Iec61850TimeQuality tq(0x2A);
	DisplayTimeQuality(tq);

	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedQualityC()
{
	// TODO: 在此添加控件通知处理程序代码
	m_TestData = _T("");

	CString str;
	Iec61850SmvQuality sq;
	Iec61850GooseQuality gq;

	str.Format(_T("\r\nIec61850SmvQuality：\r\n"));
	m_TestData += str;
	sq.SetValidity(IEC61850_QV_INVALID);
	sq.SetOverflow(true);
	sq.SetOutofRange(true);
	sq.SetBadReference(true);
	sq.SetOscillatory(true);
	sq.SetFailure(true);
	sq.SetOldData(true);
	sq.SetInconsistent(true);
	sq.SetInaccurate(true);
	sq.SetSourceType(IEC61850_QST_SUBSTITUTED);
	sq.SetTest(true);
	sq.SetOperatorBlocked(true);
	sq.SetDerived(true);
	DisplaySmvQuality(sq);
	uint32_t nSq = sq.GetQuality();
	str.Format(_T("\r\nIec61850SmvQuality：%x\r\n"),nSq);
	m_TestData += str;

	str.Format(_T("\r\nIec61850GooseQuality：\r\n"));
	m_TestData += str;
	gq.SetValidity(IEC61850_QV_INVALID);
	gq.SetBadReference(true);
	gq.SetInaccurate(true);
	gq.SetTest(true);
	DisplayGooseQuality(gq);
	uint16_t nGq = gq.GetQuality();
	str.Format(_T("\r\nIec61850GooseQuality：%x\r\n"),nGq);
	m_TestData += str;

	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedQualityD()
{
	// TODO: 在此添加控件通知处理程序代码
	m_TestData = _T("");
	CString str;
	Iec61850SmvQuality sq(0x00006C9B);
	Iec61850GooseQuality gq(0x0055);

	str.Format(_T("\r\nIec61850SmvQuality：\r\n"));
	m_TestData += str;
	DisplaySmvQuality(sq);

	str.Format(_T("\r\nIec61850GooseQuality：\r\n"));
	m_TestData += str;
	DisplayGooseQuality(gq);

	UpdateData(FALSE);
}

void CWinCETestDlg::OnBnClickedIeee1588D()
{
	// TODO: 在此添加控件通知处理程序代码
#ifdef CHSINT_SDK
	std::wstring fileName(_T("\\Storage Card\\数字化变电站分析仪资料\\其他资料\\pcap\\报文\\IEEE1588.pcap"));
#else
	std::wstring fileName(_T("\\FlashDisk\\IEEE1588.pcap"));
#endif
	PCAP_PACKET_HEADER pckHeader;
	CString str;
	DWORD dwStartTime;
	DWORD dwFinishTime;
	uint8_t *pckData = NULL;
	m_TestData = _T("");

	ETHER_II_HEADER ethHdr;
	ETHER_II_HEADER_OFFSET ethOff;
	IEEE1588_HEADER ieee1588Hdr;
	IEEE1588_HEADER_OFFSET ieee1588HdrOff;

	if ( m_PcapFile->Open(fileName,PFM_READ) == PCAP_FILE_ERROR )
	{
		str.Format(_T("Open Write Error!\r\n\r\n"));
		m_TestData += str;
	}

	static int pckOff = 0;
	if ( m_PcapFile->SeekPckToRead(pckOff) == PCAP_FILE_ERROR )
	{
		str.Format(_T("Seek Pck Error!\r\n\r\n"));
		m_TestData += str;
	}
	if ( m_PcapFile->ReadOnePckHeader(pckHeader) == PCAP_FILE_ERROR )
	{
		str.Format(_T("ReadOnePckHeader ERROR!!\r\n\r\n"));
		m_TestData += str;
	}
	if ( pckData == NULL )
	{
		pckData = new uint8_t[pckHeader.capLen];
	}
	if ( m_PcapFile->ReadOnePckData(pckData,pckHeader.capLen) == PCAP_FILE_ERROR )
	{
		str.Format(_T("ReadOnePckData ERROR!!\r\n\r\n"));
		m_TestData += str;
	}
	str.Format(_T("Close File: %d\r\n\r\n"),m_PcapFile->Close());
	m_TestData += str;
	pckOff = (++pckOff)%14;

	size_t dSize;
	m_DecTime = 0;
	m_CodeTime = 0;
	dwStartTime = GetTickCount();

	for ( int i = 0; i < MSG_CODEC_COUNT; i++ )
	{
		// 解码
//		dwStartTime = GetTickCount();
		dSize =  m_pIeee1588Msg->Decode(pckData,pckHeader.capLen);
// 		dwFinishTime = GetTickCount();
// 		m_DecTime += dwFinishTime-dwStartTime;

		// 取值
//		dwStartTime = GetTickCount();
		if ( dSize != 0 )
		{
			m_pIeee1588Msg->GetEthernetHeader(ethHdr);
			m_pIeee1588Msg->GetIeee1588Header(ieee1588Hdr);
			m_pIeee1588Msg->GetEthernetHeaderOff(ethOff);
			m_pIeee1588Msg->GetIeee1588HeaderOff(ieee1588HdrOff);
		}
// 		dwFinishTime = GetTickCount();
// 		m_CodeTime += dwFinishTime-dwStartTime;
	}
	dwFinishTime = GetTickCount();
	m_CodeTime   = dwFinishTime-dwStartTime;

	DisplayIeee1588Header(ieee1588Hdr);
	DisplayIeee1588HeaderFlags(ieee1588Hdr.flags);

	RETAILMSG(1,(_T("Decode Time:%d\r\nGetVal Time:%d\r\n\r\n"),m_DecTime,m_CodeTime));

	str.Format(_T("Src Size: %d\r\nDecode Size: %d\r\n\r\n"),pckHeader.capLen,dSize);
	m_TestData += str;

	delete[] pckData;

	UpdateData(FALSE);
}
