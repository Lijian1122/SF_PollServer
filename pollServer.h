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

2019.05.30
1.增加配置可灵活配置轮询事件模块
2.优化轮询事件任务处理
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
#include "PollLiveState.h"
#include "common.h"
#include "BaseList.h"

using namespace muduo;
using namespace muduo::net;
using namespace std;

extern string  ServerIP,ServerAllID,SelectState,UpdataState,PullUrl;
extern int StatePollTime,BroadcastPollTime;

EventLoop* g_loop;
CommonList* g_commList;

//通过web接口控制的轮询事件队列
CommonList *timer_commList;

//本地可配置的轮询事件队列
CommonList *timer_confList;

bool manageFlag = true;
string logstr = "./log/";
pthread_t manage_t;
ThreadPool pool("PollThreadPool");

//停止运行服务
void StopServer(int sig);

//创建日志文件夹
int CreateLogFileDir(const char *sPathName);

//处理web接口参数
void *manage_fun(void *data);

//处理轮询事件
void handleEvent(TimerParam  *param);

//处理线程池事件
void handleData(POLLTYPE type , shared_ptr<void> vptr);

//添加轮询任务
void addPollEvent(int type, int intervalTime);
