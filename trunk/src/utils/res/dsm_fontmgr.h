/**
 * \file
 *      dsm_fontmgr.h
 *
 * \brief
 *      字体资源管理器头文件
 *
 *  \note
 *      该管理器内的字体除了字体nHeight，其他属性均一致
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/6/14
 */


#ifndef DSM_FONTMGR_H__
#define DSM_FONTMGR_H__



/**
 *      字体缓存管理器
 *
 *  \note
 *      程序内部使用的缓存字体
 */
class dsm_fontmgr
{
public:

    /**
     *      设置字体样式
     */
    void SetStyle(int nWidth, int nEscapement, int nOrientation, int nWeight, BYTE bItalic, BYTE bUnderline, BYTE cStrikeOut, BYTE nCharSet, BYTE nOutPrecision, BYTE nClipPrecision, BYTE nQuality, BYTE nPitchAndFamily, LPCTSTR lpszFacename);

    /**
     *      根据高度获取指定字体
     */
    CFont* GetFont(int nHeight);

    /**
     *      释放资源
     */
    void Clear();


    /**
     *      获取实例对象指针
     */
    static dsm_fontmgr* GetInstance();


private:
    dsm_fontmgr();

    virtual ~dsm_fontmgr();

private:
    typedef CMap<int ,int, CFont*, CFont*>          CMapFont;

    CMapFont                m_mapFont;              ///< 字体映射表


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






