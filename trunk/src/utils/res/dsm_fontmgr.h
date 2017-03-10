/**
 * \file
 *      dsm_fontmgr.h
 *
 * \brief
 *      ������Դ������ͷ�ļ�
 *
 *  \note
 *      �ù������ڵ������������nHeight���������Ծ�һ��
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/6/14
 */


#ifndef DSM_FONTMGR_H__
#define DSM_FONTMGR_H__



/**
 *      ���建�������
 *
 *  \note
 *      �����ڲ�ʹ�õĻ�������
 */
class dsm_fontmgr
{
public:

    /**
     *      ����������ʽ
     */
    void SetStyle(int nWidth, int nEscapement, int nOrientation, int nWeight, BYTE bItalic, BYTE bUnderline, BYTE cStrikeOut, BYTE nCharSet, BYTE nOutPrecision, BYTE nClipPrecision, BYTE nQuality, BYTE nPitchAndFamily, LPCTSTR lpszFacename);

    /**
     *      ���ݸ߶Ȼ�ȡָ������
     */
    CFont* GetFont(int nHeight);

    /**
     *      �ͷ���Դ
     */
    void Clear();


    /**
     *      ��ȡʵ������ָ��
     */
    static dsm_fontmgr* GetInstance();


private:
    dsm_fontmgr();

    virtual ~dsm_fontmgr();

private:
    typedef CMap<int ,int, CFont*, CFont*>          CMapFont;

    CMapFont                m_mapFont;              ///< ����ӳ���


    int m_nWidth;
    int m_nEscapement;
    int m_nOrientation;
    int m_nWeight;
    BYTE m_bItalic;
    BYTE m_bUnderline;
    BYTE m_cStrikeOut;
    BYTE m_nCharSet;
    BYTE m_nOutPrecision;
    BYTE m_nClipPrecision;
    BYTE m_nQuality;
    BYTE m_nPitchAndFamily;
    CString m_csFacename;
};


#endif // DSM_FONTMGR_H__






