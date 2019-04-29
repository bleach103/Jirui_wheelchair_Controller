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
        unsigned A_ChenalAddr;      //A_Chenal DAC control y-row of wheelchair forward and fallback  
        unsigned B_ChenalAddr;      //B_Chenal DAC control x-row of wheelchair left and right
        bool     manualFlag;
        int      currentStatu;
        
        void initController();
        bool goForward();
        bool goLeft();
        bool goRight();
        void brake();
    public:
        static enum ACTION{
            FORWARD,STOP,LEFT,RIGHT
        };
        static enum STATUS{
            READY,RUNNING,BLOCKED
        };
        Wheelchair();
        Wheelchair(int A_ChenalAddr,int B_ChenalAddr);
        int  getA_ChenalAddr();
        int  getB_ChenalAddr();
        void setManual(bool manualFlag);
        bool isManual();
        int  getCurrent();
        int  setCurrent(int statu); 
        bool doAction(int action);
};
