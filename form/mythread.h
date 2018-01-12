#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <rpc/client.h>

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = 0);
    void run();
    bool stop;
    std::vector<uchar> result;

signals:
    void FrameChange(std::vector<uchar>);
    void SignChange(int);
public slots:

};

#endif // MYTHREAD_H
