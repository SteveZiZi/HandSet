/**
 * \file
 *      IPageData.h
 *
 * \brief
 *      ҳ�����ݽӿ�ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/5/20
 */


#pragma once

class IPageDataExtra;


/**
 *      ҳ��״̬ö��ֵ
 */
enum DsmPageAction
{
    PageUnknow,             ///< δ֪״̬
    PageEnter,              ///< ����ҳ��
    PageLeave,              ///< �뿪ҳ��
    PageBack,               ///< ����ҳ��
};


/**
 *      ҳ��״̬������
 */
struct CPageData
{
public:
    CPageData(UINT nPageID = UINT_MAX)
    : m_nPageID(nPageID)
    , m_nCtrlID(UINT_MAX)
    , m_eAction(PageEnter)
    , m_pPageDataExtra(NULL)
    , m_dwCtrlData(0)
    , m_dwData(0)
    {

    }

    virtual ~CPageData()
    {

    }


public:

    DsmPageAction       m_eAction;          ///< ҳ��״̬

    UINT                m_nPageID;          ///< ���ݹ���ҳ��ID

    int                 m_nCtrlID;          ///< �ؼ�ID
    DWORD               m_dwCtrlData;       ///< �ؼ���������

    DWORD               m_dwData;           ///< ҳ���������
    IPageDataExtra*     m_pPageDataExtra;   ///< ҳ�������������
};


/**
 *      ҳ��״̬����������
 */
class IPageDataExtra
{
public:
    virtual ~IPageDataExtra() {}

public:

    /**
     *      ��ȡ����������󶨵�����ҳ������ָ��
     */
    virtual CPageData* GetPageData() = 0;
};

/**
 *      ���ƿ������Ϣ - ���ƿ�����
 */
class ICtrlBlkData
{
public:
    virtual ~ICtrlBlkData() {}
};

