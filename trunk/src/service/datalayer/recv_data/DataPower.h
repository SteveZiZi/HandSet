/// @file
///
/// @brief
///     定义功率数据信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.4.23
///
/// 修改历史：
///

#pragma once

#include "DataEffectiveVal.h"


///
/// @brief
///    功率类，包含每个通道的有效值，相位等信息，可以从有效值类继承使用
///
class CDataPower : public CDataEffectiveVal
{
public:
    CDataPower(void);
    ~CDataPower(void);

    ///
    /// @brief
    ///    初始化获取相别/类型信息，在添加完所有通道后进行调用
    ///
    void init();

    ///
    /// @brief
    ///    设置三相电压通道
    ///
    /// @param[in]  nVolPhaseA A相电压通道号
    /// @param[in]  nVolPhaseB B相电压通道号
    /// @param[in]  nVolPhaseC C相电压通道号
    ///
    bool SetVoltageChannles(uint32_t nVolPhaseA, uint32_t nVolPhaseB, uint32_t nVolPhaseC);

    ///
    /// @brief
    ///    获取三相电压通道
    ///
    /// @param[out]  nVolPhaseA 保存A相电压通道号
    /// @param[out]  nVolPhaseB 保存B相电压通道号
    /// @param[out]  nVolPhaseC 保存C相电压通道号
    ///
    bool GetVoltageChannles(uint32_t& nVolPhaseA, uint32_t& nVolPhaseB, uint32_t& nVolPhaseC);

    ///
    /// @brief
    ///    设置三相电流通道
    ///
    /// @param[in]  nCurPhaseA A相电流通道号
    /// @param[in]  nCurPhaseB B相电流通道号
    /// @param[in]  nCurPhaseC C相电流通道号
    ///
    bool SetCurrentChannles(uint32_t nCurPhaseA, uint32_t nCurPhaseB, uint32_t nCurPhaseC);

    ///
    /// @brief
    ///    获取三相电流通道
    ///
    /// @param[out]  nCurPhaseA 保存A相电流通道号
    /// @param[out]  nCurPhaseB 保存B相电流通道号
    /// @param[out]  nCurPhaseC 保存C相电流通道号
    ///
    bool GetCurrentChannles(uint32_t& nCurPhaseA, uint32_t& nCurPhaseB, uint32_t& nCurPhaseC);

    ///
    /// @brief
    ///    获取有功功率
    ///
    /// @param[out]  fPhaseAData        保存A相有功功率
    /// @param[out]  fPhaseBData        保存B相有功功率
    /// @param[out]  fPhaseCData        保存C相有功功率
    /// @param[out]  fTotalData         保存总有功功率
    ///
    /// @return
    ///
    void GetActivePower( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData, float& fTotalData);

    ///
    /// @brief
    ///    获取无功功率
    ///
    /// @param[out]  fPhaseAData        保存A相无功功率
    /// @param[out]  fPhaseBData        保存B相无功功率
    /// @param[out]  fPhaseCData        保存C相无功功率
    /// @param[out]  fTotalData         保存总无功功率
    ///
    /// @return
    ///
    void GetReactivePower( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData, float& fTotalData);

    ///
    /// @brief
    ///    获取视在功率
    ///
    /// @param[out]  fPhaseAData        保存A相视在功率
    /// @param[out]  fPhaseBData        保存B相视在功率
    /// @param[out]  fPhaseCData        保存C相视在功率
    /// @param[out]  fTotalData         保存总视在功率
    ///
    /// @return
    ///
    void GetApparentPower( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData, float& fTotalData);

    ///
    /// @brief
    ///    获取功率因数
    ///
    /// @param[out]  fPhaseAData        保存A相功率因数
    /// @param[out]  fPhaseBData        保存B相功率因数
    /// @param[out]  fPhaseCData        保存C相功率因数
    /// @param[out]  fTotalData         保存总功率因数
    ///
    /// @return
    ///
    void GetPowerFactor( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData, float& fTotalData);

    ///
    /// @brief
    ///    获取电压有效值信息
    ///
    /// @param[out]  fPhaseAData        保存A相电压有效值
    /// @param[out]  fPhaseBData        保存B相电压有效值
    /// @param[out]  fPhaseCData        保存C相电压有效值
    ///
    /// @return
    ///     获取成功返回true
    ///
    bool GetVotageEffectValue( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData);

    ///
    /// @brief
    ///    获取电压相位信息
    ///
    /// @param[out]  fPhaseAData        保存A相电压相位
    /// @param[out]  fPhaseBData        保存B相电压相位
    /// @param[out]  fPhaseCData        保存C相电压相位
    ///
    /// @return
    ///     获取成功返回true
    ///
    bool GetVoltageAngle( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData);

    ///
    /// @brief
    ///    获取电流有效值信息
    ///
    /// @param[out]  fPhaseAData        保存A相电流有效值
    /// @param[out]  fPhaseBData        保存B相电流有效值
    /// @param[out]  fPhaseCData        保存C相电流有效值
    ///
    /// @return
    ///     获取成功返回true
    ///
    bool GetCurrentEffectValue( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData);

    ///
    /// @brief
    ///    获取电流相位信息
    ///
    /// @param[out]  fPhaseAData        保存A相电流相位
    /// @param[out]  fPhaseBData        保存B相电流相位
    /// @param[out]  fPhaseCData        保存C相电流相位
    ///
    /// @return
    ///     获取成功返回true
    ///
    bool GetCurrentAngle( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData);


    ///
    /// @brief
    ///    清空功率信息
    ///
    void ClearPowerData();

private:
    friend class CFrameRecv;

protected:
    float    m_fActivePower[3];                                       ///< 有功功率，分别对应ABC三相
    float    m_fReactivePower[3];                                     ///< 无功功率，分别对应ABC三相
    float    m_fApparentPower[3];                                     ///< 视在功率，分别对应ABC三相
    float    m_fPowerFactor[3];                                       ///< 功率因数，分别对应ABC三相
    float    m_fTotalActivePower;                                     ///< 总有功功率
    float    m_fTotalReactivePower;                                   ///< 总无功功率
    float    m_fTotalApparentPower;                                   ///< 总视在功率
    float    m_fTotalPowerFactor;                                     ///< 总功率因数
};
