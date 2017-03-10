/// @file
///     StrManager.h
///
/// @brief
///     �ַ�����Դ������ͷ�ļ�
///
/// @note
///     ��ҪΪʵ�ֶ�����ԵĹ��ܣ������õ���ʾ�ַ���ʱ
///     ��Ҫͨ���˹����������ַ���ID����ַ���
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.06.03
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#ifndef SCL_CHANGER_STR_MANAGER_H_

#define SCL_CHANGER_STR_MANAGER_H_

/**
*      ��ȡģ��·���ַ���
*
* \return
*      CString     ģ���·���ַ���
* \note
*      ����x:\aaaaa\bbbb\ccc.exe�򷵻�x:\aaaaa\bbbb\
*/
extern const CString _GetAppDir();

#include <map>


/**
*      �ַ�����Դ��������
*
*  \note
*      ��ԴID��һ������0��UINT����
*/
class CStrManager
{
public:

	/**
	*      ����ָ����Դ�ļ�
	*/
	static bool Load(UINT uResID);

	/**
	*      ��ȡָ����ʶ���ַ���
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
	*      ��ȡ��̬ʵ������
	*
	*  \return
	*      CStrManager*          ���ؾ�̬ʵ�������ָ��
	*/
	static CStrManager* getInstance()
	{
		static CStrManager s_StringMgr;
		return &s_StringMgr;
	}

private:
	typedef std::map<UINT, CString>         stringmap;
	typedef std::pair<UINT, CString>        stringpair;

	stringmap                               m_mapString;        ///< �ַ���ӳ���
};

#endif