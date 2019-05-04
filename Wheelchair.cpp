#include <Arduino.h>
#include "wheelchair.h"

void Wheelchair::initController()
{
    Wire.begin();
    Wire.beginTransmission(A_ChenalAddr);
    Wire.write(0x40);     //set PCF8591 output mode
    Wire.write(STOP);      //Wheelchair boot voltage
    Wire.endTransmission();

    Wire.beginTransmission(B_ChenalAddr);
    Wire.write(0x40);     //set PCF8591 output mode
    Wire.write(STOP);      //Wheelchair boot voltage
    Wire.endTransmission();
}

Wheelchair::Wheelchair()
{
    A_ChenalAddr = 0x48;
    B_ChenalAddr = 0x49;
    manualFlag   = false;
    currentStatu = Wheelchair::STATUS::READY;
    initController();
}

Wheelchair::Wheelchair(int A_ChenalAddr,int B_ChenalAddr)
{
    this->A_ChenalAddr = A_ChenalAddr;
    this->B_ChenalAddr = B_ChenalAddr;
    manualFlag         = false;
    currentStatu       = Wheelchair::STATUS::READY;
    initController();
}

void Wheelchair::setManual(bool manualFlag)
{
    this->manualFlag = manualFlag;
    if(manualFlag)
        setCurrent(Wheelchair::STATUS::MANUAL);
    else
    {
        brake();
        setCurrent(Wheelchair::STATUS::READY);
    }
}

bool Wheelchair::isManual()
{
    return manualFlag;
}

int Wheelchair::getA_ChenalAddr()
{
    return A_ChenalAddr;
}

int Wheelchair::getB_ChenalAddr()
{
    return B_ChenalAddr;
}

int Wheelchair::getCurrent()
{
    return currentStatu;
}

int Wheelchair::setCurrent(int statu)
{
    this->currentStatu = statu;
}

bool Wheelchair::goForward()
{
    if(getCurrent() != Wheelchair::STATUS::READY) return false;

    Wire.beginTransmission(B_ChenalAddr);         //STOP x-row
    Wire.write(0x40);
    Wire.write(STOP);
    Wire.endTransmission();

    Wire.beginTransmission(A_ChenalAddr);
    Wire.write(0x40);
    Wire.write(FORWARD_SPEED);
    Wire.endTransmission();

    setCurrent(Wheelchair::STATUS::RUNNING);
    return true;
}

bool Wheelchair::goLeft()
{
    if(getCurrent() != Wheelchair::STATUS::READY || 
    getCurrent() != Wheelchair::STATUS::BLOCKED) return false;

    Wire.beginTransmission(A_ChenalAddr);         //STOP y-row
    Wire.write(0x40);
    Wire.write(STOP);
    Wire.endTransmission();

    Wire.beginTransmission(B_ChenalAddr);
    Wire.write(0x40);
    Wire.write(LEFT_SPEED);
    Wire.endTransmission();

    setCurrent(Wheelchair::STATUS::RUNNING);
    return true;
}

bool Wheelchair::goRight()
{
    if(getCurrent() != Wheelchair::STATUS::READY || 
    getCurrent() != Wheelchair::STATUS::BLOCKED) return false;

    Wire.beginTransmission(A_ChenalAddr);         //STOP y-row
    Wire.write(0x40);
    Wire.write(STOP);
    Wire.endTransmission();

    Wire.beginTransmission(B_ChenalAddr);
    Wire.write(0x40);
    Wire.write(RIGHT_SPEED);
    Wire.endTransmission();

    setCurrent(Wheelchair::STATUS::RUNNING);
    return true;
}

void Wheelchair::brake()
{
    Wire.beginTransmission(B_ChenalAddr);
    Wire.write(0x40);
    Wire.write(STOP);
    Wire.endTransmission();

    Wire.beginTransmission(A_ChenalAddr);
    Wire.write(0x40);
    Wire.write(STOP);
    Wire.endTransmission();

    setCurrent(Wheelchair::STATUS::READY);
}

String Wheelchair::doAction(int action)
{
    int status = getCurrent();
    String ret = "";
    bool doActFlag = false;
    if(status == STATUS::MANUAL) return "DENY!";
    if(status == STATUS::BLOCKED)
    {
        if(action == ACTION::FORWARD)
        {
            ret = "DENY!";
        }
        else
        {
            ret = "OK!";
            doActFlag = true;
        }
    }
    else
    {
        ret = "OK!";
        doActFlag = true;
    }
    
    if(doActFlag)
    {
        switch (action)
        {
            case ACTION::BRAKE:
                brake();
                break;
            case ACTION::FORWARD:
                goForward();
                break;
            case ACTION::LEFT:
                goLeft();
                break;
            case ACTION::RIGHT:
                goRight();
                break;                
            default:
                ret = "ERROR!";
                break;
        }
    }

    return ret;
}
