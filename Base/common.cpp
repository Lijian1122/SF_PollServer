#include "common.h"
#include <string>

using namespace std;

const char *s_http_port = "8083";

string  APIStr="/live/merge";
string  updateOnlineUrl;
string  RELATIVEPATH= "/recordServer/SF_MergeServer/";
string  IPPORT= "http://192.168.1.205:8080/live/";
string  serverName = "合成服务105";
string  IpPort = "http://";
string  LOGDIR = "./mergeLog/";

//Http API方法名
string ServerCreate;
string ServerDelete;
string ServerSelect;
string ServerUpdate;

//合成服务ID
string merge_serverId;

string liveUpdate;
string liveSelect;
string liveUpload;

string  AACSTR = ".aac";
string  H264STR = ".h264";
string  JSONSTR = ".json";
string  MERGESTR = "merge.";

//轮询服务http接口地址
string  ServerIP;
string  ServerAllID;
string  SelectState;
string  UpdataState;
string  PullUrl;

int  StatePollTime;
int  BroadcastPollTime;
