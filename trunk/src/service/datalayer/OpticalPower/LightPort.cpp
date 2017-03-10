/// @file
///
/// @brief
///     ����⹦�ʵĶ˿�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.7.19
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "LightPort.h"
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\include\ctrlcode_def.h"

///
/// @brief
///    ���캯��
///
/// @param[in]  ePortName    �˿��豸��ʶ
/// @param[in]  hGPMDevice    GPM�������
///
CLightPort::CLightPort(DsmPortName ePortName, HANDLE hGPMDevice)
: m_ePortName(ePortName)
, m_hGPMDevice(hGPMDevice)
, m_bInited(false)
, m_fCurOpticalPower(0.0f)
, m_fMinOpticalPower(FLT_MAX)
, m_fMaxOpticalPower(FLT_GUARD)
{
    // �����豸��ʶ��ʼ������
    switch(m_ePortName)
    {
    case NET_PORT_LIGHT_A:
    case NET_PORT_LIGHT_B:
        {
            m_iWaveLength = 1310;
        }
        break;

    default:
        {
            m_iWaveLength = 850;
        }
    }
}

///
/// @brief
///    ��������
///
CLightPort::~CLightPort(void)
{
}

///
/// @brief
///    ��ù�ڶ�������Ʊ��
///
/// @return
///     DsmPortName    ������Ʊ��
///
DsmPortName CLightPort::GetPortName()
{
    return m_ePortName;
}

///
/// @brief
///    ��ù�ڲ�����nm��
///
/// @return
///     int    ������nm��
///
int CLightPort::GetWaveLength()
{
    return m_iWaveLength;
}

///
/// @brief
///    ��ù�ڵĵ�ǰ�⹦��
///
/// @param[out]  fDBM    ��ǰ�⹦�ʣ���dBmΪ��λ
/// @param[out]  fUW    ��ǰ�⹦�ʣ���uWΪ��λ
///
void CLightPort::GetCurOpticalPower(float& fDBM, float& fUW)
{
    fUW     = m_fCurOpticalPower;
    fDBM    = (float)(10 * log10(fUW/1000));
}

///
/// @brief
///    ��ù�ڵ���ʷ��С�⹦�ʣ�dBm��
///
/// @return
///     float    �⹦�ʣ�dBm��
///
float CLightPort::GetMinOpticalPower()
{
    float       fUW = 0.0f;


    // ����ѳɹ���ȡһ������
    if (m_bInited)
    {
        fUW = m_fMinOpticalPower;
    }

    return (float)(10 * log10(fUW/1000));
}

///
/// @brief
///    ��ù�ڵ���ʷ���⹦�ʣ�dBm��
///
/// @return
///     float    �⹦�ʣ�dBm��
///
float CLightPort::GetMaxOpticalPower()
{
    float       fUW = 0.0f;


    // ����ѳɹ���ȡһ������
    if (m_bInited)
    {
        fUW = m_fMaxOpticalPower;
    }

    return (float)(10 * log10(fUW/1000));
}

///
/// @brief
///    ���¶�ȡ��ڵĵ�ǰ�⹦��
///
void CLightPort::__GatherCurOpticalPower()
{
    unsigned short  usOpticalPower  = 0;


    // �ڴ˶�ȡ��ڹ���(0.1uW)
    if (DeviceIoControl(
        m_hGPMDevice, 
        IOCTRL_GET_OPTICAL_POWER, 
        &m_ePortName, 
        sizeof(m_ePortName), 
        &usOpticalPower, 
        sizeof(usOpticalPower), 
        NULL, 
        NULL
        ) == FALSE)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (L"CLightPort::__GatherCurOpticalPower(): IOCTRL_GET_OPTICAL_POWER failed\n"));
        return ;
    }

    // ���Ϊ�ѳɹ���ȡ������
    m_bInited = true;

    // ��¼��ǰ�⹦��(uW)
    m_fCurOpticalPower = (float)usOpticalPower/10;

    // �����ǰ�⹦�ʱ���ʷ���⹦�ʴ�
    if (m_fCurOpticalPower > m_fMaxOpticalPower)
    {
        m_fMaxOpticalPower = m_fCurOpticalPower;
    }

    // �����ǰ�⹦�ʱ���ʷ��С�⹦��С
    if (m_fCurOpticalPower < m_fMinOpticalPower)
    {
        m_fMinOpticalPower = m_fCurOpticalPower;
    }
}

///
/// @brief
///    ��λͳ��ֵ
///
void CLightPort::Reset()
{
    // ���Ϊδ��ȡһ������
    m_bInited           = false;

    // ��λ��Сֵ���ֵ
    m_fMinOpticalPower  = FLT_MAX;
    m_fMaxOpticalPower  = FLT_GUARD;
}
