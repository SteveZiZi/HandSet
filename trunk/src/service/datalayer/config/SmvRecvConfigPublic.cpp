/// @file
///
/// @brief
///     SMV�������ù������ò���
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.4.26
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "SmvRecvConfigPublic.h"
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\utility\data_utility.h"
#include "src\service\datalayer\DsmDataLayer.h"

//������뾯��todo��Ϣ
//#pragma message(__FILE__ "(" PRAGMA_STRING(__LINE__) "): ����Uninitʱ�Ƿ�Ҫ��ֹͣ����ӿ����ݽ���")
#define PRAGMA_STRING2(x) #x
#define PRAGMA_STRING(x) PRAGMA_STRING2(x)

// ����xml�ڵ���������
static std::wstring ELEM_NODE_ATTR_NAME       = L"value";                ///< ����ֵ��
static std::wstring GROUP_NODE_ATTR_NAME      = L"name";                 ///< ������������

//����xml��ͬ��������
static std::wstring GROUP_NAME_BASIC_CONFIG   =  L"SmvBaseConfig";       ///< ���ջ�����������
static std::wstring GROUP_NAME_HARMONIC_SHOW  =  L"HarmonicShow";        ///< г��������������
static std::wstring GROUP_NAME_POLARITY_CHECK =  L"PolarityCheck";       ///< ���Լ����������
static std::wstring GROUP_NAME_PHASE_CHECK    =  L"PhaseCheck";          ///< ������������

//����xml�нڵ�Ԫ�ص�����
static std::wstring ELEM_NODE_SAMPLE_SHOWTYPE           = L"SampleValueShowType";  ///< ����ֵ��ʾ����
static std::wstring ELEM_NODE_SAMPLE_RATE               = L"SampleRate";           ///< ������
static std::wstring ELEM_NODE_PHASE_CHECK_ALIGN         = L"PhaseCheckAlign";      ///< �������ݶ��뷽ʽ
static std::wstring ELEM_NODE_POLARITY_TRANSFORMER_TYPE = L"TransformerType";      ///< ����������
static std::wstring ELEM_NODE_POLARITY_SILL_VALUE       = L"SillValue";            ///< �ż�ֵ
static std::wstring ELEM_NODE_POLARITY_NULL_SHIFT_VALUE = L"NullShiftValue";       ///< ��Ưֵ


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

#define SILL_VALUE_DEFAULT          4.0f                                ///< �ż�ֵĬ��ֵ
#define NULL_SHIFT_VALUE_DEFAULT    3.0f                                ///< ��ƯֵĬ��ֵ

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
///    ���ý�����ʾ��������
///
/// @param[in]  enSmvDispType   Ҫ���õ���ʾ��������
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
///    ��ȡ������ʾ��������
///
/// @return
///     ���ص�ǰ���õĽ�����ʾ��������
///
enumSmvValueType CSmvRecvConfigPublic::GetSmvDispType()
{
    return m_enSmvDispType;
}

///
/// @brief
///    ���ò�������Ϣ
///
/// @param[in]  enSampleRate   Ҫ���õĲ�����
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
            //�����ò����ʺ�֪ͨdsp�޸�ʹ�õĲ�����
            uint32_t nSmapleRate = 4000;
            if (enSampleRate == SMV92_SAMPLE_RATE_AUTO_DETECT)
            {
                pCurrentCtrlBlk = pDataLayer->GetCurrentCtrlBlk();
                if (pCurrentCtrlBlk)
                {
                    nSmapleRate = pCurrentCtrlBlk->nSampleRateDetect;
                    //�޸�dsp��������Ϣ
                    pDataLayer->SetDspSampleRate(nSmapleRate);
                    //�޸ĺ���������
                    pDataLayer->GetSampleInterSync()->SetResampleConfig(DSM_CONFIG_DEFAULT_ELEC_FREQUENCY, nSmapleRate, nSmapleRate);
                }
            }
            else
            {
                if (enSampleRate == SMV92_SAMPLE_RATE_12P8K)
                {
                    nSmapleRate = 12800;
                }
                //�޸�dsp��������Ϣ
                pDataLayer->SetDspSampleRate(nSmapleRate);
                //�޸ĺ���������
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
///    ��ȡ9-2��������Ϣ
///
/// @return
///     ���ص�ǰ���õ�9-2��������Ϣ
///
SMV92_SAMPLE_RATE CSmvRecvConfigPublic::GetSmv92SampleRate()
{
    return m_enSmv92SampleRate;
}

///
/// @brief
///    ���ú���ʱ����ֵ���뷽ʽ
///
/// @param[in] enPhaseCheckAlign ָ�����뷽ʽ
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
///    ��ȡ�����ǲ���ֵ���뷽ʽ
///
/// @return
///     SMV_PHASE_CHECK_ALIGNMENT ���غ���ʱ���ݶ��뷽ʽ
///
SMV_PHASE_CHECK_ALIGNMENT CSmvRecvConfigPublic::GetPhaseCheckAlign()
{
    return m_enPhaseCheckAlign;
}

///
/// @brief
///    �����Ƿ���ʾĳ��г����Ϣ
///
/// @param[in]   enHarmonicType Ҫ���õ�г������
/// @param[in]   bEnable        �Ƿ���ʾ�����͵�г����Ϣ
///
/// @return
///     true��ʾ���óɹ�
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
///    ��ȡĳ��г�����Ƿ���ʾ���
///
/// @param[in]  enHarmonicType  Ҫ��ȡ��г������
///
/// @return
///     ����true��ʾ�����͵�г������ʾ
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
///    �������ļ��м�����Ϣ
///
/// @param[in]   strConfigPath  �����ļ�����(���·��)
///
/// @return
///     ���سɹ�����true
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
///    ���浱ǰ��Ϣ�������ļ�
///
/// @param[in]   strConfigPath  �����ļ�����(���·��)
///
/// @return
///     ����ɹ�����true
///
bool CSmvRecvConfigPublic::SaveToConfigFile()
{
    if (!m_bModified)
    {
        return true;
    }
    //�������ֵ��ʾ����
    if (!SetElemAttrVal(GROUP_NAME_BASIC_CONFIG, ELEM_NODE_SAMPLE_SHOWTYPE, static_cast<int>(m_enSmvDispType)))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T(" CSmvRecvConfigPublic::SaveToConfigFile: SetElemAttrVal(%s) Failed \n"),ELEM_NODE_SAMPLE_SHOWTYPE.c_str()));
    }
    //���������
//     if (!SetElemAttrVal(GROUP_NAME_BASIC_CONFIG, ELEM_NODE_SAMPLE_RATE, static_cast<int>(m_enSmv92SampleRate)))
//     {
//         RETAILMSG(OUTPUT_LOG_MSG, (_T(" CSmvRecvConfigPublic::SaveToConfigFile: SetElemAttrVal(%s) Failed \n"),ELEM_NODE_SAMPLE_RATE.c_str()));
//     }
    //����������
    if (!SetElemAttrVal(GROUP_NAME_PHASE_CHECK, ELEM_NODE_PHASE_CHECK_ALIGN, static_cast<int>(m_enPhaseCheckAlign)))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T(" CSmvRecvConfigPublic::SaveToConfigFile: SetElemAttrVal(%s) Failed \n"),ELEM_NODE_PHASE_CHECK_ALIGN.c_str()));
    }
    //���û���������
    if (!SetElemAttrVal(GROUP_NAME_POLARITY_CHECK, ELEM_NODE_POLARITY_TRANSFORMER_TYPE, static_cast<int>(m_enTransFormerType)))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (_T(" CSmvRecvConfigPublic::SaveToConfigFile: SetElemAttrVal(%s) Failed \n"),ELEM_NODE_POLARITY_TRANSFORMER_TYPE.c_str()));
    }
    //�����ż�ֵ
    std::wstring strResult;
    if (FloatStrConvert(m_fSillValue, strResult))
    {
        if (!SetElemAttrVal(GROUP_NAME_POLARITY_CHECK, ELEM_NODE_POLARITY_SILL_VALUE, strResult))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T(" CSmvRecvConfigPublic::SaveToConfigFile: SetElemAttrVal(%s) Failed \n"),ELEM_NODE_POLARITY_SILL_VALUE.c_str()));
        }
    }

    //������Ưֵ
    if (FloatStrConvert(m_fNullShiftValue, strResult))
    {
        if (!SetElemAttrVal(GROUP_NAME_POLARITY_CHECK, ELEM_NODE_POLARITY_NULL_SHIFT_VALUE, strResult))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (_T(" CSmvRecvConfigPublic::SaveToConfigFile: SetElemAttrVal(%s) Failed \n"),ELEM_NODE_POLARITY_NULL_SHIFT_VALUE.c_str()));
        }
    }

    //���ò�ͬ��г���Ƿ���Ҫ��ʾ
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
///    ��ȡ����ʵ���ӿ�
///
CSmvRecvConfigPublic* CSmvRecvConfigPublic::getInstance()
{ 
    static CSmvRecvConfigPublic s_SmvRecvConfigPub;
    return &s_SmvRecvConfigPub;
}

///
/// @brief
///    ��ȡָ���ڵ����������
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
///     ����ָ���ڵ����������
///
/// @param[in]  strElement Ҫ�������ݵ�Ԫ������
/// @param[in]  strAttrValue Ҫ���õ�����ֵ
///
/// @return
///     bool    true��ʾ���óɹ�
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
            else //��ǰ��δ�ҵ���Ӧ��node
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
        //δ�ҵ�group�����group�����ʵ�ʽڵ�
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
///     ����ָ���ڵ����������
///
/// @param[in]  strElement Ҫ�������ݵ�Ԫ������
/// @param[in]  nAttrValue Ҫ���õ�����ֵ
///
/// @return
///     bool    true��ʾ���óɹ�
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
            else //��ǰ��δ�ҵ���Ӧ��node
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
        //δ�ҵ�group�����group�����ʵ�ʽڵ�
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
///    �ָ�Ĭ������
///
bool CSmvRecvConfigPublic::Reset()
{
    // ��ȡ���������ļ�·��
    std::wstring strBackup, strRun;
    strBackup = GetAppDir() + DEFAULT_CFG_BACKUP + m_strCfgFile;
    strRun    = GetAppRunTimeCfgDir() + m_strCfgFile;
    // �������ݵ������ļ�
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
///    ��m_markUpXml���ݼ��ص��ڴ�ı�����
///
void CSmvRecvConfigPublic::_LoadToMem()
{
    std::wstring strResult;
    //��ȡ����ֵ��ʾ����
    strResult = GetElemAttrVal(GROUP_NAME_BASIC_CONFIG, ELEM_NODE_SAMPLE_SHOWTYPE);
    if (!strResult.empty())
    {
        m_enSmvDispType = static_cast<enumSmvValueType>(_ttoi(strResult.c_str()));
    }

    //��ȡ������
//     strResult = GetElemAttrVal(GROUP_NAME_BASIC_CONFIG, ELEM_NODE_SAMPLE_RATE);
//     if (!strResult.empty())
//     {
//         m_enSmv92SampleRate = static_cast<SMV92_SAMPLE_RATE>(_ttoi(strResult.c_str()));
//     }

    //��ȡ������뷽ʽ
    strResult = GetElemAttrVal(GROUP_NAME_PHASE_CHECK, ELEM_NODE_PHASE_CHECK_ALIGN);
    if (!strResult.empty())
    {
        m_enPhaseCheckAlign = static_cast<SMV_PHASE_CHECK_ALIGNMENT>(_ttoi(strResult.c_str()));
    }

    //��ȡ����������
    strResult = GetElemAttrVal(GROUP_NAME_POLARITY_CHECK, ELEM_NODE_POLARITY_TRANSFORMER_TYPE);
    if (!strResult.empty())
    {
        m_enTransFormerType = static_cast<TRANSFORMER_TYPE>(_ttoi(strResult.c_str()));
    }

    //��ȡ�ż�ֵ
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

    // ��ȡ��Ưֵ
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

    //��ȡ��ͬ��г���Ƿ���Ҫ��ʾ
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
