#pragma once

class TimerScope {
public:
    Timer mTimer;
            
    TimerScope(string desc) : mDesc(desc) { 
        mTimer.start();                
    }
            
    ~TimerScope() {
        mTimer.stop();
        double dt = mTimer.getSeconds();
        printf("%s dt=%.4fms\n", mDesc.c_str(), dt*1000.f);
    }
            
    string mDesc;
};
