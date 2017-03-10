/// @file
///
/// @brief
///     �⹦�ʹ�����
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.7.19
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "OpticalPowerManager.h"
#include "src\service\datalayer\include\config.h"

///
/// @brief
///     ���캯��
///
COpticalPowerManager::COpticalPowerManager(void)
: m_bIsGathering(false)
, m_hGPMDevice(NULL)
{
    m_vecLightPorts.clear();
    m_eventStopGather.ResetEvent();
}

///
/// @brief
///     ��������
///
COpticalPowerManager::~COpticalPowerManager(void)
{
    ClearAllLightPorts();
}

///
/// @brief
///     ��ȡ�⹦�ʹ���������
///
COpticalPowerManager* COpticalPowerManager::GetInstance()
{
    static COpticalPowerManager s_opticalPowerManager;
    return &s_opticalPowerManager;
}

///
/// @brief
///     ��ʼ��
///
/// @param[in]  hGPMDevice    GPM�������
///
/// @return
///     bool    ��ʼ���ɹ�����true
///
bool COpticalPowerManager::Init(HANDLE hGPMDevice)
{
    // ����GPM�������
    if(NULL == hGPMDevice)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"COpticalPowerManager::Init(): Parameter error \n"));
        return false;
    }
    m_hGPMDevice = hGPMDevice;

    // ����˿�����Ϊ�գ���ô���������˿ڶ���
    if (0 == m_vecLightPorts.size())
    {
        DsmPortName ePortName[] = 
        {
            NET_PORT_LIGHT_A,                                           ///< ��̫�����A
            NET_PORT_LIGHT_B,                                           ///< ��̫�����B
        };


        // ����������ӹ�ڶ���
        for (int i = 0; i < sizeof(ePortName)/sizeof(ePortName[0]); i++)
        {
            // ������ڶ���
            CLightPort* pLightPort = new CLightPort(ePortName[i], m_hGPMDevice);
            if (NULL == pLightPort)
            {
                return false;
            }

            // �������ָ�뵽������
            m_vecLightPorts.push_back(pLightPort);
        }
    }

    return true;
}

///
/// @brief
///    ������еĹ�ڶ���
///
void COpticalPowerManager::ClearAllLightPorts()
{
    std::vector<CLightPort*>::iterator iter;


    // �ͷ������е����ж���
    for (iter = m_vecLightPorts.begin(); iter != m_vecLightPorts.end(); iter++)
    {
        CLightPort* pLightPort = *iter;
        delete pLightPort;
    }

    // �������
    m_vecLightPorts.clear();
}

///
/// @brief
///    �⹦�ʲɼ��߳�
///
UINT COpticalPowerManager::__OpticalPowerGatherThread(LPVOID pParam)
{
    std::vector<CLightPort*>::iterator iter;


    // ��òɼ�������ָ��
    COpticalPowerManager* pManager = (COpticalPowerManager*)pParam;
    if (NULL == pManager)
    {
        goto ExitHandle;
    }

    // �����ǰ���ڲɼ��⹦�ʣ���ôֱ�ӷ���
    pManager->m_criticalGatherStatus.Lock();
    if (pManager->m_bIsGathering)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"COpticalPowerManager::__OpticalPowerGatherThread(): Gather thread already started \n"));
        pManager->m_criticalGatherStatus.Unlock();
        return true;
    }
    // ���Ϊ���ڲɼ��⹦��
    pManager->m_bIsGathering = true;
    pManager->m_criticalGatherStatus.Unlock();

    // ��ʼ���ϲɼ�����
    while(1)
    {
        // ����һ��ʱ�䣬ͬʱ�ж��Ƿ���Ҫֹͣ�ɼ��߳�
        if (pManager->m_eventStopGather.Lock(100))
        {
            break;
        }

        // ����ÿһ����ڶ���ˢ����⹦�ʺͲ���
        for (iter = pManager->m_vecLightPorts.begin(); iter != pManager->m_vecLightPorts.end(); iter++)
        {
            CLightPort* pLightPort = *iter;
            if (NULL != pLightPort)
            {
                // ˢ�¹⹦��
                pLightPort->__GatherCurOpticalPower();
            }
        }
    }

ExitHandle:
    // ���Ϊ��ֹͣ�ɼ��⹦��
    pManager->m_bIsGathering = false;
    return TRUE;
}

///
/// @brief
///    ��ʼ�ɼ��⹦��
///
/// @return
///     bool    �ɹ�����true
///
bool COpticalPowerManager::StartGatherOpticalPower()
{
    // ����Ƿ��ѳ�ʼ��
    if (0 == m_vecLightPorts.size())
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"COpticalPowerManager::StartGatherOpticalPower(): Optical power manager uninit \n"));
        return false;
    }

    // �����⹦�ʲɼ����߳�
    if (NULL == AfxBeginThread(__OpticalPowerGatherThread, this))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"COpticalPowerManager::StartGatherOpticalPower(): AfxBeginThread failed \n"));
        return false;
    }

    return true;
}

///
/// @brief
///    ֹͣ�ɼ��⹦��
///
/// @return
///     bool    �ɹ�����true
///
bool COpticalPowerManager::StopGatherOpticalPower()
{
    // �����ǰ���ڲɼ��⹦�ʣ���ôֹͣ�ɼ��߳�
    if (m_bIsGathering)
    {
        // ��λȡ���¼�
        m_eventStopGather.SetEvent();

        // ѭ���ȴ��ɼ��߳̽���
        while(m_bIsGathering)
        {
            Sleep(10);
        }

        // ��λȡ���¼�
        m_eventStopGather.ResetEvent();
    }

    return true;
}

///
/// @brief
///    ���ָ�����ֵĹ�ڶ���
///
/// @param[in]  ePortName    �����
///
/// @return
///     CLightPort*    ָ���ڶ����ָ�룬ʧ��ʱ����NULL
///
CLightPort* COpticalPowerManager::GetLightPort( DsmPortName ePortName )
{
    std::vector<CLightPort*>::iterator iter;


    // ���������е����ж���
    for (iter = m_vecLightPorts.begin(); iter != m_vecLightPorts.end(); iter++)
    {
        CLightPort* pLightPort = *iter;


        // ����ҵ���ָ��������Ķ�����ô���ض���ָ��
        if ((NULL != pLightPort) && (ePortName == pLightPort->GetPortName()))
        {
            return pLightPort;
        }
    }

    return NULL;
}

///
/// @brief
///    ��λͳ��ֵ
///
void COpticalPowerManager::Reset()
{
    std::vector<CLightPort*>::iterator  iter;
    bool                                bIsGathering    = false;


    // ����ɼ�״̬
    bIsGathering = m_bIsGathering;

    // ֹͣ�ɼ��⹦��
    StopGatherOpticalPower();

    // ���������е����ж��󣬸�λÿһ���˿ڶ����ͳ������
    for (iter = m_vecLightPorts.begin(); iter != m_vecLightPorts.end(); iter++)
    {
        CLightPort* pLightPort = *iter;


        // ��λÿһ���˿ڵ�ͳ������
        if (NULL != pLightPort)
        {
            pLightPort->Reset();
        }
    }

    // �����ǰ���ڲɼ����ݣ���ô���´����ɼ��߳�
    if (bIsGathering)
    {
        // ��ʼ�ɼ��⹦��
        StartGatherOpticalPower();
    }
}
