#include "udp.h"

FILE *m_write;
FILE *m_write1;
FILE *m_writeframe;

FILE *m_bit1;
FILE *m_qp1;
FILE *m_frame1;
FILE *m_bit2;
FILE *m_qp2;
FILE *m_frame2;
FILE *m_bit3;
FILE *m_qp3;
FILE *m_frame3;
FILE *m_bit4;
FILE *m_qp4;
FILE *m_frame4;
FILE *m_bit5;
FILE *m_qp5;
FILE *m_frame5;
FILE *m_bit6;
FILE *m_qp6;
FILE *m_frame6;
FILE *m_bit7;
FILE *m_qp7;
FILE *m_frame7;
#define ChannelNum 16
#define MaxChannel 16
#define FrameRate 25

udp::udp()
{

}

udp::~udp()
{

}

int udp::Init()
{
        m_write = fopen("real16.txt", "wb+");
        m_write1 = fopen("qp.txt", "wb+");
        m_writeframe = fopen("frame.txt", "wb+");

        m_bit1 = fopen("m-real1.txt", "wb+");
        m_qp1 = fopen("m-qp1.txt","wb+");
        m_frame1 = fopen("m-frame1.txt","wb+");
        m_bit2 = fopen("m-real2.txt", "wb+");
        m_qp2 = fopen("m-qp2.txt","wb+");
        m_frame2 = fopen("m-frame2.txt","wb+");
        m_bit3 = fopen("m-real3.txt", "wb+");
        m_qp3 = fopen("m-qp3.txt","wb+");
        m_frame3 = fopen("m-frame3.txt","wb+");
        m_bit4 = fopen("m-real4.txt", "wb+");
        m_qp4 = fopen("m-qp4.txt","wb+");
        m_frame4 = fopen("m-frame4.txt","wb+");
        m_bit5 = fopen("m-real5.txt", "wb+");
        m_qp5 = fopen("m-qp5.txt","wb+");
        m_frame5 = fopen("m-frame5.txt","wb+");
        m_bit6 = fopen("m-real6.txt", "wb+");
        m_qp6 = fopen("m-qp6.txt","wb+");
        m_frame6 = fopen("m-frame6.txt","wb+");
        m_bit7 = fopen("m-real7.txt", "wb+");
        m_qp7 = fopen("m-qp7.txt","wb+");
        m_frame7 = fopen("m-frame7.txt","wb+");

        for(int i=0; i<ChannelNum; i++){
            m_ChannelGet[i] = new udpsocket();
            m_ChannelGet[i]->thread_init(i+1);  //udp接收线程
        }

        int AverageNum = 12;
        int AverageNum1 = 36;
        long long BitrateCount = 0, RealBitrateCount = 0, RealBitrateCount1 = 0;
        long long Bitrate[MaxChannel] = {0};
        long long BitrateAv[MaxChannel] = {0};
        long long BitrateAv1[MaxChannel] = {0};
        long long RealBitrate[MaxChannel][12] = {0};
        long long RealBitrate1[MaxChannel][36] = {0};
        long long Number[MaxChannel] = {0};
        long long NumberBefore[MaxChannel] = {0};
        int qpi = 25;
        int qpi1 = 25;
        int qp[MaxChannel] = {0};
        char str[100];
        char str1[100];
        char str2[100];
        for(int i=0; i<ChannelNum; i++){
            for(int j=0; j<AverageNum; j++)
                RealBitrate[i][j] = 0;
            for(int j=0; j<AverageNum1; j++)
                RealBitrate1[i][j] = 0;
            qp[i] = 25;
        }
        while(1){
            Number[0] = m_ChannelGet[0]->GetFrameNum();
            if(NumberBefore[0] != Number[0]){
                BitrateCount = 0;
                RealBitrateCount = 0;
                RealBitrateCount1 = 0;
                Bitrate[0] = m_ChannelGet[0]->GetBitRate()/(Number[0] - NumberBefore[0]);
                BitrateCount += Bitrate[0];
                for(int i=0; i<Number[0] - NumberBefore[0]; i++){
                    //12average
                    BitrateAv[0] += Bitrate[0]/AverageNum;
                    BitrateAv[0] -= RealBitrate[0][(NumberBefore[0]+i+1)%AverageNum]/AverageNum;
                    RealBitrate[0][(NumberBefore[0]+i+1)%AverageNum] = Bitrate[0];
                    //36average
                    BitrateAv1[0] += Bitrate[0]/AverageNum1;
                    BitrateAv1[0] -= RealBitrate1[0][(NumberBefore[0]+i+1)%AverageNum1]/AverageNum1;
                    RealBitrate1[0][(NumberBefore[0]+i+1)%AverageNum1] = Bitrate[0];
                }
                sprintf(str, "%lld", BitrateAv[0]*FrameRate);
                fputs(str, m_bit1);
                fputc('\r', m_bit1);
                fputc('\n', m_bit1);
                sprintf(str, "%d", qp[0]*100000);
                fputs(str, m_qp1);
                fputc('\r', m_qp1);
                fputc('\n', m_qp1);
                sprintf(str, "%lld", Number[0]);
                fputs(str, m_frame1);
                fputc('\r', m_frame1);
                fputc('\n', m_frame1);
                RealBitrateCount += BitrateAv[0];
                RealBitrateCount1 += BitrateAv1[0];
                NumberBefore[0] = Number[0];
                for(int i=1; i<ChannelNum; i++){
                    Number[i] = m_ChannelGet[i]->GetFrameNum();
                    if((Number[i-1] - Number[i])%48<1)
                        m_ChannelGet[i]->SetDelay( 1 - ( Number[i-1]-Number[i] )%48 );
                    if(Number[i] != NumberBefore[i]){
                        Bitrate[i] = m_ChannelGet[i]->GetBitRate()/(Number[i] - NumberBefore[i]);
                        for(int j=0; j<Number[i] - NumberBefore[i]; j++){
                            //12average
                            BitrateAv[i] += Bitrate[i]/AverageNum;
                            BitrateAv[i] -= RealBitrate[i][(NumberBefore[i]+j+1)%AverageNum]/AverageNum;
                            RealBitrate[i][(NumberBefore[i]+j+1)%AverageNum] = Bitrate[i];

                            //36average
                            BitrateAv1[i] += Bitrate[i]/AverageNum1;
                            BitrateAv1[i] -= RealBitrate1[i][(NumberBefore[i]+j+1)%AverageNum1]/AverageNum1;
                            RealBitrate1[i][(NumberBefore[i]+j+1)%AverageNum1] = Bitrate[i];
                        }
                        NumberBefore[i] = Number[i];
                    }
                    if(i == 2){
                        sprintf(str, "%lld", BitrateAv[i]*FrameRate);
                        fputs(str, m_bit2);
                        fputc('\r', m_bit2);
                        fputc('\n', m_bit2);
                        sprintf(str, "%d", qp[i]*100000);
                        fputs(str, m_qp2);
                        fputc('\r', m_qp2);
                        fputc('\n', m_qp2);
                        sprintf(str, "%lld", Number[i]);
                        fputs(str, m_frame2);
                        fputc('\r', m_frame2);
                        fputc('\n', m_frame2);
                    }
                    else if(i == 3){
                        sprintf(str, "%lld", BitrateAv[i]*FrameRate);
                        fputs(str, m_bit3);
                        fputc('\r', m_bit3);
                        fputc('\n', m_bit3);
                        sprintf(str, "%d", qp[i]*100000);
                        fputs(str, m_qp3);
                        fputc('\r', m_qp3);
                        fputc('\n', m_qp3);
                        sprintf(str, "%lld", Number[i]);
                        fputs(str, m_frame3);
                        fputc('\r', m_frame3);
                        fputc('\n', m_frame3);
                    }
                    else if(i == 4){
                        sprintf(str, "%lld", BitrateAv[i]*FrameRate);
                        fputs(str, m_bit4);
                        fputc('\r', m_bit4);
                        fputc('\n', m_bit4);
                        sprintf(str, "%d", qp[i]*100000);
                        fputs(str, m_qp4);
                        fputc('\r', m_qp4);
                        fputc('\n', m_qp4);
                        sprintf(str, "%lld", Number[i]);
                        fputs(str, m_frame4);
                        fputc('\r', m_frame4);
                        fputc('\n', m_frame4);
                    }
                    else if(i == 5){
                        sprintf(str, "%lld", BitrateAv[i]*FrameRate);
                        fputs(str, m_bit5);
                        fputc('\r', m_bit5);
                        fputc('\n', m_bit5);
                        sprintf(str, "%d", qp[i]*100000);
                        fputs(str, m_qp5);
                        fputc('\r', m_qp5);
                        fputc('\n', m_qp5);
                        sprintf(str, "%lld", Number[i]);
                        fputs(str, m_frame5);
                        fputc('\r', m_frame5);
                        fputc('\n', m_frame5);
                    }
                    else if(i == 6){
                        sprintf(str, "%lld", BitrateAv[i]*FrameRate);
                        fputs(str, m_bit6);
                        fputc('\r', m_bit6);
                        fputc('\n', m_bit6);
                        sprintf(str, "%d", qp[i]*100000);
                        fputs(str, m_qp6);
                        fputc('\r', m_qp6);
                        fputc('\n', m_qp6);
                        sprintf(str, "%lld", Number[i]);
                        fputs(str, m_frame6);
                        fputc('\r', m_frame6);
                        fputc('\n', m_frame6);
                    }
                    else if(i == 7){
                        sprintf(str, "%lld", BitrateAv[i]*FrameRate);
                        fputs(str, m_bit7);
                        fputc('\r', m_bit7);
                        fputc('\n', m_bit7);
                        sprintf(str, "%d", qp[i]*100000);
                        fputs(str, m_qp7);
                        fputc('\r', m_qp7);
                        fputc('\n', m_qp7);
                        sprintf(str, "%lld", Number[i]);
                        fputs(str, m_frame7);
                        fputc('\r', m_frame7);
                        fputc('\n', m_frame7);
                    }
                    RealBitrateCount += BitrateAv[i];
                    RealBitrateCount1 += BitrateAv1[i];
                    BitrateCount += Bitrate[i];
                }
                sprintf(str, "%lld", RealBitrateCount*FrameRate);
                fputs(str, m_write);
                fputc('\r', m_write);
                fputc('\n', m_write);
                sprintf(str2, "%lld", Number[0]);
                fputs(str2, m_writeframe);
                fputc('\r', m_writeframe);
                fputc('\n', m_writeframe);
                qpi = 54 - (int)(((long double)log((long double)RealBitrateCount1*FrameRate)-19.8991)/(-0.1213));
                qpi1 = 54 - (int)(((long double)log((long double)RealBitrateCount*FrameRate)-19.8991)/(-0.1213));
//                printf("frame = %lld , %lld , %lld ,%lld , %lld , %lld \n",Number[0], Number[1], Number[2], Number[5], Number[9],Number[12]);
                printf("frame = %lld ,count = %lld, qpi = %d, qpi1 = %d\n",Number[0], RealBitrateCount*FrameRate, qpi , qpi1);
                if(qpi1 > qpi)
                    qpi = qpi1;
//                if(qpi > 24)
//                    qpi = qpi + 2;
                if(qpi > 40)
                    qpi = 40;
                else if(qpi < 22)
                    qpi = 22;
                if(Number[0] > 100){
                    for(int i=0; i<ChannelNum; i++){
                        qp[i] = qpi;
                        if(Bitrate[i]*FrameRate>1200000 && BitrateCount>15000000)
                            qp[i] += 1;
                        m_ChannelGet[i]->SetQP(qp[i]);
//                        m_ChannelGet[i]->SetQP(25);
                    }
                    sprintf(str1, "%d", qpi*100000);
                    fputs(str1, m_write1);
                    fputc('\r', m_write1);
                    fputc('\n', m_write1);
                }
                else {
                    sprintf(str1, "%d", 2500000);
                    fputs(str1, m_write1);
                    fputc('\r', m_write1);
                    fputc('\n', m_write1);
                }
            }
        }

        return 0;
}



