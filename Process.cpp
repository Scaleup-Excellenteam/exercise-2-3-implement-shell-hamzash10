#include "Process.h"

process::process(const pid_t &pid, string command, const Status &status):pid(pid),command(std::move(command)),status(status){}

void process::ToString() const
{
        cout<<"PID: "<<pid<<endl;
        cout<<"command: "<<command<<endl;
        switch (status) {
            case Running:
                cout<<"status: Running\n"<<endl;
                break;
            case Finished:
                cout<<"status: Finished\n"<<endl;
                break;
        }
    }
