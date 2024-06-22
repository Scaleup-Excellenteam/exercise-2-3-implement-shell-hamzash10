#ifndef PROCESS_H
#define PROCESS_H   

#include "modules.h"

using namespace std;
class process{
public:
    pid_t pid;
    string command;
    Status status;
    process(const pid_t& pid,string command,const Status& status);
    void ToString() const;
};

#endif