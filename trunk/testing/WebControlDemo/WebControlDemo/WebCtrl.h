#pragma once

#include "mshtmhst.h"
#include "interned.h"
#define MAX_URL  2048




// 类名
#define ID_VIEW_STOP			500
#define ID_VIEW_REFRESH			501
#define ID_GO_BACK				502
#define ID_GO_FORWARD			503
#define ID_FOCUS_URL			504
#define ID_OPEN					505
#define ID_GO_HOME				506
#define ID_GO_SEARCH			507
#define ID_FULLSCREEN			508
#define ID_INTERNET_OPTIONS		509
#define ID_FIND					510
#define ID_CLOSE                511
#define ID_ZOOMUP               512
#define ID_ZOOMDOWN             513

#define IDS_IE					20000
#define IDS_ADDRESS				20001
#define IDS_FILESCHEME			20002
#define IDS_HYPHEN				20003
#define IDS_BROWSEFILTER		20010
#define WEBCTRL_CLASSNAME       L"WebCtrl"
// 调试
#define WC_DEBUGLOG  NKDbgPrintfW

// CWebCtrl

class CWebCtrl : 
    public CWnd ,
    public IOleContainer,
    public IOleClientSite,
    public IOleInPlaceSite,
    public IServiceProvider,
    public DWebBrowserEvents2,
    public IDocHostUIHandler,
    public IDocHostShowUI,              // msgbox, help window
    public IHTMLOMWindowServices        // for window move, resize events
{
	DECLARE_DYNAMIC(CWebCtrl)

public:    
    //==========================================================================
    // 创建
    //==========================================================================
	CWebCtrl();
	virtual ~CWebCtrl();
    
    virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
        UINT nID, CCreateContext* pContext = NULL);

    virtual BOOL CreateEx(DWORD dwExStyle, DWORD dwStyle, const RECT& rect, 
        CWnd* pParentWnd, UINT nID, LPVOID lpParam = NULL);

public:    
    //==========================================================================
    // 操作
    //==========================================================================    
    VOID                    Close();
    HWND                    GetWindow() {   return m_hWnd;}
    LRESULT                 HandleCommand(WPARAM wParam, LPARAM lParam);

    IWebBrowser2            *_pBrowser;

public:    
    //==========================================================================
    // 实现COM接口方法
    //==========================================================================  
    //IUnknown methods
    STDMETHOD(QueryInterface) (REFIID riid, LPVOID * ppv)
    {
        if ((riid == IID_IOleContainer) || (riid == IID_IUnknown))
        {
            *ppv = (IOleContainer *) this;
        }
        else if (riid == IID_IOleClientSite)
        {
            *ppv = (IOleClientSite *)this;
        }
        else if (riid == IID_IOleInPlaceSite)
        {
            *ppv = (IOleInPlaceSite *)this;
        }
        else if (riid == IID_IOleWindow)
        {
            *ppv = (IOleWindow *)this;
        }
        else if ((riid == DIID_DWebBrowserEvents2) || (riid == IID_IDispatch))
        {
            *ppv = (DWebBrowserEvents2 *)this;
        }
        else if(riid == IID_IServiceProvider)
        {
            *ppv = (IServiceProvider *)this;
        }
        else if (riid == IID_IDocHostUIHandler)
        {
            *ppv = (IDocHostUIHandler *)this;
        }
        else if (riid == IID_IDocHostShowUI)
        {
            *ppv = (IDocHostShowUI *)this;
        }
        else if (riid == IID_IHTMLOMWindowServices)
        {
            *ppv = (IHTMLOMWindowServices *)this;
        }
        else
        {
            *ppv = NULL;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }

    STDMETHOD_(ULONG, AddRef) (void)
    {
        InterlockedIncrement((LONG*)&_ulRefs);
        return _ulRefs;
    }

    STDMETHOD_(ULONG, Release) (void)
    {
        ULONG ulRefs = _ulRefs;
        if (InterlockedDecrement((LONG*)&_ulRefs) == 0)
        {
            delete this;
            return 0;
        }
        return ulRefs - 1;
    }
    //==========================================================================
    // 最简实现OLE等接口
    //========================================================================== 
    // IOleContainer methods
    STDMETHOD(ParseDisplayName)(IBindCtx *, LPOLESTR, ULONG *, IMoniker **) { return E_NOTIMPL;}
    STDMETHOD(EnumObjects)(DWORD, IEnumUnknown **)
    {
        return E_NOTIMPL;
    }
    STDMETHOD(LockContainer)(BOOL)                          { return S_OK;}

    // IOleClientSite methods
    STDMETHOD(SaveObject) (void)                            { return E_NOTIMPL;}
    STDMETHOD(GetMoniker) (DWORD dwAssign, DWORD dwWhichMoniker, LPMONIKER * ppmk) { return E_NOTIMPL;}
    STDMETHOD(GetContainer) (LPOLECONTAINER * ppContainer)
    {
        return E_NOINTERFACE;
    }
    STDMETHOD(ShowObject) (void)                            { return E_NOTIMPL;}
    STDMETHOD(OnShowWindow) (BOOL fShow)                    { return E_NOTIMPL;}
    STDMETHOD(RequestNewObjectLayout) (void)                { return E_NOTIMPL;}

    // IOleWindow methods
    STDMETHOD(GetWindow)(HWND *phwnd)
    {
        *phwnd = m_hWnd;
        return S_OK;
    }
    STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode)        { return E_NOTIMPL;}

    // IOleInPlaceSite methods
    STDMETHOD(CanInPlaceActivate)   (void)                  { return S_OK;}
    STDMETHOD(OnInPlaceActivate)    (void)
    {
        _bInPlaceActive = TRUE;
        return S_OK;
    }

    STDMETHOD(OnUIActivate)         (void)                  { return E_NOTIMPL;}
    STDMETHOD(GetWindowContext) (
        LPOLEINPLACEFRAME FAR *     lplpFrame,
        LPOLEINPLACEUIWINDOW FAR *  lplpDoc,
        LPRECT                      lprcPosRect,
        LPRECT                      lprcClipRect,
        LPOLEINPLACEFRAMEINFO       lpFrameInfo)
    {

        ::GetClientRect(m_hWnd, lprcPosRect);
        ::GetClientRect(m_hWnd, lprcClipRect);

        return S_OK;
    }

    STDMETHOD(Scroll)               (SIZE scrollExtent)     { return E_NOTIMPL;}
    STDMETHOD(OnUIDeactivate)       (BOOL fUndoable)        { return E_NOTIMPL;}
    STDMETHOD(OnInPlaceDeactivate)  (void)
    {
        _bInPlaceActive = FALSE;
        return S_OK;
    }

    STDMETHOD(DiscardUndoState)     (void)                  { return E_NOTIMPL;}
    STDMETHOD(DeactivateAndUndo)    (void)                  { return E_NOTIMPL;}
    STDMETHOD(OnPosRectChange)      (LPCRECT lprcPosRect)   { return E_NOTIMPL;}

    // IServiceProvider

    STDMETHOD(QueryService)(REFGUID guidService, REFIID riid, void **ppvObj)
    {
        if(guidService == IID_IHTMLOMWindowServices)
        {
            return QueryInterface(riid, ppvObj);
        }
        else
            return E_FAIL;
    }

    //DWebBrowserEvents
    //IDispatch methods
    STDMETHOD(GetTypeInfoCount)(UINT FAR* pctinfo)          { return E_NOTIMPL;}

    STDMETHOD(GetTypeInfo)(UINT itinfo,LCID lcid,ITypeInfo FAR* FAR* pptinfo)   { return E_NOTIMPL;}

    STDMETHOD(GetIDsOfNames)(REFIID riid,OLECHAR FAR* FAR* rgszNames,UINT cNames,
        LCID lcid, DISPID FAR* rgdispid)                  { return E_NOTIMPL;}

    STDMETHOD(Invoke)(DISPID dispidMember,REFIID riid,LCID lcid,WORD wFlags,
        DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
        EXCEPINFO FAR* pexcepinfo,UINT FAR* puArgErr);

    // IDocHostUIHandler methods
    STDMETHOD(EnableModeless)(BOOL fEnable) { return E_NOTIMPL; }
    STDMETHOD(FilterDataObject)(IDataObject *pDO, IDataObject **ppDORet) { return E_NOTIMPL; }
    STDMETHOD(GetDropTarget)(IDropTarget *pDropTarget, IDropTarget **ppDropTarget) { return E_NOTIMPL; }
    STDMETHOD(GetExternal)(IDispatch **ppDispatch) { return E_NOTIMPL; }
    STDMETHOD(GetHostInfo)(DOCHOSTUIINFO *pInfo);
    STDMETHOD(GetOptionKeyPath)(LPOLESTR *pchKey, DWORD dw) { return E_NOTIMPL; }
    STDMETHOD(HideUI)(void) { return E_NOTIMPL; }
    STDMETHOD(OnDocWindowActivate)(BOOL fActivate) { return E_NOTIMPL; }
    STDMETHOD(OnFrameWindowActivate)(BOOL fActivate) { return E_NOTIMPL; }
    STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fFrameWindow) { return E_NOTIMPL; }
    STDMETHOD(ShowContextMenu)(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved) { return E_NOTIMPL; }
    STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject *pActiveObject,
        IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame,
        IOleInPlaceUIWindow *pDoc) { return S_FALSE; }
    STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID *pgudCmdGroup, DWORD nCmdID) { return E_NOTIMPL; }
    STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut) { return E_NOTIMPL; }
    STDMETHOD(UpdateUI)(void) { return E_NOTIMPL; }

    // IDocHostShowUI methods
    STDMETHOD(ShowHelp)( HWND hwnd, LPOLESTR pszHelpFile, UINT uCommand,
        DWORD dwData, POINT ptMouse, IDispatch *pDispatchObjectHit)
    { return E_NOTIMPL; }


    STDMETHOD(ShowMessage)( HWND hwnd, LPOLESTR lpstrText, LPOLESTR lpstrCaption, DWORD dwType,
        LPOLESTR lpstrHelpFile, DWORD dwHelpContext, LRESULT *plResult)
    {
        int res = ::MessageBox(hwnd, lpstrText, lpstrCaption, dwType);
        if(plResult)
            *plResult = res;
        return S_OK;
    }

    // IHTMLOMWindowServices methods
    STDMETHOD(moveTo)( LONG x, LONG y);     
    STDMETHOD(moveBy)( LONG x, LONG y);
    STDMETHOD(resizeTo)( LONG x,LONG y);
    STDMETHOD(resizeBy)( LONG x, LONG y);

protected:
    BOOL            PreTranslateMessage(LPMSG msg);
    HRESULT         FindString();
    VOID            ChangeFontSize(BOOL fInit);
    HWND            CreateBrowser();
    HRESULT         Activate(IOleObject *pObject);
    HRESULT         InitEvents();
    //
    //======================================================================
    // 辅助
    //======================================================================
    //
    static BOOL  RegisterWndClass (HINSTANCE hInstance, LPCTSTR lpszClassName,
        UINT style, HICON hIcon = 0, int bgStockBrush = NULL_BRUSH);

protected:
    //==========================================================================
    // 消息处理
    //========================================================================== 
    DECLARE_MESSAGE_MAP()
#if 0
    afx_msg UINT  OnGetDlgCode ();
    afx_msg void  OnPaint ();
    afx_msg BOOL  OnEraseBkgnd (CDC* pDC);
    afx_msg void  OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void  OnDestroy ();
    afx_msg void  OnKillFocus (CWnd* pNewWnd);
    afx_msg void  OnSetFocus (CWnd* pOldWnd);
#endif
public:
    LPTSTR                  _lpszUrl;
    BOOL                    _fEmpty;
    BOOL                    _fFullScreen;
    RECT                    _rcWnd;
    //HWND                    m_hWnd;
protected:
    HACCEL                  _hAccelTbl;			//a handle to the accelerator table    

    ULONG                   _ulRefs;            //reference count for the interfaces supported by the container

    HWND                    _hWndBrowser;       //handle to the browser window

    IOleObject              *_pObject;
    IConnectionPoint        *_pCP;
    IOleInPlaceActiveObject *_pIPActiveObj;
    BOOL                    _bInPlaceActive;
    DWORD                   _dwEventCookie;

    TCHAR                   _szTitle[MAX_URL];   //title of the current document
    HWND                    _hwndProgress;          // progress bar
    RECT                    _rcProgress;

    int                     _wZoom;
    int			            _wDLCCounter;
};


