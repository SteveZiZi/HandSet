/**
 * \file
 *      PageData.h
 *
 * \brief
 *      页面数据类头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/5/28
 */


#pragma once

#include "IPageData.h"


/**
 *      页面状态附件数据实现类 - 控制块
 */
class CPageDataExtra : public IPageDataExtra
{
public:
    CPageDataExtra(CPageData* pPageData)
    : m_pPageData(pPageData)
    {

    }

    virtual ~CPageDataExtra()
    {
        int nIndex          = 0;
        ICtrlBlkData* pData = NULL;
        POSITION pos        = m_mapCtrlBlkData.GetStartPosition();

        while(pos)
        {
            m_mapCtrlBlkData.GetNextAssoc(pos, nIndex, pData);

            if(pData)
            {
                delete pData;
            }
        }

        m_mapCtrlBlkData.RemoveAll();
    }

public:

    /**
     *      获取附加数据类绑定的数据页面数据指针
     */
    virtual CPageData* GetPageData()
    {
        return m_pPageData;
    }


    /**
     *      根据控制块ID获取控制块绑定的数据
     *
     *  \param[in]
     *      nIndex              控制块ID
     *  \return
     *      SMV_PHASOR_DATA*    返回控制块绑定数据信息
     */
    ICtrlBlkData* Get(int nIndex)
    {
        ICtrlBlkData* pData = NULL;

        if(m_mapCtrlBlkData.Lookup(nIndex, pData))
        {
            return pData;
        }

        return NULL;
    }

    /**
     *      设置控制块绑定信息
     *
     *  \param[in]
     *      nIndex          控制块ID
     *  \param[in]
     *      pData           控制块绑定数据信息
     *  \return
     *      bool            设置成功返回true，失败返回false
     */
    bool Set(int nIndex, ICtrlBlkData* pData)
    {
        ICtrlBlkData* pDataRet = NULL;

        if(m_mapCtrlBlkData.Lookup(nIndex, pDataRet))
        {
            delete pDataRet;
            m_mapCtrlBlkData.RemoveKey(nIndex);
        }

        m_mapCtrlBlkData.SetAt(nIndex, pData);

        return true;
    }

private:
    typedef CMap<int, int, ICtrlBlkData*, ICtrlBlkData*> CMapCtrlBlk;

    CPageData*          m_pPageData;            ///< 附件数据关联的页面数据指针
    CMapCtrlBlk         m_mapCtrlBlkData;       ///< 控制块选中通道的信息
};


/**
 *      主页面状态附件数据实现类
 */
class CMainPageDataExtra : public IPageDataExtra
{
public:
    CMainPageDataExtra(CPageData* pPageData)
        : m_pPageData(pPageData)
        , m_nPageIndex(0)
        , m_csSelectFile(L"")
    {

    }

    ~CMainPageDataExtra()
    {

    }

public:
    /**
     *      获取附加数据类绑定的数据页面数据指针
     */
    virtual CPageData* GetPageData()
    {
        return m_pPageData;
    }

    /**
     *      获取页面索引
     */
    int GetPageIndex()
    {
        return m_nPageIndex;
    }

    /**
     *      设置页面索引
     */
    void SetPageIndex(int nPageIndex)
    {
        m_nPageIndex = nPageIndex;
    }

    /**
     *      设置页面最后的焦点控件位置
     */
    void SetPageLastCtrl(int key, CPoint value)
    {
        m_mapPage2Ctrl.SetAt(key, value);
    }

    /**
     *      获取页面最后的焦点控件位置
     */
    bool GetPageLastCtrl(int key, CPoint& value)
    {
        if(m_mapPage2Ctrl.Lookup(key, value))
        {
            return true;
        }

        return false;
    }

    /**
     *      设置上一次波形分析选中的文件
     */
    void SetPcapFile(const CString csFile)
    {
        m_csSelectFile = csFile;
    }

    /**
     *      获取上一次波形分析选中的文件
     */
    CString GetPcapFile()
    {
        return m_csSelectFile;
    }

private:
    typedef CMap<int, int, CPoint, CPoint> CMapCP;
    CPageData*              m_pPageData;            ///< 附件数据关联的页面数据指针
    int                     m_nPageIndex;           ///< 页面索引
    CMapCP                  m_mapPage2Ctrl;         ///< 页面上一次的焦点控件位置
    CString                 m_csSelectFile;         ///< 选中的文件
};
