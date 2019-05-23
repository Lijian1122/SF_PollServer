/*================================================================
*   Copyright (C) 2019 All rights reserved.
*   
*   文件名称：Condition.h
*   创 建 者：lijian
*   创建日期：2019年05月15日
*   描    述：
*
#pragma once
================================================================*/

#ifndef _CONDITION_H_
#define _CONDITION_H_

#include <pthread.h>
#include "Lock.h"

class CCondition
{
public:
	CCondition(CLock *pLock);
	~CCondition();
	 
    void wait();
	
	bool waitTime(uint64_t nWaitTime);
	void notify();
	void notifyAll();
	
private:
    CLock *m_pLock;
	pthread_cond_t m_cond;
};

#endif
