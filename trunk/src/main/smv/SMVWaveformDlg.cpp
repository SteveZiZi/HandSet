/**
 * \file
 *      SMVWaveformDlg.h
 *
 * \brief
 *      ����ͼҳ��ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/19
 */


// SMVWaveformDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SMVWaveformDlg.h"

#include <algorithm>

#include "src/main/mgr/statusmgr.h"
#include "src/main/smv/common_smv.h"
#include "src/main/pagedata/pagedata.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/service/datalayer/dsmdatalayer.h"
#include "src/main/resourcebmp.h"

#define DSM_SMV_WAVEFORM_ZOOM           0x01        ///< ����ģʽ
#define DSM_SMV_WAVEFORM_WATCH          0x02        ///< �۲�ģʽ

#define DSM_SMV_WAVEFORM_MAX_TIMES      10          ///< ����ηŴ���
#define DSM_SMV_WAVEFORM_MIN_TIMES      -10
#define DSM_SMV_TIMELINE_MAX_TIMES      10          ///< ���ʱ����Ŵ���
#define DSM_SMV_TIMELINE_MIN_TIMES      -10

#define WAVEFORM_DISP_FONT_SZIE         18          ///< ����ͼ�������С

#define DSM_WFG_FONT_COLOR              RGB(128,128,135)      ///< ů��ɫ



namespace SMVWaveform
{
    void conversion(float& val)
    {
        val = val / 1000;
    }
}


/**
 *      ����ҳ����ƿ��������
 */
class CCtrlBlkDataWaveform : public ICtrlBlkData
{
public:
    CCtrlBlkDataWaveform()
        : m_nPage(0)
        , m_nTimelineT(0)
        , m_nWaveformT(0)
    {

    }

    virtual ~CCtrlBlkDataWaveform() {}

public:
    int         m_nPage;                                ///< ��ǰҳ��
    int         m_nWaveformT;                           ///< Y��Ŵ���
    int         m_nTimelineT;                           ///< X��Ŵ���
};



// CSMVWaveformDlg �Ի���

IMPLEMENT_DYNAMIC(CSMVWaveformDlg, DSM_SMVBASEDLG)

CSMVWaveformDlg::CSMVWaveformDlg(CWnd* pParent /*=NULL*/)
	: DSM_SMVBASEDLG(CSMVWaveformDlg::IDD, DSM_STR_SMV_WAVEFORM_CAPTION, pParent)
    , m_nPage(0)
    , m_dwWaveMode(DSM_SMV_WAVEFORM_WATCH)
    , m_nSamplePer(0)
    , m_nSamplePer1(0)
    , m_nSample(0)
    , m_fAmperage(0.0f)
    , m_fVoltage(0.0)
    , m_nWaveformT(0)
    , m_nTimelineT(0)
    , m_eDispType(SMV_VALUE_TYPE_PRIMARY)
    , m_eFrameType(SMV_VALUE_TYPE_PRIMARY)
{
}

CSMVWaveformDlg::~CSMVWaveformDlg()
{
    CStatusMgr::SetTips(L"", 200);
}

void CSMVWaveformDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_SMVBASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSMVWaveformDlg, DSM_SMVBASEDLG)
    ON_WM_TIMER()
    ON_COMMAND(ID_MENU_SMV_F4, &CSMVWaveformDlg::OnMenuF4)
END_MESSAGE_MAP()


// CSMVWaveformDlg ��Ϣ�������

BOOL CSMVWaveformDlg::OnInitDialog()
{
    DSM_SMVBASEDLG::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    // ��ԭҳ��״̬
    _restorePage();

    // �������οؼ�
    _createWaveform();

    return FALSE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

void CSMVWaveformDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    if(nIDEvent == m_nTimerID)
    {
        // ˢ�²��οؼ�����
        _updateWaveform();
    }

    CSMVBaseDlg::OnTimer(nIDEvent);
}

void CSMVWaveformDlg::_createWaveform()
{
    CRect rcClient;
    GetClientRect(rcClient);
    rcClient.DeflateRect(CRect(10, 10, 10, 10));

    // �������οؼ�
    m_ctrlWaveform.Create(NULL, WS_CHILD | WS_VISIBLE, rcClient, this, IDC_SMV_WAVFORM_CTRL);

    // ���ò��οؼ�����
    m_ctrlWaveform.ModifyStyle(0, SS_OWNERDRAW | WS_TABSTOP);
    m_ctrlWaveform.SetXScaleValVisible(TRUE);
    m_ctrlWaveform.SetTitleVisible(TRUE);
    m_ctrlWaveform.SetGramCount(DSM_SMV_WAVEFORM_GRAM_X, DSM_SMV_WAVEFORM_GRAM_Y);
    m_ctrlWaveform.SetHorGridVisible(FALSE);
	m_ctrlWaveform.SetBkgndColor(DSM_COLOR_BK);

    m_ctrlWaveform.SetWaveAreaGrid(3, 2);
    m_ctrlWaveform.SetTitleColor(DSM_WFG_FONT_COLOR);
	m_ctrlWaveform.SetYIntervalMaxCharNum(10);

#if DSM_OSCILLOGRAM_SHOW_EFFECT_PHASE
    m_ctrlWaveform.SetWaveDescVisible(TRUE);
#else
    m_ctrlWaveform.SetWaveDescVisible(FALSE);
#endif

    for(int i = 0; i < 3; i++)
    {
        m_ctrlWaveform.SetWaveDescColor(i, 0, DSM_WFG_FONT_COLOR);
    }

    // ���ò��οռ�����Ҫ������
    CFont* pFont = dsm_fontmgr::GetInstance()->GetFont(WAVEFORM_DISP_FONT_SZIE);
    m_ctrlWaveform.SetScaleValFont(pFont);
    m_ctrlWaveform.SetTitleFont(pFont);
    m_ctrlWaveform.SetWaveDescFont(pFont);

    // ��ȡʱ�����Ĭ�Ͽ̶�    X��
    _setXScale();

    // ���ò���ͼ������        Y��
    _setWaveform();

    m_ctrlWaveform.SetFocus();
}


void CSMVWaveformDlg::_updateWaveform()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pDataLayer);

    if(m_bEnableTimer)
    {
        // ͨ�������仯��
        if(m_pCurrentCtrlBlk->nChannelCount != m_pDataLayer->GetCurrentCtrlBlkRealTimeChannelCounts())
        {
            if(CStatusMgr::ShowTipsStyle() == 0x00)
            {
                // ����tips��ʾͨ�������仯
                CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_CHANNEL_NUM_CHANGE));
            }

            if(!m_bChannelChange)
            {
                m_bChannelChange = true;

                // ȡ�����ֲ˵�����
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, L"");
                m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(3, NULL);
                m_pNaviBar->UpdateNaviMenu();

                m_ctrlWaveform.EraseWave();
                m_ctrlWaveform.Invalidate(TRUE);
            }

            return;
        }
        else
        {
            if(m_bChannelChange)
            {
                m_bChannelChange = false;
                CStatusMgr::SetTips(L"", 0);

                // �ָ����ֲ˵�����
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_TIMER_STOP));
                m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(m_dwWaveMode == DSM_SMV_WAVEFORM_ZOOM ? DSM_BMP_M_SMV_SCAN_STRETCH_SEL : DSM_BMP_M_SMV_SCAN_SEL), NULL, NULL);
                m_pNaviBar->UpdateNaviMenu();
            }
        }

        // ��ȡ��������
        if(!m_pDataLayer->GetOscillogramData(&m_data))
        {
            // ��������
            m_ctrlWaveform.EraseWave();
            m_ctrlWaveform.Invalidate(TRUE);
            return;
        }
    }

    CString csEffect(dsm_stringMgr::Get(DSM_STR_SMV_WAVEFORM_EFFECT));   // ��ȡ��Чֵ����
    CString csAngle(dsm_stringMgr::Get(DSM_STR_SMV_WAVEFORM_ANGLE));     // ��ȡ��λ������

    CString csValue;
    COscillogramChannelInfo* pInfo = NULL;

    // ��������
    m_ctrlWaveform.EraseWave();

    // ��䲨������
    for(uint32_t nGram = 0, i = 0; i < DSM_SMV_WAVEFORM_NUMS; i++, nGram++)
    {
        // ��ȡָ��ͨ���Ĳ�������
        pInfo = m_data.GetOneChannelInfo(m_arrChannels[i]);

        if(pInfo)
        {

#if DSM_OSCILLOGRAM_SHOW_EFFECT_PHASE
            // ��������ͨ����������Ϣ
            if(CHANNEL_TYPE_SMV_CURRENT == m_arrChannelType[i])
            {
                csValue.Format(L"%s: %0.3f %s\r\n%s: %0.3f��", csEffect, pInfo->m_fEffectiveValue, m_arrUnits[i], csAngle, pInfo->m_fPhase);
            }
            // ��ѹ����ͨ����������Ϣ
            else if(CHANNEL_TYPE_SMV_VOLTAGE == m_arrChannelType[i])
            {
                // һ�ζ���ֵ��λ���ݻ���
                if(m_eDispType == SMV_VALUE_TYPE_PRIMARY || (m_eDispType == SMV_VALUE_TYPE_ORIGINANL && m_eFrameType == SMV_VALUE_TYPE_PRIMARY))
                {
                    pInfo->m_fEffectiveValue = pInfo->m_fEffectiveValue / 1000;

                    for_each(pInfo->m_vecChannelData.begin(), pInfo->m_vecChannelData.end(), SMVWaveform::conversion);
                }

                csValue.Format(L"%s: %0.3f %s\r\n%s: %0.3f��", csEffect, pInfo->m_fEffectiveValue, m_arrUnits[i], csAngle, pInfo->m_fPhase);
            }
            else if(CHANNEL_TYPE_SMV_TIME == m_arrChannelType[i])
            {
                csValue.Format(L"%s: %0.3f %s\r\n%s: %0.3f��", csEffect, pInfo->m_fEffectiveValue, m_arrUnits[i], csAngle, pInfo->m_fPhase);
            }
            // �������͵�ͨ��������Ϣ��û�е�λ
            else
            {
                csValue.Format(L"%s: %0.3f\r\n%s: %0.3f��", csEffect, pInfo->m_fEffectiveValue, csAngle, pInfo->m_fPhase);
            }
#endif
            // ��ͨ���Ĳ�����Ϣ���õ����οؼ�
            m_ctrlWaveform.DrawWave(nGram, &pInfo->m_vecChannelData[0], pInfo->m_vecChannelData.size(), csValue);
        }
    }

    m_ctrlWaveform.Invalidate(TRUE);
}

BOOL CSMVWaveformDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���
    if(WM_KEYDOWN == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case VK_UP:

            if(!m_bChannelChange)
            {
                // �۲�ģʽ ��ҳ
                if(DSM_SMV_WAVEFORM_WATCH == m_dwWaveMode)
                {
                    _pageupChannel();
                }
                // ����ģʽ ��ֵ
                else
                {
                    _zoominWaveform();
                }

                // ���²�������
                _updateWaveform();
            }

            return TRUE;

        case VK_DOWN:
            if(!m_bChannelChange)
            {
                // �۲�ģʽ ��ҳ
                if(DSM_SMV_WAVEFORM_WATCH == m_dwWaveMode)
                {
                    _pagedownChannel();
                }
                // ����ģʽ ��ֵ
                else
                {
                    _zoomoutWaveform();
                }

                // ���²�������
                _updateWaveform();
            }

            return TRUE;

        case VK_LEFT:
            // ����ģʽ ʱ����
            if(!m_bChannelChange && DSM_SMV_WAVEFORM_ZOOM == m_dwWaveMode)
            {
                _zoomoutTimeline();

                // ���²�������
                _updateWaveform();
            }

            return TRUE;

        case VK_RIGHT:
            // ����ģʽ ʱ����
            if(!m_bChannelChange && DSM_SMV_WAVEFORM_ZOOM == m_dwWaveMode)
            {
                _zoominTimeline();

                // ���²�������
                _updateWaveform();
            }

            return TRUE;

        default:
            break;
        }
    }

    return CSMVBaseDlg::PreTranslateMessage(pMsg);
}

/**
 *      �Ϸ�ͨ��
 */
void CSMVWaveformDlg::_pageupChannel()
{
    if(0 == m_nPage)
    {
        return;
    }

    // ҳ��������1
    m_nPage--;

    // ���ò���ͼ�ؼ�������
    _setWaveform();
}

/**
 *      �·�ͨ��
 */
void CSMVWaveformDlg::_pagedownChannel()
{
    ASSERT(m_pCurrentCtrlBlk);

    // �����һҳͨ����������ͨ���������򷵻�
    if((m_nPage + 1) * DSM_SMV_WAVEFORM_NUMS >= m_pCurrentCtrlBlk->nChannelCount)
    {
        return;
    }

    // ҳ��������1
    m_nPage++;

    // ������ʾͨ����Ĭ�Ϸ�ֵ
    _setWaveform();
}

/**
 *      �޸ĵײ��˵�
 */
void CSMVWaveformDlg::_modifyMenu()
{
    __super::_modifyMenu();

	m_menuMain.SetMenuItemCaption(0, dsm_stringMgr::Get(DSM_STR_SMV_WAVEFORM_CAPTION));
	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
	m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);


    // ����F4����ѡ��ͼ��
    m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(m_dwWaveMode == DSM_SMV_WAVEFORM_ZOOM ? DSM_BMP_M_SMV_SCAN_STRETCH_SEL : DSM_BMP_M_SMV_SCAN_SEL), NULL, NULL);

	
	//��Ҫ����������ѡ�е�һ��ֵ���Ƕ���ֵ�����жϣ�����������ʾ
	CSmvRecvConfigPublic* pCfg = m_pDataLayer->GetSmvConfigPublic();

	if (pCfg->GetSmvDispType() == SMV_VALUE_TYPE_SECONDARY)
	{
		m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_SECOND_SEL), NULL, NULL);
	}
	else if (pCfg->GetSmvDispType() == SMV_VALUE_TYPE_PRIMARY)
	{
		m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_FIRST_SEL), NULL, NULL);
	}
	

}

/**
 *      ���ݲ˵�ID�л���ָ���Ŀ��ƿ�
 *
 *  \param[in]
 *      nCmdID          �˵�ID
 *  \return
 *      bool            �л����ƿ�ɹ�����true
 */
bool CSMVWaveformDlg::_switchCtrlBlk( UINT nCmdID )
{
    // ����ԭ���ƿ����
    CBaseDetectItem* pPreCtrlBlk = m_pCurrentCtrlBlk;

    // �л����ƿ�
    if(!__super::_switchCtrlBlk(nCmdID))
    {
        return false;
    }

    // ��ѡ���ϴεĲ˵��򷵻�
    if(pPreCtrlBlk == m_pCurrentCtrlBlk && pPreCtrlBlk->nID == m_pCurrentCtrlBlk->nID)
    {
        return true;
    }

    // ��ȡԭ���ƿ����������
    CCtrlBlkDataWaveform* pData = dynamic_cast<CCtrlBlkDataWaveform*>(m_pPageDataExtra->Get(pPreCtrlBlk->nID));

    // ��ȡ����Ϊ���򴴽��µĿ��ƿ��������
    if(!pData)
    {
        pData = new CCtrlBlkDataWaveform;
        m_pPageDataExtra->Set(pPreCtrlBlk->nID, pData);
    }

    // ����Ŵ���
    pData->m_nTimelineT = m_nTimelineT;
    pData->m_nWaveformT = m_nWaveformT;


    // �л����ƿ��Կؼ���������

    // �����µĿ��ƿ���Ϣ����ҳ��״̬
    _restorePage();

    // �����µĿ��ƿ���Ϣ���ò��οؼ�
    _setWaveform();

    return true;
}

/**
 *      ��Ӧ��F4������/ѡ����ģʽ���л��������²˵�ͼ��
 */
void CSMVWaveformDlg::OnMenuF4()
{
    if(m_bChannelChange)
    {
        return;
    }

    m_dwWaveMode = m_dwWaveMode == DSM_SMV_WAVEFORM_ZOOM ? DSM_SMV_WAVEFORM_WATCH : DSM_SMV_WAVEFORM_ZOOM;

    // �л����ε�ģʽΪ�����š�
    if(DSM_SMV_WAVEFORM_ZOOM == m_dwWaveMode)
    {
        m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_STRETCH_SEL), NULL, NULL);
    }
    // �л����ε�ģʽΪ��ѡ��
    else
    {
        m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_SEL), NULL, NULL);
    }

    m_pNaviBar->UpdateNaviMenu();
}

/**
 *      �Ŵ���
 *
 *  \note
 *      ����Y���ֵ���Ŵ���
 */
void CSMVWaveformDlg::_zoominWaveform()
{
    // �жϷŴ����Ƿ񳬹�����
    if(DSM_SMV_WAVEFORM_MIN_TIMES >= m_nWaveformT)
    {
        return;
    }

    // ���ٷ�ֵ�����ű���
    m_nWaveformT--;

    // ѭ������ÿ�����εĿ̶���Ϣ����Ҫ�Ǹı�Y��̶ȣ���ֵ��
    for(int i = 0; i < DSM_SMV_WAVEFORM_NUMS; i++)
    {
        m_ctrlWaveform.SetScaleInterval(i, 
            (float)(m_nSamplePer * pow(2, m_nTimelineT)), 
            m_arrChannelAmplitude[i] * (float)pow(2, m_nWaveformT), 
            m_arrUnits[i]);
    }

    // ���ò��οռ�ʹ������Ч
    m_ctrlWaveform.Init();
}

/**
 *      ��С����
 *
 *  \note
 *      ����Y���ֵ����С����
 */
void CSMVWaveformDlg::_zoomoutWaveform()
{
     // �ж����ű����Ƿ񳬹�����
    if(DSM_SMV_WAVEFORM_MAX_TIMES <= m_nWaveformT)
    {
        return;
    }

    // �����ֵ���ű���
    m_nWaveformT++;

    // ѭ������ÿ�����εĵĿ̶���Ϣ����Ҫ�Ǹı�Y��̶ȣ���ֵ��
    for(int i = 0; i < DSM_SMV_WAVEFORM_NUMS; i++)
    {
        m_ctrlWaveform.SetScaleInterval(i, 
            (float)(m_nSamplePer * pow(2, m_nTimelineT)), 
            m_arrChannelAmplitude[i] * (float)pow(2, m_nWaveformT), 
            m_arrUnits[i]);
    }

    // ���ò��οռ�ʹ������Ч
    m_ctrlWaveform.Init();
}

/**
 *      �Ŵ�ʱ���ᣨX�ᣩ
 *
 *  \note
 *      ����X��ÿ�����ӵĲ������������첨��
 */
void CSMVWaveformDlg::_zoominTimeline()
{
    // �ж����ű����Ƿ񳬹�����
    if(DSM_SMV_TIMELINE_MIN_TIMES >= m_nTimelineT)
    {
        return;
    }

    // ���ٱ�������
    m_nTimelineT--;

    // ѭ������ÿ�����εĿ̶���Ϣ����Ҫ�Ǹı�X��̶ȣ�ÿ�����Ӳ���������
    for(int i = 0; i < DSM_SMV_WAVEFORM_NUMS; i++)
    {
        m_ctrlWaveform.SetScaleInterval(i, 
            (float)(m_nSamplePer * pow(2, m_nTimelineT)), 
            m_arrChannelAmplitude[i] * (float)pow(2, m_nWaveformT), 
            m_arrUnits[i]);

        // ���ò����·�ÿ�����ӵĲ�����������ʾ��Ϣ
        m_ctrlWaveform.SetXOneGridPointCount(i, (int)(m_nSamplePer1 * (float)pow(2, m_nTimelineT)));
    }

    // ���ò��οռ�ʹ������Ч
    m_ctrlWaveform.Init();
}

/**
 *      ��Сʱ���ᣨX�ᣩ
 *
 *  \note
 *      ����X��ÿ�����ӵĲ���������ѹ������
 */
void CSMVWaveformDlg::_zoomoutTimeline()
{
    // �ж����ű����Ƿ񳬹�����
    if(DSM_SMV_WAVEFORM_MAX_TIMES <= m_nTimelineT)
    {
        return;
    }

    // ����������
    m_nTimelineT++;

    // ѭ������ÿ�����εĿ̶���Ϣ����Ҫ�Ǹı�X��̶ȣ�ÿ�����Ӳ���������
    for(int i = 0; i < DSM_SMV_WAVEFORM_NUMS; i++)
    {
        m_ctrlWaveform.SetScaleInterval(i, 
            (float)(m_nSamplePer * pow(2, m_nTimelineT)), 
            m_arrChannelAmplitude[i] * (float)pow(2, m_nWaveformT), 
            m_arrUnits[i]);

        // ���ò����·�ÿ�����ӵĲ�����������ʾ��Ϣ
        m_ctrlWaveform.SetXOneGridPointCount(i, (int)(m_nSamplePer1 * (float)pow(2, m_nTimelineT)));
    }

    // ���ò��οռ�ʹ������Ч
    m_ctrlWaveform.Init();
}

/**
 *      ����ͨ�����������ò���ͼ������
 *
 *  \note
 *      ����ͨ����Ϣ���ò�������ʾ�ĵ�λ��Ϣ��
 *      �Լ����εĻ�����ֵ��ʱ�����Ĭ��ÿ������Ĭ�ϲ�������
 */
void CSMVWaveformDlg::_setWaveform()
{
    ASSERT(m_pCurrentCtrlBlk);

    CSmvRecvConfig* pConfig = m_pCurrentCtrlBlk->pRecvConfig;

    if(!pConfig)
    {
        ASSERT(pConfig);
        return;
    }

    COLORREF color;
    CString csDesc;
    CSmvRecvChannelInfo* pInfo  = NULL;
    m_eDispType  = m_pDataLayer->GetSmvConfigPublic()->GetSmvDispType();         // ��ȡͨ��ֵ����ʾ����
    m_eFrameType = m_pDataLayer->GetParamConfigBasic()->GetFrameValueType();     // ��ȡ�ͨ�����͵���ʾ����

    // ��ȡ��ǰ��ʾ��ͨ�����
    _calcChannel();

    // ѭ��������ͨ��������ÿ�����ε���Ϣ
    for(int i = 0; i < DSM_SMV_WAVEFORM_NUMS; i++)
    {
        pInfo = pConfig->GetOneChannelInfo(m_arrChannels[i]);

        if(!pInfo)
        {
            // ���ÿյ�title
            m_ctrlWaveform.SetTitle(i, L"");

            continue;
        }

        // ���ø�ͨ����Ӧ���ε���ʾ��Ϣ - ��λ��Ĭ�Ϸ�ֵ
        // ��������������ʾ��һ�ζ���ֵ��Ϣ��������Ӧ��Ĭ�Ϸ�ֵ

        // ����ͨ���ĵ�λ��Ĭ�Ϸ�ֵ
        if(CHANNEL_TYPE_SMV_CURRENT == pInfo->enChannelType)
        {
            m_arrUnits[i] = L"A";

            switch(m_eDispType)
            {
                // ������ʾһ��ֵ���ݵ�Ĭ�Ϸ�ֵ
            case SMV_VALUE_TYPE_PRIMARY:
                m_arrChannelAmplitude[i]    = pInfo->fPrimaryRatedValue / 2;
                break;

                // ������ʾ����ֵ���ݵ�Ĭ�Ϸ�ֵ
            case SMV_VALUE_TYPE_SECONDARY:
                m_arrChannelAmplitude[i]    = pInfo->fSecondaryRatedValue / 2;
                break;

                // ��ʾԭʼ�������ݵ�Ĭ�Ϸ�ֵ
            case SMV_VALUE_TYPE_ORIGINANL:
                {
                    // û�е�λ
                    m_arrUnits[i]               = L"";

                    // ���ݱ��Ĳ�������ҳ��ġ��ͨ�����͡���Ϣ����Ĭ�ϸ���
                    if(SMV_VALUE_TYPE_PRIMARY == m_eFrameType)
                    {
                        m_arrChannelAmplitude[i]    = pInfo->fPrimaryRatedValue / 2;
                    }
                    else
                    {
                        m_arrChannelAmplitude[i]    = pInfo->fSecondaryRatedValue / 2;
                    }
                }
                break;

            default:
                ASSERT(FALSE);
                break;
            }
        }
        // ��ѹͨ���ĵ�λ��Ĭ�Ϸ�ֵ
        else if(CHANNEL_TYPE_SMV_VOLTAGE == pInfo->enChannelType)
        {
            switch(m_eDispType)
            {
                // ������ʾһ��ֵ���ݵ�Ĭ�Ϸ�ֵ
            case SMV_VALUE_TYPE_PRIMARY:
                m_arrUnits[i]               = L"kV";
                m_arrChannelAmplitude[i]    = pInfo->fPrimaryRatedValue / 2000;
                break;

                // ������ʾ����ֵ���ݵ�Ĭ�Ϸ�ֵ
            case SMV_VALUE_TYPE_SECONDARY:
                m_arrUnits[i]               = L"V";
                m_arrChannelAmplitude[i]    = pInfo->fSecondaryRatedValue / 2;
                break;

                // ��ʾԭʼ�������ݵ�Ĭ�Ϸ�ֵ
            case SMV_VALUE_TYPE_ORIGINANL:
                m_arrUnits[i]               = L"";

                // ���ݱ��Ĳ�������ҳ��ġ��ͨ�����͡���Ϣ����Ĭ�ϸ���
                if(SMV_VALUE_TYPE_PRIMARY == m_eFrameType)
                {
                    m_arrChannelAmplitude[i]    = pInfo->fPrimaryRatedValue / 2000;
                }
                else
                {
                    m_arrChannelAmplitude[i]    = pInfo->fSecondaryRatedValue / 2;
                }
                break;

            default:
                ASSERT(FALSE);
                break;
            }
        }
        // ʱ��ͨ����Ĭ�Ϸ�ֵΪ400.0
        else if(CHANNEL_TYPE_SMV_TIME == pInfo->enChannelType)
        {
            m_arrUnits[i]               = L"��s";
            m_arrChannelAmplitude[i]    = 400.0f;
        }
        // �������ͣ�Ĭ�Ϸ�ֵΪ400.0
        else
        {
            m_arrUnits[i]               = L"";
            m_arrChannelAmplitude[i]    = 400.0f;
        }

        // ����ͨ�����͵��벨�οؼ�������ı���
        m_arrChannelType[i] = pInfo->enChannelType;

        csDesc.Format(L"%d: %s", pInfo->nIndex + 1, pInfo->strChannelDesc.c_str());

        // ����ͨ�������Ĳ���ͼ����
        m_ctrlWaveform.SetTitle(i, csDesc);

        // ����ͨ��������XY��̶���Ϣ
        m_ctrlWaveform.SetScaleInterval(i, 
            (float)(m_nSamplePer * pow(2, m_nTimelineT)), 
            m_arrChannelAmplitude[i] * (float)pow(2, m_nWaveformT), 
            m_arrUnits[i]);

        // ���ò����·���X����ʾ��Ϣ
        m_ctrlWaveform.SetXOneGridPointCount(i, (int)(m_nSamplePer1 * (float)pow(2, m_nTimelineT)));

        // ������ѹͨ��������Ҫ����3�����͵���ɫ
        if(CHANNEL_TYPE_SMV_CURRENT == pInfo->enChannelType || 
            CHANNEL_TYPE_SMV_VOLTAGE == pInfo->enChannelType)
        {
            // ����ͨ��������ȡ��Ӧ����ɫ
            switch(pInfo->enChannelPhase)
            {
                // A��
            case CHANNEL_PHASE_A:
                color = DSM_PHASE_COLOR_A;
                break;

                // B��
            case CHANNEL_PHASE_B:
                color = DSM_PHASE_COLOR_B;
                break;

                // C��
            case CHANNEL_PHASE_C:
                color = DSM_PHASE_COLOR_C;
                break;

                // N��
            case CHANNEL_PHASE_N:
                color = DSM_PHASE_COLOR_N;
                break;

                // X��
            case CHANNEL_PHASE_X:
                color = DSM_PHASE_COLOR_X;
                break;

                // δ֪
            default:
                color = DSM_PHASE_COLOR_UN;
                break;
            }
        }
        else
        {
            // ��������ͨ������δ֪��ɫ
            color = DSM_PHASE_COLOR_UN;
        }

        // ����ɫ��Ϣ���õ����οؼ�
        m_ctrlWaveform.SetWaveColor(i, 0, color);
    }

    // ʹ����������Ч
    m_ctrlWaveform.Init();
}

void CSMVWaveformDlg::_setXScale()
{
    ASSERT(m_pCurrentCtrlBlk);

    // ����X���Ĭ�Ͽ̶�ֵ��Ĭ����һ�ܲ��Ĳ�������
    m_nSamplePer = m_pCurrentCtrlBlk->nSampleRate / DSM_WORK_HERTZ_50 / 2;
    m_nSamplePer1 = m_nSamplePer;
#if DSM_DSP_CALC_128K_RESAMPLE_TO_4K
    if (m_pCurrentCtrlBlk->nSampleRate == 12800)
    {
        m_nSamplePer1 = 4000 / DSM_WORK_HERTZ_50 / 2;
    }
#endif
}


/**
 *      ���浱ǰ����ҳ���״̬��Ϣ���Ա��л��ز���ҳ��ʱ����ʾ��һ���뿪ʱ��״̬
 */
void CSMVWaveformDlg::_SavePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);

    CCtrlBlkDataWaveform* pData = dynamic_cast<CCtrlBlkDataWaveform*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));
    ASSERT(pData);

    if(pData)
    {
        pData->m_nTimelineT = m_nTimelineT;     // ʱ�������ű���
        pData->m_nWaveformT = m_nWaveformT;     // ���η�ֵ���ű���
    }
}

/**
 *      ���ݿ��ƿ������ҳ��״̬��Ϣ��������
 *
 *  \return
 *      bool        ��ԭҳ��״̬�ɹ�����true��ʧ�ܷ���false
 *
 *  \note
 *      �ò�����ʾ����ƿ����������Ϣ�������ֵ�����ű���������ʾͨ������Ϣ
 */
bool CSMVWaveformDlg::_restorePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);
    ASSERT(CSMVWaveformDlg::IDD == m_pPageDataExtra->GetPageData()->m_nPageID);

    // ��ȡ��ǰ�Ŀ��ƿ�����
    CCtrlBlkDataWaveform* pData = dynamic_cast<CCtrlBlkDataWaveform*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    // ��ǰ���ƿ���û�������Ϣ���򷵻�
    if(!pData)
    {
        return false;
    }

    m_nPage      = pData->m_nPage;              // ��ǰҳ��ֵ
    m_nTimelineT = pData->m_nTimelineT;         // ʱ�������ű���
    m_nWaveformT = pData->m_nWaveformT;         // ��ֵ���ű���

    // ���ݵ�ǰҳ�������Ҫ��ʾ��ͨ������
    _calcChannel();

    return true;
}

/**
 *      ���ݵ�ǰҳ������������ʾ��ͨ������
 */
void CSMVWaveformDlg::_calcChannel()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);

    // ���㵱ǰ��ʾͨ������
    uint32_t nTotals = DSM_SMV_WAVEFORM_NUMS;
    uint32_t nFirst  = m_nPage * nTotals;            // �����һ����ʾͨ�����
    uint32_t nLast   = nFirst + nTotals;             // ���һ����ʾͨ�����

    // ͨ����Ŀ���٣�������ʼͨ����0��ʼ
    if(nFirst >= m_pCurrentCtrlBlk->nChannelCount)
    {
        nFirst  = 0;
        m_nPage = 0;
        nLast   = nFirst + nTotals;
    }

    nLast = nLast > m_pCurrentCtrlBlk->nChannelCount ? m_pCurrentCtrlBlk->nChannelCount : nLast;

    // ��ȡ��ǰ�Ŀ��ƿ�����
    CCtrlBlkDataWaveform* pData = dynamic_cast<CCtrlBlkDataWaveform*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    // ��ǰ���ƿ��ҳ��״̬��ϢΪ�գ����½�һ��
    if(!pData)
    {
        pData = new CCtrlBlkDataWaveform;
        m_pPageDataExtra->Set(m_pCurrentCtrlBlk->nID, pData);
    }

    // �����������ϢΪ��ͨ���µ�ҳ��״̬��Ϣ

    // ���ÿ��ƿ������ҳ��״̬��Ϣ
    pData->m_nPage = m_nPage;

    for(int i = 0; i < DSM_SMV_WAVEFORM_NUMS; i++)
    {
        uint32_t nChannel = nFirst + i;

        if(nChannel <= nLast)
        {
            m_arrChannels[i] = nChannel;
        }
        else
        {
            m_arrChannels[i] = UINT_MAX;
        }
    }
}


