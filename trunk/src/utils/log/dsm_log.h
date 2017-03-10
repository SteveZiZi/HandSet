/**
 * \file
 *      dsm_log.h
 *
 * \brief
 *      ��־ģ��ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/3/12
 */

#ifndef DSM_LOG_H__
#define DSM_LOG_H__



#include <string>
#include <list>


enum LOG_LEVEL
{
    LEVEL_INFO = 0,             ///< ��Ϣ
    LEVEL_WARN,                 ///< ����
    LEVEL_ERRO                  ///< ����
};


/**
 *  ��־ģ��ӿ�ʵ��ͷ�ļ�
 *
 * @note
 *      ����һ���߳�ר��д��־�ļ�
 */
class dsm_log
{
public:

    /**
     *      ��ʼ����־ģ��
     *
     * \param[in]
     *      pszFile         ��־�ļ�����Ĭ��ֵ��log.txt
     * \return
     *      bool            ��ʼ���ɹ�����true��ʧ�ܷ���false
     */
    static bool Init(const wchar_t* pszLogFile = L"log.txt");


    /**
     *      д��һ����־
     *
     * \param[in]
     *      pszMsg          ��־����
     * \param[in]
     *      level           ��־����Ĭ������Ϣ����
     * \return
     *      bool            �����־���б��У��ɹ�����true��ʧ�ܷ���false
     */
    static bool Write(const wchar_t* pszMsg, LOG_LEVEL level = LEVEL_INFO);

private:
    dsm_log();

    virtual ~dsm_log();

private:

    /**
     *      д��־�ļ��̺߳���
     */
    static DWORD WINAPI logThread(void* arg);

    /**
     *       ����ʼ����־ģ�飬������Դ
     */
    static bool uninitLog();


    /**
     *      ��ȡ��̬ʵ������
     */
    static dsm_log& getInstance()
    {
        static dsm_log s_dsmLog;
        return s_dsmLog;
    }


private:

    /**
     * ��־���ݽṹ��
     */
    struct _LOGDATA
    {
        LOG_LEVEL       level;              ///< ��־����
        SYSTEMTIME      systemtime;         ///< ��־��¼ʱ��
        std::wstring    logmsg;             ///< ��־����
    };

    typedef std::list<_LOGDATA>      listlog;                   ///< ��־��Ϣ�б�����


private:
    bool                            m_bInited;                  ///< �ѳ�ʼ��״̬
    bool                            m_bRun;                     ///< ��־ģ������״̬
    HANDLE                          m_hSemaphore;               ///< д��־�̵߳��ź���
    HANDLE                          m_hLogThread;               ///< д��־�߳�
    CString                         m_csLogFile;                ///< ��־�ļ���
    CRITICAL_SECTION                m_criticalSection;          ///< ������־��Ϣ���е��ٽ�������
    listlog                         m_lstLog;                   ///< ��д���ļ�����־��Ϣ�б�
};


#endif // DSM_LOG_H__
