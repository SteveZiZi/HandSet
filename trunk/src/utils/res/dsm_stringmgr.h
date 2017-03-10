/**
 * \file
 *      dsm_stringmgr.h
 *
 * \brief
 *      字符串资源管理器头文件
 *
 * \note
 *      主要为实现多国语言的功能，界面用到显示字符串时
 *      需要通过此管理器根据字符串ID获得字符串
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/3/14
 */

#ifndef DSM_STRINGMGR_H__
#define DSM_STRINGMGR_H__


#include <map>


/**
 *      字符串资源管理器类
 *
 *  \note
 *      资源ID是一个大于0的UINT类型
 */
class dsm_stringMgr
{
public:

    /**
     *      加载指定资源文件
     */
    static bool Load(UINT uResID);

    /**
     *      获取指定标识的字符串
     */
    static const CString& Get(UINT uResID);

private:
    dsm_stringMgr()
    {
    }

    ~dsm_stringMgr()
    {
        m_mapString.clear();
    }

    /**
     *      获取静态实例对象
     *
     *  \return
     *      dsm_stringMgr*          返回静态实例对象的指针
     */
    static dsm_stringMgr* getInstance()
    {
        static dsm_stringMgr s_StringMgr;
        return &s_StringMgr;
    }

private:
    typedef std::map<UINT, CString>         stringmap;
    typedef std::pair<UINT, CString>        stringpair;

    stringmap                               m_mapString;        ///< 字符串映射表
};

#endif // DSM_STRINGMGR_H__
