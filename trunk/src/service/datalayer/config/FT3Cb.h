///
/// @brief
///     FT3控制块
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.29
///
/// 修改历史：
///

#ifndef __FT3_CB_H__
#define __FT3_CB_H__
#include "basecb.h"


///
/// @brief
///    FT3控制块
///
class CFT3Cb : public CBaseCb
{
    friend class CParamConfigGOOSEBase;
    friend class CParamConfigGOOSESend;
    friend class CParamConfigSMVSend;
    friend class CSCLManager;
    friend class CFrameSendFt3;

protected:
    CFT3Cb(void);
    virtual ~CFT3Cb(void);

public:
    ///
    /// @brief
    ///     构造函数
    ///
    /// @param[in]  
    ///     pParamConfigBase    指向参数配置管理器的指针
    ///
    CFT3Cb(CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    获得LNName
    ///
    /// @return
    ///     int    LNName
    ///
    int GetLNName();

    ///
    /// @brief
    ///    设置LNName
    ///
    /// @param[in]  int LNName
    ///
    void SetLNName(int iLNName);

    ///
    /// @brief
    ///    获得LDName
    ///
    /// @return
    ///     int    LDName
    ///
    int GetLDName();

    ///
    /// @brief
    ///    设置LDName
    ///
    /// @param[in]  int iLDName
    ///
    /// @return
    ///     bool    true表示保存成功
    ///
    bool SetLDName(int iLDName);

    ///
    /// @brief
    ///    获得DSName
    ///
    /// @return
    ///     int    DSName
    ///
    int GetDSName();

    ///
    /// @brief
    ///    设置DSName
    ///
    /// @param[in]  int iDSName
    ///
    void SetDSName(int iDSName);

    ///
    /// @brief
    ///    获得额定相电流（A）
    ///
    /// @return
    ///     double    额定相电流（A）
    ///
    double GetRatedPhaseCurrent();

    ///
    /// @brief
    ///    设置额定相电流（A）
    ///
    /// @param[in]  dRatedPhaseCurrent 额定相电流（A）
    ///
    void SetRatedPhaseCurrent(double dRatedPhaseCurrent);

    ///
    /// @brief
    ///    获得额定中线电流（A）
    ///
    /// @return
    ///     double    额定中线电流（A）
    ///
    double GetRatedNeutralCurrent();

    ///
    /// @brief
    ///    设置额定中线电流（A）
    ///
    /// @param[in]  dRatedNeutralCurrent 额定中线电流（A）
    ///
    void SetRatedNeutralCurrent(double dRatedNeutralCurrent);

    ///
    /// @brief
    ///    获得额定相电压（kV）
    ///
    /// @return
    ///     double    额定相电压（kV）
    ///
    double GetRatedPhaseVoltage();

    ///
    /// @brief
    ///    设置额定相电压（kV）
    ///
    /// @param[in]  dRatedPhaseVoltage 额定相电压（kV）
    ///
    void SetRatedPhaseVoltage(double dRatedPhaseVoltage);

    ///
    /// @brief
    ///    获得额定延时（us）
    ///
    /// @return
    ///     int    额定延时（us）
    ///
    int GetRatedDelay();

    ///
    /// @brief
    ///    设置额定延时（us）
    ///
    /// @param[in]  iRatedDelay 额定延时（us）
    ///
    void SetRatedDelay(int iRatedDelay);

    ///
    /// @brief
    ///    获得状态字#1
    ///
    /// @return
    ///     unsigned short    状态字#1
    ///
    unsigned short GetStatusWord1();

    ///
    /// @brief
    ///    设置状态字#1
    ///
    /// @param[in]  nStatusWord1 状态字#1
    ///
    void SetStatusWord1(unsigned short nStatusWord1);

    ///
    /// @brief
    ///    获得状态字#2
    ///
    /// @return
    ///     unsigned short    状态字#2
    ///
    unsigned short GetStatusWord2();

    ///
    /// @brief
    ///    设置状态字#2
    ///
    /// @param[in]  nStatusWord2 状态字#2
    ///
    void SetStatusWord2(unsigned short nStatusWord2);

    ///
    /// @brief
    ///    使用指定的通道模板来填充控制块
    ///
    /// @param[in]  eType    模板类型
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool UseChannelTemplate(enumSmvRecvConfigTemplateType eType);

    ///
    /// @brief
    ///    更新控制块中所有电压通道的一次额定值
    ///
    /// @param[in]  dNewValue    新的电压一次额定值（V）
    ///
    void UpdateRatedPrimaryVoltage(double dNewValue);

    ///
    /// @brief
    ///    更新控制块中所有电流通道的一次额定值
    ///
    /// @param[in]  dNewValue    新的电流一次额定值（A）
    ///
    void UpdateRatedPrimaryCurrent(double dNewValue);

protected:
    ///
    /// @brief
    ///    加载FT3控制块
    ///
    /// @param[in]  pXml 指向XML对象的引用
    ///
    /// @return
    ///     bool    true表示加载成功
    ///
    bool __LoadConfig(CMarkup& xml);

    ///
    /// @brief
    ///    保存FT3控制块
    ///
    /// @param[in]  pXml 指向XML对象的引用
    ///
    /// @return
    ///     bool    true表示保存成功
    ///
    bool __SaveConfig(CMarkup& xml);

    ///
    /// @brief
    ///    深度拷贝一个FT3控制块
    ///
    /// @param[in]
    ///     pFt3Cb    -   FT3控制块
    ///
    /// @return
    ///     bool    拷贝成功返回TRUE
    ///
    virtual bool __Copy(CFT3Cb* pFt3Cb);

    ///
    /// @brief
    ///    深度拷贝一个扫描列表中的FT3控制块
    ///
    /// @param[in]
    ///     pFt3Msg    -   FT3报文的编解码对象
    ///
    /// @return
    ///     bool    拷贝成功返回TRUE
    ///
    virtual bool __Copy(NanRuiFT3Msg* pFt3Msg);

protected:
    int                                 m_iLNName;                      ///< LNName
    int                                 m_iDSName;                      ///< DSName

    double                              m_dRatedPhaseCurrent;           ///< 额定相电流（A）
    double                              m_dRatedNeutralCurrent;         ///< 额定中线电流（A）
    double                              m_dRatedPhaseVoltage;           ///< 额定相电压（kV）
    int                                 m_iRatedDelay;                  ///< 额定延时（us）
    unsigned short                      m_uiStatusWord1;                ///< 状态字#1
    unsigned short                      m_uiStatusWord2;                ///< 状态字#2
};

///
/// @brief
///    FT3扩展控制块
///
class CFT3ExpandCb: public CFT3Cb
{
    friend class CParamConfigSMVSend;
    friend class CFrameSendFt3;

public:
    ///
    /// @brief
    ///     构造函数
    ///
    /// @param[in]  
    ///     pParamConfigBase    指向参数配置管理器的指针
    ///
    CFT3ExpandCb(CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    使用指定的通道模板来填充控制块
    ///
    /// @param[in]  eType    模板类型
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool UseChannelTemplate(enumSmvRecvConfigTemplateType eType);

protected:
    ///
    /// @brief
    ///    深度拷贝一个扫描列表中的FT3扩展控制块
    ///
    /// @param[in]
    ///     pFt3ExpandMsg    -   FT3扩展报文的编解码对象
    ///
    /// @return
    ///     bool    拷贝成功返回TRUE
    ///
    virtual bool __Copy(GuoWangFT3Msg* pFt3ExpandMsg);
};

#endif
