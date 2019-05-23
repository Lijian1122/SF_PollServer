#include "flvtojpg.h"


void saveFrame(AVFrame* pFrame, int width, int height, const char *livId)
{
	char fname[128] = { 0 };
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];
	int row_stride;
	uint8_t *buffer;
	FILE *fp;

	buffer = pFrame->data[0];
	
	char szBuf[256] = {0};
        time_t timer = time(NULL);
        strftime(szBuf, sizeof(szBuf), "%Y%m%d%H%M%S", localtime(&timer));
        printf("%s/n", szBuf);
	
	string fileNameStr = FILEFOLDER;
	fileNameStr.append(livId).append("/").append(szBuf).append(".jpg");

	fp = fopen(fileNameStr.c_str(), "wb");

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
	while (cinfo.next_scanline < height)
	{
		row_pointer[0] = &buffer[cinfo.next_scanline * row_stride];
		(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(fp);
	jpeg_destroy_compress(&cinfo);
	return;
}

int flvToPic(const char *livId)
{
	
	string fileNameStr = FILEFOLDER;
	fileNameStr.append(livId).append("/").append(livId).append(".flv");
	const char *filename = fileNameStr.c_str();
	
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
		return -1;
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)//获取输入流的信息。
		return -1;
	av_dump_format(pFormatCtx, 0, filename, 0);//输出码流的基本信息到标准错误

	videoIndex = -1;
	for (i = 0; i < pFormatCtx->nb_streams; ++i){//找到流索引
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
			videoIndex = i;
			break;
		}
	}
	if (videoIndex == -1)
	{
		fprintf(stderr, "unsupport codec\n");
		return -1;
	}
	pCodecCtx = pFormatCtx->streams[videoIndex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
		return -1;

	AVFrame *pFrameRGB, *pFrame;
	pFrame = av_frame_alloc();
	pFrameRGB = av_frame_alloc();
	if (pFrame == NULL)
		return -1;

	numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
	buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));

	avpicture_fill((AVPicture*)pFrameRGB, buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);

	i = 0;
	struct SwsContext* img_convert_ctx;
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

	while (av_read_frame(pFormatCtx, &packet) >= 0){//读包

		if(picFlag)
		{
			break;
		}
		if (packet.stream_index == videoIndex){
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

			if (frameFinished)//够一帧的话就进行转码
			{
				sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
					pFrameRGB->data, pFrameRGB->linesize);

				if (++i <= 100)
					saveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, livId);//保存图片
			}
			//break;
		}
		picFlag = true;
		av_free_packet(&packet);
	}

	av_free(buffer);
	av_free(pFrameRGB);
	av_free(pFrame);
	avcodec_close(pCodecCtx);
	return 0;
}
