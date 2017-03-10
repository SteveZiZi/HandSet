#include "StdAfx.h"
#include "DataManager.h"
Lock CDataManager::m_singleLock;
CDataManager::CDataManager(void)
{
}

CDataManager::~CDataManager(void)
{
}

CDataManager & CDataManager::GetInstance()
{
    static CDataManager *pDataManager;
    if(pDataManager == NULL)
    {
        m_singleLock.Acquire();
        if(pDataManager == NULL)
        {
            pDataManager = new CDataManager();
        }
        m_singleLock.Release();
    }
    return *pDataManager;
}
