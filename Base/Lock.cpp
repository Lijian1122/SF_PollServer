/*================================================================
*   Copyright (C) 2019 All rights reserved.
*   
*   文件名称：Lock.h
*   创 建 者：lijian
*   创建日期：2019年05月15日
*   描    述：
*
#pragma once
================================================================*/

#include "Lock.h"

CLock::CLock()
{
#ifdef _WIN32
  InitializeCriticalSection(&m_critical_section);
#else
  pthread_mutex_init(&m_lock, NULL);
#endif
}

CLock::~CLock()
{
#ifdef _WIN32
  DeleteCriticalSection(&m_critical_section);
#else
  pthread_mutex_destroy(&m_lock);
#endif
}

void CLock::lock()
{
#ifdef _WIN32
  EnterCriticalSection(&m_critical_section);
#else
  pthread_mutex_lock(&m_lock);
#endif
}

void CLock::unlock()
{
#ifdef _WIN32
  LeaveCriticalSection(&m_critical_section);
#else
  pthread_mutex_unlock(&m_lock);
#endif
}

#ifndef _WIN32
bool CLock::try_lock()
{
    return pthread_mutex_trylock(&m_lock) == 0;
}
#endif

#ifdef _WIN32
CRWLock::CRWLock()
{
	pthread_rwlock_init(&m_lock, NULL);
}

CRWLock::CRWLock()
{
	pthread_rwlock_destory(&m_lock);
}
CRWLock::rlock()
{
	pthread_rwlock_rdlock(&m_lock);
}
CRWLock::wlock()
{
	pthread_rwlock_wrlock(&m_lock);
}

CRWLock::unlock()
{
	pthread_rwlock_unlock(&m_lock);
}

bool CRWLock::try_rlock()
{
	return pthread_rwlock_tryrdlock(&m_lock) == 0;
}

bool CRWLock::try_wlock()
{
	return pthread_rwlock_trywrlock(&m_lock) == 0;
}

CAutoRWLock::CAutoRWLock(CRWLock *pLock, bool bRLock)
{
	m_pLock = pLock;
	if(NULL != m_pLock)
	{
		if(bRLock)
		{
			m_pLock->rlock();
		}else
		{
			m_pLock->wlock();
		}
	}
}

CAutoRWLock::~CAutoRWLock()
{
	if(NULL != m_pLock)
	{
		m_pLock->unlock();
	}
}
#endif

CAutoLock::CAutoLock(CLock *pLock)
{
	m_pLock = pLock;
	if(NULL != m_pLock)
	{
		m_pLock->lock();
	}
}

CAutoLock::~CAutoLock()
{
	if(NULL != m_pLock)
	{
	   m_pLock->unlock();
	}
}
