// ParamConfigTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include "src\service\datalayer\utility\data_utility.h"
#include "src\service\datalayer\config\ParamConfigBasic.h"
#include "src\service\datalayer\config\ParamConfigSMVSend.h"
#include "src\service\datalayer\config\ParamConfigGOOSESend.h"
#include "src\service\datalayer\config\ParamConfigGOOSERecv.h"
#include "src\service\datalayer\scl\SCLManager.h"
#include "src\service\datalayer\config\ParamconfigManualTest.h"
#include "src\service\datalayer\config\ParamconfigStateSequence.h"

#define PARAM_CONFIG_SMV_SEND       L"paramconfig_smv_send.xml"         ///< SMV���������ļ���
#define PARAM_CONFIG_GOOSE_SEND     L"paramconfig_goose_send.xml"       ///< GOOSE���������ļ���
#define PARAM_CONFIG_GOOSE_RECV     L"paramconfig_goose_recv.xml"       ///< GOOSE���������ļ���
#define PARAM_GuoWangMUTest_SCD     L"GuoWangMUTest.SCD"                ///< ����ȫվ�����ļ�

int _tmain(int argc, _TCHAR* argv[])
{
#ifdef _DEBUG
    CheckMemoryLeak;
#endif

    // ��������
    {
        CParamConfigBasic* m_pParamConfigBasic = CParamConfigBasic::getInstance();
        if (NULL == m_pParamConfigBasic)
        {
            return (-1);
        }

        if (!m_pParamConfigBasic->LoadConfig(PARAM_CONFIG_BASIC))
        {
            return (-1);
        }

        int i = 0;
    }

    //
    // SMV��������
    //
    {
        CParamConfigSMVSend* m_pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
        if (NULL == m_pParamConfigSMVSend)
        {
            return (-1);
        }

        // ���������ļ�
        if (!m_pParamConfigSMVSend->LoadConfig(GetAppDir() + PARAM_CONFIG_SMV_SEND))
        {
            return (-1);
        }

        // ���SMV����
        m_pParamConfigSMVSend->SetSMVType(FRAME_TYPE_FT3_NANRUI);
        FrameType m_eSMVType = m_pParamConfigSMVSend->GetSMVType();

        // ��ò���ֵ��ʾ��ʽ
        m_pParamConfigSMVSend->SetSVDisplay(SMV_VALUE_DISP_TYPE_SECONDARY);
        SMV_DISP_TYPE m_eSVDisplay = m_pParamConfigSMVSend->GetSVDisplay();

        // ��ò���Ƶ��
        m_pParamConfigSMVSend->SetFrequency(FT3_SAMP_RATE_12P8K);
        FT3_SAMP_RATE m_eFrequency = m_pParamConfigSMVSend->GetFrequency();

        // ��ò�����
        m_pParamConfigSMVSend->SetBaudRate(FT3_BAUD_RATE_5M);
        FT3_BAUD_RATE m_eBaudRate = m_pParamConfigSMVSend->GetBaudRate();

        // ���ASDU����
        m_pParamConfigSMVSend->SetASDUCount(5);
        int iASDUCount = m_pParamConfigSMVSend->GetASDUCount();

        // ���ָ������ͨ���Ľ�ֱ������
        m_pParamConfigSMVSend->SetCurrentType(L"Ua1", CURRENT_TYPE_DC);
        eCurrentType eChannelCurType = m_pParamConfigSMVSend->GetCurrentType(L"Ua1");

        m_pParamConfigSMVSend->SetCurrentType(L"Ix2", CURRENT_TYPE_DC);
        eChannelCurType = m_pParamConfigSMVSend->GetCurrentType(L"Ix2");

        // Ѱ��һ��û��ͨ���Ŀ��ƿ�
        CIEC9_1Cb* pIec9_1Cb = m_pParamConfigSMVSend->FirstIEC9_1Cb();
        while(pIec9_1Cb && pIec9_1Cb->GetChannelCount())
        {
            pIec9_1Cb = m_pParamConfigSMVSend->NextIEC9_1Cb(pIec9_1Cb);
        }

        // �������û��ͨ���Ŀ��ƿ�
        if (pIec9_1Cb)
        {
            // ���һ��Ĭ��ͨ��
            pIec9_1Cb->AddChannel();
        }

        CIEC9_2Cb* pIec9_2Cb = m_pParamConfigSMVSend->FirstIEC9_2Cb();
        if (pIec9_2Cb)
        {
            // ��õ�һ��ͨ��
            CChannel* pChannel = pIec9_2Cb->FirstChannel();
            if (pChannel)
            {
                ChannelType eType = pChannel->GetChannelType();

                // ����ͨ������
                eType = CHANNEL_TYPE_SMV_TIME;
                pChannel->SetChannelType(eType);

                eType = CHANNEL_TYPE_SMV_VOLTAGE;
                pChannel->SetChannelType(eType);
            }

            // ���һ��Ĭ��ͨ��
            pIec9_2Cb->AddChannel();
        }

        if (!m_pParamConfigSMVSend->SaveConfig())
        {
            return (-1);
        }

        int i = 0;
    }

    //
    // GOOSE��������
    //
    {
        CParamConfigGOOSESend* m_pParamConfigGOOSESend = CParamConfigGOOSESend::getInstance();
        if (NULL == m_pParamConfigGOOSESend)
        {
            return (-1);
        }

        // ���������ļ�
        if (!m_pParamConfigGOOSESend->LoadConfig(GetAppDir() + PARAM_CONFIG_GOOSE_SEND))
        {
            return (-1);
        }

        // �������
        m_pParamConfigGOOSESend->SetHeartbeatInterval(5000);
        int iHeartbeatInterval = m_pParamConfigGOOSESend->GetHeartbeatInterval();

        // ��С���
        m_pParamConfigGOOSESend->SetMinimumInterval(2);
        int iMinimumInterval = m_pParamConfigGOOSESend->GetMinimumInterval();

        // ��õ�һ��GOOSE���ƿ�
        CGooseCb*   pGocb               = m_pParamConfigGOOSESend->FirstGocb();
        if (pGocb)
        {
            bool            bSelected       = pGocb->IsSelected();
            std::wstring    strDescription  = pGocb->GetDescription();

            // �������ƿ�����ͨ��
            CChannel*   pChannel            = pGocb->FirstChannel();
            while((NULL != pChannel) && (CHANNEL_TYPE_GOOSE_STRUCT == pChannel->GetChannelType()))
            {
                pChannel                = pGocb->NextChannel(pChannel);
            }

            if (pChannel)
            {
                strDescription                  = pChannel->GetDescription();
                ChannelType eChannelType        = pChannel->GetChannelType();
                pChannel->SetChannelType(CHANNEL_TYPE_GOOSE_STRUCT);
                pChannel->SetChannelType(CHANNEL_TYPE_GOOSE_POINT);
                pChannel->SetChannelType(CHANNEL_TYPE_GOOSE_DOUBLE_POINT);
                pChannel->SetChannelType(CHANNEL_TYPE_GOOSE_STRUCT);
                pChannel->SetChannelType(CHANNEL_TYPE_GOOSE_DOUBLE_POINT);
                pChannel->SetChannelType(CHANNEL_TYPE_GOOSE_POINT);
                std::wstring strVirtualChannel  = pChannel->GetVirtualChannel();

                CChannel*   pSubChannel         = pChannel->FirstSubChannel();
                pSubChannel                     = pChannel->NextSubChannel(pSubChannel);
                strDescription                  = pSubChannel->GetDescription();
                eChannelType                    = pSubChannel->GetChannelType();
                strVirtualChannel               = pSubChannel->GetVirtualChannel();
            }
        }

        // ����DO����ͨ��
        CGOOSESendVirtualChannel*   pVirtualChannel = m_pParamConfigGOOSESend->FirstGOOSESendVirtualChannel();
        while(pVirtualChannel)
        {
            std::wstring    strName     = pVirtualChannel->GetName();
            CChannel*       pChannel    = pVirtualChannel->FirstChannel();
            while(pChannel)
            {
                pChannel = pVirtualChannel->NextChannel(pChannel);
            }

            pVirtualChannel = m_pParamConfigGOOSESend->NextGOOSESendVirtualChannel(pVirtualChannel);
        }

        // ��������
        if (!m_pParamConfigGOOSESend->SaveConfig())
        {
            return (-1);
        }

        int i = 0;
    }

    //
    // GOOSE��������
    //
    {
        CParamConfigGOOSERecv* m_pParamConfigGOOSERecv = CParamConfigGOOSERecv::getInstance();
        if (NULL == m_pParamConfigGOOSERecv)
        {
            return (-1);
        }

        // ���������ļ�
        if (!m_pParamConfigGOOSERecv->LoadConfig(GetAppDir() + PARAM_CONFIG_GOOSE_RECV))
        {
            return (-1);
        }

        // ���GOOSE���տ��ƿ�
        CGooseCb*   pGocb               = m_pParamConfigGOOSERecv->FirstGocb();
        pGocb                           = m_pParamConfigGOOSERecv->NextGocb(pGocb);
        if (pGocb)
        {
            bool            bSelected       = pGocb->IsSelected();
            std::wstring    strDescription  = pGocb->GetDescription();

            CChannel*   pChannel            = pGocb->FirstChannel();
            pChannel                        = pGocb->NextChannel(pChannel);
            strDescription                  = pChannel->GetDescription();
            ChannelType eChannelType        = pChannel->GetChannelType();
            std::wstring strVirtualChannel  = pChannel->GetVirtualChannel();

            CChannel*   pSubChannel         = pChannel->FirstSubChannel();
            pSubChannel                     = pChannel->NextSubChannel(pSubChannel);
            strDescription                  = pSubChannel->GetDescription();
            eChannelType                    = pSubChannel->GetChannelType();
            strVirtualChannel               = pSubChannel->GetVirtualChannel();
        }

        // ����DI����ͨ��
        CGOOSERecvVirtualChannel*   pVirtualChannel = m_pParamConfigGOOSERecv->FirstGOOSERecvVirtualChannel();
        while(pVirtualChannel)
        {
            std::wstring    strName     = pVirtualChannel->GetName();
            CChannel*       pChannel    = pVirtualChannel->GetChannel();

            pVirtualChannel = m_pParamConfigGOOSERecv->NextGOOSERecvVirtualChannel(pVirtualChannel);
        }

        // ��������
        if (!m_pParamConfigGOOSERecv->SaveConfig())
        {
            return (-1);
        }

        int i = 0;
    }

    //
    // ȫվ�����ļ�
    //
    {
        // ���ȫվ�����ļ�����������
        CSCLManager*        m_pSCLManager    = CSCLManager::getInstance();
        if (NULL == m_pSCLManager)
        {
            return (-1);
        }

        // ����ȫվ�����ļ�
        if (!m_pSCLManager->Load(GetAppDir() + PARAM_GuoWangMUTest_SCD))
        {
            return false;
        }

        // ����ȫվ�����ļ��������е�GOOSE���ƿ�
        CGooseCb* pGocb = m_pSCLManager->FirstGooseCb();
        while (NULL != pGocb)
        {
            std::wstring strDesc = pGocb->GetDescription();
            pGocb = m_pSCLManager->NextGooseCb(pGocb);
        }

        // ����ȫվ�����ļ��������е�SMV���ƿ�
        CIEC9_2Cb* pSMVCb = m_pSCLManager->FirstIEC9_2Cb();
        while(NULL != pSMVCb)
        {
            std::wstring strDesc = pSMVCb->GetDescription();
            pSMVCb = m_pSCLManager->NextIEC9_2Cb(pSMVCb);
        }

        //
        // ���GOOSE���͹���������
        //
        CParamConfigGOOSESend* m_pParamConfigGOOSESend = CParamConfigGOOSESend::getInstance();
        if (NULL == m_pParamConfigGOOSESend)
        {
            return (-1);
        }
        // ����GOOSE���ƿ鵽GOOSE���͹�����������
        CGooseCb*       pGooseCbSend = m_pParamConfigGOOSESend->AddGocb(m_pSCLManager->FirstGooseCb());
        if (!pGooseCbSend)
        {
            return (-1);
        }

        //
        // ���GOOSE���չ���������
        //
        CParamConfigGOOSERecv* m_pParamConfigGOOSERecv = CParamConfigGOOSERecv::getInstance();
        if (NULL == m_pParamConfigGOOSERecv)
        {
            return (-1);
        }
        // ����GOOSE���ƿ鵽GOOSE���չ�����������
        CGooseCb*       pGooseCbRecv = m_pParamConfigGOOSERecv->AddGocb(m_pSCLManager->FirstGooseCb());
        if (!pGooseCbRecv)
        {
            return (-1);
        }

        //
        // ���SMV���Ͳ������ù�����
        //
        CParamConfigSMVSend*    m_pParamConfigSmvSend = CParamConfigSMVSend::getInstance();
        if (NULL == m_pParamConfigSmvSend)
        {
            return (-1);
        }

        // ����IEC61850-9-2���ƿ鵽SMV���͹�������IEC61850-9-1���ƿ�������
        CIEC9_1Cb*      pIEC9_1Cb   = m_pParamConfigSmvSend->AddIEC9_1Cb(m_pSCLManager->FirstIEC9_2Cb());
        if (!pIEC9_1Cb)
        {
            return (-1);
        }

        // ����IEC61850-9-2���ƿ鵽SMV���͹�������IEC61850-9-2���ƿ�������
        CIEC9_2Cb*      pIEC9_2Cb   = m_pParamConfigSmvSend->AddIEC9_2Cb(m_pSCLManager->FirstIEC9_2Cb());
        if (!pIEC9_2Cb)
        {
            return (-1);
        }

        // ����IEC61850-9-2���ƿ鵽SMV���͹�������FT3���ƿ�������
        CFT3Cb*         pFt3Cb      = m_pParamConfigSmvSend->AddFt3Cb(m_pSCLManager->FirstIEC9_2Cb());
        if (!pFt3Cb)
        {
            return (-1);
        }

        // ����IEC61850-9-2���ƿ鵽SMV���͹�������FT3��չ���ƿ�������
        CFT3ExpandCb*   pFt3ExpandCb    = m_pParamConfigSmvSend->AddFt3ExpandCb(m_pSCLManager->FirstIEC9_2Cb());
        if (!pFt3ExpandCb)
        {
            return (-1);
        }

        //
        // ɾ������ӵĿ��ƿ�
        //
        m_pParamConfigSmvSend->DeleteIEC9_1Cb(pIEC9_1Cb);
        m_pParamConfigSmvSend->DeleteIEC9_2Cb(pIEC9_2Cb);
        m_pParamConfigSmvSend->DeleteFt3Cb(pFt3Cb);
        m_pParamConfigSmvSend->DeleteFt3ExpandCb(pFt3ExpandCb);

        m_pParamConfigGOOSESend->DeleteGocb(pGooseCbSend);
        m_pParamConfigGOOSERecv->DeleteGocb(pGooseCbRecv);
    }

    //
    // �ֶ�����
    //
    {
        // �����ֶ��������ù�����ʵ��
        CParamconfigManualTest* pParamconfigManualTest = CParamconfigManualTest::getInstance();
        if (NULL == pParamconfigManualTest)
        {
            return (-1);
        }

        // ����ȫվ�����ļ�
        if (!pParamconfigManualTest->LoadConfig(GetAppDir() + PARAM_CONFIG_MANUAL_TEST))
        {
            return false;
        }

        // ������������ͨ��
        CSMVSendVirtualChannel* pVirtualChannel = pParamconfigManualTest->FirstSMVSendVirtualChannel();
        while(pVirtualChannel)
        {

            pVirtualChannel = pParamconfigManualTest->NextSMVSendVirtualChannel(pVirtualChannel);
        }

        GOOSE_INT8      stGooseInt8;
        stGooseInt8.id.type     = GSEDT_INT8;
        stGooseInt8.datVal      = 25;

        GOOSE_DATA_VARIANT m_Variant;
        m_Variant = stGooseInt8;
        m_Variant.m_stInt8.datVal = 50;

        stGooseInt8 = m_Variant;

        GOOSE_DATA_VARIANT m_Variant2 = m_Variant;

        GOOSE_DATA_VARIANT m_strVariant1;
        m_strVariant1.m_stUnKnown.id.type = GSEDT_BIT_STR;
        std::wstring strTMP = m_strVariant1.m_stBitString.GetString();
        m_strVariant1.m_stBitString.SetString(L"110101011101010111010");
        strTMP = m_strVariant1.m_stBitString.GetString();
        wprintf(L"m_strVariant1.m_stBitString = %s\n", m_strVariant1.m_stBitString.GetString().c_str());
        std::wcout << L"m_strVariant1.m_stBitString = " << m_strVariant1.m_stBitString.GetString() << std::endl;
        
        GOOSE_DATA_VARIANT m_strVariant2 = m_strVariant1;
        strTMP = m_strVariant2.m_stBitString.GetString();

        // ������������ļ�
        if (!pParamconfigManualTest->SaveConfig())
        {
            return (-1);
        }

        int i = 0;
    }

    //
    // ״̬����
    //
    {
        CStatus* pStatus    = NULL;


        CParamConfigStateSequence* pParamconfigStateSequence = CParamConfigStateSequence::getInstance();
        if (NULL == pParamconfigStateSequence)
        {
            return (-1);
        }

        // ����ȫվ�����ļ�
        if (!pParamconfigStateSequence->LoadConfig(GetAppDir() + PARAM_CONFIG_STATE_SEQUENCE))
        {
            return false;
        }

        // ���һ��״̬����
        pStatus = pParamconfigStateSequence->AddStatus();
        if (NULL == pStatus)
        {
            return false;
        }

        // ɾ��ָ��״̬����
        if (!pParamconfigStateSequence->DeleteStatus(pStatus))
        {
            return false;
        }

        // ��������״̬����
        pStatus = pParamconfigStateSequence->FirstStatus();
        while(pStatus)
        {
            CSMVSendVirtualChannel* pVirtualChannel = pStatus->FirstSMVSendVirtualChannel();
            while(pVirtualChannel)
            {
                int nIndex  = 0;


                nIndex = pVirtualChannel->GetSelectedIndex();
                pVirtualChannel->SetSelectedIndex(1);
                nIndex = pVirtualChannel->GetSelectedIndex();
                pVirtualChannel = pStatus->NextSMVSendVirtualChannel(pVirtualChannel);
            }

            pStatus = pParamconfigStateSequence->NextStatus(pStatus);
        }

        // ������������ļ�
        if (!pParamconfigStateSequence->SaveConfig())
        {
            return (-1);
        }

        int i = 0;
    }

    return 0;
}

