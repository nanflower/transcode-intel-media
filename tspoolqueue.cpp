#include "tspoolqueue.h"

//uint8_t* q_buf;
//uint8_t* dst;
//uint8_t* src;
//int bufsize;
//volatile int write_ptr;
//volatile int read_ptr;

//FILE *fpv1;

//tspoolqueue::tspoolqueue()
//{
//    pthread_mutex_init(&locker, NULL);
//    q_buf = (uint8_t*)av_mallocz(sizeof(uint8_t)*1024*1024*2);
//    write_ptr = 0;
//    read_ptr = 0;
//    bufsize = 1024*1024;
//    dst = q_buf;
//    src = q_buf;

//    fpv1 = fopen("channel 1 video 1.h264","wb");
//}


//tspoolqueue::~tspoolqueue()
//{
//    pthread_mutex_destroy(&locker);
//    av_free(q_buf);
//}


//void tspoolqueue::free_queue(void) {
//    pthread_mutex_destroy(&locker);
//    av_free(q_buf);
//}


//void tspoolqueue::put_queue(unsigned char* buf, int size) {
//    printf("putqueue begin\n");
//    pthread_mutex_lock(&locker);
//    dst = q_buf + write_ptr;
//    if ((write_ptr + size) > bufsize) {
//        memcpy(dst, buf, (bufsize - write_ptr));
//        memcpy(q_buf, buf+(bufsize - write_ptr), size-(bufsize - write_ptr));
//    } else {
//        memcpy(dst, buf, size*sizeof(uint8_t));
//    }
//    write_ptr = (write_ptr + size) % bufsize;
//    pthread_mutex_unlock(&locker);
//        printf("putqueue over\n");
//}

//int tspoolqueue::get_queue(uint8_t* buf, int size) {
//    printf("getqueue begin\n");
//    pthread_mutex_lock(&locker);
//    src = q_buf + read_ptr;
//    int wrap = 0;

//    int pos = write_ptr;

//    if (pos < read_ptr) {
//        pos += bufsize;
//        if(size + read_ptr > bufsize)
//            wrap = 1;
//    }

//    if ( (read_ptr + size) > pos) {
//        pthread_mutex_unlock(&locker);
//        return 1;
//    }

//    if (wrap) {
//        fprintf(stdout, "wrap...\n");
//        memcpy(buf, src, (bufsize - read_ptr));
//        src = q_buf + 0;
//        memcpy(buf+(bufsize - read_ptr), src, size-(bufsize - read_ptr));
//    } else {
//        memcpy(buf, src, sizeof(uint8_t)*size);
//    }
//    read_ptr = (read_ptr + size) % bufsize;
//    pthread_mutex_unlock(&locker);
//    printf("getqueue over\n");

//    return 0;
//}

//void tspoolqueue::write_buffer(uint8_t* buf, int size) {
//    fwrite(buf, size, 1, fpv1);
//}
