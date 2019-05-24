/*#include <stdio.h>
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
ThreadPool pool("PollThreadPool");*/

#include "pollServer.h"

//打印PID
void printTid()
{
  printf("pid = %d, tid = %d\n", getpid(), CurrentThread::tid());
  printf("now %s\n", Timestamp::now().toString().c_str());
  LOG(INFO) << "PID :"<< getpid();
}

void print(const char* msg)
{
   printf("msg %s %s\n", Timestamp::now().toString().c_str(), msg);
}

//直播间轮询截图
void pullflvTojpg(int type , const std::string& pullUrl , const std::string& liveid)
{
   printf("tid=%d  %s\n", muduo::CurrentThread::tid(), liveid.c_str());
   
   LOG(INFO) << "轮询类型  type:"<<type << "   tid: "<<muduo::CurrentThread::tid()<<"   liveID:"<<liveid;
   //int resCode = flvToJpg(pullUrl.c_str(), liveid.c_str());
   /*if(0 != resCode)
   {
       printf("保存转换图片失败  %s\n", liveid.c_str()); 
	   LOG(ERROR) << "保存转换图片失败 liveID:"<<liveid;
   }*/
}

void handleData(int type , shared_ptr<void> vptr)
{
    if(type == int(POLLTYPE::BROADCAST))
    {
        printf("POLLTYPE::BROADCAST\n");
        shared_ptr<CommonList> list = static_pointer_cast<CommonList>(vptr);
        list->display();
    }
}

//处理定时器事件
void handleEvent(TimerParam  *param)
{
    switch(param->type)
    {
       //直播状态
       case POLLTYPE::BROADCAST:
       {
           printf("直播状态正在轮询中 %s\n", param->paramstr.c_str());
    
           shared_ptr<void> vptr = shared_ptr<CommonList>(new CommonList);
           
           pool.run(std::bind(handleData, int(param->type), vptr));       
           break;
       }
       //课程状态
       case POLLTYPE::COURSE:
       {
           printf("课程状态正在轮询中 %s\n", param->paramstr.c_str());
           break;
       }
       //直播间状态
       case POLLTYPE::BROADCAST_ROOM:
       {
           printf("直播间状态正在轮询中 %s\n", param->paramstr.c_str());
           /*if(ThreadPoolFlag)
           {
               //muduo::ThreadPool pool("ChatRoomThreadPool");
               pool.setMaxQueueSize(50);
               pool.start(5);
               ThreadPoolFlag = false;
           }*/

           for (int i = 0; i < 10; ++i)
           {
              char buf[32];
              snprintf(buf, sizeof(buf), "livdID%d", i);
              //pool.run(std::bind(printString, std::string(buf)));
              pool.run(std::bind(pullflvTojpg, int(param->type), "rtmp://192.168.1.207/live/liveid666 live=1", std::string(buf)));
           }
           break;
       }
       default:
         break;
    }
}


//处理web接口参数
void *manage_fun(void *data)
{
    while(manageFlag)
    {                 
       void  *parmdata = g_commList->popLockList(); 
       if(NULL != parmdata)
       {
	    PollParam  *param = (PollParam*)parmdata;         
            printf("参数： %d  %d  %s\n",(int)param->type , (int)param->state , param->paramstr.c_str());
            
            LOG(INFO) << "web管理线程获取参数:"<<(int)param->type<<"  "<<(int)param->state<<"  "<<param->paramstr;
           
            //添加轮询任务事件
            if(POLLSTATE::START == param->state)
            {
                  void *timerParam = timer_commList->findList((int)param->type);        
                  if(NULL == timerParam)
                  {
                      TimerParam  *timerparam = new TimerParam;
                      timerparam->type = param->type;
                      timerparam->state = param->state;
                      timerparam->paramstr = param->paramstr;
                      
                      TimerId fd = g_loop->runEvery(10,std::bind(handleEvent, timerparam));
                      timerparam->fd = fd;

                      //定时器事件对象入队列
                      timer_commList->pushList((void*)timerparam);					  
                       LOG(INFO) << "增加轮询事件成功 type:"<<(int)param->type;
                 }else
                 {
                      printf("该事件已在轮询中\n");
		      LOG(ERROR) << "该事件已在轮询中 type:"<<(int)param->type;
                 }
                  
            //取消轮询任务事件
            }else if(POLLSTATE::STOP == param->state)
            {
                 void *timerParam = timer_commList->findList((int)param->type);
                 if( NULL != timerParam)
                 {
                      TimerParam  *timerData = (TimerParam *)timerParam;                
                      TimerId fd = timerData->fd;
                      g_loop->cancel(fd);

                      //定时器事件对象出队列
                      timer_commList->popList(timerParam);
					  
		     LOG(INFO) << "取消轮询事件成功 type:"<<(int)param->type;
                 }else
                 {
                      printf("未找到该定时器任务事件\n");
		      LOG(ERROR) << "取消轮询事件失败,未找到该轮询事件  type:"<<(int)param->type;
                 }
            }else
            {
                 printf("收到非法指令！\n");
				 LOG(ERROR) << "未知的命令 type:"<<(int)param->type;
            } 

            delete param;
            param = NULL;			
       }
   }  
   return data;
}

//创建日志文件夹
int CreateLogFileDir(const char *sPathName)  
{  
      char DirName[256];  
      strcpy(DirName, sPathName);  
      int i,len = strlen(DirName);
      for(i=1; i<len; i++)  
      {  
          if(DirName[i]=='/')  
          {  
              DirName[i] = 0; 
              if(access(DirName, F_OK)!=0)  
              {     
                  if(mkdir(DirName, 0755)==-1)  
                  {   
                      printf("mkdir log error\n");
                      LOG(ERROR) << "创建日志文件夹失败";					  
                      return -1;   
                  }                   
              }  
              DirName[i] = '/';  
          }  
      }  
      return 0;  
}

//停止运行服务
void StopServer(int sig)
{
    //web服务停止
    int resCode = stopWebServer();
    if(0 != resCode)
	{
		 LOG(ERROR) << "退出web服务异常 "<<" "<<"resCode:"<<resCode;
	}
	LOG(INFO) << "已退出web服务";
   
    //web参数管理线程
    manageFlag = false;
    resCode = pthread_join(manage_t, NULL);
    if(0 != resCode)
    {
      printf("web参数管理线程  ret:%d" , resCode);
	  LOG(ERROR) << "web参数管理线程 "<<" "<<"resCode:"<<resCode;
    }
	LOG(INFO) << "已退出web参数管理线程";
	
	//回收线程池
    pool.stop();
	
    //定时器服务停止
    g_loop->quit();
    if(!g_loop)
    {
	delete g_loop;
	g_loop = NULL;
    }
    LOG(INFO) << "已退出定时器服务停止";

    //删除队列任务
    if(!g_commList)
    {
       delete g_commList;
       g_commList = NULL;
    } 
    if(!timer_commList)
    {
      delete timer_commList;
      timer_commList = NULL;
    }
}

int main()
{

  int resCode = 0;
  printTid();

  //从配置文件读取http接口参数
  CConfigFileReader config_file("poll.conf");
    
  ServerIP  = config_file.GetConfigName("ServerIP");
  ServerAllID = config_file.GetConfigName("ServerAllID");
  SelectState = config_file.GetConfigName("SelectState");
  UpdataState = config_file.GetConfigName("UpdataState");
  PullUrl  = config_file.GetConfigName("PullUrl");

  string StateTime = config_file.GetConfigName("StatePollTime");
  string BroadcastTime = config_file.GetConfigName("BroadcastPollTime");
  StatePollTime =  std::stoi(StateTime);
  BroadcastPollTime = std::stoi(BroadcastTime);
  //char* str_max_conn_cnt = config_file.GetConfigName("MaxConnCnt");
  //char* str_aes_key = config_file.GetConfigName("aesKey");
  //uint32_t db_server_count = 0;
  //serv_info_t* db_server_list = read_server_config(&config_file, "DBServerIP", "DBServerPort", db_server_count);
  
  printf("configInfo: %s %s %s %s %s %d %d\n", ServerIP.c_str(), ServerAllID.c_str(),SelectState.c_str(),
                 UpdataState.c_str(),PullUrl.c_str(),StatePollTime,BroadcastPollTime);
    

  //停止服务信号
  signal(SIGINT, StopServer);
  signal(SIGHUP,StopServer);
  
  //创建日志文件夹
  CreateLogFileDir(logstr.c_str());
  
  //创建log初始化
  google::InitGoogleLogging("");

  string logfile = logstr + "poll";
  google::SetLogDestination(google::GLOG_INFO,logfile.c_str());
  FLAGS_logbufsecs = 0; //缓冲日志输出，默认为30秒，此处改为立即输出
  FLAGS_max_log_size = 500; //最大日志大小为 100MB
  FLAGS_stop_logging_if_full_disk = true; //当磁盘被写满时，停止日志输出*/
  
  //web接口参数队列
  g_commList = new CommonList(true);
  if(NULL == g_commList)
  {
     resCode = 1;
     printf("resCode: %d errInfo:%s", resCode, "web参数队列初始化失败！");
	 LOG(ERROR) << "web参数队列初始化失败 "<<" "<<"resCode:"<<resCode;
     return resCode;
  }

  //定时器事件队列
  timer_commList = new CommonList(false);
  if(NULL == timer_commList)
  {
     resCode = 2;
     printf("resCode: %d errInfo:%s", resCode, "定时器事件队列初始化失败！");
	 LOG(ERROR) << "定时器事件队列初始化失败 "<<" "<<"resCode:"<<resCode;
     return resCode;
  }
  
  //启动线程池
  pool.setMaxQueueSize(1000);
  pool.start(10);
 
  //web参数管理线程
  resCode = pthread_create(&manage_t, NULL, manage_fun, NULL);
  if(0 != resCode)
  { 
       printf("resCode: %d errInfo:%s", resCode, "管理线程创建失败");
       LOG(ERROR) << "管理线程创建失败"<<" "<<"resCode:"<<resCode; 
       return resCode; 
  }
  LOG(INFO) << "已启动web参数管理线程";
 
  //启动web服务
  resCode = startWebServer(g_commList);
  if(0 != resCode)
  {
     printf("resCode: %d errInfo:%s", resCode, "启动web服务失败！");
	 LOG(ERROR) << "启动web服务失败"<<" "<<"resCode:"<<resCode; 
     return resCode;
  }
  LOG(INFO) << "已启动web服务"; 
  
  //定时器服务
  g_loop =new EventLoop();
  if(NULL == g_loop)
  {
     resCode = 3;
	 LOG(ERROR) << "初始化定时器失败"<<" "<<"resCode:"<<resCode; 
     printf("resCode: %d errInfo:%s", resCode, "初始化定时器失败"); 
     return resCode;
  }
  LOG(INFO) << "已启动定时器服务";
  
  g_loop->runEvery(1, std::bind(print, "once1"));

  //定时器服务开始轮询
  g_loop->loop();
  
  return resCode;
}
