#include "Wheelchair.h"

#define LSR_ECHO 7
#define RSR_ECHO 6
#define LSR_TRIG 5
#define RSR_TRIG 4
#define LINK_LED 12
#define SYS_LED  13

Wheelchair car;
float distance[4] = {0,0,0,0};

void getFrontdistance()
{
    
}

void getLdistaance()
{

}

void getRdistance()
{

}

void getAlldis()
{

}
void setup()
{
    Serial.begin(9600);

//  超声波传感器引脚初始化
    pinMode(LSR_ECHO, INPUT);
    pinMode(RSR_ECHO, INPUT);
    pinMode(LSR_TRIG, OUTPUT);
    pinMode(RSR_TRIG, OUTPUT);

    delay(2000);
    Serial.println("*cmd-init-1-end*"); 

}

void loop()
{
    
}