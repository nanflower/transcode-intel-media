#include "decodepool.h"


decodepool::decodepool()
{

}

decodepool::~decodepool()
{
    av_free(decode_buffer);
    pthread_mutex_destroy(&lockdecode);
}

void decodepool::Init()
{

    decode_buffer = (uint8_t*)av_mallocz(sizeof(uint8_t)*100000*5);
    deread_ptr = 0;
    dewrite_ptr = 0;
    decode_bufsize = 100000*5;

    pthread_mutex_init(&lockdecode, NULL);

    DeTimeStamp = 0;
//    printf("init decode pool\n");
}

bool decodepool::getbuffer(uint8_t *pData, int LastLength, int *DataLength,unsigned long long *plTimeStamp)
{

//    while(1)
//    {
//         if( dewrite_ptr > deread_ptr  || ( (dewrite_ptr+50000) < deread_ptr) ){
//             break;
//         }
//    }
    if(dewrite_ptr == deread_ptr)
        return false;
    else if(dewrite_ptr == 0 && deread_ptr == decode_bufsize){
        deread_ptr = 0;
        return false;
    }
    int Length = 0;

    pthread_mutex_lock(&lockdecode);
    if(dewrite_ptr > deread_ptr)
    {
        Length = dewrite_ptr - deread_ptr;
        if(LastLength < Length){
            memcpy( pData, decode_buffer + deread_ptr, LastLength);
            deread_ptr += LastLength;
            *DataLength = LastLength;
        }
        else{
            memcpy( pData, decode_buffer + deread_ptr, Length);
            deread_ptr = dewrite_ptr;
            *DataLength = Length;
        }
    }
    else if(dewrite_ptr < deread_ptr)
    {
        Length = decode_bufsize - deread_ptr + dewrite_ptr;
        if(LastLength <Length){
            if(decode_bufsize - deread_ptr > LastLength){
                memcpy( pData, decode_buffer + deread_ptr, LastLength);
                deread_ptr += LastLength;
                *DataLength = LastLength;
            }
            else{
                memcpy( pData, decode_buffer + deread_ptr, decode_bufsize - deread_ptr);
                memcpy( pData + decode_bufsize - deread_ptr, decode_buffer, LastLength - decode_bufsize + deread_ptr);
                deread_ptr = LastLength - decode_bufsize + deread_ptr;
                *DataLength = LastLength;
            }
        }
        else{
            memcpy( pData, decode_buffer + deread_ptr, decode_bufsize - deread_ptr);
            memcpy( pData + decode_bufsize - deread_ptr, decode_buffer, dewrite_ptr);
            deread_ptr = dewrite_ptr;
            *DataLength = Length;
        }
    }
    else {
        *plTimeStamp = DeTimeStamp;
        pthread_mutex_unlock(&lockdecode);

        return false;
    }

    *plTimeStamp = DeTimeStamp;
    pthread_mutex_unlock(&lockdecode);

    return true;
}

bool decodepool::putbuffer(AVPacket *pVideoPacket)
{

    pthread_mutex_lock(&lockdecode);

    DeTimeStamp = pVideoPacket->pts;
    if(dewrite_ptr + pVideoPacket->size <= decode_bufsize){
        memcpy(decode_buffer + dewrite_ptr, pVideoPacket->data, pVideoPacket->size);
        dewrite_ptr += pVideoPacket->size;
    }
    else{
        int LastLength = decode_bufsize - dewrite_ptr;
        memcpy(decode_buffer + dewrite_ptr, pVideoPacket->data, LastLength);
        memcpy(decode_buffer , pVideoPacket->data + LastLength, pVideoPacket->size - LastLength);
        dewrite_ptr = pVideoPacket->size - LastLength;

    }
    av_free_packet(pVideoPacket);

    pthread_mutex_unlock(&lockdecode);
    return true;
}


