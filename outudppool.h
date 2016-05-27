#ifndef OUTUDPPOOL_H
#define OUTUDPPOOL_H

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

#include <pthread.h>
#include "values.h"

const long NLOOPBUF_SIZE = 1024*1024;

typedef struct tagSample
{
    ULONG   lSampleLength;
    ULONG   lTimeStamp;
    BYTE  abySample[1];
}SAMPLE, *PSAMPLE;

class outudppool
{
public:
    outudppool( unsigned long lBufSize = NLOOPBUF_SIZE );
    virtual ~outudppool( void );

    bool Get( PSAMPLE pSample, bool bGetSampleFromBuffer = true );
    bool Write(const PSAMPLE pSample );
    int GetSampleCount();
    int m_deviceid;

private:

    int video;
    unsigned long m_tmpPcr;
    PBYTE m_pabyBuffer;
    volatile unsigned long m_lHead;
    volatile unsigned long m_lRear;
    int continuity_cnt;
    unsigned long m_MAXCNT;
    unsigned long m_lBufferSize;
    pthread_mutex_t m_mutex;
    int m_nSampleCount;
};

#endif // OUTUDPPOOL_H
