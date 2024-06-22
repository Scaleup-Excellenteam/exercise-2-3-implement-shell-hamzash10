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

    string input_file,output_file,append_file;
    bool is_custom_command=false;
    pwd();
    // read the command line
    while(getline(cin,input)) {
    
        //add command to history
        history_file<<input<<endl;

        args=split(input,input_file,output_file,append_file);

        bool background=false;
        if(args.back() == "&"){
            background=true;
            args.pop_back();
        }

        command=args.front();

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
       
        if(command==EXIT){
            break;
        }
        
        if(!is_custom_command && !args.empty()){
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
                        string empty_string="";
                        ifstream file(input_file);
                        if (!file.is_open()) {
                            cerr << "Failed to open file: " << input_file << endl;
                            return 1;
                        }

                        string line;
                        while (getline(file, line)) { 
                            auto line_content=split(line,empty_string,empty_string,empty_string);
                            args.insert(args.end(),line_content.begin(),line_content.end());
                            args.push_back("\n");
                        }

                        file.close(); // Close the file
                    }
                    // handel output redirection
                    else if(!output_file.empty()){
                        int fd = open(output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (fd == -1) {
                            cerr << "Failed to open output file: " << output_file << endl;
                            successfuly_opened_file = false;
                        }
                        dup2(fd, STDOUT_FILENO);
                        close(fd);
                    }
                    // handel append redirection
                    else if(!append_file.empty()){
                        int fd = open(append_file.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
                        if (fd == -1) {
                            cerr << "Failed to open append file: " << append_file << endl;
                            successfuly_opened_file = false;
                        }
                        dup2(fd, STDOUT_FILENO);
                        close(fd);
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
        is_custom_command=false;

    }

    history_file.close();

    return 0;
}
