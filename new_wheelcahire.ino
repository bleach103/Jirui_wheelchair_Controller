#include "Wheelchair.h"
#include <MsTimer2.h>
#include <string.h>

#define LSR_ECHO       7
#define RSR_ECHO       6
#define LSR_TRIG       5
#define RSR_TRIG       4
#define DSA            2
#define DSB            3
#define LINK_LED       12
#define SYS_LED        13
#define FRONT_LIMITED  20.0  //小于20公分时开始停车
#define LRSIDE_LIMITED 24.0  // 左右的超声波传感器最小探测距离为23公分

Wheelchair car;
float distance[4] = {0,0,0,0};
String cmdData = "";
bool Rmark = false;
bool linkFlag = false;
//                  左前 右前 左 右
char* command_buffer[4];

void checkLink()
{
    if(!linkFlag)
    {
        car.brake();
        digitalWrite(LINK_LED, LOW);
    }
}

void onTimeup()
{
    linkFlag = !linkFlag;
}

void strToken(char str[],char* token)
{
  int i=0;
  char *buffer;
  char *word=strtok_r(str,token,&buffer);
  while(word!=NULL)
  {
    command_buffer[i]=word;
    i++;
    word=strtok_r(NULL,token,&buffer);
  }
}

void changeToManual()
{
    digitalWrite(DSA,HIGH);
    digitalWrite(DSB,HIGH);
}

void changeToController()
{
    digitalWrite(DSA,LOW);
    digitalWrite(DSB,LOW);
}

void getFrontdistance()
{
    digitalWrite(LSR_TRIG, LOW);   // 给触发脚低电平2μs
    delayMicroseconds(2);
    digitalWrite(LSR_TRIG, HIGH);  // 给触发脚高电平10μs，这里至少是10μs
    delayMicroseconds(10);
    digitalWrite(LSR_TRIG, LOW);    // 持续给触发脚低电
    float LFdistance = pulseIn(LSR_ECHO, HIGH);  // 读取高电平时间(单位：微秒)
    LFdistance /= 58;       //为什么除以58等于厘米，  Y米=（X秒*344）/2
     // X秒=（ 2*Y米）/344 ==》X秒=0.0058*Y米 ==》厘米=微秒/58
    distance[0] = LFdistance;

    digitalWrite(RSR_TRIG, LOW);   // 给触发脚低电平2μs
    delayMicroseconds(2);
    digitalWrite(RSR_TRIG, HIGH);  // 给触发脚高电平10μs，这里至少是10μs
    delayMicroseconds(10);
    digitalWrite(RSR_TRIG, LOW);    // 持续给触发脚低电
    float RFdistance = pulseIn(RSR_ECHO, HIGH);  // 读取高电平时间(单位：微秒)
    RFdistance /= 58;       //为什么除以58等于厘米，  Y米=（X秒*344）/2
     // X秒=（ 2*Y米）/344 ==》X秒=0.0058*Y米 ==》厘米=微秒/58
    distance[1] = RFdistance;
}

void getLdistance()
{
    Serial1.write(0x01);
    int i = 0;
    byte recvBuffer[4] = {0,0,0,0};
    int ret = 0;
    while(Serial1.available() > 0)
    {
        if(i<4)
        {
            recvBuffer[i] = Serial1.read();
            delay(2);
            i++;
        }
    }
    ret = (recvBuffer[1]<<8)|recvBuffer[2];
    distance[2] = ret/10.0;
}

void getRdistance()
{
    Serial2.write(0x01);
    int i = 0;
    byte recvBuffer[4] = {0,0,0,0};
    int ret = 0;
    while(Serial2.available() > 0)
    {
        if(i<4)
        {
            recvBuffer[i] = Serial2.read();
            delay(2);
            i++;
        }
    }
    ret = (recvBuffer[1]<<8)|recvBuffer[2];
    distance[3] = ret/10.0;
}

void getAlldis()
{
    getDistanceFront();
    getLdistance();
    getRdistance();
}

void checkRunnable()
{
    //规则：
    // 1.若左右间距小于24cm则判断前是否有障碍物若过没有设置为可恢复阻塞(STATUS::REBLOCK)
    // 等待用户的进一步操作
    // 2.若左右有任何一边小于24cm并且前方有一侧小于20cm则设置为不可恢复阻塞
    // 3.不可恢复阻塞只能通过转向解除
    if(car.getCurrent() == Wheelchair::STATUS::MANUAL) return;
    if(distance[2]<=LRSIDE_LIMITED || distance[3]<=LRSIDE_LIMITED)
    {
        car.brake();
        if(distance[0]<=FRONT_LIMITED || distance[1]<=FRONT_LIMITED)
        {
            car.setCurrent(Wheelchair::STATUS::BLOCKED);
        }
        else
        {
            car.setCurrent(Wheelchair::STATUS::REBLOCKED);
        }
    }
    else if(distance[0]<=FRONT_LIMITED || distance[1]<=FRONT_LIMITED)
    {
        car.brake();
        car.setCurrent(Wheelchair::STATUS::BLOCKED);
    }
    
}

void setup()
{
    Serial.begin(9600);
//  侧向超声波传感器 1左 2右
    Serial1.begin(9600);
    Serial2.begin(9600);

//  超声波传感器引脚初始化
    pinMode(LSR_ECHO, INPUT);
    pinMode(RSR_ECHO, INPUT);
    pinMode(LSR_TRIG, OUTPUT);
    pinMode(RSR_TRIG, OUTPUT);

//  指示灯初始化
    pinMode(SYS_LED, OUTPUT);
    pinMode(LINK_LED, OUTPUT);
    digitalWrite(SYS_LED,LOW);
    digitalWrite(LINK_LED,LOW);

//  CD4053初始化
    pinMode(DSA, OUTPUT);
    pinMode(DSB, OUTPUT);

    MsTimer2::set(2000,onTimeup);

    delay(2000);
    Serial.println("initOK!"); 
    digitalWrite(SYS_LED,HIGH);
}

void loop()
{
    getAlldis();
    checkRunnable();
    while(Serial.available() > 0)        //Command string deal
    {
        cmdData += char(Serial.read());
        delay(2);
        Rmark = true;
    }
    if(Rmark)
    {
        Rmark=false;
        if(cmdData[0]=='*')
        {
            strToken(cmdData.c_str(),"-");
            if(String(command_buffer[1])=="keep"))
            {
                linkFlag = true;
                Serial.println("OK!");
            }
            if(String(command_buffer[1])=="mod")
            {
                if(String(command_buffer[2])=="0")
                {
                    car.setManual(false);

                }
                else if(String(command_buffer[2])=="1")
                {
                    Car.setManual(true);
                }
                Serial.println("OK!");
            }
            else if(String(command_buffer[1])=="forward")
            {
                Serial.println(car.doAction(Wheelchair::ACTION::FORWARD));
            }
            else if(String(command_buffer[1])=="stop")
            {
                Serial.println(car.doAction(Wheelchair::ACTION::BRAKE));
            }
            else if(String(command_buffer[1])=="left")
            {
                Serial.println(car.doAction(Wheelchair::ACTION::LEFT));
            }
            else if(String(command_buffer[1])=="right")
            {
                Serial.println(car.doAction(Wheelchair::ACTION::RIGHT));
            }
        }
        else
        {
            Serial.println("Invaild!");
        }
    }
    checkLink();
    if(linkFlag)
    {
        digitalWrite(LINK_LED, HIGH);
    }
}