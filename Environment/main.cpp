#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <math.h>
#include <time.h>

#include "rpc/server.h"
#include "rpc/client.h"
#include "rpc/detail/client_error.h"
#include "stop_watch.hpp"
#include "transmitter.hpp"
#include "wiringPi.h"
#include "wiringPiSPI.h"
#include "ms5003st.h"
#include "lcd.h"

using namespace cm::gpio;
using namespace std;
using std::string;

struct SensorData
{
    int stdPM10_;
    int stdPM25_;
    int stdPM100_;
    int airPM10_;
    int airPM25_;
    int airPM100_;
    int um3_;
    int um5_;
    int um10_;
    int um25_;
    int um50_;
    int um100_;
    int error_;
    float formaldehyde_;
    float temperature_;
    float humidity_;
    float ppmplg_;
    float ppmco_;
    float ppmch4_;
    MSGPACK_DEFINE_ARRAY(stdPM10_, stdPM25_, stdPM100_, airPM10_, airPM25_, airPM100_, um3_, um5_, um10_, um25_, um50_, um100_, error_, formaldehyde_,temperature_, humidity_, ppmplg_, ppmco_, ppmch4_); //val[3])
};

struct yourMessages
{
    string messages_ = "No message";
    MSGPACK_DEFINE_ARRAY(messages_);
};

void MS5003();                                                             //MS5003's finction
void show(MS5003ST* d);
void MQ9();                                                                //MQ9's function
void loadSpiDriver();
void spiSetup (int spiChannel);
int myAnalogRead(int spiChannel,int channelConfig,int analogChannel);
float MQReadDateChange(float value);
float MQResistanceCalculation(float Vo, float value);
float PPM_CH4(float ratio_);
float PPM_LPG(float ratio_);
float PPM_CO(float ratio_);

void conn();
void getMessage();
void showLCD();

static int myFd ;                                                           //mcp3008's
SensorData data;
char msgs[180] = "No message",pmsgs1[180], pmsgs2[180];
using namespace std;

rpc::client c("0.0.0.0", 5540);
int main()
{
    std::thread thread_connect(bind(conn));
    std::thread thread_message(bind(getMessage));
    std::thread thread_ms5003(bind(MS5003));
    std::thread thread_lcd(bind(showLCD));
    std::thread thread_mq9(bind(MQ9));

    thread_ms5003.join();
    thread_lcd.join();
    thread_mq9.join();
    thread_connect.join();
    thread_message.join();

    return 0;
}

void conn()
{
    while(1)
    {

        int status = 0;
        cout << "to sent!" << endl;
        c.async_call("sensor", data);
        cout << "sent!" << endl;
        cout << "status: " << status << endl;

        delay(5000);
    }
}

void getMessage()
{
    cout << "messageOPen " << endl;
    while(1)
    {
        memset(pmsgs1, 0, sizeof pmsgs1);
        memset(pmsgs2, 0, sizeof pmsgs2);
        cout << "FFFFFFFFFFFFFFFF" << endl;
        int j=0, k=0;
        auto result_obj = c.async_call("transmsg");
        auto result = result_obj.get().as<string>();

        strcpy(msgs, result.c_str());

        for (int i=0; msgs[i]; i++)
        {
            if(i<20)
                pmsgs1[j++] = msgs[i];
            else
                pmsgs2[k++] = msgs[i];
        }
        cout << "message" << msgs << endl;
        delay(5000);
    }
}

void MS5003()
{
    MS5003ST p(9600);
    p.valueChanged().connect(&show);
    p.startRead();
    char n;
    cin >> n;
    p.stopRead();
}

void show(MS5003ST* d)
{
    data.stdPM10_ = d->stdPM10();
    data.stdPM25_ = d->stdPM25();
    data.stdPM100_ = d->stdPM100();
    data.airPM10_ = d->airPM10();
    data.airPM25_ = d->airPM25();
    data.airPM100_ = d->airPM100();
    data.um3_ = d->um3();
    data.um5_ = d->um5();
    data.um10_ = d->um10();
    data.um25_ = d->um25();
    data.um50_ = d->um50();
    data.um100_ = d->um100();
    data.formaldehyde_ = d->formaldehyde();
    data.temperature_ = d->temperature();
    data.humidity_ = d->humidity();
    data.error_ = d->error();

    cout << data.airPM10_ << " " << data.stdPM25_ << " " << data.stdPM100_ << " " << data.airPM10_ <<
             " " << data.airPM25_ << " " << data.airPM100_ << " " << data.um3_ << " " <<
            data.um5_ << " " << data.um10_ << " " << data.um25_ << " " << data.um50_ << " " << data.um100_ << " " <<
            data.formaldehyde_ << " " << data.temperature_ << " " << data.humidity_ << data.error_ << endl;
}

void MQ9()
{
    bool loadSpi = false;
    int analogChannel = 0;
    int spiChannel = 0;
    int channelConfig = 8;
    float mcp3008Read, Ratio;
    float Vo = 0;

    if(loadSpi == true)
        loadSpiDriver();
    wiringPiSetup () ;
    spiSetup(spiChannel);

    // MQCalibration Start
    float rs = 0.0;
    cout << "Calibrating..." << endl;
    for(int i=0; i<5; i++) //(READ_SAMPLE_TIMES)
    {
        rs += MQReadDateChange(myAnalogRead(spiChannel,channelConfig,0));
        delay(1000);
    }
    Vo = rs/5;
    cout << "Calibration is done..." << endl;
    cout << "Vo = " << Vo << endl;

    while(1)
    {
        if(analogChannel>0)
        {
            printf("MCP3008(CE%d,%s): analogChannel %d = %d\n",spiChannel,(channelConfig==8)
                   ?"single-ended":"differential",analogChannel,myAnalogRead(spiChannel,channelConfig,analogChannel-1));
        }
        else
        {
            mcp3008Read = myAnalogRead(spiChannel,channelConfig,0);
        }

        Ratio = MQResistanceCalculation(Vo,MQReadDateChange(mcp3008Read));

        data.ppmplg_ = PPM_LPG(Ratio);
        data.ppmco_ = PPM_CO(Ratio);
        data.ppmch4_ = PPM_CH4(Ratio);

        cout << "LPG: " << data.ppmplg_ << endl;
        cout << "CO: " << data.ppmco_ << endl;
        cout << "CH4: " << data.ppmch4_ << endl;
        cout << "**************************" << endl;

        delay(5000);
    }

    close (myFd) ;

}

void showLCD()
{
    struct tm *timeinfo;
    time_t rawtime ;
    char strResponse [128];
    if (wiringPiSetup () == -1) exit (1);
    bool check = true;
    LCD lcd;
    string getMSG;
    char showmessages[200];

    bool light = true;

    if (light)
    {
            lcd.backLightOn();
    }
    else
    {
            lcd.backLightOff();
    }

    while(check)
    {
        lcd.ClrLcd();

        lcd.lcdLoc(LINE1);
        lcd.typeln("PM2.5: ");
        lcd.typeInt(data.airPM25_);

        lcd.lcdLoc(LINE2);
        lcd.typeln("UM5: ");
        lcd.typeInt(data.um5_);

        lcd.lcdLoc(LINE3);
        lcd.typeln("temperature: ");
        lcd.typeFloat(data.temperature_);
        lcd.typeln(" C");

        lcd.lcdLoc(LINE4);
        lcd.typeln("humidity: ");
        lcd.typeFloat(data.humidity_);

        delay(5000);

        lcd.ClrLcd();
        lcd.lcdLoc(LINE1);
        lcd.typeln("PLG: ");
        lcd.typeFloat(data.ppmplg_);
        lcd.typeln(" ppm");

        lcd.lcdLoc(LINE2);
        lcd.typeln("CO: ");
        lcd.typeFloat(data.ppmco_);
        lcd.typeln(" ppm");

        lcd.lcdLoc(LINE3);
        lcd.typeln("CH4: ");
        lcd.typeFloat(data.ppmch4_);
        lcd.typeln(" ppm");

        delay(5000);

        lcd.ClrLcd();
        lcd.lcdLoc(LINE1);
        lcd.typeln("Today is: ");

        lcd.lcdLoc(LINE2);

        rawtime = time(NULL);
        timeinfo = localtime(&rawtime);
        strftime(strResponse, 128, "%Y/%m/%d %H:%M:%S", timeinfo);

        cout << strResponse << endl;
        lcd.typeln(strResponse);

        lcd.lcdLoc(LINE3);
        lcd.typeln(pmsgs1);

        lcd.lcdLoc(LINE4);
        lcd.typeln(pmsgs2);


        delay(5000);
    }
}

void loadSpiDriver()
{
    if (system("gpio load spi") == -1)
    {
        fprintf (stderr, "Can't load the SPI driver: %s\n", strerror (errno)) ;
        exit (EXIT_FAILURE) ;
    }
}

void spiSetup (int spiChannel)
{
    if ((myFd = wiringPiSPISetup (spiChannel, 1000000)) < 0)
    {
        fprintf (stderr, "Can't open the SPI bus: %s\n", strerror (errno)) ;
        exit (EXIT_FAILURE) ;
    }
}

int myAnalogRead(int spiChannel,int channelConfig,int analogChannel)
{
    if(analogChannel<0 || analogChannel>7)
        return -1;
    unsigned char buffer[3] = {1}; // start bit
    buffer[1] = (channelConfig+analogChannel) << 4;
    wiringPiSPIDataRW(spiChannel, buffer, 3);
    return ( (buffer[1] & 3 ) << 8 ) + buffer[2]; // get last 10 bits
}

float MQReadDateChange(float value)     //Vrl
{
    return (value * 5 / 1024);
}

float MQResistanceCalculation(float Vo, float value) //ratio
{
    if(value < Vo)
        value = Vo;
    return float(Vo / (5-Vo)*((5-value)/value));
}

float PPM_CH4(float ratio_)
{
    float m = (log(10000)-log(200))/(log(0.063)-log(0.309));
    float data = (m*(log10(ratio_)-log10(0.309))) + log10(200);
    return pow(10,data);
}

float PPM_LPG(float ratio_)
{
    float m = (log(10000)-log(200))/(log(0.03485)-log(0.209));
    float data = m*(log10(ratio_)-log10(0.209))+log10(200);
    return pow(10,data);
}

float PPM_CO(float ratio_)
{
    float m = (log(1000)-log(200))/(log(0.0695)-log(0.162));
    float data = (m*(log10(ratio_)-log10(0.162)) + log10(200));
    return pow(10,data);
}
