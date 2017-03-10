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

#ifndef __OPTICAL_POWER_MANAGER_H__
#define __OPTICAL_POWER_MANAGER_H__
#include <vector>
#include <afxmt.h>
#include "LightPort.h"

class COpticalPowerManager
{
private:
    COpticalPowerManager(void);
    ~COpticalPowerManager(void);

public:
    ///
    /// @brief
    ///     ��ȡ�⹦�ʹ���������
    ///
    static COpticalPowerManager* GetInstance();

    ///
    /// @brief
    ///     ��ʼ��
    ///
    /// @param[in]  hGPMDevice    GPM�������
    ///
    /// @return
    ///     bool    ��ʼ���ɹ�����true
    ///
    bool Init(HANDLE hGPMDevice);

    ///
    /// @brief
    ///    ������еĹ�ڶ���
    ///
    void ClearAllLightPorts();

    ///
    /// @brief
    ///    ��ʼ�ɼ��⹦��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool StartGatherOpticalPower();

    ///
    /// @brief
    ///    ֹͣ�ɼ��⹦��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool StopGatherOpticalPower();

    ///
    /// @brief
    ///    ���ָ�����ֵĹ�ڶ���
    ///
    /// @param[in]  ePortName    �����
    ///
    /// @return
    ///     CLightPort*    ָ���ڶ����ָ�룬ʧ��ʱ����NULL
    ///
    CLightPort* GetLightPort(DsmPortName ePortName);

    ///
    /// @brief
    ///    ��λͳ��ֵ
    ///
    void Reset();

protected:
    ///
    /// @brief
    ///    �⹦�ʲɼ��߳�
    ///
    static UINT __OpticalPowerGatherThread(LPVOID pParam);

private:
    HANDLE                              m_hGPMDevice;                   ///< GPM�豸���
    std::vector<CLightPort*>            m_vecLightPorts;                ///< ��Ŷ˿ڶ���ָ�������

    CCriticalSection                    m_criticalGatherStatus;         ///< �ɼ�״̬�ٽ���
    bool                                m_bIsGathering;                 ///< ��ǰ�Ƿ����ڲɼ��⹦��
    CEvent                              m_eventStopGather;              ///< ֹͣ�ɼ��¼�
};
#endif
