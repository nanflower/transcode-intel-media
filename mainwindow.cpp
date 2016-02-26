#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "global.h"

#include "udp.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    for(int i = 0; i < 32; i++)
    {
        if(i < 16)
        {
            //VIDEO 0-15
            send_Buffer[i] = new outudppool(1);
            send_Buffer[i]->m_deviceid = i;//2*i+1;
        }
        else
        {
            //AUDIO 16-31
            send_Buffer[i] = new outudppool(1);
            send_Buffer[i]->m_deviceid = i;
        }
    }
    for(int i=0; i<16; i++){
        decode_Buffer[i] = new decodepool();
        decode_Buffer[i]->Init();
        transcode_Buffer[i] = new transcodepool();
        transcode_Buffer[i]->Init();
    }

    udp udpsocketrev;
    udpsocketrev.Init();

}

MainWindow::~MainWindow()
{
    delete ui;
}
