#ifndef __WRITE_LOG_H
#define __WRITE_LOG_H

//本头文件用于声明一些记录测试日志的函数

//在调用WriteLog函数之前必须先调用
BOOL InitWriteLog(const WCHAR *strLogPath,HWND hMainWnd);
void DeInitWritelog();
int WriteLog(const WCHAR *strFormat,... );

//以下两个函数用于外界获取最近写入的日志
DWORD   GetLastLog(const WCHAR *&pLogStr);
BOOL    ReleaseLastLog();
#endif