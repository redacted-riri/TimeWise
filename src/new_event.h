#pragma once
#include <string>

class NewEvent {
public:
    static void makeEvent();

    static void setEvent(bool set) {
        makeNew = set;
    }

    static bool getEvent() {
        return makeNew;
    }
private:
    NewEvent() {}
    NewEvent(const NewEvent&) = delete; 
    void operator=(const NewEvent&) = delete; 

    static bool makeNew;

    static void formatTime(int* hour, int* minute, int* ehour, int* eminute, int* duration);
    static void convertICal(char* name, char* locate, int* shour, int* sminute, int* duration, tm* date);
    // static int formatTime(int* hour, int* minute, int* ampm, int index);
    
};
