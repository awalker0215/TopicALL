#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#pragma once

#include <mutex>
#include <condition_variable>

template <class T>
class transmitter
{
    public:

        transmitter() : _running(true), _new(true) {}

        void produce(const T &src)  //input the data
        {
            std::lock_guard<std::mutex> lock(this->_mutex);
            _loc = src;
            _new = true;
            _cv.notify_one();
        }
        void stop()
        {
            std::lock_guard<std::mutex> lock(this->_mutex);
            _running = false;
        }
        void wait()
        {
            std::unique_lock<std::mutex> lock(this->_mutex);
            _cv.wait(lock, std::bind(&transmitter<T>::new_data, this));
        }
        bool consume(T &dst)    //distory the data
        {
            std::lock_guard<std::mutex> lock(this->_mutex);
            dst = _loc;
            _new = false;
            return _running;
        }

    private:

        bool new_data()
        {
            return _new;
        }

        T _loc;
        std::mutex _mutex;
        std::condition_variable _cv;
        bool _running;
        bool _new;
};


#endif // TRANSMITTER_H
