/// @file
///
/// @brief
///     �����и����߳����ȼ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    zhangli    2013.7.16
/// @note
///     ���ȼ�����ԽС�����ȼ�Խ��

#ifndef THREAD_PRIORITY_H_
#define THREAD_PRIORITY_H_

#define     DATA_RECV_DETECT_THREAD_PRIO    251                         ///< ��������/FT3�����߳�̽��Ĭ�����ȼ�
#define     DATA_RECV_PROCESS_THREAD_PRIO   108                         ///< ���崦������ʱ�߳����ȼ�
#define     DSP_RECV_PROC_PRIO (DATA_RECV_PROCESS_THREAD_PRIO - 2)      ///< DSP�����߳����ȼ�
#define     DSP_SEND_PROC_PRIO (DATA_RECV_PROCESS_THREAD_PRIO - 1)      ///< DSP�����߳����ȼ�

#endif //THREAD_PRIORITY_H_