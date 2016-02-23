#include "udp.h"

FILE *m_write;
udp::udp()
{

}

udp::~udp()
{

}

int udp::Init()
{
        m_write = fopen("real16.txt","wb+");

        for(int i=0; i<16; i++){
            m_ChannelGet[i] = new udpsocket();
            m_ChannelGet[i]->thread_init(i+1);  //udp接收线程
        }

        long long BitrateCount = 0;
        long long Bitrate[16] = {0};
        long long Number[16] = {0};
        long long NumberBefore[16] = {0};
        int qpi = 25;
        int qp[16] = {0};
        char str[100];
        for(int i=0;i<16;i++)
            qp[i] = 25;
        while(1){
            Number[0] = m_ChannelGet[0]->GetFrameNum();
            if(NumberBefore[0] != Number[0]){
                BitrateCount = 0;
                Bitrate[0] = m_ChannelGet[0]->GetBitRate()/(Number[0] - NumberBefore[0]);
                NumberBefore[0] = Number[0];
                BitrateCount += Bitrate[0];
                for(int i=1; i<16; i++){
                    Number[i] = m_ChannelGet[i]->GetFrameNum();
                    if(Number[i] != NumberBefore[i]){
                        Bitrate[i] = m_ChannelGet[i]->GetBitRate()/(Number[i] - NumberBefore[i]);
                        NumberBefore[i] = Number[i];
                    }
                    BitrateCount += Bitrate[i];
                }
                sprintf(str, "%lld", BitrateCount*25);
                fputs(str, m_write);
                fputc('\r', m_write);
                fputc('\n', m_write);
                qpi = 53 - (int)(((long double)log((long double)BitrateCount*25)-19.8991)/(-0.1213));
                printf("frame = %lld ,bitrate = %lld, count = %lld, qpi = %d\n",Number[0], Bitrate[0]*25, BitrateCount*25, qpi);
                if(qpi > 40)
                    qpi = 40;
                else if(qpi < 23)
                    qpi = 23;
                if(Number[0] > 100){
                    for(int i=0; i<16; i++){
//                        if(Bitrate[i]*25<500000){
//                            if(qp[i]>qpi)
//                                qp[i] = qpi;
//                        }
//                        else if(Bitrate[i]*25>1300000 && BitrateCount>15000000){
//                            qp[i] = qpi + 3;
//                            if(qp[i] > 40)
//                                qp[i] = 40;
//                        }
//                        else
                        qp[i] = qpi;
                        m_ChannelGet[i]->SetQP(qp[i]);
                    }
                }
            }
        }

        return 0;
}



