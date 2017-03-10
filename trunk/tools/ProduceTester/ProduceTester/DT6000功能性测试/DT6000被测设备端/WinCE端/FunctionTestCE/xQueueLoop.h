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
**  Descriptions: 定义实现了一个线程安全的循环队列类CxQueueLoop
个类仅支持构造和析构函数不带动态分配的类型并且已经重载好了=号运算
符，当然也支持之各种类型指针，但这个类不会负责所存入指针所指向资源的释放
**  重要说明：这个类以简单的同步机制保证在不同线程向类的对象操作的安全性
              但并不保证各操作的次序，举个简单的例子，在多个线程对一个类对象
              进行入队操作，由于线程的并发性，则对象仅能保证在队列未满的情况
              下，各元素能够入队，各元素入队顺序应先程并发性，而不可预测

  所以正确的操作，应该仅在一个线程入队，同一个线程或另一个线程出队；

**----------------------------------------------------------------------*/
#ifndef _XQUEUE_LOOP
#define _XQUEUE_LOOP

#include <windows.h>
#include <assert.h>


template<class Elem>
class CxQueueLoop
{
public:
    CxQueueLoop(int nElemMaxNum=15);                    //构造函数，nElemMaxNum表示队列的容量
    ~CxQueueLoop();

    BOOL IsEmpty();                                     //队列是否为空
    BOOL IsFull();                                      //队列是否已满

    BOOL InQueue(const Elem &e);                              //将e入队，失败返回FALSE
    BOOL OutQueue(Elem &e);                             //出队，失败返回FALSE，成功的话e为出队的元素


private:
    CRITICAL_SECTION m_cs;
    int rear,head;                                      //rear指向刚出队的元素，head指向刚入队的元素
    int   m_nElemMaxNum;                                //可以容纳的最大元素个数
    Elem *pElem;
};

template<class Elem>
CxQueueLoop<Elem>::CxQueueLoop(int nElemMaxNum/* =15 */)
{
    //虽然此处应用了malloc，但windows已经保证了malloc的线安全
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
        //assert(0);                  //队列已满
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
        assert(0);                  //队列为空
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