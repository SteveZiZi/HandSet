/// @file
///
/// @brief
///     SMV发送虚拟通道
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.06.07
///
/// 修改历史：
///

#include "StdAfx.h"
#include "SMVSendVirtualChannel.h"
#include "ParamConfigSMVSend.h"
#include "ParamconfigManualTest.h"

#define NODE_VirtualChannel_Name                L"Name"                 ///< 虚拟通道名
#define NODE_VirtualChannel_Amplitude           L"Amplitude"            ///< 幅值
#define NODE_VirtualChannel_PhaseAngle          L"PhaseAngle"           ///< 相角
#define NODE_VirtualChannel_Frequency           L"Frequency"            ///< 频率
#define NODE_VirtualChannel_Amplitude_Step      L"AmplitudeStep"        ///< 幅值的步长
#define NODE_VirtualChannel_PhaseAngle_Step     L"PhaseAngleStep"       ///< 幅值的步长
#define NODE_VirtualChannel_Frequency_Step      L"FrequencyStep"        ///< 幅值的步长
#define NODE_VirtualChannel_SelIndex            L"SelectedIndex"        ///< SVM通道映射表中被勾选的通道的索引（从0开始计数）
#define NODE_Harmonic                           L"Harmonic"             ///< 谐波设置
#define NODE_Harmonic_Type                      L"Type"                 ///< 谐波类型
#define NODE_Harmonic_Amplitude                 L"Amplitude"            ///< 谐波幅值
#define NODE_Harmonic_PhaseAngle                L"PhaseAngle"           ///< 谐波相角

///
/// @brief
///    构造函数
///
/// @param[in]  eType    功能类型
/// @param[in]  pParamConfigBase    配置管理器对象
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
///    析构函数
///
CSMVSendVirtualChannel::~CSMVSendVirtualChannel(void)
{
}

///
/// @brief
///    获得虚拟通道名
///
/// @return
///     std::wstring    虚拟通道名
///
std::wstring CSMVSendVirtualChannel::GetName()
{
    return m_strName;
}

///
/// @brief
///    设置虚拟通道名
///
/// @param[in]  strName    虚拟通道名
///
/// @return
///     bool    设置成功返回true
///
bool CSMVSendVirtualChannel::SetName( std::wstring strName )
{
    if (m_strName != strName)
    {
        m_strName = strName;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }

    return true;
}

///
/// @brief
///    获得幅值（电压为V，电流为A，时间为us）
///
/// @return
///     double    幅值
///
double CSMVSendVirtualChannel::GetAmplitude()
{
    double                      dRet = 0.0f;


    // 获得SMV发送参数配置管理器
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return dRet;
    }

    //
    // 根据采样值显示方式，算出最终的返回值
    //
    switch(pParamConfigSMVSend->GetSVDisplay())
    {
    case SMV_VALUE_TYPE_PRIMARY:                                   ///< 一次值
        {
            dRet =  m_dAmplitude;
        }
        break;

    case SMV_VALUE_TYPE_SECONDARY:                                 ///< 二次值
        {
            CChannel* pChannel = GetChannel(GetSelectedIndex());   ///< 获得当前选中的通道
            if (NULL != pChannel)
            {
                // 获得通道的一次额定值、二次额定值
                double dOnceRatings          = pChannel->GetOnceRatings();
                double dSecondaryRatings     = pChannel->GetSecondaryRatings();

                // 根据比例关系，算出最终返回值
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
///    设置幅值（电压为V，电流为A，时间为us）
///
/// @param[in]  dAmplitude    幅值
///
/// @return
///     bool    设置成功返回true
///
bool CSMVSendVirtualChannel::SetAmplitude(double dAmplitude)
{
    double                          dData = 0.0f;


    // 获得SMV发送参数配置管理器
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return false;
    }

    //
    // 根据采样值显示方式，算出最终的幅值一次值
    //
    switch(pParamConfigSMVSend->GetSVDisplay())
    {
    case SMV_VALUE_TYPE_PRIMARY:                                        ///< 一次值
        {
            if (fabs(m_dAmplitude - dAmplitude) >= FLOAT_PRECISION)
            {
                m_dAmplitude = dAmplitude;

                // 标记为已修改
                if (NULL != m_pParamConfigBase)
                {
                    m_pParamConfigBase->SetModified(true);
                }
            }
        }
        break;

    case SMV_VALUE_TYPE_SECONDARY:                                      ///< 二次值
        {
            CChannel* pChannel = GetChannel(GetSelectedIndex());        ///< 获得当前选中的通道
            if (NULL == pChannel)
            {
                return false;
            }

            // 获得通道的一次额定值、二次额定值
            double      dOnceRatings          = pChannel->GetOnceRatings();
            double      dSecondaryRatings     = pChannel->GetSecondaryRatings();

            // 根据比例关系，算出最终返回值
            if (fabs(dSecondaryRatings) < FLOAT_PRECISION)
            {
                return false;
            }

            // 算出一次值
            dData = dAmplitude * dOnceRatings / dSecondaryRatings;
            if (fabs(m_dAmplitude - dData) >= FLOAT_PRECISION)
            {
                m_dAmplitude = dData;

                // 标记为已修改
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
///     设置通道额定值
///
/// @return
///     bool    成功返回true
///
bool CSMVSendVirtualChannel::SetRatedAmplitude()
{
    double      dOnceRatings;
    CChannel*   pChannel;


    // 获得当前选中的通道
    pChannel = GetChannel(GetSelectedIndex());
    if (NULL == pChannel)
    {
        return false;
    }

    // 获得通道的一次额定值
    dOnceRatings = pChannel->GetOnceRatings();
    if (L'U' == m_strName[0])                                           ///< 电压通道
    {
        m_dAmplitude = dOnceRatings / sqrt(3);
    }
    else                                                                ///< 电流、时间通道
    {
        m_dAmplitude = dOnceRatings;
    }

    return true;
}

///
/// @brief
///    获得相角
///
/// @return
///     double    相角
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
///    设置相角
///
/// @param[in]  dPhaseAngle    相角
///
/// @return
///     bool    设置成功返回true
///
bool CSMVSendVirtualChannel::SetPhaseAngle( double dPhaseAngle )
{
    if (fabs(m_dPhaseAngle - dPhaseAngle) >= FLOAT_PRECISION)
    {
        m_dPhaseAngle = dPhaseAngle;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }

    return true;
}

///
/// @brief
///    获得频率
///
/// @return
///     double    频率
///
double CSMVSendVirtualChannel::GetFrequency()
{
    return m_dFrequency;
}

///
/// @brief
///    设置频率
///
/// @param[in]  dFrequency    频率
///
/// @return
///     bool    设置成功返回true
///
bool CSMVSendVirtualChannel::SetFrequency( double dFrequency )
{
    // 检查输入参数
    if (dFrequency < 0)
    {
        return false;
    }

    if (fabs(m_dFrequency - dFrequency) >= FLOAT_PRECISION)
    {
        m_dFrequency = dFrequency;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }

    return true;
}

///
/// @brief
///    获得幅值步长（电压为V，电流为A，时间为us）
///
/// @return
///     double    幅值步长
///
double CSMVSendVirtualChannel::GetAmplitudeStep()
{
    double                      dRet = 0.0f;


    // 获得SMV发送参数配置管理器
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return dRet;
    }

    // 根据采样值显示方式（一次值、二次值），算出最终的返回值
    switch(pParamConfigSMVSend->GetSVDisplay())
    {
    case SMV_VALUE_TYPE_PRIMARY:                                ///< 一次值
        {
            dRet =  m_dAmplitudeStep;
        }
        break;

    case SMV_VALUE_TYPE_SECONDARY:                              ///< 二次值
        {
            CChannel* pChannel = GetChannel(GetSelectedIndex());///< 获得当前选中的通道
            if (NULL != pChannel)
            {
                // 获得通道的一次额定值、二次额定值
                double dOnceRatings          = pChannel->GetOnceRatings();
                double dSecondaryRatings     = pChannel->GetSecondaryRatings();

                // 根据比例关系，算出最终返回值
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
///    获得相角步长
///
/// @return
///     double    相角步长
///
double CSMVSendVirtualChannel::GetPhaseAngleStep()
{
    return m_dPhaseAngleStep;
}

///
/// @brief
///    获得频率步长
///
/// @return
///     double    频率步长
///
double CSMVSendVirtualChannel::GetFrequencyStep()
{
    return m_dFrequencyStep;
}

///
/// @brief
///    设置幅值步长（电压为V，电流为A，时间为us）
///
/// @param[in]  dStep    幅值步长
///
/// @return
///     bool    设置成功返回true
///
bool CSMVSendVirtualChannel::SetAmplitudeStep( double dStep )
{
    double                          dData = 0.0f;


    // 获得SMV发送参数配置管理器
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return false;
    }

    // 根据采样值显示方式（一次值、二次值），算出最终的幅值一次值
    switch(pParamConfigSMVSend->GetSVDisplay())
    {
    case SMV_VALUE_TYPE_PRIMARY:                                ///< 一次值
        {
            if (fabs(m_dAmplitudeStep - dStep) >= FLOAT_PRECISION)
            {
                m_dAmplitudeStep = dStep;

                // 标记为已修改
                if (NULL != m_pParamConfigBase)
                {
                    m_pParamConfigBase->SetModified(true);
                }
            }
        }
        break;

    case SMV_VALUE_TYPE_SECONDARY:                              ///< 二次值
        {
            CChannel* pChannel = GetChannel(GetSelectedIndex());///< 获得当前选中的通道
            if (NULL == pChannel)
            {
                return false;
            }

            // 获得通道的一次额定值、二次额定值
            double      dOnceRatings          = pChannel->GetOnceRatings();
            double      dSecondaryRatings     = pChannel->GetSecondaryRatings();

            // 根据比例关系，算出最终返回值
            if (fabs(dSecondaryRatings) < FLOAT_PRECISION)
            {
                return false;
            }

            // 算出一次值
            dData = dStep * dOnceRatings / dSecondaryRatings;
            if (fabs(m_dAmplitudeStep - dData) >= FLOAT_PRECISION)
            {
                m_dAmplitudeStep = dData;

                // 标记为已修改
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
///    设置相角步长
///
/// @param[in]  dStep    幅值步长
///
/// @return
///     bool    设置成功返回true
///
bool CSMVSendVirtualChannel::SetPhaseAngleStep( double dStep )
{
    if (fabs(m_dPhaseAngleStep - dStep) >= FLOAT_PRECISION)
    {
        m_dPhaseAngleStep = dStep;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }

    return true;
}

///
/// @brief
///    设置频率步长
///
/// @param[in]  dStep    频率步长
///
/// @return
///     bool    设置成功返回true
///
bool CSMVSendVirtualChannel::SetFrequencyStep( double dStep )
{
    if (fabs(m_dFrequencyStep - dStep) >= FLOAT_PRECISION)
    {
        m_dFrequencyStep = dStep;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }

    return true;
}

///
/// @brief
///    获得映射到此虚拟通道的真实通道的数量
///
/// @return
///     int    映射到此虚拟通道的真实通道的数量
///
int CSMVSendVirtualChannel::GetChannelCount()
{
    int     iRet    = 0;


    //
    // 手动试验、状态序列功能同名的虚拟通道共用同一个通道容器
    // 如果是手动试验的虚拟通道，那么直接返回通道容器大小
    // 如果是状态序列的虚拟通道，那么返回手动试验中同名的虚拟通道的通道容器大小
    //
    if (FUNCTION_TYPE_MANUAL_TEST == m_eFunctionType)
    {
        iRet = (int)m_vecChannelGroup.size();
    }
    else
    {
        // 获得手动试验对象
        CParamconfigManualTest* pParamConfigManualTest = CParamconfigManualTest::getInstance();
        if (pParamConfigManualTest)
        {
            // 获得手动试验管理器中指定名字的虚拟通道
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
///    获得SVM通道映射表中被勾选的通道的索引（从0开始计数）
///
/// @return
///     int    被勾选的通道的索引
///
int CSMVSendVirtualChannel::GetSelectedIndex()
{
    int     iRet    = 0;


    //
    // 手动试验、状态序列功能同名的虚拟通道共用同一个索引
    // 如果是手动试验的虚拟通道，那么直接返回索引
    // 如果是状态序列的虚拟通道，那么返回手动试验中同名的虚拟通道的索引
    //
    if (FUNCTION_TYPE_MANUAL_TEST == m_eFunctionType)
    {
        iRet = m_iSelectedIndex;
    }
    else
    {
        // 获得手动试验对象
        CParamconfigManualTest* pParamConfigManualTest = CParamconfigManualTest::getInstance();
        if (pParamConfigManualTest)
        {
            // 获得手动试验管理器中指定名字的虚拟通道
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
///    设置SVM通道映射表中被勾选的通道的索引（从0开始计数）
///
/// @param[in]  nIndex    被勾选的通道的索引
///
/// @return
///     bool    成功返回true
///
bool CSMVSendVirtualChannel::SetSelectedIndex( int nIndex )
{
    //
    // 手动试验、状态序列功能同名的虚拟通道共用同一个索引
    // 如果是手动试验的虚拟通道，那么直接修改索引
    // 如果是状态序列的虚拟通道，那么修改手动试验中同名的虚拟通道的索引
    //
    if (FUNCTION_TYPE_MANUAL_TEST == m_eFunctionType)
    {
        if ((m_iSelectedIndex != nIndex))
        {
            m_iSelectedIndex = nIndex;

            // 标记为已修改
            if (NULL != m_pParamConfigBase)
            {
                m_pParamConfigBase->SetModified(true);
            }
        }
    }
    else
    {
        // 获得手动试验对象
        CParamconfigManualTest* pParamConfigManualTest = CParamconfigManualTest::getInstance();
        if (!pParamConfigManualTest)
        {
            return false;
        }

        // 获得手动试验管理器中指定名字的虚拟通道
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
///    枚举第一个Channel
///
/// @return
///     CChannel*    成功时返回通道指针，失败时返回NULL
///
CChannel* CSMVSendVirtualChannel::FirstChannel()
{
    //
    // 手动试验、状态序列功能同名的虚拟通道共用同一个通道容器
    // 如果是手动试验的虚拟通道，那么直接返回通道容器中的第一个通道
    // 如果是状态序列的虚拟通道，那么返回手动试验中同名的虚拟通道的通道容器中的第一个通道
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
        // 获得手动试验对象
        CParamconfigManualTest* pParamConfigManualTest = CParamconfigManualTest::getInstance();
        if (pParamConfigManualTest)
        {
            // 获得手动试验管理器中指定名字的虚拟通道
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
///    枚举下一个Channel
///
/// @param[in]  pCurChannel 当前Channel的指针
///
/// @return
///     CChannel*    成功时返回Channel指针，失败时返回NULL
///
CChannel* CSMVSendVirtualChannel::NextChannel( CChannel* pCurChannel )
{
    //
    // 手动试验、状态序列功能同名的虚拟通道共用同一个通道容器
    // 如果是手动试验的虚拟通道，那么直接返回通道容器中的下一个通道
    // 如果是状态序列的虚拟通道，那么返回手动试验中同名的虚拟通道的通道容器中的下一个通道
    //
    if (FUNCTION_TYPE_MANUAL_TEST == m_eFunctionType)
    {
        CChannel*                           pChannel   = NULL;
        std::vector<CChannel*>::iterator    iter;


        iter = m_vecChannelGroup.begin();
        while (m_vecChannelGroup.end() != iter)
        {
            // 查找当前GOOSE控制块的位置
            pChannel = *iter;
            if (pChannel == pCurChannel)
            {
                // 获得下一个GOOSE控制块的位置
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
        // 获得手动试验对象
        CParamconfigManualTest* pParamConfigManualTest = CParamconfigManualTest::getInstance();
        if (pParamConfigManualTest)
        {
            // 获得手动试验管理器中指定名字的虚拟通道
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
///    获得指定索引号的映射到此虚拟通道的真实通道
///
/// @param[in]  nIndex    通道索引号
///
/// @return
///     CChannel*    成功时返回Channel指针，失败时返回NULL
///
CChannel* CSMVSendVirtualChannel::GetChannel( int nIndex )
{
    CChannel* pChannel = NULL;


    //
    // 手动试验、状态序列功能同名的虚拟通道共用同一个通道容器
    // 如果是手动试验的虚拟通道，那么直接返回通道容器中的指定索引号的通道
    // 如果是状态序列的虚拟通道，那么返回手动试验中同名的虚拟通道的通道容器中的指定索引号的通道
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
        // 获得手动试验对象
        CParamconfigManualTest* pParamConfigManualTest = CParamconfigManualTest::getInstance();
        if (pParamConfigManualTest)
        {
            // 获得手动试验管理器中指定名字的虚拟通道
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
///    深度拷贝一个虚拟通道对象
///
/// @param[in]
///     pOldVirtualChannel    -   已存在的虚拟通道对象
///
/// @return
///     bool    拷贝成功返回TRUE
///
bool CSMVSendVirtualChannel::__Copy( CSMVSendVirtualChannel* pOldVirtualChannel )
{
    // 基本参数
    m_strName                   = pOldVirtualChannel->m_strName;
    m_dAmplitude                = pOldVirtualChannel->m_dAmplitude;
    m_dPhaseAngle               = pOldVirtualChannel->m_dPhaseAngle;
    m_dFrequency                = pOldVirtualChannel->m_dFrequency;
    m_dAmplitudeStep            = pOldVirtualChannel->m_dAmplitudeStep;
    m_dPhaseAngleStep           = pOldVirtualChannel->m_dPhaseAngleStep;
    m_dFrequencyStep            = pOldVirtualChannel->m_dFrequencyStep;

    // 谐波参数
    m_harmonic.m_eHarmonicType  = pOldVirtualChannel->m_harmonic.m_eHarmonicType;
    m_harmonic.m_dAmplitude     = pOldVirtualChannel->m_harmonic.m_dAmplitude;
    m_harmonic.m_dPhaseAngle    = pOldVirtualChannel->m_harmonic.m_dPhaseAngle;

    return true;
}

///
/// @brief
///    加载SMV发送虚拟通道
///
/// @param[in]  xml    指向XML对象的引用
///
/// @return
///     bool    true表示加载成功
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

    // 如果是状态序列的虚拟通道，那么加载虚拟通道的谐波设置
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
        // 如果是手动试验的虚拟通道，那么加载步长

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
///    保存SMV发送虚拟通道
///
/// @param[in]  xml    指向XML对象的引用
///
/// @return
///     bool    true表示保存成功
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

    // 如果是状态序列的虚拟通道，那么保存虚拟通道的谐波设置
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
        // 如果是手动试验的虚拟通道，那么保存步长

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
///    获得虚拟通道的电流类型
///
/// @return
///     eCurrentType    电流类型
///
eCurrentType CSMVSendVirtualChannel::GetACDCType()
{
    eCurrentType eType = CURRENT_TYPE_UNKNOWN;


    //
    // 手动试验、状态序列功能同名的虚拟通道共用电流类型
    // 如果是手动试验的虚拟通道，那么直接返回通道的电流类型
    // 如果是状态序列的虚拟通道，那么返回手动试验中同名的虚拟通道的电流类型
    //
    if (FUNCTION_TYPE_MANUAL_TEST == m_eFunctionType)
    {
        eType = m_eACDCType;
    }
    else
    {
        // 获得手动试验对象
        CParamconfigManualTest* pParamConfigManualTest = CParamconfigManualTest::getInstance();
        if (pParamConfigManualTest)
        {
            // 获得手动试验管理器中指定名字的虚拟通道
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
///    获得谐波设置对象的指针
///
/// @return
///     CHarmonic*    谐波设置对象的指针
///
CHarmonic* CSMVSendVirtualChannel::GetHarmonic()
{
    return &m_harmonic;
}

///
/// @brief
///    幅值是否发生了变化
///
/// @param[in]  bIsChanged    是否发生了变化
///
bool CSMVSendVirtualChannel::IsAmplitudeChanged()
{
    return m_bIsAmplitudeChanged;
}

///
/// @brief
///    设置通道的幅值是否发生了变化
///
/// @param[in]  bIsChanged    是否发生了变化
///
void CSMVSendVirtualChannel::__SetAmplitudeChanged( bool bIsChanged )
{
    m_bIsAmplitudeChanged = bIsChanged;
}

///
/// @brief
///    相角是否发生了变化
///
/// @param[in]  bIsChanged    是否发生了变化
///
bool CSMVSendVirtualChannel::IsPhaseAngleChanged()
{
    return m_bIsPhaseAngleChanged;
}

///
/// @brief
///    设置通道的相角是否发生了变化
///
/// @param[in]  bIsChanged    是否发生了变化
///
void CSMVSendVirtualChannel::__SetPhaseAngleChanged( bool bIsChanged )
{
    m_bIsPhaseAngleChanged = bIsChanged;
}

///
/// @brief
///    频率是否发生了变化
///
/// @param[in]  bIsChanged    是否发生了变化
///
bool CSMVSendVirtualChannel::IsFrequencyChanged()
{
    return m_bIsFrequencyChanged;
}

///
/// @brief
///    设置通道的频率是否发生了变化
///
/// @param[in]  bIsChanged    是否发生了变化
///
void CSMVSendVirtualChannel::__SetFrequencyChanged( bool bIsChanged )
{
    m_bIsFrequencyChanged = bIsChanged;
}

///
/// @brief
///     比较虚拟通道的幅值、相角、频率、谐波
///
/// @param[in]  eCompareType    比较方式
/// @param[in]  pOldSMVSendVirtualChannel   与当前通道进行比较的虚拟通道
///
/// @return
///     bool    相同返回true
///
bool CSMVSendVirtualChannel::Compare( enumCompareType eCompareType, CSMVSendVirtualChannel* pOldSMVSendVirtualChannel )
{
    bool    bRet = true;


    // 比较虚拟通道的幅值
    if (fabs(m_dAmplitude - pOldSMVSendVirtualChannel->m_dAmplitude) >= FLOAT_PRECISION)
    {
        // 如果需要找出所有的不同，那么设置变位标识
        if (TYPE_FIND_ALL_DIFFERENT == eCompareType)
        {
            __SetAmplitudeChanged(true);
            bRet = false;
        }
        // 如果只需找出一处不同，那么直接返回
        else if (TYPE_FIND_ONE_DIFFERENT == eCompareType)
        {
            return false;
        }
    }

    // 比较相角
    if (fabs(m_dPhaseAngle - pOldSMVSendVirtualChannel->m_dPhaseAngle) >= FLOAT_PRECISION)
    {
        // 如果需要找出所有的不同，那么设置变位标识
        if (TYPE_FIND_ALL_DIFFERENT == eCompareType)
        {
            __SetPhaseAngleChanged(true);
            bRet = false;
        }
        // 如果只需找出一处不同，那么直接返回
        else if (TYPE_FIND_ONE_DIFFERENT == eCompareType)
        {
            return false;
        }
    }

    // 比较频率
    if (fabs(m_dFrequency - pOldSMVSendVirtualChannel->m_dFrequency) >= FLOAT_PRECISION)
    {
        // 如果需要找出所有的不同，那么设置变位标识
        if (TYPE_FIND_ALL_DIFFERENT == eCompareType)
        {
            __SetFrequencyChanged(true);
            bRet = false;
        }
        // 如果只需找出一处不同，那么直接返回
        else if (TYPE_FIND_ONE_DIFFERENT == eCompareType)
        {
            return false;
        }
    }

    // 比较谐波
    if ((FUNCTION_TYPE_STATE_SEQUENCE == m_eFunctionType) && !(m_harmonic.Compare(pOldSMVSendVirtualChannel->m_harmonic)))
    {
        bRet = false;
    }

    return bRet;
}
