/*****************************************************
版权所有:北京三海教育科技有限公司
作者：lijian
版本：V0.0.1
时间：2019年05月20日
功能：轮询直播间用ffmpeg拉流生成flv，再用解析生成h264,利用libjpeg库生成图片

v 0.0.1
******************************************************/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#define __STDC_CONSTANT_MACROS
#define TEST_H264  1
#define FILEFOLDER "./RecordFile/"
#define USE_H264BSF 0

#ifndef  __POLLBROADCASTROOM_H
#define  __POLLBROADCASTROOM_H

#ifdef _WIN32
//Windows
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/mathematics.h"
#include "libavutil/time.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libjpeg/jpeglib.h"
#include "libjpeg/jmorecfg.h"
#include "libjpeg/jconfig.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/mathematics.h>
#include <libavutil/time.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <jpeglib.h>
#include <jconfig.h>
#ifdef __cplusplus
};
#endif
#endif

using namespace std;

class PollBroadcastRoom{
	
  public:
	 PollBroadcastRoom();
	 ~PollBroadcastRoom();

	 //先查询轮询正在直播的直播，然后依次轮询每个直播间
	 void pollBroadcast();

	 //根据liveID拉流生成jpeg图片
	 int flvToJpg(const char *pullUrl, const char *liveId);	
  private:
  
     //创建保存图片文件夹
     int CreateFileDir(const char *sPathName);
  
     //拉流生成Flv
     int reciveFlv(const char *pullUrl, const char *liveId);
	 
	 //解析flv生成Jpeg图片
     int flvToPic(const char *fileName);
	 
	 //解析一帧H264生成Jpeg图片
	 void saveH264ToJpeg(AVFrame* pFrame, int width, int height, const char *livId);
};
#endif

