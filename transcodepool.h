#ifndef TRANSCODEPOOL_H
#define TRANSCODEPOOL_H

#ifdef _WIN32
//Windows
extern "C"
{
#include "libavformat/avformat.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#ifdef __cplusplus
};
#endif
#endif

#include "pthread.h"
#define PIN_NUM 7

//#include "mfxmvc.h"
//#include "mfxjpeg.h"
#include "mfxplugin.h"
#include "mfxplugin++.h"
#include "mfxvideo.h"
#include "mfxvideo++.h"

typedef unsigned char BYTE;

class transcodepool
{
public:
    transcodepool();
    ~transcodepool();
    void Init();
    bool GetFrame( uint8_t *YFrameBuf,  int DataLength, unsigned long * plTimeStamp);
    bool PutFrame( mfxFrameSurface1 *pSurface);
    bool PutFrame( AVFrame *pVideoframe );

private:
    pthread_mutex_t lockerx;
    uint8_t* yQueue_buf;
    int ybufsize;
    volatile int ywrite_ptr;
    volatile int yread_ptr;
    unsigned long TimeStamp;
};

#endif // TRANSCODEPOOL_H
