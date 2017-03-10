/**
 * \file
 *      SMVWaveformDlg.h
 *
 * \brief
 *      ����ͼҳ��ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/19
 */

#pragma once

#include "src/main/smv/smvbasedlg.h"


#include "src/utils/electriccontrol/waveformgramctrl/waveformgramctrl.h"

#define DSM_SMV_WAVEFORM_GRAM_X         1           ///< x�Შ����
#define DSM_SMV_WAVEFORM_GRAM_Y         3           ///< y�Შ����

#define DSM_SMV_WAVEFORM_NUMS           DSM_SMV_WAVEFORM_GRAM_X * DSM_SMV_WAVEFORM_GRAM_Y

class CPageDataExtra;


// CSMVWaveformDlg �Ի���

class CSMVWaveformDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSMVWaveformDlg)

public:
	CSMVWaveformDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSMVWaveformDlg();

// �Ի�������
	enum { IDD = IDD_SMV_WAVEFORM_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnMenuF4();

public:

    /**
     *      ����ҳ��
     */
    virtual void _SavePage();


protected:

    /**
     *      �޸ĵײ��˵�
     */
    virtual void _modifyMenu();

    /**
     *      ���ݲ˵�ID�л���ָ���Ŀ��ƿ�
     *
     *  \param[in]
     *      nCmdID          �˵�ID
     *  \return
     *      bool            �л����ƿ�ɹ�����true
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);


private:
    /**
     *      �������οؼ�
     */ 
    void _createWaveform();

    /**
     *      �������ݲ��õ�����ˢ�²���
     */
    void _updateWaveform();

    /**
     *      �Ϸ�ͨ��
     */
    void _pageupChannel();

    /**
     *      �·�ͨ��
     */
    void _pagedownChannel();

    /**
     *      �Ŵ���
     */
    void _zoominWaveform();

    /**
     *      ��С����
     */
    void _zoomoutWaveform();

    /**
     *      �Ŵ�ʱ���ᣨX�ᣩ
     */
    void _zoominTimeline();

    /**
     *      ��Сʱ���ᣨX�ᣩ
     */
    void _zoomoutTimeline();

    /**
     *      ����ͨ�����������ò���ͼ������
     */
    void _setWaveform();

    /**
     *      ����X��Ĭ�Ͽ̶�
     */
    void _setXScale();

    /**
     *      ���ݿ��ƿ������ҳ��״̬��Ϣ��������
     */
    bool _restorePage();

    /**
     *      ���ݵ�ǰҳ������������ʾ��ͨ������
     */
    void _calcChannel();


private:
    uint32_t                    m_nPage;                ///< ��ǰҳ��ֵ

    // DSM_SMV_WAVEFORM_ZOOM    0x01        ����ģʽ
    // DSM_SMV_WAVEFORM_WATCH   0x02        �۲�ģʽ
    DWORD                       m_dwWaveMode;           ///< ��ǰ����ͼ��ʾģʽ��F3����

    int                         m_nSample;              ///< ��ǰ������
    int                         m_nSamplePer;           ///< ÿ�����Ӳ���������Ĭ��һ������Ϊһ���ܲ��ĵ���
    int                         m_nSamplePer1;          ///< ʵ��ÿ�����ӵĲ���������12.8k���ز���ֻ��4k��������
    float                       m_fVoltage;             ///< ��ѹ�ֵ����ֵ
    float                       m_fAmperage;            ///< �����ֵ����ֵ


    int                         m_nWaveformT;           ///< Y��Ŵ���
    int                         m_nTimelineT;           ///< X��Ŵ���

    // ���������Ǳ��浱ǰҳ������ʾͨ������Ϣ
    float                       m_arrChannelAmplitude[DSM_SMV_WAVEFORM_NUMS];       ///< ��ͨ���ķ�ֵ
    uint32_t                    m_arrChannels[DSM_SMV_WAVEFORM_NUMS];               ///< ��ǰҳ����ʾ��ͨ������
    enumChannelType             m_arrChannelType[DSM_SMV_WAVEFORM_NUMS];            ///< ��ǰҳ����ʾ��ͨ������
    CString                     m_arrUnits[DSM_SMV_WAVEFORM_NUMS];                  ///< ��ǰҳ����ʾ��ͨ�������ݵ�λ

    CDataOscillogram            m_data;                 ///< ��������

    CWaveFormGramCtrl           m_ctrlWaveform;         ///< ���οؼ�

    enumSmvValueType            m_eDispType;            ///< ͨ��ֵ����ʾ����
    enumSmvValueType            m_eFrameType;           ///< �ͨ�����͵���ʾ����
};
