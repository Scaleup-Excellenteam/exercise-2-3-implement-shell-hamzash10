#ifndef MODULES_H
#define MODULES_H


//------------------includes--------------------
#include <iostream>
#include <filesystem>
#include <iostream>
#include <utility>
#include <vector>
#include <sstream>
#include <sys/unistd.h>
#include <sys/wait.h>
#include <thread>
#include <fstream>
#include <fcntl.h>
#include <unordered_map>
using namespace std;


//---------------const and enums-----------------
namespace fs = filesystem;
// const string EXIT="exit";
// const string MYJOBS="myjobs";
enum Command {
    EXIT,
    MYJOBS,
    MYHISTORY,
    UNKNOWN
};

const string PIPE1_FILE="pipe1.txt";
const string PIPE2_FILE="pipe2.txt";
const char PIPE='|';
const char DOLLAR='$';
const char LEFT_BRACE='{';
const char RIGHT_BRACE='}';
const char SLASH='/';
const string LEFT_REDIRECTION="<";
const string RIGHT_REDIRECTION=">";
const string DOUBLE_RIGHT_REDIRECTION=">>";
enum Status{
    Running,
    Finished
};

#endif // MODULES_H
