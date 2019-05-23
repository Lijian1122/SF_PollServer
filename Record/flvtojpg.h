#include <stdio.h>
#include <time.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#define __STDC_CONSTANT_MACROS


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

#ifdef __cplusplus
extern "C" {
#endif
#define TEST_H264  1
#define FILEFOLDER "./RecordFile/"

	int flvToJpg(const char *pullUrl, const char *liveId);
        int reciveFlv(const char *pullUrl, const char *liveId);
        int flvToPic(const char *fileName);
#ifdef __cplusplus
}
#endif

