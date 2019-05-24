/*****************************************************
版权所有:北京三海教育科技有限公司
作者：lijian
版本：V0.0.1
时间：2019-05-17
功能：轮询服务，定时执行任务

v 0.0.1
2019.01.17 完成轮询服务得基本框架，包括web接口

v 0.0.2
2019.05.24 
1.优化轮询服务代码
2.增加监控进程

******************************************************/

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <string.h>
#include "webserver.h"
#include "EventLoop.h"
#include "EventLoopThread.h"
#include "Thread.h"
#include "ThreadPool.h"
#include "Exception.h"
#include "ConfigFileReader.h"
#include "PollBroadcastRoom.h"
#include "common.h"
#include "BaseList.h"

using namespace muduo;
using namespace muduo::net;
using namespace std;

extern string  ServerIP,ServerAllID,SelectState,UpdataState,PullUrl;
extern int StatePollTime,BroadcastPollTime;

EventLoop* g_loop;
CommonList* g_commList;
CommonList *timer_commList;

bool manageFlag = true;
string logstr = "./log/";
pthread_t manage_t;
ThreadPool pool("PollThreadPool");
