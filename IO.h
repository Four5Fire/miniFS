//
// Created by caojunyi on 2018/9/10.
//

#ifndef AI_IO_H
#define AI_IO_H

#endif //AI_IO_H

#include "pch.h"

void iniSys(string sysName);

void mountSys(string sysName);

bool get_mountStatus();

string get_sysName();

void show_sys();

void fmtSys();

void closeSys();

void att(); //显示空间文件属性

void getFiles(string path, vector<string>& files, regex reg);//获取给定目录下的所有文件的文件名

void test();