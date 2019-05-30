#include <stdio.h>
#include <string.h>
#include <string>
#include "EventLoop.h"
#include "EventLoopThread.h"
#include "Thread.h"

using namespace muduo;
using namespace muduo::net;
using namespace std;

//轮询类型枚举
enum POLLTYPE{
   BROADCAST = 0, //直播状态
   COURSE, //课程状态
   BROADCAST_ROOM //直播间状态
};

//轮询状态枚举
enum POLLSTATE{
  START = 0, //开始轮询
  STOP   //停止轮询
};

//Http服务返回值枚举
enum RESCODE{
   NO_ERROR = 0,
   LIVEID_ERROR,
   METHOD_ERROR,
   MALLOC_ERROR,
   URL_ERROR  
};

//轮询参数结构体
typedef struct _PollParam{
  POLLTYPE  type;
  POLLSTATE state;  
  string  paramstr;  
}PollParam;


//Timer事件结构体
typedef struct _TimerParam{
  POLLTYPE  type;
  POLLSTATE state;
  string  paramstr;
  int  intervalTime = 600;   //轮询间隔时间单位为s
  TimerId fd;  
}TimerParam;


















