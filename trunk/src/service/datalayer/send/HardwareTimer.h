///
/// @brief
///     Ӳ����ʱ��
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.12.23
///
/// �޸���ʷ��
///
#pragma once

class CHardwareTimer
{
public:
    CHardwareTimer(void);
    virtual ~CHardwareTimer(void);

    ///
    /// @brief
    ///     ��ʼ��Ӳ����ʱ��
    ///
    /// @param[in]  csName  Ӳ����ʱ������
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool Init(CString csName);

    ///
    /// @brief
    ///     ж��Ӳ����ʱ��
    ///
    void UnInit();

    ///
    /// @brief
    ///     �ȴ�ָ��ʱ����û�ȡ���¼���λ
    ///
    /// @param[in]  dwDelay     �ȴ���ʱ��ms��
    /// @param[in]  hEvtCancel  �û�ȡ���¼�
    ///
    /// @return
    ///     bool    �ȴ��ɹ�����true
    ///
    bool Wait(DWORD dwDelay, HANDLE hEvtCancel);

private:
    HANDLE                              m_hTmrDev;                      ///< Ӳ����ʱ�����
    HANDLE                              m_hEvtExpired;                  ///< ��ʱ�¼����
};
