#ifndef OUTUDPPOOL_H
#define OUTUDPPOOL_H

#include <pthread.h>
#include "values.h"

const long NLOOPBUF_SIZE = 1024*1024;

typedef struct tagSample
{
    unsigned long   lSampleLength;
    unsigned long   lTimeStamp;
    long   lDecodeTimeStamp;
    BYTE  abySample[1];
}SAMPLE, *PSAMPLE;

class outudppool
{
public:
    outudppool( unsigned long lBufSize = NLOOPBUF_SIZE );
    virtual ~outudppool( void );

    bool getbuffer( uint8_t *pData, int LastLength, int *DataLength, unsigned long long *plTimeStamp);
    bool putbuffer( uint8_t *pData, int DataLength, unsigned long long plTimeStamp);
    int m_deviceid;

private:
    unsigned long m_tmpPcr;
    PBYTE m_pabyBuffer;
    volatile unsigned long m_lHead;
    volatile unsigned long m_lRear;
    int continuity_cnt;
    unsigned long m_MAXCNT;
    unsigned long m_lBufferSize;
    pthread_mutex_t m_mutex;
};

#endif // OUTUDPPOOL_H
