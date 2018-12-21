//
// Created by caojunyi on 2018/9/10.
//

#ifndef AI_PCH_H
#define AI_PCH_H

#endif //AI_PCH_H
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <vector>
#include <io.h>
#include <regex>
#include <stdlib.h>
#include <windows.h>
using namespace std;

#define DIR_MAX_NUM 8704
#define FCB_MAX_NUM 16129
#define DATA_MAX_NUM 237310
#define BLOCK_SIZE 4096
#define STR_LENGTH 80

#define DIR_MAX 1000
#define FCB_MAX 4000
#define DATA_MAX 262143


#define DEBUG_order cout<<"parametar error"<<endl;
#define DEBUG_argu cout << "The parameter can only consist of an alphanumeric underscore and cannot start with a number" << endl;