/// @file
///
/// @brief
///     ���ݲ�����ṩ������
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.4.2
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "DsmDataLayer.h"
#include "include\ctrlcode_def.h"
#include "src\service\datalayer\utility\data_utility.h"
#include "src\dspclient\include\Dsp.h"

//������뾯��todo��Ϣ
//#pragma message(__FILE__ "(" PRAGMA_STRING(__LINE__) "): ����Uninitʱ�Ƿ�Ҫ��ֹͣ����ӿ����ݽ���")
#define PRAGMA_STRING2(x) #x
#define PRAGMA_STRING(x) PRAGMA_STRING2(x)

CDsmDataLayer::CDsmDataLayer(void) : m_pFrameSend(NULL)
                                   , m_pFrameRecv(NULL)
                                   , m_pSclManager(NULL)
                                   , m_hGPMDevice(INVALID_HANDLE_VALUE)
                                   , m_pParamConfigBasic(NULL)
                                   , m_pParamConfigSMVSend(NULL)
                                   , m_pParamConfigGOOSESend(NULL)
                                   , m_pParamConfigGOOSERecv(NULL)
                                   , m_pOpticalPowerManager(NULL)
                                   , m_pPcapFileAnalysis(NULL)
{
}

CDsmDataLayer::~CDsmDataLayer(void)
{
}

///
/// @brief
///    ��ȡ���ݲ�Ӧ��ʵ��
///
CDsmDataLayer* CDsmDataLayer::getInstance()
{
    static CDsmDataLayer s_DsmDataLayer;
    return &s_DsmDataLayer;
}

///
/// @brief
///    ��������ֹͣFPGA
///
/// @param[in] bStart ����ֹͣFPGA��true����FPGA��falseֹͣFPGA
///
/// @return
///     true��ʾ��������ֹͣ�ɹ�
///
bool CDsmDataLayer::_StartFPGA( bool bStart )
{
    BOOL bStartFPGA = bStart;
    if (FALSE == DeviceIoControl(m_hGPMDevice, IOCTRL_START_FPGA, &bStartFPGA, sizeof(bStartFPGA), NULL,NULL,NULL,NULL) )
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::_StartFPGA : DeviceIoControl:IOCTRL_START_FPGA failed \n"));
        return false;
    }
    return true;
}

///
/// @brief
///    ����FPGA
///
/// @return
///     true��ʾ�����ɹ�
///
bool CDsmDataLayer::StartFPGA()
{
    return _StartFPGA(true);
}

///
/// @brief
///    �ر�FPGA
///
/// @return
///     true��ʾ�رճɹ�
///
bool CDsmDataLayer::StopFPGA()
{
    return _StartFPGA(false);
}

///
/// @brief
///    ��ʼ�����ݲ���Ҫ������
///
/// @return
///     true��ʾ�رճɹ�
///
bool CDsmDataLayer::Init()
{
    std::wstring        strSrcPathName;
    std::wstring        strDstPathName;

    // GPM����
    if (INVALID_HANDLE_VALUE == m_hGPMDevice)
    {
        //��GPM����
        m_hGPMDevice = CreateFile(L"GPM1:", GENERIC_READ|GENERIC_WRITE, 0, 0,OPEN_ALWAYS, 0, NULL);
        if (INVALID_HANDLE_VALUE == m_hGPMDevice)
        {
            RETAILMSG(1, (_T(" CDsmDataLayer::Init(): Open GPM Device failed\r\n")));
            return false;
        }
    }
    RETAILMSG(1, (_T("Current Gpm Driver Version : %s\r\n"), GetGPMDriverVersion().c_str()));

    //����FPGA���ͽ��չ���
    StartFPGA();

    m_pParamConfigBasic = CParamConfigBasic::getInstance();
    if (NULL != m_pParamConfigBasic)
    {
         m_pParamConfigBasic->LoadConfig(PARAM_CONFIG_BASIC);
    }
    else
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T(" CDsmDataLayer::Init(): CParamConfigBasic::getInstance() Failed \n")));
    }

    // ��ʼ��ȫվ�����ļ�
    m_pSclManager = CSCLManager::getInstance();
    if (NULL != m_pSclManager)
    {
#if CHUN_HUA
        m_pSclManager->Load(GetAppDir() + m_pParamConfigBasic->GetSclFile());
#else
		m_pSclManager->Load(m_pParamConfigBasic->GetSclPath()+m_pParamConfigBasic->GetSclFile());
#endif
    }
    else
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T(" CDsmDataLayer::Init(): CSCLManager::getInstance() Failed \n")));
    }

    // SMV�������ù�����
    m_pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL != m_pParamConfigSMVSend)
    {
        // ���Ŀ�������ļ������ڣ���ô����Դ�����ļ���Ŀ��·��
        strSrcPathName  = GetAppDir() + DEFAULT_CFG_BACKUP + PARAM_CONFIG_SMV_SEND;
        strDstPathName  = GetAppRunTimeCfgDir() + PARAM_CONFIG_SMV_SEND;
        CopyFile(strSrcPathName.c_str(), strDstPathName.c_str(), TRUE);

        // ����Ŀ�������ļ�
        if (!m_pParamConfigSMVSend->LoadConfig(strDstPathName))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T(" CDsmDataLayer::Init(): CParamConfigSMVSend::LoadConfig() Failed \n")));
        }
    }

    // GOOSE�������ù�����
    m_pParamConfigGOOSESend = CParamConfigGOOSESend::getInstance();
    if (NULL != m_pParamConfigGOOSESend)
    {
        // ���Ŀ�������ļ������ڣ���ô����Դ�����ļ���Ŀ��·��
        strSrcPathName  = GetAppDir() + DEFAULT_CFG_BACKUP + PARAM_CONFIG_GOOSE_SEND;
        strDstPathName  = GetAppRunTimeCfgDir() + PARAM_CONFIG_GOOSE_SEND;
        CopyFile(strSrcPathName.c_str(), strDstPathName.c_str(), TRUE);

        // ����Ŀ�������ļ�
        if (!m_pParamConfigGOOSESend->LoadConfig(strDstPathName))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T(" CDsmDataLayer::Init(): CParamConfigGOOSESend::LoadConfig() Failed \n")));
        }
    }

    // GOOSE�������ù�����
    m_pParamConfigGOOSERecv = CParamConfigGOOSERecv::getInstance();
    if (NULL != m_pParamConfigGOOSERecv)
    {
        // ���Ŀ�������ļ������ڣ���ô����Դ�����ļ���Ŀ��·��
        strSrcPathName  = GetAppDir() + DEFAULT_CFG_BACKUP + PARAM_CONFIG_GOOSE_RECV;
        strDstPathName  = GetAppRunTimeCfgDir() + PARAM_CONFIG_GOOSE_RECV;
        CopyFile(strSrcPathName.c_str(), strDstPathName.c_str(), TRUE);

        // ����Ŀ�������ļ�
        if (!m_pParamConfigGOOSERecv->LoadConfig(strDstPathName))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T(" CDsmDataLayer::Init(): CParamConfigGOOSERecv::LoadConfig() Failed \n")));
        }
    }

    // �ֶ��������ù�����
    m_pParamConfigManualTest = CParamconfigManualTest::getInstance();
    if (NULL != m_pParamConfigManualTest)
    {
        // ���Ŀ�������ļ������ڣ���ô����Դ�����ļ���Ŀ��·��
        strSrcPathName  = GetAppDir() + DEFAULT_CFG_BACKUP + PARAM_CONFIG_MANUAL_TEST;
        strDstPathName  = GetAppRunTimeCfgDir() + PARAM_CONFIG_MANUAL_TEST;
        CopyFile(strSrcPathName.c_str(), strDstPathName.c_str(), TRUE);

        // ����Ŀ�������ļ�
        if (!m_pParamConfigManualTest->LoadConfig(strDstPathName))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T(" CDsmDataLayer::Init(): CParamconfigManualTest::LoadConfig() Failed \n")));
        }
    }

    // ״̬�������ù�����
    m_pParamConfigStateSequence = CParamConfigStateSequence::getInstance();
    if (NULL != m_pParamConfigStateSequence)
    {
        // ���Ŀ�������ļ������ڣ���ô����Դ�����ļ���Ŀ��·��
        strSrcPathName  = GetAppDir() + DEFAULT_CFG_BACKUP + PARAM_CONFIG_STATE_SEQUENCE;
        strDstPathName  = GetAppRunTimeCfgDir() + PARAM_CONFIG_STATE_SEQUENCE;
        CopyFile(strSrcPathName.c_str(), strDstPathName.c_str(), TRUE);

        // ����Ŀ�������ļ�
        if (!m_pParamConfigStateSequence->LoadConfig(strDstPathName))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T(" CDsmDataLayer::Init(): CParamConfigStateSequence::LoadConfig() Failed \n")));
        }
    }

    //��ʼ��DSP
    if ( !DspInit())
    {
        RETAILMSG(1, (_T("Dsp Init Failed \r\n")));
        return false;
    }

    if (NULL == m_pFrameRecv)
    {
        m_pFrameRecv = new CFrameRecv(m_hGPMDevice);
        if (NULL == m_pFrameRecv)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CDsmDataLayer::Init(): CDsmDataLayer::Init() new CFrameRecv Failed \n")));
            return false;
        }
        if (! m_pFrameRecv->InitFrameRecv())
        {
            return false;
        }
    }

    // ���ݷ��͹�����
    if (NULL == m_pFrameSend)
    {
        m_pFrameSend = CFrameSend::getInstance();                       ///< ��ȡ���ݷ��͹�����
        if (NULL == m_pFrameSend)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CDsmDataLayer::Init(): CFrameSend get instance failed \n")));
            return false;
        }

        if (!m_pFrameSend->InitFrameSend(m_hGPMDevice))                 ///< ��ʼ�����ݷ��͹�����
        {
            return false;
        }
    }

    // �⹦��
    if (NULL == m_pOpticalPowerManager)
    {
        m_pOpticalPowerManager = COpticalPowerManager::GetInstance();   ///< ��ù⹦�ʹ�����
        if (NULL == m_pOpticalPowerManager)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CDsmDataLayer::Init(): COpticalPowerManager get instance failed \n")));
            return false;
        }

        if (!m_pOpticalPowerManager->Init(m_hGPMDevice))                ///< ��ʼ��
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CDsmDataLayer::Init(): COpticalPowerManager init failed \n")));
            return false;
        }
    }

    //pcap�ļ�ͳ�Ʒ���
    if (NULL == m_pPcapFileAnalysis)
    {
        m_pPcapFileAnalysis = CPcapAnalysis::getInstance();
        if (NULL == m_pPcapFileAnalysis)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CDsmDataLayer::Init(): CPcapAnalysis::getInstance() failed")));
        }
    }

    return true;
}

///
/// @brief
///    ����ʼ�����������ݲ������
///
/// @return
///     true��ʾ����ɹ�
///
bool CDsmDataLayer::UnInit()
{
    if (NULL != m_pFrameRecv)
    {
        m_pFrameRecv->UnInitFrameRecv();
        delete m_pFrameRecv;
        m_pFrameRecv = NULL;
    }

    if (NULL != m_pFrameSend)
    {
        m_pFrameSend->UnInitFrameSend();
        m_pFrameSend = NULL;
    }

    //ж��DSP
    DspRelease();

    //�ر�FPGA�ķ��ͽ��չ���
    StopFPGA();

    //�ر�FPGA�ļ����
    if (INVALID_HANDLE_VALUE != m_hGPMDevice)
    {
        CloseHandle(m_hGPMDevice);
        m_hGPMDevice = INVALID_HANDLE_VALUE;
    }

    return true;
}

///
/// @brief
///    �����˳��͹���ģʽ
///
/// @param[in] bLowPowerMode �����˳��͹���ģʽ��true����͹��ģ�false�˳��͹���ģʽ
///
/// @return
///     true��ʾ��������˳��ɹ�
///
bool CDsmDataLayer::_EnterLowPowerMode( bool bLowPowerMode )
{
    BOOL bEnterLowPowerMode = bLowPowerMode;
    if (FALSE == DeviceIoControl(m_hGPMDevice, IOCTRL_LOWER_POWER, &bEnterLowPowerMode, sizeof(bEnterLowPowerMode), NULL,NULL,NULL,NULL) )
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::_EnterLowPowerMode : DeviceIoControl:IOCTRL_LOWER_POWER failed \n"));
        return false;
    }
    return true;
}

///
/// @brief
///    ����͹���ģʽ
///
/// @return
///     true��ʾ����͹��ĳɹ�
///
bool CDsmDataLayer::EnterLowPowerMode()
{
     return _EnterLowPowerMode(true);
}

///
/// @brief
///    �˳��͹���ģʽ
///
/// @return
///     true��ʾ�˳��͹��ĳɹ�
///
bool CDsmDataLayer::LeaveLowPowerMode()
{
    return _EnterLowPowerMode(false);
}

///
/// @brief
///    �������ݲ㣬Ҫ��ȡ̽����
///    ���ݲ�Ҫȥ���ײ�Ĺ��ˣ���ȡ����ӿڵ����б���
///
void CDsmDataLayer::BeginDetect( NetFt3RecvFrameType enNetRecvFrameType, bool bEnabelDetect /*= true*/ )
{
    if (NULL != m_pFrameRecv)
    {
        m_pFrameRecv->BeginDetect(enNetRecvFrameType, bEnabelDetect);
    }
}

///
/// @brief
///    ��ȡ̽������Ŀ
///
/// @return
///     ����̽��������Ŀ
///
uint32_t CDsmDataLayer::GetNetDetectItemsCount()
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetNetDetectItemsCount();
    }
    return 0;
}

///
/// @brief
///    ��ȡһ��̽����
///
/// @param[in]  nIndex  Ҫ��ȡ̽��������������0��ʼ
///
/// @return
///     CNetDetectItem* ����ָ������̽���������ָ��
///
CNetDetectItem* CDsmDataLayer::GetNetDetectItem( uint32_t nIndex )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetNetDetectItem(nIndex);
    }
    return NULL;
}

///
/// @brief
///    ��ȡ̽������Ŀ
///
/// @return
///     ����̽��������Ŀ
///
uint32_t CDsmDataLayer::GetFt3DetectItemsCount()
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetFt3DetectItemsCount();
    }
    return 0;
}

///
/// @brief
///    ��ȡһ��̽����
///
/// @param[in]  nIndex  Ҫ��ȡ̽��������������0��ʼ
///
/// @return
///     CFt3DetectItem* ����ָ������̽���������ָ��
///
CFt3DetectItem* CDsmDataLayer::GetFt3DetectItem( uint32_t nIndex )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetFt3DetectItem(nIndex);
    }
    return NULL;
}

///
/// @brief
///    ��ȡ1588̽������Ŀ
///
/// @return
/// ����̽��������Ŀ
///
uint32_t CDsmDataLayer::GetNet1588DetectItemsCount()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetNet1588DetectItemsCount();
    }
    return 0;
}

///
/// @brief
///    ��ȡһ��1588̽����
///
/// @param[in]  nIndex  Ҫ��ȡ̽��������������0��ʼ
///
/// @return
///     CNetDetectItem* ����ָ������̽���������ָ��
///
CNet1588DetectItem* CDsmDataLayer::GetNet1588DetectItem( uint32_t nIndex )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetNet1588DetectItem(nIndex);
    }
    return NULL;
}

///
/// @brief
///    SCL�ļ���������ȡ�ӿ�
///
/// @return
///     CSCLManager* ����Scl��������ָ��
///
CSCLManager* CDsmDataLayer::GetSclManager()
{
    return m_pSclManager;
}

///
/// @brief
///    ��ȡ�Ѿ�ѡ�еĿ��ƿ�����
///
/// size_t      �����Ѿ�ѡ�еĿ��ƿ�����
///
size_t CDsmDataLayer::GetSelectedCtrlBlkCount()
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetSelectedCtrlBlkCount();
    }
    return 0;
}


///
/// @brief
///    ���õ�ǰ�Ŀ��ƿ飬dsp����˿��ƿ����Ϣ
///
/// @param[in] pDetectItem  Ҫ���õĵ�ǰ���ƿ�
///
void CDsmDataLayer::SetCurrentCtrlBlk( CBaseDetectItem* pDetectItem  )
{
    if (NULL != m_pFrameRecv)
    {
        m_pFrameRecv->SetCurrentCtrlBlk(pDetectItem);
    }
}

///
/// @brief
///    ��ȡ��ǰ�Ŀ��ƿ�ָ��
///
/// @return
///     CBaseDetectItem*   ���ص�ǰ���ƿ�ָ�룬ʧ�ܷ���Null
///
CBaseDetectItem* CDsmDataLayer::GetCurrentCtrlBlk()
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetCurrentCtrlBlk();
    }
    return NULL;
}

///
/// @brief
///    ��ȡ��ǰ�Ŀ��ƿ�ʵʱͨ����
///
/// @return
///     uint32_t   ���ص�ǰ���ƿ�ʵʱͨ����
///
uint32_t CDsmDataLayer::GetCurrentCtrlBlkRealTimeChannelCounts()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetCurrentCtrlBlkRealTimeChannelCounts();
    }
    return 0;
}


///
/// @brief
///    ���õ�ǰģʽ
///
/// @param[in]  enDsmMode Ҫ���õ�ģʽ
///
void CDsmDataLayer::SetCurrentMode( DSM_OPERATE_MODE enDsmMode )
{
    if(NULL != m_pFrameRecv)
    {
        m_pFrameRecv->SetCurrentMode(enDsmMode);
    }
}

///
/// @brief
///    ��ȡ��ǰģʽ
///
/// @return
///     DSM_OPERATE_MODE    ���ص�ǰϵͳģʽ
///
DSM_OPERATE_MODE CDsmDataLayer::GetCurrentMode()
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetCurrentMode();
    }
    return DSM_MODE_UNKOWN;
}


///
/// @brief
///    ��ȡ��ǰ��̽������
///
/// @return
///     NetFt3RecvFrameType ���ص�ǰ��̽����������
///
NetFt3RecvFrameType CDsmDataLayer::GetCurrentRecvType()
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetCurrentRecvType();
    }
    return RECV_TYPE_UNKOWN;
}

///
/// @brief
///    ���һ��ѡ�еĿ��ƿ�
///
/// @param[in] pDetectItem  Ҫ���õĵ�ǰ���ƿ�
///
/// @return
///     bool    ��ӳɹ�����true
///
bool CDsmDataLayer::AddOneSelectedCtrlBlk( CBaseDetectItem* pDetectItem )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->AddOneSelectedCtrlBlk(pDetectItem);
    }
    return false;
}

///
/// @brief
///    �Ƴ�һ���Ѿ���ӵĿ��ƿ�
///
/// @param[in] pDetectItem  Ҫ�Ƴ��Ŀ��ƿ�
///
void CDsmDataLayer::RemoveOneSelectedCtrlBlk( CBaseDetectItem* pDetectItem )
{
    if (NULL != m_pFrameRecv)
    {
        m_pFrameRecv->RemoveOneSelectedCtrlBlk(pDetectItem);
    }
}

///
/// @brief
///    ��ȡ��һ�����ƿ���Ϣ
///
/// @param[in]  pCurrentCtrlBlk ָ��ǰ�Ŀ��ƿ�
///
/// @return
///     CBaseDetectItem* ��ȡ��һ�����ƿ�ָ�룬û����һ���򷵻�NULL
///
CBaseDetectItem* CDsmDataLayer::GetPrevSelectCtrlBlk( CBaseDetectItem* pCurrentCtrlBlk )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetPrevSelectCtrlBlk(pCurrentCtrlBlk);
    }
    return NULL;
}

///
/// @brief
///    ��ȡ��һ�����ƿ���Ϣ
///
/// @param[in]  pCurrentCtrlBlk ָ��ǰ�Ŀ��ƿ�
///
/// @return
///     CBaseDetectItem* ��ȡ��һ�����ƿ�ָ�룬û����һ���򷵻�NULL
///
CBaseDetectItem* CDsmDataLayer::GetNextSelectCtrlBlk( CBaseDetectItem* pCurrentCtrlBlk )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetNextSelectCtrlBlk(pCurrentCtrlBlk);
    }
    return NULL;
}

///
/// @brief
///    ����id��ȡ�Ѿ�ѡ�еĿ��ƿ�
///
/// @param[in]  nID     ������ɨ���б�ʱʹ�õ�������ȡ���ƿ�ָ��
///
/// @return
///     CBaseDetectItem* ��ȡ����id��Ӧ�Ŀ��ƿ�ָ�룬�������򷵻�NULL
///
CBaseDetectItem* CDsmDataLayer::GetSelectCtrlBlkByID( uint32_t nID )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetSelectCtrlBlkByID(nID);
    }
    return NULL;
}


///
/// @brief
///    ��ȡ��һ�����ƿ���Ϣ
///
/// @return
///     CBaseDetectItem* ��ȡ��һ�����ƿ�ָ�룬û���򷵻�NULL
///
CBaseDetectItem* CDsmDataLayer::GetFirstSelectCtrlBlk()
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetFirstSelectCtrlBlk();
    }
    return NULL;
}

///
/// @brief
///    ��ȡ��Чֵ����
///
/// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
///
/// @return
///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
///
/// @note
///     pData ָ����ڴ������ɵ������Լ�����
///
bool CDsmDataLayer::GetEffectiveVal( CDataEffectiveVal* pData )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetEffectiveVal(pData);
    }
    return false;
}

///
/// @brief
///    ��ȡ����ͼ����
///
/// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
///
/// @return
///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
///
/// @note
///     pData ָ����ڴ������ɵ������Լ�����
///
bool CDsmDataLayer::GetOscillogramData( CDataOscillogram* pData )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetOscillogramData(pData);
    }
    return false;
}

///
/// @brief
///    ��ȡSmv���չ������ýӿ�
///
CSmvRecvConfigPublic* CDsmDataLayer::GetSmvConfigPublic()
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetSmvConfigPublic();
    }
    return NULL;
}

///
/// @brief
///    ��ȡ��������
///
/// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
///
/// @return
///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
///
/// @note
///     pData ָ����ڴ������ɵ������Լ�����
///
bool CDsmDataLayer::GetPowerData( CDataPower* pData )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetPowerData(pData);
    }
    return false;
}

///
/// @brief
///    ��ȡ˫AD����
///
/// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
///
/// @return
///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
///
/// @note
///     pData ָ����ڴ������ɵ������Լ�����
///
bool CDsmDataLayer::GetDoubleAdData( CDataDoubleAD* pData )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetDoubleAdData(pData);
    }
    return false;
}

///
/// @brief
///    ��ȡ״̬��������
///
/// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
///
/// @return
///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
///
/// @note
///     pData ָ����ڴ������ɵ������Լ�����
///
bool CDsmDataLayer::GetStatusWatchData( CDataStatusWatch* pData )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetStatusWatchData(pData);
    }
    return false;
}

///
/// @brief
///    ��ȡ��������
///
/// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
///
/// @return
///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
///
/// @note
///     pData ָ����ڴ������ɵ������Լ�����
///
bool CDsmDataLayer::GetXuliangData( CDataXuliang* pData )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetXuliangData(pData);
    }
    return false;
}

///
/// @brief
///    ��ȡ��������
///
/// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
///
/// @return
///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
///
/// @note
///     pData ָ����ڴ������ɵ������Լ�����
///
bool CDsmDataLayer::GetPhasorData( CDataPhasor* pData )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetPhasorData(pData);
    }
    return false;
}

///
/// @brief
///    ��ȡ��ǰ���ƿ��Ӧ��ͨ��״̬��Ϣ
///
/// @return
///     CSmvCtrlBlkQuality* ���ص�ǰ���ƿ�ͨ����Ϣ��һ������ָ��
///
CSmvCtrlBlkQuality* CDsmDataLayer::GetCurrentCtrlBlkChannelStatus()
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetCurrentCtrlBlkChannelStatus();
    }
    return NULL;
}

///
/// @brief
///    ��ȡг����Ϣ����
///
/// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
///
/// @return
///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
///
/// @note
///     pData ָ����ڴ������ɵ������Լ�����
///
bool CDsmDataLayer::GetHarmonicData( CDataHarmonic* pData )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetHarmonicData(pData);
    }
    return false;
}

///
/// @brief
///    ��ȡ����ͳ��/��ɢ����Ϣ
///
/// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
///
/// @return
///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
///
/// @note
///     pData ָ����ڴ������ɵ������Լ�����
///
bool CDsmDataLayer::GetFrameStatistics( CDataFrameStatistics* pData )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetFrameStatistics(pData);
    }
    return false;
}

///
/// @brief
///    ��ȡ������Ϣ
///
/// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
///
/// @return
///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
///
/// @note
///     pData ָ����ڴ������ɵ������Լ�����
///
bool CDsmDataLayer::GetPolarityData( CDataPolarity* pData )
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->GetPolarityData(pData);
    }
    return false;
}

///
/// @brief
///    �������е�ͨ������
///
void CDsmDataLayer::ResetAllPorlarity()
{
    if (NULL != m_pFrameRecv)
    {
        return m_pFrameRecv->ResetAllPorlarity();
    }
}

///
/// @brief
///    ��ȡ��������/�����������ýӿ�
///
CParamConfigBasic* CDsmDataLayer::GetParamConfigBasic()
{
    return m_pParamConfigBasic;
}

///
/// @brief
///    ���ͳ�����ݣ�����ͳ��
///
/// @return
///     bool    ���óɹ�����true
///
bool CDsmDataLayer::ResetSmvFrameStatistics()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->ResetSmvFrameStatistics();
    }
    return false;
}

///
/// @brief
///    ��ȡ����һ֡��������
///
/// @param[out] pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
///
/// @return
///     bool    ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
///
bool CDsmDataLayer::GetRealTimeMsg( CDataRealTimeMsgData* pData )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetRealTimeMsg(pData);
    }
    return false;
}

///
/// @brief
///    ��ȡ����������Ϣ
///
/// @param[out] pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
///
/// @return
///     bool    ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
///
bool CDsmDataLayer::GetPhaseCheckData( CDataPhaseCheck* pData )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetPhaseCheckData(pData);
    }
    return false;
}

///
/// @brief
///    ��ȡ���ƿ��Ӧ��һ������֡��Ϣ
///
/// @param[in]      pDetectItem     Ҫ��ȡ����֡�Ŀ��ƿ�ָ��
///
/// @return
///     CDetectItemFrame*   ���ؿ��ƿ��Ӧ��һ������֡��NULL��ʾ��ȡʧ��
///
CDetectItemFrame* CDsmDataLayer::GetDetectItemFrame( CBaseDetectItem* pDetectItem )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetDetectItemFrame(pDetectItem);
    }
    return NULL;
}

///
/// @brief
///    ���ú���ʹ�õĵ�1�����ƿ�
///
/// @param[in] pDetectItem  Ҫ���õĿ��ƿ�
///
void CDsmDataLayer::SetPhaseCheckSmv1CtrBlk( CBaseDetectItem* pDetectItem )
{
    if (m_pFrameRecv)
    {
        m_pFrameRecv->SetPhaseCheckSmv1CtrBlk(pDetectItem);
    }
}

///
/// @brief
///    ���ú���ʹ�õĵڶ������ƿ�
///
/// @param[in] pDetectItem  Ҫ���õĵ�ǰ���ƿ�
///
void CDsmDataLayer::SetPhaseCheckSmv2CtrBlk( CBaseDetectItem* pDetectItem )
{
    if (m_pFrameRecv)
    {
        m_pFrameRecv->SetPhaseCheckSmv2CtrBlk(pDetectItem);
    }
}

///
/// @brief
///    ��ȡ����ʹ�õĵ�1�����ƿ�
///
/// @return
///     CBaseDetectItem*   ���ص�ǰ���ƿ�ָ�룬ʧ�ܷ���Null
///
CBaseDetectItem* CDsmDataLayer::GetPhaseCheckSmv1CtrlBlk()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetPhaseCheckSmv1CtrlBlk();
    }
    return NULL;
}

///
/// @brief
///    ��ȡ����ʹ�õĵڶ������ƿ�
///
/// @return
///     CBaseDetectItem*   ���ص�ǰ���ƿ�ָ�룬ʧ�ܷ���Null
///
CBaseDetectItem* CDsmDataLayer::GetPhaseCheckSmv2CtrlBlk()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetPhaseCheckSmv2CtrlBlk();
    }
    return NULL;
}

///
/// @brief
///    ��ȡGOOSEʵʱֵ
///
/// @param[out] pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
///
/// @return
///     bool    ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
///
bool CDsmDataLayer::GetGooseRealTimeValue( CGooseInfo* pData )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetGooseRealTimeValue(pData);
    }
    return false;
}

///
/// @brief
///    ��ȡGOOSEʵʱ����
///
/// @param[out] pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
///
/// @return
///     bool    ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
///
bool CDsmDataLayer::GetGooseRealTimeMsg( CDataRealTimeMsgData* pData )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetGooseRealTimeMsg(pData);
    }
    return false;
}

///
/// @brief
///    ��ȡGOOSE����ͳ����Ϣ
///
/// @param[out] pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
///
/// @return
///     bool    ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
///
bool CDsmDataLayer::GetGooseFrameStatistics( CGooseFrameStatistics* pData )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetGooseFrameStatistics(pData);
    }
    return false;
}

///
/// @brief
///    ��ȡ�����汾�����ذ汾�ַ��� �� 2.34
///
std::wstring CDsmDataLayer::GetGPMDriverVersion()
{
    if (INVALID_HANDLE_VALUE != m_hGPMDevice)
    {
        uint16_t uBootVer[2];
        wchar_t  wsVer[10] = {0};
        if (DeviceIoControl(m_hGPMDevice, IOCTRL_GET_DRV_VERSION, NULL, NULL, &uBootVer,sizeof(uBootVer),NULL,NULL))
        {
            _snwprintf_s(wsVer, 10, 10, L"%x.%02x",(uBootVer[0]>>8)&0xF, (uBootVer[0]&0xFF));
            return wsVer;
        }
    }
    return L"0.00";
}

///
/// @brief
///    ��ȡFPGA�汾�����ذ汾�ַ��� �� 2.34
///
std::wstring CDsmDataLayer::GetGPMFPGAVersion()
{
    if (INVALID_HANDLE_VALUE != m_hGPMDevice)
    {
        uint16_t uBootVer[2];
        wchar_t  wsVer[10] = {0};
        if (DeviceIoControl(m_hGPMDevice, IOCTRL_GET_DRV_VERSION, NULL, NULL, &uBootVer,sizeof(uBootVer),NULL,NULL))
        {
            _snwprintf_s(wsVer, 10, 10, L"%x.%02x",(uBootVer[1]>>8)&0xF, (uBootVer[1]&0xFF));
            return wsVer;
        }
    }
    return L"0.00";
}


///
/// @brief
///    ��ȡdsp�汾��Ϣ
///
uint8_t CDsmDataLayer::GetDspVersion()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetDspVersion();
    }
    return 0;
}

///
/// @brief
///    ���Goose����ͳ�����ݣ�����ͳ��
///
/// @return
///     bool    ���óɹ�����true
///
bool CDsmDataLayer::ResetGooseFrameStatistics()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->ResetGooseFrameStatistics();
    }
    return false;
}

///
/// @brief
///    ��ȡGOOSE��λ��Ϣ
///
/// @param[out] pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ���������� 
///
/// @return
///     bool    ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
///
bool CDsmDataLayer::GetGooseChangeList( CDataGooseChangeList* pData )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetGooseChangeList(pData);
    }
    return false;
}

///
/// @brief
///    ����¼���ļ���С,��Ҫ�ڿ�ʼ¼��ǰ����
///
/// @return
///     bool    ���óɹ�����true
///
bool CDsmDataLayer::SetRecordWaveSize( uint64_t nSizeInByte )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->SetRecordWaveSize(nSizeInByte);
    }
    return false;
}

///
/// @brief
///    ��ȡ���õ�¼���ļ���С
///
uint64_t CDsmDataLayer::GetRecordWaveSize()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetRecordWaveSize();
    }
    return 0;
}
#if !CHUN_HUA
///
/// @brief
///    ����SD����С
///
void CDsmDataLayer::SetSDCardSize(uint64_t SDCardFreeSize)
{
	if (m_pFrameRecv)
	{
		m_pFrameRecv->SetSDCardSize(SDCardFreeSize);
	}	
}

#endif

///
/// @brief
///    ����¼���ļ�·��,��Ҫ�ڿ�ʼ¼��ǰ����
///
void CDsmDataLayer::SetRecordWavePath( const std::wstring& strRecordWavePath )
{
    if (m_pFrameRecv)
    {
        m_pFrameRecv->SetRecordWavePath(strRecordWavePath);
    }
}

///
/// @brief
///    ��ȡ���õ�¼���ļ�·��
///
std::wstring CDsmDataLayer::GetRecordWavePath()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetRecordWavePath();
    }
    return L"";
}


///
/// @brief
///    ����֪ͨ����ʱʹ�õĴ���
///
bool CDsmDataLayer::SetNotifyWnd(CWnd* pWnd)
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->SetNotifyWnd(pWnd);
    }
    return false;
}

///
/// @brief
///    ��ȡ���õ�֪ͨ����ʹ�õ�Cwnd����
///
CWnd* CDsmDataLayer::GetNotifyWnd()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetNotifyWnd();
    }
    return NULL;
}

///
/// @brief
///    ���¼�����ƿ�,��Ҫ�����������֡���������ֵ��Goose��
///
bool CDsmDataLayer::AddRecordItem( CBaseDetectItem* pRecordItem )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->AddRecordItem(pRecordItem);
    }
    return false;
}

///
/// @brief
///    ��¼���б���Ŀ���Ƴ�һ�����ƿ�
///
bool CDsmDataLayer::RemoveRecordItem( CBaseDetectItem* pRecordItem )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->RemoveRecordItem(pRecordItem);
    }
    return false;
}

///
/// @brief
///    ��ʼ¼�������ֵ���ݣ���pcap��ʽ����
///
bool CDsmDataLayer::StartRecordNetSmv()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->StartRecordNetSmv();
    }
    return false;
}

///
/// @brief
///    ��ʼ¼Goose���ģ�����pcap��ʽ����
///
bool CDsmDataLayer::StartRecordGoose()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->StartRecordGoose();
    }
    return false;
}

///
/// @brief
///    ��ʼ¼FT3����֡
///
bool CDsmDataLayer::StartRecordFt3()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->StartRecordFt3();
    }
    return false;
}

///
/// @brief
///    ֹͣ¼��
///
/// @param[out]     ��¼ʧ��ʱ��ʱʧ��ԭ��
///
/// @return
///     bool ֹͣ�ɹ�
///
bool CDsmDataLayer::StopRecordWave(uint32_t& nResult)
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->StopRecordWave(nResult);
    }
    return false;
}

///
/// @brief
///    ȡ����ǰ¼��
///
/// @return
///      ȡ���ɹ�����true
///
bool CDsmDataLayer::CancelRecordWave()
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->CancelRecordWave();
    }
    return false;
}

///
/// @brief
///    ��ȡB���ʱʱ����Ϣ
///
/// @param[out] pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
///
/// @return
///     bool    ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
///
bool CDsmDataLayer::GetTimeSyncIRIGB( CDataTimeSyncIRIGB* pData )
{
    if (INVALID_HANDLE_VALUE != m_hGPMDevice && IsPortActive(LIGHT_PORT_IRIG_B))
    {
         /*
         *  B�������Ԫ
         *  Bit8~bit0��ӦB���������P6��9λ��
         *  Bit15~bit9��ӦB���������P7�ĵ�6λbit5~bit0
         */
        uint16_t nIrigBCtrlCode = 0;
        if (!DeviceIoControl(m_hGPMDevice, IOCTRL_GET_IRIGB_CONTROL, NULL, NULL, &nIrigBCtrlCode, sizeof(nIrigBCtrlCode), NULL, NULL) )
        {
            RETAILMSG(1, (_T("DeviceIoControl IOCTRL_GET_IRIGB_CONTROL Failed !\r\n")));
            return false;
        }

        if (pData)
        {
            pData->ResetData();
            UTC_TIME_t stUtcTime;
            if(GetFpgaUtcTime(&stUtcTime))
            {
                SYSTEMTIME sysTime = UtcTime2SystemTime(stUtcTime);
                pData->SetDateTime(&sysTime);
            }

            pData->SetDST(IRIGB_GET_DST(nIrigBCtrlCode));
            pData->SetDSP(IRIGB_GET_DSP(nIrigBCtrlCode));
            pData->SetLS(IRIGB_GET_LS(nIrigBCtrlCode));
            pData->SetLSP(IRIGB_GET_LSP(nIrigBCtrlCode));
            pData->SetTimeQuality(IRIGB_GET_QUALITY(nIrigBCtrlCode));

            float fOffset = static_cast<float>(IRIGB_GET_OFFSET(nIrigBCtrlCode));
            if (IRIGB_GET_OFFSET_0_5(nIrigBCtrlCode))
            {
                fOffset += 0.5;
            }
            if (IRIGB_GET_OFFSET_SIGN(nIrigBCtrlCode))
            {
                fOffset = -fOffset;
            }
            pData->SetTimeOffset(fOffset);

            return true;
        }
    }
    return false;
}

///
/// @brief
///    �������ݲ㣬��ѡ��Ŀ��ƿ��Ѿ�������
///
void CDsmDataLayer::AddSelectedCtrlBlkFinished()
{
    if (m_pFrameRecv)
    {
        m_pFrameRecv->AddSelectedCtrlBlkFinished();
    }
}

///
/// @brief
///    ��ʼ�ֶ�����SMV���͵��߳�
///
UINT StartManualTestSMVThread(LPVOID pParam)
{
    // ������ָ��
    if (NULL == pParam)
    {
        return FALSE;
    }

    // �����̲߳���
    SendThreadParam stParam = *(SendThreadParam*)pParam;
    if (NULL == stParam.m_pFrameSend)
    {
        return FALSE;
    }

    // ����SMV���ƿ�
    enumErrorCode   eRet = stParam.m_pFrameSend->StartManualTestSMV(stParam.m_hWnd);
    if (ERROR_CODE_SUCCESS != eRet)
    {
        // �򴰿ڷ���֪ͨ��Ϣ
        PostMessage(stParam.m_hWnd, WM_MANUAL_TEST_RESULT, eRet, 0);
        return FALSE;
    }

    return TRUE;
}

///
/// @brief
///    ��ʼ�����ֶ�����SMV���ƿ�
///
/// @param[in]  hWnd  ���ھ������DIֵ�����仯ʱ���ᷢ��WM_DI_CHANGED��Ϣ���˴���
///
/// @return
///     bool    �ɹ�����true
///
bool CDsmDataLayer::StartManualTestSMV( HWND hWnd )
{
    // �жϷ��͹������Ƿ��ѳ�ʼ��
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::SendSMVCbs(): Data sent manager uninit \n"));
        return false;
    }

    // �жϴ��ھ���Ƿ���һ�����ڵĴ���
    if (!IsWindow(hWnd))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::SendSMVCbs(): Parameter error \n"));
        return false;
    }

    // ׼�����̲߳���
    SendThreadParam stParam;
    stParam.m_pFrameSend    = m_pFrameSend;
    stParam.m_hWnd          = hWnd;

    // �������̣߳���ʼ״̬Ϊ��ͣ
    CWinThread* pThread = AfxBeginThread(StartManualTestSMVThread, &stParam, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
    if (NULL == pThread)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::SendSMVCbs(): AfxBeginThread failed \n"));
        return false;
    }

    // �����߳����ȼ�
    CeSetThreadPriority(pThread->m_hThread, DATA_RECV_PROCESS_THREAD_PRIO);

    // ��ʼ�������߳�
    pThread->ResumeThread();

    // ����һ��ʱ�䣬�����߳̿�������
    Sleep(100);
    return true;
}

///
/// @brief
///    �ֶ�����SMV���ݸ����߳�
///
UINT UpateManualTestSMVThread(LPVOID pParam)
{
    // ������ָ��
    if (NULL == pParam)
    {
        return FALSE;
    }

    // �����̲߳���
    SendThreadParam stParam = *(SendThreadParam*)pParam;
    if (NULL == stParam.m_pFrameSend)
    {
        return FALSE;
    }

    // �����ֶ�����SMV���ƿ�
    if (!stParam.m_pFrameSend->UpateManualTestSMV())
    {
        return FALSE;
    }

    return TRUE;
}

///
/// @brief
///    �����ֶ�����SMV���͵�����
///
/// @return
///     bool    �ɹ�����true
///
bool CDsmDataLayer::UpdateManualTestSMV()
{
    // �жϷ��͹������Ƿ��ѳ�ʼ��
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::UpdateSMVSend(): Data sent manager uninit \n"));
        return false;
    }

    // ׼�����̲߳���
    SendThreadParam stParam;
    stParam.m_pFrameSend    = m_pFrameSend;
    stParam.m_hWnd          = NULL;

    // �������̣߳���ʼ״̬Ϊ��ͣ
    CWinThread* pThread = AfxBeginThread(UpateManualTestSMVThread, &stParam, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
    if (NULL == pThread)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::UpdateSMVSend(): AfxBeginThread failed \n"));
        return false;
    }

    // �����߳����ȼ�
    CeSetThreadPriority(pThread->m_hThread, DATA_RECV_PROCESS_THREAD_PRIO);

    // ��ʼ�������߳�
    pThread->ResumeThread();

    // ����һ��ʱ�䣬�����߳̿�������
    Sleep(100);
    return true;
}

///
/// @brief
///    ֹͣ�ֶ�����SMV����
///
bool CDsmDataLayer::StopManualTestSMV()
{
    // �жϷ��͹������Ƿ��ѳ�ʼ��
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::StopSMVSend(): Data sent manager uninit \n"));
        return false;
    }

    // ֹͣSMV����
    return m_pFrameSend->StopManualTestSMV();
}

///
/// @brief
///    �ֶ����鷢��Goose���ݵ��߳�
///
UINT StartManualTestGooseThread(LPVOID pParam)
{
    // ������ָ��
    if (NULL == pParam)
    {
        return FALSE;
    }

    // �����̲߳���
    SendThreadParam stParam = *(SendThreadParam*)pParam;
    if (NULL == stParam.m_pFrameSend)
    {
        return FALSE;
    }

    // ����Goose���ƿ�
    enumErrorCode eRet = stParam.m_pFrameSend->StartManualTestGoose(stParam.m_hWnd);
    if (ERROR_CODE_SUCCESS != eRet)
    {
        // �򴰿ڷ���֪ͨ��Ϣ
        PostMessage(stParam.m_hWnd, WM_MANUAL_TEST_RESULT, eRet, 0);
        return FALSE;
    }

    return TRUE;
}

///
/// @brief
///    ��ʼ�����ֶ�����Goose���ƿ�
///
/// @param[in]  hWnd  ���ھ������DIֵ�����仯ʱ���ᷢ��WM_DI_CHANGED��Ϣ���˴���
///
/// @return
///     bool    ���������̳߳ɹ�����true
///
bool CDsmDataLayer::StartManualTestGoose( HWND hWnd )
{
    // �жϷ��͹������Ƿ��ѳ�ʼ��
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::SendGooseCbs(): Data sent manager uninit \n"));
        return false;
    }

    // �жϴ��ھ���Ƿ���һ�����ڵĴ���
    if (!IsWindow(hWnd))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::SendGooseCbs(): Parameter error \n"));
        return false;
    }

    // ׼�����̲߳���
    SendThreadParam stParam;
    stParam.m_pFrameSend    = m_pFrameSend;
    stParam.m_hWnd          = hWnd;

    // �������̣߳���ʼ״̬Ϊ��ͣ
    CWinThread* pThread = AfxBeginThread(StartManualTestGooseThread, &stParam, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
    if (NULL == pThread)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::SendGooseCbs(): AfxBeginThread failed \n"));
        return false;
    }

    // �����߳����ȼ�
    CeSetThreadPriority(pThread->m_hThread, DATA_RECV_PROCESS_THREAD_PRIO);

    // ��ʼ�������߳�
    pThread->ResumeThread();

    // ����һ��ʱ�䣬�����߳̿�������
    Sleep(100);
    return true;
}

///
/// @brief
///    �����ֶ�����GOOSE���ݵ��߳�
///
UINT UpateManualTestGooseThread(LPVOID pParam)
{
    // ������ָ��
    if (NULL == pParam)
    {
        return FALSE;
    }

    // �����̲߳���
    SendThreadParam stParam = *(SendThreadParam*)pParam;
    if (NULL == stParam.m_pFrameSend)
    {
        return FALSE;
    }

    // ����Goose���ƿ�
    if (!stParam.m_pFrameSend->UpateManualTestGoose())
    {
        return FALSE;
    }

    return TRUE;
}

///
/// @brief
///    �����ֶ�����Goose���͵�����
///
/// @return
///     bool    �ɹ�����true
///
bool CDsmDataLayer::UpdateManualTestGoose()
{
    // �жϷ��͹������Ƿ��ѳ�ʼ��
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::UpdateGooseSend(): Data sent manager uninit \n"));
        return false;
    }

    // ׼�����̲߳���
    SendThreadParam stParam;
    stParam.m_pFrameSend    = m_pFrameSend;
    stParam.m_hWnd          = NULL;

    // �������̣߳���ʼ״̬Ϊ��ͣ
    CWinThread* pThread = AfxBeginThread(UpateManualTestGooseThread, &stParam, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
    if (NULL == pThread)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::UpdateGooseSend(): AfxBeginThread failed \n"));
        return false;
    }

    // �����߳����ȼ�
    CeSetThreadPriority(pThread->m_hThread, DATA_RECV_PROCESS_THREAD_PRIO);

    // ��ʼ�������߳�
    pThread->ResumeThread();

    // ����һ��ʱ�䣬�����߳̿�������
    Sleep(100);
    return true;
}

///
/// @brief
///    ֹͣ�ֶ�����Goose����
///
bool CDsmDataLayer::StopManualTestGoose()
{
    // �жϷ��͹������Ƿ��ѳ�ʼ��
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::StopGooseSend(): Data sent manager uninit \n"));
        return false;
    }

    // ֹͣGoose����
    return m_pFrameSend->StopManualTestGoose();
}

///
/// @brief
///    ����ֶ�������������
///
/// @return
///     unsigned int    �ֶ�������������
///
unsigned int CDsmDataLayer::GetManualTestResultCount()
{
    // �жϷ��͹������Ƿ��ѳ�ʼ��
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::GetManualTestResultCount(): Data sent manager uninit \n"));
        return 0;
    }

    return m_pFrameSend->GetManualTestResultCount();
}

///
/// @brief
///    ���ָ�������ŵ��ֶ�������������
///
/// @param[in]  nIndex    �����������(��0��ʼ����)
///
/// @return
///     CStateTestResult*    ���ָ��״̬���Խ���Ķ���ʧ��ʱ����NULL
///
CStateTestResult* CDsmDataLayer::GetManualTestResult( unsigned int nIndex )
{
    // �жϷ��͹������Ƿ��ѳ�ʼ��
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::GetManualTestResult(): Data sent manager uninit \n"));
        return 0;
    }

    return m_pFrameSend->GetManualTestResult(nIndex);
}

///
/// @brief
///    ���ָ���ֶ��������������������
///
/// @param[in]  pCurResult    �ֶ�������������
///
/// @return
///     int    ����������ţ���0��ʼ������ʧ��ʱ����-1
///
int CDsmDataLayer::GetManualTestResultIndex( CStateTestResult* pCurResult )
{
    // �жϷ��͹������Ƿ��ѳ�ʼ��
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::GetManualTestResultIndex(): Data sent manager uninit \n"));
        return 0;
    }

    return m_pFrameSend->GetManualTestResultIndex(pCurResult);
}

///
/// @brief
///    �����һ���ֶ�������������
///
/// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
///
/// @return
///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
///
CStateTestResult* CDsmDataLayer::NextManualTestResult( CStateTestResult* pCurTestResult )
{
    // ����������
    if (NULL == pCurTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::NextManualTestResult(): Parameter error \n"));
        return NULL;
    }

    // �жϷ��͹������Ƿ��ѳ�ʼ��
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::NextManualTestResult(): Data sent manager uninit \n"));
        return NULL;
    }

    return m_pFrameSend->NextManualTestResult(pCurTestResult);
}

///
/// @brief
///    �����һ���ֶ�������������
///
/// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
///
/// @return
///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
///
CStateTestResult* CDsmDataLayer::PrevManualTestResult( CStateTestResult* pCurTestResult )
{
    // ����������
    if (NULL == pCurTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::PrevManualTestResult(): Parameter error \n"));
        return NULL;
    }

    // �жϷ��͹������Ƿ��ѳ�ʼ��
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::PrevManualTestResult(): Data sent manager uninit \n"));
        return NULL;
    }

    return m_pFrameSend->PrevManualTestResult(pCurTestResult);
}

///
/// @brief
///    �����һ������SMV����ͨ����Ϣ���ֶ�������������
///
/// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
///
/// @return
///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
///
CStateTestResult* CDsmDataLayer::NextManualTestSMVResult( CStateTestResult* pCurTestResult )
{
    // ����������
    if (NULL == pCurTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::NextManualTestSMVResult(): Parameter error \n"));
        return NULL;
    }

    // �жϷ��͹������Ƿ��ѳ�ʼ��
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::NextManualTestSMVResult(): Data sent manager uninit \n"));
        return NULL;
    }

    return m_pFrameSend->NextManualTestSMVResult(pCurTestResult);
}

///
/// @brief
///    �����һ������SMV����ͨ����Ϣ���ֶ�������������
///
/// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
///
/// @return
///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
///
CStateTestResult* CDsmDataLayer::PrevManualTestSMVResult( CStateTestResult* pCurTestResult )
{
    // ����������
    if (NULL == pCurTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::PrevManualTestSMVResult(): Parameter error \n"));
        return NULL;
    }

    // �жϷ��͹������Ƿ��ѳ�ʼ��
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::PrevManualTestSMVResult(): Data sent manager uninit \n"));
        return NULL;
    }

    return m_pFrameSend->PrevManualTestSMVResult(pCurTestResult);
}

///
/// @brief
///    �����һ������GOOSE���ƿ���ֶ�������������
///
/// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
///
/// @return
///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
///
CStateTestResult* CDsmDataLayer::NextManualTestGooseResult( CStateTestResult* pCurTestResult )
{
    // ����������
    if (NULL == pCurTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::NextManualTestGooseResult(): Parameter error \n"));
        return NULL;
    }

    // �жϷ��͹������Ƿ��ѳ�ʼ��
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::NextManualTestGooseResult(): Data sent manager uninit \n"));
        return NULL;
    }

    return m_pFrameSend->NextManualTestGooseResult(pCurTestResult);
}

///
/// @brief
///    �����һ������GOOSE���ƿ���ֶ�������������
///
/// @param[in]  pCurTestResult    ָ��ǰ����������ָ��
///
/// @return
///     CStateTestResult*    ָ����һ������������ָ�룬ʧ��ʱ����NULL
///
CStateTestResult* CDsmDataLayer::PrevManualTestGooseResult( CStateTestResult* pCurTestResult )
{
    // ����������
    if (NULL == pCurTestResult)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::PrevManualTestGooseResult(): Parameter error \n"));
        return NULL;
    }

    // �жϷ��͹������Ƿ��ѳ�ʼ��
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::PrevManualTestGooseResult(): Data sent manager uninit \n"));
        return NULL;
    }

    return m_pFrameSend->PrevManualTestGooseResult(pCurTestResult);
}

///
/// @brief
///    ״̬���������߳�
///
UINT StateSequenceTestThread(LPVOID pParam)
{
    // ������ָ��
    if (NULL == pParam)
    {
        return FALSE;
    }
    SendThreadParam stParam = *(SendThreadParam*)pParam;                ///< �����̲߳���

    // ������ݷ��͹�����ָ��
    if (NULL == stParam.m_pFrameSend)
    {
        return FALSE;
    }

    // ��ʼ״̬���в���
    enumErrorCode eRet = stParam.m_pFrameSend->StartStateSequenceTest(stParam.m_hWnd);

    // ��Ի�������Ϣ
    PostMessage(stParam.m_hWnd, WM_STATUS_SEQUENCE_RESULT, eRet, 0);
    return TRUE;
}

///
/// @brief
///    ��ʼ״̬���в���
///
/// @param[in]  hWnd  ���ھ������DIֵ�����仯ʱ���ᷢ��WM_DI_CHANGED��Ϣ���˴���
///                             ����Ҫѯ���û��Ƿ��ֶ��л�ʱ���ᷢ��WM_WHETHER_SWITCH��Ϣ���˴���
///                             ��״̬�л��󣬻ᷢ��WM_STATUS_CHANGED��Ϣ���˴���
///                             ��������ɺ󣬻ᷢ��WM_TEST_COMPLETED��Ϣ���˴���
///
/// @return
///     bool    ���������̳߳ɹ�����true
///
bool CDsmDataLayer::StartStateSequenceTest( HWND hWnd )
{
    // �жϷ��͹������Ƿ��ѳ�ʼ��
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::StartStateSequenceTest(): Data sent manager uninit \n"));
        return false;
    }

    // �жϴ��ھ���Ƿ���һ�����ڵĴ���
    if (!IsWindow(hWnd))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::StartStateSequenceTest(): Parameter error \n"));
        return false;
    }

    // ׼�����̲߳���
    SendThreadParam stParam;
    stParam.m_pFrameSend    = m_pFrameSend;
    stParam.m_hWnd          = hWnd;

    // �������̣߳���ʼ״̬Ϊ��ͣ
    CWinThread* pThread = AfxBeginThread(StateSequenceTestThread, &stParam, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
    if (NULL == pThread)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::StartStateSequenceTest(): AfxBeginThread failed \n"));
        return false;
    }

    // �����߳����ȼ�
    CeSetThreadPriority(pThread->m_hThread, DATA_RECV_PROCESS_THREAD_PRIO);

    // ��ʼ�������߳�
    pThread->ResumeThread();

    // ����һ��ʱ�䣬�����߳̿�������
    Sleep(100);
    return true;
}

///
/// @brief
///    ֹͣ״̬���в���
///
bool CDsmDataLayer::StopStateSequenceTest()
{
    // �жϷ��͹������Ƿ��ѳ�ʼ��
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::StopStateSequenceTest(): Data sent manager uninit \n"));
        return false;
    }

    return m_pFrameSend->StopStateSequenceTest();
}

///
/// @brief
///    ���״̬���н��������
///
/// @return
///     unsigned int    ״̬���н��������
///
unsigned int CDsmDataLayer::GetStateSequenceResultCount()
{
    // �жϷ��͹������Ƿ��ѳ�ʼ��
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::GetStateSequenceResultCount(): Data sent manager uninit \n"));
        return 0;
    }

    return m_pFrameSend->GetStateSequenceResultCount();
}

///
/// @brief
///    ���ָ��״̬�ŵ�״̬���н��������
///
/// @param[in]  iStateIndex    ״̬��(��0��ʼ����)
///
/// @return
///     CStateTestResult*    ���ָ��״̬���Խ���Ķ���ʧ��ʱ����NULL
///
CStateTestResult* CDsmDataLayer::GetStateSequenceResult( unsigned int iStateIndex )
{
    // �жϷ��͹������Ƿ��ѳ�ʼ��
    if ((NULL == m_pFrameSend) || !m_pFrameSend->IsInited())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CDsmDataLayer::GetStateSequenceResult(): Data sent manager uninit \n"));
        return 0;
    }

    return m_pFrameSend->GetStateSequenceResult(iStateIndex);
}

///
/// @brief
///    ��ʼ�����ض���������
///
/// @param[enRecvType]  ���ý����������ͣ�goose��smv��ft3
/// @param[enMode]      ��������Ĭ�Ͻ����ģʽ
///
/// @note
///     ���ý��պ󣬶�����˿ڻ����Detect�ӿڽ���˵���������ݣ�û�й��ˣ�����FT3��ʼ���ݽ���
///
void CDsmDataLayer::StartRecvData( NetFt3RecvFrameType enRecvType, DSM_OPERATE_MODE enMode /*= DSM_MODE_UNKOWN*/ )
{
    if (m_pFrameRecv)
    {
        m_pFrameRecv->StartRecvData(enRecvType, enMode);
    }
}

///
/// @brief
///    ֹͣ���ݽ���
///
void CDsmDataLayer::StopRecvData()
{
    if (m_pFrameRecv)
    {
        m_pFrameRecv->StopRecvData();
    }
}

///
/// @brief
///    ��ȡSmv1����ͨ��
///
/// @param[out]  nPhaseA ����A��ͨ����
/// @param[out]  nPhaseB ����B��ͨ����
/// @param[out]  nPhaseC ����C��ͨ����
///
/// return
///     bool    ��ȡ�ɹ�����true
///
bool CDsmDataLayer::GetSmv1ABCChannles( uint32_t& nPhaseA, uint32_t& nPhaseB, uint32_t& nPhaseC )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetSmv1ABCChannles(nPhaseA, nPhaseB, nPhaseC);
    }
    return false;
}

///
/// @brief
///    ����Smv1�����ѹ/����ͨ��
///
/// @param[in]  nPhaseA A��ͨ����
/// @param[in]  nPhaseB B��ͨ����
/// @param[in]  nPhaseC C��ͨ����
///
/// return
///     bool    ���óɹ�����true
///
bool CDsmDataLayer::SetSmv1ABCChannles( uint32_t nPhaseA, uint32_t nPhaseB, uint32_t nPhaseC )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->SetSmv1ABCChannles(nPhaseA, nPhaseB, nPhaseC);
    }
    return false;
}

///
/// @brief
///    ��ȡSmv2����ͨ��
///
/// @param[out]  nPhaseA ����A��ͨ����
/// @param[out]  nPhaseB ����B��ͨ����
/// @param[out]  nPhaseC ����C��ͨ����
///
/// return
///     bool    ��ȡ�ɹ�����true
///
bool CDsmDataLayer::GetSmv2ABCChannles( uint32_t& nPhaseA, uint32_t& nPhaseB, uint32_t& nPhaseC )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetSmv2ABCChannles(nPhaseA, nPhaseB, nPhaseC);
    }
    return false;
}

///
/// @brief
///    ����Smv2�����ѹ/����ͨ��
///
/// @param[in]  nPhaseA A��ͨ����
/// @param[in]  nPhaseB B��ͨ����
/// @param[in]  nPhaseC C��ͨ����
///
/// return
///     bool    ���óɹ�����true
///
bool CDsmDataLayer::SetSmv2ABCChannles( uint32_t nPhaseA, uint32_t nPhaseB, uint32_t nPhaseC )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->SetSmv2ABCChannles(nPhaseA, nPhaseB, nPhaseC);
    }
    return false;
}

///
/// @brief
///    ��ȡFpgaʱ��ͬ����ʽ
///
FpgaTimeSyncMode CDsmDataLayer::GetFpgaTimeSyncMode()
{
    if (INVALID_HANDLE_VALUE != m_hGPMDevice)
    {
        FpgaTimeSyncMode enTimeSyncMode;
        if (DeviceIoControl(m_hGPMDevice, IOCTRL_GET_SYNCTIME_MODE, NULL, NULL, &enTimeSyncMode, sizeof(enTimeSyncMode), NULL ,NULL) == FALSE)
        {
            RETAILMSG(1, (_T("CDsmDataLayer::GetFpgaTimeSyncMode() IOCTRL_GET_SYNCTIME_MODE failed\r\n")));
            return FPGA_TIME_SYNC_NONE;
        }
        return enTimeSyncMode;
    }
    return FPGA_TIME_SYNC_NONE;
}


///
/// @brief
///    ����Fpgaʱ��ͬ����ʽ
///
/// @param[in]  enMode Ҫ���õĶ�ʱģʽ
///
/// @return
///     bool ���óɹ�����true
///
bool CDsmDataLayer::SetFpgaTimeSyncMode( FpgaTimeSyncMode enMode )
{
    if (INVALID_HANDLE_VALUE != m_hGPMDevice)
    {
        if (DeviceIoControl(m_hGPMDevice, IOCTRL_SET_SYNCTIME_MODE, &enMode, sizeof(enMode), NULL, NULL, NULL ,NULL) == FALSE)
        {
            RETAILMSG(1, (_T("CDsmDataLayer::SetFpgaTimeSyncMode() IOCTRL_SET_SYNCTIME_MODE failed\r\n")));
            return false;
        }
        return true;
    }
    return false;
}


///
/// @brief
///    ��ȡFPGAʱ��
///
/// @param[out] pUtcTime    �����ȡ����utcʱ��
///
/// @return
///     bool    ��ȡ�ɹ�����true
///
bool CDsmDataLayer::GetFpgaUtcTime( UTC_TIME_t* pUtcTime )
{
    if (INVALID_HANDLE_VALUE != m_hGPMDevice && NULL != pUtcTime)
    {
        if (DeviceIoControl(m_hGPMDevice, IOCTRL_GET_UTC_TIME, NULL, NULL, pUtcTime, sizeof(UTC_TIME_t), NULL ,NULL) == FALSE)
        {
            RETAILMSG(1, (_T("CDsmDataLayer::GetFpgaUtcTime IOCTRL_GET_UTC_TIME failed\r\n")));
            return false;
        }
        return true;
    }
    return false;
}

///
/// @brief
///    ����FPGAʱ��
///
/// @param[in] pUtcTime    Ҫ���õ�utcʱ��
///
/// @return
///     bool    ���óɹ�����true
///
bool CDsmDataLayer::SetFpgaUtcTime( UTC_TIME_t* pUtcTime )
{
    if (INVALID_HANDLE_VALUE != m_hGPMDevice && NULL != pUtcTime)
    {
        if (DeviceIoControl(m_hGPMDevice, IOCTRL_SET_UTC_TIME, pUtcTime, sizeof(UTC_TIME_t), NULL, NULL, NULL ,NULL) == FALSE)
        {
            RETAILMSG(1, (_T("CDsmDataLayer::SetFpgaUtcTime IOCTRL_SET_UTC_TIME failed\r\n")));
            return false;
        }
        return true;
    }
    return false;

}

///
/// @brief
///    ���ý�������dsp����ʹ�õĲ�����
///
bool CDsmDataLayer::SetDspSampleRate( uint32_t nSampleRate )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->SetDspSampleRate(nSampleRate);
    }
    return false;
}

///
/// @brief
///    ��ʼ/ֹͣ������Ϣ���
///
/// @param[in]  bStart  �Ƿ�ʼ���Լ�⣬true��ʼ��⣬falseֹͣ���
///
/// @note
///     ֻ���ڼ��Լ��ģʽ����Ч
void CDsmDataLayer::StartPolarityCheck( bool bStart /*= true*/ )
{
    if (m_pFrameRecv)
    {
        m_pFrameRecv->StartPolarityCheck(bStart);
    }
}

///
/// @brief
///    ���ݲ�ͬ�ĳ���Э��Ĭ�����ã����ض�Ӧ��ͨ������
///
/// @param[in]  enConfigType    Ҫ���ص�����ģ������
/// @param[out] pSmvRecvConfig  Ҫ����������Ϣ
///
/// @return
///     bool    ���سɹ�����true
///
bool CDsmDataLayer::SetSmvRecvChannelTemplate( enumSmvRecvConfigTemplateType enConfigType, CSmvRecvConfig* pSmvRecvConfig )
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->SetSmvRecvChannelTemplate(enConfigType, pSmvRecvConfig);
    }
    return false;
}

///
/// @brief
///    ���»���Ŀ��ƿ�������Ϣһ��/����ֵ
///
/// @param[in]  enChannelType Ҫ�޸ĵ�ͨ�����ͣ���ѹͨ��/����ͨ��
/// @param[in]  nPriSectype   Ҫ�޸ĵ�ֵ���ͣ���һ��ֵ��1�����Ƕ���ֵ��2��
/// @param[in]  nVal          ���µ���ֵ
///
void CDsmDataLayer::UpdateCachedRatedValues( enumChannelType enChannelType, uint32_t nPriSecType, float fVal )
{
    if (m_pFrameRecv)
    {
        m_pFrameRecv->UpdateCachedRatedValues(enChannelType, nPriSecType, fVal);
    }
}


///
/// @brief
///    ��ȡ�����������ƿ�ʵʱͨ����
///
/// @param[out]  nChannelCountSmv1 ������ƿ�1��ͨ����Ŀ
/// @param[out]  nChannelCountSmv2 ������ƿ�2��ͨ����Ŀ
///
/// @return
///     bool   ��ȡ�ɹ�����true
///
bool CDsmDataLayer::GetPhaseCheckRealTimeChannelCounts(uint32_t& nChannelCountSmv1, uint32_t& nChannelCountSmv2)
{
    if (m_pFrameRecv)
    {
        return m_pFrameRecv->GetPhaseCheckRealTimeChannelCounts(nChannelCountSmv1, nChannelCountSmv2);
    }
    return false;
}

///
/// @brief
///    ��ȡ�˿�����״̬��
///
/// @param[out] nState  ��ȡ�ɹ���������˿ڵ�״̬��Ϣ
///
/// @return
///     bool    ��ȡ�ɹ�����true
///
bool CDsmDataLayer::GetPortState( uint16_t& nState )
{
    if (INVALID_HANDLE_VALUE != m_hGPMDevice)
    {
        if (DeviceIoControl(m_hGPMDevice, IOCTRL_GET_OPTICAL_STATE, NULL, NULL, &nState, sizeof(uint16_t), NULL, NULL))
        {
            return true;
        }
        else
        {
            RETAILMSG(1, (_T("DeviceIoControl IOCTRL_GET_OPTICAL_STATE Failed \r\n")));
        }
    }
    return false;
}

///
/// @brief
///    �ж�ĳ���˿ڵ�����״̬,�����̫������Ч
///
bool CDsmDataLayer::IsPortLinked( DsmPortName enPortName )
{
    bool bRet = false;
    uint16_t nState = 0;
    if (GetPortState(nState))
    {
        switch (enPortName)
        {
        case NET_PORT_LIGHT_A:
            bRet = ((nState & NET_PORT_LIGHT_A_LINK) == NET_PORT_LIGHT_A_LINK);
            break;
        case NET_PORT_LIGHT_B:
            bRet = ((nState & NET_PORT_LIGHT_B_LINK) == NET_PORT_LIGHT_B_LINK);
            break;
        case NET_PORT_ELECTRONIC:
            bRet = ((nState & NET_PORT_LIGHT_ELEC_LINK) == NET_PORT_LIGHT_ELEC_LINK);
            break;
        default:
            break;
        }
    }
    return bRet;
}

///
/// @brief
///    �ж�ĳ���˿��Ƿ������շ�����
///
bool CDsmDataLayer::IsPortActive( DsmPortName enPortName )
{
    bool bRet = false;
    uint16_t nState = 0;
    if (GetPortState(nState))
    {
        switch (enPortName)
        {
        case NET_PORT_LIGHT_A:
            bRet = ((nState & NET_PORT_LIGHT_A_ACTIVE) == NET_PORT_LIGHT_A_ACTIVE);
            break;
        case NET_PORT_LIGHT_B:
            bRet = ((nState & NET_PORT_LIGHT_B_ACTIVE) == NET_PORT_LIGHT_B_ACTIVE);
            break;
        case NET_PORT_ELECTRONIC:
            bRet = ((nState & NET_PORT_LIGHT_ELEC_ACTIVE) == NET_PORT_LIGHT_ELEC_ACTIVE);
            break;
        case FT3_PORT_LIGHT_A:
            bRet = ((nState & FT3_PORT_A_ACTIVE) == FT3_PORT_A_ACTIVE);
            break;
        case FT3_PORT_LIGHT_B:
            bRet = ((nState & FT3_PORT_B_ACTIVE) == FT3_PORT_B_ACTIVE);
            break;
        case FT3_PORT_LIGHT_C:
            bRet = ((nState & FT3_PORT_C_ACTIVE) == FT3_PORT_C_ACTIVE);
            break;
        case LIGHT_PORT_IRIG_B:
            bRet = ((nState & LIGHT_PORT_IRIGB_ACTIVE) == LIGHT_PORT_IRIGB_ACTIVE);
            break;
        default:
            break;
        }
    }
    return bRet;
}

///
/// @brief
///    ��ʼ����b��(ͨ��FT3���Ͷ˿�)
///
/// @return
///     bool    ���ͳɹ�����true
///
bool CDsmDataLayer::StartIRIGBTx()
{
    return _SetFt3TxStyle(FT3_TX_STYLE_IRIG_B);
}

///
/// @brief
///    ֹͣ����b��(ͨ��FT3���Ͷ˿�)
///
/// @return
///     bool    ���ͳɹ�����true
///
bool CDsmDataLayer::StopIRIGBTx()
{
    return _SetFt3TxStyle(FT3_TX_STYLE_FT3);
}

///
/// @brief
///    ����FT3���Ͷ˿ڷ�����������(Ft3/B��)
///
/// @return
///     bool    ���óɹ�����true
///
bool CDsmDataLayer::_SetFt3TxStyle( enumFt3TxStyle enTxStyle )
{
    if (m_hGPMDevice != INVALID_HANDLE_VALUE)
    {
        if (DeviceIoControl(m_hGPMDevice, IOCTRL_SET_FT3_TX_STYLE, (PBYTE)&enTxStyle, sizeof(enTxStyle), NULL, 0, NULL, NULL))
        {
            return true;
        }
    }
    return false;
}


///
/// @brief
///    ����1588��ʱʱ����Ϣ
///
/// @param[in] 
///
/// @return
///     bool    ���ͳɹ�����true
///
bool CDsmDataLayer::SetIeee1588MainClock( DsmPortName enPortName, const IEEE1588_CLOCK_SYNC_INFO& stIEEE1588ClockInfo)
{
    if (INVALID_HANDLE_VALUE != m_hGPMDevice)
    {
        if (enPortName >= NET_PORT_LIGHT_A && enPortName <= NET_PORT_ELECTRONIC)
        {
            IOCTRL_IN_PARAM_NET_BIG stIoctrlInParam;
            stIoctrlInParam.nDeviceItem = enPortName;
            memcpy_s(stIoctrlInParam.nInBuf, sizeof(stIoctrlInParam.nInBuf), &stIEEE1588ClockInfo, sizeof(stIEEE1588ClockInfo));
            if (DeviceIoControl(m_hGPMDevice, IOCTRL_IEEE1588_MAIN_CLOCK, &stIoctrlInParam, sizeof(stIoctrlInParam), NULL, NULL, NULL, NULL))
            {
                return true;
            }
        }
    }
    return false;
}
