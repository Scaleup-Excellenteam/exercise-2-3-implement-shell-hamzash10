#ifndef FUNCTIONS_H
#define FUNCTIONS_H   
#include "modules.h"
#include "Process.h"

vector<vector<string>> split(const string& input,string& input_file,string& output_file,string& append_file);

string find_path(const string& command);

void fix_status(const shared_ptr<process>& proc);

void pwd();


#endif