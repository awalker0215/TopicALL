/*
opencv rpclib
*/


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <servo.h>
#include "opencv2/core.hpp"
#include "opencv2/face.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"
#include <opencv2/opencv.hpp>
#include <thread>
#include "transmitter.h"
#include "rpc/client.h"

#define RESOLUTION_WIDTH		640
#define RESOLUTION_HEIGHT		480
#define DETECTION_WIDTH			320
#define DETECTION_HEIGHT		240

using namespace cv;
using namespace cv::face;
using namespace std;
using namespace cm::gpio;

rpc::client client("0.0.0.0", 5540);

class FaceData
{
    public :
        FaceData():_name(),_facesrisize(){}
        vector<int> getname(){return _name;}
        vector<Rect> getfacerisize(){return _facesrisize;}
        Rect getfacevalue(int index){return _facesrisize[index];}
        int gettlx(int index){return _facesrisize[index].tl().x;}
        void setname(int name){_name.push_back(name);}
        void setallname(vector<int> allname){_name = allname;}
        void clearname() {_name.clear();}
        void setfaceresize(int facesize){_facesrisize.resize(facesize);}
        void setfacevalue(int index,Rect data){_facesrisize[index] = data;}

    private:
        vector<int> _name;
        vector<Rect> _facesrisize;

};

void Detect(transmitter<Mat> &stream, transmitter<vector<Rect>> &solutions, transmitter<FaceData> &faceData);
void Display(transmitter<Mat> &stream, transmitter<vector<Rect>> &solutions, transmitter<FaceData> &faceData);

void moveMotor(int midx, int midy);
int motorCount = 0;
float moveX=0, moveY=0;
float prevX=0, prevY=0;

String whois(int id)
{
    String name = "";
    switch (id)
    {
    case -1:
        name = "unknow";
        break;
    case 1:
        name = "umav";
        break;
    case 2:
        name = "minghan";
        break;
    case 3:
        name = "opama";
        break;
    case 4:
        name = "EJ";
        break;
    case 5:
        name = "DD";
        break;
    case 6:
        name = "nancy";
        break;
    case 7:
        name = "boxhorse";
        break;
    case 8:
        name = "andy";
        break;
    }
    return name;
}
void Detect(transmitter<Mat> &stream, transmitter<vector<Rect>> &solutions, transmitter<FaceData> &faceData)
{
    float frameX, frameY;
    float lenght=0;
    float tempx, tempy;
    float midx = 0, midy = 0;

    frameX = 320/2;
    frameY = 240/2;

    wiringPiSetup();
    SG90 servoX(1, HARD);
    SG90 servoY(23, HARD);
    moveX=0, moveY=0;
    servoX.turnToAngle(moveX);
    servoY.turnToAngle(moveY);

    bool running = true;
    FaceData oringinal_facedata;
    vector<Rect> last_faceresize,now_faceresize;
    vector<int> last_name,now_name;
    constexpr float scale_x = (float)RESOLUTION_WIDTH/(float)DETECTION_WIDTH;
    constexpr float scale_y = (float)RESOLUTION_HEIGHT/(float)DETECTION_HEIGHT;
    Mat frame, gray,gray_;
    CascadeClassifier face_cascade;

    string face_cascade_name = "/home/pi/Downloads/opencv/opencv-3.2.0/data/haarcascades/haarcascade_frontalface_alt2.xml";

    if (!face_cascade.load(face_cascade_name))
    {
        throw runtime_error("Unable to locate cascade classvoid facepoint(transmitter<float> &x,transmitter<float> &y);ifier " + face_cascade_name);
    }
    Ptr<FaceRecognizer> model = createLBPHFaceRecognizer(1, 8, 8, 8, 65);//createFisherFaceRecognizer(),65 is the reject limit;
    model->load("/home/pi/Documents/opencv_conn/rpclibopencv/facelist.yml");
    cout<<"load complete"<<endl;
    int prediction = 0;
    while (running)
    {
        int num = 0;
        unsigned int count = 0;
        vector<Rect> faces;
        running = stream.consume(frame);
        cvtColor(frame, gray, CV_BGR2GRAY);
        cvtColor(frame, gray_, CV_BGR2GRAY);
        resize(gray, gray, Size(DETECTION_WIDTH, DETECTION_HEIGHT));

        face_cascade.detectMultiScale(gray, faces, 1.1, 3, CV_HAAR_SCALE_IMAGE, Size(50, 50));
        oringinal_facedata.setfaceresize(faces.size());
            for (unsigned int i = 0; i< faces.size(); i++)
            {
                Rect face = faces[i];
                oringinal_facedata.setfacevalue(i,Rect(face.tl().x * scale_x, face.tl().y * scale_y, face.width * scale_x, face.height * scale_y));
                if(i==0)
                {
                    midx = frameX - (face.x+face.width*0.5);
                    midy = (face.y+face.height*0.5) - frameY;

                    lenght = sqrt(midx*midx+midy*midy);
                    cout << "lenght = " << lenght << endl;
                    if(lenght > 5)
                    {
                        tempx = prevX + midx/100;
                        tempy = prevY + midy/80;
                        if(tempx != prevX);
                        {
                            moveX = tempx;
                            servoX.turnToAngle(moveX);
                            if(moveX >= 90)
                                prevX = 90;
                            else if(moveX <= -90)
                                prevX = -90;
                            else
                                prevX = moveX;
                            cout << "moveX = " << moveX << endl;
                         }
                         if(tempy != prevY)
                         {
                             moveY = prevY + midy/100;
                             servoY.turnToAngle(moveY);
                             if(moveY >= 0)
                                 prevY = 0;
                             else if(moveY <= -90)
                                 prevY = -90;
                             else
                                 prevY = moveY;
                             cout << "moveY = " << moveY << endl;
                         }
                      }


                }
            }
            now_faceresize = oringinal_facedata.getfacerisize();//now is vector<Rect> face information

            last_name = oringinal_facedata.getname();
            if(now_faceresize.size()==last_faceresize.size())
            {
                count = 0;
                now_name.resize(last_name.size());
                for(unsigned int o_index = 0 ; o_index<now_faceresize.size();o_index++)
                {
                    for(unsigned int l_index = 0 ; l_index<last_faceresize.size();l_index++)
                    {
                        CvPoint p1,p2;
                        p1.x = std::max(now_faceresize[o_index].x,last_faceresize[l_index].x);
                        p1.y = std::max(now_faceresize[o_index].y,last_faceresize[l_index].y);
                        p2.x = std::min(now_faceresize[o_index].x+now_faceresize[o_index].width,last_faceresize[l_index].x+last_faceresize[l_index].width);
                        p2.y = std::min(now_faceresize[o_index].y+now_faceresize[o_index].height,last_faceresize[l_index].y+last_faceresize[l_index].height);
                        float join = 0;
                        if(p2.x>p1.x && p2.y>p1.y)
                        {
                            join = (p2.x-p1.x)*(p2.y-p1.y);
                        }
                        if(join/(last_faceresize[l_index].width*last_faceresize[l_index].height) > 0.55)
                        {

                            count ++;
                            now_name[o_index] = last_name[l_index];
                            break;
                        }
                    }
                }
            }
            if(count != faces.size())
            {
                oringinal_facedata.clearname();
                for (unsigned int i = 0; i < faces.size(); i++)
                {
                    Rect face_i = oringinal_facedata.getfacevalue(i);
                    Mat face = gray_(face_i);
                    Mat face_resized_;
                    if (faces.size()>0)
                    {
                    cv::resize(face, face_resized_, Size(200, 200), 1.0, 1.0, INTER_CUBIC);
                    prediction = model->predict(face_resized_);
                    if(prediction==-1)
                    {
                        vector<uchar> unknowcode;
                        Mat unknown_cut = frame(Range(face_i.y,(face_i.y+face_i.height)),Range(face_i.x,(face_i.x+face_i.width)));
                        //imwrite("/home/pi/un.bmp",unknown_cut);
                        imencode(".jpg",unknown_cut,unknowcode);
                        client.async_call("getunimg",unknowcode);
                    }
                    else
                    {
                        client.async_call("setsign");
                    }
                    oringinal_facedata.setname(prediction);
                    }

                }
            }
            else
            {
                oringinal_facedata.setallname(now_name);
            }
            last_faceresize = oringinal_facedata.getfacerisize();

        faceData.produce(oringinal_facedata);
    }
    faceData.stop();
}

void Display(transmitter<cv::Mat> &stream, transmitter<std::vector<cv::Rect>> &solutions, transmitter<FaceData> &faceData)
{
    Ptr<FaceRecognizer> model = createLBPHFaceRecognizer(1, 8, 8, 8, 65);
    model->load("/home/pi/Documents/opencv_conn/rpclibopencv/facelist.yml");
    FaceData oringinal_facedata_D;
    VideoCapture capture(0);

    bool running = true;

    if (!capture.isOpened())
    {
        throw std::runtime_error("Unable to open Raspberry Pi camera");
    }

    capture.set(CV_CAP_PROP_FRAME_WIDTH,RESOLUTION_WIDTH);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,RESOLUTION_HEIGHT);
    int numf = 0;
    Mat frame;

    vector<Rect> faces;
    vector<uchar> matcode;
    while(running)
    {
        capture >> frame;
        stream.produce(frame);
        faceData.consume(oringinal_facedata_D);
        faces = oringinal_facedata_D.getfacerisize();

        Mat gray;

        for (unsigned int i = 0 ; i<faces.size() ;i++)
        {
            numf++;
            vector<int> face_id = oringinal_facedata_D.getname();
            rectangle(frame, faces[i], Scalar::all(255), 1);
            string box_text = whois(face_id[i]);
            int pos_x = std::max(faces[i].tl().x - 10, 0);
            int pos_y = std::max(faces[i].tl().y - 10, 0);
            putText(frame, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0, 255, 0), 2.0);
        }
        imencode(".jpg",frame,matcode);
        client.async_call("show",matcode);
        frame.release();

    }
    faceData.stop();
    stream.stop();
}

int main(void)
{

    transmitter<FaceData> faceData;
    transmitter<Mat> stream;
    transmitter<vector<Rect>> solutions;

    stream.produce(Mat(RESOLUTION_HEIGHT, RESOLUTION_WIDTH, CV_8UC3, Scalar::all(0)));

    std::thread detect(bind(Detect, ref(stream), ref(solutions), ref(faceData)));
    std::thread display(bind(Display, ref(stream), ref(solutions), ref(faceData)));

    display.join();
    detect.join();

    return 0;
}


