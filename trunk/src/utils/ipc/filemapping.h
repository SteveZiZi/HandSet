#pragma once



/*******************************************************************
 filename: CShareMemory.h
 purpose:	��װ�˹����ڴ������
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
	��������Open
	���ܣ�  ��������ڴ�ӳ���ļ�	
	������
	        [in]szMapName:		Ҫ�����Ĺ����ڴ�����
	        [in]dwSize:		    ���������ڴ�Ĵ�С
	����ֵ��
	        0:	    ʧ��
            1:		�����ɹ�
	        2:      �ļ��Ѵ���
	***********************************************************/
	DWORD Open(LPCTSTR szMapName,DWORD dwSize);

	/**********************************************************
	��������Read
	���ܣ�  �ӹ����ڴ�ָ��λ�ö�ȡ����	
	������
	        [out]pBuf:		    ��Ŷ�ȡ������
	        [in]dwSize:		    ��ȡ���ݵĴ�С
	        [in]dwOffset        �๲���ڴ���ʼλ�õ�ƫ����
	����ֵ��
	        TRUE: �ɹ� FALSE:ʧ��
	***********************************************************/
	BOOL Read(void* pBuf,DWORD dwSize,DWORD dwOffset = 0);

	/**********************************************************
	��������Write
	���ܣ�  �ӹ����ڴ�ָ��λ��д������	
	������
	        [in]pBuf:		    ��д�������ָ��
	        [in]dwSize:		    д�����ݵĴ�С
	        [in]dwOffset        �๲���ڴ���ʼλ�õ�ƫ����
	����ֵ��
	        TRUE: ʧ�� FALSE:ʧ��
	***********************************************************/
	BOOL Write(const void* pBuf,DWORD dwSize,DWORD dwOffset = 0);
	void Close(void);
private:
	HANDLE m_hShareMemory;
	LPVOID m_pMapBuffer;
	HANDLE m_hAccessMutex;
};

