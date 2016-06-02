#include "outudppool.h"

outudppool::outudppool( unsigned long lBufSize )
{
    m_lBufferSize = lBufSize;
    if(1024*1024 == lBufSize)
        video = 0;
    else
        video = 1;
    m_pabyBuffer = new BYTE[m_lBufferSize];
    if( NULL != m_pabyBuffer )
        memset( m_pabyBuffer, 0, m_lBufferSize );

    m_lHead = 0;
    m_lRear = 0;
    m_nSampleCount = 0;
    pthread_mutex_init(&m_mutex, NULL);
}

outudppool::~outudppool(void)
{
    if( m_pabyBuffer )
    {
        delete m_pabyBuffer;
        m_pabyBuffer = NULL;
    }
    pthread_mutex_destroy( &m_mutex );
}

bool outudppool::Get( PSAMPLE pGetSample, bool bGetSampleFromBuffer )
{
    if( 0 == m_nSampleCount )	//	buffer is empty
        return false;

    if( NULL == m_pabyBuffer || NULL == pGetSample )
        return false;

    pthread_mutex_lock( &m_mutex );
    bool bRet = true;
    PBYTE pBuf = m_pabyBuffer;
    PSAMPLE pSample = (PSAMPLE)(pBuf+m_lHead);
    ULONG lRear = m_lRear;
    ULONG lSize = pSample->lSampleLength+sizeof( pSample->lTimeStamp )+sizeof( pSample->lSampleLength );
    if( lSize >= m_lBufferSize)
        return false;

    memcpy( pGetSample, pSample, lSize );

    if( lRear > m_lHead )
    {
        if( ( lRear - m_lHead ) >= lSize )
        {
            if( bGetSampleFromBuffer )
            {
                m_lHead += lSize;
                --m_nSampleCount;
            }
        }
        else
        {
            bRet = false;
        }
    }
    else
    {
        if( m_lBufferSize - m_lHead >= lSize && pSample->lSampleLength != 0 )
        {
            if( bGetSampleFromBuffer )
            {
                m_lHead += lSize;
                --m_nSampleCount;
            }
        }
        else
        {
            pSample = (PSAMPLE)pBuf;
            lSize = pSample->lSampleLength+sizeof(pSample->lTimeStamp)+sizeof( pSample->lSampleLength );
            if( lSize >= m_lBufferSize)
                return false;
            memcpy( pGetSample, pSample, lSize );
            if( lRear >= lSize )
            {
                if( bGetSampleFromBuffer )
                {
                    m_lHead = lSize;
                    --m_nSampleCount;
                }
            }
            else
                bRet = false;
        }
    }

    pthread_mutex_unlock( &m_mutex );
    return bRet;
}

bool outudppool::Write(const PSAMPLE pSample )
{
    if( NULL == pSample )
        return true;

    if( NULL == m_pabyBuffer )
        return false;

    pthread_mutex_lock( &m_mutex );

    bool bRet = true;
    ULONG lHead = m_lHead;
    ULONG lSize = pSample->lSampleLength+sizeof( pSample->lTimeStamp )+sizeof( pSample->lSampleLength );
    if( lSize >= m_lBufferSize)
        return false;
    if( m_lRear >= lHead )
    {
        if( lSize <= (m_lBufferSize - m_lRear) )
        {
            memcpy( &m_pabyBuffer[m_lRear], pSample, lSize );
            m_lRear += lSize;
            ++m_nSampleCount;
        }
        else
        {
            //  not enough buffer, set rest buffer to zero
            memset( &m_pabyBuffer[m_lRear], 0, (m_lBufferSize-m_lRear) );
            if( lSize <= lHead )
            {
                memcpy( &m_pabyBuffer[0], pSample, lSize );
                m_lRear = lSize;
                ++m_nSampleCount;
            }
            else
            {
                bRet = false;
            }
        }
    }
    else
    {
        if( m_lRear + lSize <= lHead )		//	循环队列不会溢出
        {
            memcpy( &m_pabyBuffer[m_lRear], pSample, lSize );
            m_lRear += lSize;
            ++m_nSampleCount;
        }
        else
        {
            bRet = false;
        }
    }
    pthread_mutex_unlock( &m_mutex );
    return bRet;
}

int outudppool::GetSampleCount()
{
    return m_nSampleCount;
}


