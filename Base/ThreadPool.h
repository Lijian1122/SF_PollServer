/*================================================================
*   Copyright (C) 2019 All rights reserved.
*   
*   文件名称：ThreadPoll.h
*   创 建 者：lijian
*   创建日期：2019年06月03日
*   描    述：封装linux 和 windows下的线程池的封装,利用C++11的特性
*
#pragma once
================================================================*/
#ifndef THREADPOOL_H_
#define THREADPOOL_H_
#include "ostype.h"
#include "Thread.h"
#include <pthread.h>
#include <list>

using namespace std;

typedef std::function<void()> CTask;

class CWorkerThread 
{
  public:
	CWorkerThread();
	~CWorkerThread();
    
	static void* StartRoutine(void* arg);

	/*启动线程*/
	void Start();
	
	/*执行线程池任务*/
	CTask Execute();
	
	/*任务放入任务队列*/
	void PushTask(CTask pTask);

	/*设置线程唯一标示符*/
	void SetThreadIdx(uint32_t idx) {m_thread_idx = idx;}
private:

	uint32_t		m_thread_idx;
	uint32_t		m_task_cnt;
	pthread_t		m_thread_id;
	CThreadNotify	m_thread_notify;
    list<CTask>	m_task_list;
};

class CThreadPool 
{
public:
	CThreadPool();
	virtual ~CThreadPool();

	/*初始化线程池*/
	int Init(uint32_t worker_size);
	
	/*线程池增加任务*/
	void AddTask(CTask pTask);
	
	/*销毁线程池*/
	void Destory();
private:
	uint32_t 		m_worker_size;
	CWorkerThread* 	m_worker_list;
};
#endif /* THREADPOOL_H_ */
