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

#ifndef __LIGHT_PORT_H__
#define __LIGHT_PORT_H__
#include "src\service\datalayer\include\datatypes.h"

class CLightPort
{
    friend class COpticalPowerManager;

public:
    ///
    /// @brief
    ///    ���캯��
    ///
    /// @param[in]  ePortName    �˿��豸��ʶ
    /// @param[in]  hGPMDevice    GPM�������
    ///
    CLightPort(DsmPortName ePortName, HANDLE hGPMDevice);

    ///
    /// @brief
    ///    ��������
    ///
    virtual ~CLightPort(void);

    ///
    /// @brief
    ///    ��ù�ڶ�������Ʊ��
    ///
    /// @return
    ///     DsmPortName    ������Ʊ��
    ///
    DsmPortName GetPortName();

    ///
    /// @brief
    ///    ��ù�ڲ�����nm��
    ///
    /// @return
    ///     int    ������nm��
    ///
    int GetWaveLength();

    ///
    /// @brief
    ///    ��ù�ڵĵ�ǰ�⹦��
    ///
    /// @param[out]  fDBM    ��ǰ�⹦�ʣ���dBmΪ��λ
    /// @param[out]  fUW    ��ǰ�⹦�ʣ���uWΪ��λ
    ///
    void GetCurOpticalPower(float& fDBM, float& fUW);

    ///
    /// @brief
    ///    ��ù�ڵ���ʷ��С�⹦�ʣ�dBm��
    ///
    /// @return
    ///     float    �⹦�ʣ�dBm��
    ///
    float GetMinOpticalPower();

    ///
    /// @brief
    ///    ��ù�ڵ���ʷ���⹦�ʣ�dBm��
    ///
    /// @return
    ///     float    �⹦�ʣ�dBm��
    ///
    float GetMaxOpticalPower();

    ///
    /// @brief
    ///    ��λͳ��ֵ
    ///
    void Reset();

protected:

    ///
    /// @brief
    ///    ���¶�ȡ��ڵĵ�ǰ�⹦��
    ///
    void __GatherCurOpticalPower();

private:
    DsmPortName                         m_ePortName;                    ///< �˿��豸��ʶ
    HANDLE                              m_hGPMDevice;                   ///< GPM�豸���
    bool                                m_bInited;                      ///< �Ƿ��ѳɹ���ȡһ������

    int                                 m_iWaveLength;                  ///< ������nm��
    float                               m_fCurOpticalPower;             ///< ��ǰ�⹦�ʣ�uW��
    float                               m_fMinOpticalPower;             ///< ��С�⹦�ʣ�uW��
    float                               m_fMaxOpticalPower;             ///< ���⹦�ʣ�uW��
};
#endif
