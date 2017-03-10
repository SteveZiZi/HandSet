/**
 * \file
 *      IPageData.h
 *
 * \brief
 *      页面数据接口头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/5/20
 */


#pragma once

class IPageDataExtra;


/**
 *      页面状态枚举值
 */
enum DsmPageAction
{
    PageUnknow,             ///< 未知状态
    PageEnter,              ///< 进入页面
    PageLeave,              ///< 离开页面
    PageBack,               ///< 返回页面
};


/**
 *      页面状态数据类
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

    DsmPageAction       m_eAction;          ///< 页面状态

    UINT                m_nPageID;          ///< 数据关联页面ID

    int                 m_nCtrlID;          ///< 控件ID
    DWORD               m_dwCtrlData;       ///< 控件关联数据

    DWORD               m_dwData;           ///< 页面关联数据
    IPageDataExtra*     m_pPageDataExtra;   ///< 页面关联额外数据
};


/**
 *      页面状态附加数据类
 */
class IPageDataExtra
{
public:
    virtual ~IPageDataExtra() {}

public:

    /**
     *      获取附加数据类绑定的数据页面数据指针
     */
    virtual CPageData* GetPageData() = 0;
};

/**
 *      控制块关联信息 - 控制块数据
 */
class ICtrlBlkData
{
public:
    virtual ~ICtrlBlkData() {}
};

