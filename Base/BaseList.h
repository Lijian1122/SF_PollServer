/*****************************************************
版权所有:北京三海教育科技有限公司
作者：lijian
版本：V0.0.1
时间：2018-01-23
功能：把线程同步操作队列的过程封装为一个类，方便调用
1.数据结构为链表；
2.线程同步用的是信号量；
3.实现了队列的基本操作，包括入队,出队,查询等的操作
******************************************************/
#ifndef COMMONLIST_H
#define COMMONLIST_H

#include <pthread.h>
#include <stdio.h>
#include <list>
#include <string>
#include <string.h>
#include <iostream>
#include <algorithm>

#include "define.h"
#include "Condition.h"
#include "glog/logging.h"


using namespace std;

/*根据liveId查找队列中的录制对象*/
typedef struct finder_t
{
  finder_t(int type)
    : m_type(type)
  {
  }
  bool operator()(void *data)
  {  
      PollParam *m_data = (PollParam*)data;
      int type = (int)m_data->type;
      return (m_type == type);
      //return false;
     
  }
  int m_type;
}finder_t;


/*线程同步类封装*/
class CommonList
{
public:
    CommonList(bool lockFlag = false);
    ~CommonList();
    int getRescode(){return m_ret;};
    
	/*加锁入队列*/
    void pushLockList(void* data);
	
	/*加锁出队列*/
    void* popLockList();
	
	/*不加锁入队列*/
	void pushList(void* data);
	
	/*不加锁出队列*/
	void popList(void *data);
	
	/*查找队列中元素*/
	void* findList(int type);

    void display(){ printf("commlist is 6666\n");}
	
private:
   
   /*条件变量*/
   CCondition *m_emptycond;
   CCondition *m_fullcond;
   
   /*锁*/
   CLock *m_lock;
   
   int m_ret;
   
   /*数据队列*/
   std::list<void*> m_list;
   
   /*是否需要加锁*/
   bool m_lockFlag;
};

#endif // COMMONLIST_H
