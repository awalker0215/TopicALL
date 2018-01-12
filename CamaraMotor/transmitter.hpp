#pragma once

#include <mutex>
#include <condition_variable>

namespace cm
{
namespace utils
{

template <class T>
class transmitter
{
    public:

        transmitter() : _running(true), _new(true) {}

        void produce(const T &src)
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
        bool consume(T &dst)
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

} //namespace utils
} //namespace cm

