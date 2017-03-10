/// @file
///
/// @brief
///     �Լ��ص�pcap�ļ����з���/ͳ��
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.10.31
///
/// �޸���ʷ��
///


#include "stdafx.h"
#include <math.h>
#include "pcapanalsis.h"
#include "src\service\datalayer\recv\FrameRecvStruct.h"
#include "src\service\datalayer\DsmDataLayer.h"

#define mpysp_i(a,b) ((a)*(b))
#define addsp_i(a,b) ((a)+(b))
#define divsp_i(a,b) ((a)/(b))
#define sqrtsp_i(a) sqrtf(a)

#define FLOAT_ZERO_ABS 0.000001

/**
 *  40Hz-70Hz��2�״�ͨ�˲�����������2ά��ʾ�����ʣ�4000Hz,5000Hz,8000Hz,10000Hz,12800Hz��
 */
static float a2_40_70Hz[5][5]={
{1.0000000000000000, -3.9197982160636045, 5.7756167061962405, -3.7913012649742948, 0.9355289049791802},
{1.0000000000000000, -3.9379743972595675, 5.8242790308669683, -3.8343673131140292, 0.9480817061067395},
{1.0000000000000000, -3.9632561319188047, 5.8937992239074362, -3.8977675868501578, 0.9672274281518589},
{1.0000000000000000, -3.9711483282387512, 5.9160218097821948, -3.9185671479490636, 0.9736948719763124},
{1.0000000000000000, -3.9778324437423040, 5.9350685930193521, -3.9366250488624668, 0.9793893500287969}
};
static float b2_40_70Hz[5][5]={
{0.0005371697747744, -0.0000000000000000, -0.0010743395495488, -0.0000000000000000, 0.0005371697747744},
{0.0003460413376464, -0.0000000000000000, -0.0006920826752927, -0.0000000000000000, 0.0003460413376464},
{0.0001365107218804, -0.0000000000000000, -0.0002730214437608, -0.0000000000000000, 0.0001365107218804},
{0.0000876555487554, -0.0000000000000000, -0.0001753110975108, -0.0000000000000000, 0.0000876555487554},
{0.0000536556798714, -0.0000000000000000, -0.0001073113597428, -0.0000000000000000, 0.0000536556798714}
};

///
/// @brief
///    ��ʼpcap�ļ�ͳ��
///
///
/// @return
///     bool    ��ʼ�ɹ�����true��ʧ��false
///
bool CPcapAnalysis::StartPcapFileStatisticsAnalysis()
{
    ///
    ///    ��ջ�������
    ///
    _ResetData();

    //����״̬Ϊ���ڷ����ļ�
    m_enAnalysisStatus = PCAP_FILE_ANALYSIS_ING;

    if (_OpenPcapFile())
    {
        if (m_eventCancelAnalysis.Lock(0))
        {
            _NotifyMessage(WM_PCAPFILE_ANALYSIS_MESSAGE, MSG_PCAPFILE_ANALYSIS_USER_CACEL);
            m_enAnalysisStatus =  PCAP_FILE_ANALYSIS_USER_CANCEL;
            RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("User Cacel Analysis\r\n")));
            return false;
        }

        PCAP_PACKET_HEADER stPcapPckHeader;
        uint32_t nIndexInPcapFile = 0;
        uint32_t nFrameLen = NET_MAX_FRAME_LEN;
        if (PCAP_FILE_SUCCESS == m_pPcapFile->GotoFirstPckToRead())
        {
            enumFrameType enFrameType = FRAME_TYPE_UNKOWN;
            uint16_t nAppid = 0;
            uint16_t nAsdu = 1;
            uint32_t nChannelCount = 0;
            FrameTimeStamp stFrameTime;

            m_pPcapFile->ReadOnePckHeader(stPcapPckHeader);
            if (m_pFrameBuffer == NULL)
            {
                m_pFrameBuffer = new uint8_t[NET_MAX_FRAME_LEN];
            }

            if (stPcapPckHeader.capLen == stPcapPckHeader.len)
            {
                nFrameLen = stPcapPckHeader.capLen < NET_MAX_FRAME_LEN ? stPcapPckHeader.capLen : NET_MAX_FRAME_LEN;
                m_pPcapFile->ReadOnePckData(m_pFrameBuffer, nFrameLen);

                //�������ĵ�һ֡��appid��Ϊ��ǰ�Ŀ��ƿ�id
                m_cMsgRapidDecoder.GetAppID(m_pFrameBuffer, nFrameLen, nAppid);
                m_nCurrentAppid = nAppid;

                stFrameTime.ts_sec = stPcapPckHeader.timeStamp.timeStamp_Second;
                stFrameTime.ts_usec = stPcapPckHeader.timeStamp.timeStamp_USec;

                enFrameType = m_cMsgRapidDecoder.GetMessageType(m_pFrameBuffer, nFrameLen);
                m_cMsgRapidDecoder.GetNoASDU(m_pFrameBuffer, nFrameLen, enFrameType, nAsdu);
                m_cMsgRapidDecoder.GetChannelNum(m_pFrameBuffer, nFrameLen, enFrameType, nChannelCount);

                //��¼ʱ�䣬����������
                m_cAppidInfoAnalysis.ParseAppidInfo(stFrameTime, nAppid, enFrameType, nAsdu, nChannelCount, m_pFrameBuffer, nFrameLen);
                //�����쳣ͳ����Ϣ������ֵ������������Ϣ������¼֡ʱ��
                _HandleOneFrame(nIndexInPcapFile, stFrameTime, m_pFrameBuffer, nFrameLen);
            }

            while(PCAP_FILE_END != m_pPcapFile->GotoNextPckToRead())
            {
                if (m_eventCancelAnalysis.Lock(0))
                {
                    _NotifyMessage(WM_PCAPFILE_ANALYSIS_MESSAGE, MSG_PCAPFILE_ANALYSIS_USER_CACEL);
                    m_enAnalysisStatus =  PCAP_FILE_ANALYSIS_USER_CANCEL;
                    RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("User Cacel Analysis\r\n")));
                    return false;
                }

                m_pPcapFile->ReadOnePckHeader(stPcapPckHeader);
                if (stPcapPckHeader.capLen != stPcapPckHeader.len)
                {
                    continue;
                }

                if (PCAP_FILE_ERROR != m_pPcapFile->ReadOnePckData(m_pFrameBuffer, nFrameLen))
                {
                    nFrameLen = stPcapPckHeader.capLen < NET_MAX_FRAME_LEN ? stPcapPckHeader.capLen : NET_MAX_FRAME_LEN;
                    nIndexInPcapFile++;

                    enFrameType = m_cMsgRapidDecoder.GetMessageType(m_pFrameBuffer, nFrameLen);
                    m_cMsgRapidDecoder.GetAppID(m_pFrameBuffer, nFrameLen, nAppid);
                    m_cMsgRapidDecoder.GetNoASDU(m_pFrameBuffer, nFrameLen, enFrameType, nAsdu);
                    m_cMsgRapidDecoder.GetChannelNum(m_pFrameBuffer, nFrameLen, enFrameType, nChannelCount);

                    stFrameTime.ts_sec = stPcapPckHeader.timeStamp.timeStamp_Second;
                    stFrameTime.ts_usec = stPcapPckHeader.timeStamp.timeStamp_USec;

                    //����ʱ����Ϣ�����������
                    m_cAppidInfoAnalysis.ParseAppidInfo(stFrameTime, nAppid, enFrameType, nAsdu, nChannelCount, m_pFrameBuffer, nFrameLen);

                    //�����쳣ͳ����Ϣ������ֵ������������Ϣ������¼֡ʱ��
                    _HandleOneFrame(nIndexInPcapFile, stFrameTime, m_pFrameBuffer, nFrameLen);
                } 
                else
                {
                    RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("StartPcapFileStatisticsAnalysis() ReadOnePckData() Failed \r\n")));
                }
            }

            //�ļ���ȡ��ϣ���������ʣ������ݲ����ʼ�����ɢ����Ϣ��smv������Ϣ
            m_cAppidInfoAnalysis.CalcSmpRate();
            _CalcAllAppidDispersion();

            m_enAnalysisStatus = PCAP_FILE_ANALYSIS_DONE;
            _NotifyMessage(WM_PCAPFILE_ANALYSIS_MESSAGE, MSG_PCAPFILE_STATUS_ANALYSIS_DONE);
            return true;
        }
    }
    _NotifyMessage(WM_PCAPFILE_ANALYSIS_MESSAGE, MSG_PCAPFILE_ANALYSIS_FAILED);
    m_enAnalysisStatus = PCAP_FILE_NOT_ANALYSISED;
    return false;
}

///
/// @brief
///    ��ȡpcap�ļ�������֡��
///
uint32_t CPcapAnalysis::GetPcapFileTotalFrameCount()
{
    if (m_pPcapFile)
    {
        return m_pPcapFile->GetPckNum();
    }
    return 0;
}

///
/// @brief
///    ��һ������֡���з���
///
void CPcapAnalysis::_HandleOneFrame(uint32_t nPcapFileIndex, FrameTimeStamp stFrameTime, uint8_t *pBuffer, uint32_t nBufferSize )
{
    enumFrameType enFrameType = m_cMsgRapidDecoder.GetMessageType(pBuffer, nBufferSize);
    AppidInfo stAppidInfo;
    stAppidInfo.enFrameType = enFrameType;

#if 0
    if (enFrameType != m_enFrameType)
    {
        RETAILMSG(1,(_T("frametype not math the first frame\r\n")));
        return;
    }
#endif

    //nAppid���ڱ�ʶ��ͬ�Ŀ��ƿ飬�����9-1/9-2,appidΪ0��ʾ����ʧ��
    //���ft3���ݣ�ʹ��nAppid = 0����ʶ
    uint16_t nAppid = 0;
    if (enFrameType == FRAME_TYPE_NET_9_1)
    {
        SAV91_ASDU_PUBLIC    stSav91AsduPublic;
        if ( NULL == m_pSav91Msg)
        {
            m_pSav91Msg = CreateSav91MsgImpl();
        }
        if (0 == m_pSav91Msg->Decode(pBuffer, nBufferSize))
        {
            //����ʧ��
            return;
        }

        m_cMsgRapidDecoder.GetAppID(pBuffer, nBufferSize, nAppid);
        if (nAppid == 0)
        {
            return;
        }

        CPcapFileSmvSmpStatus* pSmvFrameStatistic = NULL;
        MapAppidSmvStatistics::iterator itrMap = m_mapSmvStatistics.find(nAppid);
        if (itrMap != m_mapSmvStatistics.end())
        {
            pSmvFrameStatistic = &(itrMap->second);
        }
        else
        {
            CPcapFileSmvSmpStatus smvFrameStatistic;
            m_mapSmvStatistics.insert(std::make_pair(nAppid, smvFrameStatistic));
            itrMap = m_mapSmvStatistics.find(nAppid);
            if (itrMap != m_mapSmvStatistics.end())
            {
                pSmvFrameStatistic = &(itrMap->second);
            }
        }

        CAppidSampleMapping* pAppidSamleMapping = NULL;
        MapAppidSampleMapping::iterator itrMapping = m_mapAppidSampleMapping.find(nAppid);
        if (itrMapping != m_mapAppidSampleMapping.end())
        {
            pAppidSamleMapping = &(itrMapping->second);
        }
        else
        {
            CAppidSampleMapping appidSampleMapping;
            m_mapAppidSampleMapping.insert(std::make_pair(nAppid, appidSampleMapping));
            itrMapping = m_mapAppidSampleMapping.find(nAppid);
            if (itrMapping != m_mapAppidSampleMapping.end())
            {
                pAppidSamleMapping = &(itrMapping->second);
            }
        }

        uint32_t nChannelCount = m_pSav91Msg->GetASDUDefaultSmpsNum();
        uint32_t nAsdu = m_pSav91Msg->GetNoASDU();

        //���쵱ǰ���ݵ�һ�����ƿ����ݣ�ͳ����Ҫ���ƿ���Ϣ
        CNetDetectItem stDetectItem;
        stDetectItem.enFrameType = enFrameType;
        stDetectItem.nChannelCount = nChannelCount;
        stDetectItem.nAsdu = nAsdu;

        SampleIndexPosition stSampleInfo;
        stSampleInfo.nPcapFrameIndex = nPcapFileIndex;
        stSampleInfo.nAsduCount = nAsdu;

        FrameStatus *pFrameStatus = new FrameStatus[nAsdu];
        for (uint32_t i = 0; i < nAsdu; ++i)
        {
            //���������ʵ��λ����Ϣ
            stSampleInfo.nAsduIndex = i;
            pAppidSamleMapping->ParseSampleInfo(stSampleInfo, stFrameTime, nBufferSize);

            //���������������ֵ
            m_pSav91Msg->GetASDUPublicInfo(i, stSav91AsduPublic);
            stDetectItem.nSampleRate = stSav91AsduPublic.smpRate != 0 ? stSav91AsduPublic.smpRate * DSM_CONFIG_DEFAULT_ELEC_FREQUENCY : 12800;


            //������ޣ�ʧ������Ϣ
            CSmvCtrlBlkQuality smvCtrlBlkQuality;
            smvCtrlBlkQuality.SetCtrlBlk(&stDetectItem);
            smvCtrlBlkQuality.SetCtrlBlkStatusWords(stSav91AsduPublic.statusWord1, stSav91AsduPublic.statusWord2);

            pFrameStatus[i] = pSmvFrameStatistic->ParseSamplePoint(nPcapFileIndex, &stDetectItem,
                                                 stSav91AsduPublic.smpCnt,smvCtrlBlkQuality);
        }

        //����һ֡����״̬��Ϣ
        FrameStatus stStatus;
        stStatus.nData = pFrameStatus[0].GetValue();
        stStatus.vecSmpCnt.push_back(pFrameStatus[0].vecSmpCnt.size() > 0 ? pFrameStatus[0].vecSmpCnt.front() : 0);
        for (uint32_t i = 1; i < nAsdu; ++i)
        {
            stStatus.nData |= pFrameStatus[i].GetValue();
            stStatus.vecSmpCnt.push_back(pFrameStatus[i].vecSmpCnt.size() > 0 ? pFrameStatus[i].vecSmpCnt.front() : 0);
        }
        m_mapPcapFileFrameIndexStatus[nPcapFileIndex] = stStatus;

        delete[] pFrameStatus;

    }
    else if (enFrameType == FRAME_TYPE_NET_9_2)
    {
        SAV92_ASDU_PUBLIC    stSav92AsduPublic;
        uint32_t p92SmvQlt[NET_9_2_LE_MAX_CHANNELS_PER_ASDU];

        if ( NULL == m_pSav92Msg)
        {
            m_pSav92Msg = CreateSav92MsgImpl();
        }
        if (0 == m_pSav92Msg->Decode(pBuffer, nBufferSize))
        {
            //����ʧ��
            return;
        }

        m_cMsgRapidDecoder.GetAppID(pBuffer, nBufferSize, nAppid);
        if (nAppid == 0)
        {
            return;
        }

        CPcapFileSmvSmpStatus* pSmvFrameStatistic = NULL;
        MapAppidSmvStatistics::iterator itrMap = m_mapSmvStatistics.find(nAppid);
        if (itrMap != m_mapSmvStatistics.end())
        {
            pSmvFrameStatistic = &(itrMap->second);
        }
        else
        {
            CPcapFileSmvSmpStatus smvFrameStatistic;
            m_mapSmvStatistics.insert(std::make_pair(nAppid, smvFrameStatistic));
            itrMap = m_mapSmvStatistics.find(nAppid);
            if (itrMap != m_mapSmvStatistics.end())
            {
                pSmvFrameStatistic = &(itrMap->second);
            }
        }

        CAppidSampleMapping* pAppidSamleMapping = NULL;
        MapAppidSampleMapping::iterator itrMapping = m_mapAppidSampleMapping.find(nAppid);
        if (itrMapping != m_mapAppidSampleMapping.end())
        {
            pAppidSamleMapping = &(itrMapping->second);
        }
        else
        {
            CAppidSampleMapping appidSampleMapping;
            m_mapAppidSampleMapping.insert(std::make_pair(nAppid, appidSampleMapping));
            itrMapping = m_mapAppidSampleMapping.find(nAppid);
            if (itrMapping != m_mapAppidSampleMapping.end())
            {
                pAppidSamleMapping = &(itrMapping->second);
            }
        }

        uint32_t nAsdu = m_pSav92Msg->GetNoASDU();
        CNetDetectItem stDetectItem;
        stDetectItem.nAsdu = nAsdu;
        stDetectItem.enFrameType = enFrameType;
        stDetectItem.nSampleRate = 4000;

        SampleIndexPosition stSampleInfo;
        stSampleInfo.nPcapFrameIndex = nPcapFileIndex;
        stSampleInfo.nAsduCount = nAsdu;

        FrameStatus* pFrameStatus = new FrameStatus[nAsdu];
        for (uint32_t i = 0; i < nAsdu; ++i)
        {
            //���������ʵ��λ����Ϣ
            stSampleInfo.nAsduIndex = i;
            pAppidSamleMapping->ParseSampleInfo(stSampleInfo, stFrameTime, nBufferSize);

            //���������������ֵ
            m_pSav92Msg->GetASDUPublicInfo(i, stSav92AsduPublic);
            uint32_t nChannelCount = m_pSav92Msg->GetASDUSmpsNum(i);
            stDetectItem.nChannelCount = nChannelCount;

            //������ޣ�ʧ������Ϣ
            CSmvCtrlBlkQuality smvCtrlBlkQuality;
            smvCtrlBlkQuality.SetCtrlBlk(&stDetectItem);
            smvCtrlBlkQuality.SetMuSync(stSav92AsduPublic.smpSynch);

            uint32_t nHandleChannelCount = min(nChannelCount, NET_9_2_LE_MAX_CHANNELS_PER_ASDU);
            m_pSav92Msg->GetASDUSmps(i, NULL, p92SmvQlt, nHandleChannelCount);
            for (uint16_t k = 0; k < nHandleChannelCount; ++k)
            {
                smvCtrlBlkQuality.SetChannelQuality(k, p92SmvQlt[k]);
            }

            pFrameStatus[i] = pSmvFrameStatistic->ParseSamplePoint(nPcapFileIndex, &stDetectItem,
                                                 stSav92AsduPublic.smpCnt, smvCtrlBlkQuality);
        }

        //����һ֡����״̬��Ϣ
        FrameStatus stStatus;
        stStatus.nData = pFrameStatus[0].GetValue();
        stStatus.vecSmpCnt.push_back(pFrameStatus[0].vecSmpCnt.size() > 0 ? pFrameStatus[0].vecSmpCnt.front() : 0);
        for (uint32_t i = 1; i < nAsdu; ++i)
        {
            stStatus.nData |= pFrameStatus[i].GetValue();
            stStatus.vecSmpCnt.push_back(pFrameStatus[i].vecSmpCnt.size() > 0 ? pFrameStatus[i].vecSmpCnt.front() : 0);
        }
        m_mapPcapFileFrameIndexStatus[nPcapFileIndex] = stStatus;

        delete[] pFrameStatus;
    }
    else if (enFrameType == FRAME_TYPE_NET_GOOSE)
    {
        if (NULL == m_pGooseMsg)
        {
            m_pGooseMsg = CreateGooseMsgImpl();
        }

        if (0 == m_pGooseMsg->Decode(pBuffer, nBufferSize))
        {
            return;
        }

        GOOSE_PDU_PUBLIC stGoosePduPublic;
        m_pGooseMsg->GetPduPublicInfo(stGoosePduPublic);
        m_cMsgRapidDecoder.GetAppID(pBuffer, nBufferSize, nAppid);
        if (nAppid == 0)
        {
            return;
        }

        CPcapFileGooseFrameStatus* pGooseRtFrameStatistic = NULL;
        MapAppidGooseStatistics::iterator itrMap = m_mapGooseStatistics.find(nAppid);
        if (itrMap != m_mapGooseStatistics.end())
        {
            pGooseRtFrameStatistic = &(itrMap->second);
        }
        else
        {
            CPcapFileGooseFrameStatus gooseRtStatistics;
            m_mapGooseStatistics.insert(std::make_pair(nAppid, gooseRtStatistics));
            itrMap = m_mapGooseStatistics.find(nAppid);
            if (itrMap != m_mapGooseStatistics.end())
            {
                pGooseRtFrameStatistic = &(itrMap->second);
            }
        }

        CAppidSampleMapping* pAppidSamleMapping = NULL;
        MapAppidSampleMapping::iterator itrMapping = m_mapAppidSampleMapping.find(nAppid);
        if (itrMapping != m_mapAppidSampleMapping.end())
        {
            pAppidSamleMapping = &(itrMapping->second);
        }
        else
        {
            CAppidSampleMapping appidSampleMapping;
            m_mapAppidSampleMapping.insert(std::make_pair(nAppid, appidSampleMapping));
            itrMapping = m_mapAppidSampleMapping.find(nAppid);
            if (itrMapping != m_mapAppidSampleMapping.end())
            {
                pAppidSamleMapping = &(itrMapping->second);
            }
        }

        SampleIndexPosition stSampleInfo;
        stSampleInfo.nPcapFrameIndex = nPcapFileIndex;
        stSampleInfo.nAsduCount = 1;
        pAppidSamleMapping->ParseSampleInfo(stSampleInfo, stFrameTime, nBufferSize);

        FrameStatus stFrameStatus = pGooseRtFrameStatistic->ParseGooseFrame(stGoosePduPublic.timeAllowedtoLive, stFrameTime,
                                                                            stGoosePduPublic.eventTm.ts_q,
                                                                            stGoosePduPublic.stNum, stGoosePduPublic.sqNum);

        //����ǰ֡��֡״̬��Ϣ
        m_mapPcapFileFrameIndexStatus[nPcapFileIndex] = stFrameStatus;

    }
    else if (enFrameType == FRAME_TYPE_FT3_GUOWANG)
    {
        if (NULL == m_pGuoWangFT3Msg)
        {
            m_pGuoWangFT3Msg = CreateGuoWangFT3MsgImpl();
        }

        if (0 == m_pGuoWangFT3Msg->Decode(reinterpret_cast<uint16_t*>(pBuffer), nBufferSize/2))
        {
            return;
        }

        CPcapFileSmvSmpStatus* pSmvFrameStatistic = NULL;
        MapAppidSmvStatistics::iterator itrMap = m_mapSmvStatistics.find(nAppid);       //��ʱappid6ӦΪ0����ʾft3����
        if (itrMap != m_mapSmvStatistics.end())
        {
            pSmvFrameStatistic = &(itrMap->second);
        }
        else
        {
            CPcapFileSmvSmpStatus smvFrameStatistic;
            m_mapSmvStatistics.insert(std::make_pair(nAppid, smvFrameStatistic));
            itrMap = m_mapSmvStatistics.find(nAppid);
            if (itrMap != m_mapSmvStatistics.end())
            {
                pSmvFrameStatistic = &(itrMap->second);
            }
        }

        CAppidSampleMapping* pAppidSamleMapping = NULL;
        MapAppidSampleMapping::iterator itrMapping = m_mapAppidSampleMapping.find(nAppid);
        if (itrMapping != m_mapAppidSampleMapping.end())
        {
            pAppidSamleMapping = &(itrMapping->second);
        }
        else
        {
            CAppidSampleMapping appidSampleMapping;
            m_mapAppidSampleMapping.insert(std::make_pair(nAppid, appidSampleMapping));
            itrMapping = m_mapAppidSampleMapping.find(nAppid);
            if (itrMapping != m_mapAppidSampleMapping.end())
            {
                pAppidSamleMapping = &(itrMapping->second);
            }
        }

        //���������ʵ��λ����Ϣ
        SampleIndexPosition stSampleInfo;
        stSampleInfo.nPcapFrameIndex = nPcapFileIndex;
        stSampleInfo.nAsduCount = 1;
        stSampleInfo.nAsduIndex = 0;
        pAppidSamleMapping->ParseSampleInfo(stSampleInfo, stFrameTime, nBufferSize);

        GUOWANG_FT3_ASDU_PUBLIC stAsduPbl;
        m_pGuoWangFT3Msg->GetASDUPublicInfo(stAsduPbl);

        CFt3DetectItem stDetectItem;
        stDetectItem.enFrameType = enFrameType;
        stDetectItem.nAsdu = 1;
        stDetectItem.nSampleRate = 4000;

        CSmvCtrlBlkQuality smvCtrlBlkQuality;
        smvCtrlBlkQuality.SetCtrlBlk(&stDetectItem);
        smvCtrlBlkQuality.SetCtrlBlkStatusWords(stAsduPbl.statusWord1,stAsduPbl.statusWord2);

        FrameStatus stFrameStatus = pSmvFrameStatistic->ParseSamplePoint(nPcapFileIndex, &stDetectItem,
                                             stAsduPbl.smpCnt, smvCtrlBlkQuality);

        //����һ֡��״̬��Ϣ
        m_mapPcapFileFrameIndexStatus[nPcapFileIndex] = stFrameStatus;
    }
    else if (enFrameType == FRAME_TYPE_FT3_NANRUI)
    {
        if (NULL == m_pNanRuiFt3Msg)
        {
            m_pNanRuiFt3Msg = CreateNanRuiFT3MsgImpl();
        }

        if (0 == m_pNanRuiFt3Msg->Decode(reinterpret_cast<uint16_t*>(pBuffer), nBufferSize/2))
        {
            return;
        }

        CPcapFileSmvSmpStatus* pSmvFrameStatistic = NULL;
        MapAppidSmvStatistics::iterator itrMap = m_mapSmvStatistics.find(nAppid);       //��ʱappid6ӦΪ0����ʾft3����
        if (itrMap != m_mapSmvStatistics.end())
        {
            pSmvFrameStatistic = &(itrMap->second);
        }
        else
        {
            CPcapFileSmvSmpStatus smvFrameStatistic;
            m_mapSmvStatistics.insert(std::make_pair(nAppid, smvFrameStatistic));
            itrMap = m_mapSmvStatistics.find(nAppid);
            if (itrMap != m_mapSmvStatistics.end())
            {
                pSmvFrameStatistic = &(itrMap->second);
            }
        }

        CAppidSampleMapping* pAppidSamleMapping = NULL;
        MapAppidSampleMapping::iterator itrMapping = m_mapAppidSampleMapping.find(nAppid);
        if (itrMapping != m_mapAppidSampleMapping.end())
        {
            pAppidSamleMapping = &(itrMapping->second);
        }
        else
        {
            CAppidSampleMapping appidSampleMapping;
            m_mapAppidSampleMapping.insert(std::make_pair(nAppid, appidSampleMapping));
            itrMapping = m_mapAppidSampleMapping.find(nAppid);
            if (itrMapping != m_mapAppidSampleMapping.end())
            {
                pAppidSamleMapping = &(itrMapping->second);
            }
        }
        //���������ʵ��λ����Ϣ
        SampleIndexPosition stSampleInfo;
        stSampleInfo.nPcapFrameIndex = nPcapFileIndex;
        stSampleInfo.nAsduCount = 1;
        stSampleInfo.nAsduIndex = 0;
        pAppidSamleMapping->ParseSampleInfo(stSampleInfo, stFrameTime, nBufferSize);

        NANRUI_FT3_ASDU_PUBLIC stAsduPbl;
        m_pNanRuiFt3Msg->GetASDUPublicInfo(stAsduPbl);

        CFt3DetectItem stDetectItem;
        stDetectItem.enFrameType = enFrameType;
        stDetectItem.nAsdu = 1;
        stDetectItem.nSampleRate = stAsduPbl.smpRate != 0 ? stAsduPbl.smpRate * DSM_CONFIG_DEFAULT_ELEC_FREQUENCY : 12800;

        CSmvCtrlBlkQuality smvCtrlBlkQuality;
        smvCtrlBlkQuality.SetCtrlBlk(&stDetectItem);
        smvCtrlBlkQuality.SetCtrlBlkStatusWords(stAsduPbl.statusWord1,stAsduPbl.statusWord2);

        FrameStatus stFrameStatus = pSmvFrameStatistic->ParseSamplePoint(nPcapFileIndex, &stDetectItem,
                                    stAsduPbl.smpCnt, smvCtrlBlkQuality);

        //����һ֡��״̬��Ϣ
        m_mapPcapFileFrameIndexStatus[nPcapFileIndex] = stFrameStatus;
    }
}

///
/// @brief
///    ֹͣpcap�ļ�ͳ��
///
/// @return
///     bool    ֹͣ�ɹ�����true��ʧ��false
///
bool CPcapAnalysis::StopPcapFileStatisticsAnalysis()
{
    RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("m_eventCancelAnalysis.SetEvent(); \r\n")));
    m_eventCancelAnalysis.SetEvent();
    uint32_t n = 50;
    while (n > 0)
    {
        if (m_enAnalysisStatus == PCAP_FILE_ANALYSIS_DONE ||
            m_enAnalysisStatus == PCAP_FILE_ANALYSIS_USER_CANCEL)
        {
            return true;
        }
        Sleep(10);
        n--;
    }
    return false;
}

///
/// @brief
///    ��ȡpcap�ļ��п��ƿ飨appid����Ŀ
///
uint32_t CPcapAnalysis::GetPcapFileAppidCount()
{
    return m_cAppidInfoAnalysis.m_mapAppidInfoCalcInfo.size();
}

///
/// @brief
///    ��ȡһ�����ƿ�Appid
///
int32_t CPcapAnalysis::GetPcapFileOneAppid( uint32_t nIndex )
{
    if (nIndex < m_cAppidInfoAnalysis.m_mapAppidInfoCalcInfo.size())
    {
        MapAppidInfo::iterator itr = m_cAppidInfoAnalysis.m_mapAppidInfoCalcInfo.begin();
        advance(itr, nIndex);
        if (itr != m_cAppidInfoAnalysis.m_mapAppidInfoCalcInfo.end())
        {
            return itr->first;
        }
    }
    return -1;
}

///
/// @brief
///    ��ȡpcap�ļ�goose�������
///
/// @param[in]  pData �ⲿ����Ŀռ�ָ�룬���ڱ���pcapͳ�ƽ��
///
/// @return
///     bool    ��ȡ�ɹ�����true��ʧ��false
///
bool CPcapAnalysis::GetPcapFileGooseStatisticsResult( CGooseFrameStatistics* pData )
{
    if (m_nCurrentAppid == -1 || pData == NULL|| m_enAnalysisStatus != PCAP_FILE_ANALYSIS_DONE)
    {
        return false;
    }

    CPcapFileGooseFrameStatus* pGooseRtStatistics = NULL;
    MapAppidGooseStatistics::iterator itr = m_mapGooseStatistics.find(m_nCurrentAppid);
    if (itr != m_mapGooseStatistics.end())
    {
        pGooseRtStatistics = &(itr->second);
    }
    if (pGooseRtStatistics)
    {
        pData->m_stGooseFrameStatistics = pGooseRtStatistics->m_stGooseFrameStatistics;
        return true;
    }
    return false;
}

///
/// @brief
///    ��ȡpcap�ļ�����ֵ���������������ɢ����Ϣ
///
/// @param[in]  pData �ⲿ����Ŀռ�ָ�룬���ڱ���pcapͳ�ƽ��
///
/// @return
///     bool    ����ֵ��ȡ�ɹ�����true��ʧ��false
///
bool CPcapAnalysis::GetPcapFileSmvStatisticsResult( CDataFrameStatistics* pData )
{
    if (m_nCurrentAppid == -1 || pData == NULL || m_enAnalysisStatus != PCAP_FILE_ANALYSIS_DONE)
    {
        return false;
    }

    CPcapFileSmvSmpStatus* pSmvFrameStatistic = NULL;
    MapAppidSmvStatistics::iterator itrMap = m_mapSmvStatistics.find(m_nCurrentAppid);
    if (itrMap != m_mapSmvStatistics.end())
    {
        pSmvFrameStatistic = &(itrMap->second);
    }
    if (!pSmvFrameStatistic)
    {
        return false;
    }
    pData->m_stSmvFrameStatistics = pSmvFrameStatistic->m_stSmvFrameStatistics;

    CMsgDisperisonCalc* pMsgDisp = NULL;
    MapAppidMsgDispersion::iterator itr = m_mapAppidDispersion.find(m_nCurrentAppid);
    if (itr != m_mapAppidDispersion.end())
    {
        pMsgDisp = itr->second;
    }
    if (!pMsgDisp)
    {
        return false;
    }
    pMsgDisp->GetDispersion(pData->m_fMsgDispersion, MSG_DISPERSION_ARRAY_SIZE);
    return true;
}

///
/// @brief
///    ��ȡ��ǰ���ƿ������֡����
///
enumFrameType CPcapAnalysis::GetPcapFileAppidFrameType( int32_t nAppid )
{
    return m_cAppidInfoAnalysis.GetAppidFrameType(nAppid);
}

///
/// @brief
///    ���ݵ�ǰ���ƿ�appid������ʱ����ɢ����Ϣ
///
void CPcapAnalysis::_HandleFrameTimeDispersion(int32_t nAppid, FrameTimeStamp stFrameTime )
{
    CMsgDisperisonCalc* pMsgDisp = NULL;
    MapAppidMsgDispersion::iterator itr = m_mapAppidDispersion.find(nAppid);
    if (itr != m_mapAppidDispersion.end())
    {
        pMsgDisp = itr->second;
    }
    else
    {
        pMsgDisp = new CMsgDisperisonCalc;
        m_mapAppidDispersion[nAppid] = pMsgDisp;

        //������ɢ��ǰ�����ö�Ӧ���ƿ�Ĳ����ʵ�������Ϣ
        DispersionCalcConfig config;
        config.nASDUCount = m_cAppidInfoAnalysis.GetAppidNoAsdu(nAppid);
        config.nRatedFreq = 50;
        config.nSamplingFreq = m_cAppidInfoAnalysis.GetAppidSmpRate(nAppid);
        pMsgDisp->SetCalcConfig(config);

    }

    pMsgDisp->ParsingFrameTimeStamp(stFrameTime);

}


///
/// @brief
///    ��ȡpcap��ĳ��appid��Ӧ��֡��
/// 
uint32_t CPcapAnalysis::GetPcapFileAppidFrameCount( int32_t nAppid )
{
    MapAppidSampleMapping::iterator itr = m_mapAppidSampleMapping.find(nAppid);
    if (itr != m_mapAppidSampleMapping.end())
    {
        uint32_t nAppidFrameCount = itr->second.m_vecAppidSampleMap.back().nAppidFrameIndex + 1;
        if (nAppidFrameCount != itr->second.m_vecAppidFrameIndexMap.size())
        {
            RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("GetPcapFileAppidFrameCount(): Appid FrameCount Not Match!\r\n")));
        }
        return nAppidFrameCount;
    }
    RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("GetPcapFileAppidFrameCount(): Appid Not Found \r\n")));
    return 0;
}

uint32_t CPcapAnalysis::GetPcapFileAppidSampleCount( int32_t nAppid )
{
    MapAppidSampleMapping::iterator itr = m_mapAppidSampleMapping.find(nAppid);
    if (itr != m_mapAppidSampleMapping.end())
    {
        return itr->second.m_vecAppidSampleMap.size();
    }
    RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("GetPcapFileAppidFrameCount(): Appid Not Found \r\n")));
    return 0;
}

///
/// @brief
///    ��ȡһ��appid����ֵ��Ӧ����ֵ/֡��pcap�е�ӳ����Ϣ
///
const CAppidSampleMapping* CPcapAnalysis::GetPcapFileAppidSampleMapping( int32_t nAppid )
{
    MapAppidSampleMapping::iterator itr = m_mapAppidSampleMapping.find(nAppid);
    if (itr != m_mapAppidSampleMapping.end())
    {
        return &itr->second;
    }
    RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("GetPcapFileAppidSampleMapping(): Appid Not Found \r\n")));
    return NULL;
}

///
/// @brief
///    ��ȡappid�Ĳ�����
///
uint32_t CPcapAnalysis::GetPcapFileAppidSampleRate( int32_t nAppid )
{
    return m_cAppidInfoAnalysis.GetAppidSmpRate(nAppid);
}

///
/// @brief
///    ��ȡpcap��ĳ��appid��Ӧ�Ĳ�������
///
uint32_t CPcapAnalysis::GetPcapFileAppidNoAsdu( int32_t nAppid )
{
    return m_cAppidInfoAnalysis.GetAppidNoAsdu(nAppid);
}

///
/// @brief
///    ��ʼ���÷�Χ�ڲ�����Ĳ��δ���
///
/// @return
///     bool ����true��ʾ���ÿ�ʼ�������ݳɹ�
///
bool CPcapAnalysis::StartPcapFileWaveAnalysis()
{
    RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("StartPcapFileWaveAnalysis()\r\n")));
    if (m_enAnalysisStatus != PCAP_FILE_ANALYSIS_DONE)
    {
        RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("StartPcapFileWaveAnalysis() : PcapFile must analysised before wave analysis\r\n")));
        return false;
    }

    //���η���ǰ����ղ������ݻ���
    _ClearWaveData();

    //����״̬
    m_enAnalysisWaveStatus = PCAP_FILE_ANALYSIS_CALC_WAVE;

    if ((m_nWaveIndexBegin == m_nWaveIndexEnd)
      ||(m_nWaveIndexEnd == 0) 
      || (m_nWaveIndexBegin > m_nWaveIndexEnd))
    {
        RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("StartPcapFileWaveAnalysis(): Wrong Index \r\n")));
        return false;
    }
    MapAppidSampleMapping::iterator itr = m_mapAppidSampleMapping.find(m_nCurrentAppid);
    if (itr == m_mapAppidSampleMapping.end())
    {
        RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("StartPcapFileWaveAnalysis(): Current Appid(%x) Not Found\r\n"), m_nCurrentAppid));
        return false;
    }
    CAppidSampleMapping& rappidSampleMapping = itr->second;
    uint32_t nSampleCount = rappidSampleMapping.m_vecAppidSampleMap.size();
    if (m_nWaveIndexBegin >= nSampleCount || m_nWaveIndexEnd >= nSampleCount)
    {
        RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("StartPcapFileWaveAnalysis(): Index Overflow\r\n")));
        return false;
    }

    if (!m_pPcapFile)
    {
        RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("StartPcapFileWaveAnalysis(): PcapFile not opened !\r\n")));
        m_enAnalysisWaveStatus = PCAP_FILE_NOT_ANALYSISED;
        return false;
    }

    try
    {
        uint32_t nChannelCount = m_cAppidInfoAnalysis.GetAppidChannelCount(m_nCurrentAppid);
        m_dataOscillogram.GenNChannels(nChannelCount, m_nWaveIndexEnd -  m_nWaveIndexBegin + 1);

        //��ʼ��ȡѡȡ��Χ�ڵ�֡��Ϣ
        PCAP_PACKET_HEADER stPcapPckHeader;
        uint32_t nFrameLen = NET_MAX_FRAME_LEN;
        for (uint32_t i = m_nWaveIndexBegin; i <= m_nWaveIndexEnd; ++i)
        {
            if (m_eventCancelWaveAnalysis.Lock(0))
            {
                _NotifyMessage(WM_PCAPFILE_ANALYSIS_MESSAGE, MSG_PCAPFILE_ANALYSIS_USER_CACEL);
                m_enAnalysisWaveStatus =  PCAP_FILE_ANALYSIS_USER_CANCEL;
                RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("User Cacel WaveAnalysis() Actual\r\n")));
                return false;
            }

            SampleIndexPosition stSampleIndexPos = rappidSampleMapping.m_vecAppidSampleMap[i];
            if (PCAP_FILE_SUCCESS == m_pPcapFile->SeekPckToRead(stSampleIndexPos.nPcapFrameIndex))
            {
                m_pPcapFile->ReadOnePckHeader(stPcapPckHeader);
                if (stPcapPckHeader.capLen != stPcapPckHeader.len)
                {
                    continue;
                }
                nFrameLen = stPcapPckHeader.capLen < NET_MAX_FRAME_LEN ? stPcapPckHeader.capLen : NET_MAX_FRAME_LEN;
                m_pPcapFile->ReadOnePckData(m_pFrameBuffer, nFrameLen);
                _HandleFrame4Wave(stSampleIndexPos, m_pFrameBuffer, nFrameLen);
            }
        }

        //����ѡ��֡�Ĳ���ֵ������ϣ���ʼ������Чֵ/��λ��Ϣ
        _CalcChoosenSmpEffVale();

        m_enAnalysisWaveStatus = PCAP_FILE_ANALYSIS_CALC_WAVE_DONE;
        _NotifyMessage(WM_PCAPFILE_ANALYSIS_MESSAGE, MSG_PCAPFILE_WAVE_ANALYSIS_DONE);
        RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("StartPcapFileWaveAnalysis() done\r\n")));
        return true;
    }
    catch (std::bad_alloc& ba)
    {
        m_enAnalysisWaveStatus = PCAP_FILE_NOT_ANALYSISED;
        _NotifyMessage(WM_PCAPFILE_ANALYSIS_MESSAGE, MSG_PCAPFILE_ANALYSIS_FAILED);
        _ClearWaveData();
        RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("StartPcapFileWaveAnalysis() Memory Alloc Failed (%s) : %d\r\n"), ba.what()));
        return false;
    }
}

void CPcapAnalysis::_HandleFrame4Wave(const SampleIndexPosition& stSampleIndexPos, uint8_t* pBuffer, uint32_t nBufferSize )
{
    enumFrameType enFrameType = m_cMsgRapidDecoder.GetMessageType(pBuffer, nBufferSize);
    SMV_VALUE stSmvValue;

    //nAppid���ڱ�ʶ��ͬ�Ŀ��ƿ飬�����9-1/9-2,appidΪ0��ʾ����ʧ��
    //���ft3���ݣ�ʹ��nAppid = 0����ʶ
    uint16_t nAppid = 0;
    if (enFrameType == FRAME_TYPE_NET_9_1)
    {
        uint16_t             nSmpCnt = 0;
        int16_t              pSmvValue[NET_9_1_CHANNELS_PER_FRAME];            ///< ����ͨ������ֵ

        if ( NULL == m_pSav91Msg)
        {
            m_pSav91Msg = CreateSav91MsgImpl();
        }
        if (0 == m_pSav91Msg->Decode(pBuffer, nBufferSize))
        {
            //����ʧ��
            return;
        }

        uint32_t nChannelCount = m_pSav91Msg->GetASDUDefaultSmpsNum();
        if (nChannelCount != NET_9_1_CHANNELS_PER_FRAME)
        {
            RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("9-1 ChannelCount : %d\r\n"), nChannelCount));
            return;
        }

        uint32_t nAsdu = m_pSav91Msg->GetNoASDU();
        if (nAsdu == stSampleIndexPos.nAsduCount)
        {
            m_pSav91Msg->GetSmpCntInASDU(stSampleIndexPos.nAsduIndex, nSmpCnt);
            m_pSav91Msg->GetASDUSmps(stSampleIndexPos.nAsduIndex, reinterpret_cast<uint16_t*>(pSmvValue), nChannelCount);

            stSmvValue.nChannelCount = nChannelCount;
            stSmvValue.nSmpCnt       = nSmpCnt;
            for (uint32_t j = 0; j < nChannelCount; ++j)
            {
                stSmvValue.fSmvValue[j] = static_cast<float>(pSmvValue[j]);
            }

            _AddSmv4CurrentOscillogram(stSmvValue);
        }
    }
    else if (FRAME_TYPE_NET_9_2 == enFrameType)
    {
        uint16_t             nSmpCnt = 0;
        int32_t              pSmvValue[NET_9_2_LE_MAX_CHANNELS_PER_ASDU];            ///< ����ͨ������ֵ

        if ( NULL == m_pSav92Msg)
        {
            m_pSav92Msg = CreateSav92MsgImpl();
        }
        if (0 == m_pSav92Msg->Decode(pBuffer, nBufferSize))
        {
            //����ʧ��
            return;
        }

        uint32_t nAsdu = m_pSav92Msg->GetNoASDU();
        if (nAsdu == stSampleIndexPos.nAsduCount)
        {
            m_pSav92Msg->GetSmpCntInASDU(stSampleIndexPos.nAsduIndex, nSmpCnt);

            uint32_t nChannelCount = m_pSav92Msg->GetASDUSmpsNum(stSampleIndexPos.nAsduIndex);
            if (nChannelCount > NET_9_2_LE_MAX_CHANNELS_PER_ASDU)
            {
                RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("9-2 ChannelCount(%d) : %d\r\n"), nChannelCount));
                return;
            }

            m_pSav92Msg->GetASDUSmps(stSampleIndexPos.nAsduIndex, reinterpret_cast<uint32_t*>(pSmvValue), NULL, nChannelCount);

            stSmvValue.nChannelCount = nChannelCount;
            stSmvValue.nSmpCnt       = nSmpCnt;
            for (uint32_t j = 0; j < nChannelCount; ++j)
            {
                stSmvValue.fSmvValue[j] = static_cast<float>(pSmvValue[j]);
            }

            _AddSmv4CurrentOscillogram(stSmvValue);
        }
    }
    else if (FRAME_TYPE_FT3_GUOWANG == enFrameType)
    {
        GUOWANG_FT3_ASDU_PUBLIC stAsduPublic;
        int16_t                 pSmvValue[GUOWANG_CHANNELS_PER_FRAME];            ///< ����ͨ������ֵ

        if ( NULL == m_pGuoWangFT3Msg)
        {
            m_pGuoWangFT3Msg = CreateGuoWangFT3MsgImpl();
        }
        if (0 == m_pGuoWangFT3Msg->Decode((uint16_t*)pBuffer, nBufferSize/2))
        {
            //����ʧ��
            return;
        }

        uint32_t nChannelCount = m_pGuoWangFT3Msg->GetASDUSmpsNum();
        if (nChannelCount != GUOWANG_CHANNELS_PER_FRAME)
        {
            RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("ft3 guowang ChannelCount : %d\r\n"), nChannelCount));
            return;
        }

        m_pGuoWangFT3Msg->GetASDUPublicInfo(stAsduPublic);
        m_pGuoWangFT3Msg->GetASDUSmps(reinterpret_cast<uint16_t*>(pSmvValue), nChannelCount);

        stSmvValue.nChannelCount = nChannelCount;
        stSmvValue.nSmpCnt       = stAsduPublic.smpCnt;
        for (uint32_t j = 0; j < nChannelCount; ++j)
        {
            stSmvValue.fSmvValue[j] = static_cast<float>(pSmvValue[j]);
        }

        _AddSmv4CurrentOscillogram(stSmvValue);
    }
    else if (FRAME_TYPE_FT3_NANRUI == enFrameType)
    {
        NANRUI_FT3_ASDU_PUBLIC stAsduPublic;
        int16_t                pSmvValue[NANRUI_CHANNELS_PER_FRAME];            ///< ����ͨ������ֵ

        if ( NULL == m_pNanRuiFt3Msg)
        {
            m_pNanRuiFt3Msg = CreateNanRuiFT3MsgImpl();
        }
        if (0 == m_pNanRuiFt3Msg->Decode((uint16_t*)pBuffer, nBufferSize/2))
        {
            //����ʧ��
            return;
        }

        uint32_t nChannelCount = m_pNanRuiFt3Msg->GetASDUSmpsNum();
        if (nChannelCount != NANRUI_CHANNELS_PER_FRAME)
        {
            RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("ft3 nanrui ChannelCount : %d\r\n"), nChannelCount));
            return;
        }

        m_pNanRuiFt3Msg->GetASDUPublicInfo(stAsduPublic);
        m_pNanRuiFt3Msg->GetASDUSmps(reinterpret_cast<uint16_t*>(pSmvValue), nChannelCount);

        stSmvValue.nChannelCount = nChannelCount;
        stSmvValue.nSmpCnt       = stAsduPublic.smpCnt;
        for (uint32_t j = 0; j < nChannelCount; ++j)
        {
            stSmvValue.fSmvValue[j] = static_cast<float>(pSmvValue[j]);
        }

        _AddSmv4CurrentOscillogram(stSmvValue);
    }
}

void CPcapAnalysis::_CalcChoosenSmpEffVale()
{
    uint32_t nChannelCount = m_cAppidInfoAnalysis.GetAppidChannelCount(m_nCurrentAppid);
    COscillogramChannelInfo* pChannelInfoUser = NULL;

    for (uint32_t i = 0; i < nChannelCount; ++i)
    {
        if (m_eventCancelWaveAnalysis.Lock(0))
        {
            _NotifyMessage(WM_PCAPFILE_ANALYSIS_MESSAGE, MSG_PCAPFILE_ANALYSIS_USER_CACEL);
            m_enAnalysisWaveStatus =  PCAP_FILE_ANALYSIS_USER_CANCEL;
            RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("User Cacel WaveAnalysis() Phase\r\n")));
            return;
        }

        pChannelInfoUser = m_dataOscillogram.GetOneChannelInfo(i);
        if (pChannelInfoUser)
        {
             pChannelInfoUser->m_fEffectiveValue = effValueCalc(&pChannelInfoUser->m_vecChannelData[0], pChannelInfoUser->m_vecChannelData.size());
             pChannelInfoUser->m_fPhase = 0.0f;
        }
    }
}

///
/// @brief
///    ��Чֵ����
/// @param[in] 
///    pSampleValues    ������ֵ
///    sampleNum        ��������
///
/// @return
///    float           ��Чֵ
///
float CPcapAnalysis::effValueCalc( const float pSampleValues[], uint32_t sampleNum )
{
    float tmpEffValue = 0.0;

    // ����ƽ����
    for (uint32_t i = 0; i < sampleNum; i++)
    {
        tmpEffValue = addsp_i(tmpEffValue, mpysp_i((float)pSampleValues[i],
            (float)pSampleValues[i]));
    }

    // ���㷽����
    tmpEffValue = sqrtsp_i(divsp_i(tmpEffValue, sampleNum));
    return tmpEffValue;
}

///
/// @brief
///    �˲�
/// @param[in] 
///    ord                  �˲�������,�ݹ̶�Ϊ4
///    a                    �˲�������a_w
///    b                    �˲�������b_w
///    np                   ��������
///    x                    ����
///    leftxlen             y���Ԫ�ظ����������������㣬��ָ��Ϊ0
/// @param[out]
///    y                    �˲�������
///
void CPcapAnalysis::filterx( int ord, const float *a, const float *b, int np, const float *x, float *y, int leftxlen )
{
    int   i;
#if 0
    int   j;
    int   k;
#endif
    int   left;
    float sum;
    const float *pab;
    const float *pabEnd;
    const float *pxy;
    //    const float *pxyEnd;

    for (i=0;i<np; i++)
    {
        left = i-ord;
        if ( left < -leftxlen )
        {
            left = -leftxlen;
        }
        sum=0.0;
        pab = b;
        pabEnd = b+i-left+1;
        pxy = x+i;

        /* for (j=left,k=i-left; j<=i; j++, k--) { */
        /*     sum += b[k]*x[j]; */
        /* } */
        for ( ;pab<pabEnd; )
        {
            sum += *pab++ * *pxy--;
        }
        pxy = y+i-1;
        pab = a+1;
        pabEnd = a+i-left+1;
        /* for (j=left,k=i-left; j<i; j++, k-- ) { */
        /* 	sum -= a[k]*y[j]; */
        /* } */
        for ( ;pab<pabEnd; )
        {
            sum -= *pab++ * *pxy--;
        }
        y[ i ] = sum;
    }
}

///
/// @brief
///    ���ݲ����ʷ��ض�Ӧ���˲�������a,b����Ҫ���ݵ�����
///
uint32_t CPcapAnalysis::GetSmpRateIndex( uint32_t nSmpRate )
{
    uint32_t nIndex = 0;
    switch(nSmpRate)
    {
    case 4000:
        nIndex = 0;
        break;
    case 5000:
        nIndex = 1;
        break;
    case 8000:
        nIndex = 2;
        break;
    case 10000:
        nIndex = 3;
        break;
    case 12800:
        nIndex = 4;
        break;
    default:
        nIndex = 0;
        break;
    }
    return nIndex;
}

///
/// @brief
///    ��ȡ����ȥ�������Ϣ
///
/// @param[in]  pData  ���������ݵ�ַ
/// @param[in]  nCount ���������ݸ���
/// @param[out] stZeroCrossPoint ��ȡ���Ĺ������Ϣ
///
/// @return
///     bool    ��ȡ�ɹ�����true��ʧ�ܷ���false
///
bool CPcapAnalysis::GetZeroCrossingPoint( float *pData, uint32_t nCount, ZeroCrossPoint& stZeroCrossPoint )
{
    if (pData == NULL)
    {
        return false;
    }
    std::vector<ZeroCrossPoint> vecZeroPoints;

    //4k������Ĭ������������ֱ�ӵ���λ��
    float fPhasePerSmp = 360.0/79.0;
    uint32_t nSmpRate = m_cAppidInfoAnalysis.GetAppidSmpRate(m_nCurrentAppid);
    if (nSmpRate != 0)
    {
        fPhasePerSmp = 360.0f/(nSmpRate/50 - 1);
    }
    ZeroCrossPoint st0CsPt;
    for (uint32_t i=0; i<nCount - 1; ++i)
    {
        bool bFound = false;
        float fabs1 = static_cast<float>(fabs(pData[i]));
        float fabs2 = static_cast<float>(fabs(pData[i+1]));
        //�����Ƕ�Ϊ0��
        if (pData[i] <= 0 && pData[i+1] > 0)
        {
            st0CsPt.fPhase = 0.0f;
            bFound = true;
            if (fabs1 < FLOAT_ZERO_ABS)
            {
                st0CsPt.bPreciseZero = true;
                st0CsPt.fSmpValue = pData[i];
                st0CsPt.nIndex = i;
            }
            else if (fabs2 < FLOAT_ZERO_ABS)
            {
                st0CsPt.bPreciseZero = true;
                st0CsPt.fSmpValue = pData[i+1];
                st0CsPt.nIndex = i+1;
            }
            else if (fabs1 < fabs2)
            {
                st0CsPt.bPreciseZero = false;
                st0CsPt.fSmpValue = pData[i];
                st0CsPt.nIndex = i;
                st0CsPt.fPhase = 0 - fabs1/(fabs1+fabs2)*fPhasePerSmp;
            }
            else if (fabs1 > fabs2)
            {
                st0CsPt.bPreciseZero = false;
                st0CsPt.fSmpValue = pData[i+1];
                st0CsPt.nIndex = i+1;
                st0CsPt.fPhase = fabs2/(fabs1+fabs2)*fPhasePerSmp;
            }

        }
        //�����Ƕ�Ϊ180��
        else if (pData[i] >= 0 && pData[i+1] <0)
        {
            st0CsPt.fPhase = 180.0f;
            bFound = true;
            if (fabs1 < FLOAT_ZERO_ABS)
            {
                st0CsPt.bPreciseZero = true;
                st0CsPt.fSmpValue = pData[i];
                st0CsPt.nIndex = i;
            }
            else if (fabs2 < FLOAT_ZERO_ABS)
            {
                st0CsPt.bPreciseZero = true;
                st0CsPt.fSmpValue = pData[i+1];
                st0CsPt.nIndex = i+1;
            }
            else if (fabs1 < fabs2)
            {
                st0CsPt.bPreciseZero = false;
                st0CsPt.fSmpValue = pData[i];
                st0CsPt.nIndex = i;
                st0CsPt.fPhase = 180.0f - fabs1/(fabs1+fabs2)*fPhasePerSmp;
            }
            else if (fabs1 > fabs2)
            {
                st0CsPt.bPreciseZero = false;
                st0CsPt.fSmpValue = pData[i+1];
                st0CsPt.nIndex = i+1;
                st0CsPt.fPhase = -180.0f + fabs2/(fabs1+fabs2)*fPhasePerSmp;
            }
        }

        if (bFound)
        {
            //�������з��־�ȷ����㣬ֱ�ӷ��ض�Ӧ����
            if (st0CsPt.bPreciseZero)
            {
                stZeroCrossPoint = st0CsPt;
                return true;
            }
            vecZeroPoints.push_back(st0CsPt);
        }
    }

    //��������ȡ����Ϊ��ȷ�Ĺ������Ϣ
    uint32_t nVecSize = vecZeroPoints.size();
    if (nVecSize > 0)
    {
        //���������һ����Ϣ����
        stZeroCrossPoint = vecZeroPoints[0];
        return true;
    }
    return false;
}

///
/// @brief
///    ����λ�Ƕ��������� -180--180��Χ֮��
///
float CPcapAnalysis::PhaseNormaLizing( float fPhaseSrc )
{
    if (fPhaseSrc > -180.0f && fPhaseSrc <= 180.0f )
    {
        return fPhaseSrc;
    }
    if (fPhaseSrc >= 360.0f || fPhaseSrc <= -360.0f)
    {
        float ftmp = fPhaseSrc - (int)fPhaseSrc;
        fPhaseSrc = (int)fPhaseSrc % 360 + ftmp;
    }
    if (fPhaseSrc > 180.0 && fPhaseSrc < 360.0)
    {
        fPhaseSrc = fPhaseSrc - 360.0f;
    }
    if (fPhaseSrc > -360.0f && fPhaseSrc <= -180.0f)
    {
        fPhaseSrc = fPhaseSrc + 360.0f;
    }
    return fPhaseSrc;
}

///
/// @brief
///    ֹͣpcap�ļ�����ͼ���ݷ���
///
/// @return
///     bool    ֹͣ�ɹ�����true��ʧ��false
///
bool CPcapAnalysis::StopPcapFileWaveAnalysis()
{
    RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("m_eventCancelWaveAnalysis.SetEvent(); \r\n")));
    m_eventCancelWaveAnalysis.SetEvent();
    uint32_t n = 50;
    while (n > 0)
    {
        if (m_enAnalysisWaveStatus == PCAP_FILE_ANALYSIS_CALC_WAVE_DONE ||
            m_enAnalysisWaveStatus == PCAP_FILE_ANALYSIS_USER_CANCEL)
        {
            return true;
        }
        Sleep(10);
        n--;
    }
    return false;
}

///
/// @brief
///    ��ȡָ��������Χ�ڲ���ͼ/��λ/��Чֵ����
///
/// @param[out]  pData   �ⲿ��������ݶ���ָ�룬�������ָ��ָ����������
///
/// @return
///     bool   ���ݻ�ȡ�ɹ�����true��pData�б����ȡ��������
///
/// @note
///     pData ָ����ڴ������ɵ������Լ�����
///
bool CPcapAnalysis::GetPcapFileWaveOscillogramData( CDataOscillogram* pData )
{
    if (m_nCurrentAppid == -1 || !pData || m_enAnalysisWaveStatus != PCAP_FILE_ANALYSIS_CALC_WAVE_DONE)
    {
        return false;
    }

    pData->ClearChannelVec();
    uint32_t nChannelCount = m_cAppidInfoAnalysis.GetAppidChannelCount(m_nCurrentAppid);
    pData->GenNChannels(nChannelCount);

    for (uint32_t i=0; i < nChannelCount; ++i)
    {
        pData->GetOneChannelInfo(i)->m_fEffectiveValue = m_dataOscillogram.GetOneChannelInfo(i)->m_fEffectiveValue;
        pData->GetOneChannelInfo(i)->m_fPhase = m_dataOscillogram.GetOneChannelInfo(i)->m_fPhase;
        pData->GetOneChannelInfo(i)->m_vecChannelData.assign(m_dataOscillogram.GetOneChannelInfo(i)->m_vecChannelData.begin(),
                                                             m_dataOscillogram.GetOneChannelInfo(i)->m_vecChannelData.end());
    }
    return true;
}

///
/// @brief
///    ��յ�ǰ���ƿ����Ϣ���統ǰ���ƿ�Ĳ�����Ϣ��
///
void CPcapAnalysis::_ClearCurrenAppidData()
{
    m_dataOscillogram.ClearChannelVec();
}

///
/// @brief
///    ֪ͨ���������ɸ�������
///
bool CPcapAnalysis::_NotifyMessage( uint32_t nMessageId, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/ )
{
    HWND hWnd = NULL;
    if (m_pNotifyWnd)
    {
        hWnd = m_pNotifyWnd->GetSafeHwnd();
    }
    if (::IsWindow(hWnd))
    {
        return ::PostMessage(hWnd, nMessageId, wParam, lParam) != 0 ? true : false;
    }
    return false;
}

///
/// @brief
///    pcap�ļ���
///
bool CPcapAnalysis::_OpenPcapFile()
{
    if (m_pPcapFile == NULL)
    {
        if (m_strPcapFile.empty())
        {
            _NotifyMessage(WM_PCAPFILE_ANALYSIS_MESSAGE, MSG_PCAPFILE_OPEN_FAILED);
            return false;
        }
        m_pPcapFile = CreatePcapFileImpl();
        if ( PCAP_FILE_SUCCESS != m_pPcapFile->Open(m_strPcapFile, PFM_READ) )
        {
            //�ļ���ʧ��
            ReleasePcapFileImpl(m_pPcapFile);
            m_pPcapFile = NULL;
            _NotifyMessage(WM_PCAPFILE_ANALYSIS_MESSAGE, MSG_PCAPFILE_OPEN_FAILED);
            return false;
        }
    }
    return true;
}

///
/// @brief
///    �ر�pcap�ļ�
///
bool CPcapAnalysis::_ClosePcapFile()
{
    if (m_pPcapFile)
    {
        m_pPcapFile->Close();
        ReleasePcapFileImpl(m_pPcapFile);
        m_pPcapFile = NULL;
    }
    return true;
}

///
/// @brief
///    ��ȡpcap�ļ���Ӧ��������֡���쳣��Ϣ
///
FrameStatus CPcapAnalysis::GetPcapFileFrameIndexStatus( uint32_t nIndex )
{
    MapPcapFileFrameIndexStatus::iterator itr = m_mapPcapFileFrameIndexStatus.find(nIndex);
    if (itr != m_mapPcapFileFrameIndexStatus.end())
    {
        return itr->second;
    }
    RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("FrameIndex %d Not Found \r\n"), nIndex));
    return FrameStatus(0);
}

///
/// @brief
///    ��ȡĳ�����ƿ��ͨ����Ŀ
///
uint32_t CPcapAnalysis::GetPcapFileAppidChannelCount( int32_t nAppid )
{
    return m_cAppidInfoAnalysis.GetAppidChannelCount(nAppid);
}

///
/// @brief
///    ���ݲ���ֵ������֡��ӳ����Ϣ���������appid����ɢ�ȣ�������Ϣ
///
void CPcapAnalysis::_CalcAllAppidDispersion()
{
    MapAppidSampleMapping::iterator itr = m_mapAppidSampleMapping.begin();
    while(itr != m_mapAppidSampleMapping.end())
    {
        int32_t nAppid = itr->first;
        CAppidSampleMapping& rAppidSampleMapping = itr->second;

        CPcapFileSmvSmpStatus* pSmvFrameStatistic = NULL;
        MapAppidSmvStatistics::iterator itrMap = m_mapSmvStatistics.find(nAppid);
        if (itrMap != m_mapSmvStatistics.end())
        {
            pSmvFrameStatistic = &(itrMap->second);
        }

        if (!pSmvFrameStatistic)
        {
            ++itr;
            continue;
        }

        uint32_t nAsdu = m_cAppidInfoAnalysis.GetAppidNoAsdu(nAppid);
        uint32_t nSmpRate = m_cAppidInfoAnalysis.GetAppidSmpRate(nAppid);

        VecAppidFrameIndex2PcapFrameIndex::iterator itrVec = rAppidSampleMapping.m_vecAppidFrameIndexMap.begin();
        while(itrVec != rAppidSampleMapping.m_vecAppidFrameIndexMap.end())
        {
            //������ɢ��
            _HandleFrameTimeDispersion(nAppid, itrVec->stTimeStamp);
            //���㶶��
            pSmvFrameStatistic->ParseFrameTimeStamp(itrVec->stTimeStamp, nAsdu, nSmpRate);

            ++itrVec;
        }

        ++itr;
    }
    
}

///
/// @brief
///    ���ݲ���ֵ������֡��ӳ����Ϣ���������appid����ɢ�ȣ�������Ϣ
///
void CPcapAnalysis::_AddSmv4CurrentOscillogram( const SMV_VALUE& stSmvValue )
{
    COscillogramChannelInfo* pChannelInfoUser = NULL;
    for (uint32_t i = 0; i < stSmvValue.nChannelCount; ++i)
    {
        pChannelInfoUser =  m_dataOscillogram.GetOneChannelInfo(i);
        if (pChannelInfoUser)
        {
            pChannelInfoUser->m_vecChannelData.push_back(stSmvValue.fSmvValue[i]);
        }
    }
}

///
/// @brief
///    �߳���ڣ�����pcap�ļ�ͳ��
///
UINT __cdecl PcapFileAnalysisProc( LPVOID pParam )
{
    // ������ָ��
    if (NULL == pParam)
    {
        return FALSE;
    }

    //��ʼ�ļ�����
    CPcapAnalysis* pPcapFileAnalysis = reinterpret_cast<CPcapAnalysis*>(pParam);
    DWORD dwStart = GetTickCount();
    if (pPcapFileAnalysis && !pPcapFileAnalysis->StartPcapFileStatisticsAnalysis())
    {
        RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("PcapFileAnalysisProc() Failed (%d)\r\n"), GetTickCount() - dwStart));
        return FALSE;
    }
    RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("PcapFileAnalysisProc() Success (%d)\r\n"), GetTickCount() - dwStart));
    return TRUE;
}

UINT __cdecl PcapFileWaveAnalysisProc( LPVOID pParam )
{
    // ������ָ��
    if (NULL == pParam)
    {
        return FALSE;
    }

    //��ʼ�ļ�����
    CPcapAnalysis* pPcapFileAnalysis = reinterpret_cast<CPcapAnalysis*>(pParam);
    DWORD dwStart = GetTickCount();
    if (pPcapFileAnalysis && !pPcapFileAnalysis->StartPcapFileWaveAnalysis())
    {
        RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("StartPcapFileWaveAnalysis() Failed (%d)\r\n"), GetTickCount() - dwStart));
        return FALSE;
    }
    RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("StartPcapFileWaveAnalysis() Success (%d)\r\n"), GetTickCount() - dwStart));
    return TRUE;
}

CAppidSampleMapping::CAppidSampleMapping()
{

}

///
/// @brief
///    ����һ����������Ϣ
///
void CAppidSampleMapping::ResetData()
{
    VecAppidSampleMapping().swap(m_vecAppidSampleMap);
    VecAppidFrameIndex2PcapFrameIndex().swap(m_vecAppidFrameIndexMap);
    m_vecAppidSampleMap.clear();
    m_vecAppidFrameIndexMap.clear();
}

CAppidSampleMapping::~CAppidSampleMapping()
{
    ResetData();
}

///
/// @brief
///    ����һ����������Ϣ
///
/// @param[in]  stSamplePosition ��ʾ���������Ϣ��
///             ��ҪnPcapFrameIndex, nAsduCount, nAsduIndex�����ֶ���Ϣ
///             ʣ�������ֶΣ���Ҫ���ݵ�ǰ���ƿ�Ĳ����������֡��������
///
void CAppidSampleMapping::ParseSampleInfo(SampleIndexPosition stSamplePosition, const FrameTimeStamp& stTimeStamp, uint32_t nFrameLen)
{
    ///
    /// ������ӳ����Ϣ
    ///
    //��appid�ĵ�һ��������
    if (m_vecAppidSampleMap.size() == 0)
    {
        stSamplePosition.nAppidSampleIndex = 0;
        stSamplePosition.nAppidFrameIndex = 0;
    }
    else
    //���ǵ�һ��������
    {
        //��ȡ��������һ����������Ϣ
        const SampleIndexPosition& rLastSampleInfo = m_vecAppidSampleMap.back();

        //���������ϴλ����ϼ�1
        stSamplePosition.nAppidSampleIndex = rLastSampleInfo.nAppidSampleIndex + 1;
        //֡��ſ��Ƿ�����һ֡���ݣ��������֡������֡���
        if (rLastSampleInfo.nPcapFrameIndex == stSamplePosition.nPcapFrameIndex)
        {
            stSamplePosition.nAppidFrameIndex = rLastSampleInfo.nAppidFrameIndex;
        }
        else
        {
            stSamplePosition.nAppidFrameIndex = rLastSampleInfo.nAppidFrameIndex + 1;
        }
    }
    m_vecAppidSampleMap.push_back(stSamplePosition);

    ///
    /// ����֡ӳ����Ϣ
    ///
    FrameIndexInfo stFrameInfo;
    stFrameInfo.nPcapFileFrameIndex = stSamplePosition.nPcapFrameIndex;
    stFrameInfo.nFramelen           = nFrameLen;
    stFrameInfo.stTimeStamp         = stTimeStamp;
    if (m_vecAppidFrameIndexMap.size() == 0)
    {
        m_vecAppidFrameIndexMap.push_back(stFrameInfo);
    }
    else
    {
        if (m_vecAppidFrameIndexMap.back().nPcapFileFrameIndex != stSamplePosition.nPcapFrameIndex)
        {
            m_vecAppidFrameIndexMap.push_back(stFrameInfo);
        }
    }

    return;
}


///
/// @brief
///    ����ÿһ֡����Ϣ
///
void CAppidInfoAnalysis::ParseAppidInfo( FrameTimeStamp stFrameTime, int32_t nAppid, enumFrameType enFrameType, uint32_t nAsdu, uint32_t nChannelCount, uint8_t* pBuff, uint32_t nBufLen)
{
    if (enFrameType == FRAME_TYPE_UNKOWN)
    {
        return;
    }
    AppidInfo* pAppidInfo = NULL;
    uint32_t nSmpRate = 4000;
    MapAppidInfo::iterator itr = m_mapAppidInfoCalcInfo.find(nAppid);
    if (itr == m_mapAppidInfoCalcInfo.end())
    {
        m_mapAppidInfoCalcInfo[nAppid] = AppidInfo();
        itr = m_mapAppidInfoCalcInfo.find(nAppid);
        if (itr != m_mapAppidInfoCalcInfo.end())
        {
            pAppidInfo = &itr->second;
        }

        if (enFrameType == FRAME_TYPE_NET_9_1)
        {
            Sav91Msg* pSav91Msg = CreateSav91MsgImpl();
            if (0 != pSav91Msg->Decode(pBuff, nBufLen))
            {
                SAV91_ASDU_PUBLIC    stSav91AsduPublic;
                pSav91Msg->GetASDUPublicInfo(0, stSav91AsduPublic);
                nSmpRate = stSav91AsduPublic.smpRate != 0 ? stSav91AsduPublic.smpRate * DSM_CONFIG_DEFAULT_ELEC_FREQUENCY : 12800;
            }
            ReleaseSav91MsgImpl(pSav91Msg);
        }
        else if (enFrameType == FRAME_TYPE_FT3_NANRUI)
        {
            NanRuiFT3Msg* pMsg = CreateNanRuiFT3MsgImpl();
            if (0 != pMsg->Decode(reinterpret_cast<uint16_t*>(pBuff), nBufLen/2))
            {
                NANRUI_FT3_ASDU_PUBLIC stPub;
                pMsg->GetASDUPublicInfo(stPub);
                nSmpRate = stPub.smpRate != 0 ? stPub.smpRate * DSM_CONFIG_DEFAULT_ELEC_FREQUENCY : 12800;
            }
            ReleaseNanRuiFT3MsgImpl(pMsg);
        }
    }
    else
    {
        pAppidInfo = &itr->second;
    }

    if (!pAppidInfo)
    {
        return;
    }

    switch(enFrameType)
    {
    case FRAME_TYPE_NET_9_1:
    case FRAME_TYPE_NET_9_2:
    case FRAME_TYPE_FT3_GUOWANG:
    case FRAME_TYPE_FT3_NANRUI:
        {
            //��һ֡
            if (pAppidInfo->stFirstTimeStamp.ts_sec == 0 && pAppidInfo->stFirstTimeStamp.ts_usec == 0)
            {
                pAppidInfo->stFirstTimeStamp = stFrameTime;
                pAppidInfo->stLastTimeStamp  = stFrameTime;
                pAppidInfo->nAppid = nAppid;
                pAppidInfo->nAsdu  = nAsdu;
                pAppidInfo->nChannelCount = nChannelCount;
                pAppidInfo->enFrameType = enFrameType;
                pAppidInfo->nAppidSmpCount += nAsdu;
                pAppidInfo->nSmpRate = nSmpRate;
            }
            else
            {
                pAppidInfo->stLastTimeStamp = stFrameTime;
                pAppidInfo->nAppidSmpCount += nAsdu;
            }
        }
        break;
    case FRAME_TYPE_NET_GOOSE:
        {
            //��һ֡
            if (pAppidInfo->stFirstTimeStamp.ts_sec == 0 && pAppidInfo->stFirstTimeStamp.ts_usec == 0)
            {
                pAppidInfo->enFrameType = enFrameType;
            }
        }
        break;
    default:
        break;
    }
}

///
/// @brief
///    ����֡������ϣ������������Ϣ
///
void CAppidInfoAnalysis::CalcSmpRate()
{
    MapAppidInfo::iterator itr = m_mapAppidInfoCalcInfo.begin();
    while(itr != m_mapAppidInfoCalcInfo.end())
    {
        AppidInfo& rAppidInfo = itr->second;
        int64_t nSmpRate = 0;
        //ʱ��΢����
        int64_t nTimeDiff = rAppidInfo.stLastTimeStamp - rAppidInfo.stFirstTimeStamp;
        if (nTimeDiff > 0)
        {
            nSmpRate = (int64_t)rAppidInfo.nAppidSmpCount*1000000/nTimeDiff;
        }
        switch(rAppidInfo.enFrameType)
        {
        //9-1 �������д��ڲ�������Ϣ��������ﲻ�����
        case FRAME_TYPE_NET_9_1:
        case FRAME_TYPE_FT3_NANRUI:
            break;
        case FRAME_TYPE_NET_9_2:
            {
                if (nSmpRate >= 0 && nSmpRate <= 6500)
                {
                    rAppidInfo.nSmpRate = 4000;
                }
                else
                {
                    rAppidInfo.nSmpRate = 12800;
                }
            }
            break;
        case FRAME_TYPE_FT3_GUOWANG:
            {
                if (nSmpRate >= 0 && nSmpRate <= 4500)
                {
                    rAppidInfo.nSmpRate = 4000;
                }
                else if (nSmpRate > 4500 && nSmpRate <= 5500)
                {
                    rAppidInfo.nSmpRate = 5000;
                }
                else if (nSmpRate > 5500 && nSmpRate <= 9000)
                {
                    rAppidInfo.nSmpRate = 8000;
                }
                else if (nSmpRate > 9000 && nSmpRate < 11000)
                {
                    rAppidInfo.nSmpRate = 10000;
                }
                else
                {
                    rAppidInfo.nSmpRate = 12800;
                }
            }
            break;
        default:
            break;
        }
        ++itr;
    }
}

///
/// @brief
///    ��ȡĳ��appid�Ĳ�����
///
uint32_t CAppidInfoAnalysis::GetAppidSmpRate( int32_t nAppid )
{
    MapAppidInfo::iterator itr = m_mapAppidInfoCalcInfo.find(nAppid);
    if (itr != m_mapAppidInfoCalcInfo.end())
    {
        return itr->second.nSmpRate;
    }
    RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("GetAppidSmpRate nAppid(%x) Not Found \r\n"), nAppid));
    return 0;
}

///
/// @brief
///    ��ȡĳ�����ƿ��asdu����
///
uint32_t CAppidInfoAnalysis::GetAppidNoAsdu( int32_t nAppid )
{
    MapAppidInfo::iterator itr = m_mapAppidInfoCalcInfo.find(nAppid);
    if (itr != m_mapAppidInfoCalcInfo.end())
    {
        return itr->second.nAsdu;
    }
    RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("GetAppidNoAsdu nAppid(%x) Not Found \r\n"), nAppid));
    return 0;
}

///
/// @brief
///    ��ȡĳ�����ƿ������֡����
///
enumFrameType CAppidInfoAnalysis::GetAppidFrameType( int32_t nAppid )
{
    MapAppidInfo::iterator itr = m_mapAppidInfoCalcInfo.find(nAppid);
    if (itr != m_mapAppidInfoCalcInfo.end())
    {
        return itr->second.enFrameType;
    }

    RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("GetPcapFileAppidFrameType nAppid(%x) Not Found \r\n"), nAppid));
    return FRAME_TYPE_UNKOWN;
}

///
/// @brief
///    ��ȡĳ�����ƿ��ͨ����Ŀ
///
uint32_t CAppidInfoAnalysis::GetAppidChannelCount( int32_t nAppid )
{
    MapAppidInfo::iterator itr = m_mapAppidInfoCalcInfo.find(nAppid);
    if (itr != m_mapAppidInfoCalcInfo.end())
    {
        return itr->second.nChannelCount;
    }

    RETAILMSG(PCAP_FILE_ANALYSIS_OUTPUT_LOG, (_T("GetAppidChannelCount nAppid(%x) Not Found \r\n"), nAppid));
    return 0;
}

///
/// @brief
///    ����һ֡���ģ����ر��ĸ�֡״̬��Ϣ
///
FrameStatus CPcapFileGooseFrameStatus::ParseGooseFrame( uint32_t nTTL, FrameTimeStamp& stFrameTimeStamp, uint8_t nGooseUtcTimeQuality, int32_t nStNum, int32_t nSqNum )
{
    FrameStatus stFrameStatus;

    //ͳ����֡��
    m_stGooseFrameStatistics.m_nTotalFrameCount++;

    //ͳ��ttl��Ч���
    if (nTTL == 0)
    {
        stFrameStatus.SetValue(FRAME_STATUS_GOOSE_TTL_INVALID);
        m_stGooseFrameStatistics.m_nInvalidTTLCount++;
    }

    //ͳ��ͨ�ų�ʱ/�ж�
    {
        if (m_stLastFrameTimeStamp.ts_sec != 0 || m_stLastFrameTimeStamp.ts_usec != 0)
        {
            int64_t nTTLms  = nTTL * 1000;
            int64_t nTimeDiff = stFrameTimeStamp - m_stLastFrameTimeStamp;
            if (nTimeDiff > nTTLms && nTimeDiff <= nTTLms * 2)
            {
                stFrameStatus.SetValue(FRAME_STATUS_GOOSE_FRAME_LOST);
                m_stGooseFrameStatistics.m_nLostDataGramCount++;
            }
            else if (nTimeDiff > nTTLms * 2)
            {
                stFrameStatus.SetValue(FRAME_STATUS_GOOSE_COMM_BREAK);
                m_stGooseFrameStatistics.m_nCommBreakCount++;
            }
        }
        //������ϣ��������һ��ʱ���
        m_stLastFrameTimeStamp = stFrameTimeStamp;
    }

    //ʱ�ӹ���/δͬ����Ϣ
    {
        Iec61850TimeQuality tmQuality(nGooseUtcTimeQuality);
        //ʱ�ӹ���
        if (tmQuality.IsClockFailure())
        {
            stFrameStatus.SetValue(FRAME_STATUS_GOOSE_CLK_FAILED);
            m_stGooseFrameStatistics.m_nTimeQualityFaultCount++;
        }
        //ʱ��δͬ��
        if (tmQuality.IsClockNotSync())
        {
            stFrameStatus.SetValue(FRAME_STATUS_GOOSE_CLK_NOT_SYNC);
            m_stGooseFrameStatistics.m_nTimeQualityNotSyncCount++;
        }
    }

    //st/sqnumber����
    {
        if (m_nSqNum != -1 && m_nStNum != -1)
        {
            if (m_nStNum == nStNum)
            {
                //sq��֡
                if(nSqNum > m_nSqNum + 1)
                {
                    stFrameStatus.SetValue(FRAME_STATUS_GOOSE_SQ_LOST);
                    m_stGooseFrameStatistics.m_nLostFrameCount++;
                }
                //����
                else if (nSqNum !=0 && m_nSqNum > nSqNum)
                {
                    stFrameStatus.SetValue(FRAME_STATUS_GOOSE_SQ_MIS_ORDER);
                    m_stGooseFrameStatistics.m_nMisOrderFrameCount++;
                }
                //�ط�
                else if (nSqNum == m_nSqNum)
                {
                    stFrameStatus.SetValue(FRAME_STATUS_GOOSE_SQ_RETRY);
                    m_stGooseFrameStatistics.m_nRetryFrameCount++;
                }
            }
            //st��֡
            else if (nStNum > m_nStNum + 1)
            {
                stFrameStatus.SetValue(FRAME_STATUS_GOOSE_ST_LOST);
                m_stGooseFrameStatistics.m_nStLostFrameCount++;
            }
            else if (nStNum == m_nSqNum + 1)
            {
                //st��������ʱҪ�ж�sq,�����ʱsq���Ǵ�0����1��ʼ������Ϊ�ж�֡
                if (!(nSqNum == 0 || nSqNum == 1))
                {
                    stFrameStatus.SetValue(FRAME_STATUS_GOOSE_SQ_LOST);
                    m_stGooseFrameStatistics.m_nLostFrameCount++;
                }
            }
            //st����
            else if (nStNum != 0 && m_nStNum > nStNum)
            {
                stFrameStatus.SetValue(FRAME_STATUS_GOOSE_ST_MIS_ORDER);
                m_stGooseFrameStatistics.m_nStMisOrderFrameCount++;
            }
        }

        //������ϣ��������һ�ε�st��sq����
        m_nStNum = nStNum;
        m_nSqNum  = nSqNum;
    }

    return stFrameStatus;
}

///
/// @brief
///    ��������һ����������Ϣ
///
FrameStatus CPcapFileSmvSmpStatus::ParseSamplePoint(int32_t nPcapFileFrameIndex, CBaseDetectItem* pDetectItem, int32_t nSmpIndex, CSmvCtrlBlkQuality& smvCtrlBlkQuality )
{
    FrameStatus stFrameStatus;

    //��֡��
    {
        if (nPcapFileFrameIndex != m_nLastPcapFileFrameIndex)
        {
            m_stSmvFrameStatistics.m_nTotalFrameCount++;
        }
        m_nLastPcapFileFrameIndex = nPcapFileFrameIndex;
    }

    {
        if (m_nLastSampleCountIndex != -1)
        {
            if (nSmpIndex == m_nLastSampleCountIndex + 1)
                //����
            {
            }
            //�ط�
            else if (nSmpIndex == m_nLastSampleCountIndex)
            {
                stFrameStatus.SetValue(FRAME_STATUS_SMP_RETRY);
                m_stSmvFrameStatistics.m_nRetryFrameCount++;
            }
            //��֡
            else if (nSmpIndex > m_nLastSampleCountIndex + 1 )
            {
                stFrameStatus.SetValue(FRAME_STATUS_SMP_LOST);
                m_stSmvFrameStatistics.m_nLostFrameCount++;
            }
            //����
            else if (nSmpIndex < m_nLastSampleCountIndex && nSmpIndex != 0)
            {
                stFrameStatus.SetValue(FRAME_STATUS_SMP_MIS_ORDER);
                m_stSmvFrameStatistics.m_nMisOrderFrameCount++;
            }
        }
        m_nLastSampleCountIndex = nSmpIndex;
        stFrameStatus.vecSmpCnt.push_back(nSmpIndex);
    }

    {
        //ʧ��
        if (!smvCtrlBlkQuality.IsMuSync())
        {
            stFrameStatus.SetValue(FRAME_STATUS_SMP_LOST_SYNC);
            m_stSmvFrameStatistics.m_nLostSyncFrameCount++;
        }

        //����
        if (smvCtrlBlkQuality.GetCtrlBlk()->enFrameType == FRAME_TYPE_NET_9_2)
        {
            for (uint32_t i = 0; i < smvCtrlBlkQuality.GetCtrlBlk()->nChannelCount; ++i)
            {
                if (smvCtrlBlkQuality.IsChannelTest(i))
                {
                    stFrameStatus.SetValue(FRAME_STATUS_SMP_TEST);
                    m_stSmvFrameStatistics.m_nTestFrameCount++;
                    break;
                }
            }
            //Ʒ����Ч����
            for (uint32_t i = 0; i < smvCtrlBlkQuality.GetCtrlBlk()->nChannelCount; ++i)
            {
                if (!smvCtrlBlkQuality.IsChannelValid(i))
                {
                    stFrameStatus.SetValue(FRAME_STATUS_SMP_Q_INVALID);
                    m_stSmvFrameStatistics.m_nQualityInvalidFrameCount++;
                    break;
                }
            }
        }
        else
        {
            if (smvCtrlBlkQuality.IsChannelTest())
            {
                stFrameStatus.SetValue(FRAME_STATUS_SMP_TEST);
                m_stSmvFrameStatistics.m_nTestFrameCount++;
            }
        }
    }

    return stFrameStatus;
}

///
/// @brief
///    ��������һ��֡ʱ�����Ϣ
///
void CPcapFileSmvSmpStatus::ParseFrameTimeStamp(FrameTimeStamp& stFrameTimeStamp, uint32_t nAsdu/* = 1*/, uint32_t nSmpRate/* = 4000*/)
{
    if (m_stLastFrameTimeStamp.ts_sec != 0 || m_stLastFrameTimeStamp.ts_usec != 0)
    {
        if(_IsTimeStampJitter(m_stLastFrameTimeStamp, stFrameTimeStamp, nAsdu, nSmpRate))
        {
            m_stSmvFrameStatistics.m_nJitterFrameCount++;
        } 
    }
     
    m_stLastFrameTimeStamp = stFrameTimeStamp;
}


///
/// @brief
///    ��������ʱ���ʱ���ж�ʱ���Ƿ񶶶���ʱ������ƫ�������������10us��
///
bool CPcapFileSmvSmpStatus::_IsTimeStampJitter( const FrameTimeStamp& tm1, const FrameTimeStamp& tm2, uint32_t nAsdu/* = 1*/, uint32_t nSmpRate/* = 4000*/)
{
    //����asdu,�����ʼ�����������ʱ����
    if (nSmpRate == 0)
    {
        nSmpRate = 4000;
    }
    if (nAsdu == 0)
    {
        nAsdu = 1;
    }
    uint32_t nNormalTime = nAsdu * 1000000/nSmpRate;

    int64_t ndiff = _abs64(tm2 - tm1);
#ifdef SU_FEI
    if (ndiff > nNormalTime + 10)
    {
        return true;
    }
#else
	if ((ndiff > nNormalTime + 10) || (ndiff < nNormalTime - 10))
	{
		return true;
	}

#endif
    return false;
}
