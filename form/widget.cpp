#include "widget.h"
#include "ui_widget.h"
#include <opencv2/core.hpp>
#include <QMetaType>
#include <string>
Q_DECLARE_METATYPE(std::vector<uchar>);
Q_DECLARE_METATYPE(DBSensorData);
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{

    qRegisterMetaType<DBSensorData>("DBSensorData");
    qRegisterMetaType<DBSensorData>("DBSensorData&");
    qRegisterMetaType<std::vector<uchar>>("std::vector<uchar>");
    qRegisterMetaType<std::vector<uchar>>("std::vector<uchar>&");
    ui->setupUi(this);
    mThread = new MyThread(this);
    eThread = new EnThread(this);
    connect(ui->pushButton,SIGNAL(clicked()),SLOT(start()));
    connect(mThread,SIGNAL(FrameChange(std::vector<uchar>&)),SLOT(OnFrameChange(std::vector<uchar>&)));
    connect(mThread,SIGNAL(SignChange(int)),SLOT(OnSignChange(int)));
    connect(eThread,SIGNAL(EnDataChange(DBSensorData&)),SLOT(OnEnDataChange(DBSensorData&)));
    connect(ui->pushButton_2,SIGNAL(clicked()),SLOT(stop()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::OnFrameChange(std::vector<uchar> result)
{       img = cv::imdecode(cv::Mat(result),CV_LOAD_IMAGE_COLOR);
        cv::cvtColor(img,img,CV_BGR2RGB);
        ui->label->setPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888)));
        cv::waitKey(33);
}
void Widget::start(){

    mThread->start();
    mThread->stop = false;
}
void Widget::stop(){
    //ui->label->setText("close");
    mThread->stop = true;

}
void Widget::OnEnDataChange(DBSensorData data)
{
    ui->label_19->setText(QString::number(data.stdPM10_));
    ui->label_20->setText(QString::number(data.stdPM25_));
    ui->label_21->setText(QString::number(data.stdPM100_));
    ui->label_22->setText(QString::number(data.airPM10_));
    ui->label_23->setText(QString::number(data.airPM25_));
    ui->label_24->setText(QString::number(data.airPM100_));
    ui->label_25->setText(QString::number(data.um3_));
    ui->label_26->setText(QString::number(data.um5_));
    ui->label_27->setText(QString::number(data.um10_));
    ui->label_28->setText(QString::number(data.um25_));
    ui->label_29->setText(QString::number(data.um50_));
    ui->label_30->setText(QString::number(data.um100_));
    ui->label_31->setText(QString::number(data.formaldehyde_));
    ui->label_32->setText(QString::number(data.temperature_));
    ui->label_33->setText(QString::number(data.humidity_));
    ui->label_34->setText(QString::number(data.ppmplg_));
    ui->label_35->setText(QString::number(data.ppmco_));
    ui->label_36->setText(QString::number(data.ppmch4_));
}

void Widget::on_pushButton_3_clicked()
{
    eThread->start();
    eThread->stop = false;
}

void Widget::on_pushButton_4_clicked()
{
    eThread->stop = true;
}

void Widget::on_pushButton_5_clicked()
{
    //char msg[180];
   std::string message = ui->lineEdit->text().toStdString();
   //std::strcpy(msg, message.c_str());
   rpc::client client("203.68.252.72",5540);
   auto result_ = client.async_call("writeMessage",message);
   auto result = result_.get().as<std::string>();
   ui->label_39->setText(QString::fromStdString(result));
   //ui->pushButton_5->setText(ui->lineEdit->text());
}

void Widget::OnSignChange(int sign)
{
    if(sign ==1)
    {
        ui->label_40->setText("Stranger Waring!!");
    }
    else if(sign == 0)
    {
        ui->label_40->setText("");
    }

}
