#include "mythread.h"
#include <QtCore>

MyThread::MyThread(QObject *parent) :
    QThread(parent)
{
}
void MyThread::run(){
   //rpc::client client("192.168.10.130",5530);
   rpc::client client("203.68.252.72",5540);
    while (true) {
        QMutex mutex;
        mutex.lock();
        if(this->stop)break;
        auto result_obj = client.async_call("send");
        auto result = result_obj.get().as<std::vector<uchar>>();

        auto sign_obj = client.async_call("getsign");
        auto sign = sign_obj.get().as<int>();

        mutex.unlock();
        emit FrameChange(result);
        emit SignChange(sign);

    }
    this->stop=true;
}
