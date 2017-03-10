#ifndef     __ACTIVE_SYNC_CONNECT__
#define     __ACTIVE_SYNC_CONNECT__

class CActiveSyncConnect:public CConnect
{
public:
    CActiveSyncConnect();
    ~CActiveSyncConnect();

    BOOL Connect();

    int SendAPacket(const PACKET *pPacket);
    int SendAPacket(UINT16 uiCmd,UINT16 param,UINT16 uiDatalen,const BYTE *pData);
    int RecvAPacket(PACKET *pPacket,unsigned int uiTimeOut=0);
    int RecvAPacket(UINT16 &index,UINT16 &param,UINT16 &uiDatalen,BYTE *pData,
        unsigned int uiTimeOut=0);
    const WCHAR *GetLocalID(){return L"WinCE";};
    void Clear();

    int TestConnect();

};


#endif