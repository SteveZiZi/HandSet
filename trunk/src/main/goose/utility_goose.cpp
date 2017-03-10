/**
 * \file
 *      utility_goose.cpp
 *
 * \brief
 *      goose业务常用方法实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/7/5
 */


#include "stdafx.h"
#include "utility_goose.h"

#include "src/main/resourcestr.h"
#include "src/main/utils/utility.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/service/datalayer/dsmdatalayer.h"

extern void DSM_GetGooseDataInfo( GOOSE_DATA* pData, CString& csType, CString& csValue )
{
    ASSERT(pData);

    UINT dwType;

    switch(pData->type)
    {
    case GSEDT_BOOLEAN:
        dwType = DSM_STR_FRAME_DTYPE_BOOLEAN;
        if(((GOOSE_BOOLEAN*)pData)->boolVal)
        {
            csValue = "TRUE";
        }
        else
        {
            csValue = "FALSE";
        }
        break;

    case GSEDT_BIT_STR:
        {
            dwType = DSM_STR_FRAME_DTYPE_BITSTR;
            GOOSE_BIT_STRING* pBS = (GOOSE_BIT_STRING*)pData;

            int nStrSize    = pBS->strSize;
            int nByteSize   = sizeof(*pBS->bitStr) * 8;
            int nEffect     = nStrSize * 8 - pBS->padding;

            CString csBS;
            for(int i = 0; i < nStrSize; i++)
            {
                uint8_t* pBytes = pBS->bitStr + i;

                for(int j = nByteSize - 1; j >= 0; j--)
                {
                    if((i + 1) * 8 - j <= nEffect)
                    {
                        // 高位-》
                        csBS += ((*pBytes >> j) & 0x01) == 0x01 ? L"1" : L"0";
                    }
                    else
                    {
                        csBS += L"#";
                    }
                }

                csBS += L" ";
            }

            // 解释补位信息，双点的init、on、off、bad(0123)
            if(pBS->strSize == 1 && pBS->padding == 6)
            {
                int val = *pBS->bitStr >> 6;

                switch(val)
                {
                    // init
                case 0x00:
                    csValue.Format(L"%s(%s)", csBS, dsm_stringMgr::Get(DSM_STR_MANUAL_D_POINT_INIT));
                    break;

                    // off
                case 0x01:
                    csValue.Format(L"%s(%s)", csBS, dsm_stringMgr::Get(DSM_STR_MANUAL_D_POINT_OFF));
                    break;

                    // on
                case 0x02:
                    csValue.Format(L"%s(%s)", csBS, dsm_stringMgr::Get(DSM_STR_MANUAL_D_POINT_ON));
                    break;

                    // bad
                case 0x03:
                    csValue.Format(L"%s(%s)", csBS, dsm_stringMgr::Get(DSM_STR_MANUAL_D_POINT_BAD));
                    break;

                default:
                    csValue.Format(L"%s(%u)", csBS, pBS->padding);
                    break;
                }
            }
            else
            {
                // 增加补位信息
                csValue.Format(L"%s(%u)", csBS, pBS->padding);
            }
        }
        break;

    case GSEDT_INT8:
        dwType = DSM_STR_TYPE_DATA_INT8;
        csValue.Format(L"%d", ((GOOSE_INT8*)pData)->datVal);
        break;

    case GSEDT_INT16:
        dwType = DSM_STR_TYPE_DATA_INT16;
        csValue.Format(L"%hd", ((GOOSE_INT16*)pData)->datVal);
        break;

    case GSEDT_INT24:
        dwType = DSM_STR_TYPE_DATA_INT24;
        csValue.Format(L"%d", ((GOOSE_INT24*)pData)->datVal);
        break;

    case GSEDT_INT32:
        dwType = DSM_STR_TYPE_DATA_INT32;
        csValue.Format(L"%d", ((GOOSE_INT32*)pData)->datVal);
        break;

    case GSEDT_UINT8:
        dwType = DSM_STR_TYPE_DATA_UINT8;
        csValue.Format(L"%u", ((GOOSE_UINT8*)pData)->datVal);
        break;

    case GSEDT_UINT16:
        dwType = DSM_STR_TYPE_DATA_UINT16;
        csValue.Format(L"%hu", ((GOOSE_UINT16*)pData)->datVal);
        break;

    case GSEDT_UINT24:
        dwType = DSM_STR_TYPE_DATA_UINT24;
        csValue.Format(L"%u", ((GOOSE_UINT24*)pData)->datVal);
        break;

    case GSEDT_UINT32:
        dwType = DSM_STR_TYPE_DATA_UINT32;
        csValue.Format(L"%u", ((GOOSE_UINT32*)pData)->datVal);
        break;

    case GSEDT_FLOAT:
        dwType = DSM_STR_FRAME_DTYPE_FLOAT;
        csValue.Format(L"%f", ((GOOSE_FLOAT*)pData)->datVal);
        break;

    case GSEDT_OCTET_STR:
        {
            dwType = DSM_STR_FRAME_DTYPE_OCTETSTR;
            GOOSE_OCTET_STRING* pOStr = (GOOSE_OCTET_STRING*)pData;

            int nStrSize = pOStr->strSize;

            CString csOChar;

            for(int i = 0; i < nStrSize; i++)
            {
                csOChar.Format(L"%X", *(pOStr->octetStr+i));

                if(i % 2 == 0 && i != 0)
                {
                    csValue += L" " + csOChar;
                }
                else
                {
                    csValue += csOChar;
                }
            }
        }
        break;

    case GSEDT_VISIBLE_STR:
        {
            dwType = DSM_STR_FRAME_DTYPE_VISIBLESTR;
            GOOSE_BIT_STRING* pBStr = (GOOSE_BIT_STRING*)pData;

            char* p = new char[pBStr->strSize + 1];
            memset((void*)p, 0, pBStr->strSize);
            memcpy(p, pBStr->bitStr, pBStr->strSize);

            csValue = CString(p);
            delete []p;
        }
        break;

    case GSEDT_UTC_TIME:
        {
            dwType = DSM_STR_TYPE_DATA_TIMETICK;

            GOOSE_UTC_TIME* pTime = (GOOSE_UTC_TIME*)pData;

            UTC_TIME_t ut = {pTime->t_s, pTime->t_ns / 1000};
            csValue = DM_GetSystemTimeStrFromUTC(ut);
        }
        break;

    case GSEDT_STRUCT:
        dwType  = DSM_STR_TYPE_DATA_STRUCT;
        csValue = L"";
        break;

    case GSEDT_UNKNOW:
        {
            dwType = DSM_STR_TYPE_DATA_UNKNOWN;
            // format(L"%d %X%X %X%X", tag, xx xx)..
            GOOSE_UNKNOW* pUn = (GOOSE_UNKNOW*)pData;

            // tag值
            csValue.Format(L"%d", pUn->gooseTag);

            CString csChar;
            int nStrSize = pUn->datLen;

            for(int i = 0; i < nStrSize; i++)
            {
                csChar.Format(L"%X", *(pUn->datVal+i));

                if(i % 2 == 0)
                {
                    csValue += L" " + csChar;
                }
                else
                {
                    csValue += csChar;
                }
            }
        }
        break;

    default:
        dwType = DSM_STR_FRAME_DTYPE_UNKNOWN;
        csValue = L"UNKNOWN";
        ASSERT(FALSE);
        break;
    }

    csType = dsm_stringMgr::Get(dwType);
}


/**
 *      根据通道类型获取通道关联的数据值与类型
 *
 *  \param[in]
 *      pChannel        goose通道指针
 *  \param[out]
 *      csType          goose通道数据类型
 *  \param[out]
 *      csValue         goose通道数据值
 */
extern void DSM_GetGooseDataInfo( CGooseChannel* pChannel, CString& csType, CString& csValue )
{
    ASSERT(pChannel);

    UINT dwType         = DSM_STR_TYPE_DATA_UNKNOWN;
    GOOSE_DATA* pData   = pChannel->GetChannelData();
    ASSERT(pData);

    GOOSE_DATA_VARIANT var(pData);

    // 2013/8/8 增加判断channel 类型与data类型是否有对应关系
    bool bUnknown = false;

    switch(pChannel->GetChannelType())
    {
    case CHANNEL_TYPE_GOOSE_TIME:
        if(pData->type != GSEDT_UTC_TIME)
        {
            bUnknown = true;
            break;
        }

        dwType  = DSM_STR_TYPE_DATA_TIMETICK;
        {
            UTC_TIME_t ut = {var.m_stUtcTime.t_s, var.m_stUtcTime.t_ns / 1000};
            csValue = DM_GetSystemTimeStrFromUTC(ut);
        }
        break;

    case CHANNEL_TYPE_GOOSE_POINT:
        if(pData->type != GSEDT_BIT_STR)
        {
            bUnknown = true;
            break;
        }

        dwType  = DSM_STR_TYPE_DATA_POINT;
        csValue = var.m_stBoolean.boolVal ? dsm_stringMgr::Get(DSM_STR_MANUAL_POINT_ON) : dsm_stringMgr::Get(DSM_STR_MANUAL_POINT_OFF);
        break;

    case CHANNEL_TYPE_GOOSE_DOUBLE_POINT:
        if(pData->type != GSEDT_BIT_STR)
        {
            bUnknown = true;
            break;
        }

        dwType  = DSM_STR_TYPE_DATA_DUALPOINT;
        csValue = var.m_stBitString.GetString().c_str();
        break;

    case CHANNEL_TYPE_GOOSE_QUALITY:
        if(pData->type != GSEDT_BIT_STR)
        {
            bUnknown = true;
            break;
        }

        dwType  = DSM_STR_TYPE_DATA_QUALITY;
        csValue = var.m_stBitString.GetString().c_str();
        break;

    case CHANNEL_TYPE_GOOSE_FLOAT:
        if(pData->type != GSEDT_FLOAT)
        {
            bUnknown = true;
            break;
        }

        dwType  = DSM_STR_TYPE_DATA_FLOAT;
        csValue.Format(L"%f", var.m_stFloat.datVal);
        break;

    case CHANNEL_TYPE_GOOSE_STRUCT:
        if(pData->type != GSEDT_STRUCT)
        {
            bUnknown = true;
            break;
        }

        dwType  = DSM_STR_TYPE_DATA_STRUCT;
        csValue = L"";
        break;

    case CHANNEL_TYPE_GOOSE_INT8:
        if(pData->type != GSEDT_INT8)
        {
            bUnknown = true;
            break;
        }

        dwType  = DSM_STR_TYPE_DATA_INT8;
        csValue.Format(L"%d", var.m_stInt8.datVal);
        break;

    case CHANNEL_TYPE_GOOSE_INT16:
        if(pData->type != GSEDT_INT16)
        {
            bUnknown = true;
            break;
        }

        dwType  = DSM_STR_TYPE_DATA_INT16;
        csValue.Format(L"%hd", var.m_stInt16.datVal);
        break;

    case CHANNEL_TYPE_GOOSE_INT24:
        if(pData->type != GSEDT_INT24)
        {
            bUnknown = true;
            break;
        }

        dwType  = DSM_STR_TYPE_DATA_INT24;
        csValue.Format(L"%d", var.m_stInt24.datVal);
        break;

    case CHANNEL_TYPE_GOOSE_INT32:
        if(pData->type != GSEDT_INT32)
        {
            bUnknown = true;
            break;
        }

        dwType  = DSM_STR_TYPE_DATA_INT32;
        csValue.Format(L"%d", var.m_stInt32.datVal);
        break;

    case CHANNEL_TYPE_GOOSE_UINT8:
        if(pData->type != GSEDT_UINT8)
        {
            bUnknown = true;
            break;
        }

        dwType  = DSM_STR_TYPE_DATA_UINT8;
        csValue.Format(L"%u", var.m_stUInt8.datVal);
        break;

    case CHANNEL_TYPE_GOOSE_UINT16:
        if(pData->type != GSEDT_UINT16)
        {
            bUnknown = true;
            break;
        }

        dwType  = DSM_STR_TYPE_DATA_UINT16;
        csValue.Format(L"%hu", var.m_stUInt16.datVal);
        break;

    case CHANNEL_TYPE_GOOSE_UINT24:
        if(pData->type != GSEDT_UINT24)
        {
            bUnknown = true;
            break;
        }

        dwType  = DSM_STR_TYPE_DATA_UINT24;
        csValue.Format(L"%u", var.m_stUInt24.datVal);
        break;

    case CHANNEL_TYPE_GOOSE_UINT32:
        if(pData->type != GSEDT_UINT32)
        {
            bUnknown = true;
            break;
        }

        dwType  = DSM_STR_TYPE_DATA_UINT32;
        csValue.Format(L"%u", var.m_stUInt32.datVal);
        break;

        // 2013/8/8 添加通道是未知类型时，对其包含的goose data进行判断
    case CHANNEL_TYPE_UNKOWN:
        bUnknown = true;
        break;

    default:
        csValue = L"";
        break;
    }

    csType = dsm_stringMgr::Get(dwType);

    // channel类型与data类型不同，按照data的类型获取数值
    if(bUnknown)
    {
        DSM_GetGooseDataInfo(pData,csType, csValue);
    }
}

/**
 *      计算一个合理的相对时间显示值
 *
 *  \param[in]
 *      nInt1           被减数整数部分
 *  \param[in]
 *      nDec1           被减数小数部分
 *  \param[in]
 *      nInt2           减数整数部分
 *  \param[in]
 *      nDec2           减数小数部分
 *  \param[in]
 *      nT              小数部分精度
 *  \return
 *      CString         显示字符串
 */
extern CString DSM_GetDValue( uint32_t nInt1, uint32_t nDec1, uint32_t nInt2, uint32_t nDec2, int nT /* = 9 */ )
{
    if(nT > 9 || nT <= 0)
    {
        nT = 9;
    }

    CString csVal(L"");
    CString csFmt, csFmtN;

    csFmt.Format(L"%%u.%%0%du", nT);
    csFmtN.Format(L"-%%u.%%0%du", nT);

    uint32_t nXub = 1;

    while(nT--)
    {
        nXub *= 10;
    }

    // 结果为正数
    if(nInt1 > nInt2)
    {
        if(nDec1 >= nDec2)
        {
            csVal.Format(csFmt, nInt1 - nInt2, nDec1 - nDec2);
        }
        else
        {
            nInt1--;
            csVal.Format(csFmt, nInt1 - nInt2, nXub - nDec2 + nDec1);
        }
    }
    // 有可能是负数
    else if(nInt1 == nInt2)
    {
        if(nDec1 >= nDec2)
        {
            csVal.Format(csFmt, nInt1 - nInt2, nDec1 - nDec2);
        }
        // 负数
        else
        {
            csVal.Format(csFmtN, nInt1 - nInt2, nDec2 - nDec1);
        }
    }
    else
    {
        if(nDec2 >= nDec1)
        {
            csVal.Format(csFmtN, nInt2 - nInt1, nDec2 - nDec1);
        }
        else
        {
            nInt2--;
            csVal.Format(csFmtN, nInt2 - nInt1, nXub - nDec1 + nDec2);
        }
    }

    return csVal;
}
