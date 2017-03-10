///
/// @brief
///     谐波类
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.6.14
///
/// 修改历史：
///

#include "StdAfx.h"
#include "Harmonic.h"
#include "ParamConfigSMVSend.h"
#include "SMVSendVirtualChannel.h"

///
/// @brief
///    构造函数
///
CHarmonic::CHarmonic(void)
:m_pSmvSendVirtualChannel(NULL)
,m_pParamConfigBase(NULL)
,m_eHarmonicType(HARMONIC_NONE)
,m_dAmplitude(0.0f)
,m_dPhaseAngle(0.0f)
{
}

///
/// @brief
///    构造函数
///
/// @param[in]  pParamConfigBase    参数配置管理器
///
CHarmonic::CHarmonic( CParamConfigBase* pParamConfigBase )
:m_pSmvSendVirtualChannel(NULL)
,m_pParamConfigBase(pParamConfigBase)
,m_eHarmonicType(HARMONIC_NONE)
,m_dAmplitude(0.0f)
,m_dPhaseAngle(0.0f)
{

}

///
/// @brief
///    构造函数
///
/// @param[in]  pSmvSendVirtualChannel  指向虚拟发送通道对象的指针
/// @param[in]  pParamConfigBase    参数配置管理器
///
CHarmonic::CHarmonic( CSMVSendVirtualChannel* pSmvSendVirtualChannel, CParamConfigBase* pParamConfigBase )
:m_pSmvSendVirtualChannel(pSmvSendVirtualChannel)
,m_pParamConfigBase(pParamConfigBase)
,m_eHarmonicType(HARMONIC_NONE)
,m_dAmplitude(0.0f)
,m_dPhaseAngle(0.0f)
{

}

///
/// @brief
///    析构函数
///
CHarmonic::~CHarmonic(void)
{
}

///
/// @brief
///    获得谐波类型
///
/// @return
///     enumHarmonicType    谐波类型
///
enumHarmonicType CHarmonic::GetHarmonicType()
{
    return m_eHarmonicType;
}

///
/// @brief
///    设置谐波类型
///
/// @param[in]  eNewType    谐波类型
///
void CHarmonic::SetHarmonicType( enumHarmonicType eNewType )
{
    if (m_eHarmonicType != eNewType)
    {
        m_eHarmonicType = eNewType;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    获得幅值
///
/// @return
///     double    幅值
///
double CHarmonic::GetAmplitude()
{
    double                      dRet = 0.0f;


    // 获得SMV发送参数配置管理器
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if ((NULL == pParamConfigSMVSend) || (NULL == m_pSmvSendVirtualChannel))
    {
        return dRet;
    }

    //
    // 根据采样值显示方式，算出最终的返回值
    //
    switch(pParamConfigSMVSend->GetSVDisplay())
    {
    case SMV_VALUE_TYPE_PRIMARY:                                        ///< 一次值
        {
            dRet =  m_dAmplitude;
        }
        break;

    case SMV_VALUE_TYPE_SECONDARY:                                      ///< 二次值
        {
            // 获得当前选中的通道
            CChannel* pChannel = m_pSmvSendVirtualChannel->GetChannel(m_pSmvSendVirtualChannel->GetSelectedIndex());
            if (NULL != pChannel)
            {
                // 获得通道的一次额定值、二次额定值
                double dOnceRatings         = pChannel->GetOnceRatings();
                double dSecondaryRatings    = pChannel->GetSecondaryRatings();

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
///    设置幅值
///
/// @param[in]  fAmplitude    新的幅值
///
void CHarmonic::SetAmplitude( double dAmplitude )
{
    double                          dData = 0.0f;


    // 获得SMV发送参数配置管理器
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if ((NULL == pParamConfigSMVSend) || (NULL == m_pSmvSendVirtualChannel))
    {
        return;
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
            // 获得当前选中的通道
            CChannel* pChannel = m_pSmvSendVirtualChannel->GetChannel(m_pSmvSendVirtualChannel->GetSelectedIndex());
            if (NULL == pChannel)
            {
                return;
            }

            // 获得通道的一次额定值、二次额定值
            double      dOnceRatings          = pChannel->GetOnceRatings();
            double      dSecondaryRatings     = pChannel->GetSecondaryRatings();

            // 根据比例关系，算出最终返回值
            if (fabs(dSecondaryRatings) < FLOAT_PRECISION)
            {
                return;
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
    }
}

///
/// @brief
///    获得相角
///
/// @return
///     double    相角
///
double CHarmonic::GetPhaseAngle()
{
    return m_dPhaseAngle;
}

///
/// @brief
///    设置相角
///
/// @param[in]  fPhaseAngle    相角
///
void CHarmonic::SetPhaseAngle( double dPhaseAngle )
{
    if (m_dPhaseAngle != dPhaseAngle)
    {
        m_dPhaseAngle = dPhaseAngle;

        // 标记为已修改
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///     获得谐波对象所属的虚拟通道
///
/// @return
///     CSMVSendVirtualChannel* 指向虚拟通道对象的指针
///
CSMVSendVirtualChannel* CHarmonic::GetSmvSendVirtualChannel() const
{
    return m_pSmvSendVirtualChannel;
}

///
/// @brief
///     设置谐波对象所属的虚拟通道
///
/// @param[in]  val 指向虚拟通道对象的指针
///
void CHarmonic::SetSmvSendVirtualChannel( CSMVSendVirtualChannel* val )
{
    m_pSmvSendVirtualChannel = val;
}

///
/// @brief
///     比较谐波类型、幅值、相角
///
/// @param[in]  oldHarmonic     待对比的谐波对象
///
/// @return
///     bool    相同返回true
///
bool CHarmonic::Compare( CHarmonic& oldHarmonic )
{
    // 比较谐波类型
    if (m_eHarmonicType != oldHarmonic.m_eHarmonicType)
    {
        return false;
    }
    // 如果无谐波，那么不进行幅值、相角比较
    else if (HARMONIC_NONE == m_eHarmonicType)
    {
        return true;
    }
    // 比较幅值
    else if (fabs(m_dAmplitude - oldHarmonic.m_dAmplitude) >= FLOAT_PRECISION)
    {
        return false;
    }
    // 比较相角
    else if (fabs(m_dPhaseAngle - oldHarmonic.m_dPhaseAngle) >= FLOAT_PRECISION)
    {
        return false;
    }

    return true;
}
