#include "one_process.h"
#include "global.h"
#include <QDateTime>

const int CONNECT_TIMER = 2000;

FILE *fprtmp_v;
FILE *fprtmp_a;

one_process::one_process( int index)
{
//    g_pCaptureDeviceVec[0]->Init();
    fprtmp_v = fopen("rtmpout.264","wb+");
    fprtmp_a = fopen("rtmpout.aac","wb+");

    m_bIsEncoderEnable = false;
    m_index = index;
    m_bStopEncoder = false;
    m_pVideoEncoder = NULL;
    InitEncoderPar();
    pthread_mutex_init( &m_Mutex, NULL );
    pthread_cond_init( &m_Cond, NULL );

    m_bStopDecoder = false;
    m_pVideoDecoder = NULL;
//    InitDecoderPar();
    pthread_mutex_init( &m_DeMutex, NULL );
    pthread_cond_init( &m_DeCond, NULL );

//    QVector<LOG_INFO>  m_vecSendLog;

    m_pRtmpClient1 = NULL;
    m_pRtmpClient2 = NULL;

    m_bRtmpEnable1 = true;
    m_bRtmpEnable2 = true;
    connect( &m_ConnectRtmpTimer1, SIGNAL(timeout()), this, SLOT(ConnectRtmp1()) );
    m_ConnectRtmpTimer1.setSingleShot( true );
    connect( this, SIGNAL(ConnectRtmpTimer1()), this, SLOT(ResponseConnectRtmpTimer1()) );
    connect( &m_ConnectRtmpTimer2, SIGNAL(timeout()), this, SLOT(ConnectRtmp2()) );
    m_ConnectRtmpTimer2.setSingleShot( true );
    connect( this, SIGNAL(ConnectRtmpTimer2()), this, SLOT(ResponseConnectRtmpTimer2()) );
    m_nRtmpDisconnectTime1 = 0;
    m_nRtmpDisconnectTime2 = 0;
}

one_process::~one_process()
{
    DestroyVideoEncoder();
    DestroyVideoDecoder();
}

void one_process::Init(void)
{

    bool bInitRtmpClient1 = InitRtmpClient1(); // Init rtmp client 1
    bool bInitRtmpClient2 = InitRtmpClient2();  // Init rtmp client 2
//    bool bInitUdpRtpClient = InitUdpRtpClient();   // Init udp/rtp client
    if( !bInitRtmpClient1 && !bInitRtmpClient2 )
        return ;
    m_bIsEncoderEnable = true;

    //视频decoder
    pthread_t mpeg2_decode_thread;
    memset( &mpeg2_decode_thread, 0, sizeof( mpeg2_decode_thread ) );

    if( 0 != pthread_create( &mpeg2_decode_thread, NULL, video_decoder, this ))
        printf("%s:%d  Error: Create video encoder thread failed !!!\n", __FILE__, __LINE__ );

    pthread_detach(mpeg2_decode_thread);

    //视频encoder
    pthread_t h264_encoder_thread;
    memset( &h264_encoder_thread, 0, sizeof( h264_encoder_thread ) );

    if( 0 != pthread_create( &h264_encoder_thread, NULL, video_encoder, this ))
        printf("%s:%d  Error: Create video encoder thread failed !!!\n", __FILE__, __LINE__ );

    pthread_detach(h264_encoder_thread);

    //rtmp
    pthread_t m_SendRtmpThread;
    memset( &m_SendRtmpThread, 0, sizeof( m_SendRtmpThread ) );

    if( 0 == m_SendRtmpThread && (bInitRtmpClient1 || bInitRtmpClient2) )
    {
        if( 0 != pthread_create( &m_SendRtmpThread, NULL, SendRtmpThread, this ) )
            printf("%s:%d   Error: Create send thread failed !!!\n", __FILE__, __LINE__ );
    }

    //udp sender
//    pthread_t udp_send_thread;
//    memset( &udp_send_thread, 0, sizeof( udp_send_thread ) );

//    if( 0 != pthread_create( &udp_send_thread, NULL, udp_send, this ))
//        printf("%s:%d  Error: Create video encoder thread failed !!!\n", __FILE__, __LINE__ );

//    pthread_detach(udp_send_thread);
}

void* one_process::video_decoder(void *pArg){
    one_process* pTemp = (one_process*)pArg;
    if( pTemp )
        pTemp->run_video_decoder();
    return (void*)NULL;
}

void one_process::run_video_decoder(){
    if( m_bStopDecoder )
        return;
    printf("video decoder work\n");
    bool bInitVideoDecoder = true;

    sInputParams Params;
    mfxStatus sts = MFX_ERR_NONE;
    // Init video encoder
    if( bInitVideoDecoder )
    {
        sts = InitVideoDecoder( &Params );
        if( sts != MFX_ERR_NONE  )
            return;
    }
    printf("Init over \n");
    while( 1 )
    {

        sts = m_pVideoDecoder->RunDecoding();
        if ( MFX_ERR_DEVICE_LOST == sts || MFX_ERR_DEVICE_FAILED == sts )
        {
            bInitVideoDecoder = false;
            sts = m_pVideoDecoder->ResetDecoder(&Params);
            continue;
        }
        else if( ( int )sts == 10000 )
        {
            // Pause video encoder
            PauseVideoDecoderThread();

            // Exit application
            if( m_bExitDeApplication )
                return;

            bInitVideoDecoder = true;
            continue;
        }
        else
            break;
    }

    if( m_pVideoDecoder )
        m_pVideoDecoder->Close();
}

void* one_process::video_encoder(void *pArg){
    one_process* pTemp = (one_process*)pArg;
    if( pTemp )
        pTemp->run_video_encoder();
    return (void*)NULL;
}

void one_process::run_video_encoder(){
    if( m_bStopEncoder )
        return;
    printf("video encoder work\n");
    bool bInitVideoEncoder = true;
    while( 1 )
    {
        sParams Params;
        mfxStatus sts = MFX_ERR_NONE;
        // Init video encoder
        if( bInitVideoEncoder )
        {
            sts = InitVideoEncoder( &Params );
            if( sts != MFX_ERR_NONE  )
                return;
        }

        sts = m_pVideoEncoder->Run();
        if ( MFX_ERR_DEVICE_LOST == sts || MFX_ERR_DEVICE_FAILED == sts )
        {
            bInitVideoEncoder = false;
            sts = m_pVideoEncoder->ResetMFXComponents( &Params );
            continue;
        }
        else if( ( int )sts == 10000 )
        {
            // Pause video encoder
            PauseVideoEncoderThread();

            // Exit application
            if( m_bExitApplication )
                return;

            bInitVideoEncoder = true;
            continue;
        }
        else
            break;
    }

    if( m_pVideoEncoder )
        m_pVideoEncoder->Close();
}

void* one_process::udp_send(void *pArg){
    one_process* pTemp = (one_process*)pArg;
    if( pTemp )
        pTemp->run_udp_send();
    return (void*)NULL;
}

void one_process::run_udp_send(){
    /* 创建UDP套接口 */
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //server_addr.sin_addr.s_addr = inet_addr("1.8.84.12");
    int port = 10791 + m_index;
    server_addr.sin_port = htons(port);

    /* 创建socket */
    int server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(server_socket_fd == -1)
    {
         perror("Create Socket Failed:");
         exit(1);
    }

    memset(server_addr.sin_zero,0,8);
     int re_flag=1;
     int re_len=sizeof(int);
     setsockopt(server_socket_fd,SOL_SOCKET,SO_REUSEADDR,&re_flag,re_len);

    /* 绑定套接口 */
    if(-1 == (bind(server_socket_fd,(struct sockaddr*)&server_addr,sizeof(server_addr))))
    {
         perror("Server Bind Failed:");
         exit(1);
    }
    /* 数据传输 */

    while(1)
    {
         /* 定义一个地址，用于捕获客户端地址 */
         struct sockaddr_in client_addr;
         socklen_t client_addr_length = sizeof(client_addr);
         /* 接收数据 */
         uint8_t *buffer[BUFFER_SIZE];
         bzero(buffer, BUFFER_SIZE);

//         send_Buffer[m_index]->get
//         struct timeval tv;
//         fd_set readfds;
//         tv.tv_sec = 3;
//         tv.tv_usec = 10;
//         FD_ZERO(&readfds);
//         FD_SET(server_socket_fd, &readfds);
//         select(server_socket_fd+1,&readfds,NULL, NULL, &tv);

//         if (FD_ISSET(server_socket_fd,&readfds))
//         {

        int len = sendto(server_socket_fd, buffer, BUFFER_SIZE,0,(struct sockaddr*)&client_addr, sizeof(client_addr_length));
        if (len == -1)
        {
            printf("send data error!\n");
        }
        av_free(buffer);
//         }
//         else
//         {
//             printf("error is %d\n",errno);
//             printf("timeout!there is no data arrived!\n");
//         }
    }
}

void * one_process::SendRtmpThread( void * pArg )
{
    one_process* pTemp = (one_process*) pArg;
    if( pTemp )
        pTemp->RunSendRtmp();
    return (void*)NULL;
}

void one_process::RunSendRtmp()
{
    PSAMPLE pVideoSample = (PSAMPLE)new BYTE[8+4096*1024];
    MSDK_CHECK_POINTER_NO_RET( pVideoSample );
    MSDK_ZERO_MEMORY( *pVideoSample );

    PSAMPLE pAudioSample = (PSAMPLE)new BYTE[8+4096*10];
    MSDK_CHECK_POINTER_NO_RET( pAudioSample );
    MSDK_ZERO_MEMORY( *pAudioSample );

    ULONG lVideoMaxTime = 0;
    bool bFirstSendRtmp1 = true;
    bool bFirstSendRtmp2 = true;

    while( m_bIsEncoderEnable )
    {
        // -------------------- RTMP1, RTMP2 send ------------------//
//    printf("rtmp work.....................\n");
        if( send_Buffer[m_index+16]->Get( pAudioSample ) )
        {
//            printf("rtmp audio work.....................\n");
            if( m_bRtmpEnable1 && m_pRtmpClient1 )
            {
//                printf("audio:  %lu\n", pAudioSample->lTimeStamp);

//                fwrite(pAudioSample->abySample,pAudioSample->lSampleLength,1,fprtmp_a);
//                printf("rtmp  A length = %ld, timestamp = %ld \n", pAudioSample->lSampleLength, pAudioSample->lTimeStamp);
                if( 0 == m_pRtmpClient1->Send( pAudioSample->abySample, (int)pAudioSample->lSampleLength, RTMP_PACKET_TYPE_AUDIO, (DWORD)pAudioSample->lTimeStamp ) )
                    Rtmp1SendError();
            }

//            if( m_bRtmpEnable2 && m_pRtmpClient2 )
//            {
//                if( 0 == m_pRtmpClient2->Send( pAudioSample->abySample, (int)pAudioSample->lSampleLength, RTMP_PACKET_TYPE_AUDIO, (DWORD)pAudioSample->lTimeStamp ) )
//                    Rtmp2SendError();
//            }
            // ----------------- send one video frame --------------------- //
//            printf("rtmp get  video  work..................... sample count = %d\n",m_pVideoEncoder->GetSampleCount());
//            if( m_pVideoEncoder && m_pVideoEncoder->GetSampleCount() >= 2 )
//            {
//                if( false == send_Buffer[m_index]->Get( pVideoSample )  )
//                {
//                    printf("%s:%d  Error: Get current video sample failed ,m_index = %d!!!\n", __FILE__, __LINE__,m_index );
//                    break;
//                }
//                // send  video rtmp
//                ULONG lNextTimeStamp = 0;

//                if( m_pVideoEncoder->GetTimeStamp( lNextTimeStamp ) )
//                {
////                    printf("rtmp video work.....................\n");
//                    ULONG lCurTimeStamp = pVideoSample->lTimeStamp;
//                    int nVideoTimeStampDelta = m_pVideoEncoder->GetVideoTimeStampDelta();
//                    lVideoMaxTime = ( lCurTimeStamp > lVideoMaxTime ) ? lCurTimeStamp : ( lVideoMaxTime+ nVideoTimeStampDelta );
//                    unsigned int dwVideoDelta = ( lNextTimeStamp < lCurTimeStamp )? (lCurTimeStamp - lNextTimeStamp + nVideoTimeStampDelta) : 0;

//                    if( m_bRtmpEnable1 && m_pRtmpClient1 )
//                    {
//                        if( bFirstSendRtmp1 )
//                        {
//                            bFirstSendRtmp1 = false;
//                            unsigned short nWidth, nHeight;
//                            nWidth = 720;
//                            nHeight = 576;
//                            m_pRtmpClient1->SetVideoParam( nWidth, nHeight, 25 );
//                        }
////                        printf("video:  %lu\n", pVideoSample->lTimeStamp);

////                        printf("rtmp V  = %ld ,t imestamp = %ld \n",pVideoSample->lSampleLength, pVideoSample->lTimeStamp);
//                        fwrite(pVideoSample->abySample,pVideoSample->lSampleLength,1,fprtmp_v);
////                        if( 0 == m_pRtmpClient1->Send( pVideoSample->abySample, (int)pVideoSample->lSampleLength, RTMP_PACKET_TYPE_VIDEO, (DWORD)pVideoSample->lTimeStamp ) ){
//                        if( 0 == m_pRtmpClient1->Send( pVideoSample->abySample, (int)pVideoSample->lSampleLength, RTMP_PACKET_TYPE_VIDEO, (DWORD)lVideoMaxTime, dwVideoDelta ) ){
////                            bFirstSendRtmp1 = true;
//                            Rtmp1SendError();
//                        }


////                    if( m_bRtmpEnable2 && m_pRtmpClient2 )
////                    {
////                        if( bFirstSendRtmp2 )
////                        {
////                            bFirstSendRtmp2 = false;
////                            unsigned short nWidth, nHeight;
////                            nWidth = 720;
////                            nHeight = 576;
////                            m_pRtmpClient2->SetVideoParam( nWidth, nHeight, 25 );
////                        }

////                        if( 0 == m_pRtmpClient2->Send( pVideoSample->abySample, (int)pVideoSample->lSampleLength, RTMP_PACKET_TYPE_VIDEO, (DWORD)lVideoMaxTime, dwVideoDelta ) ){
//////                            bFirstSendRtmp2 = true;
////                            Rtmp2SendError();
////                         }
////                    }
//                }
//                else
//                    printf("%s:%d  Error: Get next video sample timestamp failed !!!\n", __FILE__, __LINE__ );
//            }
//            while(pVideoSample->lTimeStamp > pAudioSample->lTimeStamp){
//                    if( send_Buffer[m_index+16]->Get( pAudioSample ) )
//                    {
//            //            printf("rtmp audio work.....................\n");
//                        if( m_bRtmpEnable1 && m_pRtmpClient1 )
//                        {
//            //                printf("audio:  %lu\n", pAudioSample->lTimeStamp);

////                            fwrite(pAudioSample->abySample,pAudioSample->lSampleLength,1,fprtmp_a);
//            //                printf("rtmp  A length = %ld, timestamp = %ld \n", pAudioSample->lSampleLength, pAudioSample->lTimeStamp);
//                            if( 0 == m_pRtmpClient1->Send( pAudioSample->abySample, (int)pAudioSample->lSampleLength, RTMP_PACKET_TYPE_AUDIO, (DWORD)pAudioSample->lTimeStamp ) )
//                                Rtmp1SendError();
//                        }
//                    }
//                }
        }
        else
            usleep( 5000 );
    }
    MSDK_SAFE_DELETE( pVideoSample );
    MSDK_SAFE_DELETE( pAudioSample );
}

mfxStatus one_process::InitVideoDecoder( sInputParams *pParams )
{
    MSDK_CHECK_POINTER( pParams, MFX_ERR_NULL_PTR );
    if( NULL ==  m_pVideoDecoder )
    {
        m_pVideoDecoder = new CDecodingPipeline();
        m_pVideoDecoder->m_deviceid = m_index;
        MSDK_CHECK_POINTER( m_pVideoDecoder, MFX_ERR_MEMORY_ALLOC );
    }

    MSDK_ZERO_MEMORY( *pParams );
    InitVideoDecoderParam( pParams );
//    printf("init decoder\n");
    return m_pVideoDecoder->Init( pParams );
}


void one_process::InitVideoDecoderParam( sInputParams *pParams )
{
    if( NULL == pParams )
        return;
//    pParams->decode = g_decodeDevice[0];
//    pParams->transcode = g_pCaptureDeviceVec[0];
    pParams->decodeID = m_index;
    pParams->videoType = MFX_CODEC_MPEG2;
    pParams->bUseHWLib = false;
    if(pParams->nAsyncDepth == 0)
        pParams->nAsyncDepth = 4;
//    pParams->width = 720;
//    pParams->height = 576;
//    pParams->fourcc = MFX_FOURCC_NV12;

}

void one_process::PauseVideoDecoderThread()
{
    pthread_mutex_lock( &m_DeMutex );
    while ( m_bStopDecoder )
    {
        DestroyVideoDecoder();
        pthread_cond_wait( &m_DeCond, &m_DeMutex );
    }
    pthread_mutex_unlock( &m_DeMutex );
}

void one_process::DestroyVideoDecoder()
{
    if( m_pVideoDecoder )
    {
        delete m_pVideoDecoder;
        m_pVideoDecoder = NULL;
    }
}

mfxStatus one_process::InitVideoEncoder( sParams *pParams )
{
    MSDK_CHECK_POINTER( pParams, MFX_ERR_NULL_PTR );
    if( NULL ==  m_pVideoEncoder )
    {
        m_pVideoEncoder = new CEncodingPipeline();
        m_pVideoEncoder->m_deviceid = m_index;
        MSDK_CHECK_POINTER( m_pVideoEncoder, MFX_ERR_MEMORY_ALLOC );
    }

    MSDK_ZERO_MEMORY( *pParams );
    InitVideoEncoderParam( pParams );
    return m_pVideoEncoder->Init( pParams );
}

void one_process::InitEncoderPar()
{
    m_EncoderParInfo.bLookAhead = true;
    m_EncoderParInfo.bUseHWLib = false;
    m_EncoderParInfo.nAsyncDepth = 1;
    m_EncoderParInfo.nBitRate = 1000;
    m_EncoderParInfo.nChannelNum = 0;
    m_EncoderParInfo.nCodecLevel = 40;
    m_EncoderParInfo.nCodecProfile = 100;
    m_EncoderParInfo.nFrameRate = 25;
    m_EncoderParInfo.nGopOptFlag = MFX_GOP_STRICT;
    m_EncoderParInfo.nGopPicSize = 48;
    m_EncoderParInfo.nGopRefDist = 6;
//    m_EncoderParInfo.nLADepth = 49;
    m_EncoderParInfo.nNumSlice = 1;
    m_EncoderParInfo.nPicStruct = 0;
//    m_EncoderParInfo.nRateControlMethod = MFX_RATECONTROL_LA_ICQ;
    m_EncoderParInfo.nRateControlMethod = MFX_RATECONTROL_CQP;
    m_EncoderParInfo.nTargetUsage = 4;
    m_EncoderParInfo.strCodecId = QString("AVC");
    m_EncoderParInfo.strColorFormat = QString("");
}

void one_process::InitVideoEncoderParam( sParams *pParams )
{
    if( NULL == pParams )
        return;
    pParams->ndeviceid = m_index;
//    pParams->vd = g_pCaptureDeviceVec[0];
    pParams->nCodecLevel = m_EncoderParInfo.nCodecLevel;
    pParams->nCodecProfile = m_EncoderParInfo.nCodecProfile;
//    pParams->nPicStruct = MFX_PICSTRUCT_PROGRESSIVE;
    pParams->nPicStruct = MFX_PICSTRUCT_FIELD_TFF;
    pParams->nRateControlMethod = m_EncoderParInfo.nRateControlMethod;
    pParams->nGopRefDist = m_EncoderParInfo.nGopRefDist;
    pParams->nGopPicSize = m_EncoderParInfo.nGopPicSize;
    pParams->nNumSlice = m_EncoderParInfo.nNumSlice;
    pParams->nTargetUsage = m_EncoderParInfo.nTargetUsage;
    pParams->nGopOptFlag = m_EncoderParInfo.nGopOptFlag;
    pParams->nWidth = 720;
    pParams->nHeight = 576;
//    pParams->nICQQuality = 36;
    GetQuality( pParams->nDstWidth, pParams->nDstHeight);
//    pParams->nLADepth = m_EncoderParInfo.nLADepth;
    pParams->CodecId = MFX_CODEC_AVC;//GetCodecId();
    pParams->dFrameRate = m_EncoderParInfo.nFrameRate;
    pParams->bUseHWLib = true;//m_EncoderParInfo.bUseHWLib;
    pParams->nBitRate = m_EncoderParInfo.nBitRate;//m_EncoderParInfo.nBitRate;
    pParams->ColorFormat = MFX_FOURCC_YV12;//GetColorFormat();
    pParams->nAsyncDepth = m_EncoderParInfo.nAsyncDepth;
    pParams->MVC_flags = MVC_VIEWOUTPUT;
}

void one_process::GetQuality( unsigned short &nDstWidth, unsigned short &nDstHeight )
{

        nDstWidth = 720;
        nDstHeight = 576;

}

void one_process::PauseVideoEncoderThread()
{
    pthread_mutex_lock( &m_Mutex );
    while ( m_bStopEncoder )
    {
        DestroyVideoEncoder();
        pthread_cond_wait( &m_Cond, &m_Mutex );
    }
    pthread_mutex_unlock( &m_Mutex );
}

void one_process::DestroyVideoEncoder()
{
    if( m_pVideoEncoder )
    {
        delete m_pVideoEncoder;
        m_pVideoEncoder = NULL;
    }
}

long long one_process::GetBitRate()
{
    return m_pVideoEncoder->GetBitRate();
}

long long one_process::GetFrameNum()
{
    return m_pVideoEncoder->GetFrameNum();
}

void one_process::SetQP(int QP)
{
    m_pVideoEncoder->SetQP(QP);
}

void one_process::SetDelay(int Time)
{
    m_pVideoEncoder->SetDelay(Time);
}

bool one_process::InitRtmpClient1()
{
//    if( !m_CurChannelSettingInfo.bRtmpEnable1 )
//    {
//        SendLogToShow( RTMP1, true, QString("Rtmp 1 is disabled! ") );
//        return false;
//    }

    if( m_pRtmpClient1 )
        m_pRtmpClient1->Reset();
    else
    {
        m_pRtmpClient1 = new CRtmpClient();
        if( NULL == m_pRtmpClient1 )
        {
            printf("%s:%d  Error: m_pRtmpClient1 is NULL, Cannot to here!!!\n", __FILE__, __LINE__ );
            return false;
        }
    }

    if( m_pRtmpClient1->Initialize() < 0 )
    {
        SendLogToShow( RTMP1, true, QString("Rtmp1 initialize failed! ") );
        return false;
    }

//    if( m_CurChannelSettingInfo.strRtmpAddr1.isEmpty()  )
//    {
//        SendLogToShow( RTMP1, true, QString("Rtmp1 url is empty! ") );
//        return false;
//    }
    const char* addr = "1.8.23.98/live/stream90";
    m_pRtmpClient1->SetURL( addr );
//    m_pRtmpClient1->SetAudioDelay( 5000 );

    if( false == m_pRtmpClient1->Connect() )
    {
        printf("rtmp1 connect failed!\n");
        SendLogToShow( RTMP1, true, QString("Rtmp1 client connect falied! ") );
        return false;
    }

    m_bRtmpEnable1 = true;
    printf("rtmp1 running.....................!\n");
    SendLogToShow( RTMP1, false, QString( "Running " ) );
    return true;
}

bool one_process::InitRtmpClient2()
{
//    if( !m_CurChannelSettingInfo.bRtmpEnable2 )
//    {
//        SendLogToShow( RTMP2, true,  QString("Rtmp 2 is disabled!") );
//        return false;
//    }

    if( m_pRtmpClient2 )
        m_pRtmpClient2->Reset();
    else
    {
        m_pRtmpClient2 = new CRtmpClient();
        if( NULL == m_pRtmpClient2 )
        {
            printf("%s:%d  Error: m_pRtmpClient2 is NULL, Cannot to here!!!\n", __FILE__, __LINE__ );
            return false;
        }
    }

    if( m_pRtmpClient2->Initialize() < 0 )
    {
        SendLogToShow( RTMP2, true, QString("Rtmp2 initialize failed!") );
        return false;
    }

//    if( m_CurChannelSettingInfo.strRtmpAddr2.isEmpty()  )
//    {
//        SendLogToShow( RTMP2, true, QString("Rtmp2 url is empty!") );
//        return false;
//    }

    m_pRtmpClient2->SetURL( "1.8.23.98/live/stream91" );
//    m_pRtmpClient2->SetAudioDelay( 1500 );

    if( false == m_pRtmpClient2->Connect() )
    {
        SendLogToShow( RTMP2, true, QString("Rtmp2 client connect falied!") );
        return false;
    }
    m_bRtmpEnable2 = true;
    SendLogToShow( RTMP2, false, QString( "Running " ) );

    return true;
}

//bool one_process::InitUdpRtpClient()
//{
////    if( !m_CurChannelSettingInfo.bUdpOrRtpEnable )
////    {
////        SendLogToShow( UDP_RTP, true, QString("UDP or RTP is disabled!") );
////        return false;
////    }

//    if( m_pTSEncapsulator )
//        m_pTSEncapsulator->Reset();
//    else
//    {
//        m_pTSEncapsulator = new CTSEncapsulator();
//        if( NULL == m_pTSEncapsulator )
//            return false;
//    }

//    m_pTSEncapsulator->SetAudioDelay( m_CurChannelSettingInfo.nUdpOrRtpAudioDelay );
//    m_pTSEncapsulator->SetServerIP( m_CurChannelSettingInfo.strUdpOrRtpIp.toLatin1().data() );
//    m_pTSEncapsulator->SetServerPort( m_CurChannelSettingInfo.nUdpOrRtpPort );
//    m_pTSEncapsulator->SetOutType( m_CurChannelSettingInfo.bUdpOrRtpAddr ? 0:1 );
//    if( m_pTSEncapsulator->Initialize() < 0 )
//    {
//        SendLogToShow( UDP_RTP, true, QString("UDP or RTP initialize failed!") );
//        return false;
//    }

////    m_bUdpRtpEnable = m_CurChannelSettingInfo.bUdpOrRtpEnable;
//    SendLogToShow( UDP_RTP, false, QString( "Running " ) );
//    return true;
//}

void one_process::SendLogToShow( SEND_CLIENT_INDEX sendClientIndex, bool bError, const QString strLogMsg )
{
//    int nSendLogCount = m_vecSendLog.count();
//    if( nSendLogCount <= 0 || nSendLogCount <= sendClientIndex )
//        return;

//    LOG_INFO sendLog = m_vecSendLog[sendClientIndex];
//    if( (sendLog.bError == bError) && strLogMsg.contains(sendLog.strLogMsg) )
//        return;

//    sendLog.bError = bError;
//    sendLog.strLogMsg = strLogMsg;
//    m_vecSendLog.replace( sendClientIndex, sendLog );

    QString strSendLog = QString( "" );
    bool bSendError = false;
//    for( int i=0; i<nSendLogCount; i++ )
//    {
//        if( m_vecSendLog[i].bError )
//        {
//            bSendError = m_vecSendLog[i].bError;
//            strSendLog.append( m_vecSendLog[i].strLogMsg );
//        }
//        else
//            m_vecSendLog[i].strLogMsg.clear();
////    }
//    if( !bSendError )
//        strSendLog = QString( "Running" );

//    emit emitSendLog( m_index, strSendLog, bSendError );
}

//void one_process::Rtmp1SendError()
//{
//    CloseRtmp1();
//    SendLogToShow( RTMP1, true, QString( "Rtmp1 send failed! try to connect ..........." ) );
//    m_nRtmpDisconnectTime1 = QDateTime::currentDateTime().toTime_t();
//    // send 失败时，启动一个2sec timer 尝试connect,共尝试10mins
//    emit ConnectRtmpTimer1(); //pthread cannot start a timer, so use signal to start timer
//}

//void one_process::Rtmp2SendError()
//{
//    CloseRtmp2();
//    SendLogToShow( RTMP2, true, QString( "Rtmp2 send failed! try to connect ..........." ) );
//    m_nRtmpDisconnectTime2 = QDateTime::currentDateTime().toTime_t();
//    emit ConnectRtmpTimer2();
//}

/////////////////// slot ////////////////////////
void one_process::ConnectRtmp1()
{
    // Fixme: 10 min try connect
    if( QDateTime::currentDateTime().toTime_t() < (m_nRtmpDisconnectTime1 + 10*60) )
    {
        if( InitRtmpClient1() ) // 连接成功
        {
            SendLogToShow( RTMP1, false, QString( "Running " ) );
            m_nRtmpDisconnectTime1 = 0;
            return;
        }
        m_ConnectRtmpTimer1.start( CONNECT_TIMER );
    }
    else
        SendLogToShow( RTMP1, true, QString( "Rtmp1 network connect error! " ) );
}

void one_process::ResponseConnectRtmpTimer1()
{
    m_ConnectRtmpTimer1.start( CONNECT_TIMER );
}

void one_process::ConnectRtmp2()
{
    // Fixme: 10 min try connect
    if( QDateTime::currentDateTime().toTime_t() < (m_nRtmpDisconnectTime2 + 10*60) )
    {
        if( InitRtmpClient2() ) // 连接成功
        {
            SendLogToShow( RTMP2, false, QString("Running") );
            m_nRtmpDisconnectTime2 = 0;
            return;
        }
        m_ConnectRtmpTimer2.start( CONNECT_TIMER );
    }
    else
        SendLogToShow( RTMP2, true, QString( "Rtmp2 network connect error! " ) );
}

void one_process::ResponseConnectRtmpTimer2()
{
    m_ConnectRtmpTimer2.start( CONNECT_TIMER );
}

void one_process::Rtmp1SendError()
{
    CloseRtmp1();
    SendLogToShow( RTMP1, true, QString( "Rtmp1 send failed! try to connect ..........." ) );
    m_nRtmpDisconnectTime1 = QDateTime::currentDateTime().toTime_t();
    // send 失败时，启动一个2sec timer 尝试connect,共尝试10mins
    emit ConnectRtmpTimer1(); //pthread cannot start a timer, so use signal to start timer
}

void one_process::Rtmp2SendError()
{
    CloseRtmp2();
    SendLogToShow( RTMP2, true, QString( "Rtmp2 send failed! try to connect ..........." ) );
    m_nRtmpDisconnectTime2 = QDateTime::currentDateTime().toTime_t();
    emit ConnectRtmpTimer2();
}

void one_process::CloseRtmp1()
{
    if( m_pRtmpClient1 )
    {
        m_bRtmpEnable1 = false;
        m_pRtmpClient1->Close();
        m_pRtmpClient1->FreeRtmp();
    }
}

void one_process::CloseRtmp2()
{
    if( m_pRtmpClient2 )
    {
        m_bRtmpEnable2 = false;
        m_pRtmpClient2->Close();
        m_pRtmpClient2->FreeRtmp();
    }
}
