/**
 * \file
 *      dsm_stringmgr.h
 *
 * \brief
 *      �ַ�����Դ������ͷ�ļ�
 *
 * \note
 *      ��ҪΪʵ�ֶ�����ԵĹ��ܣ������õ���ʾ�ַ���ʱ
 *      ��Ҫͨ���˹����������ַ���ID����ַ���
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/3/14
 */

#ifndef DSM_STRINGMGR_H__
#define DSM_STRINGMGR_H__


#include <map>


/**
 *      �ַ�����Դ��������
 *
 *  \note
 *      ��ԴID��һ������0��UINT����
 */
class dsm_stringMgr
{
public:

    /**
     *      ����ָ����Դ�ļ�
     */
    static bool Load(UINT uResID);

    /**
     *      ��ȡָ����ʶ���ַ���
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
     *      ��ȡ��̬ʵ������
     *
     *  \return
     *      dsm_stringMgr*          ���ؾ�̬ʵ�������ָ��
     */
    static dsm_stringMgr* getInstance()
    {
        static dsm_stringMgr s_StringMgr;
        return &s_StringMgr;
    }

private:
    typedef std::map<UINT, CString>         stringmap;
    typedef std::pair<UINT, CString>        stringpair;

    stringmap                               m_mapString;        ///< �ַ���ӳ���
};

#endif // DSM_STRINGMGR_H__
