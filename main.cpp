#include "modules.h"
#include "Process.h"
#include "functions.h"

int main() {
    string input,command;
    vector<string> args;
    vector<shared_ptr<process>> background_processes;
    //open the history file for read/write
    fstream history_file("history.txt",ios::app);
    if(!history_file.is_open()){
        cerr<<"ERROR couldn't open history file"<<endl;
    }


    pwd();

    // read the command line
    getline(cin,input);
    //add command to history
    history_file<<input<<endl;
    args=split(input);

    bool background=false;
    if(args.back() == "&"){
        background=true;
        args.pop_back();
    }

    command=args.front();
    bool is_custom_command=false;
    while(command!=EXIT) {


        if(command==MYJOBS) {
            for (const auto &proc: background_processes) {
                fix_status(proc);
                proc->ToString();
            }
            is_custom_command=true;
        }

        if(command==MYHISTORY){
            history_file.close();
            history_file.open("history.txt",ios::in);
            string line;
            while(getline(history_file,line))
                cout<<line<<endl;
            history_file.close();
            history_file.open("history.txt",ios::app);
            is_custom_command=true;
        }
        
        if(!is_custom_command){
            // check if the command exist
            string path=find_path(command);
            if(path.empty())
                cerr<<"command doesnt exist: "<<command<<endl;
            else{

                pid_t pid = fork();

                //child process
                if(pid==0){
                    // Convert each string to const char* and add it to the vector
                    vector<const char*> string_to_const_char;
                    for (const string& str : args)
                        string_to_const_char.push_back(str.c_str());
                    string_to_const_char.push_back(nullptr);
                    execv(path.c_str(), const_cast<char* const*>(string_to_const_char.data()));
                    cerr<<"unable to execute command: "<<command<<endl;
                }
                //parent process
                else if(pid>0)
                    if(!background)
                        waitpid(pid, nullptr, 0);
                    else
                        background_processes.push_back(make_shared<process>(pid,input,Running));
                //failed to fork
                else
                    cerr<<"fork failed"<<endl;
            }
        }



        pwd();

        // read the command line
        getline(cin,input);
        //add command to history
        history_file<<input<<endl;
        args=split(input);

        bool background=false;
        if(args.back() == "&"){
            background=true;
            args.pop_back();
        }

        command=args.front();
        is_custom_command=false;
    }

    history_file.close();

    return 0;
}
