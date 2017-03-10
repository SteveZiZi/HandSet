#pragma once

/*
 *  以太网报文的VLAN固定标记
 */
#define NET_VLAN_TYPE    (0x8100)


/*
 *  以太网目的MAC地址在报文中的位置
 */
#define NET_DEST_MAC_POS     (0)

/*
 *  以太网源MAC地址在报文中的位置
 */
#define NET_SOURCE_MAC_POS   (6)

/*
 *  以太网报文的VLAN标记在报文中的位置
 */
#define NET_VLAN_POS         (12)

/*
 *  带VLAN的以太网报文的报文类型在报文中的位置
 */
#define NET_FRAME_TYPE_POS   (16)

/*
 *  不带VLAN的以太网报文的报文类型在报文中的位置
 */
#define NET_noVLAN_FRAME_TYPE_POS    (12)

/*
 *  带VLAN的以太网报文的APPID在报文中的位置
 */
#define NET_APPID_POS   (18)

/*
 *  不带VLAN的以太网报文的APPID在报文中的位置
 */
#define NET_noVLAN_APPID_POS (14)


/*
 *  带VLAN的以太网报文的PDU在报文中的位置
 */
#define NET_PDU_POS   (26)

/*
 *  不带VLAN的以太网报文的PDU在报文中的位置
 */
#define NET_noVLAN_PDU_POS (22)


#define MAKE_WORD_BIGEND(Byte0, Byte1) ((Byte0)<<8|(Byte1))
#define MAKE_DWORD_BIGEND(Byte0, Byte1, Byte2, Byte3) ((Byte0)<<24|(Byte1<<16)|(Byte2)<<8|(Byte3))


#define DECODE_ERROR  ((UINT16)-1)

#define LIB_ERRORMSG 

class codeMgr_c{

public:

    typedef enum {
        FRAME_INC_VLAN = 0,
        FRAME_NO_VLAN,
        FRAME_VLAN_UNKNOW
    }NET_FRAME_VLAN_e;


    enum _SMV92_FIELD_e{
        Smv92_Field_savPdu = 0x60,
        Smv92_Field_noASDU = 0x80,
        Smv92_Field_security = 0x81,
        Smv92_Field_sequenceOfAsdu = 0xA2,
    };
    enum _SMV92_ASDU_FIELD_e {
        Smv92_Field_Asdu_no = 0x30,
        Smv92_Field_Asdu_svId = 0x80,
        Smv92_Field_Asdu_dataSet,
        Smv92_Field_Asdu_smpCnt,
        Smv92_Field_Asdu_confRev,
        Smv92_Field_Asdu_referTm,
        Smv92_Field_Asdu_smpSynch,
        Smv92_Field_Asdu_smpRate,
        Smv92_Field_Asdu_sequenceOfData,
    };


    BOOL HadVlanType(PBYTE msg, UINT16 len) {
        ASSERT(msg);
        ASSERT(len > NET_VLAN_POS+2);
        return (NET_VLAN_TYPE == MAKE_WORD_BIGEND(msg[NET_VLAN_POS], msg[NET_VLAN_POS+1]));
    }

    PBYTE GetAppid(PBYTE msg, UINT16 len, NET_FRAME_VLAN_e eFrameType) {
        ASSERT(msg);
        BOOL bVlan = TRUE;
        if (eFrameType == FRAME_VLAN_UNKNOW) {
            bVlan = HadVlanType(msg, len);
        }
        else if (eFrameType == FRAME_NO_VLAN) {
            bVlan = FALSE;
        }

        if (bVlan) {
            ASSERT(len > NET_APPID_POS+2);
            return msg+NET_APPID_POS;
        }
        else {
            ASSERT(len > NET_noVLAN_APPID_POS+2);
            return msg+NET_noVLAN_APPID_POS;
        }
    }

    UINT16 GetPduPos(PBYTE msg, UINT16 len, NET_FRAME_VLAN_e eFrameType)
    {
        ASSERT(msg);
        BOOL bVlan = TRUE;
        if (eFrameType == FRAME_VLAN_UNKNOW) {
            bVlan = HadVlanType(msg, len);
        }
        else if (eFrameType == FRAME_NO_VLAN) {
            bVlan = FALSE;
        }

        if (bVlan) {
            return NET_PDU_POS;
        }
        else {
            return NET_noVLAN_PDU_POS;
        }
    }

    UINT16 GetSmv92AsduNum(PBYTE pduMsg, UINT16 len) {
        ASSERT(pduMsg);
        //Tag
        if (pduMsg[0] != Smv92_Field_savPdu) {
            LIB_ERRORMSG((_T("Frame is not SMV92 PDU Type\r\n")));
            return DECODE_ERROR;
        }
        //Length
        UINT16 uPos = GetTLVLength(pduMsg, len);
        if (uPos == DECODE_ERROR) {
            LIB_ERRORMSG((_T("Decode PDU Length fail\r\n")));
            return DECODE_ERROR;
        }
        //Value
        do {
            if (pduMsg[uPos] == Smv92_Field_noASDU) {
                DWORD dwAsduNum = 0;
                uPos = GetIntegerFieldVal(&pduMsg[uPos], len-uPos, &dwAsduNum);
                if (uPos  == DECODE_ERROR) {
                    LIB_ERRORMSG((_T("Decode ASDU Number fail.\r\n")));
                    return DECODE_ERROR;
                }
                return (UINT16)dwAsduNum;
            }
            uPos += GetNextTLVPos(pduMsg+uPos, len-uPos);
        }while(uPos < len);

        LIB_ERRORMSG((_T("not Found Smv92_Field_noASDU\r\n")));
        return DECODE_ERROR;
    }

    UINT16 GetSmv92SequenceOfAsduOffsetPos(PBYTE pduMsg, UINT16 len) {
        ASSERT(pduMsg);
        //Tag
        if (pduMsg[0] != Smv92_Field_savPdu) {
            LIB_ERRORMSG((_T("Frame is not SMV92 PDU Type\r\n")));
            return DECODE_ERROR;
        }
        //Length
        UINT16 uPos = GetTLVLength(pduMsg, len);
        if (uPos == DECODE_ERROR) {
            LIB_ERRORMSG((_T("Decode PDU Length fail\r\n")));
            return DECODE_ERROR;
        }
        //Value
        do {
            if (pduMsg[uPos] == Smv92_Field_sequenceOfAsdu) {
                return uPos;
            }
            uPos += GetNextTLVPos(pduMsg+uPos, len-uPos);
        }while(uPos < len);

        LIB_ERRORMSG((_T("not Found Smv92_Field_sequenceOfAsdu\r\n")));
        return DECODE_ERROR;
    }

    UINT16 GetSmv92SmpCnt(PBYTE pduMsg, UINT16 len, UINT16 *pSmpCnt, BYTE dwAsduNo) {
        ASSERT(pduMsg);
       // ASSERT(dwAsduNo > EPM_MAX_ASDU_NUM);

        //Tag
        UINT16 uPos = GetSmv92SequenceOfAsduOffsetPos(pduMsg, len);
        if (uPos == DECODE_ERROR) {
            return DECODE_ERROR;
        }
        //Length
        UINT16 uSequenceOfAsduLength;
        uPos += GetTLVLength(&pduMsg[uPos], len-uPos, &uSequenceOfAsduLength);
        if (uPos+uSequenceOfAsduLength > len) {
            LIB_ERRORMSG((_T("Decode SequenceOfAsdu Length Fail.\r\n")));
            return DECODE_ERROR;
        }
        //Value
        for(BYTE i = 0; i < dwAsduNo; i++) {
            //ASDU Tag
            if (pduMsg[uPos] != Smv92_Field_Asdu_no) {
                LIB_ERRORMSG((_T("Decode ASDU Field Fail\r\n")));
                return DECODE_ERROR;
            }
            //ASDU Length
            UINT16 uAsduLength;
            uPos += GetTLVLength(&pduMsg[uPos], len-uPos, &uAsduLength);
            if (uPos+uAsduLength > len) {
                LIB_ERRORMSG((_T("Decode ASDU Length Fail.\r\n")));
                return DECODE_ERROR;
            }
            //ASDU Value
            do {
                if (pduMsg[uPos] == Smv92_Field_Asdu_smpCnt) {
                    uPos = GetIntegerFieldVal(pduMsg+uPos, len-uPos, (DWORD*)&pSmpCnt[i]);
                    if (uPos != DECODE_ERROR) {
                        continue;
                    }
                    else {
                        LIB_ERRORMSG((_T("Decode ASDU%d SmpCnt Fail.\r\n"), i));
                        return DECODE_ERROR;
                    }
                }
                uPos += GetNextTLVPos(pduMsg+uPos, len-uPos);
            }while(uPos < len);
            if (uPos >= len) {
                LIB_ERRORMSG((_T("not Found ASDU%d Smv92_Field_Asdu_smpCnt\r\n"), i));
                return DECODE_ERROR;
            }
        }
    }

    UINT16 SetSmv92SmpCnt(PBYTE pduMsg, UINT16 len, UINT16 *pSmpCnt, BYTE dwAsduNo) {
        ASSERT(pduMsg);
        //ASSERT(dwAsduNo < EPM_MAX_ASDU_NUM);

        //Tag
        UINT16 uPos = GetSmv92SequenceOfAsduOffsetPos(pduMsg, len);
        if (uPos == DECODE_ERROR) {
            return DECODE_ERROR;
        }
        //Length
        UINT16 uSequenceOfAsduLength;
        uPos += GetTLVLength(&pduMsg[uPos], len-uPos, &uSequenceOfAsduLength);
        if (uPos+uSequenceOfAsduLength > len) {
            LIB_ERRORMSG((_T("Decode SequenceOfAsdu Length Fail.\r\n")));
            return DECODE_ERROR;
        }
        //Value
        for(BYTE i = 0; i < dwAsduNo; i++) {
            //ASDU Tag
            if (pduMsg[uPos] != Smv92_Field_Asdu_no) {
                LIB_ERRORMSG((_T("Decode ASDU Field Fail\r\n")));
                return DECODE_ERROR;
            }
            //ASDU Length
            UINT16 uAsduLength;
            uPos += GetTLVLength(&pduMsg[uPos], len-uPos, &uAsduLength);
            if (uPos+uAsduLength > len) {
                LIB_ERRORMSG((_T("Decode ASDU Length Fail.\r\n")));
                return DECODE_ERROR;
            }
            //ASDU Value
            do {
                if (pduMsg[uPos] == Smv92_Field_Asdu_smpCnt) {
                    uPos = SetIntegerFieldVal(pduMsg+uPos, len-uPos, pSmpCnt[i]);
                    if (uPos != DECODE_ERROR) {
                        break;
                    }
                    else {
                        LIB_ERRORMSG((_T("Decode ASDU%d SmpCnt Fail.\r\n"), i));
                        return DECODE_ERROR;
                    }
                }
                uPos += GetNextTLVPos(pduMsg+uPos, len-uPos);
            }while(uPos < len);
            if (uPos >= len) {
                LIB_ERRORMSG((_T("not Found ASDU%d Smv92_Field_Asdu_smpCnt\r\n"), i));
                return DECODE_ERROR;
            }
        }
    }

private:
    UINT16 GetTLVLength(PBYTE pTLV, UINT16 len, UINT16 *pusLength = NULL) {
        ASSERT(pTLV);
        ASSERT(len >= 2);
        if (pTLV[1] < 0x80) {
            if (pusLength) {
                *pusLength = pTLV[1];
            }
            return 2;
        }
        else if (pTLV[1] == 0x81) {
            if (pusLength) {
                *pusLength = pTLV[2];
            }
            return 3;
        }
        else if (pTLV[1] == 0x82) {
            if (pusLength) {
                *pusLength = MAKE_WORD_BIGEND(pTLV[2], pTLV[3]);
            }
            return 4;
        }
        else {
            return DECODE_ERROR;
        }
    }
    
    UINT16 GetIntegerFieldVal(PBYTE pTLV, UINT16 len, DWORD *pdwVal) {
        ASSERT(pTLV);
        ASSERT(len >= 2 && len >= pTLV[1]+2);
        if (pTLV[1] == 0x01) {
            if (pdwVal) {
                *pdwVal = pTLV[2];
            }
        }
        else if (pTLV[1] == 0x02) {
            if (pdwVal) {
                *pdwVal = MAKE_WORD_BIGEND(pTLV[2], pTLV[3]);
            }
        }
        else if (pTLV[1] == 0x03) {
            if (pdwVal) {
                *pdwVal = (MAKE_WORD_BIGEND(pTLV[2], pTLV[3])<<8)+pTLV[4];
            }
        }
        else if (pTLV[1] == 0x04) {
            if (pdwVal) {
                *pdwVal = MAKE_DWORD_BIGEND(pTLV[2], pTLV[3], pTLV[4], pTLV[5]);
            }
        }
        else if (pTLV[1] > 4) {
            if (pdwVal) {
                UINT16 uPos = pTLV[1]-2;
                *pdwVal = MAKE_DWORD_BIGEND(pTLV[uPos], pTLV[uPos+1], pTLV[uPos+2], pTLV[uPos+3]);
            }
        }
        return pTLV[1]+2;
    }

    UINT16 SetIntegerFieldVal(PBYTE pTLV, UINT16 len, DWORD dwNewVal) {
        ASSERT(pTLV);
        ASSERT(len >= 2 && len >= pTLV[1]+2);
        PBYTE pVal = (PBYTE)&dwNewVal;

        if (pTLV[1] == 0x01) {
            pTLV[2] = pVal[0];
        }
        else if (pTLV[1] == 0x02) {
            pTLV[2] = pVal[1];
            pTLV[3] = pVal[0];
        }
        else if (pTLV[1] == 0x03) {
            pTLV[2] = pVal[2];
            pTLV[3] = pVal[1];
            pTLV[4] = pVal[0];
        }
        else if (pTLV[1] == 0x04) {
            pTLV[2] = pVal[3];
            pTLV[3] = pVal[2];
            pTLV[4] = pVal[1];
            pTLV[5] = pVal[0];
        }
        else if (pTLV[1] > 4) {
            UINT16 uPos = pTLV[1]-2;
            pTLV[uPos]   = pVal[3];
            pTLV[uPos+1] = pVal[2];
            pTLV[uPos+2] = pVal[1];
            pTLV[uPos+3] = pVal[0];
        }
        return pTLV[1]+2;
    }


    UINT16 GetNextTLVPos(PBYTE pTLV, UINT16 len) {
        ASSERT(pTLV);
        UINT16 uLength;
        UINT16 uValuePos = GetTLVLength(pTLV, len, &uLength);
        if (uValuePos == DECODE_ERROR || uValuePos+uLength > len) {
            return DECODE_ERROR;
        }
        return uValuePos+uLength;
    }
};

__inline codeMgr_c *GetDecodeMgr() {
    static codeMgr_c s_instance;
    return &s_instance;
}
