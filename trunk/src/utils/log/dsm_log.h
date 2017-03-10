/**
 * \file
 *      dsm_log.h
 *
 * \brief
 *      日志模块头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
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
    LEVEL_INFO = 0,             ///< 信息
    LEVEL_WARN,                 ///< 警告
    LEVEL_ERRO                  ///< 错误
};


/**
 *  日志模块接口实现头文件
 *
 * @note
 *      开启一个线程专门写日志文件
 */
class dsm_log
{
public:

    /**
     *      初始化日志模块
     *
     * \param[in]
     *      pszFile         日志文件名，默认值是log.txt
     * \return
     *      bool            初始化成功返回true，失败返回false
     */
    static bool Init(const wchar_t* pszLogFile = L"log.txt");


    /**
     *      写入一条日志
     *
     * \param[in]
     *      pszMsg          日志内容
     * \param[in]
     *      level           日志级别，默认是信息类型
     * \return
     *      bool            添加日志到列表中，成功返回true，失败返回false
     */
    static bool Write(const wchar_t* pszMsg, LOG_LEVEL level = LEVEL_INFO);

private:
    dsm_log();

    virtual ~dsm_log();

private:

    /**
     *      写日志文件线程函数
     */
    static DWORD WINAPI logThread(void* arg);

    /**
     *       反初始化日志模块，清理资源
     */
    static bool uninitLog();


    /**
     *      获取静态实例对象
     */
    static dsm_log& getInstance()
    {
        static dsm_log s_dsmLog;
        return s_dsmLog;
    }


private:

    /**
     * 日志内容结构体
     */
    struct _LOGDATA
    {
        LOG_LEVEL       level;              ///< 日志级别
        SYSTEMTIME      systemtime;         ///< 日志记录时间
        std::wstring    logmsg;             ///< 日志内容
    };

    typedef std::list<_LOGDATA>      listlog;                   ///< 日志信息列表类型


private:
    bool                            m_bInited;                  ///< 已初始化状态
    bool                            m_bRun;                     ///< 日志模块运行状态
    HANDLE                          m_hSemaphore;               ///< 写日志线程的信号量
    HANDLE                          m_hLogThread;               ///< 写日志线程
    CString                         m_csLogFile;                ///< 日志文件名
    CRITICAL_SECTION                m_criticalSection;          ///< 访问日志信息队列的临界区对象
    listlog                         m_lstLog;                   ///< 待写入文件的日志信息列表
};


#endif // DSM_LOG_H__
