/**
 * \file
 *      decode.cpp
 *
 * \brief
 *      解析报文数据的实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/11/18
 */

#include "stdafx.h"

#include "src/main/common.h"
#include "src/main/resourcestr.h"
#include "src/main/utils/utility.h"
#include "src/main/goose/utility_goose.h"
#include "src/service/datalayer/dsmdataLayer.h"
#include "..\..\utils\MessageCodec\msg_impl\goose_msg_impl.h"
#include "..\..\utils\MessageCodec\msg_impl\sav91_msg_impl.h"
#include "..\..\utils\MessageCodec\msg_impl\sav92_msg_impl.h"

void DSM_Decode92( CTreeV& rTree, Sav92Msg* pMsg92, enumFrameType type, const uint8_t* pBuff, int bufflen )
{
    if(!pMsg92 || type != FRAME_TYPE_NET_9_2)
    {
        return;
    }

    if(pMsg92->Decode(pBuff, bufflen) == 0)
    {
        DSM_DEBUG(L"dsm: _decode92: Decode failed");
        ASSERT(FALSE);
        return;
    }

    // 先不优化
    rTree.DeleteAllTreeItems();

    //////////////////////////////////////////////////////////////////////////

    // 以太网帧头
    ETHER_II_HEADER head;
    IEEE802_1Q_VLAN_INFO vlan;
    pMsg92->GetEthernetHeader(head);
    pMsg92->Get8021QVlanInfo(vlan);

    CString csDes, csSrc, csEthType, csVlan, csFrameType;

    csDes.Format(L"%02X-%02X-%02X-%02X-%02X-%02X", head.desMac[0], head.desMac[1], head.desMac[2], head.desMac[3], head.desMac[4], head.desMac[5]);
    csSrc.Format(L"%02X-%02X-%02X-%02X-%02X-%02X", head.srcMac[0], head.srcMac[1], head.srcMac[2], head.srcMac[3], head.srcMac[4], head.srcMac[5]);

#if ZY_PRJ 
    csEthType.Format(L"802.1Q VLAN (0x%04X)", head.etherType);
    csVlan.Format(L"PRI: %u  CFI: %u  VID: %u", vlan.tci.userPriority, vlan.tci.cfi, vlan.tci.vlanID);

    if(vlan.etherType == 0x88BA)
    {
        csFrameType.Format(L"0x%04X: %s", vlan.etherType, dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_PROTO_SV));
    }
    else
    {
        csFrameType.Format(L"0x%04X", vlan.etherType);
    }

    LBT_HTREEITEM hEthHead = rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_ETH_FRAME_HEAD));
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_TARGET_ADDR), hEthHead), 1, csDes);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_SOURCE_ADDR), hEthHead), 1, csSrc);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_ETH_MSG_TYPE), hEthHead), 1, csEthType);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_VLAN), hEthHead), 1, csVlan);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_FRAME_TYPE), hEthHead), 1, csFrameType);
#else
	Sav92MsgImpl *Sav92Msg = dynamic_cast<Sav92MsgImpl*>(pMsg92);
	if (Sav92Msg->m_bHasVlanFlag)
	{
		csEthType.Format(L"802.1Q VLAN (0x%04X)", head.etherType);
		csVlan.Format(L"PRI: %u  CFI: %u  VID: %u", vlan.tci.userPriority, vlan.tci.cfi, vlan.tci.vlanID);

		if(vlan.etherType == 0x88BA)
		{
			csFrameType.Format(L"0x%04X: %s", vlan.etherType, dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_PROTO_SV));
		}
		else
		{
			csFrameType.Format(L"0x%04X", vlan.etherType);
		}

		LBT_HTREEITEM hEthHead = rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_ETH_FRAME_HEAD));
		rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_TARGET_ADDR), hEthHead), 1, csDes);
		rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_SOURCE_ADDR), hEthHead), 1, csSrc);
		rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_ETH_MSG_TYPE), hEthHead), 1, csEthType);
		rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_VLAN), hEthHead), 1, csVlan);
		rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_FRAME_TYPE), hEthHead), 1, csFrameType);
	}
	else
	{
		csEthType.Format(L"IEC-SV (0x%04X)", head.etherType);
		 

		LBT_HTREEITEM hEthHead = rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_ETH_FRAME_HEAD));
		rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_TARGET_ADDR), hEthHead), 1, csDes);
		rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_SOURCE_ADDR), hEthHead), 1, csSrc);
		rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_ETH_MSG_TYPE), hEthHead), 1, csEthType); 
	}
#endif
    //////////////////////////////////////////////////////////////////////////

    // 92报文头

    IEC8802_3_PDU_PUBLIC iec8802_3;
    pMsg92->GetIEC8802Info(iec8802_3);
    LBT_HTREEITEM hIEC = rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_FRAME_SV92));

    CString csAppID, csAPDULen, csReserve1, csReserve2;

    csAppID.Format(L"0x%04X", iec8802_3.appID);
    csAPDULen.Format(L"%u", iec8802_3.len);
    csReserve1.Format(L"0x%04X", iec8802_3.reserved1);
    csReserve2.Format(L"0x%04X", iec8802_3.reserved2);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_APPID), hIEC), 1, csAppID);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_APDU_LEN), hIEC), 1, csAPDULen);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_RESERVE1), hIEC), 1, csReserve1);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_RESERVE2), hIEC), 1, csReserve2);

    //////////////////////////////////////////////////////////////////////////

    // PDU信息

    uint32_t nSequ      = 0;
    size_t nLen         = pMsg92->GetSavPduLen();
    uint16_t nAsduNums  = pMsg92->GetNoASDU();

    LBT_HTREEITEM hPdu = rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_PDU_LEN));

    for(uint16_t i = 0; i < nAsduNums; i++)
    {
        nSequ += pMsg92->GetSeqOfDatLen(i);
    }

    CString csPduLen, csAsduNum, csSequNums;
    csPduLen.Format(L"%u", nLen);
    csAsduNum.Format(L"%u", nAsduNums);
    csSequNums.Format(L"%u", nSequ);

    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_PDU_LEN), hPdu), 1, csPduLen);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_ASDU_NUM), hPdu), 1, csAsduNum);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_SEQU_ASDU), hPdu), 1, csSequNums);

    //////////////////////////////////////////////////////////////////////////
    // asdu

    CString csAsdu, csText;
    SAV92_ASDU_PUBLIC asduinfo;
    SAV92_SAVPDU_OPTS asduopt;
    UTC_TIME_t utc;

    // 枚举ASDU
    for(size_t i = 0; i < nAsduNums; i++)
    {
        if(!pMsg92->GetASDUPublicInfo(i, asduinfo))
        {
            continue;
        }

        csAsdu.Format(L"%s[0x%04X]", dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_ASDU), i);
        LBT_HTREEITEM hAsdu = rTree.InsertTreeItem(csAsdu);

        csText = asduinfo.svIDStr;
        rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_SMVID), hAsdu), 1, csText);

        csText.Format(L"%u", asduinfo.smpCnt);
        rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_SAMPLECNT), hAsdu), 1, csText);

        csText.Format(L"%u", asduinfo.confRev);
        rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_CONFREV), hAsdu), 1, csText);

        csText.Format(L"%u", asduinfo.smpSynch);
        rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_SAMPLESYNC), hAsdu), 1, csText);

        // 增加datset,refrtm,smprate信息
        pMsg92->GetSavPduOpts(asduopt);

        if(asduopt.bDataSetOpt)
        {
            csText = asduinfo.datSetStr;
            rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_DATASET), hAsdu), 1, csText);
        }

        if(asduopt.bRefrTmOpt)
        {
            utc.uSecondSinceEpoch = asduinfo.refrTm.ts_s;
            utc.uFractionOfSecond = asduinfo.refrTm.ts_ns / 1000;
            csText.Format(L"%s q=0x%02X", DM_GetSystemTimeStrFromUTC(utc), asduinfo.refrTm.ts_q);
            rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_REFRTM), hAsdu), 1, csText);
        }

        if(asduopt.bSmpRateOpt)
        {
            csText.Format(L"%d", asduinfo.smpRate);
            rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_SMPRATE), hAsdu), 1, csText);
        }

        csText.Format(L"%u", pMsg92->GetSeqOfDatLen(i));
        LBT_HTREEITEM pChann = rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_SEQUENCEDATA), hAsdu);
        rTree.SetTreeItemText(pChann, 1, csText);

        // channel DM8000最多30个通道
        size_t nChannelNums = pMsg92->GetASDUSmpsNum(i);
        uint32_t* pSmvVal = new uint32_t[nChannelNums];
        uint32_t* pSmvQlt = new uint32_t[nChannelNums];
        ASSERT(pSmvVal);
        ASSERT(pSmvQlt);

        CString csVal;

        pMsg92->GetASDUSmps(i, pSmvVal, pSmvQlt, nChannelNums);

        for(size_t j = 0; j < nChannelNums; j++)
        {
            csText.Format(L"0x%04X", j);
            csVal.Format(L"v= 0x%08X, q=0x%08X", pSmvVal[j], pSmvQlt[j]);

            rTree.SetTreeItemText(rTree.InsertTreeItem(csText, pChann), 1, csVal);
        }

        // 频繁分配内存，需要优化
        delete []pSmvVal;
        delete []pSmvQlt;

        //SAV92_ASDU_PUBLIC 其他信息？？？
    }
}


void DSM_Decode91( CTreeV& rTree, Sav91Msg* pMsg91, enumFrameType type, const uint8_t* pBuff, int bufflen )
{
    if(!pMsg91 || type != FRAME_TYPE_NET_9_1)
    {
        return;
    }

    if(pMsg91->Decode(pBuff, bufflen) == 0)
    {
        DSM_DEBUG(L"dsm: _decode91: Decode failed\n");
        ASSERT(FALSE);
        return;
    }

    // 先不优化
    rTree.DeleteAllTreeItems();

    ETHER_II_HEADER head;
    pMsg91->GetEthernetHeader(head);
    IEEE802_1Q_VLAN_INFO vlan;
    pMsg91->Get8021QVlanInfo(vlan);

    CString csDes, csSrc, csEthType, csVlan, csFrameType;

    csDes.Format(L"%02X-%02X-%02X-%02X-%02X-%02X", head.desMac[0], head.desMac[1], head.desMac[2], head.desMac[3], head.desMac[4], head.desMac[5]);
    csSrc.Format(L"%02X-%02X-%02X-%02X-%02X-%02X", head.srcMac[0], head.srcMac[1], head.srcMac[2], head.srcMac[3], head.srcMac[4], head.srcMac[5]);

#if ZY_PRJ
    csEthType.Format(L"802.1Q VLAN (0x%04X)", head.etherType);
    csVlan.Format(L"PRI: %u  CFI: %u  VID: %u", vlan.tci.userPriority, vlan.tci.cfi, vlan.tci.vlanID);

    if(0x88BA == vlan.etherType)
    {
        csFrameType.Format(L"0x%04X: %s", vlan.etherType, dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_PROTO_SV));
    }
    else
    {
        csFrameType.Format(L"0x%04X", vlan.etherType);
    }

    // 插入以太网帧头
    LBT_HTREEITEM hEthHead = rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_ETH_FRAME_HEAD));
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_TARGET_ADDR), hEthHead), 1, csDes);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_SOURCE_ADDR), hEthHead), 1, csSrc);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_ETH_MSG_TYPE), hEthHead), 1, csEthType);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_VLAN), hEthHead), 1, csVlan);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_FRAME_TYPE), hEthHead), 1, csFrameType);
#else
	Sav91MsgImpl *Sav91Msg = dynamic_cast<Sav91MsgImpl*>(pMsg91);
	if (Sav91Msg->m_bHasVlanFlag)
	{
		csEthType.Format(L"802.1Q VLAN (0x%04X)", head.etherType);
		csVlan.Format(L"PRI: %u  CFI: %u  VID: %u", vlan.tci.userPriority, vlan.tci.cfi, vlan.tci.vlanID);

		if(0x88BA == vlan.etherType)
		{
			csFrameType.Format(L"0x%04X: %s", vlan.etherType, dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_PROTO_SV));
		}
		else
		{
			csFrameType.Format(L"0x%04X", vlan.etherType);
		}

		// 插入以太网帧头
		LBT_HTREEITEM hEthHead = rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_ETH_FRAME_HEAD));
		rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_TARGET_ADDR), hEthHead), 1, csDes);
		rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_SOURCE_ADDR), hEthHead), 1, csSrc);
		rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_ETH_MSG_TYPE), hEthHead), 1, csEthType);
		rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_VLAN), hEthHead), 1, csVlan);
		rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_FRAME_TYPE), hEthHead), 1, csFrameType);
	}
	else{
		csEthType.Format(L"IEC-SV (0x%04X)", head.etherType);
		 

		// 插入以太网帧头
		LBT_HTREEITEM hEthHead = rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_ETH_FRAME_HEAD));
		rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_TARGET_ADDR), hEthHead), 1, csDes);
		rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_SOURCE_ADDR), hEthHead), 1, csSrc);
		rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_ETH_MSG_TYPE), hEthHead), 1, csEthType); 
	}
#endif
    //////////////////////////////////////////////////////////////////////////
    // 报文头

    IEC8802_3_PDU_PUBLIC iec8802_3;
    pMsg91->GetIEC8802Info(iec8802_3);

    // 插入IEC61850 sv 91报文头
    LBT_HTREEITEM hIEC = rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_FRAME_SV91));

    CString csAppID, csAPDULen, csReserve1, csReserve2;

    csAppID.Format(L"0x%04X", iec8802_3.appID);
    csAPDULen.Format(L"%u", iec8802_3.len);
    csReserve1.Format(L"0x%04X", iec8802_3.reserved1);
    csReserve2.Format(L"0x%04X", iec8802_3.reserved2);

    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_APPID), hIEC), 1, csAppID);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_APDU_LEN), hIEC), 1, csAPDULen);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_RESERVE1), hIEC), 1, csReserve1);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_RESERVE2), hIEC), 1, csReserve2);

    //////////////////////////////////////////////////////////////////////////

    // PDU信息

    size_t nLen     = pMsg91->GetSavPduLen();     //< APDU长度？
    uint16_t nNums  = pMsg91->GetNoASDU();

    LBT_HTREEITEM hPdu = rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_PDU));

    CString csPduLen, csAsduNum;
    csPduLen.Format(L"%u", nLen);
    csAsduNum.Format(L"%u", nNums);

    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_PDU_LEN), hPdu), 1, csPduLen);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_ASDU_NUM), hPdu), 1, csAsduNum);

    CString csAsdu, csText;
    SAV91_ASDU_PUBLIC asduinfo;

    size_t nSmvNums = pMsg91->GetASDUDefaultSmpsNum();
    uint16_t* pSmvChanns = new uint16_t[nSmvNums];
    ASSERT(pSmvChanns);

    // 枚举ASDU
    for(size_t i = 0; i < nNums; i++)
    {
        // 插入public信息
        if(!pMsg91->GetASDUPublicInfo(i, asduinfo))
        {
            continue;
        }

        csAsdu.Format(L"%s[0x%04X]", dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_ASDU), i);
        LBT_HTREEITEM hAsdu = rTree.InsertTreeItem(csAsdu);

        csText.Format(L"%u", asduinfo.asduLen);
        rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_LENGTHASDU), hAsdu), 1, csText);

        csText.Format(L"0x%02X", asduinfo.lnName);
        rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_LNNAME), hAsdu), 1, csText);

        csText.Format(L"0x%02X", asduinfo.dataSetName);
        rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_DATESETNAME), hAsdu), 1, csText);

        csText.Format(L"0x%04X", asduinfo.ldName);
        rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_LDNAME), hAsdu), 1, csText);

        csText.Format(L"%u", asduinfo.phsArtg);
        rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_IEPHS), hAsdu), 1, csText);

        csText.Format(L"%u", asduinfo.neutArtg);
        rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_IEMID), hAsdu), 1, csText);

        csText.Format(L"%u", asduinfo.phsVrtg);
        rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_UEPHS), hAsdu), 1, csText);

        csText.Format(L"%u", asduinfo.delayTime);
        rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_DATETIME), hAsdu), 1, csText);

        // 获取asdu通道信息
        pMsg91->GetASDUSmps(i, pSmvChanns, nSmvNums);

        LBT_HTREEITEM pChann = NULL;

        CString csVal;

        // 枚举通道
        for(size_t j = 0; j < nSmvNums; j++)
        {
            if(0 == j)
            {
                pChann = rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_DATESET), hAsdu);
            }

            ASSERT(pChann);

            csText.Format(L"%s%03u", dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_DATAS), j);
            csVal.Format(L"0x%04X", pSmvChanns[j]);

            rTree.SetTreeItemText(rTree.InsertTreeItem(csText, pChann), 1, csVal);
        }

        csText.Format(L"0x%04X", asduinfo.statusWord1);
        rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_STATE1), hAsdu), 1, csText);

        csText.Format(L"0x%04X", asduinfo.statusWord2);
        rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_STATE2), hAsdu), 1, csText);

        csText.Format(L"%u", asduinfo.smpCnt);
        rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_SAMPLECNT), hAsdu), 1, csText);

        csText.Format(L"%u", asduinfo.smpRate);
        rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_SAMPLERATE), hAsdu), 1, csText);

        csText.Format(L"%u", asduinfo.confRev);
        rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_CONFREV), hAsdu), 1, csText);
    }

    delete []pSmvChanns;
}

void DSM_DecodeNanruiFT3( CTreeV& rTree, NanRuiFT3Msg* pMsgNanRui, enumFrameType type, const uint8_t* pBuff, int bufflen )
{
    if(!pMsgNanRui || type != FRAME_TYPE_FT3_NANRUI)
    {
        return;
    }

    if(pMsgNanRui->Decode((uint16_t*)pBuff, bufflen / 2) == 0)
    {
        DSM_DEBUG(L"dsm: decodeNanruiFT3: Decode failed\n");
        ASSERT(FALSE);
        return;
    }

    // 先不优化
    rTree.DeleteAllTreeItems();

    NANRUI_FT3_ASDU_PUBLIC info;
    if(!pMsgNanRui->GetASDUPublicInfo(info))
    {
        DSM_DEBUG(L"dsm: decodeNanruiFT3: GetASDUPublicInfo: error\n");
        ASSERT(FALSE);
        return;
    }

    CString csText;
    LBT_HTREEITEM hHead = rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_ASDU));

    csText.Format(L"%u", info.asduLen);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_LENGTHASDU), hHead), 1, csText);

    csText.Format(L"0x%02X", info.lnName);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_LNNAME), hHead), 1, csText);

    csText.Format(L"0x%02X", info.dataSetName);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_DATESETNAME), hHead), 1, csText);

    csText.Format(L"0x%04X", info.ldName);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_LDNAME), hHead), 1, csText);

    csText.Format(L"%u", info.phsArtg);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_IEPHS), hHead), 1, csText);

    csText.Format(L"%u", info.neutArtg);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_IEMID), hHead), 1, csText);

    csText.Format(L"%u", info.phsVrtg);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_UEPHS), hHead), 1, csText);

    csText.Format(L"%u", info.delayTime);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_DATETIME), hHead), 1, csText);

    csText.Format(L"0x%04X", info.reserved);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_RESERVE), hHead), 1, csText);

    LBT_HTREEITEM hDatas = rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_CHANDATA), hHead);

    // 枚举通道
    size_t nCounts = pMsgNanRui->GetASDUSmpsNum();

    uint16_t* pSmpData = new uint16_t[nCounts];

    nCounts = pMsgNanRui->GetASDUSmps(pSmpData, nCounts);

    CString csVal;
    for(size_t i = 0; i < nCounts; i++)
    {
        csText.Format(L"%s%03u", dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_DATAS), i);
        csVal.Format(L"0x%04X", pSmpData[i]);
        rTree.SetTreeItemText(rTree.InsertTreeItem(csText, hDatas), 1, csVal);
    }

    delete []pSmpData;

    csText.Format(L"0x%04X", info.statusWord1);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_STATE1), hHead), 1, csText);

    csText.Format(L"0x%04X", info.statusWord2);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_STATE2), hHead), 1, csText);

    csText.Format(L"%u", info.smpCnt);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_SAMPLECNT), hHead), 1, csText);

    csText.Format(L"%u", info.smpRate);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_SAMPLERATE), hHead), 1, csText);

    csText.Format(L"%u", info.confRev);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_CONFREV), hHead), 1, csText);
}

void DSM_DecodeGuowangFT3( CTreeV& rTree, GuoWangFT3Msg* pMsgGuoWang, enumFrameType type, const uint8_t* pBuff, int bufflen )
{
    if(!pMsgGuoWang || type != FRAME_TYPE_FT3_GUOWANG)
    {
        return;
    }

    if(pMsgGuoWang->Decode((uint16_t*)pBuff, bufflen / 2) == 0)
    {
        DSM_DEBUG(L"dsm: decodeGuoWangFT3: Decode failed\n");
        ASSERT(FALSE);
        return;
    }

    // 先不优化
    rTree.DeleteAllTreeItems();

    GUOWANG_FT3_ASDU_PUBLIC info;
    if(!pMsgGuoWang->GetASDUPublicInfo(info))
    {
        DSM_DEBUG(L"dsm: _decodeGuowangFT3: GetASDUPublicInfo error\n");
        ASSERT(FALSE);
        return;
    }

    CString csText;
    LBT_HTREEITEM hHead = rTree.InsertTreeItem(L"ASDU");

    csText.Format(L"%u", info.asduLen);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_LENGTHASDU), hHead), 1, csText);

    csText.Format(L"0x%02X", info.lnName);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_LNNAME), hHead), 1, csText);

    csText.Format(L"0x%02X", info.dataSetName);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_DATESETNAME), hHead), 1, csText);

    csText.Format(L"0x%04X", info.ldName);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_LDNAME), hHead), 1, csText);

    csText.Format(L"%u", info.phsArtg);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_IEPHS), hHead), 1, csText);

    csText.Format(L"%u", info.neutArtg);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_IEMID), hHead), 1, csText);

    csText.Format(L"%u", info.phsVrtg);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_UEPHS), hHead), 1, csText);

    csText.Format(L"%u", info.delayTime);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_DATETIME), hHead), 1, csText);

    csText.Format(L"%u", info.smpCnt);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_SAMPLECNT), hHead), 1, csText);

    LBT_HTREEITEM hDatas = rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_CHANDATA), hHead);

    // 枚举通道
    size_t nCounts = pMsgGuoWang->GetASDUSmpsNum();

    uint16_t* pSmpData = new uint16_t[nCounts];

    nCounts = pMsgGuoWang->GetASDUSmps(pSmpData, nCounts);

    CString csVal;
    for(size_t i = 0; i < nCounts; i++)
    {
        csText.Format(L"%s%03u", dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_DATAS), i);
        csVal.Format(L"0x%04X", pSmpData[i]);
        rTree.SetTreeItemText(rTree.InsertTreeItem(csText, hDatas), 1, csVal);
    }

    delete []pSmpData;

    csText.Format(L"0x%04X", info.statusWord1);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_STATE1), hHead), 1, csText);

    csText.Format(L"0x%04X", info.statusWord2);
    rTree.SetTreeItemText(rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_STATE2), hHead), 1, csText);
}


/**
 *      解码goose协议数据
 */
void DSM_DecodeGoose( CTreeV& rTree, GooseMsg* pGooseMsg, enumFrameType type, const uint8_t* pBuff, int bufflen )
{
    if(!pGooseMsg)
    {
        pGooseMsg = CreateGooseMsgImpl();
        ASSERT(pGooseMsg);
    }

    if(FRAME_TYPE_NET_GOOSE != type)
    {
        DSM_DEBUG(L"dsm: _decodeGoose: type not match %d\n", type);
        ASSERT(FALSE);
        return;
    }

    if(0 == pGooseMsg->Decode(pBuff, bufflen))
    {
        DSM_DEBUG(L"dsm: _decodeGoose: Decode failed\n");
        ASSERT(FALSE);
        return;
    }

    // 先不优化
    rTree.DeleteAllTreeItems();

    ETHER_II_HEADER head;
    IEEE802_1Q_VLAN_INFO vlan;
    pGooseMsg->GetEthernetHeader(head);
    pGooseMsg->Get8021QVlanInfo(vlan);

    // 格式化以太网帧头数据
    CString csDesMac, csSrcMac, csEthType, csVlan, csPackage;

    csDesMac.Format(L"%02X-%02X-%02X-%02X-%02X-%02X", 
        head.desMac[0], head.desMac[1], head.desMac[2], head.desMac[3], head.desMac[4], head.desMac[5]);

    csSrcMac.Format(L"%02X-%02X-%02X-%02X-%02X-%02X", 
        head.srcMac[0], head.srcMac[1], head.srcMac[2], head.srcMac[3], head.srcMac[4], head.srcMac[5]);

#if ZY_PRJ
    csEthType.Format(L"802.1Q VLAN (0x%04X)", head.etherType);

    csVlan.Format(L"PRI: %u  CFI: %u  VID: %u", vlan.tci.userPriority, vlan.tci.cfi, vlan.tci.vlanID);

    if(0x88B8 == vlan.etherType)
    {
        csPackage.Format(L"IEC-GOOSE (0x%04X)", vlan.etherType);
    }
    else
    {
        csPackage.Format(L"0x%04X", vlan.etherType);
    }

    // 插入以太网帧头
    LBT_HTREEITEM hEthHead = rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_ETHERNET));

    rTree.SetTreeItemText(
        rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_DES_MAC), hEthHead), 1, csDesMac);

    rTree.SetTreeItemText(
        rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_SRC_MAC), hEthHead), 1, csSrcMac);

    rTree.SetTreeItemText(
        rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_ETH_TYPE), hEthHead), 1, csEthType);

    rTree.SetTreeItemText(
        rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_VLAN_TCI), hEthHead), 1, csVlan);

    rTree.SetTreeItemText(
        rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_PACKAGE_TYPE), hEthHead), 1, csPackage);

#else
	GooseMsgImpl *gmsi = dynamic_cast<GooseMsgImpl*>(pGooseMsg);
	if (gmsi->m_bHasVlanFlag)
	{
		csEthType.Format(L"802.1Q VLAN (0x%04X)", head.etherType);

		csVlan.Format(L"PRI: %u  CFI: %u  VID: %u", vlan.tci.userPriority, vlan.tci.cfi, vlan.tci.vlanID);

		if(0x88B8 == vlan.etherType)
		{
			csPackage.Format(L"IEC-GOOSE (0x%04X)", vlan.etherType);
		}
		else
		{
			csPackage.Format(L"0x%04X", vlan.etherType);
		}

		// 插入以太网帧头
		LBT_HTREEITEM hEthHead = rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_ETHERNET));

		rTree.SetTreeItemText(
			rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_DES_MAC), hEthHead), 1, csDesMac);

		rTree.SetTreeItemText(
			rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_SRC_MAC), hEthHead), 1, csSrcMac);

		rTree.SetTreeItemText(
			rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_ETH_TYPE), hEthHead), 1, csEthType);

		rTree.SetTreeItemText(
			rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_VLAN_TCI), hEthHead), 1, csVlan);

		rTree.SetTreeItemText(
			rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_PACKAGE_TYPE), hEthHead), 1, csPackage);
	}
	else
	{
		csEthType.Format(L"IEC-GOOSE (0x%04X)", head.etherType); 

		 

		// 插入以太网帧头
		LBT_HTREEITEM hEthHead = rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_ETHERNET));

		rTree.SetTreeItemText(
			rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_DES_MAC), hEthHead), 1, csDesMac);

		rTree.SetTreeItemText(
			rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_SRC_MAC), hEthHead), 1, csSrcMac);

		rTree.SetTreeItemText(
			rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_ETH_TYPE), hEthHead), 1, csEthType);
 
	}
	
#endif

    // 格式化goose pdu公共信息

    IEC8802_3_PDU_PUBLIC pducom;
    pGooseMsg->GetIEC8802Info(pducom);

    CString csAppID, csLength, csRev1, csRev2;

    csAppID.Format(L"0x%04X", pducom.appID);
    csLength.Format(L"%u", pducom.len);
    csRev1.Format(L"%u", pducom.reserved1);
    csRev2.Format(L"%u", pducom.reserved2);

    // 插入goose pdu公共信息
    LBT_HTREEITEM hGoose = rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_TYPE_GOOSE));

    rTree.SetTreeItemText(
        rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_APPID), hGoose), 1, csAppID);

    rTree.SetTreeItemText(
        rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_LENGTH), hGoose), 1, csLength);

    rTree.SetTreeItemText(
        rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_RESERVED1), hGoose), 1, csRev1);

    rTree.SetTreeItemText(
        rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_RESERVED2), hGoose), 1, csRev2);

    // 格式化PDU信息
    GOOSE_PDU_PUBLIC pdu;
    if(!pGooseMsg->GetPduPublicInfo(pdu))
    {
        DSM_DEBUG(L"dsm: _decodeGoose: get pdu public info failed\n");
        ASSERT(FALSE);
        return;
    }

    CString csPduLen, csGcRef, csTTL, csDataSetRef, csGcID, csTime, csStNum, csSqNum, csTest, csConfRev, \
        csNdsCom, csEntriesNum;

    csPduLen.Format(L"%u", pGooseMsg->GetGoosePduLen());
    csGcRef = pdu.gocbRef.c_str();
    csTTL.Format(L"%u", pdu.timeAllowedtoLive);
    csDataSetRef =pdu.datSet.c_str();
    csGcID = pdu.goID.c_str();

    UTC_TIME_t ut = {pdu.eventTm.ts_s, pdu.eventTm.ts_ns / 1000};
    CString csTimeFmt = DM_GetSystemTimeStrFromUTC(ut);

    if(csTimeFmt.IsEmpty())
    {
        csTime.Format(L"%u.%09u Tq: %u", pdu.eventTm.ts_s, pdu.eventTm.ts_ns, pdu.eventTm.ts_q);
    }
    else
    {
        csTime.Format(L"%s.%09u Tq: %u", csTimeFmt, pdu.eventTm.ts_ns, pdu.eventTm.ts_q);
    }

    csStNum.Format(L"%u", pdu.stNum);
    csSqNum.Format(L"%u", pdu.sqNum);
    csTest.Format(L"%s(%u)", (pdu.test ? L"TRUE" : L"FALSE"), pdu.test);
    csConfRev.Format(L"%u", pdu.confRev);
    csNdsCom.Format(L"%s(%u)", (pdu.ndsCom ? L"TRUE" : L"FALSE"), pdu.ndsCom);
    csEntriesNum.Format(L"%u", pdu.numDatSetEntries);

    // 插入PDU信息
    LBT_HTREEITEM hPdu = rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_PDU));

    rTree.SetTreeItemText(
        rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_PDU_LENGTH), hPdu), 1, csPduLen);

    rTree.SetTreeItemText(
        rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_GCRef), hPdu), 1, csGcRef);

    rTree.SetTreeItemText(
        rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_TTL), hPdu), 1, csTTL);

    rTree.SetTreeItemText(
        rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_DATASET_REF), hPdu), 1, csDataSetRef);

    rTree.SetTreeItemText(
        rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_APPID_GCID), hPdu), 1, csGcID);

    rTree.SetTreeItemText(
        rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_EVENT_TIME), hPdu), 1, csTime);

    rTree.SetTreeItemText(
        rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_STNUM), hPdu), 1, csStNum);

    rTree.SetTreeItemText(
        rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_SQNUM), hPdu), 1, csSqNum);

    rTree.SetTreeItemText(
        rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_TEST), hPdu), 1, csTest);

    rTree.SetTreeItemText(
        rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_CONFREV), hPdu), 1, csConfRev);

    rTree.SetTreeItemText(
        rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_NDSCOM), hPdu), 1, csNdsCom);

    rTree.SetTreeItemText(
        rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_ENTRIESNUM), hPdu), 1, csEntriesNum);

    if(pGooseMsg->GetSecurityOpt())
    {
        CString csSecurity;
        csSecurity = pdu.security.c_str();

        rTree.SetTreeItemText(
            rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_SECURITY), hPdu), 1, csSecurity);
    }

    // 枚举数据集

    LBT_HTREEITEM hDataSet = rTree.InsertTreeItem(dsm_stringMgr::Get(DSM_STR_FRAME_DATASET));

    CString csText, csValue;
    LBT_HTREEITEM hChannel;
    GOOSE_DATA* pData = NULL;
    size_t nCounts = pGooseMsg->GetChannelNum();

    for(size_t i = 0; i < nCounts; i++)
    {
        pData = pGooseMsg->GetAChannel(i);
        ASSERT(pData);

        csText.Format(L"%03u", i);
        hChannel = rTree.InsertTreeItem(csText, hDataSet);

        if(pData->type == GSEDT_STRUCT)
        {
            GOOSE_STRUCT* data = (GOOSE_STRUCT*)pData;

            typedef std::vector<GOOSE_DATA *> VecData;
            VecData::const_iterator itBegin = data->member.begin();
            VecData::const_iterator itEnd = data->member.end();

            for(; itBegin != itEnd; itBegin++)
            {
                DSM_GetGooseDataInfo(*itBegin, csText, csValue);
                rTree.SetTreeItemText(rTree.InsertTreeItem(csText, hChannel), 1, csValue);
            }
        }
        else
        {
            DSM_GetGooseDataInfo(pData, csText, csValue);
            rTree.SetTreeItemText(rTree.InsertTreeItem(csText, hChannel), 1, csValue);
        }
    }
}

