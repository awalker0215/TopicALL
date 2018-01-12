#pragma once

#include <wiringSerial.h>
#include <monitor.h>
#include <thread>

namespace cm
{
namespace gpio
{

class UART;
void read(UART *device);

class UART
{
public:

    UART(int baud)
    {
        _fd = serialOpen ( defaultDevice, baud) ;
    }

    UART (char *device, int baud)
    {
        _fd = serialOpen ( device, baud) ;
    }

    ~UART()
    {
        if (_fd>=0)
            serialClose (_fd);
        }

    void startRead()
    {
        _run( [](Run&  run)
        {
            run.running = true;
        });

        _t = std::move( std::thread (read, this));
    }

    void stopRead()
    {
        _run( [](Run&  run)
        {
            run.running = false;
        });
        _t.join();
    }

    bool running()
    {
        return  _run( [] (Run& run) ->bool
        {
            return run.running;
        } );
    }

    void  putChar (unsigned char c)
    {
        serialPutchar (_fd, c) ;
    }

    void  putString ( char *s)
    {
        serialPuts (_fd, s) ;
    }

    void flush ()
    {
        serialFlush (_fd) ;
    }

    int dataAvail()
    {
        return serialDataAvail(_fd);
    }

    int getChar()
    {
        return serialGetchar(_fd);
    }

    virtual void readData() = 0;

protected:
    const char *defaultDevice = "/dev/ttyAMA0";
    int  _fd;
    std::thread _t;

    struct Run
    {
        bool running = false;
    };

    cm::utils::Monitor<Run> _run;
};

void read(UART *device)
{
    while(device->running())
    {
        device->readData();
    }
}

} //namespace utils
} //namespace cm

