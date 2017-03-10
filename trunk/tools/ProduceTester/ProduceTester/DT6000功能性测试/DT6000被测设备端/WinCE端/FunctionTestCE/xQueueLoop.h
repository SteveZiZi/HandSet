/************************Copyright(c)***********************************
**             GuangZhou XuanTong Electric Technology Co.,LTD.
**
**                  http://www.xtoee.com.cn
**
**---------FILE Info----------------------------------------------------
**  File Name: xQueueLoop.h
**  Latest modified Dat:2010-04-22
**  Last Version: 1.0
**  Description: create the file xQueueLoop.h
**----------------------------------------------------------------------
**  Created By: chao
**  Created date:2010-03-15
**  Version: 1.0
**  Descriptions: ����ʵ����һ���̰߳�ȫ��ѭ��������CxQueueLoop
�����֧�ֹ������������������̬��������Ͳ����Ѿ����غ���=������
������ȻҲ֧��֮��������ָ�룬������಻�Ḻ��������ָ����ָ����Դ���ͷ�
**  ��Ҫ˵����������Լ򵥵�ͬ�����Ʊ�֤�ڲ�ͬ�߳�����Ķ�������İ�ȫ��
              ��������֤�������Ĵ��򣬾ٸ��򵥵����ӣ��ڶ���̶߳�һ�������
              ������Ӳ����������̵߳Ĳ����ԣ��������ܱ�֤�ڶ���δ�������
              �£���Ԫ���ܹ���ӣ���Ԫ�����˳��Ӧ�ȳ̲����ԣ�������Ԥ��

  ������ȷ�Ĳ�����Ӧ�ý���һ���߳���ӣ�ͬһ���̻߳���һ���̳߳��ӣ�

**----------------------------------------------------------------------*/
#ifndef _XQUEUE_LOOP
#define _XQUEUE_LOOP

#include <windows.h>
#include <assert.h>


template<class Elem>
class CxQueueLoop
{
public:
    CxQueueLoop(int nElemMaxNum=15);                    //���캯����nElemMaxNum��ʾ���е�����
    ~CxQueueLoop();

    BOOL IsEmpty();                                     //�����Ƿ�Ϊ��
    BOOL IsFull();                                      //�����Ƿ�����

    BOOL InQueue(const Elem &e);                              //��e��ӣ�ʧ�ܷ���FALSE
    BOOL OutQueue(Elem &e);                             //���ӣ�ʧ�ܷ���FALSE���ɹ��Ļ�eΪ���ӵ�Ԫ��


private:
    CRITICAL_SECTION m_cs;
    int rear,head;                                      //rearָ��ճ��ӵ�Ԫ�أ�headָ�����ӵ�Ԫ��
    int   m_nElemMaxNum;                                //�������ɵ����Ԫ�ظ���
    Elem *pElem;
};

template<class Elem>
CxQueueLoop<Elem>::CxQueueLoop(int nElemMaxNum/* =15 */)
{
    //��Ȼ�˴�Ӧ����malloc����windows�Ѿ���֤��malloc���߰�ȫ
    InitializeCriticalSection(&m_cs);
    assert(nElemMaxNum);

    pElem=(Elem *)malloc((nElemMaxNum+1)*sizeof(Elem));
    assert(pElem);

    rear=nElemMaxNum;
    head=nElemMaxNum;
    m_nElemMaxNum=nElemMaxNum;
}

template<class Elem>
CxQueueLoop<Elem>::~CxQueueLoop()
{
    free(pElem);
    DeleteCriticalSection(&m_cs);
}

template<class Elem>
BOOL CxQueueLoop<Elem>::IsEmpty()
{
    EnterCriticalSection(&m_cs);
    BOOL ret;
    ret=(head==rear)?TRUE:FALSE;
    LeaveCriticalSection(&m_cs);
    return ret;
}

template<class Elem>
BOOL CxQueueLoop<Elem>::IsFull()
{
    EnterCriticalSection(&m_cs);
    BOOL ret;
    int h=(head+1)%(m_nElemMaxNum+1);
    ret=(h==rear)?TRUE:FALSE;
    LeaveCriticalSection(&m_cs);
    return ret;
}

template<class Elem>
BOOL CxQueueLoop<Elem>::InQueue(const Elem &e)
{
    EnterCriticalSection(&m_cs); 
    BOOL ret;
    int h=(head+1)%(m_nElemMaxNum+1);
    if(h==rear)
    {
        //assert(0);                  //��������
        ret=FALSE;
    }
    else
    {
        ret=TRUE;
        pElem[h]=e;
        head=h;
    }
    LeaveCriticalSection(&m_cs);
    return ret;
}

template<class Elem>
BOOL CxQueueLoop<Elem>::OutQueue(Elem &e)
{
    EnterCriticalSection(&m_cs); 
    BOOL ret;
    int h=(head+1)%(m_nElemMaxNum+1);
    if(head==rear)
    {
        assert(0);                  //����Ϊ��
        ret=FALSE;
    }
    else
    {
        ret=TRUE;
        rear=(rear+1)%(m_nElemMaxNum+1);
        e=pElem[rear];
    }
    LeaveCriticalSection(&m_cs);
    return ret;
}




#endif