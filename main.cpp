#include "mainwindow.h"
#include <QApplication>

#include "outudppool.h"
#include "decodepool.h"
#include "transcodepool.h"

outudppool *g_pLoopListBuffer[16] = {0};
decodepool *decode_Buffer[12] = {0};
transcodepool *transcode_Buffer[12] = {0};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
