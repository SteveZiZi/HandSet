///
/// @brief
///     ״̬���в�������
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.6.14
///
/// �޸���ʷ��
///

#ifndef __PARAM_CONFIG_STATE_SEQUENCE_H__
#define __PARAM_CONFIG_STATE_SEQUENCE_H__
#include <list>
#include "paramconfigbase.h"
#include "Status.h"
#include "FaultCalc.h"


class CParamConfigStateSequence :
    public CParamConfigBase
{
protected:
    CParamConfigStateSequence(void);
    virtual ~CParamConfigStateSequence(void);

public:
    ///
    /// @brief
    ///     ��ȡ����ʵ���ӿ�
    ///
    static CParamConfigStateSequence* getInstance();

    ///
    /// @brief
    ///    ���������ļ�
    ///
    /// @param[in]  strConfigFile �ļ�·��
    ///
    /// @return
    ///     bool    true��ʾ���سɹ�
    ///
    bool LoadConfig( std::wstring strConfigFile );

    ///
    /// @brief
    ///    ���������ļ�
    ///
    /// @return
    ///     bool    true��ʾ����ɹ�
    ///
    bool SaveConfig();

    ///
    /// @brief
    ///    ö�ٵ�һ��״̬
    ///
    /// @param[in]  bRebuildMap     �Ƿ���Ҫ�ؽ�SMV����ӳ���GOOSE����ӳ���GOOSE����ӳ���
    ///
    /// @return
    ///     CStatus*    ָ��״̬�����ָ�룬ʧ��ʱ����NULL
    ///
    CStatus* FirstStatus(bool bRebuildMap = true);

    ///
    /// @brief
    ///    ö����һ��״̬
    ///
    /// @param[in]  pCurStatus    ָ��ǰ״̬�����ָ��
    ///
    /// @return
    ///     CStatus*    ָ��״̬�����ָ�룬ʧ��ʱ����NULL
    ///
    CStatus* NextStatus(CStatus* pCurStatus);

    ///
    /// @brief
    ///    ������һ��״̬
    ///
    /// @return
    ///     CStatus*    ָ��״̬�����ָ�룬ʧ��ʱ����NULL
    ///
    CStatus* LastStatus();

    ///
    /// @brief
    ///    ���һ��״̬����
    ///
    /// @return
    ///     CStatus*    ָ��״̬�����ָ��
    ///
    CStatus* AddStatus();

    ///
    /// @brief
    ///    ɾ��ָ����״̬����
    ///
    /// @param[in]  pCurStatus    ָ��״̬�����ָ��
    ///
    /// @return
    ///     bool    true��ʾɾ���ɹ�
    ///
    bool DeleteStatus(CStatus* pCurStatus);

    ///
    /// @brief
    ///    ���״̬����
    ///
    /// @return
    ///     int    ״̬����
    ///
    int GetStatusCount();

    ///
    /// @brief
    ///    �������״̬����
    ///
    void ClearAllStatus();

    ///
    /// @brief
    ///    ����Ƿ���Ҫ��ʾ������
    ///
    bool GetIsShowDI() const
    {
        return m_bIsShowDI;
    }

    ///
    /// @brief
    ///    �����Ƿ���Ҫ��ʾ������
    ///
    /// @param[in]  val    �Ƿ���ʾ
    ///
    void SetIsShowDI(bool val)
    {
        m_bIsShowDI = val;
    }

    ///
    /// @brief
    ///    ��ù��ϼ������
    ///
    /// @return
    ///     CFaultCalc*    ָ����ϼ�������ָ��
    ///
    CFaultCalc* GetFaultCalc()
    {
        return &m_faultCalc;
    }

    ///
    /// @brief
    ///    ʹ�ù��ϼ�����
    ///
    /// @param[in]  pStatus   Ӧ�õ�����״̬
    /// @param[in]  iGroup    Ӧ�õ�����ͨ����1��2��
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool UseFaultCalc(CStatus* pStatus, int iGroup);

protected:
    ///
    /// @brief
    ///    ���״̬����
    ///
    /// @param[in]  pNewStatus    ָ��״̬�����ָ��
    ///
    /// @return
    ///     bool    true��ʾ��ӳɹ�
    ///
    bool __AddStatus(CStatus* pNewStatus);

    ///
    /// @brief
    ///    ��ȡһ�����õ�״̬�����ڲ�ID
    ///
    /// @return
    ///     int    ״̬�����ڲ�ID
    ///
    int __GetAvailableStatusID();

    ///
    /// @brief
    ///    �ж�ָ���ڲ�ID�ŵ�״̬�����Ƿ����
    ///
    /// @param[in]  nID    ״̬�����ڲ�ID��
    ///
    /// @return
    ///     bool    true��ʾָ��ID��״̬�������
    ///
    bool __IsStatusExists(int nID);

private:
    std::list<CStatus *>                m_listStatusGroup;              ///< ״̬��������
    bool                                m_bIsShowDI;                    ///< �Ƿ���ʾ�������Ի���
    CFaultCalc                          m_faultCalc;                    ///< ���ϼ���
};
#endif
