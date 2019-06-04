#include "PollBroadcastRoom.h"

PollBroadcastRoom::PollBroadcastRoom()
{
   //printf("PollBroadcastRoom constructor...\n");
    m_httpclient = new LibcurClient;
    if(NULL == m_httpclient) {
        printf("Http客户端初始化失败. \n");
        LOG(ERROR) << "Http客户端初始化失败";
    }
}

PollBroadcastRoom::~PollBroadcastRoom()
{
	if(NULL != m_httpclient)
	{
		delete m_httpclient;
		m_httpclient = NULL;
	}
   //printf("PollBroadcastRoom destructor...\n");
}

void PollBroadcastRoom::pollBroadcast()
{
	LOG(INFO) << "开始轮询直播间状态";

    std::string pullUrl="rtmp://192.168.1.207/live/liveid123 live=1";
    std::string livID = "live123";

	int ret = selectLiveInfoByID(livID);
}

int PollBroadcastRoom::selectLiveInfoByID(std::string &liveID)
{
	//根据liveID查询直播状态
	string url = ServerIP;
	url.append(SelectState);
	url.append(liveID).append("&userType=4");

	LOG(INFO) << "轮询直播间 查询直播状态 url："<<url  << "  liveID"<< liveID;
	int main_ret = m_httpclient->HttpGetData(url.c_str());
	string liveFlagStr;
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
					liveFlagStr = liveinfoObj.value("liveFlag", "oops");
					pullUrl = liveinfoObj.value("pullUrl", "oops");
					//正在直播中
					if(liveFlagStr == "1")
					{
						pullUrl.append(liveID);
						LOG(INFO) << "轮询直播间 检测到正在直播中开始进入直播间拉流  main_ret:"<<main_ret<<"  liveID"<< liveID;

						//进入直播间开始拉流
						flvToJpg(pullUrl ,livID);
					}
				}
			}
		}else{
			LOG(ERROR) << "轮询直播间 调用Http返回数据为空 main_ret:"<<main_ret<<"  liveID"<< liveID;
		}
	}else
	{
		LOG(ERROR) << "轮询直播间 调用Http接口失败  main_ret:"<<main_ret<<"  liveID"<< liveID;
	}
	return main_ret;
}

int PollBroadcastRoom::flvToJpg(std::string &pullUrl , std::string &liveId)
{
	LOG(INFO) << "轮询直播间 pullUrl:"<<pullUrl<<"  liveId"<<liveId ;

    int resCode = 0;
    resCode = CreateFileDir(liveId);

    std::string folderName = FILEFOLDER;
    folderName.append(liveId).append("/");
    resCode = CreateFileDir(folderName);
    if(0 != resCode)
    {
	   printf("创建文件夹失败  直播ID:%s", liveId);
	   LOG(ERROR) << "创建文件夹失败  liveId:"<<liveId;
       return resCode;   
    }

    //获取文件名
    std::string fileName = folderName;

    //获取当前时间戳
    char szBuf[256] = {0};
    time_t timer = time(NULL);
    strftime(szBuf, sizeof(szBuf), "%Y%m%d%H%M%S", localtime(&timer));
    printf("%s/n", szBuf);

    fileName.append("/").append(liveId).append(szBuf);

    resCode = reciveFlv(pullUrl,fileName,liveId);
	if(0 != resCode)
    {
	   printf("拉流失败  直播ID:%s", liveId);
	   LOG(ERROR) << "拉流生成flv失败  liveId:"<<liveId;
       return resCode;   
    }

    resCode = flvToPic(fileName, liveId);
    if(0 != resCode)
    {
        printf("flv转换jpg失败  直播ID:%s", liveId);
        LOG(ERROR) << "flv转换jpg失败  liveId:"<<liveId;
        return resCode;
    }

    //删除生成的flv
    fileName.append(".flv");
    remove(fileName.c_str());
    return resCode;
}

//生成目标文件夹
int PollBroadcastRoom::CreateFileDir(std::string &filePath)
{  
    char DirName[256];  
    strcpy(DirName, filePath.c_str());
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
                  return -1;   
              }       
            }  
            DirName[i] = '/';  
        }
    }  
    return 0;  
} 

int PollBroadcastRoom::reciveFlv(std::string &pullUrl, std::string &fileName std::string &liveId)
{
	int temp = 30;
	AVOutputFormat *ofmt = NULL;
	
	//Input AVFormatContext and Output AVFormatContext
	AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
	AVPacket pkt;
	const char *in_filename, *out_filename;
	int ret, i;
	int videoindex = -1;
	int frame_index = 0;

	//设置拉流url
    in_filename = pullUrl;

    //设置输出文件名
	std::string flvName = FILEFOLDER;
    flvName.append(".flv");
	out_filename = flvName.c_str();
	
	av_register_all();
	//Network
	avformat_network_init();
	//Input
	if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {
		printf("Could not open input file.");
        LOG(ERROR) << "拉流 Could not open input file.   liveId:"<<liveId;
		goto end;
	}
	if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
		printf("Failed to retrieve input stream information");
        LOG(ERROR) << "拉流 Failed to retrieve input stream information  liveId:"<<liveId;
		goto end;
	}

	for (i = 0; i<ifmt_ctx->nb_streams; i++)
	if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
		videoindex = i;
		break;
	}

	av_dump_format(ifmt_ctx, 0, in_filename, 0);

	//Output
	avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename); //RTMP

	if (!ofmt_ctx) {
		printf("Could not create output context\n");
        LOG(ERROR)<< "拉流 Could not create output context  liveId:"<<liveId;
		ret = AVERROR_UNKNOWN;
		goto end;
	}
	ofmt = ofmt_ctx->oformat;
	for (i = 0; i < ifmt_ctx->nb_streams; i++) 
	{
		//Create output AVStream according to input AVStream
		AVStream *in_stream = ifmt_ctx->streams[i];
		AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
		if (!out_stream) {
			printf("Failed allocating output stream\n");
            LOG(ERROR) << "拉流 Failed allocating output stream  liveId:"<<liveId;
			ret = AVERROR_UNKNOWN;
			goto end;
		}
		//Copy the settings of AVCodecContext
		ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
		if (ret < 0) {
			printf("Failed to copy context from input to output stream codec context\n");
            LOG(ERROR) << "拉流 Failed to copy context from input to output stream codec context  liveId:"<<liveId;
			goto end;
		}
		out_stream->codec->codec_tag = 0;
		if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
			out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
	}

	//Dump Format------------------
	av_dump_format(ofmt_ctx, 0, out_filename, 1);
	//Open output URL
	if (!(ofmt->flags & AVFMT_NOFILE)) {
		ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
		if (ret < 0) {
			printf("Could not open output URL '%s'", out_filename);
            LOG(ERROR) << "拉流 Could not open output URL  liveId:"<<liveId;
			goto end;
		}
	}
	//Write file header
	ret = avformat_write_header(ofmt_ctx, NULL);
	if (ret < 0) {
		printf("Error occurred when opening output URL\n");
        LOG(ERROR) << "拉流 Error occurred when opening output URL  liveId:"<<liveId;
		goto end;
	}

#if USE_H264BSF
	AVBitStreamFilterContext* h264bsfc = av_bitstream_filter_init("h264_mp4toannexb");
#endif
	while (1) {
		AVStream *in_stream, *out_stream;
		//Get an AVPacket
		ret = av_read_frame(ifmt_ctx, &pkt);
		if (ret < 0)
			break;

		in_stream = ifmt_ctx->streams[pkt.stream_index];
		out_stream = ofmt_ctx->streams[pkt.stream_index];
		/* copy packet */
		//Convert PTS/DTS
		pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
		pkt.pos = -1;

		//Print to Screen
		if (pkt.stream_index == videoindex){
			//printf("Receive %8d video frames from input URL\n", frame_index);
            //LOG(INFO) << "拉流 Error occurred when opening output URL  liveId:"<<liveId;
			frame_index++;
#if USE_H264BSF
			av_bitstream_filter_filter(h264bsfc, in_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif
		}
		ret = av_interleaved_write_frame(ofmt_ctx, &pkt);

		if (ret < 0) {
			printf("Error muxing packet\n");
            LOG(ERROR) << "拉流 Error muxing packet   liveId:"<<liveId;
			break;
		}
		av_free_packet(&pkt);
		if(frame_index > 100)
		{
			break;
		}
	}
#if USE_H264BSF
	av_bitstream_filter_close(h264bsfc);
#endif
	//Write file trailer
	av_write_trailer(ofmt_ctx);
end:
	avformat_close_input(&ifmt_ctx);
	/* close output */
	if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
		avio_close(ofmt_ctx->pb);
	avformat_free_context(ofmt_ctx);
	if (ret < 0 && ret != AVERROR_EOF) {
		printf("Error occurred.\n");
        LOG(ERROR) << "拉流 Error occurred.  liveId:"<<liveId;
		return -1;
	}

    LOG(ERROR) << "拉流 生成FLV完成  liveId:"<<liveId;
	return 0;
}

int PollBroadcastRoom::flvToPic(std::string &fileName, std::string &liveId)
{	
	string fileNameStr = fileName;
	fileNameStr.append(".flv");
	const char *filename = fileNameStr.c_str();

    LOG(ERROR) << "开始解析FLV  Filename:":<<fileNameStr<<"    liveId:"<<liveId;
	
	bool picFlag = false;
	AVFormatContext *pFormatCtx;
	int i, videoIndex;
	AVCodecContext *pCodecCtx = NULL;
	AVCodec *pCodec = NULL;

	AVPacket packet;
	int frameFinished;
	int numBytes;
	uint8_t *buffer;

	pFormatCtx = avformat_alloc_context();
	av_register_all();
	if (avformat_open_input(&pFormatCtx, filename, NULL, 0) != 0)//打开输入流，本质上是filename这个字符串来获取输入流的格式等信息。
    {
        LOG(ERROR) << "开始解析FLV  打开输入流失败  Filename:":<<fileNameStr<<"    liveId:"<<liveId;
        return -1;
    }

	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)//获取输入流的信息。
    {
        LOG(ERROR) << "开始解析FLV  获取输入流信息失败  Filename:":<<fileNameStr<<"    liveId:"<<liveId;
        return -2;
    }

	av_dump_format(pFormatCtx, 0, filename, 0);//输出码流的基本信息到标准错误

	videoIndex = -1;
	for (i = 0; i < pFormatCtx->nb_streams; ++i)
	{    //找到流索引
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoIndex = i;
			break;
		}
	}
	if (videoIndex == -1)
	{
		fprintf(stderr, "unsupport codec\n");
        LOG(ERROR) << "开始解析FLV  unsupport codec 解码失败  Filename:":<<fileNameStr<<"    liveId:"<<liveId;
		return -3;
	}
	pCodecCtx = pFormatCtx->streams[videoIndex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if(avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
    {
        LOG(ERROR) << "开始解析FLV  avcodec_open2错误  Filename:":<<fileNameStr<<"    liveId:"<<liveId;
        return -4;
    }
	AVFrame *pFrameRGB, *pFrame;
	pFrame = av_frame_alloc();
	pFrameRGB = av_frame_alloc();
	if(pFrame == NULL)
    {
        LOG(ERROR) << "开始解析FLV  av_frame_alloc为空  Filename:":<<fileNameStr<<"    liveId:"<<liveId;
        return -5;
    }

	numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
	buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
	avpicture_fill((AVPicture*)pFrameRGB, buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);

	struct SwsContext* img_convert_ctx;
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

	while (av_read_frame(pFormatCtx, &packet) >= 0)
	{   //读包
	    if(picFlag)
		{
			break;
		}
		if (packet.stream_index == videoIndex)
		{
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
			if (frameFinished)//够一帧的话就进行转码
			{

                LOG(ERROR) << "开始解析FLV 解析到一帧H264开始生成Jepg  Filename:":<<fileNameStr<<"    liveId:"<<liveId;

				sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
					pFrameRGB->data, pFrameRGB->linesize);
				saveH264ToJpeg(pFrameRGB, pCodecCtx->width, pCodecCtx->height, fileName);//保存图片
			}
		}
		picFlag = true;
		av_free_packet(&packet);
	}

	av_free(buffer);
	av_free(pFrameRGB);
	av_free(pFrame);
	avcodec_close(pCodecCtx);

    LOG(INFO) << "开始解析FLV  生成Jepg结束  Filename:":<<fileNameStr<<"    liveId:"<<liveId;

	return 0;
}

void PollBroadcastRoom::saveH264ToJpeg(AVFrame* pFrame, int width, int height, std::string &fileName, std::string &liveID)
{
	char fname[128] = { 0 };
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];
	int row_stride;
	uint8_t *buffer;
	FILE *fp;
	buffer = pFrame->data[0];

	string fileNameStr = fileName;
	fileNameStr.append(".jpg");

    LOG(INFO) << "H264开始转化Jpeg Filename:":<<fileNameStr<<"    liveId:"<<liveID;

	fp = fopen(fileNameStr.c_str(), "wb");
	if(fp == NULL)
    {
        LOG(ERROR) << "H264开始转化Jpeg 打开文件失败 Filename:":<<fileNameStr<<"    liveId:"<<liveID;
        return;
    }

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, fp);

	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 80, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	row_stride = width * 3;
	while(cinfo.next_scanline < height)
	{
		row_pointer[0] = &buffer[cinfo.next_scanline * row_stride];
		(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(fp);
	jpeg_destroy_compress(&cinfo);

    LOG(INFO) << "H264开始转化Jpeg 完成转换 Filename:":<<fileNameStr<<"    liveId:"<<liveID;
	return;
}
