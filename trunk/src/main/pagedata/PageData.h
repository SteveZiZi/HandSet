/**
 * \file
 *      PageData.h
 *
 * \brief
 *      ҳ��������ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/5/28
 */


#pragma once

#include "IPageData.h"


/**
 *      ҳ��״̬��������ʵ���� - ���ƿ�
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
     *      ��ȡ����������󶨵�����ҳ������ָ��
     */
    virtual CPageData* GetPageData()
    {
        return m_pPageData;
    }


    /**
     *      ���ݿ��ƿ�ID��ȡ���ƿ�󶨵�����
     *
     *  \param[in]
     *      nIndex              ���ƿ�ID
     *  \return
     *      SMV_PHASOR_DATA*    ���ؿ��ƿ��������Ϣ
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
     *      ���ÿ��ƿ����Ϣ
     *
     *  \param[in]
     *      nIndex          ���ƿ�ID
     *  \param[in]
     *      pData           ���ƿ��������Ϣ
     *  \return
     *      bool            ���óɹ�����true��ʧ�ܷ���false
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

    CPageData*          m_pPageData;            ///< �������ݹ�����ҳ������ָ��
    CMapCtrlBlk         m_mapCtrlBlkData;       ///< ���ƿ�ѡ��ͨ������Ϣ
};


/**
 *      ��ҳ��״̬��������ʵ����
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
     *      ��ȡ����������󶨵�����ҳ������ָ��
     */
    virtual CPageData* GetPageData()
    {
        return m_pPageData;
    }

    /**
     *      ��ȡҳ������
     */
    int GetPageIndex()
    {
        return m_nPageIndex;
    }

    /**
     *      ����ҳ������
     */
    void SetPageIndex(int nPageIndex)
    {
        m_nPageIndex = nPageIndex;
    }

    /**
     *      ����ҳ�����Ľ���ؼ�λ��
     */
    void SetPageLastCtrl(int key, CPoint value)
    {
        m_mapPage2Ctrl.SetAt(key, value);
    }

    /**
     *      ��ȡҳ�����Ľ���ؼ�λ��
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
     *      ������һ�β��η���ѡ�е��ļ�
     */
    void SetPcapFile(const CString csFile)
    {
        m_csSelectFile = csFile;
    }

    /**
     *      ��ȡ��һ�β��η���ѡ�е��ļ�
     */
    CString GetPcapFile()
    {
        return m_csSelectFile;
    }

private:
    typedef CMap<int, int, CPoint, CPoint> CMapCP;
    CPageData*              m_pPageData;            ///< �������ݹ�����ҳ������ָ��
    int                     m_nPageIndex;           ///< ҳ������
    CMapCP                  m_mapPage2Ctrl;         ///< ҳ����һ�εĽ���ؼ�λ��
    CString                 m_csSelectFile;         ///< ѡ�е��ļ�
};
