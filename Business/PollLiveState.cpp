#include "PollLiveState.h"

PollLiveState::PollLiveState()
{
    printf("直播状态正在轮询中6666...  \n");
    m_httpclient = new LibcurClient;
    if(NULL == m_httpclient) {
        printf("Http客户端初始化失败. \n");
        LOG(ERROR) << "Http客户端初始化失败";
    }
}

PollLiveState::~PollLiveState()
{
   if(NULL != m_httpclient)
   {
       delete m_httpclient;
       m_httpclient = NULL;
   }
}

//轮询直播状态
void PollLiveState::pollState()
{
    //string liveID = "100120190330P6jUPHIs";
	//int resCode = selectStateByliveID(liveID);
	LOG(INFO) << "轮询直播状态";
	printf("直播状态正在轮询中6666...  \n");
}

//根据liveID轮询状态
int  PollLiveState::selectStateByliveID(string &liveID)
{
	string url = ServerIP;
	url.append(SelectState);
	url.append(liveID).append("&userType=4");

	LOG(INFO) << "查询直播状态 url："<<url  << "  liveID"<< liveID;
	int main_ret = m_httpclient->HttpGetData(url.c_str());
	string liveFlagStr , recordFlagStr, planStartTimeStr, planDurationStr,realStartTimeStr,nowTimeStr;
    if(0 == main_ret)
    {
		std::string resdata = m_httpclient->GetResdata();
		if(!resdata.empty())
        {   
            json m_object = json::parse(resdata);
            if(m_object.is_object())
            {
               string resCode = m_object.value("code", "oops");
               int ret = atoi(resCode.c_str() );
               if(0 == ret)
               {
               	   nowTimeStr = m_object.value("timestamp", "oops");
               	   json liveObj = m_object.at("live_info");
               	   liveFlagStr = liveObj.value("liveFlag", "oops");
               	   recordFlagStr = liveObj.value("recordFlag", "oops");
               	   planStartTimeStr = liveObj.value("planStartTime", "oops");
               	   planDurationStr = liveObj.value("planDuration", "oops");
               	   realStartTimeStr = liveObj.value("realStartTime", "oops");
					 
               	   pollStateByliveID(liveID ,atoi(liveFlagStr.c_str()) ,atoi(recordFlagStr.c_str()) ,atoi(planStartTimeStr.c_str()),
               	   		             atoi(planDurationStr.c_str()), atoi(realStartTimeStr.c_str()),atoi(nowTimeStr.c_str()), 0);
			   }
			}
	    }else{
			LOG(ERROR) << "调用Http返回数据为空 main_ret:"<<main_ret<<"  liveID"<< liveID;
		}
	}else
	{
		LOG(ERROR) << "调用Http接口失败  main_ret:"<<main_ret<<"  liveID"<< liveID;
	}
	return main_ret;
}

int PollLiveState::pollStateByliveID(string &liveID ,int liveFlag ,int recordFlag, int planStartTime, int planDuration, int realStartTime, int nowTime,  
                   int lastUpdataTime)
{
	int flag = 0;
    if(realStartTime > planStartTime + planDuration) //在规定时间外上课
	{
		LOG(INFO) << "查询到在排课时间外上课,修改直播状态为 8  liveID:"<<liveID;
		flag = 8;
        //修改直播状态
	    changeStateByliveID(liveID,flag,STATETYPE::LIVE);
    }
	if(liveFlag == 0)
    {
		if(nowTime > planStartTime + planDuration)//过期没上课
		{
			LOG(INFO) << "查询到该课程已过期,修改直播状态为 8  liveID:"<<liveID;
			flag = 8;
			//修改直播状态
	        changeStateByliveID(liveID,flag,STATETYPE::LIVE);    
	    }
    }else if(liveFlag == 1)
	{
		if(nowTime > planStartTime + planDuration)//到了结束时间没结束
	    {
			LOG(INFO) << "查询到课程在结束时间外上课,修改直播状态为 7  liveID:"<<liveID;
		    flag = 7;
		    //修改直播状态
	        changeStateByliveID(liveID,flag,STATETYPE::LIVE);

	    }else if(recordFlag == 5 ||  recordFlag == 6) //判断录制状态是否在线正常录制
		{
			LOG(INFO) << "查询到在上课过程中录制状态异常 修改直播状态为录制状态 "<<recordFlag<< "   liveID:"<<liveID;
			flag = recordFlag;
			//修改直播状态
			changeStateByliveID(liveID,flag,STATETYPE::LIVE);
		}
	}else if(liveFlag == 3 || liveFlag == 4)
	{
		 if((nowTime - lastUpdataTime) > 15*60) //直播中断后规定时间内未重新直播(15分钟)
		 {
			 LOG(INFO) << "查询到直播中断后规定时间内未重新直播 ,修改直播状态为 9  liveID:"<<liveID;
			 flag = 9;
			 //修改直播状态
	         changeStateByliveID(liveID,flag,STATETYPE::LIVE);    		 
		 }	 
    }
    return 0;
}

//修改liveDI状态
int PollLiveState::changeStateByliveID(string &liveID ,int Flag,  STATETYPE type)
{
	string url;
	switch (type)
	{
		case  STATETYPE::LIVE:
	          //url = "http://192.168.1.205:8080/live/live_update?liveId=";
	          url = ServerIP;
			  url.append(UpdataState);
	          url.append(liveID).append("&liveFlag=").append(std::to_string(Flag)).append("&operateId=").append("4");
			  LOG(INFO) << "修改直播状态 url："<<url  << "  liveID"<< liveID;
			  break;
	    case  STATETYPE::RECORD:
	          //url = "http://192.168.1.205:8080/live/live_update?liveId=";
			  url = ServerIP;
			  url.append(UpdataState);
	          url.append(liveID).append("&recordFlag=").append(std::to_string(Flag)).append("&operateId=").append("4");
			  LOG(INFO) << "修改录制状态 url："<<url  << "  liveID"<< liveID;
			  break;
		case  STATETYPE::MERGE:
			  break;
	}

	//修改直播状态
	int main_ret = m_httpclient->HttpGetData(url.c_str());
	std::string resdata = m_httpclient->GetResdata();
	if(0 == main_ret)
	{
		if(!resdata.empty())
		{
			json m_object = json::parse(resdata);
			if(m_object.is_object())
			{
				string resCode = m_object.value("code", "oops");
				int ret = atoi(resCode.c_str() );
				if(0 == ret)
				{
					printf("修改状态成功");
					LOG(INFO) << "状态修改成功  ret:"<<ret<<"  liveID"<< liveID;
				}else{
					printf("修改状态失败");
					LOG(ERROR) << "状态修改失败  ret:"<<ret<<"  liveID"<< liveID;
				}
			}
		}else {
			LOG(ERROR) << "调用Http返回数据为空  main_ret:"<<main_ret<<"  liveID"<< liveID;
		}
	}else {
		LOG(ERROR) << "调用Http接口失败  main_ret:"<<"  liveID"<< liveID;
	}
    return main_ret;
}



































