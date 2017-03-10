/// @file
///
/// @brief
///     SMV接收配置公用配置部分
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.4.26
///
/// 修改历史：
///

#include "StdAfx.h"
#include "SmvRecvConfigPublic.h"
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\utility\data_utility.h"
#include "src\service\datalayer\DsmDataLayer.h"

//定义编译警告todo信息
//#pragma message(__FILE__ "(" PRAGMA_STRING(__LINE__) "): 考虑Uninit时是否要先停止网络接口数据接收")
#define PRAGMA_STRING2(x) #x
#define PRAGMA_STRING(x) PRAGMA_STRING2(x)

// 定义xml节点属性名字
static std::wstring ELEM_NODE_ATTR_NAME       = L"value";                ///< 属性值名
static std::wstring GROUP_NODE_ATTR_NAME      = L"name";                 ///< 组名字属性名

//定义xml不同分类组名
static std::wstring GROUP_NAME_BASIC_CONFIG   =  L"SmvBaseConfig";       ///< 接收基本设置组名
static std::wstring GROUP_NAME_HARMONIC_SHOW  =  L"HarmonicShow";        ///< 谐波次数配置组名
static std::wstring GROUP_NAME_POLARITY_CHECK =  L"PolarityCheck";       ///< 极性检查设置组名
static std::wstring GROUP_NAME_PHASE_CHECK    =  L"PhaseCheck";          ///< 核相设置组名

//定义xml中节点元素的名字
static std::wstring ELEM_NODE_SAMPLE_SHOWTYPE           = L"SampleValueShowType";  ///< 采样值显示类型
static std::wstring ELEM_NODE_SAMPLE_RATE               = L"SampleRate";           ///< 采样率
static std::wstring ELEM_NODE_PHASE_CHECK_ALIGN         = L"PhaseCheckAlign";      ///< 核相数据对齐方式
static std::wstring ELEM_NODE_POLARITY_TRANSFORMER_TYPE = L"TransformerType";      ///< 互感器类型
static std::wstring ELEM_NODE_POLARITY_SILL_VALUE       = L"SillValue";            ///< 门槛值
static std::wstring ELEM_NODE_POLARITY_NULL_SHIFT_VALUE = L"NullShiftValue";       ///< 零漂值


static std::wstring ELEM_NODE_HARMONIC_TYPE[] ={L"HarmonicTypeBaseWav",
                                                L"HarmonicType2",
                                                L"HarmonicType3",
                                                L"HarmonicType4",
                                                L"HarmonicType5",
                                                L"HarmonicType6",
                                                L"HarmonicType7",
                                                L"HarmonicType8",
                                                L"HarmonicType9",
                                                L"HarmonicType10",
                                                L"HarmonicType11",
                                                L"HarmonicType12",
                                                L"HarmonicType13",
                                                L"HarmonicType14",
                                                L"HarmonicType15",
                                                L"HarmonicType16",
                                                L"HarmonicType17",
                                                L"HarmonicType18",
                                                L"HarmonicType19",
                                                L"HarmonicType20",
                                                L"HarmonicType21",
                                                L"HarmonicTypeThd",
                                                L"HarmonicTypeDc"
                                                };

#define SILL_VALUE_DEFAULT          4.0f                                ///< 门槛值默认值
#define NULL_SHIFT_VALUE_DEFAULT    3.0f                                ///< 零漂值默认值

CSmvRecvConfigPublic::CSmvRecvConfigPublic(void)  : m_enSmvDispType(SMV_VALUE_TYPE_SECONDARY)
                                                  , m_enSmv92SampleRate(SMV92_SAMPLE_RATE_AUTO_DETECT)
                                                  , m_enPhaseCheckAlign(SMV_PHASE_CHECK_ALIGN_INDEX)
                                                  , m_enTransFormerType(TRANSFORMER_TYPE_ELECTRONIC)
                                                  , m_fSillValue(SILL_VALUE_DEFAULT)
                                                  , m_fNullShiftValue(NULL_SHIFT_VALUE_DEFAULT)
                                                  , m_bModified(false)
                                                  , m_strCfgFile(L"")
{
    int nSize = sizeof(ELEM_NODE_HARMONIC_TYPE)/sizeof(ELEM_NODE_HARMONIC_TYPE[0]);
    for (int i = 1; i <= nSize; ++i)
    {
        m_mapHarmonicType.insert(std::make_pair(static_cast<HARMONIC_TYPE>(i), true));
    }
}

CSmvRecvConfigPublic::~CSmvRecvConfigPublic(void)
{
    m_mapHarmonicType.clear();
}

///
/// @brief
///    设置界面显示数据类型
///
/// @param[in]  enSmvDispType   要设置的显示数据类型
///
void CSmvRecvConfigPublic::SetSmvDispType( enumSmvValueType enSmvDispType )
{
    if (enSmvDispType != m_enSmvDispType)
    {
        m_enSmvDispType = enSmvDispType;
        m_bModified = true;
    }
}

///
/// @brief
///    获取界面显示数据类型
///
/// @return
///     返回当前设置的界面显示数据类型
///
enumSmvValueType CSmvRecvConfigPublic::GetSmvDispType()
{
    return m_enSmvDispType;
}

///
/// @brief
///    设置采样率信息
///
/// @param[in]  enSampleRate   要设置的采样率
///
void CSmvRecvConfigPublic::SetSmv92SampleRate( SMV92_SAMPLE_RATE enSampleRate , bool bUpdateSelectCtrlBlk/* = true*/)
{
    if (enSampleRate != m_enSmv92SampleRate)
    {
        m_enSmv92SampleRate = enSampleRate;
        m_bModified = true;

        CDsmDataLayer* pDataLayer  = CDsmDataLayer::getInstance();
        CBaseDetectItem* pCurrentCtrlBlk = NULL;
        if (pDataLayer)
        {
            //在设置采样率后，通知dsp修改使用的采样率
            uint32_t nSmapleRate = 4000;
            if (enSampleRate == SMV92_SAMPLE_RATE_AUTO_DETECT)
            {
                pCurrentCtrlBlk = pDataLayer->GetCurrentCtrlBlk();
                if (pCurrentCtrlBlk)
                {
                    nSmapleRate = pCurrentCtrlBlk->nSampleRateDetect;
                    //修改dsp采样率信息
                    pDataLayer->SetDspSampleRate(nSmapleRate);
                    //修改核相对齐参数
                    pDataLayer->GetSampleInterSync()->SetResampleConfig(DSM_CONFIG_DEFAULT_ELEC_FREQUENCY, nSmapleRate, nSmapleRate);
                }
            }
            else
            {
                if (enSampleRate == SMV92_SAMPLE_RATE_12P8K)
                {
                    nSmapleRate = 12800;
                }
                //修改dsp采样率信息
                pDataLayer->SetDspSampleRate(nSmapleRate);
                //修改核相对齐参数
                pDataLayer->GetSampleInterSync()->SetResampleConfig(DSM_CONFIG_DEFAULT_ELEC_FREQUENCY, nSmapleRate, nSmapleRate);
            }

            if (bUpdateSelectCtrlBlk)
            {
                uint32_t nCount = pDataLayer->GetNetDetectItemsCount();
                CNetDetectItem* pDetectItem = NULL;
                for (uint32_t i=0; i<nCount; ++i)
                {
                    pDetectItem = pDataLayer->GetNetDetectItem(i);
                    if (pDetectItem)
                    {
                        if (enSampleRate == SMV92_SAMPLE_RATE_AUTO_DETECT)
                        {
                            pDetectItem->nSampleRate = pDetectItem->nSampleRateDetect;
                        }
                        else
                        {
                            pDetectItem->nSampleRate = nSmapleRate;
                        }
                    }
                }
            }
        }
    }
}

///
/// @brief
///    获取9-2采样率信息
///
/// @return
///     返回当前设置的9-2采样率信息
///
SMV92_SAMPLE_RATE CSmvRecvConfigPublic::GetSmv92SampleRate()
{
    return m_enSmv92SampleRate;
}

///
/// @brief
///    设置核相时采样值对齐方式
///
/// @param[in] enPhaseCheckAlign 指定对齐方式
///
void CSmvRecvConfigPublic::SetPhaseCheckAlign( SMV_PHASE_CHECK_ALIGNMENT enPhaseCheckAlign )
{
    if (enPhaseCheckAlign != m_enPhaseCheckAlign)
    {
        m_enPhaseCheckAlign = enPhaseCheckAlign;
        m_bModified = true;
    }
}

///
/// @brief
///    获取核相是采样值对齐方式
///
/// @return
///     SMV_PHASE_CHECK_ALIGNMENT 返回核相时数据对齐方式
///
SMV_PHASE_CHECK_ALIGNMENT CSmvRecvConfigPublic::GetPhaseCheckAlign()
{
    return m_enPhaseCheckAlign;
}

///
/// @brief
///    设置是否显示某次谐波信息
///
/// @param[in]   enHarmonicType 要设置的谐波类型
/// @param[in]   bEnable        是否显示此类型的谐波信息
///
/// @return
///     true表示设置成功
///
bool CSmvRecvConfigPublic::SetHarmonicTypeEnable( HARMONIC_TYPE enHarmonicType, bool bEnable )
{
    std::map<HARMONIC_TYPE, bool>::iterator itr;
    itr = m_mapHarmonicType.find(enHarmonicType);
    if (itr != m_mapHarmonicType.end())
    {
        if (bEnable != itr->second)
        {
            itr->second = bEnable;
            m_bModified = true;
            return true;
        }
    }
    return false;
}

///
/// @brief
///    获取某次谐波的是否显示情况
///
/// @param[in]  enHarmonicType  要获取的谐波类型
///
/// @return
///     返回true表示此类型的谐波会显示
///
bool CSmvRecvConfigPublic::GetHarmonicTypeEnabled( HARMONIC_TYPE enHarmonicType )
{
    std::map<HARMONIC_TYPE, bool>::iterator itr;
    itr = m_mapHarmonicType.find(enHarmonicType);
    if (itr != m_mapHarmonicType.end())
    {
        return itr->second;
    }
    return false;
}

///
/// @brief
///    从配置文件中加载信息
///
/// @param[in]   strConfigPath  配置文件名字(相对路径)
///
/// @return
///     加载成功返回true
///
bool CSmvRecvConfigPublic::LoadConfigFile( std::wstring strConfigFile )
{
    if (strConfigFile.empty())
    {
        strConfigFile = SMV_RECV_CONFIG_PUBLIC;
    }
    m_strCfgFile = strConfigFile;
    std::wstring strCfgPath = GetAppRunTimeCfgDir() + strConfigFile;
    if (!m_markUpXml.Load(strCfgPath))
    {
        return Reset();
    }

    _LoadToMem();
    return true;
}

///
/// @brief
///    保存当前信息到配置文件
///
/// @param[in]   strConfigPath  配置文件名字(相对路径)
///
/// @return
///     保存成功返回true
///
bool CSmvRecvConfigPublic::SaveToConfigFile()
{
    if (!m_bModified)
    {
        return true;
    }
    //保存采样值显示类型
    if (!SetElemAttrVal(GROUP_NAME_BASIC_CONFIG, ELEM_NODE_SAMPLE_SHOWTYPE, static_cast<int>(m_enSmvDispType)))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T(" CSmvRecvConfigPublic::SaveToConfigFile: SetElemAttrVal(%s) Failed \n"),ELEM_NODE_SAMPLE_SHOWTYPE.c_str()));
    }
    //保存采样率
//     if (!SetElemAttrVal(GROUP_NAME_BASIC_CONFIG, ELEM_NODE_SAMPLE_RATE, static_cast<int>(m_enSmv92SampleRate)))
//     {
//         RETAILMSG(OUTPUT_LOG_MSG, (_T(" CSmvRecvConfigPublic::SaveToConfigFile: SetElemAttrVal(%s) Failed \n"),ELEM_NODE_SAMPLE_RATE.c_str()));
//     }
    //保存核相对齐
    if (!SetElemAttrVal(GROUP_NAME_PHASE_CHECK, ELEM_NODE_PHASE_CHECK_ALIGN, static_cast<int>(m_enPhaseCheckAlign)))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T(" CSmvRecvConfigPublic::SaveToConfigFile: SetElemAttrVal(%s) Failed \n"),ELEM_NODE_PHASE_CHECK_ALIGN.c_str()));
    }
    //设置互感器类型
    if (!SetElemAttrVal(GROUP_NAME_POLARITY_CHECK, ELEM_NODE_POLARITY_TRANSFORMER_TYPE, static_cast<int>(m_enTransFormerType)))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T(" CSmvRecvConfigPublic::SaveToConfigFile: SetElemAttrVal(%s) Failed \n"),ELEM_NODE_POLARITY_TRANSFORMER_TYPE.c_str()));
    }
    //设置门槛值
    std::wstring strResult;
    if (FloatStrConvert(m_fSillValue, strResult))
    {
        if (!SetElemAttrVal(GROUP_NAME_POLARITY_CHECK, ELEM_NODE_POLARITY_SILL_VALUE, strResult))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T(" CSmvRecvConfigPublic::SaveToConfigFile: SetElemAttrVal(%s) Failed \n"),ELEM_NODE_POLARITY_SILL_VALUE.c_str()));
        }
    }

    //设置零漂值
    if (FloatStrConvert(m_fNullShiftValue, strResult))
    {
        if (!SetElemAttrVal(GROUP_NAME_POLARITY_CHECK, ELEM_NODE_POLARITY_NULL_SHIFT_VALUE, strResult))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T(" CSmvRecvConfigPublic::SaveToConfigFile: SetElemAttrVal(%s) Failed \n"),ELEM_NODE_POLARITY_NULL_SHIFT_VALUE.c_str()));
        }
    }

    //设置不同次谐波是否需要显示
    int nSize = sizeof(ELEM_NODE_HARMONIC_TYPE)/sizeof(ELEM_NODE_HARMONIC_TYPE[0]);
    bool bResult = false;
    for (int i = 1; i <= nSize; ++i)
    {
        if (GetHarmonicTypeEnabled(static_cast<HARMONIC_TYPE>(i)))
        {
            bResult = SetElemAttrVal(GROUP_NAME_HARMONIC_SHOW, ELEM_NODE_HARMONIC_TYPE[i - 1], L"true" );
        }
        else
        {
            bResult = SetElemAttrVal(GROUP_NAME_HARMONIC_SHOW, ELEM_NODE_HARMONIC_TYPE[i - 1], L"false" );
        }

        if (!bResult)
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T("CSmvRecvConfigPublic::SaveToConfigFile: pSmvCfgFile->SetElemAttrVal(%s) Failed \n"),ELEM_NODE_HARMONIC_TYPE[i - 1].c_str()));
        }
    }

    std::wstring strCfgPath = GetAppRunTimeCfgDir() + m_strCfgFile;
    if (!m_markUpXml.Save(strCfgPath))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T("CSmvRecvConfigFile::Save() : Save to (%s)  Failed \n"), m_strCfgFile.c_str()));
        return false;
    }

    m_bModified = false; 
    return true;
}

///
/// @brief
///    获取配置实例接口
///
CSmvRecvConfigPublic* CSmvRecvConfigPublic::getInstance()
{ 
    static CSmvRecvConfigPublic s_SmvRecvConfigPub;
    return &s_SmvRecvConfigPub;
}

///
/// @brief
///    获取指定节点的属性数据
///
std::wstring CSmvRecvConfigPublic::GetElemAttrVal(const std::wstring& strGroup, const std::wstring& strElement)
{
    m_markUpXml.ResetPos();

    while(m_markUpXml.FindChildElem(_T("group")))
    {
        m_markUpXml.IntoElem();
        if (m_markUpXml.GetAttrib(GROUP_NODE_ATTR_NAME) == strGroup)
        {
            if(m_markUpXml.FindChildElem(strElement) && m_markUpXml.IntoElem())
            {
                return m_markUpXml.GetAttrib(ELEM_NODE_ATTR_NAME);
            }
        }
        m_markUpXml.OutOfElem();
    }
    RETAILMSG(OUTPUT_LOG_MSG, (_T("CSmvRecvConfigFile::_GetElemAttr(): Element (%s) Not Exits in (%s) \n"), strElement.c_str(), m_strCfgFile.c_str()));
    return _T("");
}

///
/// @brief
///     设置指定节点的属性数据
///
/// @param[in]  strElement 要设置数据的元素名字
/// @param[in]  strAttrValue 要设置的属性值
///
/// @return
///     bool    true表示设置成功
///
bool CSmvRecvConfigPublic::SetElemAttrVal( const std::wstring& strGroup, const std::wstring& strElement, const std::wstring& strAttrValue)
{
    m_markUpXml.ResetPos();
    bool bGroupFound = false;
    while(m_markUpXml.FindChildElem(_T("group")))
    {
        m_markUpXml.IntoElem();
        if (m_markUpXml.GetAttrib(GROUP_NODE_ATTR_NAME) == strGroup)
        {
            bGroupFound = true;
            if(m_markUpXml.FindChildElem(strElement) && m_markUpXml.IntoElem())
            {
                return m_markUpXml.SetAttrib(ELEM_NODE_ATTR_NAME, strAttrValue);
            }
            else //当前组未找到对应的node
            {
                if (m_markUpXml.AddChildElem(strElement))
                {
                    m_markUpXml.IntoElem();
                    return m_markUpXml.SetAttrib(ELEM_NODE_ATTR_NAME, strAttrValue);
                }
            }
        }
        m_markUpXml.OutOfElem();
    }
    if(!bGroupFound)
    {
        //未找到group，添加group，添加实际节点
        if (m_markUpXml.AddChildElem(L"group"))
        {
            m_markUpXml.IntoElem();
            m_markUpXml.SetAttrib(GROUP_NODE_ATTR_NAME, strGroup);

            if (m_markUpXml.AddChildElem(strElement))
            {
                m_markUpXml.IntoElem();
                return m_markUpXml.SetAttrib(ELEM_NODE_ATTR_NAME, strAttrValue);
            }
        }
    }
    return false;
}

///
/// @brief
///     设置指定节点的属性数据
///
/// @param[in]  strElement 要设置数据的元素名字
/// @param[in]  nAttrValue 要设置的属性值
///
/// @return
///     bool    true表示设置成功
///
bool CSmvRecvConfigPublic::SetElemAttrVal(const std::wstring& strGroup, const std::wstring& strElement, int nAttrValue )
{
    m_markUpXml.ResetPos();
    bool bGroupFound = false;
    while(m_markUpXml.FindChildElem(_T("group")))
    {
        m_markUpXml.IntoElem();
        if (m_markUpXml.GetAttrib(GROUP_NODE_ATTR_NAME) == strGroup)
        {
            bGroupFound = true;
            if(m_markUpXml.FindChildElem(strElement) && m_markUpXml.IntoElem())
            {
                return m_markUpXml.SetAttrib(ELEM_NODE_ATTR_NAME, nAttrValue);
            }
            else //当前组未找到对应的node
            {
                if (m_markUpXml.AddChildElem(strElement))
                {
                    m_markUpXml.IntoElem();
                    return m_markUpXml.SetAttrib(ELEM_NODE_ATTR_NAME, nAttrValue);
                }
            }
        }
        m_markUpXml.OutOfElem();
    }
    if(!bGroupFound)
    {
        //未找到group，添加group，添加实际节点
        if (m_markUpXml.AddChildElem(L"group"))
        {
            m_markUpXml.IntoElem();
            m_markUpXml.SetAttrib(GROUP_NODE_ATTR_NAME, strGroup);

            if (m_markUpXml.AddChildElem(strElement))
            {
                m_markUpXml.IntoElem();
                return m_markUpXml.SetAttrib(ELEM_NODE_ATTR_NAME, nAttrValue);
            }
        }
    }
    return false;
}

///
/// @brief
///    恢复默认设置
///
bool CSmvRecvConfigPublic::Reset()
{
    // 获取备份配置文件路径
    std::wstring strBackup, strRun;
    strBackup = GetAppDir() + DEFAULT_CFG_BACKUP + m_strCfgFile;
    strRun    = GetAppRunTimeCfgDir() + m_strCfgFile;
    // 拷贝备份的配置文件
    if(!CopyFile(strBackup.c_str(),strRun.c_str(), FALSE))
    {
        return false;
    }

    if (!m_markUpXml.Load(strRun))
    {
        return false;
    }

    _LoadToMem();

    return true;
}

///
/// @brief
///    将m_markUpXml内容加载到内存的变量中
///
void CSmvRecvConfigPublic::_LoadToMem()
{
    std::wstring strResult;
    //获取采样值显示类型
    strResult = GetElemAttrVal(GROUP_NAME_BASIC_CONFIG, ELEM_NODE_SAMPLE_SHOWTYPE);
    if (!strResult.empty())
    {
        m_enSmvDispType = static_cast<enumSmvValueType>(_ttoi(strResult.c_str()));
    }

    //获取采样率
//     strResult = GetElemAttrVal(GROUP_NAME_BASIC_CONFIG, ELEM_NODE_SAMPLE_RATE);
//     if (!strResult.empty())
//     {
//         m_enSmv92SampleRate = static_cast<SMV92_SAMPLE_RATE>(_ttoi(strResult.c_str()));
//     }

    //获取核相对齐方式
    strResult = GetElemAttrVal(GROUP_NAME_PHASE_CHECK, ELEM_NODE_PHASE_CHECK_ALIGN);
    if (!strResult.empty())
    {
        m_enPhaseCheckAlign = static_cast<SMV_PHASE_CHECK_ALIGNMENT>(_ttoi(strResult.c_str()));
    }

    //获取互感器类型
    strResult = GetElemAttrVal(GROUP_NAME_POLARITY_CHECK, ELEM_NODE_POLARITY_TRANSFORMER_TYPE);
    if (!strResult.empty())
    {
        m_enTransFormerType = static_cast<TRANSFORMER_TYPE>(_ttoi(strResult.c_str()));
    }

    //获取门槛值
    strResult = GetElemAttrVal(GROUP_NAME_POLARITY_CHECK, ELEM_NODE_POLARITY_SILL_VALUE);
    if (!strResult.empty())
    {
        double fVal = 0.0;
        if (StrFloatConvert(strResult, NULL, &fVal))
        {
            m_fSillValue = static_cast<float>(fVal);
        }
        else
        {
            m_fSillValue = SILL_VALUE_DEFAULT;
        }
    }

    // 获取零漂值
    strResult = GetElemAttrVal(GROUP_NAME_POLARITY_CHECK, ELEM_NODE_POLARITY_NULL_SHIFT_VALUE);
    if (!strResult.empty())
    {
        double fVal = 0.0;
        if (StrFloatConvert(strResult, NULL, &fVal))
        {
            m_fNullShiftValue = static_cast<float>(fVal);
        }
        else
        {
            m_fSillValue = NULL_SHIFT_VALUE_DEFAULT;
        }
    }

    //获取不同次谐波是否需要显示
    int nSize = sizeof(ELEM_NODE_HARMONIC_TYPE)/sizeof(ELEM_NODE_HARMONIC_TYPE[0]);
    bool bResult = false;
    for (int i = 1; i <= nSize; ++i)
    {
        strResult = GetElemAttrVal(GROUP_NAME_HARMONIC_SHOW, ELEM_NODE_HARMONIC_TYPE[i - 1]);
        if (!strResult.compare(L"true"))
        {
            bResult = true;
        }
        else
        {
            bResult = false;
        }
        SetHarmonicTypeEnable(static_cast<HARMONIC_TYPE>(i), bResult);
    }
}
