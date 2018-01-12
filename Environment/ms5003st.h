#pragma once

#include <iostream>
#include <chrono>
#include <uart.h>
#include <signals.h>


using namespace cm::utils;

namespace cm
{
namespace gpio
{

class MS5003ST : public UART
{
public:

    MS5003ST(int baud) : UART(baud)
    {
    }

    MS5003ST (char *device, int baud) : UART(device, baud)
    {
        _fd = serialOpen ( device, baud) ;
    }

    virtual void readData()
    {
        while(dataAvail()> 0)
        {
            buffer[counter++] =  getChar();
            if (buffer[0]!=0x42 && buffer[1]!=0x4d)
                counter = 0;

            if(counter>39)
            {
                _stdPM10 = buffer[4]*256+buffer[5];
                _stdPM25 = buffer[6]*256+buffer[7];
                _stdPM100 = buffer[8]*256+buffer[9];
                _airPM10 = buffer[10]*256+buffer[11];
                _airPM25 = buffer[12]*256+buffer[13];
                _airPM100 = buffer[14]*256+buffer[15];
                _um3 =  buffer[16]*256+buffer[17];
                _um5 =  buffer[18]*256+buffer[19];
                _um10 =  buffer[20]*256+buffer[21];
                _um25 =  buffer[22]*256+buffer[23];
                _um50 =  buffer[24]*256+buffer[25];
                _um100 =  buffer[26]*256+buffer[27];
                _formaldehyde = (float)(buffer[28]*256+buffer[29])/1000;
                _temperature = (float)(buffer[30]*256+buffer[31])/10;
                _humidity= (float)(buffer[32]*256+buffer[33])/10;
                _error = buffer[37];
                counter = 0;
                _valueChanged.emit(this);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    int  stdPM10 ()
    {
        return _stdPM10;
    }

    int stdPM25()
    {
        return _stdPM25;
    }

    int stdPM100()
    {
        return  _stdPM100;
    }

    int airPM10 ()
    {
        return _airPM10;
    }

    int airPM25 ()
    {
        return _airPM25;
    }

    int airPM100 ()
    {
        return _airPM100;
    }

    int um3 ()
    {
        return _um3;
    }

    int um5()
    {
        return _um5;
    }

    int um10()
    {
        return _um10;
    }

    int um25()
    {
        return _um25;
    }

    int um50 ()
    {
        return _um50;
    }

    int um100 ()
    {
        return _um100;
    }

    float formaldehyde()
    {
        return _formaldehyde;
    }

    float temperature()
    {
        return _temperature;
    }

    float humidity()
    {
        return _humidity;
    }

    int error()
    {
        return _error;
    }

    signal<void(MS5003ST *)> & valueChanged()
    {
        return _valueChanged;
    }

private:
    int buffer[40];
    int counter = 0;

    int  _stdPM10;
    int _stdPM25;
    int _stdPM100;
    int _airPM10;
    int _airPM25;
    int _airPM100;
    int _um3;
    int _um5;
    int _um10;
    int _um25;
    int _um50;
    int _um100;
    float _formaldehyde;
    float _temperature;
    float _humidity;
    int _error;

    signal<void(MS5003ST *)>  _valueChanged;
};

} //namespace utils
} //namespace cm
