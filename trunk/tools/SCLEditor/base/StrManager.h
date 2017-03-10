/// @file
///     StrManager.h
///
/// @brief
///     字符串资源管理器头文件
///
/// @note
///     主要为实现多国语言的功能，界面用到显示字符串时
///     需要通过此管理器根据字符串ID获得字符串
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.06.03
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#ifndef SCL_CHANGER_STR_MANAGER_H_

#define SCL_CHANGER_STR_MANAGER_H_

/**
*      获取模块路径字符串
*
* \return
*      CString     模块的路径字符串
* \note
*      比如x:\aaaaa\bbbb\ccc.exe则返回x:\aaaaa\bbbb\
*/
extern const CString _GetAppDir();

#include <map>


/**
*      字符串资源管理器类
*
*  \note
*      资源ID是一个大于0的UINT类型
*/
class CStrManager
{
public:

	/**
	*      加载指定资源文件
	*/
	static bool Load(UINT uResID);

	/**
	*      获取指定标识的字符串
	*/
	static CString Get(UINT uResID);

private:
	CStrManager()
	{
	}

	~CStrManager()
	{
		m_mapString.clear();
	}

	/**
	*      获取静态实例对象
	*
	*  \return
	*      CStrManager*          返回静态实例对象的指针
	*/
	static CStrManager* getInstance()
	{
		static CStrManager s_StringMgr;
		return &s_StringMgr;
	}

private:
	typedef std::map<UINT, CString>         stringmap;
	typedef std::pair<UINT, CString>        stringpair;

	stringmap                               m_mapString;        ///< 字符串映射表
};

#endif