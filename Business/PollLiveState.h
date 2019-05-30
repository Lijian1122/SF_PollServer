/*****************************************************
版权所有:北京三海教育科技有限公司
作者：lijian
版本：V0.0.1
时间：2019年05月16日
功能：询直播状态类，用来轮询直播，录制等状态

v 0.0.1
******************************************************/

#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/time.h>
#include <unistd.h>
#include "Base/base.h"

using namespace std;
using json = nlohmann::json;

#ifndef   __POLLLIVESTATE_H_
#define  __POLLLIVESTATE_H_

//轮询类型枚举
enum STATETYPE{
   LIVE = 0, //直播状态
   RECORD, //录制状态
   MERGE //合成状态
};

class PollLiveState
{
    public:

        PollLiveState();
        ~PollLiveState();

        //轮询直播状态
        void pollState();

        //查询liveID状态
        int selectStateByliveID(string &liveID);
		
	    //判断liveID状态
	    int pollStateByliveID(string &liveID ,int liveFlag ,int recordFlag, int planStartTime, int planDuration, int realStartTime, int nowTime,int lastUpdataTime);

        //修改liveID状态
        int changeStateByliveID(string &liveID ,int Flag , STATETYPE type);		

   private:
      
	    LibcurClient *m_httpclient;
};
#endif






































