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
using namespace std;
namespace fs = filesystem;
const string EXIT="exit";
const string MYJOBS="myjobs";
const string MYHISTORY="myhistory";


enum Status{
    Running,
    Finished
};

#endif // MODULES_H
