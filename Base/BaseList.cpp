#include "BaseList.h"

CommonList::CommonList(bool lockFlag)
{
	m_lockFlag = lockFlag;
	
	if(m_lockFlag)
	{
		m_lock = new CLock();
	    m_emptycond = new  CCondition(m_lock);
        m_fullcond = new CCondition(m_lock); 
    }	
    m_list.clear();

    printf("CommonList construction...\n");
}

void CommonList::pushLockList(void *data)
{
     m_lock->lock();
     m_list.push_back(data);  
     m_fullcond->notify();  
     m_lock->unlock();	
}

void *CommonList::popLockList()
{  		
     void *data = NULL;
	
     m_lock->lock();		

     if(m_list.empty())
     {
        //m_fullcond->wait();
        
        if(!m_fullcond->waitTime(3000))
        {
                //printf("timeout  in  %ld\n",pthread_self());
                m_lock->unlock();
                return data;
        }
     }

     data = m_list.front();
     m_list.pop_front();
     m_lock->unlock();	
     return data;
} 

void CommonList::pushList(void *data)
{			
   m_list.push_back(data);			
}

void CommonList::popList(void *data)
{
   m_list.remove(data);
}

/*查找队列中元素*/
void* CommonList::findList(int type)
{
   std::list<void*>::iterator iter;
   void *res = NULL;
	
   iter = find_if(m_list.begin(),m_list.end(), finder_t(type));
   if(iter != m_list.end())
   {
       res = *iter;
   }
   return res;
}

CommonList::~CommonList()
{
	if(m_lockFlag)
	{
	   if(!m_lock)
	   {
		   delete m_lock;
		   m_lock = NULL;
	   } 
	   if(!m_emptycond)
	   {
		   delete m_emptycond;
		   m_emptycond = NULL;
	   }   
	   if(!m_fullcond)
	   {
		   delete m_fullcond;
		   m_fullcond = NULL;		   
	   }
	}
	m_list.clear();

        printf("CommonList destruct...\n");
}
