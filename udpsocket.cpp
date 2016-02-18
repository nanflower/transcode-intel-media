#include "udpsocket.h"

#include "global.h"

FILE *fp_write;
FILE *fp_v;
FILE *fp_v1;
FILE *fp_a;
FILE *fp_a1;

pthread_mutex_t locker;
uint8_t* q_buf;
int bufsize;
volatile int write_ptr;
volatile int read_ptr;

pthread_mutex_t locker1;
uint8_t* q_buf1;
volatile int write_ptr1;
volatile int read_ptr1;

pthread_mutex_t locker2;
uint8_t* q_buf2;
volatile int write_ptr2;
volatile int read_ptr2;

pthread_mutex_t locker3;
uint8_t* q_buf3;
volatile int write_ptr3;
volatile int read_ptr3;

pthread_mutex_t locker4;
uint8_t* q_buf4;
volatile int write_ptr4;
volatile int read_ptr4;

pthread_mutex_t locker5;
uint8_t* q_buf5;
volatile int write_ptr5;
volatile int read_ptr5;

pthread_mutex_t locker6;
uint8_t* q_buf6;
volatile int write_ptr6;
volatile int read_ptr6;

int write_buffer(void *opaque, uint8_t *buf, int buf_size){
    if(!feof(fp_write)){
        int true_size=fwrite(buf,buf_size,1,fp_write);
        return true_size;
    }else{
        return -1;
    }
}

int read_data(void *opaque, uint8_t *buf, int buf_size) {

    udpsocket* pTemp = (udpsocket*)opaque;
    int size = buf_size;
    bool ret;
    do {
        ret = pTemp->get_queue( buf, size);
    } while (ret);

    return size;
}

int read_data1(void *opaque, uint8_t *buf, int buf_size) {

    udpsocket* pTemp = (udpsocket*)opaque;
    int size = buf_size;
    bool ret;
    do {
        ret = pTemp->get_queue1( buf, size);
    } while (ret);

    return size;
}

int read_data2(void *opaque, uint8_t *buf, int buf_size) {

    udpsocket* pTemp = (udpsocket*)opaque;
    int size = buf_size;
    bool ret;
    do {
        ret = pTemp->get_queue2( buf, size);
    } while (ret);

    return size;
}

int read_data3(void *opaque, uint8_t *buf, int buf_size) {

    udpsocket* pTemp = (udpsocket*)opaque;
    int size = buf_size;
    bool ret;
    do {
        ret = pTemp->get_queue3( buf, size);
    } while (ret);

    return size;
}

int read_data4(void *opaque, uint8_t *buf, int buf_size) {

    udpsocket* pTemp = (udpsocket*)opaque;
    int size = buf_size;
    bool ret;
    do {
        ret = pTemp->get_queue4( buf, size);
    } while (ret);

    return size;
}

int read_data5(void *opaque, uint8_t *buf, int buf_size) {

    udpsocket* pTemp = (udpsocket*)opaque;
    int size = buf_size;
    bool ret;
    do {
        ret = pTemp->get_queue5( buf, size);
    } while (ret);

    return size;
}

int read_data6(void *opaque, uint8_t *buf, int buf_size) {

    udpsocket* pTemp = (udpsocket*)opaque;
    int size = buf_size;
    bool ret;
    do {
        ret = pTemp->get_queue6( buf, size);
    } while (ret);

    return size;
}

udpsocket::udpsocket()
{
    bitratebefore = 0;
    curbitrate = 0;

    pthread_mutex_init(&locker, NULL);
    q_buf = (uint8_t*)av_mallocz(sizeof(uint8_t)*1024*256);
    write_ptr = 0;
    read_ptr = 0;
    bufsize = 1024*256;

    pthread_mutex_init(&locker1, NULL);
    q_buf1 = (uint8_t*)av_mallocz(sizeof(uint8_t)*1024*256);
    write_ptr1 = 0;
    read_ptr1 = 0;

    pthread_mutex_init(&locker2, NULL);
    q_buf2 = (uint8_t*)av_mallocz(sizeof(uint8_t)*1024*256);
    write_ptr2 = 0;
    read_ptr2 = 0;

    pthread_mutex_init(&locker3, NULL);
    q_buf3 = (uint8_t*)av_mallocz(sizeof(uint8_t)*1024*256);
    write_ptr3 = 0;
    read_ptr3 = 0;

    pthread_mutex_init(&locker4, NULL);
    q_buf4 = (uint8_t*)av_mallocz(sizeof(uint8_t)*1024*256);
    write_ptr4 = 0;
    read_ptr4 = 0;

    pthread_mutex_init(&locker5, NULL);
    q_buf5 = (uint8_t*)av_mallocz(sizeof(uint8_t)*1024*256);
    write_ptr5 = 0;
    read_ptr5 = 0;

    pthread_mutex_init(&locker6, NULL);
    q_buf6 = (uint8_t*)av_mallocz(sizeof(uint8_t)*1024*256);
    write_ptr6 = 0;
    read_ptr6 = 0;

    fp_write=fopen("cuc60anniversary_start.h264","wb+"); //输出文件
}

udpsocket::~udpsocket()
{

}

void udpsocket::thread_init(int index)
{

    protindex = index;

    pthread_t udp_recv_thread;
    memset( &udp_recv_thread, 0, sizeof( udp_recv_thread ) );
    if( 0 != pthread_create( &udp_recv_thread, NULL, udp_tsrecv, this ) )
        printf("%s:%d  Error: Create udp receive thread failed !!!\n", __FILE__, __LINE__ );

    pthread_t ts_demux_thread;
    memset( &ts_demux_thread, 0, sizeof( ts_demux_thread ) );

    if( 0 != pthread_create( &ts_demux_thread, NULL, ts_demuxer, this ) )
        printf("%s:%d  Error: Create ts demux thread failed !!!\n", __FILE__, __LINE__ );

    pthread_detach(ts_demux_thread);
    pthread_detach(udp_recv_thread);

    m_transProcess = new one_process(protindex-1);
    m_transProcess->Init();
}

void *udpsocket::udp_tsrecv(void *pArg)
{
    udpsocket* pTemp = (udpsocket*)pArg;
    if( pTemp )
        pTemp->udp_ts_recv();
    return (void*)NULL;
}

void *udpsocket::ts_demuxer(void *pArg)
{
    udpsocket* pTemp = (udpsocket*) pArg;
    if( pTemp )
        pTemp->ts_demux(1);
    //    pTemp->thread_test();
    return (void*)NULL;
}

int udpsocket::ts_demux(int index)
{
    AVCodec *pAudioCodec[AUDIO_NUM];
    AVCodecContext *pAudioCodecCtx[AUDIO_NUM];
    AVIOContext * pb;
    AVInputFormat *piFmt;
    AVFormatContext *pFmt;
    uint8_t *buffer;
    int videoindex[VIDEO_NUM];
    int audioindex[AUDIO_NUM];
    AVStream *pAst[AUDIO_NUM];
    AVFrame *pVideoframe[VIDEO_NUM];
    AVFrame *pAudioframe[AUDIO_NUM];
    AVFrame *pOutAudioframe[AUDIO_NUM];


    AVPacket pkt;
    int frame_size;

    for( int i=0; i<VIDEO_NUM; i++ ){
        videoindex[i] = -1;
        pVideoframe[i] = NULL;
        pVideoframe[i] = av_frame_alloc();
    }
    for( int i=0; i<AUDIO_NUM; i++ ){
        pAudioCodec[i] = NULL;
        pAudioCodecCtx[i] = NULL;
        audioindex[i] = -1;
        pAst[i] = NULL;
        pOutAudioframe[i] = NULL;
        pOutAudioframe[i] = av_frame_alloc();
        pAudioframe[i] = NULL;
        pAudioframe[i] = av_frame_alloc();
    }
    pb = NULL;
    piFmt = NULL;
    pFmt = NULL;
    buffer = (uint8_t*)av_mallocz(sizeof(uint8_t)*BUFFER_SIZE);
    frame_size = AVCODEC_MAX_AUDIO_FRAME_SIZE*3/2;

    AVFormatContext *outAudioFormatCtx[AUDIO_NUM];
    AVPacket audio_pkt;
    AVStream *audio_stream[AUDIO_NUM];
    AVCodecContext *AudioEncodeCtx[AUDIO_NUM];
    AVCodec *AudioEncoder[AUDIO_NUM];

    fp_v = fopen("test0.mpg","wb+"); //输出文件
    fp_v1 = fopen("test1.mpg" , "wb+");
    fp_a = fopen("audio_out.aac","wb+");
    fp_a1 = fopen("audio1.aac","wb+");

    //FFMPEG
    av_register_all();
    if(protindex == 1)
        pb = avio_alloc_context(buffer, 4096, 0, NULL, read_data, NULL, NULL);
    else if(protindex == 2)
        pb = avio_alloc_context(buffer, 4096, 0, NULL, read_data1, NULL, NULL);
    else if(protindex == 3)
        pb = avio_alloc_context(buffer, 4096, 0, NULL, read_data2, NULL, NULL);
    else if(protindex == 4)
        pb = avio_alloc_context(buffer, 4096, 0, NULL, read_data3, NULL, NULL);
    else if(protindex == 5)
        pb = avio_alloc_context(buffer, 4096, 0, NULL, read_data4, NULL, NULL);
    else if(protindex == 6)
        pb = avio_alloc_context(buffer, 4096, 0, NULL, read_data5, NULL, NULL);
    else if(protindex == 7)
        pb = avio_alloc_context(buffer, 4096, 0, NULL, read_data6, NULL, NULL);
    printf("thread %d pid %lu tid %lu\n",index,(unsigned long)getpid(),(unsigned long)pthread_self());

    if (!pb) {
        fprintf(stderr, "avio alloc failed!\n");
        return -1;
    }
    int x = av_probe_input_buffer(pb, &piFmt, "", NULL, 0, 0);
    if (x < 0) {
        printf("probe error: %d",x);
    } else {
        fprintf(stdout, "probe success!\n");
        fprintf(stdout, "format: %s[%s]\n", piFmt->name, piFmt->long_name);
    }
    pFmt = avformat_alloc_context();
    pFmt->pb = pb;
    printf("demux work1\n");
    if (avformat_open_input(&pFmt, "", piFmt, NULL) < 0) {
        fprintf(stderr, "avformat open failed.\n");
        return -1;
    } else {
        fprintf(stdout, "open stream success!\n");
    }

    pFmt->probesize = 2048 * 200 ;
    pFmt->max_analyze_duration = 2048 * 200;
    if (avformat_find_stream_info(pFmt,0) < 0) {
        fprintf(stderr, "could not fine stream.\n");
        return -1;
    }
    printf("dump format\n");
    av_dump_format(pFmt, 0, "", 0);

    int videox = 0,audiox = 0;
    for (int i = 0; i < pFmt->nb_streams; i++) {
        if(videox == 1 && audiox == 1)
            break;
        if ( pFmt->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO && videox < 1 ) {
            videoindex[ videox++ ] = i;
        }
        if ( pFmt->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO && audiox < 1 ) {
            audioindex[ audiox++ ] = i;
        }
    }

    for(int i=0; i<VIDEO_NUM; i++)
        printf("videoindex %d = %d, audioindex %d = %d\n",i , videoindex[i], i ,audioindex[i]);


    for( int i=0; i<AUDIO_NUM; i++ ){
        pAst[i] = pFmt->streams[audioindex[i]];
        pAudioCodecCtx[i] = pAst[i]->codec;
        pAudioCodec[i] = avcodec_find_decoder(pAudioCodecCtx[i]->codec_id);
        if (!pAudioCodec[i]) {
            fprintf(stderr, "could not find audio decoder!\n");
            return -1;
        }
        if (avcodec_open2(pAudioCodecCtx[i], pAudioCodec[i], NULL) < 0) {
            fprintf(stderr, "could not open audio codec!\n");
            return -1;
        }
    }
    unsigned char* outbuffer = NULL;
    outbuffer = (unsigned char*)av_malloc(1024*1000);

    //audio encoder
    for( int i=0; i<AUDIO_NUM; i++){
        outAudioFormatCtx[i] = NULL;
        audio_stream[i] = NULL;
        AudioEncodeCtx[i] = NULL;
        AudioEncoder[i] = NULL;
    }
    const char* out_audio_file = "transcodeaudio.aac";          //Output URL

    //Method 1.
    for(int i=0; i<AUDIO_NUM; i++){
        outAudioFormatCtx[i] = avformat_alloc_context();
        outAudioFormatCtx[i]->oformat = av_guess_format(NULL, out_audio_file, NULL);
        AVIOContext *avio_audio_out = NULL;
        avio_audio_out = avio_alloc_context(outbuffer, 1024*1000, 0, NULL, NULL, write_buffer,NULL);
        if(avio_audio_out == NULL){
            printf("avio_out error\n");
            return -1;
        }
        outAudioFormatCtx[i]->pb = avio_audio_out;
        //Method 2.
        //avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, out_file);
        //fmt = pFormatCtx->oformat;

        //Open output URL
        if (avio_open(&outAudioFormatCtx[i]->pb,out_audio_file, AVIO_FLAG_READ_WRITE) < 0){
            printf("Failed to open output file!\n");
            return -1;
        }

        //Show some information
        av_dump_format(outAudioFormatCtx[i], 0, out_audio_file, 1);

        AudioEncoder[i] = avcodec_find_encoder(AV_CODEC_ID_AAC);
        if (!AudioEncoder[i]){
            printf("Can not find encoder!\n");
            return -1;
        }
        audio_stream[i] = avformat_new_stream(outAudioFormatCtx[i], AudioEncoder[i]);
        if (audio_stream[i]==NULL){
            return -1;
        }
        AudioEncodeCtx[i] = audio_stream[i]->codec;
        AudioEncodeCtx[i]->codec_id =  outAudioFormatCtx[i]->oformat->audio_codec;
        AudioEncodeCtx[i]->codec_type = AVMEDIA_TYPE_AUDIO;
        AudioEncodeCtx[i]->sample_fmt = AV_SAMPLE_FMT_S16;
        AudioEncodeCtx[i]->sample_rate= 48000;//44100
        AudioEncodeCtx[i]->channel_layout=AV_CH_LAYOUT_STEREO;
        AudioEncodeCtx[i]->channels = av_get_channel_layout_nb_channels(AudioEncodeCtx[i]->channel_layout);
        AudioEncodeCtx[i]->bit_rate = 64000;//64000
        /** Allow the use of the experimental AAC encoder */
        AudioEncodeCtx[i]->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;

        /** Set the sample rate for the container. */
        audio_stream[i]->time_base.den = pAudioCodecCtx[i]->sample_rate;
        audio_stream[i]->time_base.num = 1;

        if (avcodec_open2(AudioEncodeCtx[i], AudioEncoder[i],NULL) < 0){
            printf("Failed to open encoder!\n");
            return -1;
        }

        av_samples_get_buffer_size(NULL, AudioEncodeCtx[i]->channels,AudioEncodeCtx[0]->frame_size,AudioEncodeCtx[0]->sample_fmt, 1);
    }

    //uint8_t samples[AVCODEC_MAX_AUDIO_FRAME_SIZE*3/2];
    av_init_packet(&pkt);
    av_init_packet(&audio_pkt);
//    av_init_packet(&enc_pkt);
    AVAudioFifo *af[AUDIO_NUM];
    SwrContext *resample_context[AUDIO_NUM];
    long long pts = 0;
    /** Initialize the resampler to be able to convert audio sample formats. */
//    if (init_resampler(pAudioCodecCtx[0], AudioEncodeCtx[0],
//                       &resample_context))
    for(int i=0; i<1; i++){
        printf(" samplerate input = %d , samplerate output = %d\n",pAudioCodecCtx[i]->sample_rate, AudioEncodeCtx[i]->sample_rate);
        resample_context[i] = swr_alloc_set_opts(NULL, av_get_default_channel_layout(AudioEncodeCtx[i]->channels),
                                                          AudioEncodeCtx[i]->sample_fmt,
                                                          AudioEncodeCtx[i]->sample_rate,
                                                          av_get_default_channel_layout(pAudioCodecCtx[i]->channels),
                                                          pAudioCodecCtx[i]->sample_fmt,
                                                          pAudioCodecCtx[i]->sample_rate,
                                                          0, NULL);
        swr_init(resample_context[i]);
        af[i] = av_audio_fifo_alloc(AudioEncodeCtx[i]->sample_fmt, AudioEncodeCtx[i]->channels, 1);
        if(af[i] == NULL)
        {
            printf("error af \n");
            return -1;
        }
    }
    printf("swr over\n");


    while(1) {
        if (av_read_frame(pFmt, &pkt) >= 0) {
            for( int i=0; i<1; i++ ){
                if (pkt.stream_index == videoindex[i]) {
                    decode_Buffer[protindex-1]->putbuffer(&pkt, i);

                 }else if (pkt.stream_index == audioindex[i]) {
                    if (avcodec_decode_audio4(pAudioCodecCtx[i], pAudioframe[i], &frame_size, &pkt) >= 0) {
                        if (i == 0){

                            uint8_t *converted_input_samples = NULL;
                            converted_input_samples = (uint8_t *)calloc(AudioEncodeCtx[i]->channels, sizeof(*converted_input_samples));
                            av_samples_alloc(&converted_input_samples, NULL, AudioEncodeCtx[i]->channels, pAudioframe[i]->nb_samples, AudioEncodeCtx[i]->sample_fmt, 0);
                                        int error = 0;
                            if((error = swr_convert(resample_context[i], &converted_input_samples, pAudioframe[i]->nb_samples,
                                                   (const uint8_t**)pAudioframe[i]->extended_data, pAudioframe[i]->nb_samples))<0){
                                printf("error  : %d\n",error);
                            }
                            av_audio_fifo_write(af[i], (void **)&converted_input_samples, pAudioframe[i]->nb_samples);

                            int got_frame=0;
                            while(av_audio_fifo_size(af[i]) >= AudioEncodeCtx[i]->frame_size){
                                int frame_size = FFMIN(av_audio_fifo_size(af[i]),AudioEncodeCtx[i]->frame_size);
                                pOutAudioframe[i]->nb_samples =  frame_size;
                                pOutAudioframe[i]->channel_layout = AudioEncodeCtx[i]->channel_layout;
                                pOutAudioframe[i]->sample_rate = AudioEncodeCtx[i]->sample_rate;
                                pOutAudioframe[i]->format = AudioEncodeCtx[i]->sample_fmt;

                                av_frame_get_buffer(pOutAudioframe[i], 0);
                                av_audio_fifo_read(af[i], (void **)&pOutAudioframe[i]->data, frame_size);

                                pOutAudioframe[i]->pts=pts;
                                pts += pOutAudioframe[i]->nb_samples;

                                audio_pkt.data = NULL;
                                audio_pkt.size = 0;
                                av_init_packet(&audio_pkt);
                                avcodec_encode_audio2(AudioEncodeCtx[i], &audio_pkt, pOutAudioframe[i], &got_frame);
//                                printf("Encode %d Packet\tsize:%d\tpts:%lld\n", protindex, audio_pkt.size, audio_pkt.pts);
//                                if(protindex == 1)
//                                    fwrite(audio_pkt.data,audio_pkt.size, 1, fp_a);
//                                else if(protindex == 2)
//                                    fwrite(audio_pkt.data,audio_pkt.size, 1, fp_a1);
                            }
                        }
                        break;
                    }
                }
            }
            av_free_packet(&pkt);
        }
    }

    av_free(buffer);
    for(int i=0; i<VIDEO_NUM; i++)
        av_free(pVideoframe[i]);

    for(int i=0; i<AUDIO_NUM; i++)
        av_free(pAudioframe[i]);

    return 0;

}

void udpsocket::udp_ts_recv(void)
{
    /* 创建UDP套接口 */
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //server_addr.sin_addr.s_addr = inet_addr("1.8.84.12");
    int port = 50101 + protindex;
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
         uint8_t buffer[BUFFER_SIZE];
         bzero(buffer, BUFFER_SIZE);
         struct timeval tv;
         fd_set readfds;
         tv.tv_sec = 3;
         tv.tv_usec = 10;
         FD_ZERO(&readfds);
         FD_SET(server_socket_fd, &readfds);
         select(server_socket_fd+1,&readfds,NULL, NULL, &tv);
         if (FD_ISSET(server_socket_fd,&readfds))
         {
             int len = recvfrom(server_socket_fd, buffer, BUFFER_SIZE,0,(struct sockaddr*)&client_addr, &client_addr_length);
             if (len == -1)
             {
                 printf("received data error!\n");
             }
             if(protindex == 1)
                put_queue( buffer, len);
             else if(protindex == 2)
                 put_queue1( buffer , len);
             else if(protindex == 3)
                 put_queue2( buffer , len);
             else if(protindex == 4)
                 put_queue3( buffer , len);
             else if(protindex == 5)
                 put_queue4( buffer , len);
             else if(protindex == 6)
                 put_queue5( buffer , len);
             else if(protindex == 7)
                 put_queue6( buffer , len);
         }
         else
         {
             printf("error is %d\n",errno);
             printf("timeout!there is no data arrived!\n");
         }
     /* 从buffer中拷贝出file_name */
//     char file_name[FILE_NAME_MAX_SIZE+1];
//     bzero(file_name,FILE_NAME_MAX_SIZE+1);
//     strncpy(file_name, buffer, strlen(buffer)>FILE_NAME_MAX_SIZE?FILE_NAME_MAX_SIZE:strlen(buffer));
     //printf("%s\n", file_name);
    }
}


//int udpsocket::read_data(void *opaque, uint8_t *buf, int buf_size) {

//    udpsocket* pTemp = (udpsocket*)opaque;
//    int size = buf_size;
//    bool ret;
//    do {
//        ret = pTemp->get_queue( buf, size);
//    } while (ret);

//    return size;
//}

void udpsocket::put_queue(unsigned char* buf, int size) {
    pthread_mutex_lock(&locker);
    if ((write_ptr + size) > bufsize) {
        memcpy(q_buf + write_ptr, buf, (bufsize - write_ptr));
        memcpy(q_buf, buf+(bufsize - write_ptr), size-(bufsize - write_ptr));
    } else {
        memcpy(q_buf + write_ptr, buf, size*sizeof(uint8_t));
    }
    write_ptr = (write_ptr + size) % bufsize;
    pthread_mutex_unlock(&locker);
}

int udpsocket::get_queue(uint8_t* buf, int size) {
    pthread_mutex_lock(&locker);
    int wrap = 0;

    int pos = write_ptr;

    if (pos < read_ptr) {
        pos += bufsize;
        if(size + read_ptr > bufsize)
            wrap = 1;
    }

    if ( (read_ptr + size) > pos) {
        pthread_mutex_unlock(&locker);
        return 1;
    }

    if (wrap) {
        fprintf(stdout, "wrap...\n");
        memcpy(buf, q_buf + read_ptr, (bufsize - read_ptr));
        memcpy(buf+(bufsize - read_ptr), q_buf + 0, size-(bufsize - read_ptr));
    } else {
        memcpy(buf, q_buf + read_ptr, sizeof(uint8_t)*size);
    }
    read_ptr = (read_ptr + size) % bufsize;
    pthread_mutex_unlock(&locker);

    return 0;
}

void udpsocket::put_queue1(unsigned char* buf, int size) {
    pthread_mutex_lock(&locker1);
    if ((write_ptr1 + size) > bufsize) {
        memcpy(q_buf1 + write_ptr1, buf, (bufsize - write_ptr1));
        memcpy(q_buf1, buf+(bufsize - write_ptr1), size-(bufsize - write_ptr1));
    } else {
        memcpy(q_buf1 + write_ptr1, buf, size*sizeof(uint8_t));
    }
    write_ptr1 = (write_ptr1 + size) % bufsize;
    pthread_mutex_unlock(&locker1);
}

int udpsocket::get_queue1(uint8_t* buf, int size) {
    pthread_mutex_lock(&locker1);
    int wrap = 0;

    int pos = write_ptr1;

    if (pos < read_ptr1) {
        pos += bufsize;
        if(size + read_ptr1 > bufsize)
            wrap = 1;
    }

    if ( (read_ptr1 + size) > pos) {
        pthread_mutex_unlock(&locker1);
        return 1;
    }

    if (wrap) {
        fprintf(stdout, "wrap...\n");
        memcpy(buf, q_buf1 + read_ptr1, (bufsize - read_ptr1));
        memcpy(buf+(bufsize - read_ptr1), q_buf1 + 0, size-(bufsize - read_ptr1));
    } else {
        memcpy(buf, q_buf1 + read_ptr1, sizeof(uint8_t)*size);
    }
    read_ptr1 = (read_ptr1 + size) % bufsize;
    pthread_mutex_unlock(&locker1);

    return 0;
}

void udpsocket::put_queue2(unsigned char* buf, int size) {
    pthread_mutex_lock(&locker2);
    if ((write_ptr2 + size) > bufsize) {
        memcpy(q_buf2 + write_ptr2, buf, (bufsize - write_ptr2));
        memcpy(q_buf2, buf+(bufsize - write_ptr2), size-(bufsize - write_ptr2));
    } else {
        memcpy(q_buf2 + write_ptr2, buf, size*sizeof(uint8_t));
    }
    write_ptr2 = (write_ptr2 + size) % bufsize;
    pthread_mutex_unlock(&locker2);
}

int udpsocket::get_queue2(uint8_t* buf, int size) {
    pthread_mutex_lock(&locker2);
    int wrap = 0;

    int pos = write_ptr2;

    if (pos < read_ptr2) {
        pos += bufsize;
        if(size + read_ptr2 > bufsize)
            wrap = 1;
    }

    if ( (read_ptr2 + size) > pos) {
        pthread_mutex_unlock(&locker2);
        return 1;
    }

    if (wrap) {
        fprintf(stdout, "wrap...\n");
        memcpy(buf, q_buf2 + read_ptr2, (bufsize - read_ptr2));
        memcpy(buf+(bufsize - read_ptr2), q_buf2 + 0, size-(bufsize - read_ptr2));
    } else {
        memcpy(buf, q_buf2 + read_ptr2, sizeof(uint8_t)*size);
    }
    read_ptr2 = (read_ptr2 + size) % bufsize;
    pthread_mutex_unlock(&locker2);

    return 0;
}

void udpsocket::put_queue3(unsigned char* buf, int size) {
    pthread_mutex_lock(&locker3);
    if ((write_ptr3 + size) > bufsize) {
        memcpy(q_buf3 + write_ptr3, buf, (bufsize - write_ptr3));
        memcpy(q_buf3, buf+(bufsize - write_ptr3), size-(bufsize - write_ptr3));
    } else {
        memcpy(q_buf3 + write_ptr3, buf, size*sizeof(uint8_t));
    }
    write_ptr3 = (write_ptr3 + size) % bufsize;
    pthread_mutex_unlock(&locker3);
}

int udpsocket::get_queue3(uint8_t* buf, int size) {
    pthread_mutex_lock(&locker3);
    int wrap = 0;

    int pos = write_ptr3;

    if (pos < read_ptr3) {
        pos += bufsize;
        if(size + read_ptr3 > bufsize)
            wrap = 1;
    }

    if ( (read_ptr3 + size) > pos) {
        pthread_mutex_unlock(&locker3);
        return 1;
    }

    if (wrap) {
        fprintf(stdout, "wrap...\n");
        memcpy(buf, q_buf3 + read_ptr3, (bufsize - read_ptr3));
        memcpy(buf+(bufsize - read_ptr3), q_buf3 + 0, size-(bufsize - read_ptr3));
    } else {
        memcpy(buf, q_buf3 + read_ptr3, sizeof(uint8_t)*size);
    }
    read_ptr3 = (read_ptr3 + size) % bufsize;
    pthread_mutex_unlock(&locker3);

    return 0;
}

void udpsocket::put_queue4(unsigned char* buf, int size) {
    pthread_mutex_lock(&locker4);
    if ((write_ptr4 + size) > bufsize) {
        memcpy(q_buf4 + write_ptr4, buf, (bufsize - write_ptr4));
        memcpy(q_buf4, buf+(bufsize - write_ptr4), size-(bufsize - write_ptr4));
    } else {
        memcpy(q_buf4 + write_ptr4, buf, size*sizeof(uint8_t));
    }
    write_ptr4 = (write_ptr4 + size) % bufsize;
    pthread_mutex_unlock(&locker4);
}

int udpsocket::get_queue4(uint8_t* buf, int size) {
    pthread_mutex_lock(&locker4);
    int wrap = 0;

    int pos = write_ptr4;

    if (pos < read_ptr4) {
        pos += bufsize;
        if(size + read_ptr4 > bufsize)
            wrap = 1;
    }

    if ( (read_ptr4 + size) > pos) {
        pthread_mutex_unlock(&locker4);
        return 1;
    }

    if (wrap) {
        fprintf(stdout, "wrap...\n");
        memcpy(buf, q_buf4 + read_ptr4, (bufsize - read_ptr4));
        memcpy(buf+(bufsize - read_ptr4), q_buf4 + 0, size-(bufsize - read_ptr4));
    } else {
        memcpy(buf, q_buf4 + read_ptr4, sizeof(uint8_t)*size);
    }
    read_ptr4 = (read_ptr4 + size) % bufsize;
    pthread_mutex_unlock(&locker4);

    return 0;
}

void udpsocket::put_queue5(unsigned char* buf, int size) {
    pthread_mutex_lock(&locker5);
    if ((write_ptr5 + size) > bufsize) {
        memcpy(q_buf5 + write_ptr5, buf, (bufsize - write_ptr5));
        memcpy(q_buf5, buf+(bufsize - write_ptr5), size-(bufsize - write_ptr5));
    } else {
        memcpy(q_buf5 + write_ptr5, buf, size*sizeof(uint8_t));
    }
    write_ptr5 = (write_ptr5 + size) % bufsize;
    pthread_mutex_unlock(&locker5);
}

int udpsocket::get_queue5(uint8_t* buf, int size) {
    pthread_mutex_lock(&locker5);
    int wrap = 0;

    int pos = write_ptr5;

    if (pos < read_ptr5) {
        pos += bufsize;
        if(size + read_ptr5 > bufsize)
            wrap = 1;
    }

    if ( (read_ptr5 + size) > pos) {
        pthread_mutex_unlock(&locker5);
        return 1;
    }

    if (wrap) {
        fprintf(stdout, "wrap...\n");
        memcpy(buf, q_buf5 + read_ptr5, (bufsize - read_ptr5));
        memcpy(buf+(bufsize - read_ptr5), q_buf5 + 0, size-(bufsize - read_ptr5));
    } else {
        memcpy(buf, q_buf5 + read_ptr5, sizeof(uint8_t)*size);
    }
    read_ptr5 = (read_ptr5 + size) % bufsize;
    pthread_mutex_unlock(&locker5);

    return 0;
}

void udpsocket::put_queue6(unsigned char* buf, int size) {
    pthread_mutex_lock(&locker6);
    if ((write_ptr6 + size) > bufsize) {
        memcpy(q_buf6 + write_ptr6, buf, (bufsize - write_ptr6));
        memcpy(q_buf6, buf+(bufsize - write_ptr6), size-(bufsize - write_ptr6));
    } else {
        memcpy(q_buf6 + write_ptr6, buf, size*sizeof(uint8_t));
    }
    write_ptr6 = (write_ptr6 + size) % bufsize;
    pthread_mutex_unlock(&locker6);
}

int udpsocket::get_queue6(uint8_t* buf, int size) {
    pthread_mutex_lock(&locker6);
    int wrap = 0;

    int pos = write_ptr6;

    if (pos < read_ptr6) {
        pos += bufsize;
        if(size + read_ptr6 > bufsize)
            wrap = 1;
    }

    if ( (read_ptr6 + size) > pos) {
        pthread_mutex_unlock(&locker6);
        return 1;
    }

    if (wrap) {
        fprintf(stdout, "wrap...\n");
        memcpy(buf, q_buf6 + read_ptr6, (bufsize - read_ptr6));
        memcpy(buf+(bufsize - read_ptr6), q_buf6 + 0, size-(bufsize - read_ptr6));
    } else {
        memcpy(buf, q_buf6 + read_ptr6, sizeof(uint8_t)*size);
    }
    read_ptr6 = (read_ptr6 + size) % bufsize;
    pthread_mutex_unlock(&locker6);

    return 0;
}

long long udpsocket::GetBitRate()
{
    long long bitrate =  m_transProcess->GetBitRate() - bitratebefore;

    if(bitrate == 0){
        return curbitrate;
    }
    curbitrate = bitrate;
    bitratebefore = m_transProcess->GetBitRate();
    return bitrate;
}

long long udpsocket::GetFrameNum()
{
    return m_transProcess->GetFrameNum();
}

void udpsocket::SetQP(int QP)
{
    m_transProcess->SetQP(QP);
}
