/// @file
///
/// @brief
///     IEC61850 9-2 控制块
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.29
///
/// 修改历史：
///

#ifndef __IEC9_2_CB_H__
#define __IEC9_2_CB_H__
#include "netcb.h"

///
/// @brief
///     对IEC61850-9-2通道品质的操作
///
enum enumQualityOperateType
{
    QUALITY_OPERATE_CLEAR = 0,                                          ///< 清零
    QUALITY_OPERATE_TEST,                                               ///< 置检修
    QUALITY_OPERATE_RUN                                                 ///< 置运行
};

///
/// @brief
///     IEC61850-9-2控制块
///
class CIEC9_2Cb : public CNetCb
{
    friend class CParamConfigGOOSEBase;
    friend class CParamConfigGOOSESend;
    friend class CParamConfigSMVSend;
    friend class CSCLManager;
    friend class CFrameSendNet;

protected:
    CIEC9_2Cb(void);
    virtual ~CIEC9_2Cb(void);

public:
    ///
    /// @brief
    ///     构造函数
    ///
    /// @param[in]  
    ///     pParamConfigBase    指向参数配置管理器的指针
    ///
    CIEC9_2Cb(CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    获得SvID
    ///
    /// @return
    ///     wstring    SvID
    ///
    std::wstring GetSvID();

    ///
    /// @brief
    ///    设置SvID
    ///
    /// @param[in]  strSvID  SvID
    ///
    void SetSvID(std::wstring strSvID);

    ///
    /// @brief
    ///    获得数据集索引
    ///
    /// @return
    ///     wstring    数据集索引
    ///
    std::wstring GetDataSetRef();

    ///
    /// @brief
    ///    设置数据集索引
    ///
    /// @param[in]  strDataSetRef  数据集索引
    ///
    void SetDataSetRef(std::wstring strDataSetRef);

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
    ///    获得数据集可选项
    ///
    /// @return
    ///     bool    是否发送数据集
    ///
    bool GetDataSetOpt() const;

    ///
    /// @brief
    ///    设置数据集可选项
    ///
    /// @param[in]  bNewValue    是否发送数据集
    ///
    void SetDataSetOpt(bool bNewValue);

    ///
    /// @brief
    ///    获得刷新时间可选项
    ///
    /// @return
    ///     bool    是否发送刷新时间
    ///
    bool GetRefrTmOpt() const;

    ///
    /// @brief
    ///    设置刷新时间可选项
    ///
    /// @param[in]  bNewValue    是否发送刷新时间
    ///
    void SetRefrTmOpt(bool bNewValue);

    ///
    /// @brief
    ///    获得采样率可选项
    ///
    /// @return
    ///     bool    是否发送采样率
    ///
    bool GetSmpRateOpt() const;

    ///
    /// @brief
    ///    设置采样率可选项
    ///
    /// @param[in]  bNewValue    是否发送采样率
    ///
    void SetSmpRateOpt(bool bNewValue);

    ///
    /// @brief
    ///    获得报文加密可选项
    ///
    /// @return
    ///     bool    是否对报文加密
    ///
    bool GetSecurityOpt() const;

    ///
    /// @brief
    ///    设置报文加密可选项
    ///
    /// @param[in]  bNewValue    是否对报文加密
    ///
    void SetSecurityOpt(bool bNewValue);

    ///
    /// @brief
    ///    设置控制块所有通道的品质
    ///
    /// @param[in]  eType    操作类型，如：清零、置检修、置运行
    ///
    void SetAllChannelsQuality(enumQualityOperateType eType);

protected:
    ///
    /// @brief
    ///    加载IEC61850-9-2控制块
    ///
    /// @param[in]  pXml 指向XML对象的引用
    ///
    /// @return
    ///     bool    true表示加载成功
    ///
    bool __LoadConfig(CMarkup& xml);

    ///
    /// @brief
    ///    保存IEC61850-9-2控制块
    ///
    /// @param[in]  pXml 指向XML对象的引用
    ///
    /// @return
    ///     bool    true表示保存成功
    ///
    bool __SaveConfig(CMarkup& xml);

    ///
    /// @brief
    ///    深度拷贝一个IEC61850-9-2控制块
    ///
    /// @param[in]
    ///     pIEC9_2Cb    -   控制块指针
    ///
    /// @return
    ///     bool    拷贝成功返回TRUE
    ///
    virtual bool __Copy(CIEC9_2Cb* pIEC9_2Cb);

    ///
    /// @brief
    ///    深度拷贝一个扫描列表中的IEC61850-9-2控制块
    ///
    /// @param[in]
    ///     pSav92Msg    -   IEC61850-9-2报文的编解码对象
    ///
    /// @return
    ///     bool    拷贝成功返回TRUE
    ///
    virtual bool __Copy(Sav92Msg* pSav92Msg);

private:
    std::wstring                        m_strSvID;                      ///< SvID
    std::wstring                        m_strDataSetRef;                ///< 数据集索引
    bool                                m_bDataSetOpt;                  ///< 是否发送数据集
    bool                                m_bRefrTmOpt;                   ///< 是否发送刷新时间
    bool                                m_bSmpRateOpt;                  ///< 是否发送采样率
    bool                                m_bSecurityOpt;                 ///< 是否对报文进行加密
};
#endif
