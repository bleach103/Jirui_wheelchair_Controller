#ifndef TwoWire_h
    #include <Wire.h>
#endif

#define FORWARD_SPEED 255
#define LEFT_SPEED    0
#define RIGHT_SPEED   255
#define STOP  128

class Wheelchair
{
    private:
        int A_ChenalAddr;      //A_Chenal DAC control y-row of wheelchair forward and fallback  
        int B_ChenalAddr;      //B_Chenal DAC control x-row of wheelchair left and right
        bool     manualFlag;
        int      currentStatu;
        
        void initController();
        bool goForward();
        bool goLeft();
        bool goRight();
    public:
        enum ACTION{
            FORWARD,BRAKE,LEFT,RIGHT
        };
        enum STATUS{
            READY,RUNNING,BLOCKED,REBLOCKED,MANUAL
        };
        Wheelchair();
        Wheelchair(int A_ChenalAddr,int B_ChenalAddr);
        int  getA_ChenalAddr();
        int  getB_ChenalAddr();
        void setManual(bool manualFlag);
        bool isManual();
        int  getCurrent();
        int  setCurrent(int statu); 
        String doAction(int action);
        void brake();

};
