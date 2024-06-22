#include "functions.h"


//splits the input into individual args
vector<string> split(const string& input,string& input_file,string& output_file,string& append_file){
    vector<string> args;
    istringstream ss(input);
    string arg;
    input_file = "";
    output_file = "";
    append_file = "";
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
        else if(arg=="<"){
            if(ss>>arg)
                input_file=arg;
            else{
                cerr << "Syntax error: Expected file after '<'\n";
                args.clear();
                return args;
            }
            continue;
        }
        else if(arg==">"){
            if(ss>>arg)
                output_file=arg;
            else{
                cerr << "Syntax error: Expected file after '>'\n";
                args.clear();
                return args;
            }
            continue;
        }
        else if(arg==">>"){
            if(ss>>arg)
                append_file=arg;
            else{
                cerr << "Syntax error: Expected file after '>>'\n";
                args.clear();
                return args;
            }
            continue;
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

void pwd(){
    //print the current working directory as the command line
    try {
        fs::path cwd = fs::current_path();
        cout << cwd.string() << "> ";
    }  catch (const fs::filesystem_error& e) {
        cerr << "Error: " << e.what() << endl;
    } catch (const system_error& e) {
        cerr << "System error: " << e.what() << endl;
    }
}
