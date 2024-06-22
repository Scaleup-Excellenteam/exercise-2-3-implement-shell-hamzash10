#ifndef MODULES_H
#define MODULES_H

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
using namespace std;
namespace fs = filesystem;
const string EXIT="exit";
const string MYJOBS="myjobs";
const string MYHISTORY="myhistory";
const string PIPE1_FILE="pipe1.txt";
const string PIPE2_FILE="pipe2.txt";

enum Status{
    Running,
    Finished
};

#endif // MODULES_H
