/**
 * \file
 *      dsm_bmpmgr.h
 *
 * \brief
 *      图片资源管理器头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/3/15
 */

#ifndef DSM_BMPMGR_H__
#define DSM_BMPMGR_H__

#include <map>

/**
 *      图片资源管理器类
 *
 *  \@note
 *      提供图片访问功能，内部可以做预加载图片
 */
class dsm_bmpmgr
{
public:
    /**
     *      加载指定标识的资源
     */
    static bool Load(UINT nResID);

    /**
     *      获取指定标识的图片句柄
     */
    static HBITMAP Get(UINT nResID);

    /**
     *      获取指定标识的CBitmap对象指针
     */
    static CBitmap* GetCB(UINT nResID);

    /**
     *      获取指定标识的图片路径
     */
    static CString GetBmpPath(UINT nResID);

private:
    dsm_bmpmgr();

    ~dsm_bmpmgr();


    /**
     *      获取静态实例对象
     */
    static dsm_bmpmgr* getInstance();

    /**
     *      根据图片路径映射表加载资源
     */
    static void loadBMP();

    /**
     *      清理资源
     */
    static void clearRes();


private:
    typedef std::map<UINT, HBITMAP>     maphbmp;
    typedef std::map<UINT, CString>     mapcsbmp;
    typedef std::map<UINT, CBitmap*>    mapcbmp;

    bool                    m_bPreload;             ///< 预加载标志
    maphbmp                 m_mapBitmap;            ///< 图片句柄映射表
    mapcsbmp                m_mapStrBmp;            ///< 图片路径映射表
    mapcbmp                 m_mapCBitmap;           ///< CBitmap指针对象映射表
};

#endif // DSM_BMPMGR_H__


