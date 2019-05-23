/*****************************************************
版权所有:北京三海教育科技有限公司
作者：lijian
版本：V0.0.1
时间：2019-05-13
功能：轮询所有功能

v 0.0.1
2019.01.22 开始写轮询服务主体框架
******************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/vfs.h>
#include <sys/syscall.h>
#include <dirent.h>
#include <iostream>
#include <map>
#include <queue>
#include <list>
#include <pthread.h>
#include <semaphore.h>
#include <string>

#include "glog/logging.h"
#include "mongoose.h"
#include "Base/base.h"
#include "Base/common.h"
//#include "Base/CommonList.h"
#include "Base/BaseList.h"

extern string IpPort,ServerCreate,ServerDelete,ServerSelect,liveUpdate,liveSelect,liveUpload,
              merge_serverId,LOGDIR,IpPort,serverName,APIStr,updateOnlineUrl;
extern const char * s_http_port;

/*int httpSev_flag = 1;
int merge_flag = 1;

typedef pair<int , string> PAIR;

ostream& operator<<(ostream& out, const PAIR& p) 
{
  return out << p.first << "\t" << p.second;
}*/


//Http接口返回值类型
enum PARSE_TYPE{ 
   GETAPI = 0,
   REGISTONLINE,
   UPDATA
};

using json = nlohmann::json;
using namespace std;

/*CommonList *ParmList;

pthread_t httpServer_t, httpTime_t, mergeManage_t;
LibcurClient *m_httpclient, *s_httpclient;*/

#ifdef  __cplusplus
	extern "C" {
#endif	

//Http处理请求
void ev_handler(struct mg_connection *nc, int ev, void *ev_data);

//处理web参数线程
void *pollManage_fun(void *data);

//解析Http返回json数据
int parseResdata(string &resdata,  int ret ,PARSE_TYPE m_Type);

//Http服务监听线程
void *httpServer_fun(void *pdata);

//停止服务
int stopWebServer();

//启动服务
int startWebServer(CommonList *list);

#ifdef __cplusplus
	}
#endif
