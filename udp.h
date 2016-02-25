#ifndef UDP_H
#define UDP_H

#include "udpsocket.h"
#include "one_process.h"
#include "decodepool.h"

class udp
{
public:
    udp();
    ~udp();
    int Init();

private:
    udpsocket* m_ChannelGet[16];

};



#endif // UDP_H
