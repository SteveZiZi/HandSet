// WebCtrl.cpp : 实现文件
//

#include "stdafx.h"

#include "WebCtrl.h"

//#include "stdafx.h"
#include <windows.h>
// Include the automation definitions...
#include <exdisp.h>
#include <exdispid.h>
#include <mshtmdid.h> // AMBIENT_DLCONTROL

#include <objbase.h>

#include <tchar.h>

#include <wininet.h>
#include <afdfunc.h>	//ras stuff
#include <mshtml.h>
#include <commctrl.h>
#include <iostream>

#define MAX(a,b)  (a > b) ? a : b
#define MIN(a,b)  (a > b) ? b : a

LONG        glThreadCount	= 0;
HWND		ghWndAddressEdit= NULL;
HANDLE      ghExitEvent		= NULL;
//HINSTANCE   g_hInstance		= NULL;
DWORD       g_dwMainWindowStackSize = 0x20000;

IMPLEMENT_DYNAMIC(CWebCtrl, CWnd)

CWebCtrl::CWebCtrl()
{
    RegisterWndClass(NULL, WEBCTRL_CLASSNAME, 0);
    _ulRefs = 1;    
    _pBrowser = NULL;
    _pObject = NULL;
    _pCP = NULL;
    _lpszUrl = NULL;
    _fFullScreen = FALSE;
    _fEmpty = FALSE;
    _szTitle[0] = 0;

    _wZoom = 2; // default zoom
    _wDLCCounter = 0; // counter for Download Completes

    CoInitializeEx(NULL, COINIT_MULTITHREADED);
}

CWebCtrl::~CWebCtrl()
{
    WC_DEBUGLOG(L"IESIMPLE Exiting ~CWebCtrl\r\n");
    if(_pBrowser)
        _pBrowser->Release();
    CoUninitialize();
}

#if 1
BEGIN_MESSAGE_MAP(CWebCtrl, CWnd)
END_MESSAGE_MAP()
#endif
BOOL CWebCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
                      UINT nID, CCreateContext* pContext /*= NULL*/)
{
    BOOL ret = CWnd::Create(WEBCTRL_CLASSNAME, L"", dwStyle, rect, pParentWnd, 
            nID, pContext);

    if(ret != TRUE)
    {
        return FALSE;
    }
    if (!(_hWndBrowser = CreateBrowser()))
        return FALSE;
    ::SetFocus(m_hWnd);    
    return TRUE;
}

BOOL CWebCtrl::CreateEx( DWORD dwExStyle, DWORD dwStyle, const RECT& rect, 
                        CWnd* pParentWnd, UINT nID, LPVOID lpParam /*= NULL*/ )
{
    BOOL ret = CWnd::CreateEx(dwExStyle, WEBCTRL_CLASSNAME, L"", dwStyle, rect, 
        pParentWnd, nID, lpParam);
    if (!(_hWndBrowser = CreateBrowser()))
        return FALSE;
    ::SetFocus(m_hWnd);
    return TRUE;
}

// CWebCtrl 消息处理程序
HWND CWebCtrl::CreateBrowser()
{
    HRESULT hr;
    IUnknown *pUnk = NULL;
    IOleObject *pObject = NULL;


    if (!_pBrowser)
    {        
        hr = CoCreateInstance(CLSID_WebBrowser, NULL,
            CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER,
            IID_IUnknown, (LPVOID *)(&pUnk));
        if (FAILED(hr))
            return FALSE;

        hr = pUnk->QueryInterface(IID_IOleObject, (LPVOID *)(&pObject));
        if (FAILED(hr))
            goto Cleanup;

        DWORD dwFlags;
        hr = pObject->GetMiscStatus(DVASPECT_CONTENT, &dwFlags);
        if (FAILED(hr))
            goto Cleanup;

        if (dwFlags & OLEMISC_SETCLIENTSITEFIRST)
        {
            IOleClientSite *pClientSite;
            hr = QueryInterface(IID_IOleClientSite, (LPVOID *)(&pClientSite));
            if (FAILED(hr))
                goto Cleanup;

            hr = pObject->SetClientSite(pClientSite);
            pClientSite->Release();
            if (FAILED(hr))
                goto Cleanup;
        }

        hr = Activate(pObject);
        if (FAILED(hr))
            goto Cleanup;

        hr = _pObject->QueryInterface(IID_IWebBrowser2, (void **)&_pBrowser);
        if (FAILED(hr))
            goto Cleanup;

        // See if there might be a url in lpszUrl
        hr = pUnk->QueryInterface(IID_IOleInPlaceActiveObject, (LPVOID *)(&_pIPActiveObj));
        if (FAILED(hr))
            _pIPActiveObj = NULL;

        hr = S_FALSE;

        if (_fEmpty)
        {

            BSTR bstrURL = SysAllocString(_lpszUrl);
            if (bstrURL && bstrURL[0])
                _pBrowser->Navigate(bstrURL, NULL, NULL, NULL, NULL);
            else
                _pBrowser->GoHome();

            SysFreeString(bstrURL);

        }
        hr = InitEvents();
    }

Cleanup:
    if (pUnk)
        pUnk->Release();
    if (pObject)
        pObject->Release();

    IOleWindow *pWnd = NULL;
    HWND hwndBrowser = NULL;

    if (_pBrowser)
    {
        hr = _pBrowser->QueryInterface(IID_IOleWindow, (LPVOID *)(&pWnd));
        if (FAILED(hr))
            return NULL;
    }

    if (pWnd)
    {
        hr = pWnd->GetWindow(&hwndBrowser);
        pWnd->Release();
    }
    return hwndBrowser;
}


HRESULT CWebCtrl::Activate(IOleObject *pObject)
{
    _pObject = pObject;
    _pObject->AddRef();

    RECT rc;
    ::GetClientRect(m_hWnd, &rc);

    HRESULT hr;
    hr    =    _pObject->DoVerb(   OLEIVERB_UIACTIVATE,    NULL,    this,    0,    m_hWnd,    &rc);

    if (FAILED(hr))
        goto Cleanup;

Cleanup:
    return hr;
}

HRESULT CWebCtrl::InitEvents()
{
    HRESULT                     hr;
    IConnectionPointContainer  *pCPCont = NULL;
    DWebBrowserEvents          *pEvents = NULL;

    if (!_pBrowser)
        return S_FALSE;
    hr = _pBrowser->QueryInterface(IID_IConnectionPointContainer, (LPVOID *)&pCPCont);
    if (FAILED(hr))
        return S_FALSE;
    hr = pCPCont->FindConnectionPoint(DIID_DWebBrowserEvents2, &_pCP);
    if (FAILED(hr))
    {
        _pCP = NULL;
        goto Cleanup;
    }

    hr = QueryInterface(DIID_DWebBrowserEvents2, (LPVOID *)(&pEvents));
    if (FAILED(hr))
        goto Cleanup;
    hr = _pCP->Advise(pEvents, &(_dwEventCookie));
    if (FAILED(hr))
        goto Cleanup;


Cleanup:
    if (pCPCont)
        pCPCont->Release();
    if (pEvents)
        pEvents->Release();
    return hr;
}

STDMETHODIMP CWebCtrl::GetHostInfo(DOCHOSTUIINFO *pInfo)
{

    pInfo->cbSize = sizeof(DOCHOSTUIINFO);
    //pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER|DOCHOSTUIFLAG_FLAT_SCROLLBAR;
    //pInfo->dwFlags |= DOCHOSTUIFLAG_SCROLL_NO;

    return S_OK;

}

STDMETHODIMP CWebCtrl::Invoke (
                               DISPID dispidMember,
                               REFIID riid,
                               LCID lcid,
                               WORD wFlags,
                               DISPPARAMS FAR* pdparams,
                               VARIANT FAR* pvarResult,
                               EXCEPINFO FAR* pexcepinfo,
                               UINT FAR* puArgErr
                               )
{
    switch (dispidMember)
    {
    case DISPID_AMBIENT_DLCONTROL:
        if(pvarResult)
        {
            V_VT(pvarResult) = VT_I4;
            // always set these three unless they should be disabled
            pvarResult->lVal = DLCTL_DLIMAGES|DLCTL_VIDEOS|DLCTL_BGSOUNDS;

            // put the browser in download only mode
            // pvarResult->lVal |= DLCTL_DOWNLOADONLY;
        }
        break;

    case DISPID_STATUSTEXTCHANGE:
        {
        }
        break;

    case DISPID_SETSECURELOCKICON:
        {
        }
        break;

    case DISPID_PROGRESSCHANGE:
        {
            LONG lProgMax = pdparams->rgvarg[0].lVal;
            LONG lProg = pdparams->rgvarg[1].lVal;
            UINT nPos = (lProg == -1) ? 999 : ((lProg-1)%1000);

            ::SendMessage(_hwndProgress, PBM_SETPOS, nPos, 0);
        }
        break;

        // notification for file download
    case DISPID_FILEDOWNLOAD:
        break;


    case DISPID_NAVIGATECOMPLETE2:
        break;

    case DISPID_COMMANDSTATECHANGE:
        break;

    case DISPID_TITLECHANGE:
        if (pdparams && pdparams->rgvarg[0].vt == VT_BSTR)
        {
            TCHAR szTitle[85];
            int len = wcslen(pdparams->rgvarg[0].bstrVal);

            _tcsncpy(_szTitle, pdparams->rgvarg[0].bstrVal, MAX_URL-1);
            _tcsncpy(szTitle, pdparams->rgvarg[0].bstrVal, 80);
            if(len > 80)
                _tcscat(szTitle, L"...");

            ::SetWindowText(m_hWnd, szTitle);
        }
        break;

    case DISPID_ONQUIT:
        ::PostMessage(m_hWnd, WM_CLOSE, 0, 0L);
        break;

    case DISPID_DOWNLOADBEGIN:
        _wDLCCounter++;
        ::SetWindowPos(_hwndProgress, HWND_TOP, 0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW);
        break;
    case DISPID_DOWNLOADCOMPLETE:
    case DISPID_DOCUMENTCOMPLETE:
        _wDLCCounter--;
        if(_wDLCCounter <= 0)
        {
            ::SetWindowPos(_hwndProgress,NULL, 0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_HIDEWINDOW);
            ChangeFontSize(TRUE);
            _wDLCCounter = 0;
        }
        break;
    case DISPID_PROPERTYCHANGE:
    case DISPID_BEFORENAVIGATE2:
        break;
    default:
        return DISP_E_MEMBERNOTFOUND;
    }
    return S_OK;
}


STDMETHODIMP CWebCtrl::moveTo( LONG x, LONG y)
{
    ::SetWindowPos(m_hWnd, NULL, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
    return S_OK;
}

STDMETHODIMP CWebCtrl::moveBy( LONG x, LONG y) 
{
    RECT rcWindow;

    ::GetWindowRect(m_hWnd, &rcWindow);

    ::SetWindowPos(m_hWnd, NULL, rcWindow.left + x, rcWindow.top + y, 0, 0,
        SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
    return S_OK;
}


STDMETHODIMP CWebCtrl::resizeTo( LONG x, LONG y) 
{
    // We do not want the size to be less then 100 for top level windows in browser
    if (x < 100)
        x = 100;

    if (y < 100)
        y = 100;

    ::SetWindowPos(m_hWnd, NULL, 0, 0, x, y, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);

    return S_OK;             
}

STDMETHODIMP CWebCtrl::resizeBy( LONG x, LONG y) 
{
    RECT rcWindow;
    long w, h;

    ::GetWindowRect(m_hWnd, &rcWindow);

    w = rcWindow.right - rcWindow.left + x;
    h = rcWindow.bottom - rcWindow.top + y;

    if (w < 100)
        w = 100;

    if (h < 100)
        h = 100;

    ::SetWindowPos(m_hWnd, NULL, 0, 0, w, h, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
    return S_OK;
}

LRESULT CWebCtrl::HandleCommand(WPARAM wParam, LPARAM lParam)
{
    UINT nID = LOWORD(wParam);
    switch (nID)
    {
    case ID_INTERNET_OPTIONS:
        {
            SHELLEXECUTEINFO sei;
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_FLAG_NO_UI;
            sei.hwnd = m_hWnd;
            sei.lpVerb = NULL;
            sei.lpFile = L"ctlpnl";
            sei.lpParameters = L"inetcpl.cpl";
            sei.lpDirectory = NULL;
            sei.nShow = SW_SHOWNORMAL;

            ShellExecuteEx(&sei); 
        }
        break;
    case ID_FIND:
        FindString();
        break;
    case ID_GO_BACK:
        _pBrowser->GoBack();
        return 0;
    case ID_GO_FORWARD:
        _pBrowser->GoForward();
        return 0;
    case ID_GO_HOME:
        _pBrowser->GoHome();
        return 0;
    case ID_GO_SEARCH:
        _pBrowser->GoSearch();
        return 0;
    case ID_VIEW_REFRESH:
        _pBrowser->Refresh();
        return 0;
    case ID_VIEW_STOP:
        _pBrowser->Stop();
        return 0;

    case ID_ZOOMUP:
        _wZoom++;
        if(_wZoom > 4)
            _wZoom = 4;
        ChangeFontSize(FALSE);
        return 0;

    case ID_ZOOMDOWN:
        _wZoom--;
        if(_wZoom < 0)
            _wZoom = 0;
        ChangeFontSize(FALSE);
        return 0;
    case ID_CLOSE:
        _pBrowser->ExecWB(OLECMDID_CLOSE, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
        return 0;
    case ID_FULLSCREEN:
        {
            DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
            if(_fFullScreen)
            {
                dwStyle |= (WS_OVERLAPPED | WS_SYSMENU | WS_THICKFRAME);
                SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
                ::SetWindowPos(m_hWnd,NULL,  _rcWnd.left, _rcWnd.top, _rcWnd.right-_rcWnd.left, _rcWnd.bottom-_rcWnd.top, SWP_NOZORDER);
                _fFullScreen = FALSE;
            }
            else
            {
                RECT rcWorkArea;

                SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
                _fFullScreen = TRUE;
                dwStyle &= ~(WS_OVERLAPPED | WS_SYSMENU | WS_THICKFRAME);

                SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
                ::SetWindowPos(m_hWnd,NULL,  rcWorkArea.left, rcWorkArea.top, rcWorkArea.right-rcWorkArea.left, rcWorkArea.bottom-rcWorkArea.top, SWP_NOZORDER);

            }
        }
        return 0;
    }
    return 0;
}

BOOL CWebCtrl::PreTranslateMessage(LPMSG pMsg)
{
    if (::TranslateAccelerator(m_hWnd, _hAccelTbl, pMsg))  //first shot to the main frame
        return TRUE;

    //then to mshtml
    if (_pIPActiveObj && pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
    {
        HRESULT hr = _pIPActiveObj->TranslateAccelerator(pMsg);
        return (hr != S_FALSE);
    }
    return FALSE;
}

VOID CWebCtrl::ChangeFontSize(BOOL fInit)
{
    VARIANT vaSize;
    vaSize.vt = VT_I4;

    if (fInit) {
        // Setting initial value
        vaSize.vt = 0;
        HRESULT hr = _pBrowser->ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER,
            NULL, &vaSize);
        _wZoom = vaSize.lVal;
    } else {
        vaSize.lVal = _wZoom;
        HRESULT hr = _pBrowser->ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER,
            &vaSize, NULL);
    }
}

VOID CWebCtrl::Close()
{
    RETAILMSG(1,(L"IESIMPLE CMainWnd::Close"));

    // Tell the shell to die off
    ::SendMessage(_hWndBrowser, WM_CLOSE, 0,0);


    if(_pIPActiveObj)
    {
        _pIPActiveObj->Release();
    }
    _pIPActiveObj = NULL;
    if(_pCP)
    {
        _pCP->Unadvise(_dwEventCookie);
        _pCP->Release();
    }
    _pCP = NULL;
    if(_pObject)
    {
        _pObject->Close(FALSE);
        _pObject->Release();
    }
    _pObject = NULL;
    if(_pBrowser)
        _pBrowser->Release();
    _pBrowser = NULL;

}




HRESULT CWebCtrl::FindString()
{
    LPDISPATCH pDisp = NULL;
    LPOLECOMMANDTARGET pCmdTarg = NULL;
    HRESULT sts = S_OK;
    VARIANTARG var;

#define GOERROR_S(bNotCond)  {if (!(bNotCond)) goto errorS; }

    if (!_pBrowser)
        return S_OK;
    sts = _pBrowser->get_Document(&pDisp);
    GOERROR_S(pDisp);
    sts = pDisp->QueryInterface(IID_IOleCommandTarget, (LPVOID*)&pCmdTarg);
    GOERROR_S(pCmdTarg);

    var.vt   = VT_I4;
    var.lVal = 0;
    sts = pCmdTarg->Exec(
        NULL,
        OLECMDID_FIND,
        MSOCMDEXECOPT_PROMPTUSER,
        NULL,
        &var);

errorS:
    if (pCmdTarg)
        pCmdTarg->Release(); // release document's command target
    if (pDisp)
        pDisp->Release();    // release document's dispatch interface
    return sts;
}
//
//-----------------------------------------------------------------------------
// 注册窗口类
//-----------------------------------------------------------------------------
//
BOOL  CWebCtrl::RegisterWndClass (HINSTANCE hInstance, LPCTSTR lpszClassName,
                                  UINT style, HICON hIcon, int bgStockBrush)
{
    WNDCLASS  wlsold = {0};
    if (::GetClassInfo(hInstance, lpszClassName, &wlsold))
    {
        // class已经存在
        return TRUE;
    }

    WC_DEBUGLOG(L"CWebCtrl::RegisterWndClass\n");

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