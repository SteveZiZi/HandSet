// WinCETestDlg.h : 头文件
//

#pragma once

#include "../../src/utils/PcapFile/include/pcap_file_i.h"
//#include "src/utils/PcapFile/include/pcap_file_i.h"
#include "src/utils/MessageCodec/include/sav91_msg.h"
#include "src/utils/MessageCodec/include/sav91_ex_msg.h"
#include "src/utils/MessageCodec/include/sav92_msg.h"
#include "src/utils/MessageCodec/include/goose_msg.h"
#include "src/utils/MessageCodec/include/nanrui_ft3_msg.h"
#include "src/utils/MessageCodec/include/guowang_ft3_msg.h"
#include "src/utils/MessageCodec/include/std52_ft3_msg.h"
#include "src/utils/MessageCodec/include/ctorvt_ft3_msg.h"
#include "src/utils/MessageCodec/include/iec61850_data_struct.h"
#include "src/utils/MessageCodec/include/ieee1588_msg.h"
#include "src/utils/MessageCodec/msg_seg/bswap.h"
#include "src/utils/SmvScaler/SmvScaler.h"

#include <vector>

// CWinCETestDlg 对话框
class CWinCETestDlg : public CDialog
{
// 构造
public:
	CWinCETestDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CWinCETestDlg();

// 对话框数据
	enum { IDD = IDD_WINCETEST_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()
public:
	void DisplayEthHdr(ETHER_II_HEADER &ethhdr);
	void DisplayEthHdrOff(ETHER_II_HEADER_OFFSET &offset);
	void DisplayCodeData(uint8_t *inData, size_t datLen);
	void DisplayVlanInfo(IEEE802_1Q_VLAN_INFO &info);
	void DisplayVlanInfoOff(IEEE802_1Q_VLAN_INFO_OFFSET &offset);
	void DisplayIEC88023Pdu(IEC8802_3_PDU_PUBLIC &info);
	void DisplayIEC88023PduOff(IEC8802_3_PDU_PUBLIC_OFFSET &offset);
	void DisplayChannelOff(size_t *channelOff,size_t channelNum);

	void DisplaySav91AsduPbl(SAV91_ASDU_PUBLIC &asduPbl);
	void DisplaySav91AsduPblOff(SAV91_ASDU_PUBLIC_OFFSET &offset);
	void DisplayUint16Smps(uint16_t *smpVal, size_t smpsNum);

	void DisplaySav91ExAsduPbl(SAV91_EX_ASDU_PUBLIC &asduPbl);
	void DisplaySav91ExAsduPblOff(SAV91_EX_ASDU_PUBLIC_OFFSET &offset);

	void DisplaySav92AsduPbl(SAV92_ASDU_PUBLIC &asduPbl);
	void DisplaySav92AsduPblOff(SAV92_ASDU_PUBLIC_OFFSET &offset);
	void DisplaySav92Smps(uint32_t *smpVal, uint32_t *smpQlt, size_t smpsNum);

	void DisplayGoosePduPbl(GOOSE_PDU_PUBLIC &pduPbl);
	void DisplayGoosePduPblOff(GOOSE_PDU_PUBLIC_OFFSET &offset);
	void DisplayGooseData(char *str);

	void DisplayNanRuiAsduPbl(NANRUI_FT3_ASDU_PUBLIC &asduPbl);
	void DisplayNanRuiAsduPblOff(NANRUI_FT3_ASDU_PUBLIC_OFFSET &offset);

	void DisplayGuoWangAsduPbl(GUOWANG_FT3_ASDU_PUBLIC &asduPbl);
	void DisplayGuoWangAsduPblOff(GUOWANG_FT3_ASDU_PUBLIC_OFFSET &offset);

	void DisplayStd52AsduPbl(STD52_FT3_ASDU_PUBLIC &asduPbl);
	void DisplayStd52AsduPblOff(STD52_FT3_ASDU_PUBLIC_OFFSET &offset);

	void DisplayCTorVTAsduPbl(CTORVT_FT3_ASDU_PUBLIC &asduPbl);
	void DisplayCTorVTAsduPblOff(CTORVT_FT3_ASDU_PUBLIC_OFFSET &offset);

	void DisplayCodeData(uint16_t *inData, size_t datLen);
	int  ChannelDataToStr(const GOOSE_DATA *datVal, char *outStr, const size_t strSize);

	void DisplayStatusWord(Ft3FrameStatusWord &statusWord);

	void DisplayTimeQuality(Iec61850TimeQuality &tq);

	void DisplaySmvQuality(Iec61850SmvQuality &sq);
	void DisplayGooseQuality(Iec61850GooseQuality &gq);

	void DisplayIeee1588Header(IEEE1588_HEADER &hdr);
	void DisplayIeee1588HeaderFlags(IEEE1588_HEADER_FLAGS &flags);
private:
	IPcapFile *m_PcapFile;
	CString m_TestData;
	uint32_t m_CodeTime;
	uint32_t m_DecTime;
	Sav91Msg *m_pSav91Msg;
	Sav92Msg *m_pSav92Msg;
	GooseMsg *m_pGooseMsg;
	NanRuiFT3Msg *m_pNanRuiFT3Msg;
	GuoWangFT3Msg *m_pGuoWangFT3Msg;
	Std52FT3Msg  *m_pStd52FT3Msg;
	CTorVTFT3Msg *m_p1PhaseCtvtFt3;
	CTorVTFT3Msg *m_p3PhaseCtFt3;
	CTorVTFT3Msg *m_p3PhaseVt10Ft3;
	CTorVTFT3Msg *m_p3PhaseVt11Ft3;
	CTorVTFT3Msg *m_p3PhaseCtvtFt3;
	Sav91ExMsg   *m_pSav91ExMsg;
	Ieee1588Msg  *m_pIeee1588Msg;


	typedef struct
	{
		uint16_t appID;
		Sav92Msg *pSav92Msg;
	}SAV92_MSG_OBJ;
	std::vector <SAV92_MSG_OBJ> m_vSav92Msg;
public:
	afx_msg void OnBnClickedSav91Code();
	afx_msg void OnBnClickedSav91Dec();
	afx_msg void OnBnClickedSav92Code();
	afx_msg void OnBnClickedSav92Dec();
	afx_msg void OnBnClickedGooseCode();
	afx_msg void OnBnClickedGooseDec();
	afx_msg void OnBnClickedNanRuiCode();
	afx_msg void OnBnClickedNanRuiDec();
	afx_msg void OnBnClickedGuoWangCode();
	afx_msg void OnBnClickedGuoWangDec();
	afx_msg void OnBnClickedRapidDecode();
	afx_msg void OnBnClickedBtn12();
	afx_msg void OnBnClickedSav92MulDec();
	afx_msg void OnBnClicked1phaseCtvtFt3C();
	afx_msg void OnBnClicked1phaseCtvtFt3D();
	afx_msg void OnBnClicked3phaseCtFt3C();
	afx_msg void OnBnClicked3phaseCtFt3D();
	afx_msg void OnBnClicked3phaseVt10Ft3C();
	afx_msg void OnBnClicked3phaseVt10Ft3D();
	afx_msg void OnBnClicked3phaseVt11Ft3C();
	afx_msg void OnBnClicked3phaseVt11Ft3D();
	afx_msg void OnBnClicked3phaseCtvtFt3C();
	afx_msg void OnBnClicked3phaseCtvtFt3D();
	afx_msg void OnBnClickedStd52Ft3C();
	afx_msg void OnBnClickedStd52Ft3D();
	afx_msg void OnBnClickedSav91ExC();
	afx_msg void OnBnClickedSav91ExD();
	afx_msg void OnBnClickedStatusWordC();
	afx_msg void OnBnClickedStatusWordD();
	afx_msg void OnBnClickedTimeQualityC();
	afx_msg void OnBnClickedTimeQualityD();
	afx_msg void OnBnClickedQualityC();
	afx_msg void OnBnClickedQualityD();
	afx_msg void OnBnClickedIeee1588D();

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void Sav92DecTest();
	void Sav92MulDecTest();
	void Sav92GetChannelDataTest();

	///
	/// @brief
	///     获取连续9-2报文帧的指定通道值
	///
	/// @param[in]
	///     fileName       - Pcap文件名
	/// @param[in]
	///     beginFrameIdx  - Pcap文件中连续报文帧的起始索引号（从0开始）
	/// @param[in]
	///     frameNum       - 连续报文帧的数目
	/// @param[in]
	///     channelIdx     - 通道索引号（从0开始）
	/// @param[in]
	///     channelDataArr - 保存通道数据的数组
	/// @param[in]
	///     arrSize        - 数组大小
	///
	/// @return
	///     通道值的实际数目
	///
	int GetSav92ChannelData(std::wstring &fileName, int beginFrameIdx, int frameNum, int channelIdx, uint32_t *channelDataArr, uint32_t arrSize);
};
