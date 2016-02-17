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
            g_pLoopListBuffer[i] = new outudppool(5*188);
            g_pLoopListBuffer[i]->m_LoopIndex = i;//2*i+1;
        }
    }
    for(int i=0; i<2; i++){
        decode_Buffer[i] = new decodepool();
        decode_Buffer[i]->Init(i);
        transcode_Buffer[i] = new transcodepool();
        transcode_Buffer[i]->Init();
    }
//        else
//        {
//            g_pLoopListBuffer[i] = new CLoopListBuffer(200*188);
//            g_pLoopListBuffer[i]->m_LoopIndex = i;//(i-7)*2;
//        }
//    }


    udp udpsocketrev;
    udpsocketrev.Init();



}

MainWindow::~MainWindow()
{
    delete ui;
}
