/**
 * \file
 *      PcapBrowserDlg.h
 *
 * \brief
 *      pcap�ļ������ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/11/13
 */

#pragma once

#include "vector"
#include "src/main/common.h"
#include "src/main/resource.h"
#include "src/main/dlg/basemodlg.h"

#include "src/service/datalayer/dsmdatalayer.h"

#include "src/utils/electriccontrol/horhistogramctrl/horhistogramctrl.h"
#include "src/utils/electriccontrol/waveformgramctrl/waveformgramctrl.h"

class IPcapFile;
class CNaviToolBar;
class CRNaviMenu;
class CMsgDlg;
class CDsmDataLayer;
class CProgressBarDlg;
class CAppidSampleMapping;

class Sav92Msg;
class Sav91Msg;
class GooseMsg;
class NanRuiFT3Msg;
class GuoWangFT3Msg;


#define DSM_PCAP_WAVEFORM_GRAM_X         1           ///< x�Შ����
#define DSM_PCAP_WAVEFORM_GRAM_Y         3           ///< y�Შ����

#define DSM_PCAP_WAVEFORM_NUMS           DSM_PCAP_WAVEFORM_GRAM_X * DSM_PCAP_WAVEFORM_GRAM_Y


// CPcapBrowserDlg �Ի���

class CPcapBrowserDlg : public DSM_BASEMODLG
{
	DECLARE_DYNAMIC(CPcapBrowserDlg)

public:
	CPcapBrowserDlg(const CString csFile = L"", CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPcapBrowserDlg();

// �Ի�������
	enum { IDD = IDD_PCAP_BROWSER_DLG };

private:
    enum ModePage
    {
        MODE_FRAME_LIST,            ///< ����֡�б�ģʽ
        MODE_FRAME_DETAIL,          ///< ����֡��ϸ��Ϣģʽ
        MODE_FRAME_WAVE,            ///< ���η���ģʽ
        MODE_FRAME_STATISTICS,      ///< ����ͳ��ģʽ
#if !SU_FEI
		MODE_FRAME_HISTOGRAM,		///< ������ɢ��ģʽ
#endif	

    };

    enum ModeWave
    {
        MODE_WAVE_ZOOM,             ///< ����
        MODE_WAVE_WATCH,            ///< �ۿ�
    };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()


protected:
    virtual void OnOK() {}
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

public:
    afx_msg void OnPaint();
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnF2TF4(UINT nID);

#if !CHUN_HUA
	afx_msg void OnDetailF1F2(UINT nID);

	
#endif

#if SU_FEI
    afx_msg void OnNextPreviousException(UINT nID);
#else
	afx_msg void OnMenuF6();
#endif
    afx_msg LRESULT OnLoadPcapComplete(WPARAM, LPARAM);
    afx_msg LRESULT OnUpdateWave(WPARAM, LPARAM);
    afx_msg LRESULT OnInitLoadPcap(WPARAM, LPARAM);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    afx_msg void OnItemSelFrame(NMHDR* pNotifyStruct, LRESULT *pResult);

#if !SU_FEI
	afx_msg void OnMenuF5();
#endif

private:
    struct PCAP_CTRLBLK
    {
        uint32_t            index;                      ///< ����
        uint32_t            app_id;                     ///< appid
        size_t              max_frame;                  ///< ���֡��
        size_t              first_visibility_index;     ///< ��ǰҳ��һ�������е�֡����(���ƿ�������)
        int                 focus;                      ///< ��ǰ������
        enumFrameType       type;                       ///< appid����
        const CAppidSampleMapping* data;                ///< 
        UINT                cmdid;                      ///< 

        struct {
            uint32_t        wfirst_visibility_index;    ///< ���η���ʱ��һ���ɼ�ͨ������
            int             wzoomx;                     ///< x�����ű���
            int             wzoomy;                     ///< y�����ű���
            int             wsample;                    ///< ������
            uint32_t        wbegin;                     ///< ��ʼ��
        }wave;
    };

public:

    /**
     *      ���ô�������pcap�ļ�·��
     */
    inline void _SetFile(const CString csFile)
    {
        m_csPcapFile = csFile;
    }

    /**
     *      �����ڹ�������Դ
     */
    virtual void _ClearResource();


private:

    /**
     *      ��������֡����
     */
    void _createReport();

    /**
     *      ����ָ��pcap�ļ�
     */
    bool _loadPcap();

    /**
     *      ��ʾ����֡��Ϣ
     *
     *  param[in]
     *      nIndex          ��ʾ��֡����
     */
    void _showFrameDetail(size_t nIndex);

	void _udateFrameDetail( enumFrameType eType );
	

    /**
     *      ����ָ��֡������ȡ֡����
     *
     *  \param[in]
     *      nIndex          ֡����
     *  \param[out]
     *      rType           ֡����
     */
    bool _getFrameBuff(size_t nIndex, enumFrameType& rType);

    /**
     *      ����֡��Ϣ���ؼ�
     */
    void _createFrameTree();

    /**
     *      �л���ĳ��ҳ��״̬ʱ�����в˵��������޸�
     *
     *  \param[in]
     *      mode            �µ�ҳ��ģʽ
     */
    void _switchToPage(ModePage mode);

    /**
     *      �������б�ҳ����
     */
    BOOL _preTranslateMessageList(MSG* pMsg);
#if SU_FEI
    /**
     *      ����F5/F6�����쳣����֡�ı�����Ϣ
     */
    void _updateNaviExcep();
#endif
    /**
     *      �����Ĳ��η������������Ҳ���
     */
    BOOL _preTranslateMessageWave(MSG* pMsg);

    /**
     *      �����״̬����
     */
    void _clearStatus();

    /**
     *      ��ʼ�������еĿ��ƿ���Ϣ
     */
    bool _initCtrlBlk();

    /**
     *      ���ر����еĿ��ƿ���Ϣ
     */
    bool _loadCtrlBlk();

    /**
     *      ���ɿ��ƿ�˵�
     */
    void _initCtrlBlkMenu();

    /**
     *      �����л����ƿ���Ϣ
     *
     *  \param[in]
     *      nCmdID          ѡ�еĲ˵�ID
     */
    void _switchCtrlBlk(UINT nCmdID);

    /**
     *      ��ȡָ��AppID�Ŀ��ƿ�����
     *
     *  \param[in]
     *      appid           ָ����appid
     *  \param[out]
     *      data            appid��Ӧ������
     *  \return
     *      bool            ��ȡ�ɹ�����true��ʧ�ܷ���false
     */
    bool _getCtrlBlkByAppID(uint32_t appid, PCAP_CTRLBLK** data);

        /**
     *      ��ȡָ��AppID�Ŀ��ƿ�����
     *
     *  \param[in]
     *      appid           ָ����appid
     *  \param[out]
     *      data            appid��Ӧ������
     *  \return
     *      bool            ��ȡ�ɹ�����true��ʧ�ܷ���false
     */
    bool _getCtrlBlkByAppID(uint32_t appid, PCAP_CTRLBLK& data);

    /**
     *      ˢ��UI����
     */
    void _updateUI();

    /**
     *      ˢ�±���֡�б����
     */
    void _updateUIFrameList();

    /**
     *      ˢ�±���ͳ�ƽ���
     */
    void _updateUIStatistics();

    /**
     *      ˢ��sv����ͳ�ƽ���
     */
    void _updateUIStatisticsSV();

    /**
     *      ˢ��gs����ͳ�ƽ���
     */
    void _updateUIStatisticsGS();

    /**
     *      ˢ�²��η�������
     */
    void _updateUIWave();

    /**
     *      �л�������֡�б�
     */
    void _switchFrameList();

    /**
     *      �л�����ϸ֡����
     */
    void _switchFrameDetail();

    /**
     *      �л����쳣ͳ��
     */
    void _switchFrameStatistics();

    /**
     *      �л����Ĳ��η���
     */
    void _switchFrameWave();

    /**
     *      ��ȡ֡��������
     *  \param[in]
     *      type            ֡����
     */
    CString _getFrameTypeDesc(enumFrameType type);

    /**
     *      ��ȡÿһ֡�쳣������
     *
     *  \param[in]
     *      type            ֡����
     *  \param[in]
     *      status          �쳣����
     *  \return
     *      CString         �쳣��������Ϣ
     */
    CString _getFrameStatisticsDesc(enumFrameType type, uint16_t status);

    /**
     *      ��ȡ֡������Ϣ
     *
     *  \param[in]
     *      type            ֡����
     *  \param[in]
     *      index           ֡����
     *  \return
     *      CString         ����֡������Ϣ
     */
    CString _getFrameDesc(enumFrameType type, uint32_t index);

    /**
     *      ������һ���쳣֡��λ��
     *
     *  \param[out]
     *      first_visible       ��һ����������
     *  \param[out]
     *      focus               �쳣֡λ��
     *  \return
     *      bool                �ҵ�����true���Ҳ�������false
     */
 //   bool _lookupPreviousException(size_t& first, int& focus);

    /**
     *      ������һ���쳣֡��λ��
     *
     *  \param[out]
     *      first_visible       ��һ����������
     *  \param[out]
     *      focus               �쳣֡λ��
     *  \return
     *      bool                �ҵ�����true���Ҳ�������false
     */
 //   bool _lookupNextException(size_t& first, int& focus);

    /**
     *      �ж��Ƿ�����һ���쳣֡
     */
  //  bool _hasPrevException();

    /**
     *      �ж��Ƿ�����һ���쳣֡
     */
 //   bool _hasNextException();

    /**
     *      �������н���ؼ�
     */
    void _hideAllControl();

    /**
     *      ���²�ͬҵ���Ӧ�Ĳ˵������
     */
    void _updateMenuNCaption();



    //////////////////////////////////////////////////////////////////////////
    // ����ͳ��

    /**
     *      ����SMV����ͳ�Ʊ���
     */
    void _createReportSMVStatistics();

    /**
     *      ������ɢ����״ͼ
     */
    void _createHorHistogram();

    /**
     *      ����GOOSE����ͳ�Ʊ���
     */
    void _createReportGSStatistics();

    /**
     *      ��ʾsmv����ͳ�ƽ���
     */
    void _showSMVStatistics();

    /**
     *      �޸���ɢ����״ͼ�ؼ�����
     */
    void _resetHistogram();
#if !SU_FEI
	void _showSMVRistorgram();
	void _switchFrameHistogram();
#endif
    /**
     *      ��ʾgoose����ͳ��ҳ��
     */
    void _showGOOSEStatistics();

//////////////////////////////////////////////////////////////////////////
// ���η���

    /**
     *      �������οؼ�
     */
    void _createWaveform();

    /**
     *      ���ò��οؼ�����
     */
    void _setWaveform();

    /**
     *      �л�����������۲�
     */
    void _switchWaveZoomOrWatch();

    /**
     *      ����������ͨ������ȡһ��ģ�����
     */
    void _getTemplate(enumFrameType type);

    /**
     *      �����ֵ
     */
    float _calcAmplitude(uint32_t index);

    /**
     *      �Ϸ�ҳͨ��
     */
    void _pageupChannel(PCAP_CTRLBLK* data);

    /**
     *      �·�ҳͨ��
     */
    void _pagedownChannel(PCAP_CTRLBLK* data);

    /**
     *      ��ҳͨ��
     */
    void _pageleftChannel(PCAP_CTRLBLK* data);

    /**
     *      �ҷ�ҳͨ��
     */
    void _pagerightChannel(PCAP_CTRLBLK* data);

    /**
     *      �Ŵ���
     */
    void _zoominWave(PCAP_CTRLBLK* data);

    /**
     *      ��С����
     */
    void _zoomoutWave(PCAP_CTRLBLK* data);

    /**
     *      �Ŵ�ʱ����
     */
    void _zoominTime(PCAP_CTRLBLK* data);

    /**
     *      ��Сʱ����
     */
    void _zoomoutTime(PCAP_CTRLBLK* data);

    /**
     *      ��ȡͨ������Ϣ
     */
    void _getChannelInfo(uint32_t index, COLORREF& color);

    /**
     *      ���¼���ÿ�����ӵĲ�������
     */
    inline float _calcXGridCounts(PCAP_CTRLBLK* data)
    {
        return (float)(data->wave.wsample / DSM_WORK_HERTZ_50 / 2  * pow(2, data->wave.wzoomx));
    }

    /**
     *      ͳ�Ʒ���pcap���״̬��Ϣ
     *
     *  \note
     *      ͳ���Ƿ����쳣֡
     */
#if SU_FEI
    void _statisticsLoadedPcap();
#endif

#if !CHUN_HUA
public:
	void _updateFrameDetailCaption() ;
	
#endif


private:
    CTreeV                  m_treeFrame;            ///< ����֡���ؼ�
    CReportV                m_reportFrame;          ///< ���ļ�����ݱ���
    CReportV                m_reportSMVStatistics;  ///< SMV����ͳ�Ʊ���
    CReportV                m_reportGooseStatistics;///< GOOSE����ͳ�Ʊ���
    CHorHistogramCtrl       m_ctrlHistogram;        ///< ������״ͼ

    CRect                   m_rcTop;                ///< ��ɢͼ��������
    CString                 m_csSampleInter;        ///< ��ɢ�ȱ���

    int                     m_nMaxRows;             ///< ÿҳ�������
    uint8_t*                m_pFrameBuff;           ///< ������ǰ֡���ݵĻ���
    CString                 m_csPcapFile;           ///< ���ڷ�����pcap�ļ���
    IPcapFile*              m_pPcap;                ///< pcap�ļ�������

    ModePage                m_eModePage;            ///< ��ǰҳ�������ģʽ
    ModePage                m_eLastMode;            ///< ��һҳ��ģʽ

    Sav91Msg*               m_pMsg91;               ///< 91Э�������
    Sav92Msg*               m_pMsg92;               ///< 92Э�������
    NanRuiFT3Msg*           m_pMsgNanRui;           ///< FT3����Э�������
    GuoWangFT3Msg*          m_pMsgGuoWang;          ///< FT3����Э�������
    GooseMsg*               m_pMsgGoose;            ///< goose������

    CNaviToolBar*           m_pNaviBar;             ///< �˵���������
    CRNaviMenu*             m_pRNaviCurrent;        ///< ��ǰҵ��ҳ��Ĳ˵�����ָ��

    CMsgDlg*                m_pMsgDlg;              ///< ��ʾ����pcap״̬����ָ��

    CDsmDataLayer*          m_pDataLayer;           ///< ���ݲ�ָ��

    typedef std::vector<PCAP_CTRLBLK> vecCtrlBlk;

    vecCtrlBlk              m_vecCtrlBlk;           ///< ���ƿ��
    uint32_t                m_nCurrentAppID;        ///< ��ǰѡ�еĿ��ƿ�APPID
    uint32_t                m_nCurrentChanns;       ///< ��ǰ���ƿ��ͨ����
    enumFrameType           m_eCuttentType;         ///< ��ǰ���ƿ�����
    UINT                    m_nCtrlBlkCmdEnd;       ///< ���ƿ�˵���IDβ��
    UINT                    m_nCtrlBlkHighLight;    ///< ���ƿ�����Ĳ˵�ID

    CBrush                  m_brBackground;         ///< ������ˢ
    uint32_t                m_nDetailIndex;         ///< ��ϸ֡���ݵ�����

    ModeWave                m_eModeWave;            ///< ���η���ģʽ
    HANDLE                  m_hThread;              ///< ����pcap�߳�
    DWORD                   m_dwLoadComplete;       ///< ����pcap���
#if SU_FEI
    bool                    m_arrUpdate[3];         ///< �жϸ�ҵ���Ƿ���Ҫ����ˢ������
#else
	bool					m_arrUpdate[4];			///< �жϸ�ҵ���Ƿ���Ҫ����ˢ������
#endif

#if SU_FEI
    bool                    m_bHasExceptionFrame;   ///< �жϵ�ǰ���ص�pcap�Ƿ����쳣����֡

    DWORD                   m_dwDirectTag;          ///< �ж������ƶ��ı�־ 1�����ϣ�2������
    size_t                  m_nFirstExcep;          ///< ��һ���쳣
    size_t                  m_nLastExcep;           ///< ���һ���쳣
#endif
    CWaveFormGramCtrl       m_ctrlWaveform;         ///< ���η����ؼ�
    CDataOscillogram        m_dataWave;             ///< ���η����������
    SmvChannelTemplate*     m_pChannelTemplate;     ///< ��ǰ���ƿ�ͨ��ģ�����ָ��
#if SHENG_ZE
    CProgressBarDlg*        m_pWaveDlg;             ///< ���η���������ʾ�Ի���
#else
	CMsgDlg*				m_pWaveDlg;				///< ���η���������ʾ�Ի���
#endif

#if !CHUN_HUA
	uint32_t				m_nSelectFrame;
	uint32_t				m_nSelectFrameIndex;
#endif
};
