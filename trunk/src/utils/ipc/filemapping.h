#pragma once



/*******************************************************************
 filename: CShareMemory.h
 purpose:	封装了共享内存操作类
 author:	firehood
 created:	2011.03.16
********************************************************************/
class CShareMemory
{
public:
	CShareMemory();
	~CShareMemory();
public:
	/**********************************************************
	函数名：Open
	功能：  创建或打开内存映射文件	
	参数：
	        [in]szMapName:		要创建的共享内存名称
	        [in]dwSize:		    创建共享内存的大小
	返回值：
	        0:	    失败
            1:		创建成功
	        2:      文件已存在
	***********************************************************/
	DWORD Open(LPCTSTR szMapName,DWORD dwSize);

	/**********************************************************
	函数名：Read
	功能：  从共享内存指定位置读取数据	
	参数：
	        [out]pBuf:		    存放读取的数据
	        [in]dwSize:		    读取数据的大小
	        [in]dwOffset        距共享内存起始位置的偏移量
	返回值：
	        TRUE: 成功 FALSE:失败
	***********************************************************/
	BOOL Read(void* pBuf,DWORD dwSize,DWORD dwOffset = 0);

	/**********************************************************
	函数名：Write
	功能：  从共享内存指定位置写入数据	
	参数：
	        [in]pBuf:		    待写入的数据指针
	        [in]dwSize:		    写入数据的大小
	        [in]dwOffset        距共享内存起始位置的偏移量
	返回值：
	        TRUE: 失败 FALSE:失败
	***********************************************************/
	BOOL Write(const void* pBuf,DWORD dwSize,DWORD dwOffset = 0);
	void Close(void);
private:
	HANDLE m_hShareMemory;
	LPVOID m_pMapBuffer;
	HANDLE m_hAccessMutex;
};

