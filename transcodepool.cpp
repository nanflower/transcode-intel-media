#include "transcodepool.h"

FILE *fp_temp;

transcodepool::transcodepool()
{

}

transcodepool::~transcodepool()
{
    av_free(yQueue_buf);
    pthread_mutex_destroy(&lockerx);
}

void transcodepool::Init()
{
    yQueue_buf = (uint8_t*)av_mallocz(sizeof(uint8_t)*720*576*6);
    yread_ptr = 0;
    ywrite_ptr = 0;
    ybufsize = 720*576*6;

    fp_temp = fopen("temp.yuv","wb+");
    pthread_mutex_init(&lockerx, NULL);

    TimeStamp = 0;
}

bool transcodepool::GetFrame( uint8_t *YFrameBuf, int DataLength, unsigned long * plTimeStamp, int i )
{

    int width = 720;
    int height = 576;
    DataLength = width * height * 3/2;

    while(1){
        if( (ywrite_ptr == yread_ptr + DataLength) || (ywrite_ptr == DataLength&&yread_ptr == ybufsize) ){
            break;
        }
    }
//    printf("out write = %d, out read = %d, pts =%lld \n",ywrite_ptr, yread_ptr, TimeStamp);

    pthread_mutex_lock(&lockerx);

    if(ywrite_ptr == yread_ptr || (yread_ptr == ybufsize && ywrite_ptr == 0) )
        return false;
    else if(yread_ptr == ybufsize){
        memcpy(YFrameBuf, yQueue_buf, DataLength);
        yread_ptr = DataLength;
    }
    else {
        memcpy(YFrameBuf, yQueue_buf + yread_ptr, DataLength);
        yread_ptr += DataLength;
    }

    *plTimeStamp = TimeStamp;

    pthread_mutex_unlock(&lockerx);
    return true;
}

bool transcodepool::PutFrame( AVFrame *pVideoframe, int i )
{

    TimeStamp = pVideoframe->pts;

    pthread_mutex_lock(&lockerx);

    int j=0;
    if(ywrite_ptr + pVideoframe->width*pVideoframe->height*3/2 <= ybufsize){
        for( j=0; j<pVideoframe->height; j++){
            memcpy(yQueue_buf + ywrite_ptr + pVideoframe->width*j, pVideoframe->data[0] + pVideoframe->linesize[0]*j, pVideoframe->width);
        }
        ywrite_ptr += pVideoframe->width*j;

        for( j=0; j<pVideoframe->height/2; j++){
            memcpy(yQueue_buf + ywrite_ptr + pVideoframe->width/2*j, pVideoframe->data[1] + pVideoframe->linesize[1]*j, pVideoframe->width/2);
        }
        ywrite_ptr += pVideoframe->width/2*j;
        for( j=0; j<pVideoframe->height/2; j++){
            memcpy(yQueue_buf + ywrite_ptr + pVideoframe->width/2*j, pVideoframe->data[2] + pVideoframe->linesize[2]*j, pVideoframe->width/2);
        }
        ywrite_ptr += pVideoframe->width/2*j;
    }
    else{
        ywrite_ptr = 0;
        for( j=0; j<pVideoframe->height; j++){
            memcpy(yQueue_buf + ywrite_ptr + pVideoframe->width*j, pVideoframe->data[0] + pVideoframe->linesize[0]*j, pVideoframe->width);
        }
        ywrite_ptr += pVideoframe->width*j;
        for( j=0; j<pVideoframe->height/2; j++){
            memcpy(yQueue_buf + ywrite_ptr + pVideoframe->width/2*j, pVideoframe->data[1] + pVideoframe->linesize[1]*j, pVideoframe->width/2);
        }
        ywrite_ptr += pVideoframe->width/2*j;
        for( j=0; j<pVideoframe->height/2; j++){
            memcpy(yQueue_buf + ywrite_ptr + pVideoframe->width/2*j, pVideoframe->data[2] + pVideoframe->linesize[2]*j, pVideoframe->width/2);
        }
        ywrite_ptr += pVideoframe->width/2*j;

    }

    pthread_mutex_unlock(&lockerx);
    return true;
}

bool transcodepool::PutFrame( mfxFrameSurface1 *pSurface)
{
    pthread_mutex_lock(&lockerx);

    mfxFrameInfo  &pInfo = pSurface->Info;
    mfxFrameData &pData = pSurface->Data;
    TimeStamp = pSurface->Data.TimeStamp;

    int j=0;
    if(ywrite_ptr + pInfo.CropW*pInfo.CropH*3/2 <= ybufsize){
        for( j=0; j<pInfo.CropH; j++){
            memcpy(yQueue_buf + ywrite_ptr + pInfo.CropW*j, pData.Y + (pInfo.CropY * pData.Pitch + pInfo.CropX) + j * pData.Pitch,
                   pInfo.CropW);
        }
        ywrite_ptr += pInfo.CropW*j;

        for( j=0; j<pInfo.CropH/2; j++){
            memcpy(yQueue_buf + ywrite_ptr + pInfo.CropW*j, pData.UV + (pInfo.CropY * pData.Pitch / 2 + pInfo.CropX) + j * pData.Pitch,
                   pInfo.CropW);
        }
        ywrite_ptr += pInfo.CropW*j;
    }
    else{
        ywrite_ptr = 0;
        for( j=0; j<pInfo.CropH; j++){
            memcpy(yQueue_buf + ywrite_ptr + pInfo.CropW*j, pData.Y + (pInfo.CropY * pData.Pitch + pInfo.CropX) + j * pData.Pitch,
                   pInfo.CropW);
        }
        ywrite_ptr += pInfo.CropW*j;

        for( j=0; j<pInfo.CropH/2; j++){
            memcpy(yQueue_buf + ywrite_ptr + pInfo.CropW*j, pData.UV + (pInfo.CropY * pData.Pitch / 2 + pInfo.CropX) + j * pData.Pitch,
                   pInfo.CropW);
        }
        ywrite_ptr += pInfo.CropW*j;

    }

    pthread_mutex_unlock(&lockerx);
    return true;
}
