/// @file
///
/// @brief
///     GOOSE�������û���
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.27
///
/// �޸���ʷ��
///

#ifndef __PARAM_CONFIG_GOOSE_BASE_H__
#define __PARAM_CONFIG_GOOSE_BASE_H__
#include "GooseCb.h"
#include "ParamConfigBase.h"
#include "src/utils/MessageCodec/include/goose_msg.h"
#include "src/service/datalayer/recv/FrameRecvStruct.h"

#define NODE_ROOT                       L"config"                       ///< ���ڵ�
#define NODE_HeartbeatInterval          L"HeartbeatInterval"            ///< �������T0
#define NODE_MinimumInterval            L"MinimumInterval"              ///< ��С���T1
#define NODE_GocbGroup                  L"GocbGroup"                    ///< GOOSE���ƿ���
#define NODE_Gocb                       L"Gocb"                         ///< GOOSE���ƿ�

class CParamConfigGOOSEBase : public CParamConfigBase
{
protected:
    CParamConfigGOOSEBase(void);
    virtual ~CParamConfigGOOSEBase(void);

public:
    ///
    /// @brief
    ///    ���GOOSE���ƿ�
    ///
    /// @return
    ///     CGocb*    ����ӵ�GOOSE���ƿ�����ָ��
    ///
    CGooseCb* AddGocb();

    ///
    /// @brief
    ///     ���GOOSE���ƿ飨�˺�������ȿ���һ���β���ָ����Ȼ����ӵ��ڲ������У�
    ///
    /// @param[in]
    ///     pGocb    -   GOOSE���ƿ�ָ��
    ///
    /// @return
    ///     CGooseCb*    �ɹ�ʱ��������ӵ�GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CGooseCb* AddGocb(CGooseCb* pGocb);

    ///
    /// @brief
    ///     ���ɨ���б��е�GOOSE���ƿ鵽�����У��˺�������ȿ���һ���β���ָ����Ȼ����ӵ��ڲ������У�
    ///
    /// @param[in]
    ///     pDetectedGooseItem    -   ɨ���б��е�GOOSE���ƿ�ָ��
    ///
    /// @return
    ///     CGooseCb*    �ɹ�ʱ��������ӵ�GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CGooseCb* AddGocb(CDetectItemFrame* pDetectedGooseItem);

    ///
    /// @brief
    ///    ɾ��GOOSE���ƿ�
    ///
    /// @param[in]  pCurGocb GOOSE���ƿ�ָ��
    ///
    /// @return
    ///     bool    true��ʾɾ���ɹ�
    ///
    bool DeleteGocb(CGooseCb* pCurGocb);

    ///
    /// @brief
    ///    ɾ��ָ��APPID��GOOSE���ƿ�
    ///
    /// @param[in]  iAPPID ��ɾ���Ŀ��ƿ�APPID
    ///
    /// @return
    ///     bool    true��ʾɾ���ɹ�
    ///
    bool DeleteGocb(int iAPPID);

    ///
    /// @brief
    ///    �������GOOSE���ƿ�
    ///
    void ClearAllGocb();

    ///
    /// @brief
    ///    ö�ٵ�һ��GOOSE���ƿ�
    ///
    /// @return
    ///     CGooseCb*    �ɹ�ʱ����GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CGooseCb* FirstGocb();

    ///
    /// @brief
    ///    ö����һ��GOOSE���ƿ�
    ///
    /// @param[in]  pCurGocb ��ǰλ��GOOSE���ƿ��ָ��
    ///
    /// @return
    ///     CGooseCb*    �ɹ�ʱ����GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CGooseCb* NextGocb(CGooseCb* pCurGocb);

    ///
    /// @brief
    ///    ö����һ��GOOSE���ƿ�
    ///
    /// @param[in]  pCurGocb ��ǰλ��GOOSE���ƿ��ָ��
    ///
    /// @return
    ///     CGooseCb*    �ɹ�ʱ����GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CGooseCb* PrevGocb(CGooseCb* pCurGocb);

    ///
    /// @brief
    ///    ö�����һ��GOOSE���ƿ�
    ///
    /// @return
    ///     CGooseCb*    �ɹ�ʱ����GOOSE���ƿ�ָ�룬ʧ��ʱ����NULL
    ///
    CGooseCb* LastGocb();

    ///
    /// @brief
    ///    ��ÿ��ƿ����
    ///
    /// @return
    ///     int    ���ƿ����
    ///
    int GetGocbCount();

    ///
    /// @brief
    ///    ��ñ�ѡ�еĿ��ƿ����
    ///
    /// @return
    ///     int    ��ѡ�еĿ��ƿ����
    ///
    int GetSelectedGocbCount();

protected:
    ///
    /// @brief
    ///    ���GOOSE���ƿ�
    ///
    /// @param[in]  pNewGocb ָ��GOOSE���ƿ�����ָ��
    ///
    /// @return
    ///     bool    true��ʾ��ӳɹ�
    ///
    bool __AddGocb(CGooseCb* pNewGocb);

    ///
    /// @brief
    ///     ���ָ��APPID��GOOSE���ƿ飨ע����ͬAPPID�Ŀ��ƿ�����ж�����˴�ֻ��õ�һ��ƥ��Ŀ��ƿ飩
    ///
    /// @param[in]  
    ///     iAPPID  -   ���ƿ��APPID
    ///
    /// @return
    ///     CGooseCb*   �ɹ�ʱ���ؿ��ƿ��ָ�룬ʧ��ʱ����NULL
    ///
    CGooseCb* __GetGocb(int iAPPID);

protected:
    std::list<CGooseCb *>               m_listGocbGroup;                ///< GOOSE���ƿ�����
};
#endif
