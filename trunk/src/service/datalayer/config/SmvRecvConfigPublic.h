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

#pragma once

#include "src/utils/cmarkup/markup.h"
#include <string>
#include <map>
#include "src\service\datalayer\include\datatypes.h"

///
/// @brief
///    ����9-2���ĵĲ�������Ϣ��������û�в���ֵ����Ҫ����
///
enum SMV92_SAMPLE_RATE
{
    SMV92_SAMPLE_RATE_4K = 1,                                           ///< ����Ƶ��4k
    SMV92_SAMPLE_RATE_12P8K,                                            ///< ����Ƶ��12.8k
    SMV92_SAMPLE_RATE_AUTO_DETECT                                       ///< 9-2�������Զ�̽��
};

///
/// @brief
///    ָ������ʱ���ݶ��뷽ʽ
///
enum SMV_PHASE_CHECK_ALIGNMENT
{
    SMV_PHASE_CHECK_ALIGN_TIME = 1,                                     ///< ����ֵ����ʱ�����
    SMV_PHASE_CHECK_ALIGN_INDEX                                         ///< ����ֵ������Ŷ���
};

///
/// @brief
///    ����г����Ϣ�ı�ʶ
///
enum HARMONIC_TYPE
{
    HARMONIC_TYPE_BASE_WAVE = 1,                                        ///< ������һ��г��
    HARMONIC_TYPE_2,                                                    ///< 2��г��
    HARMONIC_TYPE_3,                                                    ///< 3��г��
    HARMONIC_TYPE_4,                                                    ///< 4��г��
    HARMONIC_TYPE_5,                                                    ///< 5��г��
    HARMONIC_TYPE_6,                                                    ///< 6��г��
    HARMONIC_TYPE_7,                                                    ///< 7��г��
    HARMONIC_TYPE_8,                                                    ///< 8��г��
    HARMONIC_TYPE_9,                                                    ///< 9��г��
    HARMONIC_TYPE_10,                                                   ///< 10��г��
    HARMONIC_TYPE_11,                                                   ///< 11��г��
    HARMONIC_TYPE_12,                                                   ///< 12��г��
    HARMONIC_TYPE_13,                                                   ///< 13��г��
    HARMONIC_TYPE_14,                                                   ///< 14��г��
    HARMONIC_TYPE_15,                                                   ///< 15��г��
    HARMONIC_TYPE_16,                                                   ///< 16��г��
    HARMONIC_TYPE_17,                                                   ///< 17��г��
    HARMONIC_TYPE_18,                                                   ///< 18��г��
    HARMONIC_TYPE_19,                                                   ///< 19��г��
    HARMONIC_TYPE_20,                                                   ///< 20��г��
    HARMONIC_TYPE_21,                                                   ///< 21��г��
    HARMONIC_TYPE_THD,                                                  ///< г��������
    HARMONIC_TYPE_DC                                                    ///< ֱ������
};

///
/// @brief
///    ����У�黥��������
///
enum TRANSFORMER_TYPE
{
    TRANSFORMER_TYPE_LEGACY = 1,                                        ///< ��ʽ������
    TRANSFORMER_TYPE_ELECTRONIC,                                        ///< ���ӻ�����
    TRANSFORMER_TYPE_OTHER                                              ///< �������ͻ�����
};


class CSmvRecvConfigPublic
{
private:
    CSmvRecvConfigPublic(void);
    ~CSmvRecvConfigPublic(void);


public:
    ///
    /// @brief
    ///    ��ȡ����ʵ���ӿ�
    ///
    static CSmvRecvConfigPublic* getInstance();

//
//=============================================================================
// ��������/��ȡ�ӿ�
//=============================================================================
//
    ///
    /// @brief
    ///    ���ý�����ʾ��������
    ///
    /// @param[in]  enSmvDispType   Ҫ���õ���ʾ��������
    ///
    void SetSmvDispType(enumSmvValueType enSmvDispType);

    ///
    /// @brief
    ///    ��ȡ������ʾ��������
    ///
    /// @return
    ///     ���ص�ǰ���õĽ�����ʾ��������
    ///
    enumSmvValueType GetSmvDispType();

    ///
    /// @brief
    ///    ���ò�������Ϣ
    ///
    /// @param[in]  enSampleRate   Ҫ���õĲ�����
    ///
    void SetSmv92SampleRate(SMV92_SAMPLE_RATE enSampleRate, bool bUpdateSelectCtrlBlk = true);


    ///
    /// @brief
    ///    ��ȡ9-2��������Ϣ
    ///
    /// @return
    ///     ���ص�ǰ���õ�9-2��������Ϣ
    ///
    SMV92_SAMPLE_RATE GetSmv92SampleRate();

//
//=============================================================================
// г����Ϣ����/��ȡ
//=============================================================================
//
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
    bool SetHarmonicTypeEnable(HARMONIC_TYPE enHarmonicType, bool bEnable);

    ///
    /// @brief
    ///    ��ȡĳ��г�����Ƿ���ʾ���
    ///
    /// @param[in]  enHarmonicType  Ҫ��ȡ��г������
    ///
    /// @return
    ///     ����true��ʾ�����͵�г������ʾ
    ///
    bool GetHarmonicTypeEnabled(HARMONIC_TYPE enHarmonicType);

//
//=============================================================================
// ����ҵ������/��ȡ�ӿ�
//=============================================================================
//
    ///
    /// @brief
    ///    ���ú���ʱ����ֵ���뷽ʽ
    ///
    /// @param[in] enPhaseCheckAlign ָ�����뷽ʽ
    ///
    void SetPhaseCheckAlign(SMV_PHASE_CHECK_ALIGNMENT enPhaseCheckAlign);

    ///
    /// @brief
    ///    ��ȡ�����ǲ���ֵ���뷽ʽ
    ///
    /// @return
    ///     SMV_PHASE_CHECK_ALIGNMENT ���غ���ʱ���ݶ��뷽ʽ
    ///
    SMV_PHASE_CHECK_ALIGNMENT GetPhaseCheckAlign();

//
//=============================================================================
// ����ҵ������/��ȡ�ӿ�
//=============================================================================
//
    ///
    /// @brief
    ///    ���û���������
    ///
    void SetTransformerType(TRANSFORMER_TYPE enTransformerType)
    {
        if (enTransformerType != m_enTransFormerType)
        {
            m_enTransFormerType = enTransformerType;
            m_bModified = true;
        }
    }

    ///
    /// @brief
    ///    ��ȡ����������
    ///
    TRANSFORMER_TYPE GetTransformerType()
    {
        return m_enTransFormerType;
    }

    ///
    /// @brief
    ///    �����ż�ֵ
    ///
    void SetSillValue(float fSillValue)
    {
        m_fSillValue = fSillValue;
        m_bModified = true;
    }

    ///
    /// @brief
    ///    ��ȡ�ż�ֵ
    ///
    float GetSillValue()
    {
        return m_fSillValue;
    }

    ///
    /// @brief
    ///    ������Ưֵ
    ///
    void SetNullShiftValue(float fNullShiftValue)
    {
        m_fNullShiftValue = fNullShiftValue;
        m_bModified = true;
    }

    ///
    /// @brief
    ///    ��ȡ��Ưֵ
    ///
    float GetNullShiftValue()
    {
        return m_fNullShiftValue;
    }

//
//=============================================================================
// ���ñ���/����
//=============================================================================

    ///
    /// @brief
    ///    ��ȡָ���ڵ����������
    ///
    std::wstring GetElemAttrVal(const std::wstring& strGroup, const std::wstring& strElement);

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
    bool SetElemAttrVal(const std::wstring& strGroup, const std::wstring& strElement, const std::wstring& strAttrValue);

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
    bool SetElemAttrVal(const std::wstring& strGroup, const std::wstring& strElement, int nAttrValue);

    ///
    /// @brief
    ///    �������ļ��м�����Ϣ
    ///
    /// @param[in]   strConfigPath  �����ļ�����(���·��)
    ///
    /// @return
    ///     ���سɹ�����true
    ///
    bool LoadConfigFile(std::wstring strConfigFile);

    ///
    /// @brief
    ///    ���浱ǰ��Ϣ�������ļ�
    ///
    /// @param[in]   strConfigPath  �����ļ�����(���·��)
    ///
    /// @return
    ///     ����ɹ�����true
    ///
    bool SaveToConfigFile();

    ///
    /// @brief
    ///    �ָ�Ĭ������
    ///
    bool Reset();

    private:

    ///
    /// @brief
    ///    ��m_markUpXml���ݼ��ص��ڴ�ı�����
    ///
    void _LoadToMem();

private:
    bool                                        m_bModified;            ///< ���������Ƿ��޸�
    std::wstring                                m_strCfgFile;           ///< �����ļ�·��
    CMarkup                                     m_markUpXml;            ///< CMarkup����

    enumSmvValueType                            m_enSmvDispType;        ///< ��Ŀ��ʾ���ݵ���ֵ����
    SMV92_SAMPLE_RATE                           m_enSmv92SampleRate;    ///< 9-2��������Ϣ
    SMV_PHASE_CHECK_ALIGNMENT                   m_enPhaseCheckAlign;    ///< ָ������ʱ����ֵ���뷽ʽ
    std::map<HARMONIC_TYPE, bool>               m_mapHarmonicType;      ///< ���治ͬ���͵�г���Ƿ���Ҫ��ʾ
    TRANSFORMER_TYPE                            m_enTransFormerType;    ///< ����У��ʹ�õĻ���������
    float                                       m_fSillValue;           ///< ����У�����õ��ż�ֵ
    float                                       m_fNullShiftValue;      ///< ����У�����õ���Ưֵ
};
