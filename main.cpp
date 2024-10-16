#include "Process.h"
#include "functions.h"

unordered_map<string, Command> command_map = {
        {"default",UNKNOWN},
        {"exit", EXIT},
        {"myjobs", MYJOBS},
        {"myhistory", MYHISTORY}
};

bool handel_main_redirections(string& file, string& output_file, const int& type, bool last_pipe){

    int fd;
    switch (type)
    {
        case 1:
            //input file
            fd = open(file.c_str(), O_RDONLY);
            break;

        case 2:
            //output file
            fd = open(file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            break;

        case 3:
            //append file
            fd = open(file.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
            break;
    }

    if (fd == -1) {
        cerr << "Failed to open file: " << file << endl;
        return false;
    }
    if(type==1)
        dup2(fd, STDIN_FILENO);
    else
        dup2(fd,STDOUT_FILENO);
    close(fd);
    // for the last pipe print to the screen
    if(last_pipe)
        output_file.clear();
    return true;
}

int main() {
    string input,command;
    vector<vector<string>> piped_args;
    vector<shared_ptr<process>> background_processes;
    //open the history file for read/write
    fstream history_file("history.txt",ios::app);
    if(!history_file.is_open()){
        cerr<<"ERROR couldn't open history file"<<endl;
    }

    string input_file,output_file,append_file;
    bool is_custom_command=false;
    bool is_break=false;
    pwd();
    // read the command line
    while(getline(cin,input)) {

        //add command to history
        history_file<<input<<endl;

        piped_args=split(input,input_file,output_file,append_file);

        if(piped_args.size()==0){
            pwd();
            continue;
        }
        int current_pipe_number=0;
        for(auto args : piped_args){
            current_pipe_number++;
            bool background=false;
            if(args.back() == "&"){
                background=true;
                args.pop_back();
            }

            command=args.front();
            string line;
            Command command = command_map.count(command) ? command_map[command] : UNKNOWN;
            switch (com)
            {
                case MYJOBS:
                    for (const auto &proc: background_processes) {
                        fix_status(proc);
                        proc->ToString();
                    }
                    is_custom_command=true;
                    break;

                case MYHISTORY:
                    history_file.close();
                    history_file.open("history.txt",ios::in);
                    while(getline(history_file,line))
                        cout<<line<<endl;
                    history_file.close();
                    history_file.open("history.txt",ios::app);
                    is_custom_command=true;
                    break;

                case EXIT:
                    is_break=true;
                    break;

                default:
                    break;
            };

            if(is_break)
                break;

            if(!is_custom_command){
                // check if the command exist
                string path=find_path(command);
                if(path.empty())
                    cerr<<"command doesnt exist: "<<command<<endl;
                else{

                    pid_t pid = fork();

                    //child process
                    if(pid==0){
                        bool successfuly_opened_file = true;
                        // handel input redirection
                        if(!input_file.empty()){
                            successfuly_opened_file=handel_main_redirections(input_file,output_file,1,current_pipe_number == piped_args.size());
                        }
                        // handel output redirection
                        if(!output_file.empty()){
                            successfuly_opened_file=handel_main_redirections(output_file,output_file,2,false);
                        }
                            // handel append redirection
                        else if(!append_file.empty()){
                            successfuly_opened_file=handel_main_redirections(append_file,output_file,3,false);
                        }

                        if(!successfuly_opened_file){
                            pwd();
                            is_custom_command=false;
                            continue;
                        }

                        // Convert each string to const char* and add it to the vector
                        vector<const char*> string_to_const_char;
                        for (const string& str : args)
                            string_to_const_char.push_back(str.c_str());
                        string_to_const_char.push_back(nullptr);
                        execv(path.c_str(), const_cast<char* const*>(string_to_const_char.data()));
                        cerr<<"unable to execute command: "<<command<<endl;
                    }
                        //parent process
                    else if(pid>0) {
                        //changing the output to an input for the piping and the other way around
                        if(!input_file.empty() && input_file==PIPE1_FILE){
                            input_file=PIPE2_FILE;
                            output_file=PIPE1_FILE;
                        }else{
                            input_file=PIPE1_FILE;
                            output_file=PIPE2_FILE;
                        }
                        if (!background)
                            waitpid(pid, nullptr, 0);
                        else
                            background_processes.push_back(make_shared<process>(pid, input, Running));
                    }//failed to fork
                    else
                        cerr<<"fork failed"<<endl;
                }
            }
            if(current_pipe_number == piped_args.size())
                pwd();
            is_custom_command=false;
        }
    }

    history_file.close();

    return 0;
}


// Regular Pipe: connects two related processes via in-memory buffers, uses two files two read and write (alot like what i did)
// Named Pipe: acts as a named file in the filesystem for asynchronous communication between unrelated processes.