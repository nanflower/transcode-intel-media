#ifndef __RTMP_CLIENT_H_20150410_
#define __RTMP_CLIENT_H_20150410_

#include <rtmp.h>
#include "values.h"

#define AUDIOPACKETARRAYSIZE  10000

enum
{
    FLV_CODECID_H264 = 7,
};
// NALU单元
typedef struct _NaluUnit
{
    int type;
    int size;
    unsigned char *data;
}NaluUnit;

typedef struct _RTMPMetadata
{
    // video, must be h264 type
    unsigned int	nWidth;
    unsigned int	nHeight;
    unsigned int	nFrameRate;		// fps
    unsigned int	nVideoDataRate;	// bps
    unsigned int	nSpsLen;
    unsigned char	Sps[1024];
    unsigned int	nPpsLen;
    unsigned char	Pps[1024];

    // audio, must be aac type
    bool	        bHasAudio;
    unsigned int	nAudioSampleRate;
    unsigned int	nAudioSampleSize;
    unsigned int	nAudioChannels;
    char		    pAudioSpecCfg;
    unsigned int	nAudioSpecCfgLen;
} RTMPMetadata,*LPRTMPMetadata;

// mwb 2014.12.08 add 支持从播出端控制AudioDelay
// 保存接收到的音频数据
typedef struct tagAudioPacketData
{
    BYTE  m_abyAudioData[2048];
    int   m_nDataSize;
    DWORD m_dwPTS;
}AUDIO_PACKET_DATA,*PAUDIO_PACKET_DATA;


class CRtmpClient
{
public:
    CRtmpClient();
    virtual ~CRtmpClient();
public:
    int Initialize();
    bool Connect();
    int Send( PBYTE pbuf, int nBufLen, int type,DWORD dwTimeStamp,unsigned int nCTOffset = 0);
    int Close();
    void Reset();
    void SetVideoParam(int nVideoWidth,int nVideoHeight,int nVideoRate);
    void SetAudioParam(int nFreq);
    void VideoRateChanged(int nVideoRate);
    void SetAudioDelay(DWORD dwAudioDelay);
    void SetURL(const char *strURL);
    void FreeRtmp();

private:
    // 从缓存中读取一个NALU包
    int ReadOneNaluFromBuf(NaluUnit &nalu, BYTE *pByte, int nBufLen,unsigned int dwVideoTimeStamp,unsigned int dwCTOffset);
    void FillMetaData(NaluUnit &nalu);
    int SendRTMPPacket(PBYTE pbuf, int nBufLen, int type, unsigned int timestamp);
    int SendMetaData(LPRTMPMetadata pMetaData);
    int SendH264Packet(PBYTE pBuf,unsigned int size,bool bIsKeyFrame,unsigned int dwVideoTimeStamp,unsigned int dwCTOffset);
    int SendAACPacket(PBYTE pBuf, unsigned int size, ULONG dwAudioTimeStamp);
    int SetChunkSize();
private:
    RTMP *m_pRtmp;
    bool m_bConnected;
    bool m_bHaveSendVideoMetaData;
    bool m_bHaveSendAudioInfo;

    bool m_bsps;
    bool m_bpps;
    RTMPMetadata m_metaData;

    DWORD m_dwAudioDelay;
    char  m_strURL[MAX_PATH];
    DWORD m_dwCTOffset;

    // mwb 2014.12.08 add  支持从播出端控制AudioDelay
    AUDIO_PACKET_DATA m_aAudioPacketData[AUDIOPACKETARRAYSIZE];
    int m_nWirteAudioPacketIndex;
    int m_nReadAudioPacketIndex;

    int m_nCacheAudioSampleCount;
    int m_nBufferAudioSampleCount;
    volatile bool m_bSendAudioPacket;

public:
    int m_nVideoWidth;
    int m_nVideoHeight;
    int m_nVideoFrameRate;

    int m_nAudioFrameDur;
    int m_nVideoFrameDur;
};
#endif // __RTMP_CLIENT_H_20150410_
