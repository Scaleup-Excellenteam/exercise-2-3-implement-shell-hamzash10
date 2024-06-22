#include "functions.h"


//splits the input into individual args
vector<vector<string>> split(const string& input,string& input_file,string& output_file,string& append_file){
    vector<vector<string>> piped_args; // split the input into piped commands
    istringstream ss(input);
    string arg;
    input_file = "";
    output_file = "";
    append_file = "";
    string token;
    while (getline(ss, token, '|')) {
        vector<string> args;
        istringstream ss_token(token);
        while(ss_token>>arg) {
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
                if(ss_token>>arg)
                    input_file=arg;
                else{
                    cerr << "Syntax error: Expected file after '<'\n";
                    piped_args.clear();
                    return piped_args;
                }
                continue;
            }
            else if(arg==">"){
                if(ss_token>>arg)
                    output_file=arg;
                else{
                    cerr << "Syntax error: Expected file after '>'\n";
                    piped_args.clear();
                    return piped_args;
                }
                continue;
            }
            else if(arg==">>"){
                if(ss_token>>arg)
                    append_file=arg;
                else{
                    cerr << "Syntax error: Expected file after '>>'\n";
                    piped_args.clear();
                    return piped_args;
                }
                continue;
            }
            args.push_back(arg);
        }
        piped_args.push_back(args);
    }

    //if there is piping set the output to the pipe file so i can push it as an input to the next command
    if(piped_args.size()>1)
        output_file=PIPE1_FILE;

    return piped_args;
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
