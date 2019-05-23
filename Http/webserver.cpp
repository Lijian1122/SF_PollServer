#include "webserver.h"
//#include "Base/CommonList.h"

int httpSev_flag;

typedef pair<int , string> PAIR;
ostream& operator<<(ostream& out, const PAIR& p) 
{
  return out << p.first << "\t" << p.second;
}

CommonList *ParmList;
pthread_t httpServer_t, httpTime_t, mergeManage_t;
LibcurClient *m_httpclient, *s_httpclient;

//Http处理请求
void ev_handler(struct mg_connection *nc, int ev, void *ev_data) 
{
      switch (ev) 
      {
         case MG_EV_ACCEPT: 
         {
            char addr[32];
            mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr),
                          MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
            break;
         }
         case MG_EV_HTTP_REQUEST: 
         {
             struct http_message *hm = (struct http_message *) ev_data;
             char addr[32];
             mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr),
                          MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
             
             char url[(int)hm->uri.len];
             sprintf(url, "%.*s",(int)hm->uri.len,hm->uri.p);
             //LOG(INFO) << "Get Url:"<<url;
   
             RESCODE ret = RESCODE::NO_ERROR;
             if(strcmp(url, "/live/poll") == 0)
             {				 
		int parmlen = (int)hm->query_string.len;
                char parmStr[parmlen];
                sprintf(parmStr, "%.*s",parmlen,hm->query_string.p);		 
		printf("url参数长度：%d  %s\n",parmlen ,parmStr);
                              		        
                if(parmlen == 0 &&  parmlen > 1024)
                { 
                  LOG(ERROR)<<"Url Error";
                  ret = RESCODE::URL_ERROR;
                  goto end;
                }
                char  polltype_buff[1024] = {0};
                char  pollstate_buff[1024] = {0};
                char  pollparam_buff[1024] = {0};
               
                mg_get_http_var(&hm->query_string, "polltype", (char*)polltype_buff, parmlen);
                mg_get_http_var(&hm->query_string, "pollState", (char*)pollstate_buff, parmlen);
              
                mg_get_http_var(&hm->query_string, "pollparam", (char*)pollparam_buff, parmlen);
	
              			
                if(strcmp(polltype_buff,"") == 0 ||  strcmp(pollstate_buff,"") == 0 || strcmp(pollparam_buff, "")== 0)
                {         
                   LOG(ERROR)<<"LIVEID EMPTY";
                   ret = RESCODE::LIVEID_ERROR;
                   goto end;
                }

                int type = atoi(polltype_buff);
                int state = atoi(pollstate_buff);
                
                printf("获取参数： type:%d  state:%d  param:%s\n", type, state, pollparam_buff);
                		
	        LOG(INFO)<<"解析参数 :"<<type << state << pollparam_buff;
                PollParam *param = new  PollParam;
                param->type = (POLLTYPE)type;
                param->state = (POLLSTATE)state;
                param->paramstr = pollparam_buff;
    
                ParmList->pushLockList((void*)param);				
           }else
           {
	        ret = RESCODE::METHOD_ERROR;
           }
  end:
           char numStr[10] ={};
           snprintf(numStr, sizeof(numStr), "%d",ret);
     
           //json返回值
           json obj;
           obj["code"] = numStr;
           std::string resStr = obj.dump();

           mg_send_response_line(nc, 200, "Content-Type: application/json;charset=utf-8\r\n");
           mg_printf(nc,"\r\n%s\r\n",resStr.c_str());
    
           nc->flags |= MG_F_SEND_AND_CLOSE;
           break;
      }    
      case MG_EV_CLOSE:
      {      
         break;
      }
  }
}

//解析Http返回json数据
int parseResdata(string &resdata,  int ret ,PARSE_TYPE m_Type)
{     
    if(!resdata.empty())
    {   
        json m_object = json::parse(resdata);
        if(m_object.is_object())
        {
            string resCode = m_object.value("code", "oops");
            ret = atoi(resCode.c_str() );
            if(0 == ret)
            {	  
		        switch(m_Type) 
                {
		           case PARSE_TYPE::GETAPI:  //解析获取API接口
	               {
			            json data_object = m_object.at("data");
						
		                string IP = data_object.value("ip", "oops");
                        string port = data_object.value("port", "oops");
                        IpPort = IpPort.append(IP).append(":").append(port);

                        ServerCreate = data_object.value("server_create", "oops");
                        ServerDelete = data_object.value("server_delete", "oops");
                        ServerSelect = data_object.value("server_select", "oops");
                        //ServerUpdate = m_object.value("server_update", "oops");

                        liveUpdate = data_object.value("live_update", "oops");
                        liveSelect = data_object.value("live_select", "oops");
                        liveUpload = data_object.value("live_upload", "oops");

                        cout<<IpPort  <<ServerCreate  <<ServerDelete  <<ServerSelect  <<liveUpdate <<endl;	 
			            break;
	               }
			       case PARSE_TYPE::REGISTONLINE:  //解析注册合成服务
	               {                        
				       merge_serverId = m_object.value("serverId", "oops");
                       LOG(INFO)<<"录制服务 merge_serverId:"<<merge_serverId;	
                       printf("serverID: %s\n", merge_serverId.c_str());	
			           break;
	               }
			       case PARSE_TYPE::UPDATA:  //解析定时上传合成在线
	               {
					   break;
	               }	    
              }			  
         }else
         {
			 LOG(ERROR)<<"接口返回数据异常 ret:"<< ret<<"  错误信息:"<<m_object.value("msg", "oops");
         }  
	  }else
      {
		  ret = -1;
          LOG(ERROR)<<" 接口返回数据不完整";
	  }		   
   }else
   {
	   ret = -2;
	   LOG(ERROR) << "Http接口返回 数据为空";   
   }
   
   return ret;
}

//Http服务监听 线程
void *httpServer_fun(void *pdata)
{
   struct mg_mgr mgr;
   struct mg_connection *nc;

   mg_mgr_init(&mgr, NULL);
   nc = mg_bind(&mgr, s_http_port, ev_handler);
   if (nc == NULL) 
   {
      printf("Failed to create listener!\n");
      LOG(ERROR)<<"Failed to create listener";
      httpSev_flag = 0;
      pthread_detach(pthread_self());
      return 0;
   }

   printf("合成服务已启动 on port: %s\n" ,s_http_port); 
   LOG(INFO) <<"合成服务已启动 on port:"<<s_http_port;

   mg_set_protocol_http_websocket(nc);

   while(httpSev_flag)
   {    
      mg_mgr_poll(&mgr, 1000);
   }
   return pdata;
}

//停止服务
int stopWebServer()
{
    httpSev_flag = 0;
    int main_ret = pthread_join(httpServer_t, NULL);
    if(0 != main_ret)
    {
      printf("http服务线程退出错误  ret:%d" ,main_ret); 
      LOG(ERROR)<<"http服务线程退出错误  ret:"<<main_ret; 
    } 

    if(!m_httpclient)
    {
       delete  m_httpclient;
       m_httpclient = NULL;
    }
    return main_ret;
}

//启动服务
int startWebServer(CommonList *list)
{     
    httpSev_flag = 1;

    int main_ret = 0;
    m_httpclient = new LibcurClient;
   
    //获取Http API 
    /*string url = IPPORT;
    url = url.append("main?ClientID=1001"); 
    main_ret = m_httpclient->HttpGetData(url.c_str());
    if(0 == main_ret)
    {
	   std::string resData = m_httpclient->GetResdata();
	   main_ret = parseResdata(resData, main_ret ,PARSE_TYPE::GETAPI);
	   if(0 != main_ret)
	   {
		  LOG(ERROR) << "解析Http API接口 数据失败  main_ret:"<<main_ret; 
		  return main_ret;
	   }
    }else
    {
	   LOG(ERROR) << "调用Http API接口失败   main_ret:"<<main_ret;  
       return main_ret;
    }*/

    //注册服务接口  
	/*url = IpPort;
    url.append("server_create?serverType=2&serverName=");
    url.append(ServerCreate);
    url.append("?serverName=");
    char *format = m_httpclient->UrlEncode(serverName);
    url.append(format);
    url.append("&serverType=LiveRecord&serverApi=192.168.1.206:");
    url.append(s_http_port);    
    url.append(APIStr);
    
    main_ret = m_httpclient->HttpGetData(url.c_str());
    if(0 == main_ret)
    {
	   std::string resData = m_httpclient->GetResdata();
	   main_ret = parseResdata(resData, main_ret ,PARSE_TYPE::REGISTONLINE);
	   if(0 != main_ret)
	   {
		  LOG(ERROR) << "解析注册录制 数据失败 main_ret:"<<main_ret; 
		  //return main_ret;
	   }	  
    }else
    {  
       LOG(ERROR) << "调用注册录制服务 失败  main_ret:"<<main_ret;  
       //return main_ret;
    }*/
  
    //参数队列
    ParmList = list;
 
 
    //往消息队列里面写数据
    //int msqid = getMsgQueue();
    //char numStr[1024] ={0};
    //snprintf(numStr, sizeof(numStr), "%d",merge_serverId);
    //sendMsg(msqid, CLIENT_TYPE, numStr);
  
    //创建http服务线程  
    main_ret = pthread_create(&httpServer_t, NULL, httpServer_fun, NULL);
    if(0 != main_ret)
    { 
      printf("http服务监听线程创建失败 ret:%d" ,main_ret); 
      LOG(ERROR) << "http服务监听线程创建失败"<<" "<<"main_ret:"<<main_ret;
      return main_ret;   
    }

    return main_ret;
}
