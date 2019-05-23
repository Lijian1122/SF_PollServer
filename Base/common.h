#ifdef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <string>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <iostream>

extern const char *s_http_port;

extern  string  APIStr;
extern  string  updateOnlineUrl;
extern  string  RELATIVEPATH;
extern  string  IPPORT;
extern  string  serverName;
extern  string  IpPort;
extern  string  LOGDIR;

//Http API方法名
extern string ServerCreate;
extern string ServerDelete;
extern string ServerSelect;
extern string ServerUpdate;

//合成服务ID
extern string merge_serverId;

extern string liveUpdate;
extern string liveSelect;
extern string liveUpload;

extern string  AACSTR;
extern string  H264STR;
extern string  JSONSTR;
extern string  MERGESTR;

//轮询服务http接口地址
extern string  ServerIP;
extern string  ServerAllID;
extern string  SelectState;
extern string  UpdataState;
extern string  PullUrl;
extern int  StatePollTime;
extern int  BroadcastPollTime;

#endif
