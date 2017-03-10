/**
 * \file
 *      dsm_utility.cpp
 *
 * \brief
 *      dsm帮助函数实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/3/13
 */


#include "stdafx.h"
#include "dsm_utility.h"
#include "src/main/resourcestr.h"
#include "src/utils/res/dsm_stringmgr.h"
#include "src/main/mgr/statusmgr.h"

#define IOCTL_GPIO_GETBIT       \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0303, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define ROUND(x)    static_cast<int>((x)>=0?((x)+0.5):((x)-0.5))      ///< x四舍五入

/**
 *      获取模块路径字符串
 *
 * \return
 *      CString     模块的路径字符串
 * \note
 *      比如x:\aaaaa\bbbb\ccc.exe则返回x:\aaaaa\bbbb\
 */
const CString DSM_GetAppDir()
{
    static CString csAppDir(L"");

    if(csAppDir.IsEmpty())
    {
        ::GetModuleFileName(NULL, csAppDir.GetBuffer(MAX_PATH + 1), MAX_PATH);
        csAppDir.ReleaseBuffer();

        csAppDir = csAppDir.Left(csAppDir.ReverseFind(L'\\') + 1);
    }

    return csAppDir;
}

void DSM_ParentToChild(CWnd* pChild, CRect& rcRect)
{
    AfxGetMainWnd()->ClientToScreen(rcRect);
    pChild->ScreenToClient(rcRect);
}

LPCTSTR DSM_GetFileExtension(LPCTSTR pFile)
{
    CString csFile(pFile);

    return csFile.Right(csFile.GetLength() - csFile.ReverseFind(L'.') - 1);
}

extern void DSM_SetMenuString( HMENU hMenu, UINT uIDItem, BOOL fByPosition, CString csVal )
{
    MENUITEMINFO mii;
    memset((char*)&mii, 0, sizeof(mii));
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_TYPE;
    mii.fType = MFT_STRING;
    mii.dwTypeData = csVal.GetBuffer(csVal.GetLength() + 1);
    mii.cch = csVal.GetLength() + 1;

    SetMenuItemInfo(hMenu, uIDItem, fByPosition, &mii);

    csVal.ReleaseBuffer(csVal.GetLength() + 1);
}

extern bool DSM_Str2Float( CString csStr, float& rfVal, int nInteger /*= 5*/, int nDecimal /*= 3*/ )
{
    csStr.Trim();

    float fval      = 0.0;
    bool bDot       = false;
    bool bSign      = false;
    int nLen        = csStr.GetLength();
    int nDot        = nLen;

    for(int i = 0; i < nLen; i++)
    {
        if(0 == _istdigit(csStr.GetAt(i)))
        {
            // 判断小数点
            if(L'.' == csStr.GetAt(i) && !bDot)
            {
                bDot = true;
                nDot = i;       // 记录.位置
                continue;
            }

            // 判断符号
            if(L'+' == csStr.GetAt(i) || L'-' == csStr.GetAt(i))
            {
                if(0 == i)
                {
                    bSign = true;
                    continue;
                }
                else
                {
                    return false;
                }
            }

            return false;
        }
    }

    if(bSign)
    {
        nDot--;
    }

    // 判断整数长度
    if(nDot > nInteger)
    {
        return false;
    }

    if(EOF == swscanf(csStr, L"%f", &fval) || 0 == _finite(fval))
    {
        return false;
    }

    rfVal = fval;

    return true;
}

extern bool DSM_Str2Double( CString csStr, double& rdVal, int nInteger /*= 5*/, int nDecimal /*= 3*/ )
{
    csStr.Trim();

    double dval      = 0.0;
    bool bDot       = false;
    bool bSign      = false;
    int nLen        = csStr.GetLength();
    int nDot        = nLen;

    for(int i = 0; i < nLen; i++)
    {
        if(0 == _istdigit(csStr.GetAt(i)))
        {
            // 判断小数点
            if(L'.' == csStr.GetAt(i) && !bDot)
            {
                bDot = true;
                nDot = i;       // 记录.位置
                continue;
            }

            // 判断符号
            if(L'+' == csStr.GetAt(i) || L'-' == csStr.GetAt(i))
            {
                if(0 == i)
                {
                    bSign = true;
                    continue;
                }
                else
                {
                    return false;
                }
            }

            return false;
        }
    }

    if(bSign)
    {
        nDot--;
    }

    // 判断整数长度
    if(nDot > nInteger)
    {
        return false;
    }

    if(EOF == swscanf(csStr, L"%lf", &dval) || 0 == _finite(dval))
    {
        return false;
    }

    rdVal = dval;

    return true;
}

extern CString DSM_GetPortDesc(DsmPortName ePort)
{
    CString csDesc;

    switch(ePort)
    {
    case NET_PORT_LIGHT_A:
        csDesc = dsm_stringMgr::Get(DSM_STR_PORT_FIBERA);
        break;

    case NET_PORT_LIGHT_B:
        csDesc = dsm_stringMgr::Get(DSM_STR_PORT_FIBERB);
        break;

    case NET_PORT_ELECTRONIC:
        csDesc = dsm_stringMgr::Get(DSM_STR_PORT_ETHA);
        break;

    case FT3_PORT_LIGHT_A:
        csDesc = dsm_stringMgr::Get(DSM_STR_PORT_LIGHTA);
        break;

    case FT3_PORT_LIGHT_B:
        csDesc = dsm_stringMgr::Get(DSM_STR_PORT_LIGHTB);
        break;

    case FT3_PORT_LIGHT_C:
        csDesc = dsm_stringMgr::Get(DSM_STR_PORT_LIGHTC);
        break;

    case DSM_PORT_UNKOWN:
        csDesc = dsm_stringMgr::Get(DSM_STR_PORT_UNKNOWN);
        break;

    default:
        csDesc = dsm_stringMgr::Get(DSM_STR_PORT_UNKNOWN);
        break;
    }

    return csDesc;
}

extern CString DSM_GetCtrlBlkInfo( CBaseDetectItem* pCtrlBlk )
{
    if(!pCtrlBlk)
    {
        ASSERT(pCtrlBlk);
        return L"";
    }

    CString csInfo;

    switch(pCtrlBlk->enFrameType)
    {
        // 获取goose与91协议的控制块描述  appid : port
    case FRAME_TYPE_NET_9_1:
    case FRAME_TYPE_NET_GOOSE:
        {
            CNetDetectItem* pNet = dynamic_cast<CNetDetectItem*>(pCtrlBlk);
            ASSERT(pNet);

            csInfo.Format(L"0x%04X : %s", pNet->appId, DSM_GetPortDesc(pNet->enPortName));
        }
        break;

        // 获取92协议的控制块描述  appid : desc|port
    case FRAME_TYPE_NET_9_2:
        {
            CNetDetectItem* pNet = dynamic_cast<CNetDetectItem*>(pCtrlBlk);
            ASSERT(pNet);

            CIEC9_2Cb* pCB = CDsmDataLayer::getInstance()->GetSclManager()->GetIEC9_2Cb(pNet->appId);

            CString csDesc;
            if(pCB)
            {
                csDesc = pCB->GetDescription().c_str();
            }

            CString csPort = DSM_GetPortDesc(pNet->enPortName);

            if(csDesc.IsEmpty())
            {
                csDesc = csPort;
            }
            else
            {
                csDesc.Format(L"%s : %s", csDesc, csPort);
            }

            csInfo.Format(L"0x%04X : %s", pNet->appId, csDesc);
        }
        break;

        // 获取FT3协议的控制块描述  ldname : port
    case FRAME_TYPE_FT3_NANRUI:
    case FRAME_TYPE_FT3_GUOWANG:
        {
            CFt3DetectItem* pFT3 = dynamic_cast<CFt3DetectItem*>(pCtrlBlk);
            ASSERT(pFT3);

            csInfo.Format(L"0x%04X : %s", pFT3->nLDName, DSM_GetPortDesc(pFT3->enPortName));
        }
        break;

    case FRAME_TYPE_UNKOWN:
        csInfo.Format(L"%s %d", dsm_stringMgr::Get(DSM_STR_CTRLBLK_DESC), pCtrlBlk->nID);
        TRACE(L"dsm: error port type to get ctrlblk desc.\n");
        break;

    default:
        csInfo.Format(L"%s %d", dsm_stringMgr::Get(DSM_STR_CTRLBLK_DESC), pCtrlBlk->nID);
        TRACE(L"dsm: error port type to get ctrlblk desc.\n");
        ASSERT(FALSE);
        break;
    }

    return csInfo;
}

extern const CString DSM_GetNetCtrlBlkDesc( CBaseDetectItem* pCtrlBlk )
{
    CString csDesc(L"");
    CNetDetectItem* pNet = dynamic_cast<CNetDetectItem*>(pCtrlBlk);

    if(!pNet)
    {
        ASSERT(pNet);
        return csDesc;
    }

    switch(pCtrlBlk->enFrameType)
    {
    case FRAME_TYPE_NET_9_2:
        {
            CIEC9_2Cb* pCB = CDsmDataLayer::getInstance()->GetSclManager()->GetIEC9_2Cb(pNet->appId);

            if(pCB)
            {
                csDesc = pCB->GetDescription().c_str();
            }
        }
        break;

    case FRAME_TYPE_NET_GOOSE:
        {
            CGooseCb* pCB = CDsmDataLayer::getInstance()->GetSclManager()->GetGooseCb(pNet->appId);

            if(pCB)
            {
                csDesc = pCB->GetDescription().c_str();
            }
        }
        break;

    case FRAME_TYPE_NET_9_1:
        {

        }
        break;

    default:
        ASSERT(FALSE);
        break;
    }

    // 描述信息为空，则去MAC信息
    if(csDesc.IsEmpty())
    {
        csDesc.Format(L"%02X-%02X-%02X-%02X-%02X-%02X", 
            pNet->macAddr[0], 
            pNet->macAddr[1], 
            pNet->macAddr[2], 
            pNet->macAddr[3], 
            pNet->macAddr[4], 
            pNet->macAddr[5]);
    }

    return csDesc;
}

extern float DSM_GetRelativePhase( float fAbsPhaseMark, float fAbsPhase, int nDecimalMed/* = 6*/, float fMin/* = -180.0f*/, float fMax /*= 180.0f */)
{
    float fRange = 0.0f;
    fRange = fMax - fMin;
    float fMulti = 0;
    long long llAbsPhaseMark = 0;
    long long llAbsPhase = 0;
    long long llRange = 0;
    long long llMin = 0;
    long long llMax = 0;
    long long llResult = 0;
    fMulti = pow(10.0f, nDecimalMed);
    llAbsPhaseMark = ROUND(fAbsPhaseMark*fMulti);
    llAbsPhase = ROUND(fAbsPhase*fMulti);
    llRange = ROUND(fRange*fMulti);
    llMin = ROUND(fMin*fMulti);
    llMax = ROUND(fMax*fMulti);

    llResult = ((llAbsPhase - llAbsPhaseMark - llMin)%llRange + llRange) % llRange + llMin ;
    if (llResult == llMin)
    {
        llResult += llRange;
    }
    return llResult/fMulti;
} 

//extern float DSM_GetRelativePhase( float fAbsPhaseMark, float fAbsPhase, float fMin /*= -180.0f*/, float fMax /*= 180.0f*/ )
//{
//    ASSERT(fMin < fMax);
//    float fRange = 0.0f;
//    fRange = fMax - fMin;
//    if (fAbsPhase - fAbsPhaseMark <= fMin)
//    {
//        return fRange + (fAbsPhase-fAbsPhaseMark);
//    }
//    else if (fAbsPhase - fAbsPhaseMark > fMax)
//    {
//        return (fAbsPhase-fAbsPhaseMark) - fRange;
//    }
//    else
//    {
//        return fAbsPhase - fAbsPhaseMark;
//    }
//}

/**
 *      判断路径是否在SD卡下
 *
 *  \param[in]
 *      csPath          待判断的路径
 *  \return
 *      bool            在SD下返回true，不在SD路径下返回false
 */
extern bool DSM_PathInSD(CString csPath)
{
    CString csSDPath;

    // 获取SD驱动器路径
    if(!DSM_GetSDDriver(csSDPath))
    {
        return false;
    }

    csPath.MakeLower();
    csSDPath.MakeLower();

    int nFind = csPath.Find(csSDPath);

    // 非SD卡目录
    if(nFind == -1 || nFind != 0)
    {
        return false;
    }

    return true;
}


/**
 *      判断指定路径是否存在
 *
 * \param[in]
 *      csPath          探测指定的路径是否存在
 *  \return
 *      bool            目录存在返回true，否则返回false
 */
extern bool DSM_SDPathIsExist( CString csPath)
{
    if(DSM_PathInSD(csPath))
    {
        DWORD dwAttri = GetFileAttributes(csPath);

        // 判断目录属性，SD卡等临时存储器的目录属性还有FILE_ATTRIBUTE_TEMPORARY
        if(dwAttri != 0xFFFFFFFF && (FILE_ATTRIBUTE_DIRECTORY & dwAttri) == FILE_ATTRIBUTE_DIRECTORY)
        {
            return true;
        }
    }

    return false;
}

/**
 *      创建目录
 *
 *  \param[in]
 *      csPath          待创建目录路径
 *  \return
 *      bool            创建目录成功返回true，失败返回false
 */
extern bool DSM_CreatePath( CString csPath )
{
    DWORD dwAttri = GetFileAttributes(csPath);

    // 判断目录是否已存在，SD卡等临时存储器的目录属性还有FILE_ATTRIBUTE_TEMPORARY
    if(dwAttri != 0xFFFFFFFF && (FILE_ATTRIBUTE_DIRECTORY & dwAttri) == FILE_ATTRIBUTE_DIRECTORY)
    {
        return true;
    }

    CString csDirect(csPath);
    CList<CString, LPCTSTR> lstDirect;

    // 尾部添加 L'\\'
    if(csDirect.GetAt(csDirect.GetLength() - 1) != L'\\')
    {
        csDirect += L"\\";
    }

    // 分离每一级目录字符串
    int nBegin = csDirect.Find(L'\\', 0);
    int nEnd;

    while(nBegin >= 0 && nBegin < csPath.GetLength())
    {
        nEnd = csDirect.Find(L'\\', nBegin + 1);

        if(nEnd > nBegin)
        {
            lstDirect.AddTail(csDirect.Mid(nBegin, nEnd - nBegin));
        }

        nBegin = nEnd;
    }

    // 逐级创建目录
    CString csCreatePath(L"");
    POSITION pos = lstDirect.GetHeadPosition();
    while(pos)
    {
        csCreatePath += lstDirect.GetNext(pos);

        dwAttri = GetFileAttributes(csCreatePath);

        // 目录存在则跳过
        if(dwAttri != 0xFFFFFFFF && (FILE_ATTRIBUTE_DIRECTORY & dwAttri) == FILE_ATTRIBUTE_DIRECTORY)
        {
            continue;
        }

        // 创建目录
        if(!CreateDirectory(csCreatePath, NULL))
        {
            return false;
        }
    }

    return true;
}



/**
 *      创建SD卡下目录
 *
 *  \param[in]
 *      csPath          待创建目录路径
 *  \return
 *      bool            创建目录成功返回true，失败返回false
 */
extern bool DSM_CreateSDPath( CString csPath )
{
    // 判断路径是否在SD卡下
    if(DSM_PathInSD(csPath))
    {
        // 目录不存在则创建
        if(!DSM_SDPathIsExist(csPath))
        {
            // 创建文件夹
            return DSM_CreatePath(csPath);
        }

        return true;
    }

    return false;
}

/**
 *      获取一个可用的SD卡路径
 *
 *  \param[out]
 *      rcsSDPath       SD卡的驱动器目录
 *  \return
 *      bool            获取SD卡目录成功返回true，失败返回false
 */
extern bool DSM_GetSDDriver( CString& rcsSDPath )
{
    if (CStatusMgr::GetSD() == DSM_STATU_DISCONNECT_SD) {
        TRACE(L"dsm: statusmgr: Open SD GIO Failed\n");
        return false;
    }

#if !SU_FEI
    CString csSD(L"\\Storage Card*");
#else
	CString csSD(L"\\FlashDisk2*");
#endif

    WIN32_FIND_DATA findFileData;

    // 枚举SD卡驱动器名
    HANDLE hFind        = FindFirstFile(csSD, &findFileData);

    if(hFind == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    do
    {
        // FILE_ATTRIBUTE_TEMPORARY | FILE_ATTRIBUTE_DIRECTORY
        if((findFileData.dwFileAttributes & 0x110) == 0x110)
        {
            rcsSDPath = findFileData.cFileName;
            rcsSDPath = L"\\" + rcsSDPath;
            FindClose(hFind);
            return true;
        }
    }while(::FindNextFile(hFind, &findFileData));

    FindClose(hFind);
    return false;
}

/**
 *      获取运行时配置目录
 *
 *  \return
 *      CString     返回运行时配置文件目录
 */
extern const CString DSM_GetRuntimeDir()
{
    static CString csRuntime(L"");

    if(csRuntime.IsEmpty())
    {
        csRuntime = DSM_GetAppDir();

        if(csRuntime.GetAt(csRuntime.GetLength() - 1) == L'\\')
        {
            csRuntime.SetAt(csRuntime.GetLength() - 1, 0);
        }

        csRuntime = csRuntime.Left(csRuntime.ReverseFind(L'\\') + 1) + L"dsm_usercfg\\";

        if(!DSM_CreatePath(csRuntime))
        {
            RETAILMSG(true, (L"dsm: failed to create runtime folder\n"));
        }
    }

    return csRuntime;
}


/**
 *      判断指定文件是否存在
 *
 *  \param[in]
 *      rcsFile         文件路径
 *  \return
 *      bool            文件存在返回true，不存在返回false
 */
extern bool DSM_IsFileExist( const CString& rcsFile )
{
    DWORD dwAttr = GetFileAttributes(rcsFile);

    if(dwAttr == 0xFFFFFFFF || (dwAttr & FILE_ATTRIBUTE_ARCHIVE) != FILE_ATTRIBUTE_ARCHIVE)
    {
        return false;
    }

    return true;
}

/**
 *      设置顶层子窗口焦点
 */
extern void DSM_SetZFocus( HWND hWnd )
{
    CWnd* pWnd = CWnd::FromHandle(hWnd);

    if(pWnd && pWnd->GetSafeHwnd() && pWnd->GetTopWindow())
    {
        pWnd->GetTopWindow()->SetFocus();
    }
}

