#include "PollLiveState.h"

PollLiveState::PollLiveState()
{
	 m_httpclient = new LibcurClient;
}


//轮询直播状态
int PollLiveState::pollState(string &selectUrl, string &select)
{
        string liveID = "100120190330P6jUPHIs";
	int resCode = selectStateByliveID(liveID);
        return resCode;
}

//根据liveID轮询状态
int  PollLiveState::selectStateByliveID(string &liveID)
{
    string url = "http://192.168.1.205:8080/live/live_select?liveId=";
	url.append(liveID).append("&userType=4");
	
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
		             json liveinfoObj = m_object.at("live_info");
					 liveFlagStr = liveinfoObj.value("liveFlag", "oops");
					 recordFlagStr = liveinfoObj.value("recordFlag", "oops");
                     planStartTimeStr = liveinfoObj.value("planStartTime", "oops");							 
					 planDurationStr = liveinfoObj.value("planDuration", "oops");		                 
                     realStartTimeStr = liveinfoObj.value("realStartTime", "oops");
					 
		     pollStateByliveID(liveID ,atoi(liveFlagStr.c_str()) ,atoi(recordFlagStr.c_str()) ,atoi(planStartTimeStr.c_str()), 
			                atoi(planDurationStr.c_str()), atoi(realStartTimeStr.c_str()),atoi(nowTimeStr.c_str()), 0);
			   }
			}
	   }
	}

        return main_ret;
}

int PollLiveState::pollStateByliveID(string &liveID ,int liveFlag ,int recordFlag, int planStartTime, int planDuration, int realStartTime, int nowTime,  
                   int lastUpdataTime)
{
	//struct timeval tv;
    //gettimeofday(&tv,NULL);
    //int nowTime =  (int)tv.tv_sec;
	int flag = 0;
		     
    if(realStartTime > planStartTime + planDuration) //在规定时间外上课
	{
		flag = 8;
        
        //修改直播状态
	    changeStateByliveID(liveID,flag,STATETYPE::LIVE);    		
    }
	if(liveFlag == 0)
    {
		if(nowTime > planStartTime + planDuration)//过期没上课
		{
			flag = 8;
			 
			//修改直播状态
	        changeStateByliveID(liveID,flag,STATETYPE::LIVE);    
	    }
    }else if(liveFlag == 1)
	{
		if(nowTime > planStartTime + planDuration)//到了结束时间没结束
	    {
		   flag = 7;
		   
		   //修改直播状态
	       changeStateByliveID(liveID,flag,STATETYPE::LIVE);    
	    }
	}else if(liveFlag == 3 && liveFlag == 4)
	{
		 if(nowTime - lastUpdataTime > 15*60)//直播中断后规定时间内未重新直播(15分钟)
		 {
			 flag = 9;
			 
			 //修改直播状态
	         changeStateByliveID(liveID,flag,STATETYPE::LIVE);    		 
		 }	 
    }		   
	   
	//在直播中判断录制状态是否在线	
	if(liveFlag == 1)
    {
		  if(recordFlag == 5 &&  recordFlag == 6)
	      {
			  flag = recordFlag;
		  }
			    
	      //修改直播状态
		  changeStateByliveID(liveID,flag,STATETYPE::LIVE);   
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
	          url = "http://192.168.1.205:8080/live/live_update?liveId=";
	          url.append(liveID).append("&liveFlag=").append(std::to_string(Flag)).append("&operateId=").append("4");
			  break;
	    case  STATETYPE::RECORD:
	          url = "http://192.168.1.205:8080/live/live_update?liveId=";
	          url.append(liveID).append("&recordFlag=").append(std::to_string(Flag)).append("&operateId=").append("4");
			  break;
            case  STATETYPE::MERGE:
                  break;
	}

	int main_ret = m_httpclient->HttpGetData(url.c_str());
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
		          printf("修改状态成功");   
			   }else{
				  printf("修改状态失败");  
			   }
         }
   }else
   {
	   printf("Http接口返回 数据为空");   
   }
   return main_ret;
}

PollLiveState::~PollLiveState()
{
	if(NULL != m_httpclient)
	{
		delete m_httpclient;
		m_httpclient = NULL;
	}
}


































