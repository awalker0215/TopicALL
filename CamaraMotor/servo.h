#pragma once

#include <pin.h>

namespace cm
{
namespace gpio
{

enum PwmPinType
{
    SOFT,
    HARD
};

class Servo
{
public:

    Servo (int pin,  PwmPinType type, int frequency, double lowerDuty, double upperDuty, double lowerAngle, double upperAngle )
        :minDuty(lowerDuty),  maxDuty(upperDuty), minAngle(lowerAngle), maxAngle(upperAngle)
    {
        duty = centerDuty = (upperDuty+lowerDuty)/2 ;
        angle = centerAngle = (upperAngle+lowerAngle)/2;
        dutyAngleRatio =  (upperDuty-lowerDuty) / (upperAngle-lowerAngle);

        if (type == SOFT)
        {
            pwm = new SoftwarePwmPin (pin, frequency, centerDuty );
        }
        else
        {
            pwm = new HardwarePwmPin  (pin, frequency, centerDuty );
        }
    }

    ~Servo()
    {
        if (pwm != nullptr)  delete pwm;
    }

    void setMaxDuty(double duty)
    {
        maxDuty = duty;
        maxAngle = toAngle(duty);
    }

    void setMinDuty(double duty)
    {
        minDuty = duty;
        minAngle = toAngle(duty);
    }

    void setMaxAngle(double angle)
    {
        maxAngle = angle;
        maxDuty = toDuty(angle);
    }

    void setMinAngle(double angle)
    {
        minAngle = angle;
        minDuty = toDuty(angle);
    }

    void turnToPercent(double percent)
    {
        if (percent>100) percent = 100;
        if(percent < 0) percent = 0;
        duty =  (maxDuty-minDuty)  *  (percent  / 100) +  minDuty;
        angle = toAngle(duty);
        pwm->setDuty(duty);
    }

    void turnToAngle(double angle)
    {
        if (angle>maxAngle) angle = maxAngle;
        if (angle<minAngle) angle = minAngle;
        this->angle = angle;
        duty = toDuty(angle);
        pwm->setDuty(duty);
    }

    void moveAngle(double angle)
    {
        turnToAngle(this->angle+angle);
    }

    double getPercent()
    {
        return (duty-minDuty)/ (maxDuty-minDuty) *100;
    }

    double getAngle()
    {
        return angle;
    }

    double getDuty()
    {
        return duty;
    }


protected:
    double toDuty(double angle)
    {
        return (angle-centerAngle)  * dutyAngleRatio + centerDuty;
    }

    double toAngle(double duty)
    {
        return (duty -centerDuty) / dutyAngleRatio + centerAngle;
    }

    PwmPin  *pwm = nullptr;
    double dutyAngleRatio;
    double centerDuty;
    double centerAngle;
    double minDuty;
    double maxDuty;
    double minAngle;
    double maxAngle;
    double duty;
    double angle;
};

class SG90: public Servo
{
public:
    SG90 (int pin,  PwmPinType type) : Servo(pin, type, 50, 0.03, 0.12, -90, 90 )
    {
    }
};

class MG996R: public Servo
{
public:
    MG996R (int pin,  PwmPinType type) : Servo(pin, type, 50, 0.025, 0.125, -90, 90 )
    {
    }
};



} //namespace utils
} //namespace cm
