/**
 * \file
 *      dsm_utility.cpp
 *
 * \brief
 *      dsm��������ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
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

#define ROUND(x)    static_cast<int>((x)>=0?((x)+0.5):((x)-0.5))      ///< x��������

/**
 *      ��ȡģ��·���ַ���
 *
 * \return
 *      CString     ģ���·���ַ���
 * \note
 *      ����x:\aaaaa\bbbb\ccc.exe�򷵻�x:\aaaaa\bbbb\
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
            // �ж�С����
            if(L'.' == csStr.GetAt(i) && !bDot)
            {
                bDot = true;
                nDot = i;       // ��¼.λ��
                continue;
            }

            // �жϷ���
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

    // �ж���������
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
            // �ж�С����
            if(L'.' == csStr.GetAt(i) && !bDot)
            {
                bDot = true;
                nDot = i;       // ��¼.λ��
                continue;
            }

            // �жϷ���
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

    // �ж���������
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
        // ��ȡgoose��91Э��Ŀ��ƿ�����  appid : port
    case FRAME_TYPE_NET_9_1:
    case FRAME_TYPE_NET_GOOSE:
        {
            CNetDetectItem* pNet = dynamic_cast<CNetDetectItem*>(pCtrlBlk);
            ASSERT(pNet);

            csInfo.Format(L"0x%04X : %s", pNet->appId, DSM_GetPortDesc(pNet->enPortName));
        }
        break;

        // ��ȡ92Э��Ŀ��ƿ�����  appid : desc|port
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

        // ��ȡFT3Э��Ŀ��ƿ�����  ldname : port
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

    // ������ϢΪ�գ���ȥMAC��Ϣ
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
 *      �ж�·���Ƿ���SD����
 *
 *  \param[in]
 *      csPath          ���жϵ�·��
 *  \return
 *      bool            ��SD�·���true������SD·���·���false
 */
extern bool DSM_PathInSD(CString csPath)
{
    CString csSDPath;

    // ��ȡSD������·��
    if(!DSM_GetSDDriver(csSDPath))
    {
        return false;
    }

    csPath.MakeLower();
    csSDPath.MakeLower();

    int nFind = csPath.Find(csSDPath);

    // ��SD��Ŀ¼
    if(nFind == -1 || nFind != 0)
    {
        return false;
    }

    return true;
}


/**
 *      �ж�ָ��·���Ƿ����
 *
 * \param[in]
 *      csPath          ̽��ָ����·���Ƿ����
 *  \return
 *      bool            Ŀ¼���ڷ���true�����򷵻�false
 */
extern bool DSM_SDPathIsExist( CString csPath)
{
    if(DSM_PathInSD(csPath))
    {
        DWORD dwAttri = GetFileAttributes(csPath);

        // �ж�Ŀ¼���ԣ�SD������ʱ�洢����Ŀ¼���Ի���FILE_ATTRIBUTE_TEMPORARY
        if(dwAttri != 0xFFFFFFFF && (FILE_ATTRIBUTE_DIRECTORY & dwAttri) == FILE_ATTRIBUTE_DIRECTORY)
        {
            return true;
        }
    }

    return false;
}

/**
 *      ����Ŀ¼
 *
 *  \param[in]
 *      csPath          ������Ŀ¼·��
 *  \return
 *      bool            ����Ŀ¼�ɹ�����true��ʧ�ܷ���false
 */
extern bool DSM_CreatePath( CString csPath )
{
    DWORD dwAttri = GetFileAttributes(csPath);

    // �ж�Ŀ¼�Ƿ��Ѵ��ڣ�SD������ʱ�洢����Ŀ¼���Ի���FILE_ATTRIBUTE_TEMPORARY
    if(dwAttri != 0xFFFFFFFF && (FILE_ATTRIBUTE_DIRECTORY & dwAttri) == FILE_ATTRIBUTE_DIRECTORY)
    {
        return true;
    }

    CString csDirect(csPath);
    CList<CString, LPCTSTR> lstDirect;

    // β����� L'\\'
    if(csDirect.GetAt(csDirect.GetLength() - 1) != L'\\')
    {
        csDirect += L"\\";
    }

    // ����ÿһ��Ŀ¼�ַ���
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

    // �𼶴���Ŀ¼
    CString csCreatePath(L"");
    POSITION pos = lstDirect.GetHeadPosition();
    while(pos)
    {
        csCreatePath += lstDirect.GetNext(pos);

        dwAttri = GetFileAttributes(csCreatePath);

        // Ŀ¼����������
        if(dwAttri != 0xFFFFFFFF && (FILE_ATTRIBUTE_DIRECTORY & dwAttri) == FILE_ATTRIBUTE_DIRECTORY)
        {
            continue;
        }

        // ����Ŀ¼
        if(!CreateDirectory(csCreatePath, NULL))
        {
            return false;
        }
    }

    return true;
}



/**
 *      ����SD����Ŀ¼
 *
 *  \param[in]
 *      csPath          ������Ŀ¼·��
 *  \return
 *      bool            ����Ŀ¼�ɹ�����true��ʧ�ܷ���false
 */
extern bool DSM_CreateSDPath( CString csPath )
{
    // �ж�·���Ƿ���SD����
    if(DSM_PathInSD(csPath))
    {
        // Ŀ¼�������򴴽�
        if(!DSM_SDPathIsExist(csPath))
        {
            // �����ļ���
            return DSM_CreatePath(csPath);
        }

        return true;
    }

    return false;
}

/**
 *      ��ȡһ�����õ�SD��·��
 *
 *  \param[out]
 *      rcsSDPath       SD����������Ŀ¼
 *  \return
 *      bool            ��ȡSD��Ŀ¼�ɹ�����true��ʧ�ܷ���false
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

    // ö��SD����������
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
 *      ��ȡ����ʱ����Ŀ¼
 *
 *  \return
 *      CString     ��������ʱ�����ļ�Ŀ¼
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
 *      �ж�ָ���ļ��Ƿ����
 *
 *  \param[in]
 *      rcsFile         �ļ�·��
 *  \return
 *      bool            �ļ����ڷ���true�������ڷ���false
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
 *      ���ö����Ӵ��ڽ���
 */
extern void DSM_SetZFocus( HWND hWnd )
{
    CWnd* pWnd = CWnd::FromHandle(hWnd);

    if(pWnd && pWnd->GetSafeHwnd() && pWnd->GetTopWindow())
    {
        pWnd->GetTopWindow()->SetFocus();
    }
}

