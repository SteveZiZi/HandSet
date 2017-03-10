#pragma once
#include <vector>
#include <imaging.h>

//
// ����
#define SLIDERBOX_CLASSNAME         L"SliderBox"
// ����
#define SB_DEBUGLOG                 NKDbgPrintfW

// ��СͼƬ����ʱ��
#define SB_MIN_SCROLL_TIMEOUT       100
// ��󶯻�Ƶ�ʣ�Hz��
#define SB_MAX_ANIMATE_FREQ         1000
// CSliderBox

class CSliderBox : public CWnd
{
	DECLARE_DYNAMIC(CSliderBox)

public:
#if !SHENG_ZE
	friend class CBasicAboutDlg;
#endif
    //======================================================================
    // ��������
    //====================================================================== 
	CSliderBox();    
	virtual ~CSliderBox();
    virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
        UINT nID, CCreateContext* pContext = NULL);
    virtual BOOL CreateEx(DWORD dwExStyle, DWORD dwStyle, const RECT& rect, 
        CWnd* pParentWnd, UINT nID, LPVOID lpParam = NULL);    

    //======================================================================
    // ��������
    //====================================================================== 
    ///
    /// @method:    LoadImages
    /// @access:    public 
    /// @brief:
    ///		����ͼƬ
    /// @param[in]: 
    ///		szImgDir						Ҫ����ͼƬ���ļ���
    /// @return:   
    ///		BOOL					        TRUE���ɹ�
    ///                                     FALSE��ָ��Ŀ¼�����ڻ�Ŀ¼����ͼƬ
    ///
    BOOL LoadImages(const CString &szImgDir);

    ///
    /// @method:    SetScrollTimeout
    /// @access:    public 
    /// @brief:
    ///		����һ�λ�����ʱ��
    /// @param[in]: 
    ///		nTimeout						ʱ�䣨���룩
    /// @return:   
    ///		BOOL					        TRUE���ɹ�
    ///                                     FALSE��С����С����ʱ�䣨SB_MIN_SCROLL_TIMEOUT��
    ///
    BOOL SetScrollTimeout(UINT nTimeout);
    
    ///
    /// @method:    SetAnimateFreq
    /// @access:    public 
    /// @brief:
    ///		���ö���Ƶ��
    /// @note
    ///     ����Ƶ�ʼ�һ����ҳ��ˢ�´�����FPS��������Ƶ��Ӱ�춯����ƽ���̶ȣ�Ƶ
    ///     ��Խ�ߣ�ͼ��Խƽ��
    /// @param[in]: 
    ///		nFreq						Ƶ��ֵ
    /// @return:
    ///		BOOL					    TRUE���ɹ�
    ///                                 FALSE���������Ƶ��ֵ��SB_MAX_ANIMATE_FREQ��
    ///
    BOOL SetAnimateFreq(UINT nFreq);


    ///
    /// @method:    GetImgIndex
    /// @access:    public 
    /// @brief:
    ///		ȡ�õ�ǰͼ��������ָ��Ŀ¼�еĵڼ���ͼƬ��    
    /// @return:   
    ///		UINT					    ͼ������
    ///
    inline UINT GetImgIndex();
protected:    
    ///
    /// @method:    ScrollLeft
    /// @access:    virtual protected 
    /// @brief:
    ///		ͼƬ���󻬶�
    /// @param[in]: 
    ///		nTimeout						����ʱ��
    /// @return:   
    ///		BOOL					        TRUE���ɹ�
    ///                                     FALSE�������ǰ�ǵ�һ��ͼƬ
    ///
    virtual BOOL ScrollLeft(UINT nTimeout);
    ///
    /// @method:    ScrollLeft
    /// @access:    virtual protected 
    /// @brief:
    ///		ͼƬ���һ���
    /// @param[in]: 
    ///		nTimeout						����ʱ��
    /// @return:   
    ///		BOOL					        TRUE���ɹ�
    ///                                     FALSE�������ǰ�����һ��ͼƬ
    ///
    virtual BOOL ScrollRight(UINT nTimeout);  

    //======================================================================
    // ��Ϣ����
    //======================================================================
	DECLARE_MESSAGE_MAP()    
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    enum TIMER_ID{TIMER_SCROLL_LEFT=10000, TIMER_SCROLL_RIGHT};
    afx_msg void OnTimer(UINT_PTR nIDEvent); 
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    
    //======================================================================
    // ����
    //======================================================================    
    static BOOL  RegisterWndClass (HINSTANCE hInstance, LPCTSTR lpszClassName,
        UINT style, HICON hIcon = 0, int bgStockBrush = NULL_BRUSH);

    //======================================================================
    // ��ͼ
    //======================================================================    
    ///
    /// @method:    DrawImgsToMem
    /// @access:    protected 
    /// @brief:
    ///		��ǰһ�ţ���ǰ����һ��ͼƬ�����ڴ�DC������
    /// @param[in]
    ///     mode                    ����ģʽ�����ڿ����Ժ��ַ�ʽ����ͼƬ����ʼ��
    ///                             ģʽ����ֱ�Ӵ��ļ�����3��ͼƬ�����󻬶�������
    ///                             ����ģʽͨ��������������뻬�������������ͼƬ
    ///                             ���Ż������ٶ�
    ///                             DM_INIT:��ʼ��ģʽ
    ///                             DM_MOVE_LEFT:�����ƶ�ģʽ
    ///                             DM_MOVE_RIGHT:�����ƶ�ģʽ    
    /// @return:   
    ///		BOOL					TURE���ɹ�
    ///                             FALSE��ͼƬ�������
    ///
    enum DrawMode{DM_INIT, DM_MOVE_LEFT, DM_MOVE_RIGHT};
    BOOL   DrawImgsToMem(DrawMode mode = DM_INIT);


    ///
    /// @method:    DrawArrow
    /// @access:    protected 
    /// @brief:
    ///		����ͷ
    /// @param[in]: 
    ///		rect						��Χ��ͷ�ľ��ο��С
    ///		type						��ͷ���ͣ���������
    ///     nWidth                      ��ͷ������ϸ
    /// @return:   
    ///		VOID					
    ///
#if SHENG_ZE
    enum ARROW_TYPE{ARROW_UP, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT};    
    VOID DrawArrow(CDC *pDC, ARROW_TYPE type, POINT ptPeak, UINT nIncludedAngle, UINT nLength, UINT nWidth = 3);
#endif

    ///
    /// @method:    DrawIndicators
    /// @access:    protected 
    /// @brief:
    ///		��ͼƬָʾ��Բ��
    /// @param[in]: 
    ///		rect						��Χָʾ���ľ��δ�С
    ///		nRadius						Բ��İ뾶
    /// @return:   
    ///		BOOL					    TRUE���ɹ�
    ///                                 FALSE���뾶̫�����ͼƬ̫�໭����
    ///
    BOOL DrawIndicators(CDC *pDC, RECT rect, UINT nRadius);
protected:
    UINT                        m_nImgIndex;        ///< ��ǰͼ������
    CString                     m_szImgDir;         ///< ��ǰͼ���ļ���
    ///< �洢����ͼ���ļ�����������ֻ֧��bmpͼ��
    std::vector<CString>        m_vImgFileName;
    CDC                         m_memDC;            ///< �ڴ�DC���������ͼ��    
    UINT                        m_nAnimateFreq;     ///< ����Ƶ�ʣ�Hz��
    BOOL                        m_fScrolling;       ///< ָʾ�Ƿ����ڻ���
    UINT                        m_nScrollTimeout;   ///< һ�λ���ʱ��
private:
    inline CString GetFileTitle(const CString& szFileName);
    inline CString GetFileExt(const CString& szFileName);
    HBITMAP LoadImageFromFile(const TCHAR * pFileImage);
private:
    INT                         m_nScrollPixel;    
    HBITMAP                     m_hBitmap[3];
    CBitmap                     m_bitmap;
    CRect                       m_rtMapMem;
    IImagingFactory             *m_pImgFactory;
};
inline UINT CSliderBox::GetImgIndex()
{
    return m_nImgIndex;    
}

inline CString CSliderBox::GetFileTitle( const CString& szFileName )
{
    CString szFileTitle;    
    int nExtTokenIndex;
    int nFileTitleLength;     
    nExtTokenIndex = szFileName.ReverseFind('.');
    nFileTitleLength = nExtTokenIndex;    
    szFileTitle = szFileName.Left(nFileTitleLength);
    return szFileTitle;
}

inline CString CSliderBox::GetFileExt( const CString& szFileName )
{
    CString szFileExt;
    int nExtTokenIndex;
    int nFileTitleLength;
    int nFileExtLength; 
    nExtTokenIndex = szFileName.ReverseFind('.');
    nFileTitleLength = nExtTokenIndex;
    nFileExtLength = szFileName.GetLength() - nFileTitleLength - 1;
    szFileExt = szFileName.Right(nFileExtLength);
    return szFileExt;
}
