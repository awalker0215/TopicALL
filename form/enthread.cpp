#include "enthread.h"
#include <QtCore>
EnThread::EnThread(QObject *parent) :
    QThread(parent)
{
}
void EnThread::run()
{
    rpc::client client("203.68.252.72",5540);
     while (true) {
         QMutex mutex;
         mutex.lock();
         if(this->stop)break;
         auto result_obj = client.async_call("sentSersorData");
         auto result = result_obj.get().as<DBSensorData>();
         mutex.unlock();
         emit EnDataChange(result);

     }
     this->stop=true;
}
