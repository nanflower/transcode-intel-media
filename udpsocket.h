#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#define __STDC_CONSTANT_MACROS

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
#include <libavutil/audio_fifo.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#ifdef __cplusplus
};
#endif
#endif

#include <pthread.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<netdb.h>
#include<stdarg.h>
#include<string.h>
#include<errno.h>

#include "mfxvideo.h"
#include "mfxvp8.h"
#include "mfxvideo++.h"
#include "mfxplugin.h"
#include "mfxplugin++.h"
#include "mfxla.h"
#include <vector>
#include <memory>

//#include "tspoolqueue.h"
#include "one_process.h"

#define VIDEO_NUM 1
#define AUDIO_NUM 1
//#define VIDEOBUF_SIZE 10000*188
//#define BUF_SIZE 4096*2000 //read buffer
#define BUFFER_SIZE 4096      //recvfrom buffer
//#define FILE_NAME_MAX_SIZE 512
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000


class udpsocket
{
public:
    udpsocket();
    ~udpsocket();
    void thread_init(int index);
    static void* udp_tsrecv(void *param);
    void udp_ts_recv(void);
//    static int read_data(void *opaque, uint8_t *buf, int buf_size);
    int ts_demux(int index);
    int thread_test();
    static void* ts_demuxer( void *param );
    static void* video_encoder(void *param);
    void run_video_encoder(void);
//    tspoolqueue* m_tsRecvPool;
    one_process* m_transProcess;
    //pool
    void put_queue(unsigned char* buf, int size);
    int get_queue(uint8_t* buf, int size);
    void put_queue1(unsigned char* buf, int size);
    int get_queue1(uint8_t* buf, int size);
    void put_queue2(unsigned char* buf, int size);
    int get_queue2(uint8_t* buf, int size);
    void put_queue3(unsigned char* buf, int size);
    int get_queue3(uint8_t* buf, int size);
    void put_queue4(unsigned char* buf, int size);
    int get_queue4(uint8_t* buf, int size);
    void put_queue5(unsigned char* buf, int size);
    int get_queue5(uint8_t* buf, int size);
    void put_queue6(unsigned char* buf, int size);
    int get_queue6(uint8_t* buf, int size);
    void put_queue7(unsigned char* buf, int size);
    int get_queue7(uint8_t* buf, int size);
    void put_queue8(unsigned char* buf, int size);
    int get_queue8(uint8_t* buf, int size);
    void put_queue9(unsigned char* buf, int size);
    int get_queue9(uint8_t* buf, int size);
    void put_queue10(unsigned char* buf, int size);
    int get_queue10(uint8_t* buf, int size);
    void put_queue11(unsigned char* buf, int size);
    int get_queue11(uint8_t* buf, int size);
    void put_queue12(unsigned char* buf, int size);
    int get_queue12(uint8_t* buf, int size);
    void put_queue13(unsigned char* buf, int size);
    int get_queue13(uint8_t* buf, int size);
    void put_queue14(unsigned char* buf, int size);
    int get_queue14(uint8_t* buf, int size);
    void put_queue15(unsigned char* buf, int size);
    int get_queue15(uint8_t* buf, int size);

    long long GetBitRate();
    long long GetFrameNum();
    void SetQP(int QP);

private:
    int protindex;
    long long bitratebefore;
    long long curbitrate;

};

#endif // UDPSOCKET_H
