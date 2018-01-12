#pragma once

#include <wiringPi.h>
#include <softPwm.h>
#include <signals.h>

// Define base-frequency of hardware PWM
#define HARDWARE_PWM_BASE_FREQ   9600000

// Define base-frequency of hardware PWM
#define SOFTWARE_PWM_BASE_FREQ   10000


namespace cm
{
namespace gpio
{

class Pin
{
public:
    Pin(int pin): _pin(pin), _value(0)
    {
        _valueChanged.emit(_pin, _value);
    }
    int pin()
    {
        return _pin;
    }
    int value()
    {
        return _value;
    }
    cm::utils::signal<void(int, int)>&  valueChanged()
    {
        return _valueChanged;
    }

protected:
    int _pin;
    int _value;
    cm::utils::signal<void(int, int)> _valueChanged;
};


class InputPin : public Pin
{
public:
    InputPin(int pin): Pin(pin)
    {
        pinMode (_pin, INPUT);
    }

    int in()
    {
        int newValue = digitalRead(_pin);
        if (newValue != _value)
        {
            _value = newValue;
            _valueChanged.emit(_pin, _value);
        }
        return _value;
    }

    void setPullUp()
    {
        pullUpDnControl (_pin, PUD_UP);
    }
    void setPullDown()
    {
        pullUpDnControl (_pin, PUD_DOWN);
    }
    void setNoPullUpDown()
    {
        pullUpDnControl (_pin, PUD_DOWN);
    }

};

class OutputPin : public Pin
{
public:
    OutputPin(int pin): Pin(pin)
    {
        pinMode (_pin, OUTPUT);
    }

    void out(int value)
    {
        if(value != _value)
        {
            _value = value;
            digitalWrite (_pin, _value) ;
            _valueChanged.emit(_pin, _value);
        }
    }
};


class PwmPin : public Pin
{
public:
    PwmPin(int pin, int baseFrequency, int frequency, double duty)
        : Pin(pin),_baseFrequency(baseFrequency), _frequency(frequency),_duty(duty)
    {
        changeFrequency();
    }

    virtual ~PwmPin()
    {
    }

    void setDuty(double duty)
    {
        _duty = duty;
        changeDuty();
        setValue(_value);
    }

    int frequency()
    {
        return _frequency;
    }

    int range()
    {
        return _range;
    }

    double duty()
    {
        return _duty;
    }

    void start()
    {
        setValue(_value);
    }

    void stop ()
    {
        setValue(0);
    }

protected:
    virtual void setValue(int value) = 0;

    void changeFrequency()
    {
        if (_frequency > _baseFrequency/2)
        {
            _frequency = _baseFrequency/2;
            _range = 2;
            _value = 1;
            _duty = 0.5;
        }
        else
        {
            _range = _baseFrequency / _frequency;
            _value = _range * _duty;
            _duty = double(_value) / _range;
        }
    }

    void changeDuty()
    {
        _value = _range * _duty;
        _duty = double(_value) / _range;
    }

    int _baseFrequency;
    int _frequency;
    double _duty;
    int _range;
};


class HardwarePwmPin : public PwmPin
{
public:
    HardwarePwmPin(int pin, int frequency, double duty)
        : PwmPin(pin,  HARDWARE_PWM_BASE_FREQ, frequency, duty)
    {
        pinMode(pin, PWM_OUTPUT);
        pwmSetMode (PWM_MODE_MS);
        pwmSetClock(2);
        pwmSetRange(_range);
        setValue(_value);
    }

protected:
    virtual void setValue(int value)
    {
        pwmWrite (_pin, value);
        _valueChanged.emit(_pin, value);
    }

};

class SoftwarePwmPin : public PwmPin
{
public:
    SoftwarePwmPin(int pin, int frequency, double duty)
        : PwmPin(pin, SOFTWARE_PWM_BASE_FREQ, frequency, duty)
    {
        softPwmCreate (_pin, _value , _range);
    }

    virtual void setValue(int value)
    {
        softPwmWrite (_pin,  value);
        _valueChanged.emit(_pin, value);
    }
};

} //namespace utils
} //namespace cm
