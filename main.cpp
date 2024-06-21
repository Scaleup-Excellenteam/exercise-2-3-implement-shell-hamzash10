#include <iostream>
#include <utility>
#include <vector>
#include <sstream>
#include <sys/unistd.h>
#include <sys/wait.h>
#include <filesystem>
#include <thread>
#include <fstream>

using namespace std;

//splits the input into individual args
vector<string> split(const string& input){
    vector<string> args;
    istringstream ss(input);
    string arg;
    while(ss>>arg) {
        if(arg[0]=='$'){
            if(arg[1]=='{'){
                arg.erase(0,2);
                arg.pop_back();
            }else
                arg.erase(0,1);

            char* val=getenv(arg.c_str());
            if(val != NULL)
                arg=val;
            else
                arg="Variable $"+arg+" doesn't exist";

        }

        args.push_back(arg);

    }

    return args;
}


//finds the path of command
string find_path(const string& command){
    string path;

    // if the user provided a full path
    if (command[0]=='/')
        return command;

    //getting the relevant paths
    const char* pathEnv = std::getenv("PATH");
    if (pathEnv != nullptr) {
        istringstream pathStream(pathEnv);
        //find the command path
        while (getline(pathStream, path, ':')) {
            path+="/"+command;
            if(filesystem::exists(path))
                return path;
        }
    }
    return "";
}


enum Status{
    Running,
    Finished
};

struct process{
    pid_t pid;
    string command;
    Status status;
    process(const pid_t& pid,string command,const Status& status):pid(pid),command(std::move(command)),status(status){}
    void ToString() const{
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
};


void fix_status(const shared_ptr<process>& proc){
    int status;
    if(proc->status==Running) {
        pid_t pid = waitpid(proc->pid, &status, WNOHANG);
        if(pid ==0)
            proc->status=Running;
        else if (pid > 0)
            proc->status = Finished;
        else
            cerr << "cant get status of process: " << pid << endl;
    }
}

int main() {
    string input,command;
    vector<string> args;
    vector<shared_ptr<process>> background_processes;
    //open the history file for read/write
    std::fstream history_file("history.txt",ios::app);
    if(!history_file.is_open()){
        cerr<<"ERROR couldn't open history file"<<endl;
    }
    while(true) {

        //print the current working directory as the command line
        try {
            filesystem::path cwd = filesystem::current_path();
            cout << cwd.string() << "> ";
        } catch (const filesystem::filesystem_error& e) {
            cerr << "Error: " << e.what() << endl;
        }



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

        if(command=="exit"){
            break;
        }

        if(command=="myjobs") {
            for (const auto &proc: background_processes) {
                fix_status(proc);
                proc->ToString();
            }
            continue;
        }

        if(command=="myhistory"){
            history_file.close();
            history_file.open("history.txt",ios::in);
            string line;
            while(getline(history_file,line))
                cout<<line<<endl;
            history_file.close();
            history_file.open("history.txt",ios::app);
            continue;
        }

        // check if the command exist
        string path=find_path(command);
        if(path.empty()){
            cerr<<"command doesnt exist: "<<command<<endl;
            continue;
        }



        pid_t pid = fork();

        //child process
        if(pid==0){
            // Convert each string to const char* and add it to the vector
            vector<const char*> constChar;
            for (const string& str : args)
                constChar.push_back(str.c_str());
            constChar.push_back(nullptr);
            execv(path.c_str(), const_cast<char* const*>(constChar.data()));
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

    history_file.close();

    return 0;
}
