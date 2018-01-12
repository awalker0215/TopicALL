#include <iostream>
#include "rpc/server.h"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <thread>
#include <transmitter.h>
#include <QtCore/QCoreApplication>
#include <Qtsql/QtSql>
#include <QtSql/QSqlDatabase>
#include <Qtsql/QSqlQuery>
#include <QDebug>
#include <time.h>

using namespace cv;
using namespace std;

struct SensorData
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
    MSGPACK_DEFINE_ARRAY(stdPM10_, stdPM25_, stdPM100_, airPM10_, airPM25_, airPM100_, um3_, um5_, um10_, um25_, um50_, um100_, error_, formaldehyde_, temperature_, humidity_, ppmplg_, ppmco_, ppmch4_);
};

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
    string time_;
    MSGPACK_DEFINE_ARRAY(stdPM10_, stdPM25_, stdPM100_, airPM10_, airPM25_, airPM100_, um3_, um5_, um10_, um25_, um50_, um100_, error_, formaldehyde_, temperature_, humidity_, ppmplg_, ppmco_, ppmch4_, time_);
};

/*struct tm{
    int tm_sec;  //代表目前秒数, 正常范围为0-59, 但允许至61 秒
    int tm_min;  //代表目前分数, 范围0-59
    int tm_hour;  //从午夜算起的时数, 范围为0-23
    int tm_mday;  //目前月份的日数, 范围01-31
    int tm_mon;  //代表目前月份, 从一月算起, 范围从0-11
    int tm_year;  //从1900 年算起至今的年数
    int tm_wday;  //一星期的日数, 从星期一算起, 范围为0-6
    int tm_yday;  //从今年1 月1 日算起至今的天数, 范围为0-365
    int tm_isdst;  //日光节约时间的旗标
    MSGPACK_DEFINE_ARRAY(tm_sec,tm_min,tm_hour,tm_mday,tm_mon,tm_year,tm_wday,tm_yday,tm_isdst);
};*/

struct writeMessage {
    char sendMSG_[180];
    std::string time_;
    MSGPACK_DEFINE_ARRAY(sendMSG_, time_)
};

struct transMessage {
    char sendMSG_[180];
    MSGPACK_DEFINE_ARRAY(sendMSG_)
};

rpc::server srv(5540);

void GetCode(transmitter<vector<uchar>> &code);
void SendCode(transmitter<vector<uchar>> &code);
void getSersorData(QSqlQuery &query);
void sentSersorData(QSqlQuery &query, DBSensorData &dbdata_);
void writeMSG(QSqlQuery &query);
void transMSG(QSqlQuery &query);

void UnKnow(transmitter<int> &unksign);
void NounK(transmitter<int> &unksign);
void SendSign(transmitter<int> &unksign);

void UnKnow(transmitter<int> &unksign)
{
    srv.bind("getunimg",[&unksign](vector<uchar> matcode)
    {
        time_t timep;
        tm *p;
        int i = 1;
        time(&timep);
        p = gmtime(&timep);
        string pass = "D:\\unknownface\\";
        string time = pass+std::to_string(1900+p->tm_year)+std::to_string(1+p->tm_mon)+std::to_string(p->tm_mday)+std::to_string(p->tm_hour)+std::to_string(p->tm_min)+std::to_string(p->tm_sec)+".jpg";
        cv::Mat unknow = cv::imdecode(cv::Mat(matcode),CV_LOAD_IMAGE_COLOR);

        cv::imwrite(time,unknow);
        unksign.produce(i);
    });
    unksign.stop();
}

void NounK(transmitter<int> &unksign)
{
    srv.bind("setsign",[&unksign]()
    {
        int i = 0;
        unksign.produce(i);
    });
    unksign.stop();
}

void SendSign(transmitter<int> &unksign)
{
    srv.bind("getsign",[&unksign]()
    {
        int usign;
        unksign.consume(usign);

        return usign;
    });
    unksign.stop();
}

int main()
{
    DBSensorData dbdata_;
    transmitter<vector<uchar>> code;
    transmitter<string> msg;
    transmitter<int> unksign;

    QSqlDatabase::drivers();
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("db_app");
    db.setUserName("root");
    db.setPassword("");

    QSqlQuery query;

    std::thread saveToTran(bind(GetCode, ref(code)));
    std::thread sendToClient(bind(SendCode, ref(code)));
    std::thread thread_server(bind(getSersorData, ref(query)));
    std::thread thread_sentToShow(bind(sentSersorData, ref(query), ref(dbdata_)));
    std::thread thread_inputMessage(bind(writeMSG, ref(query)));
    std::thread thread_sendMessage(bind(transMSG, ref(query)));
    std::thread getUnknow(bind(UnKnow, ref(unksign)));
    std::thread setNoKnown(bind(NounK, ref(unksign)));
    std::thread sendSign(bind(SendSign, ref(unksign)));

    cout << "Thread start!" << endl;

    getUnknow.join();
    setNoKnown.join();
    sendSign.join();
    saveToTran.join();
    sendToClient.join();
    thread_server.join();
    thread_sentToShow.join();
    thread_inputMessage.join();
    thread_sendMessage.join();
    if(db.open()){             //測試連接
        cout << "OK" <<endl;   //成功
    }
    else{
        cout << "NO" <<endl;
    }
    srv.run();
    return 0;
}

void GetCode(transmitter<vector<uchar>> &code)
{
    srv.bind("show",[&code](vector<uchar> matcode)
    {
        code.produce(matcode);
    });
    code.stop();
}

void SendCode(transmitter<vector<uchar>> &code)
{
    srv.bind("send",[&code]()
    {
       // cout<<"send..."<<endl;
        vector<uchar> sendcode;
        code.consume(sendcode);
        return sendcode;
    });

}

void getSersorData(QSqlQuery &query)
{
     cout << "getSersorData start!" << endl;

     srv.bind("sensor", [&query](SensorData &data_){
         struct tm *timeinfo;
         time_t rawtime;
         char strResponse[128];
         rawtime = time(NULL);
         timeinfo = localtime(&rawtime);
         strftime(strResponse, 128, "%Y:%m:%d: %I:%M:%S", timeinfo);

         //query.exec("INSERT INTO environment(stdPM10, stdPM25, stdPM100, airPM10, airPM25, airPM100, um3, um5, um10, um25, um50, um100, formaldehyde, temperature, humidity, ppmplg, ppmCO, ppmCH4, mId, time) VALUES (data_.stdPM10_, data_.stdPM25_, data_.stdPM100_, data_.airPM10_, data_.airPM25_, data_.airPM100_, data_.um3_, data_.um5_, data_.um10_, data_.um25_, data_.um50_, data_.um100_, data_.formaldehyde_, data_.temperature_, data_.humidity_, data_.ppmplg_, data_.ppmco_,ppmch4_, time, 6)");
         query.prepare("INSERT INTO environment(stdPM10, stdPM25, stdPM100, airPM10, airPM25, airPM100, um3, um5, um10, um25, um50, um100, formaldehyde, temperature, humidity, ppmplg, ppmCO, ppmCH4, mId, time) "
                           "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
         query.addBindValue(data_.stdPM10_);
         query.addBindValue(data_.stdPM25_);
         query.addBindValue(data_.stdPM100_);
         query.addBindValue(data_.airPM10_);
         query.addBindValue(data_.airPM25_);
         query.addBindValue(data_.airPM100_);
         query.addBindValue(data_.um3_);
         query.addBindValue(data_.um5_);                //um25
         query.addBindValue(data_.um10_);
         query.addBindValue(data_.um25_);
         query.addBindValue(data_.um50_);
         query.addBindValue(data_.um100_);
         query.addBindValue(data_.formaldehyde_);
         query.addBindValue(data_.temperature_);
         query.addBindValue(data_.humidity_);
         query.addBindValue(data_.ppmplg_);
         query.addBindValue(data_.ppmco_);
         query.addBindValue(data_.ppmch4_);
         query.addBindValue(6);
         query.addBindValue(strResponse);

         query.exec();
     });
 }

void sentSersorData(QSqlQuery &query, DBSensorData &dbdata_)
 {
     cout << "sentSersorData start" << endl;
     srv.bind("sentSersorData", [&query, &dbdata_]() {
         cout << "sent" << endl;

         cout << "test1" << endl;
         query.exec("SELECT * FROM environment order by eId desc limit 1");
         if(query.first()){
             cout << "test2" << endl;
             dbdata_.stdPM10_ = query.value(1).toInt();
             dbdata_.stdPM25_ = query.value(2).toInt();
             dbdata_.stdPM100_ = query.value(3).toInt();
             dbdata_.airPM10_ = query.value(4).toInt();
             dbdata_.airPM25_ = query.value(5).toInt();
             dbdata_.airPM100_ = query.value(6).toInt();
             dbdata_.um3_ = query.value(7).toInt();
             dbdata_.um5_ = query.value(8).toInt();
             dbdata_.um10_ = query.value(9).toInt();
             dbdata_.um25_ = query.value(10).toInt();
             dbdata_.um50_ = query.value(11).toInt();
             dbdata_.um100_ = query.value(12).toInt();
             dbdata_.formaldehyde_ = query.value(13).toFloat();
             dbdata_.temperature_ = query.value(14).toFloat();
             dbdata_.humidity_ = query.value(15).toFloat();
             dbdata_.ppmplg_ = query.value(16).toFloat();
             dbdata_.ppmco_ = query.value(17).toFloat();
             dbdata_.ppmch4_ = query.value(18).toFloat();
             dbdata_.time_ = query.value(19).toString().toStdString();      //.toUtf8()

             cout << "dbdata: " << dbdata_.stdPM10_ << endl;

            return dbdata_;
         }
     });
 }

void writeMSG(QSqlQuery &query)
{
    srv.bind("writeMessage", [&query](string &tempmsg)   //進db
    {
        //string tmpStr;

        struct tm *timeinfo;
        time_t rawtime;
        char strResponse[128];
        rawtime = time(NULL);
        timeinfo = localtime(&rawtime);
        strftime(strResponse, 128, "%Y:%m:%d: %I:%M:%S", timeinfo);

        query.prepare("INSERT INTO messageboard(message, mId, time) VALUES (?, ?, ?)");
        query.addBindValue(QString::fromStdString(tempmsg));
        query.addBindValue(6);
        query.addBindValue(strResponse);
        query.exec();

        cout << "get message = " << tempmsg;
        return "SUCCESSFUL TO SENT MESSAGE!";
    });

}

void transMSG(QSqlQuery &query)
{
    srv.bind("transmsg", [&query]() {
        string message;
        query.exec("SELECT * FROM messageboard order by MBid desc limit 1");
        if(query.first()){
            message = query.value(1).toString().toStdString();
            cout << "message = " << message << endl;
            return message;
        }
    });
}
