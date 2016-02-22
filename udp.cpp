#include "udp.h"


udp::udp()
{

}

udp::~udp()
{

}

int udp::Init()
{

        for(int i=0; i<16; i++){
            m_ChannelGet[i] = new udpsocket();
            m_ChannelGet[i]->thread_init(i+1);  //udp接收线程
        }

//        long long bitnum[2] = {0,0};
//        long long Number[2] = {0,0};
//        while(true){
//            for(int i=1; i<2 ; i++){
//                Number[i] = m_ChannelGet[i-1]->GetFrameNum();
//                if(Number[i] != bitnum[i] ){
////                    printf("channel 7 bitrate = %lld ", m_ChannelGet[6]->GetBitRate());
////                    printf("channel 7 framenum = %d\n", m_ChannelGet[6]->GetFrameNum());
////                    printf("channel %d bitrate = %lld ", i, m_ChannelGet[i]->GetBitRate());
////                    printf("channel %d framenum = %d\n", i, m_ChannelGet[i]->GetFrameNum());
////                    printf("channel 0 bitrate = %lld ", m_ChannelGet[i-1]->GetBitRate());
////                    printf("channel 0 framenum = %d\n", Number[i]);
//                    bitnum[i] = Number[i];
//                    m_ChannelGet[i-1]->SetQP(25);
//                }
//            }
//        }

//        for(int i=0; i<1; i++)
//        {
//            m_VideoEncode[i] = new one_process(i);
//            m_VideoEncode[i]->Init();
//        }

        return 0;
}



