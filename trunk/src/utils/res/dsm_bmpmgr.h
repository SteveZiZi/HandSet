/**
 * \file
 *      dsm_bmpmgr.h
 *
 * \brief
 *      ͼƬ��Դ������ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/3/15
 */

#ifndef DSM_BMPMGR_H__
#define DSM_BMPMGR_H__

#include <map>

/**
 *      ͼƬ��Դ��������
 *
 *  \@note
 *      �ṩͼƬ���ʹ��ܣ��ڲ�������Ԥ����ͼƬ
 */
class dsm_bmpmgr
{
public:
    /**
     *      ����ָ����ʶ����Դ
     */
    static bool Load(UINT nResID);

    /**
     *      ��ȡָ����ʶ��ͼƬ���
     */
    static HBITMAP Get(UINT nResID);

    /**
     *      ��ȡָ����ʶ��CBitmap����ָ��
     */
    static CBitmap* GetCB(UINT nResID);

    /**
     *      ��ȡָ����ʶ��ͼƬ·��
     */
    static CString GetBmpPath(UINT nResID);

private:
    dsm_bmpmgr();

    ~dsm_bmpmgr();


    /**
     *      ��ȡ��̬ʵ������
     */
    static dsm_bmpmgr* getInstance();

    /**
     *      ����ͼƬ·��ӳ��������Դ
     */
    static void loadBMP();

    /**
     *      ������Դ
     */
    static void clearRes();


private:
    typedef std::map<UINT, HBITMAP>     maphbmp;
    typedef std::map<UINT, CString>     mapcsbmp;
    typedef std::map<UINT, CBitmap*>    mapcbmp;

    bool                    m_bPreload;             ///< Ԥ���ر�־
    maphbmp                 m_mapBitmap;            ///< ͼƬ���ӳ���
    mapcsbmp                m_mapStrBmp;            ///< ͼƬ·��ӳ���
    mapcbmp                 m_mapCBitmap;           ///< CBitmapָ�����ӳ���
};

#endif // DSM_BMPMGR_H__


