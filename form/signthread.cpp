#include "signthread.h"
#include <QtCore>

SignThread::SignThread(QObject *parent) :
    QThread(parent)
{

}

void SignThread::run()
{
    rpc::client client("203.68.252.72",5540);
     while (true) {
         QMutex mutex;
         mutex.lock();
         if(this->stop)break;
         auto sign_obj = client.async_call("getsign");
         auto sign = sign_obj.get().as<int>();
         mutex.unlock();
         emit SignChange(sign);

     }
     this->stop=true;
}
