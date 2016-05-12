#include "udp.h"

FILE *m_write;
FILE *m_write1;
FILE *m_writeframe;

#define ChannelNum 1
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

        for(int i=0; i<ChannelNum; i++){
            m_ChannelGet[i] = new udpsocket();
            m_ChannelGet[i]->thread_init(i+1);  //udp接收线程
        }

//        int AverageNum = 12;
//        int AverageNum1 = 36;
//        long long BitrateCount = 0, RealBitrateCount = 0, RealBitrateCount1 = 0;
//        long long Bitrate[MaxChannel] = {0};
//        long long BitrateAv[MaxChannel] = {0};
//        long long BitrateAv1[MaxChannel] = {0};
//        long long RealBitrate[MaxChannel][12] = {0};
//        long long RealBitrate1[MaxChannel][36] = {0};
//        long long Number[MaxChannel] = {0};
//        long long NumberBefore[MaxChannel] = {0};
//        int qpi = 25;
//        int qpi1 = 25;
//        int qp[MaxChannel] = {0};
//        char str[100];
//        char str1[100];
//        char str2[100];
//        for(int i=0; i<ChannelNum; i++){
//            for(int j=0; j<AverageNum; j++)
//                RealBitrate[i][j] = 0;
//            for(int j=0; j<AverageNum1; j++)
//                RealBitrate1[i][j] = 0;
//            qp[i] = 25;
//        }
//        while(1){
//            Number[0] = m_ChannelGet[0]->GetFrameNum();
//            if(NumberBefore[0] != Number[0]){
//                BitrateCount = 0;
//                RealBitrateCount = 0;
//                RealBitrateCount1 = 0;
//                Bitrate[0] = m_ChannelGet[0]->GetBitRate()/(Number[0] - NumberBefore[0]);
//                BitrateCount += Bitrate[0];
//                for(int i=0; i<Number[0] - NumberBefore[0]; i++){
//                    //12average
//                    BitrateAv[0] += Bitrate[0]/AverageNum;
//                    BitrateAv[0] -= RealBitrate[0][(NumberBefore[0]+i+1)%AverageNum]/AverageNum;
//                    RealBitrate[0][(NumberBefore[0]+i+1)%AverageNum] = Bitrate[0];
//                    //36average
//                    BitrateAv1[0] += Bitrate[0]/AverageNum1;
//                    BitrateAv1[0] -= RealBitrate1[0][(NumberBefore[0]+i+1)%AverageNum1]/AverageNum1;
//                    RealBitrate1[0][(NumberBefore[0]+i+1)%AverageNum1] = Bitrate[0];
//                }
//                RealBitrateCount += BitrateAv[0];
//                RealBitrateCount1 += BitrateAv1[0];
//                NumberBefore[0] = Number[0];
//                for(int i=1; i<ChannelNum; i++){
//                    Number[i] = m_ChannelGet[i]->GetFrameNum();
////                    if((Number[i-1] - Number[i])%48<1)
////                        m_ChannelGet[i]->SetDelay( 1 - ( Number[i-1]-Number[i] )%48 );
//                    if(Number[i] != NumberBefore[i]){
//                        Bitrate[i] = m_ChannelGet[i]->GetBitRate()/(Number[i] - NumberBefore[i]);
//                        for(int j=0; j<Number[i] - NumberBefore[i]; j++){
//                            //12average
//                            BitrateAv[i] += Bitrate[i]/AverageNum;
//                            BitrateAv[i] -= RealBitrate[i][(NumberBefore[i]+j+1)%AverageNum]/AverageNum;
//                            RealBitrate[i][(NumberBefore[i]+j+1)%AverageNum] = Bitrate[i];

//                            //36average
//                            BitrateAv1[i] += Bitrate[i]/AverageNum1;
//                            BitrateAv1[i] -= RealBitrate1[i][(NumberBefore[i]+j+1)%AverageNum1]/AverageNum1;
//                            RealBitrate1[i][(NumberBefore[i]+j+1)%AverageNum1] = Bitrate[i];
//                        }
//                        NumberBefore[i] = Number[i];
//                    }
//                    printf("%d lu frame = %lld ,count = %lld\n",i,Number[i], Bitrate[i]);
//                }
//                sprintf(str, "%lld", RealBitrateCount*FrameRate);
//                fputs(str, m_write);
//                fputc('\r', m_write);
//                fputc('\n', m_write);
//                sprintf(str2, "%lld", Number[0]);
//                fputs(str2, m_writeframe);
//                fputc('\r', m_writeframe);
//                fputc('\n', m_writeframe);
//                qpi = 54 - (int)(((long double)log((long double)RealBitrateCount1*FrameRate)-19.8991)/(-0.1213));
//                qpi1 = 54 - (int)(((long double)log((long double)RealBitrateCount*FrameRate)-19.8991)/(-0.1213));
////                printf("frame = %lld , %lld , %lld ,%lld , %lld , %lld \n",Number[0], Number[1], Number[2], Number[5], Number[9],Number[12]);
////                printf("frame = %lld ,count = %lld, qpi = %d, qpi1 = %d\n",Number[0], RealBitrateCount*FrameRate, qpi , qpi1);
//                if(qpi1 > qpi)
//                    qpi = qpi1;
////                if(qpi > 24)
////                    qpi = qpi + 2;
//                if(qpi > 40)
//                    qpi = 40;
//                else if(qpi < 22)
//                    qpi = 22;
//                if(Number[0] > 100){
//                    for(int i=0; i<ChannelNum; i++){
//                        qp[i] = qpi;
//                        if(Bitrate[i]*FrameRate>1200000 && BitrateCount>15000000)
//                            qp[i] += 1;
////                        m_ChannelGet[i]->SetQP(qp[i]);
////                        m_ChannelGet[i]->SetQP(25);
//                    }
//                    sprintf(str1, "%d", qpi*100000);
//                    fputs(str1, m_write1);
//                    fputc('\r', m_write1);
//                    fputc('\n', m_write1);
//                }
//                else {
//                    sprintf(str1, "%d", 2500000);
//                    fputs(str1, m_write1);
//                    fputc('\r', m_write1);
//                    fputc('\n', m_write1);
//                }
//            }
//        }

        return 0;
}



