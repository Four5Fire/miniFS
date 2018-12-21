//
// Created by caojunyi on 2018/9/10.
//

#ifndef AI_CONSOLE_H
#define AI_CONSOLE_H

#endif //AI_CONSOLE_H

void miniFS_prompt();//打印miniFS提示符

string getOrder();//获取用户输入，返回该输入

void orderPerform(string order);//得到用户输入的命令，对命令进行执行

vector<string> split(const string &s, const string &seperator);

void helpSys();