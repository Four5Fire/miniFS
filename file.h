//
// Created by caojunyi on 2018/9/10.
//

#ifndef AI_FILE_H
#define AI_FILE_H

#endif //AI_FILE_H

#include "pch.h"

struct dirTable;
struct FCB;

//系统信息
struct sysInfo {
    int diskSpace;	//磁盘空间，KB记
    int availSpace;	//可用空间，KB记
    int fileCount;	//文件数目
    int contentCount;	//目录数目
    int availDataBlock;	//数据块数目
    int dirLen;//目录长度
    int fcbLen;//fcb长度
    int dataLen;//数据长度
    bool need_format;
    char fill[4060];
};

//目录项
struct dirUnit{
    char name[50]; //目录项名字, 最多50个字符
    char type; //目录项类型（0为子目录，1为数据文件, 2什么都不是）
    dirTable* next_dir; //具体内容所在块号
    FCB* next_FCB;
    int blockSelf; //指向的目录块块号或者FCB块号
};

#define DIRTABLE_MAX_SIZE 63



//目录
struct dirTable{
    int dirUnitAmount; //目录项数目, 最多63
    dirUnit dirs[DIRTABLE_MAX_SIZE]; //目录项列表
    dirTable* futher; //父目录地址
    dirTable* self; //自身地址
    char free; //是否使用中,0未使用，1使用
    char name[43];
    int blockSelf;
    dirTable* next_dirTable; //下一个目录块
};


//数据块
struct data{
    int blockSelef; //自身块号
    data *next; //下一块内容存储地址
    char free; //是否使用中,0未使用，1使用
    data* next_data; //下一块数据块
    short len;
    char content[4072];
//    int len;//内容字符存取个数
    int blockNext; //指向的下一块内容的数据块块号
};

//FCB
struct FCB{
    char name[55]; //文件名
    data* next_data; //指向的数据块
    int blockSelef; //自身块号
    int fileSize; //文件大小
    char free; //是否使用中,0未使用，1使用
    FCB* next_FCB; //下一块FCB块
    char content[4016];
    int blockNext; //指向的数据块块号
};

void cd(string s);

void mkdir_dir(string s);

void dr();

string get_path();

void new_file(string s);

void dl(string s);

void memoryClean(); //清理内存

void setLoadSysFalse(); //初始化挂载状态

bool getLoadSysStatus(); //得到系统加载进内存状态

void map_file(string s); //显示指定文件在空间所占用的全部块的块号

void tree();

void completeStorage();

void look();

void cpShunt(string s1, string s2, string s3); //将cp命令进行分流

void mvShunt(string s1, string s2, string s3); //将mv命令进行分流

void read_file(string s);//显示一个文本文件内容

void opt();//优化整个空间，将每个文件尽可能连续存放（使用连续的块号存储文件内容

void tp(string s);