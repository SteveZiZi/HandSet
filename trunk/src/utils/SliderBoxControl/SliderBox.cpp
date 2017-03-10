// SliderBox.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include <InitGuid.h>
#include "SliderBox.h"
#include <Shellapi.h>
#include <vector>
#include <algorithm>
#include <cmath>


// CSliderBox

IMPLEMENT_DYNAMIC(CSliderBox, CWnd)

CSliderBox::CSliderBox()
:m_nImgIndex(0)
,m_nAnimateFreq(12)
,m_nScrollTimeout(350)
,m_fScrolling(FALSE)
,m_pImgFactory(NULL)
{
    for(int i=0; i<3; i++)
    {
        m_hBitmap[i] = NULL;
    }
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    CoCreateInstance (CLSID_ImagingFactory, NULL,
        CLSCTX_INPROC_SERVER, IID_IImagingFactory, (void **)&m_pImgFactory);
    RegisterWndClass(NULL, SLIDERBOX_CLASSNAME, 0);
}

CSliderBox::~CSliderBox()
{
    SB_DEBUGLOG(L"IESIMPLE Exiting ~CWebCtrl\r\n");
    m_pImgFactory->Release();
    for(int i=0; i<3; i++)
    {
        DeleteObject(m_hBitmap[i]);
        m_hBitmap[i] = NULL;
    }
    CoUninitialize();
}

BOOL CSliderBox::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
                        UINT nID, CCreateContext* pContext /*= NULL*/)
{
    BOOL ret = CreateEx(0, dwStyle, rect, pParentWnd, nID, pContext);
    return ret;
}

BOOL CSliderBox::CreateEx( DWORD dwExStyle, DWORD dwStyle, const RECT& rect, 
                          CWnd* pParentWnd, UINT nID, LPVOID lpParam /*= NULL*/ )
{
    BOOL ret = CWnd::CreateEx(dwExStyle, SLIDERBOX_CLASSNAME, L"", dwStyle, rect, 
        pParentWnd, nID, lpParam);

    // �����ڴ�DC
    CDC *pDC = GetDC();
    m_memDC.CreateCompatibleDC(pDC);  
    m_bitmap.CreateCompatibleBitmap(pDC, (rect.right-rect.left)*3, 
        rect.bottom-rect.top);
    m_memDC.SelectObject(&m_bitmap);
    ReleaseDC(pDC);

    // ��ʼ���ڴ�DCӳ����Σ����м��Ƿ�ͼ��λ�ã�
    GetClientRect(&m_rtMapMem);
    m_rtMapMem.MoveToX(m_rtMapMem.Width());

    // ����ÿ��ˢ���ƶ�������
    m_nScrollPixel = (rect.right-rect.left) / 
        (m_nScrollTimeout* m_nAnimateFreq / 1000);
    
    return TRUE;
}

BEGIN_MESSAGE_MAP(CSliderBox, CWnd)
    ON_WM_KEYDOWN()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_TIMER()    
END_MESSAGE_MAP()

// CSliderBox ��Ϣ�������

//
//=============================================================================
// ����
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// ע�ᴰ����
//-----------------------------------------------------------------------------
//
BOOL  CSliderBox::RegisterWndClass (HINSTANCE hInstance, LPCTSTR lpszClassName, UINT style, HICON hIcon, int bgStockBrush)
{
    WNDCLASS  wlsold = {0};
    if (::GetClassInfo(hInstance, lpszClassName, &wlsold))
    {
        // class�Ѿ�����
        return TRUE;
    }

    SB_DEBUGLOG(L"CHeaderBoxEL::RegisterWndClass\n");

    WNDCLASS wndcls;
    ZeroMemory(&wndcls, sizeof(wndcls));

    wndcls.style = style;
    wndcls.lpfnWndProc = ::DefWindowProc;
    wndcls.hInstance = hInstance ? hInstance : AfxGetInstanceHandle();
    wndcls.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
    wndcls.lpszClassName = lpszClassName;
    wndcls.hIcon = hIcon;
    wndcls.hbrBackground = (HBRUSH)::GetStockObject(bgStockBrush);

    return AfxRegisterClass(&wndcls);
}

BOOL CSliderBox::LoadImages( const CString &szImgDir)
{
    // ��ʽ��Ŀ¼��ʹ·��ĩβͳһ�з�б��
    m_szImgDir = szImgDir;
    if(m_szImgDir.Right(1)!=L"\\")
    {
        m_szImgDir+=L"\\";
    }

    // ����Ŀ¼�ļ�
    CString szFindFiles = m_szImgDir+L"*.*";
    WIN32_FIND_DATA fd;
    HANDLE hFind;
    hFind = FindFirstFile(szFindFiles, &fd);
    if(hFind == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    // ��ȡĿ¼�е�ͼƬ����Map[�ļ���]=�ļ�·���洢
    CString szFileName;
    CString szFileTitle;
    CString szFileExt;   
    do{            
        if(fd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)  
        {     
            szFileName = fd.cFileName;
            szFileTitle = this->GetFileTitle(szFileName);
            szFileExt = this->GetFileExt(szFileName);
            if(szFileExt.MakeLower() == "bmp" || szFileExt.MakeLower() == "png"
                || szFileExt.MakeLower() == "jpg")
            {
                m_vImgFileName.push_back(szFileName);
            }
        }
    }while(FindNextFile(hFind, &fd));

    // ���û�ҵ��κ�ͼƬ����FALSE
    if(m_vImgFileName.empty())
    {
        FindClose(hFind);
        return FALSE;
    }

    // ���ļ�������
    std::sort(m_vImgFileName.begin(), m_vImgFileName.end());

    DrawImgsToMem();
    Invalidate(FALSE);

    FindClose(hFind);
    return TRUE;
}

BOOL CSliderBox::ScrollLeft( UINT nTimeout )
{
    if(m_vImgFileName.size() <=0 ||m_nImgIndex == 0 || m_nImgIndex > 
        m_vImgFileName.size()-1)
    {
        return FALSE;
    }

    // ���ݶ���Ƶ������ÿ���ƶ�����
    CRect rect;
    GetClientRect(&rect);    
    m_nScrollPixel = rect.Width()/(nTimeout * m_nAnimateFreq / 1000);

    // ���ö�ʱ��
    if(!m_fScrolling)
    {
        m_fScrolling = TRUE;
        // ����ͼ������
        if(m_nImgIndex-1 >= 0)
        {
            m_nImgIndex--;
        } 
        SetTimer(TIMER_SCROLL_LEFT, nTimeout/m_nAnimateFreq, NULL);
    }
    return TRUE;
}

BOOL CSliderBox::ScrollRight( UINT nTimeout )
{
    if(m_vImgFileName.size() <= 0 || m_nImgIndex >= m_vImgFileName.size()-1)
    {
        return FALSE;
    }

    // ���ݶ���Ƶ������ÿ���ƶ�����
    CRect rect;
    GetClientRect(&rect);    
    m_nScrollPixel = rect.Width()/(nTimeout * m_nAnimateFreq / 1000);

    // ���ö�ʱ��
    if(!m_fScrolling)
    {
        m_fScrolling = TRUE;

        // ����ͼ������
        if(m_nImgIndex + 1 <= m_vImgFileName.size()-1)
        {
            m_nImgIndex++;
        }

        // ��ʼ����
        SetTimer(TIMER_SCROLL_RIGHT, nTimeout/m_nAnimateFreq, NULL);
    }    
    return TRUE;
}

void CSliderBox::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: �ڴ˴������Ϣ����������
    // ��Ϊ��ͼ��Ϣ���� CWnd::OnPaint()

    // ������
    CRect rect;
    GetClientRect(&rect);    
    if(m_vImgFileName.size() <= 0)
    {
        dc.FillSolidRect(rect, RGB(100,100,100));
        return;
    }
    else
    {
        // ��ͼ��
        dc.BitBlt(0, 0, rect.Width(), rect.Height(), &m_memDC, 
            m_rtMapMem.left, m_rtMapMem.top, SRCCOPY);
    }
#if SHENG_ZE
    // �����Ҽ�ͷ
    UINT nArrowMargin= 20;
    UINT nArrowIncludedAngle = 90;
    UINT nArrowLength = 25;
    UINT nArrowWidth =7;    

    // ���ͷ
    if(0 < m_nImgIndex)
    {
        CPoint ptLAPeak;
        ptLAPeak.x= rect.left + nArrowMargin;
        ptLAPeak.y = (rect.top+rect.bottom)/2;
        DrawArrow(&dc, ARROW_LEFT, ptLAPeak, nArrowIncludedAngle, nArrowLength, 
            nArrowWidth);
    }    

    // �Ҽ�ͷ
    if(m_vImgFileName.size()-1 > m_nImgIndex)
    {
        CPoint ptRAPeak;    
        ptRAPeak.x= rect.right - nArrowMargin - nArrowWidth;
        ptRAPeak.y= (rect.top+rect.bottom)/2;    
        DrawArrow(&dc, ARROW_RIGHT, ptRAPeak, nArrowIncludedAngle, nArrowLength,
            nArrowWidth);
    }    
#endif
    // ��ָʾ����һ��ͼƬʱ����ʾ��
    if(m_vImgFileName.size() > 1)
    {
#if SHENG_ZE
        UINT nIndicatorMargin = 20;
#else
		UINT nIndicatorMargin = 10;
#endif
        UINT nIndicatorWidth = rect.Width() - nIndicatorMargin*2;
        nIndicatorWidth = min(nIndicatorWidth, m_vImgFileName.size()*20);
#if SHENG_ZE
        UINT nIndicatorHeight = 20;
#else
		UINT nIndicatorHeight = 10;
#endif
        UINT nIndicatorRadius = 5;
        CPoint pIndicatorTL;
        pIndicatorTL.x = (rect.left + rect.right)/2 - nIndicatorWidth/2;
        pIndicatorTL.y = (rect.bottom - nIndicatorMargin - nIndicatorHeight);
        CSize sIndicatorSize(nIndicatorWidth, nIndicatorHeight);
        CRect rtIndicator(pIndicatorTL, sIndicatorSize);
        DrawIndicators(&dc, rtIndicator, nIndicatorRadius);   
    }
}

BOOL CSliderBox::DrawImgsToMem(DrawMode mode)
{
    CRect rect;    
    CDC tmpDC;
    //    HBITMAP hBitmap;
    CDC *pDC = GetDC();

    this->GetClientRect(&rect);
    tmpDC.CreateCompatibleDC(pDC);

    if(m_nImgIndex < 0 || m_nImgIndex > m_vImgFileName.size()-1)
    {
        ReleaseDC(pDC);
        return FALSE;
    }

    CString strFilePath;
    switch(mode)
    {
    case DM_MOVE_LEFT:
        DeleteObject(m_hBitmap[0]);       
        m_hBitmap[0] = m_hBitmap[1];
        m_hBitmap[1] = m_hBitmap[2];
        tmpDC.SelectObject(m_hBitmap[0]);
        m_memDC.BitBlt(0, 0, rect.Width(), rect.Height(), 
            &tmpDC, 0, 0, SRCCOPY);
        tmpDC.SelectObject(m_hBitmap[1]);
        m_memDC.BitBlt(rect.Width(), 0, rect.Width(), rect.Height(), 
            &tmpDC, 0, 0, SRCCOPY);
        //�����һ��λͼ
        if(m_nImgIndex < m_vImgFileName.size()-1)
        {
            strFilePath = m_szImgDir+m_vImgFileName[m_nImgIndex+1];
            m_hBitmap[2] = LoadImageFromFile(strFilePath);
            tmpDC.SelectObject(m_hBitmap[2]);
            m_memDC.BitBlt(2*rect.Width(), 0, rect.Width(), rect.Height(), 
                &tmpDC, 0, 0, SRCCOPY);
            //SB_DEBUGLOG(L"image file:%s\n",m_vImgFileName[m_nImgIndex+1]);
        }
        else
        {
            m_hBitmap[2] = NULL;
        }
        break;
    case DM_MOVE_RIGHT:
        DeleteObject(m_hBitmap[2]);
        m_hBitmap[2] = m_hBitmap[1];
        m_hBitmap[1] = m_hBitmap[0];
        tmpDC.SelectObject(m_hBitmap[1]);
        m_memDC.BitBlt(rect.Width(), 0, rect.Width(), rect.Height(), 
            &tmpDC, 0, 0, SRCCOPY);
        tmpDC.SelectObject(m_hBitmap[2]);
        m_memDC.BitBlt(2*rect.Width(), 0, rect.Width(), rect.Height(), 
            &tmpDC, 0, 0, SRCCOPY);
        // ����ǰһ��λͼ
        if(m_nImgIndex > 0)
        {
            strFilePath = m_szImgDir+m_vImgFileName[m_nImgIndex-1];
            m_hBitmap[0]= LoadImageFromFile(strFilePath);
            tmpDC.SelectObject(m_hBitmap[0]);
            m_memDC.BitBlt(0, 0, rect.Width(), rect.Height(), 
                &tmpDC, 0, 0, SRCCOPY);
            //SB_DEBUGLOG(L"image file:%s\n",m_vImgFileName[m_nImgIndex-1]);
        }
        else
        {
            m_hBitmap[0] = NULL;
        }
        break;
     case DM_INIT:
     default:
         // �ͷ�ԭ��ͼƬ�ڴ�
         for(int i=0; i<3; i++)
         {
             if(NULL != m_hBitmap[i])
             {
                 DeleteObject(m_hBitmap[i]);
                 m_hBitmap[i] = NULL;
             }
         }
         // ����ǰһ��λͼ
         if(m_nImgIndex > 0)
         {
             strFilePath = m_szImgDir+m_vImgFileName[m_nImgIndex-1];
             m_hBitmap[0]= LoadImageFromFile(strFilePath);
             tmpDC.SelectObject(m_hBitmap[0]);
             m_memDC.BitBlt(0, 0, rect.Width(), rect.Height(), 
                 &tmpDC, 0, 0, SRCCOPY);
             //SB_DEBUGLOG(L"image file:%s\n",m_vImgFileName[m_nImgIndex-1]);
         }

         // ���뵱ǰλͼ
         strFilePath = m_szImgDir+m_vImgFileName[m_nImgIndex];
         m_hBitmap[1] = LoadImageFromFile(strFilePath);
         tmpDC.SelectObject(m_hBitmap[1]);
         m_memDC.BitBlt(rect.Width(), 0, rect.Width(), rect.Height(), 
             &tmpDC, 0, 0, SRCCOPY);
        //SB_DEBUGLOG(L"image file:%s\n",m_vImgFileName[m_nImgIndex]);

         //�����һ��λͼ
         if(m_nImgIndex < m_vImgFileName.size()-1)
         {
             strFilePath = m_szImgDir+m_vImgFileName[m_nImgIndex+1];
             m_hBitmap[2] = LoadImageFromFile(strFilePath);
             tmpDC.SelectObject(m_hBitmap[2]);
             m_memDC.BitBlt(2*rect.Width(), 0, rect.Width(), rect.Height(), 
                 &tmpDC, 0, 0, SRCCOPY);
             //SB_DEBUGLOG(L"image file:%s\n",m_vImgFileName[m_nImgIndex+1]);
         }
         break;
    }
    ReleaseDC(pDC);
    return TRUE;
}
#if SHENG_ZE
VOID CSliderBox::DrawArrow( CDC *pDC, ARROW_TYPE type, POINT ptPeak, UINT nIncludedAngle, UINT nLength, UINT nWidth /*= 3*/ )
{    
    const double PI = 3.1415926535897932384626433832795;
    CBrush brush;
    brush.CreateSolidBrush(RGB(255,255,255));
    CBrush *pOldBrush = pDC->SelectObject(&brush);
    double nAlpha = (90-nIncludedAngle/2)*PI/180;
    UINT nl0 = (UINT)(nWidth/cos(nAlpha));
    UINT nl1 = (UINT)(nLength*sin(nAlpha));
    UINT nl2 = (UINT)(nLength*cos(nAlpha));
    UINT nl3 = (UINT)(nWidth*sin(nAlpha));
    UINT nl4 = (UINT)(nWidth*cos(nAlpha));
    POINT points[6];
    points[0].x=ptPeak.x;
    points[0].y=ptPeak.y;

    switch(type)
    {
    case ARROW_UP:
        points[3].x = points[0].x;
        points[3].y = points[0].y+nl0;
        points[1].x = points[0].x-nl2;
        points[1].y = points[0].y+nl1;
        points[2].x = points[1].x+nl3;
        points[2].y = points[1].y+nl4;
        points[5].x = points[0].x+nl2;
        points[5].y = points[0].y+nl1;
        points[4].x = points[5].x-nl3;
        points[4].y = points[5].y+nl4;        
        break;
    case ARROW_DOWN:
        points[3].x = points[0].x;
        points[3].y = points[0].y-nl0;
        points[1].x = points[0].x-nl2;
        points[1].y = points[0].y-nl1;
        points[2].x = points[1].x+nl3;
        points[2].y = points[1].y-nl4;
        points[5].x = points[0].x+nl2;
        points[5].y = points[0].y-nl1;
        points[4].x = points[5].x-nl3;
        points[4].y = points[5].y-nl4;   
        break;
    case ARROW_LEFT:
        points[3].x = points[0].x+nl0;
        points[3].y = points[0].y;
        points[1].x = points[0].x+nl1;
        points[1].y = points[0].y+nl2;
        points[2].x = points[1].x+nl4;
        points[2].y = points[1].y-nl3;
        points[5].x = points[0].x+nl1;
        points[5].y = points[0].y-nl2;
        points[4].x = points[5].x+nl4;
        points[4].y = points[5].y+nl3;   
        break;
    case ARROW_RIGHT:
        points[3].x = points[0].x-nl0;
        points[3].y = points[0].y;
        points[1].x = points[0].x-nl1;
        points[1].y = points[0].y+nl2;
        points[2].x = points[1].x-nl4;
        points[2].y = points[1].y-nl3;
        points[5].x = points[0].x-nl1;
        points[5].y = points[0].y-nl2;
        points[4].x = points[5].x-nl4;
        points[4].y = points[5].y+nl3;
        break;
    }
    pDC->Polygon(points, 6);
    pDC->SelectObject(pOldBrush);
}
#endif

BOOL CSliderBox::DrawIndicators( CDC *pDC, RECT rect, UINT nRadius )
{
    CRect rt(rect);
#if 1
    if(m_vImgFileName.size() == 0)
    {
        return FALSE;
    }
    INT nPadding = rt.Width() / m_vImgFileName.size() / 2 -nRadius;
#endif
#if 0
    INT nPadding = rt.Width() / 12 / 2 -nRadius;
#endif
    if(nPadding < 0)
    {
        return FALSE;
    }

    // ������ͼƬ��ָʾԲ����ɫ��
    CBrush brush;
    brush.CreateSolidBrush(RGB(255,255,255));
    CBrush *pOldBrush = pDC->SelectObject(&brush);    
    CRect rtIndicator;
    rtIndicator.top  = (rect.top + rect.bottom)/2 + nRadius;
    rtIndicator.bottom = (rect.top + rect.bottom)/2 - nRadius;
#if 1
    int i=0;
    std::vector<CString>::iterator it;
    for(it=m_vImgFileName.begin(),i=0; it!=m_vImgFileName.end(); it++, i++)
    {
        rtIndicator.left = rect.left + (nRadius+nPadding) * i * 2 +nPadding;        
        rtIndicator.right = rect.left + (nRadius+nPadding) * (i+1)*2-nPadding;        
        pDC->Ellipse(rtIndicator);
    }    
#endif
#if 0
    for(int i=0; i<12; i++)
    {
        rtIndicator.left = rect.left + (nRadius+nPadding) * i * 2 +nPadding;        
        rtIndicator.right = rect.left + (nRadius+nPadding) * (i+1)*2-nPadding;        
        pDC->Ellipse(rtIndicator);
    }    
#endif
    // ����ǰ��ָʾԲ������ɫ��
    CBrush brush1;
#if SHENG_ZE
    brush1.CreateSolidBrush(RGB(172, 199, 232));
#else
	brush1.CreateSolidBrush(RGB(0x66, 0x66, 0x66));
#endif
    pDC->SelectObject(&brush1);
    rtIndicator.left = rect.left + (nRadius+nPadding) * m_nImgIndex * 2 +nPadding;        
    rtIndicator.right = rect.left + (nRadius+nPadding) * (m_nImgIndex+1)*2-nPadding;        
    pDC->Ellipse(rtIndicator);

    // �ָ�DC
    pDC->SelectObject(pOldBrush);
    return TRUE;
}

BOOL CSliderBox::OnEraseBkgnd(CDC* pDC)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    return TRUE;
}

void CSliderBox::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    switch(nIDEvent)
    {
    case TIMER_SCROLL_LEFT:
        if(m_rtMapMem.left - m_nScrollPixel >= 0)
        {
            m_rtMapMem.MoveToX(m_rtMapMem.left-m_nScrollPixel);
            Invalidate(FALSE);

            if(m_rtMapMem.left - m_nScrollPixel < 0)
            {
                m_rtMapMem.MoveToX(0);
                Invalidate(FALSE);
            }
        }
        else
        {  
            // �������ֹͣ����
            KillTimer(nIDEvent);
            m_fScrolling = FALSE;            

            // ���������������3��ͼƬ
            m_rtMapMem.MoveToX(m_rtMapMem.Width());             
            DrawImgsToMem(DM_MOVE_RIGHT);
        }
        break;
    case TIMER_SCROLL_RIGHT:
        if(m_rtMapMem.left + m_nScrollPixel <= m_rtMapMem.Width()*2)
        {
            m_rtMapMem.MoveToX(m_rtMapMem.left + m_nScrollPixel);
            Invalidate(FALSE);

            if(m_rtMapMem.left + m_nScrollPixel > m_rtMapMem.Width()*2)
            {
                m_rtMapMem.MoveToX(m_rtMapMem.Width()*2);
                Invalidate(FALSE);
            }
        }      
        else
        {
            // �������ֹͣ����
            KillTimer(nIDEvent);
            m_fScrolling = FALSE;

            // ���������������3��ͼƬ
            m_rtMapMem.MoveToX(m_rtMapMem.Width());
            DrawImgsToMem(DM_MOVE_LEFT);
        }
        break;
    }            
    CWnd::OnTimer(nIDEvent);
}
BOOL CSliderBox::SetAnimateFreq(UINT nFreq)
{
    // ����Ƶ�ʹ�����Ļˢ�²�����
    if(nFreq > SB_MAX_ANIMATE_FREQ)
    {
        return FALSE;
    }
    m_nAnimateFreq = nFreq;
    CRect rect;
    GetClientRect(&rect);    
    m_nScrollPixel = rect.Width()/(m_nScrollTimeout* m_nAnimateFreq / 1000);
    return TRUE;
}

BOOL CSliderBox::SetScrollTimeout(UINT nTimeout)
{
    // ���⻬��ʱ��̫����Ļ����
    if(nTimeout < SB_MIN_SCROLL_TIMEOUT)
    {
        return FALSE;
    }
    m_nScrollTimeout = nTimeout;    
    CRect rect;
    GetClientRect(&rect);    
    m_nScrollPixel = rect.Width()/(m_nScrollTimeout* m_nAnimateFreq / 1000);
    return TRUE;
}
BOOL CSliderBox::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���
    if(pMsg->message==WM_KEYDOWN)
    {
        switch(pMsg->wParam)
        {
        case VK_LEFT:
            ScrollLeft(m_nScrollTimeout);
            break;
        case VK_RIGHT:
            ScrollRight(m_nScrollTimeout);
            break;
#if !SHENG_ZE
		case VK_UP:
			ScrollLeft(m_nScrollTimeout);
			break;
		case VK_DOWN:
			ScrollRight(m_nScrollTimeout);
			break;
#endif
        }        
    }

    return CWnd::PreTranslateMessage(pMsg);
}
HBITMAP CSliderBox::LoadImageFromFile(const TCHAR * pFileImage)
{     
    IImage *pImageBmp = NULL;     
    HBITMAP hResult = 0;

    if (NULL != m_pImgFactory)
    {
        ImageInfo imageInfo;
        HRESULT hr = m_pImgFactory->CreateImageFromFile(pFileImage, &pImageBmp);
        if (SUCCEEDED(hr)&& SUCCEEDED(pImageBmp->GetImageInfo(&imageInfo)))
        {
            CWindowDC dc(0);
            CDC dcBitmap;
            dcBitmap.CreateCompatibleDC(&dc);
            hResult = CreateCompatibleBitmap(dc.GetSafeHdc(),imageInfo.Width, 
                imageInfo.Height);
            HGDIOBJ  hOldBitmap  =  SelectObject(dcBitmap, hResult);
            //note:�ڴ治��Ļ���Draw�������ܻ�ʧ��
            if(FAILED(pImageBmp->Draw(dcBitmap,  CRect(0,  0,  imageInfo.Width, 
                imageInfo.Height),  NULL)))
            {
                SB_DEBUGLOG(L"Draw Image Failed!\n");
            }
            SelectObject(dcBitmap, hOldBitmap);            
        }  
        if(NULL != pImageBmp)
        {
            pImageBmp->Release();
        }        
    }
    return hResult;
}