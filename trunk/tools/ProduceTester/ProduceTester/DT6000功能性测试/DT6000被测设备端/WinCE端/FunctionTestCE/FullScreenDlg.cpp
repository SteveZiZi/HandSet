// FullScreenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FunctionTestCE.h"
#include "FullScreenDlg.h"

#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "ScreenTest.h"
#include "INITGUID.h"
#include "Imaging.h"
// CFullScreenDlg dialog

IMPLEMENT_DYNAMIC(CFullScreenDlg, CDialog)

CFullScreenDlg::CFullScreenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFullScreenDlg::IDD, pParent),
    m_hMemDC(NULL),
    m_hMemBitmap(NULL),
    m_iImgWidth(0),
    m_iImgHeight(0)
{

}

CFullScreenDlg::~CFullScreenDlg()
{
}

void CFullScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFullScreenDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CFullScreenDlg message handlers

BOOL CFullScreenDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    m_showLevel=SCREEN_PARAM_INIT;
    return TRUE;  
}

void CFullScreenDlg::OnPaint()
{
    CPaintDC dc(this); 

    int cx = ::GetSystemMetrics(SM_CXSCREEN);
    int cy = ::GetSystemMetrics(SM_CYSCREEN);

    RECT rect = {0, 0, cx, cy};
    CString strTitle;
    CSize size;
    CBrush brush;

    switch (m_showLevel)
    {
    case  SCREEN_PARAM_TEST_COLOR_LEVEL:
        {
            const BYTE ColorTab[10][3] = {{128, 0, 0}, {255, 0, 0}, {255, 192, 0}, {255, 255, 0}, {0, 255, 0}, {0, 255, 255}, {0, 0, 255}, {160, 0, 255}, {0, 0, 0}, {255, 255, 255}};
            const char cColorName[10][3] = {"×Ø", "ºì", "³È", "»Æ", "ÂÌ", "Çà", "À¶", "×Ï", "ºÚ", "°×"};
            //dc.SelectStockObject(SYSTEM_FIXED_FONT);
            dc.SetTextColor(RGB(128, 128, 128));
            for (DWORD i = 0; i < 10; i++) {
                RECT rect0 = {i * (cx / 10), 0, (i + 1) * (cx / 10), cy};
                brush.CreateSolidBrush(RGB(ColorTab[i][0], ColorTab[i][1], ColorTab[i][2]));
                dc.SelectObject(&brush);
                dc.FillRect(&rect0, &brush);
                strTitle = cColorName[i];
                //dc.TextOut((DWORD)((i + 0.5) * (cx / 10)) - 8, cy / 2 - 8, strTitle);
                brush.DeleteObject();
                dc.ExtTextOut((DWORD)((i + 0.5) * (cx / 10)) - 8, cy / 2 - 8, ETO_CLIPPED, NULL, strTitle, NULL);
            }
            dc.SetTextColor(RGB(128, 128, 128));
            strTitle = _T("²ÊÉ«²âÊÔ");
            size = dc.GetTextExtent(strTitle);
            //dc.TextOut((cx - size.cx) / 2, 5, strTitle); 
            dc.ExtTextOut((cx - size.cx) / 2, 5, ETO_CLIPPED, NULL, strTitle, NULL);
        }
        break;
    case  SCREEN_PARAM_TEST_IMAGE:
        if (m_hMemBitmap !=NULL)
        {
            StretchBlt(dc.m_hDC,0,0,cx,cx,m_hMemDC,0,0,m_iImgWidth,m_iImgHeight,SRCCOPY);


            strTitle = _T("Í¼Æ¬ÏÔÊ¾²âÊÔ");
            size = dc.GetTextExtent(strTitle);
            dc.SetTextColor(RGB(0, 128, 0));
            dc.ExtTextOut((cx - size.cx) / 2, 5, ETO_CLIPPED, NULL, strTitle, NULL);
            break;
        }

       
    default:
        {
            CString str=L"ÇëÉÔºò...";
            dc.DrawText(str,&rect,DT_CENTER|DT_VCENTER);
        }
        

        break;
    }

}

BOOL CFullScreenDlg::DisplayColorLevel()
{
    m_showLevel=SCREEN_PARAM_TEST_COLOR_LEVEL;
    InvalidateRect(NULL,TRUE);
    Sleep(500);
    return TRUE;
}

BOOL CFullScreenDlg::DisplayStartInterface()
{

    m_showLevel=SCREEN_PARAM_INIT;
    ShowWindow(SW_SHOW);
    UpdateWindow();
    int cx=GetSystemMetrics(SM_CXSCREEN);
    int cy=GetSystemMetrics(SM_CYSCREEN);

    MoveWindow(0,0,cx,cy,TRUE);
    ::SetWindowPos(this->m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
  
    Sleep(500);
    return TRUE;
}


BOOL CFullScreenDlg::LoadTestImage()
{
    if (m_hMemBitmap !=NULL)
    {
        return TRUE;
    }
    IImagingFactory *pImgFactory = NULL; 
    IImage *pImage = NULL; 
    CoInitializeEx(NULL, COINIT_MULTITHREADED); 
   
    //	REFCLSID CLSID_ImagingFactory_2;
    if (SUCCEEDED(CoCreateInstance(CLSID_ImagingFactory, 
        NULL, 
        CLSCTX_INPROC_SERVER, 
        IID_IImagingFactory, 
        (void **)&pImgFactory))) 
    { 
        ImageInfo imageInfo; 
        if (SUCCEEDED(pImgFactory->CreateImageFromFile(L"\\test.jpg", &pImage)) 
            && SUCCEEDED(pImage->GetImageInfo(&imageInfo))) 
        { 
            m_iImgWidth=imageInfo.Width;
            m_iImgHeight=imageInfo.Height;
            CClientDC dc(this);
            m_hMemDC=CreateCompatibleDC(dc.m_hDC);            
            m_hMemBitmap = CreateCompatibleBitmap(dc.GetSafeHdc(), imageInfo.Width, imageInfo.Height); 
            if (m_hMemBitmap) { 
                HGDIOBJ hOldBitmap =SelectObject(m_hMemDC,m_hMemBitmap); 
                pImage->Draw(m_hMemDC, CRect(0, 0, imageInfo.Width, imageInfo.Height), NULL); 
                DeleteObject(hOldBitmap); 
            } 
            pImage->Release(); 
        } 
        pImgFactory->Release(); 
    } 
    CoUninitialize(); 

    return m_hMemBitmap!=NULL; 
}

BOOL CFullScreenDlg::DisplayTestImage()
{
    if (m_hMemBitmap ==NULL)
    {
        LoadTestImage();
    }

    m_showLevel=SCREEN_PARAM_TEST_IMAGE;
    InvalidateRect(NULL,TRUE);
    Sleep(500);
    return TRUE;
}
void CFullScreenDlg::OnDestroy()
{
    CDialog::OnDestroy();

    if (m_hMemDC!=NULL)
    {
        DeleteDC(m_hMemDC);
        m_hMemDC=NULL;
    }
    if (m_hMemBitmap !=NULL)
    {
        DeleteObject(m_hMemBitmap);
        m_hMemBitmap=NULL;
    }
}
