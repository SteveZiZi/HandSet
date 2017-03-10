/// @file
///
/// @brief
///     程序中各种线程优先级
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    zhangli    2013.7.16
/// @note
///     优先级数字越小，优先级越高

#ifndef THREAD_PRIORITY_H_
#define THREAD_PRIORITY_H_

#define     DATA_RECV_DETECT_THREAD_PRIO    251                         ///< 定义网络/FT3接收线程探测默认优先级
#define     DATA_RECV_PROCESS_THREAD_PRIO   108                         ///< 定义处理数据时线程优先级
#define     DSP_RECV_PROC_PRIO (DATA_RECV_PROCESS_THREAD_PRIO - 2)      ///< DSP接收线程优先级
#define     DSP_SEND_PROC_PRIO (DATA_RECV_PROCESS_THREAD_PRIO - 1)      ///< DSP发送线程优先级

#endif //THREAD_PRIORITY_H_