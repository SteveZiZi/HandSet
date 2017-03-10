/// @file
///
/// @brief
///     SMV��������ͨ��
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.06.07
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "SMVSendVirtualChannel.h"
#include "ParamConfigSMVSend.h"
#include "ParamconfigManualTest.h"

#define NODE_VirtualChannel_Name                L"Name"                 ///< ����ͨ����
#define NODE_VirtualChannel_Amplitude           L"Amplitude"            ///< ��ֵ
#define NODE_VirtualChannel_PhaseAngle          L"PhaseAngle"           ///< ���
#define NODE_VirtualChannel_Frequency           L"Frequency"            ///< Ƶ��
#define NODE_VirtualChannel_Amplitude_Step      L"AmplitudeStep"        ///< ��ֵ�Ĳ���
#define NODE_VirtualChannel_PhaseAngle_Step     L"PhaseAngleStep"       ///< ��ֵ�Ĳ���
#define NODE_VirtualChannel_Frequency_Step      L"FrequencyStep"        ///< ��ֵ�Ĳ���
#define NODE_VirtualChannel_SelIndex            L"SelectedIndex"        ///< SVMͨ��ӳ����б���ѡ��ͨ������������0��ʼ������
#define NODE_Harmonic                           L"Harmonic"             ///< г������
#define NODE_Harmonic_Type                      L"Type"                 ///< г������
#define NODE_Harmonic_Amplitude                 L"Amplitude"            ///< г����ֵ
#define NODE_Harmonic_PhaseAngle                L"PhaseAngle"           ///< г�����

///
/// @brief
///    ���캯��
///
/// @param[in]  eType    ��������
/// @param[in]  pParamConfigBase    ���ù���������
///
CSMVSendVirtualChannel::CSMVSendVirtualChannel(eFunctionType eType, CParamConfigBase* pParamConfigBase)
:m_strName(L"")
,m_eFunctionType(eType)
,m_pParamConfigBase(pParamConfigBase)
,m_eACDCType(CURRENT_TYPE_UNKNOWN)
,m_dAmplitude(0.0f)
,m_dPhaseAngle(0.0f)
,m_dFrequency(0.0f)
,m_dAmplitudeStep(0.0f)
,m_dPhaseAngleStep(0.0f)
,m_dFrequencyStep(0.0f)
,m_iSelectedIndex(0)
,m_harmonic(pParamConfigBase)
,m_bIsAmplitudeChanged(false)
,m_bIsPhaseAngleChanged(false)
,m_bIsFrequencyChanged(false)
,m_bSelected(false)
{
    m_vecChannelGroup.clear();
    m_harmonic.SetSmvSendVirtualChannel(this);
}

///
/// @brief
///    ��������
///
CSMVSendVirtualChannel::~CSMVSendVirtualChannel(void)
{
}

///
/// @brief
///    �������ͨ����
///
/// @return
///     std::wstring    ����ͨ����
///
std::wstring CSMVSendVirtualChannel::GetName()
{
    return m_strName;
}

///
/// @brief
///    ��������ͨ����
///
/// @param[in]  strName    ����ͨ����
///
/// @return
///     bool    ���óɹ�����true
///
bool CSMVSendVirtualChannel::SetName( std::wstring strName )
{
    if (m_strName != strName)
    {
        m_strName = strName;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }

    return true;
}

///
/// @brief
///    ��÷�ֵ����ѹΪV������ΪA��ʱ��Ϊus��
///
/// @return
///     double    ��ֵ
///
double CSMVSendVirtualChannel::GetAmplitude()
{
    double                      dRet = 0.0f;


    // ���SMV���Ͳ������ù�����
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return dRet;
    }

    //
    // ���ݲ���ֵ��ʾ��ʽ��������յķ���ֵ
    //
    switch(pParamConfigSMVSend->GetSVDisplay())
    {
    case SMV_VALUE_TYPE_PRIMARY:                                   ///< һ��ֵ
        {
            dRet =  m_dAmplitude;
        }
        break;

    case SMV_VALUE_TYPE_SECONDARY:                                 ///< ����ֵ
        {
            CChannel* pChannel = GetChannel(GetSelectedIndex());   ///< ��õ�ǰѡ�е�ͨ��
            if (NULL != pChannel)
            {
                // ���ͨ����һ�ζֵ�����ζֵ
                double dOnceRatings          = pChannel->GetOnceRatings();
                double dSecondaryRatings     = pChannel->GetSecondaryRatings();

                // ���ݱ�����ϵ��������շ���ֵ
                if (fabs(dOnceRatings) >= FLOAT_PRECISION)
                {
                    dRet = m_dAmplitude * dSecondaryRatings / dOnceRatings;
                }
            }
        }
        break;

    default:
        break;
    }

    return dRet;
}

///
/// @brief
///    ���÷�ֵ����ѹΪV������ΪA��ʱ��Ϊus��
///
/// @param[in]  dAmplitude    ��ֵ
///
/// @return
///     bool    ���óɹ�����true
///
bool CSMVSendVirtualChannel::SetAmplitude(double dAmplitude)
{
    double                          dData = 0.0f;


    // ���SMV���Ͳ������ù�����
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return false;
    }

    //
    // ���ݲ���ֵ��ʾ��ʽ��������յķ�ֵһ��ֵ
    //
    switch(pParamConfigSMVSend->GetSVDisplay())
    {
    case SMV_VALUE_TYPE_PRIMARY:                                        ///< һ��ֵ
        {
            if (fabs(m_dAmplitude - dAmplitude) >= FLOAT_PRECISION)
            {
                m_dAmplitude = dAmplitude;

                // ���Ϊ���޸�
                if (NULL != m_pParamConfigBase)
                {
                    m_pParamConfigBase->SetModified(true);
                }
            }
        }
        break;

    case SMV_VALUE_TYPE_SECONDARY:                                      ///< ����ֵ
        {
            CChannel* pChannel = GetChannel(GetSelectedIndex());        ///< ��õ�ǰѡ�е�ͨ��
            if (NULL == pChannel)
            {
                return false;
            }

            // ���ͨ����һ�ζֵ�����ζֵ
            double      dOnceRatings          = pChannel->GetOnceRatings();
            double      dSecondaryRatings     = pChannel->GetSecondaryRatings();

            // ���ݱ�����ϵ��������շ���ֵ
            if (fabs(dSecondaryRatings) < FLOAT_PRECISION)
            {
                return false;
            }

            // ���һ��ֵ
            dData = dAmplitude * dOnceRatings / dSecondaryRatings;
            if (fabs(m_dAmplitude - dData) >= FLOAT_PRECISION)
            {
                m_dAmplitude = dData;

                // ���Ϊ���޸�
                if (NULL != m_pParamConfigBase)
                {
                    m_pParamConfigBase->SetModified(true);
                }
            }
        }
        break;

    default:
        return false;
    }

    return true;
}

///
/// @brief
///     ����ͨ���ֵ
///
/// @return
///     bool    �ɹ�����true
///
bool CSMVSendVirtualChannel::SetRatedAmplitude()
{
    double      dOnceRatings;
    CChannel*   pChannel;


    // ��õ�ǰѡ�е�ͨ��
    pChannel = GetChannel(GetSelectedIndex());
    if (NULL == pChannel)
    {
        return false;
    }

    // ���ͨ����һ�ζֵ
    dOnceRatings = pChannel->GetOnceRatings();
    if (L'U' == m_strName[0])                                           ///< ��ѹͨ��
    {
        m_dAmplitude = dOnceRatings / sqrt(3);
    }
    else                                                                ///< ������ʱ��ͨ��
    {
        m_dAmplitude = dOnceRatings;
    }

    return true;
}

///
/// @brief
///    ������
///
/// @return
///     double    ���
///
double CSMVSendVirtualChannel::GetPhaseAngle()
{
#if !SHENG_ZE
	if (m_dPhaseAngle < 0.000001 && m_dPhaseAngle > -0.000001)
	{
		m_dPhaseAngle = 0;
	}
	if(m_dPhaseAngle > 0)
	{
		while(m_dPhaseAngle - 360 >= 360)
		{
			m_dPhaseAngle -= 360;
		}

		while (m_dPhaseAngle > 180)
		{
			m_dPhaseAngle = m_dPhaseAngle - 360;
		}
		if (m_dPhaseAngle < 0.000001 && m_dPhaseAngle > -0.000001)
		{
			m_dPhaseAngle = 0;
		}
	}

	if(m_dPhaseAngle < 0)
	{
		while(m_dPhaseAngle + 360 <= -360)
		{
			m_dPhaseAngle += 360;
		}
		while (m_dPhaseAngle < -180 )
		{
			m_dPhaseAngle = m_dPhaseAngle - (-360);
		}
		if (m_dPhaseAngle < 0.000001 && m_dPhaseAngle > -0.000001)
		{
			m_dPhaseAngle = 0;
		}
	}
#endif
    return m_dPhaseAngle;
}

///
/// @brief
///    �������
///
/// @param[in]  dPhaseAngle    ���
///
/// @return
///     bool    ���óɹ�����true
///
bool CSMVSendVirtualChannel::SetPhaseAngle( double dPhaseAngle )
{
    if (fabs(m_dPhaseAngle - dPhaseAngle) >= FLOAT_PRECISION)
    {
        m_dPhaseAngle = dPhaseAngle;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }

    return true;
}

///
/// @brief
///    ���Ƶ��
///
/// @return
///     double    Ƶ��
///
double CSMVSendVirtualChannel::GetFrequency()
{
    return m_dFrequency;
}

///
/// @brief
///    ����Ƶ��
///
/// @param[in]  dFrequency    Ƶ��
///
/// @return
///     bool    ���óɹ�����true
///
bool CSMVSendVirtualChannel::SetFrequency( double dFrequency )
{
    // ����������
    if (dFrequency < 0)
    {
        return false;
    }

    if (fabs(m_dFrequency - dFrequency) >= FLOAT_PRECISION)
    {
        m_dFrequency = dFrequency;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }

    return true;
}

///
/// @brief
///    ��÷�ֵ��������ѹΪV������ΪA��ʱ��Ϊus��
///
/// @return
///     double    ��ֵ����
///
double CSMVSendVirtualChannel::GetAmplitudeStep()
{
    double                      dRet = 0.0f;


    // ���SMV���Ͳ������ù�����
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return dRet;
    }

    // ���ݲ���ֵ��ʾ��ʽ��һ��ֵ������ֵ����������յķ���ֵ
    switch(pParamConfigSMVSend->GetSVDisplay())
    {
    case SMV_VALUE_TYPE_PRIMARY:                                ///< һ��ֵ
        {
            dRet =  m_dAmplitudeStep;
        }
        break;

    case SMV_VALUE_TYPE_SECONDARY:                              ///< ����ֵ
        {
            CChannel* pChannel = GetChannel(GetSelectedIndex());///< ��õ�ǰѡ�е�ͨ��
            if (NULL != pChannel)
            {
                // ���ͨ����һ�ζֵ�����ζֵ
                double dOnceRatings          = pChannel->GetOnceRatings();
                double dSecondaryRatings     = pChannel->GetSecondaryRatings();

                // ���ݱ�����ϵ��������շ���ֵ
                if (fabs(dOnceRatings) >= FLOAT_PRECISION)
                {
                    dRet = m_dAmplitudeStep * dSecondaryRatings / dOnceRatings;
                }
            }
        }
        break;
    }

    return dRet;
}

///
/// @brief
///    �����ǲ���
///
/// @return
///     double    ��ǲ���
///
double CSMVSendVirtualChannel::GetPhaseAngleStep()
{
    return m_dPhaseAngleStep;
}

///
/// @brief
///    ���Ƶ�ʲ���
///
/// @return
///     double    Ƶ�ʲ���
///
double CSMVSendVirtualChannel::GetFrequencyStep()
{
    return m_dFrequencyStep;
}

///
/// @brief
///    ���÷�ֵ��������ѹΪV������ΪA��ʱ��Ϊus��
///
/// @param[in]  dStep    ��ֵ����
///
/// @return
///     bool    ���óɹ�����true
///
bool CSMVSendVirtualChannel::SetAmplitudeStep( double dStep )
{
    double                          dData = 0.0f;


    // ���SMV���Ͳ������ù�����
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return false;
    }

    // ���ݲ���ֵ��ʾ��ʽ��һ��ֵ������ֵ����������յķ�ֵһ��ֵ
    switch(pParamConfigSMVSend->GetSVDisplay())
    {
    case SMV_VALUE_TYPE_PRIMARY:                                ///< һ��ֵ
        {
            if (fabs(m_dAmplitudeStep - dStep) >= FLOAT_PRECISION)
            {
                m_dAmplitudeStep = dStep;

                // ���Ϊ���޸�
                if (NULL != m_pParamConfigBase)
                {
                    m_pParamConfigBase->SetModified(true);
                }
            }
        }
        break;

    case SMV_VALUE_TYPE_SECONDARY:                              ///< ����ֵ
        {
            CChannel* pChannel = GetChannel(GetSelectedIndex());///< ��õ�ǰѡ�е�ͨ��
            if (NULL == pChannel)
            {
                return false;
            }

            // ���ͨ����һ�ζֵ�����ζֵ
            double      dOnceRatings          = pChannel->GetOnceRatings();
            double      dSecondaryRatings     = pChannel->GetSecondaryRatings();

            // ���ݱ�����ϵ��������շ���ֵ
            if (fabs(dSecondaryRatings) < FLOAT_PRECISION)
            {
                return false;
            }

            // ���һ��ֵ
            dData = dStep * dOnceRatings / dSecondaryRatings;
            if (fabs(m_dAmplitudeStep - dData) >= FLOAT_PRECISION)
            {
                m_dAmplitudeStep = dData;

                // ���Ϊ���޸�
                if (NULL != m_pParamConfigBase)
                {
                    m_pParamConfigBase->SetModified(true);
                }
            }
        }
        break;

    default:
        return false;
    }

    return true;
}

///
/// @brief
///    ������ǲ���
///
/// @param[in]  dStep    ��ֵ����
///
/// @return
///     bool    ���óɹ�����true
///
bool CSMVSendVirtualChannel::SetPhaseAngleStep( double dStep )
{
    if (fabs(m_dPhaseAngleStep - dStep) >= FLOAT_PRECISION)
    {
        m_dPhaseAngleStep = dStep;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }

    return true;
}

///
/// @brief
///    ����Ƶ�ʲ���
///
/// @param[in]  dStep    Ƶ�ʲ���
///
/// @return
///     bool    ���óɹ�����true
///
bool CSMVSendVirtualChannel::SetFrequencyStep( double dStep )
{
    if (fabs(m_dFrequencyStep - dStep) >= FLOAT_PRECISION)
    {
        m_dFrequencyStep = dStep;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }

    return true;
}

///
/// @brief
///    ���ӳ�䵽������ͨ������ʵͨ��������
///
/// @return
///     int    ӳ�䵽������ͨ������ʵͨ��������
///
int CSMVSendVirtualChannel::GetChannelCount()
{
    int     iRet    = 0;


    //
    // �ֶ����顢״̬���й���ͬ��������ͨ������ͬһ��ͨ������
    // ������ֶ����������ͨ������ôֱ�ӷ���ͨ��������С
    // �����״̬���е�����ͨ������ô�����ֶ�������ͬ��������ͨ����ͨ��������С
    //
    if (FUNCTION_TYPE_MANUAL_TEST == m_eFunctionType)
    {
        iRet = (int)m_vecChannelGroup.size();
    }
    else
    {
        // ����ֶ��������
        CParamconfigManualTest* pParamConfigManualTest = CParamconfigManualTest::getInstance();
        if (pParamConfigManualTest)
        {
            // ����ֶ������������ָ�����ֵ�����ͨ��
            CSMVSendVirtualChannel* pVirtualChannel = pParamConfigManualTest->__GetSMVSendVirtualChannel(m_strName);
            if (pVirtualChannel)
            {
                iRet = pVirtualChannel->GetChannelCount();
            }
        }
    }

    return iRet;
}

///
/// @brief
///    ���SVMͨ��ӳ����б���ѡ��ͨ������������0��ʼ������
///
/// @return
///     int    ����ѡ��ͨ��������
///
int CSMVSendVirtualChannel::GetSelectedIndex()
{
    int     iRet    = 0;


    //
    // �ֶ����顢״̬���й���ͬ��������ͨ������ͬһ������
    // ������ֶ����������ͨ������ôֱ�ӷ�������
    // �����״̬���е�����ͨ������ô�����ֶ�������ͬ��������ͨ��������
    //
    if (FUNCTION_TYPE_MANUAL_TEST == m_eFunctionType)
    {
        iRet = m_iSelectedIndex;
    }
    else
    {
        // ����ֶ��������
        CParamconfigManualTest* pParamConfigManualTest = CParamconfigManualTest::getInstance();
        if (pParamConfigManualTest)
        {
            // ����ֶ������������ָ�����ֵ�����ͨ��
            CSMVSendVirtualChannel* pVirtualChannel = pParamConfigManualTest->__GetSMVSendVirtualChannel(m_strName);
            if (pVirtualChannel)
            {
                iRet = pVirtualChannel->GetSelectedIndex();
            }
        }
    }

    return iRet;
}

///
/// @brief
///    ����SVMͨ��ӳ����б���ѡ��ͨ������������0��ʼ������
///
/// @param[in]  nIndex    ����ѡ��ͨ��������
///
/// @return
///     bool    �ɹ�����true
///
bool CSMVSendVirtualChannel::SetSelectedIndex( int nIndex )
{
    //
    // �ֶ����顢״̬���й���ͬ��������ͨ������ͬһ������
    // ������ֶ����������ͨ������ôֱ���޸�����
    // �����״̬���е�����ͨ������ô�޸��ֶ�������ͬ��������ͨ��������
    //
    if (FUNCTION_TYPE_MANUAL_TEST == m_eFunctionType)
    {
        if ((m_iSelectedIndex != nIndex))
        {
            m_iSelectedIndex = nIndex;

            // ���Ϊ���޸�
            if (NULL != m_pParamConfigBase)
            {
                m_pParamConfigBase->SetModified(true);
            }
        }
    }
    else
    {
        // ����ֶ��������
        CParamconfigManualTest* pParamConfigManualTest = CParamconfigManualTest::getInstance();
        if (!pParamConfigManualTest)
        {
            return false;
        }

        // ����ֶ������������ָ�����ֵ�����ͨ��
        CSMVSendVirtualChannel* pVirtualChannel = pParamConfigManualTest->__GetSMVSendVirtualChannel(m_strName);
        if (!pVirtualChannel)
        {
            return false;
        }

        return pVirtualChannel->SetSelectedIndex(nIndex);
    }

    return true;
}

///
/// @brief
///    ö�ٵ�һ��Channel
///
/// @return
///     CChannel*    �ɹ�ʱ����ͨ��ָ�룬ʧ��ʱ����NULL
///
CChannel* CSMVSendVirtualChannel::FirstChannel()
{
    //
    // �ֶ����顢״̬���й���ͬ��������ͨ������ͬһ��ͨ������
    // ������ֶ����������ͨ������ôֱ�ӷ���ͨ�������еĵ�һ��ͨ��
    // �����״̬���е�����ͨ������ô�����ֶ�������ͬ��������ͨ����ͨ�������еĵ�һ��ͨ��
    //
    if (FUNCTION_TYPE_MANUAL_TEST == m_eFunctionType)
    {
        std::vector<CChannel*>::iterator    iter = m_vecChannelGroup.begin();
        if (m_vecChannelGroup.end() != iter)
        {
            return *iter;
        }
    }
    else
    {
        // ����ֶ��������
        CParamconfigManualTest* pParamConfigManualTest = CParamconfigManualTest::getInstance();
        if (pParamConfigManualTest)
        {
            // ����ֶ������������ָ�����ֵ�����ͨ��
            CSMVSendVirtualChannel* pVirtualChannel = pParamConfigManualTest->__GetSMVSendVirtualChannel(m_strName);
            if (pVirtualChannel)
            {
                return pVirtualChannel->FirstChannel();
            }
        }
    }

    return NULL;
}

///
/// @brief
///    ö����һ��Channel
///
/// @param[in]  pCurChannel ��ǰChannel��ָ��
///
/// @return
///     CChannel*    �ɹ�ʱ����Channelָ�룬ʧ��ʱ����NULL
///
CChannel* CSMVSendVirtualChannel::NextChannel( CChannel* pCurChannel )
{
    //
    // �ֶ����顢״̬���й���ͬ��������ͨ������ͬһ��ͨ������
    // ������ֶ����������ͨ������ôֱ�ӷ���ͨ�������е���һ��ͨ��
    // �����״̬���е�����ͨ������ô�����ֶ�������ͬ��������ͨ����ͨ�������е���һ��ͨ��
    //
    if (FUNCTION_TYPE_MANUAL_TEST == m_eFunctionType)
    {
        CChannel*                           pChannel   = NULL;
        std::vector<CChannel*>::iterator    iter;


        iter = m_vecChannelGroup.begin();
        while (m_vecChannelGroup.end() != iter)
        {
            // ���ҵ�ǰGOOSE���ƿ��λ��
            pChannel = *iter;
            if (pChannel == pCurChannel)
            {
                // �����һ��GOOSE���ƿ��λ��
                if (m_vecChannelGroup.end() == ++iter)
                {
                    return NULL;
                }

                return (*iter);
            }

            iter++;
        }
    }
    else
    {
        // ����ֶ��������
        CParamconfigManualTest* pParamConfigManualTest = CParamconfigManualTest::getInstance();
        if (pParamConfigManualTest)
        {
            // ����ֶ������������ָ�����ֵ�����ͨ��
            CSMVSendVirtualChannel* pVirtualChannel = pParamConfigManualTest->__GetSMVSendVirtualChannel(m_strName);
            if (pVirtualChannel)
            {
                return pVirtualChannel->NextChannel(pCurChannel);
            }
        }
    }

    return NULL;
}

///
/// @brief
///    ���ָ�������ŵ�ӳ�䵽������ͨ������ʵͨ��
///
/// @param[in]  nIndex    ͨ��������
///
/// @return
///     CChannel*    �ɹ�ʱ����Channelָ�룬ʧ��ʱ����NULL
///
CChannel* CSMVSendVirtualChannel::GetChannel( int nIndex )
{
    CChannel* pChannel = NULL;


    //
    // �ֶ����顢״̬���й���ͬ��������ͨ������ͬһ��ͨ������
    // ������ֶ����������ͨ������ôֱ�ӷ���ͨ�������е�ָ�������ŵ�ͨ��
    // �����״̬���е�����ͨ������ô�����ֶ�������ͬ��������ͨ����ͨ�������е�ָ�������ŵ�ͨ��
    //
    if (FUNCTION_TYPE_MANUAL_TEST == m_eFunctionType)
    {
        if (nIndex < (int)m_vecChannelGroup.size())
        {
            pChannel = m_vecChannelGroup[nIndex];
        }
    }
    else
    {
        // ����ֶ��������
        CParamconfigManualTest* pParamConfigManualTest = CParamconfigManualTest::getInstance();
        if (pParamConfigManualTest)
        {
            // ����ֶ������������ָ�����ֵ�����ͨ��
            CSMVSendVirtualChannel* pVirtualChannel = pParamConfigManualTest->__GetSMVSendVirtualChannel(m_strName);
            if (pVirtualChannel)
            {
                return pVirtualChannel->GetChannel(nIndex);
            }
        }
    }

    return pChannel;
}

///
/// @brief
///    ��ȿ���һ������ͨ������
///
/// @param[in]
///     pOldVirtualChannel    -   �Ѵ��ڵ�����ͨ������
///
/// @return
///     bool    �����ɹ�����TRUE
///
bool CSMVSendVirtualChannel::__Copy( CSMVSendVirtualChannel* pOldVirtualChannel )
{
    // ��������
    m_strName                   = pOldVirtualChannel->m_strName;
    m_dAmplitude                = pOldVirtualChannel->m_dAmplitude;
    m_dPhaseAngle               = pOldVirtualChannel->m_dPhaseAngle;
    m_dFrequency                = pOldVirtualChannel->m_dFrequency;
    m_dAmplitudeStep            = pOldVirtualChannel->m_dAmplitudeStep;
    m_dPhaseAngleStep           = pOldVirtualChannel->m_dPhaseAngleStep;
    m_dFrequencyStep            = pOldVirtualChannel->m_dFrequencyStep;

    // г������
    m_harmonic.m_eHarmonicType  = pOldVirtualChannel->m_harmonic.m_eHarmonicType;
    m_harmonic.m_dAmplitude     = pOldVirtualChannel->m_harmonic.m_dAmplitude;
    m_harmonic.m_dPhaseAngle    = pOldVirtualChannel->m_harmonic.m_dPhaseAngle;

    return true;
}

///
/// @brief
///    ����SMV��������ͨ��
///
/// @param[in]  xml    ָ��XML���������
///
/// @return
///     bool    true��ʾ���سɹ�
///
bool CSMVSendVirtualChannel::__LoadConfig(CMarkup& xml)
{
    double                              dData   = 0.0f;


    // <Name>
    if (!xml.FindElem(NODE_VirtualChannel_Name))
    {
        return false;
    }
    m_strName = xml.GetData();

    // <Amplitude>
    if (!xml.FindElem(NODE_VirtualChannel_Amplitude))
    {
        return false;
    }
    dData = 0.0f;
    swscanf_s(xml.GetData().c_str(), L"%lf", &dData, sizeof(dData));
    m_dAmplitude = dData;

    // <PhaseAngle>
    if (!xml.FindElem(NODE_VirtualChannel_PhaseAngle))
    {
        return false;
    }
    dData = 0.0f;
    swscanf_s(xml.GetData().c_str(), L"%lf", &dData, sizeof(dData));
    m_dPhaseAngle = dData;

    // <Frequency>
    if (!xml.FindElem(NODE_VirtualChannel_Frequency))
    {
        return false;
    }
    dData = 0.0f;
    swscanf_s(xml.GetData().c_str(), L"%lf", &dData, sizeof(dData));
    m_dFrequency = dData;

    // �����״̬���е�����ͨ������ô��������ͨ����г������
    if (FUNCTION_TYPE_STATE_SEQUENCE == m_eFunctionType)
    {
        // <Harmonic>
        if (!xml.FindElem(NODE_Harmonic) || !xml.IntoElem())
        {
            return false;
        }

        // <Type>
        if (!xml.FindElem(NODE_Harmonic_Type))
        {
            return false;
        }
        m_harmonic.m_eHarmonicType  = (enumHarmonicType)_wtoi(xml.GetData().c_str());

        // <Amplitude>
        if (!xml.FindElem(NODE_Harmonic_Amplitude))
        {
            return false;
        }
        swscanf_s(xml.GetData().c_str(), L"%lf", &(m_harmonic.m_dAmplitude), sizeof(m_harmonic.m_dAmplitude));

        // <PhaseAngle>
        if (!xml.FindElem(NODE_Harmonic_PhaseAngle))
        {
            return false;
        }
        swscanf_s(xml.GetData().c_str(), L"%lf", &(m_harmonic.m_dPhaseAngle), sizeof(m_harmonic.m_dPhaseAngle));

        // quit <Harmonic>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }
    else if (FUNCTION_TYPE_MANUAL_TEST == m_eFunctionType)
    {
        // ������ֶ����������ͨ������ô���ز���

        // <AmplitudeStep>
        if (!xml.FindElem(NODE_VirtualChannel_Amplitude_Step))
        {
            return false;
        }
        dData = 0.0f;
        swscanf_s(xml.GetData().c_str(), L"%lf", &dData, sizeof(dData));
        m_dAmplitudeStep = dData;

        // <PhaseAngleStep>
        if (!xml.FindElem(NODE_VirtualChannel_PhaseAngle_Step))
        {
            return false;
        }
        dData = 0.0f;
        swscanf_s(xml.GetData().c_str(), L"%lf", &dData, sizeof(dData));
        m_dPhaseAngleStep = dData;

        // <FrequencyStep>
        if (!xml.FindElem(NODE_VirtualChannel_Frequency_Step))
        {
            return false;
        }
        dData = 0.0f;
        swscanf_s(xml.GetData().c_str(), L"%lf", &dData, sizeof(dData));
        m_dFrequencyStep = dData;

        // <SelectedIndex>
        if(!xml.FindElem(NODE_VirtualChannel_SelIndex))
        {
            return false;
        }
        m_iSelectedIndex = _wtoi(xml.GetData().c_str());
    }

    return true;
}

///
/// @brief
///    ����SMV��������ͨ��
///
/// @param[in]  xml    ָ��XML���������
///
/// @return
///     bool    true��ʾ����ɹ�
///
bool CSMVSendVirtualChannel::__SaveConfig(CMarkup& xml)
{
    wchar_t                             cBuffer[128] = {0};


    // <Name>
    if (!xml.AddElem(NODE_VirtualChannel_Name) || !xml.SetData(m_strName))
    {
        return false;
    }

    // <Amplitude>
    memset(cBuffer, 0, sizeof(cBuffer));
    swprintf_s(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]) - 1, L"%lf", m_dAmplitude);
    if (!xml.AddElem(NODE_VirtualChannel_Amplitude) || !xml.SetData(cBuffer))
    {
        return false;
    }

    // <PhaseAngle>
    memset(cBuffer, 0, sizeof(cBuffer));
    swprintf_s(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]) - 1, L"%lf", m_dPhaseAngle);
    if (!xml.AddElem(NODE_VirtualChannel_PhaseAngle) || !xml.SetData(cBuffer))
    {
        return false;
    }

    // <Frequency>
    memset(cBuffer, 0, sizeof(cBuffer));
    swprintf_s(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]) - 1, L"%lf", m_dFrequency);
    if (!xml.AddElem(NODE_VirtualChannel_Frequency) || !xml.SetData(cBuffer))
    {
        return false;
    }

    // �����״̬���е�����ͨ������ô��������ͨ����г������
    if (FUNCTION_TYPE_STATE_SEQUENCE == m_eFunctionType)
    {
        // <Harmonic>
        if (!xml.AddElem(NODE_Harmonic) || !xml.IntoElem())
        {
            return false;
        }

        // <Type>
        if (!xml.AddElem(NODE_Harmonic_Type) || !xml.SetData((int)m_harmonic.m_eHarmonicType))
        {
            return false;
        }

        // <Amplitude>
        memset(cBuffer, 0, sizeof(cBuffer));
        swprintf_s(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]) - 1, L"%lf", m_harmonic.m_dAmplitude);
        if (!xml.AddElem(NODE_Harmonic_Amplitude) || !xml.SetData(cBuffer))
        {
            return false;
        }

        // <PhaseAngle>
        memset(cBuffer, 0, sizeof(cBuffer));
        swprintf_s(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]) - 1, L"%lf", m_harmonic.m_dPhaseAngle);
        if (!xml.AddElem(NODE_Harmonic_PhaseAngle) || !xml.SetData(cBuffer))
        {
            return false;
        }

        // quit <Harmonic>
        if (!xml.OutOfElem())
        {
            return false;
        }
    }
    else if (FUNCTION_TYPE_MANUAL_TEST == m_eFunctionType)
    {
        // ������ֶ����������ͨ������ô���沽��

        // <AmplitudeStep>
        memset(cBuffer, 0, sizeof(cBuffer));
        swprintf_s(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]) - 1, L"%lf", m_dAmplitudeStep);
        if (!xml.AddElem(NODE_VirtualChannel_Amplitude_Step) || !xml.SetData(cBuffer))
        {
            return false;
        }

        // <PhaseAngleStep>
        memset(cBuffer, 0, sizeof(cBuffer));
        swprintf_s(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]) - 1, L"%lf", m_dPhaseAngleStep);
        if (!xml.AddElem(NODE_VirtualChannel_PhaseAngle_Step) || !xml.SetData(cBuffer))
        {
            return false;
        }

        // <FrequencyStep>
        memset(cBuffer, 0, sizeof(cBuffer));
        swprintf_s(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]) - 1, L"%lf", m_dFrequencyStep);
        if (!xml.AddElem(NODE_VirtualChannel_Frequency_Step) || !xml.SetData(cBuffer))
        {
            return false;
        }

        // <SelectedIndex>
        memset(cBuffer, 0, sizeof(cBuffer));
        _itow_s(m_iSelectedIndex, cBuffer, sizeof(cBuffer)/ sizeof(cBuffer[0]), 10);
        if (!xml.AddElem(NODE_VirtualChannel_SelIndex) || !xml.SetData(cBuffer))
        {
            return false;
        }
    }

    return true;
}

///
/// @brief
///    �������ͨ���ĵ�������
///
/// @return
///     eCurrentType    ��������
///
eCurrentType CSMVSendVirtualChannel::GetACDCType()
{
    eCurrentType eType = CURRENT_TYPE_UNKNOWN;


    //
    // �ֶ����顢״̬���й���ͬ��������ͨ�����õ�������
    // ������ֶ����������ͨ������ôֱ�ӷ���ͨ���ĵ�������
    // �����״̬���е�����ͨ������ô�����ֶ�������ͬ��������ͨ���ĵ�������
    //
    if (FUNCTION_TYPE_MANUAL_TEST == m_eFunctionType)
    {
        eType = m_eACDCType;
    }
    else
    {
        // ����ֶ��������
        CParamconfigManualTest* pParamConfigManualTest = CParamconfigManualTest::getInstance();
        if (pParamConfigManualTest)
        {
            // ����ֶ������������ָ�����ֵ�����ͨ��
            CSMVSendVirtualChannel* pVirtualChannel = pParamConfigManualTest->__GetSMVSendVirtualChannel(m_strName);
            if (pVirtualChannel)
            {
                return pVirtualChannel->GetACDCType();
            }
        }
    }

    return eType;
}

///
/// @brief
///    ���г�����ö����ָ��
///
/// @return
///     CHarmonic*    г�����ö����ָ��
///
CHarmonic* CSMVSendVirtualChannel::GetHarmonic()
{
    return &m_harmonic;
}

///
/// @brief
///    ��ֵ�Ƿ����˱仯
///
/// @param[in]  bIsChanged    �Ƿ����˱仯
///
bool CSMVSendVirtualChannel::IsAmplitudeChanged()
{
    return m_bIsAmplitudeChanged;
}

///
/// @brief
///    ����ͨ���ķ�ֵ�Ƿ����˱仯
///
/// @param[in]  bIsChanged    �Ƿ����˱仯
///
void CSMVSendVirtualChannel::__SetAmplitudeChanged( bool bIsChanged )
{
    m_bIsAmplitudeChanged = bIsChanged;
}

///
/// @brief
///    ����Ƿ����˱仯
///
/// @param[in]  bIsChanged    �Ƿ����˱仯
///
bool CSMVSendVirtualChannel::IsPhaseAngleChanged()
{
    return m_bIsPhaseAngleChanged;
}

///
/// @brief
///    ����ͨ��������Ƿ����˱仯
///
/// @param[in]  bIsChanged    �Ƿ����˱仯
///
void CSMVSendVirtualChannel::__SetPhaseAngleChanged( bool bIsChanged )
{
    m_bIsPhaseAngleChanged = bIsChanged;
}

///
/// @brief
///    Ƶ���Ƿ����˱仯
///
/// @param[in]  bIsChanged    �Ƿ����˱仯
///
bool CSMVSendVirtualChannel::IsFrequencyChanged()
{
    return m_bIsFrequencyChanged;
}

///
/// @brief
///    ����ͨ����Ƶ���Ƿ����˱仯
///
/// @param[in]  bIsChanged    �Ƿ����˱仯
///
void CSMVSendVirtualChannel::__SetFrequencyChanged( bool bIsChanged )
{
    m_bIsFrequencyChanged = bIsChanged;
}

///
/// @brief
///     �Ƚ�����ͨ���ķ�ֵ����ǡ�Ƶ�ʡ�г��
///
/// @param[in]  eCompareType    �ȽϷ�ʽ
/// @param[in]  pOldSMVSendVirtualChannel   �뵱ǰͨ�����бȽϵ�����ͨ��
///
/// @return
///     bool    ��ͬ����true
///
bool CSMVSendVirtualChannel::Compare( enumCompareType eCompareType, CSMVSendVirtualChannel* pOldSMVSendVirtualChannel )
{
    bool    bRet = true;


    // �Ƚ�����ͨ���ķ�ֵ
    if (fabs(m_dAmplitude - pOldSMVSendVirtualChannel->m_dAmplitude) >= FLOAT_PRECISION)
    {
        // �����Ҫ�ҳ����еĲ�ͬ����ô���ñ�λ��ʶ
        if (TYPE_FIND_ALL_DIFFERENT == eCompareType)
        {
            __SetAmplitudeChanged(true);
            bRet = false;
        }
        // ���ֻ���ҳ�һ����ͬ����ôֱ�ӷ���
        else if (TYPE_FIND_ONE_DIFFERENT == eCompareType)
        {
            return false;
        }
    }

    // �Ƚ����
    if (fabs(m_dPhaseAngle - pOldSMVSendVirtualChannel->m_dPhaseAngle) >= FLOAT_PRECISION)
    {
        // �����Ҫ�ҳ����еĲ�ͬ����ô���ñ�λ��ʶ
        if (TYPE_FIND_ALL_DIFFERENT == eCompareType)
        {
            __SetPhaseAngleChanged(true);
            bRet = false;
        }
        // ���ֻ���ҳ�һ����ͬ����ôֱ�ӷ���
        else if (TYPE_FIND_ONE_DIFFERENT == eCompareType)
        {
            return false;
        }
    }

    // �Ƚ�Ƶ��
    if (fabs(m_dFrequency - pOldSMVSendVirtualChannel->m_dFrequency) >= FLOAT_PRECISION)
    {
        // �����Ҫ�ҳ����еĲ�ͬ����ô���ñ�λ��ʶ
        if (TYPE_FIND_ALL_DIFFERENT == eCompareType)
        {
            __SetFrequencyChanged(true);
            bRet = false;
        }
        // ���ֻ���ҳ�һ����ͬ����ôֱ�ӷ���
        else if (TYPE_FIND_ONE_DIFFERENT == eCompareType)
        {
            return false;
        }
    }

    // �Ƚ�г��
    if ((FUNCTION_TYPE_STATE_SEQUENCE == m_eFunctionType) && !(m_harmonic.Compare(pOldSMVSendVirtualChannel->m_harmonic)))
    {
        bRet = false;
    }

    return bRet;
}
