#ifndef __WRITE_LOG_H
#define __WRITE_LOG_H

//��ͷ�ļ���������һЩ��¼������־�ĺ���

//�ڵ���WriteLog����֮ǰ�����ȵ���
BOOL InitWriteLog(const WCHAR *strLogPath,HWND hMainWnd);
void DeInitWritelog();
int WriteLog(const WCHAR *strFormat,... );

//��������������������ȡ���д�����־
DWORD   GetLastLog(const WCHAR *&pLogStr);
BOOL    ReleaseLastLog();
#endif