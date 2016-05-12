#ifndef __VALUES_H__
#define __VALUES_H__

#include <QString>
#include <stdint.h>

const int DEVICE_NUM = 17;
const int VIDEO_DEVICE_NUM = 8;
const int AUDIO_DEVICE_NUM = 8;

const int SHOW_WIDGET_W = 720;
const int SHOW_WIDGET_H = 576;

typedef enum
{
    EXCELLENT = 0,
    GOOD,
    FAIR,
    POOR,
    MOBILE
}QUALITY_ID;

typedef enum
{
    INVALID_INPUT = -1,
    INPUT_1080I_50_60 = 0,
    INPUT_720P_50_60
}INPUT_VIDEO_FORMAT;


typedef enum
{
    OUTPUT_1920_1080I_25_30 = 0,
    OUTPUT_1440_1080I_25_30,
    OUTPUT_720P_50_60,
    OUTPUT_720P_25_30,
}OUTPUT_VIDEO_FORMAT;

typedef enum
{
    RES_1080_P_60 = 1,
    RES_1080_I_60 = 2,
    RES_720_P_60 = 3,
    RES_1080_P_50 = 5,
    RES_1080_I_50 = 6,
    RES_720_P_50 = 7,
}RESOLUTION;

typedef unsigned char   BYTE;
typedef unsigned char* PBYTE;
typedef unsigned long   ULONG;
typedef unsigned int    UINT;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef DWORD * PDWORD;

#define MAX_PATH 100

typedef struct tagLogInfo
{
    bool bError;
    QString strLogMsg;
}LOG_INFO,*PLOG_INFO;
#endif // __VALUES_H__

