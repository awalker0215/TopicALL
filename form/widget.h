#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <mythread.h>
#include <opencv2/core.hpp>
#include <vector>
#include <enthread.h>
#include <string>
#include <rpc/client.h>

/*struct writeMessage
{
    char sendMSG[180];
    std::string time_;
    MSGPACK_DEFINE_ARRAY(sendMSG, time_)
};*/

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    MyThread *mThread;
    EnThread *eThread;
    cv::Mat img;
private:
    Ui::Widget *ui;

public slots:
    void OnFrameChange(std::vector<uchar>);
    void OnEnDataChange(DBSensorData);
    void OnSignChange(int);
    void start();
    void stop();

private slots:
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
};

#endif // WIDGET_H
