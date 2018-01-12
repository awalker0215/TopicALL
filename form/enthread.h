/*#ifndef ENTHREAD_H
#define ENTHREAD_H

#include <QThread>

struct DBSensorData
{
    int stdPM10_;
    int stdPM25_;
    int stdPM100_;
    int airPM10_;
    int airPM25_;
    int airPM100_;
    int um3_;                               //ms5003's data
    int um5_;
    int um10_;
    int um25_;
    int um50_;
    int um100_;
    int error_;
    float formaldehyde_;
    float temperature_;
    float humidity_;                         //ms5003's data
    float ppmplg_;
    float ppmco_;
    float ppmch4_;
    MSGPACK_DEFINE_ARRAY(stdPM10_, stdPM25_, stdPM100_, airPM10_, airPM25_, airPM100_, um3_, um5_, um10_, um25_, um100_, error_, formaldehyde_, temperature_, humidity_, ppmplg_, ppmco_, ppmch4_);
};

class EnThread : public QThread
{
    Q_OBJECT
public:
    explicit EnThread(QObject *parent = 0);
    void run();
    bool stop;
    DBSensorData sensorData;
signals:
    void EnDataChange(DBSensorData);
};

#endif // ENTHREAD_H
*/
#ifndef ENTHREAD_H
#define ENTHREAD_H

#include <QThread>
#include <string>
#include <rpc/client.h>

struct DBSensorData
{
    int stdPM10_;
    int stdPM25_;
    int stdPM100_;
    int airPM10_;
    int airPM25_;
    int airPM100_;
    int um3_;                               //ms5003's data
    int um5_;
    int um10_;
    int um25_;
    int um50_;
    int um100_;
    int error_;
    float formaldehyde_;
    float temperature_;
    float humidity_;                         //ms5003's data
    float ppmplg_;
    float ppmco_;
    float ppmch4_;
    std::string time_;
    MSGPACK_DEFINE_ARRAY(stdPM10_, stdPM25_, stdPM100_, airPM10_, airPM25_, airPM100_, um3_, um5_, um10_, um25_, um50_, um100_, error_, formaldehyde_, temperature_, humidity_, ppmplg_, ppmco_, ppmch4_, time_);
};


class EnThread : public QThread
{
    Q_OBJECT
public:
    explicit EnThread(QObject *parent = 0);
    void run();
    bool stop;
    DBSensorData sensorData;
signals:
    void EnDataChange(DBSensorData);
public slots:

};

#endif // MYTHREAD_H
