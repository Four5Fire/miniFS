//
// Created by caojunyi on 2018/9/10.
//

#include "IO.h"
#include "pch.h"
#include "file.h"

string curSysName;
bool curSysMountStatus = false;

void iniSys(string sysName)
{
    regex reg("^[A-Z|a-z|_][\\w]*.minifs$");
    if (!regex_match(sysName,reg)){
        cout<<"order error, Type \"help\" for more information."<<endl;
        return;
    }

    ofstream outFile(sysName,ios::out|ios::binary);
    if (!outFile){
        cout << "The file open error!"<<endl;
        return;
    }else{
        //系统信息块初始化
        cout<<"waiting......"<<endl;
        sysInfo *sysInfo1 = new sysInfo;
        sysInfo1->diskSpace = 1048576;
        sysInfo1->availSpace = (DATA_MAX - FCB_MAX)*4;
        sysInfo1->fileCount = 0;
        sysInfo1->contentCount = 1;
        sysInfo1->dirLen = 1;
        sysInfo1->fcbLen = 0;
        sysInfo1->dataLen = 0;
        sysInfo1->need_format = true;
        sysInfo1->availDataBlock = DATA_MAX-FCB_MAX;
        outFile.seekp(0,ios::beg);
        outFile.write((char*)sysInfo1, sizeof(sysInfo));
        free(sysInfo1);

        int num = 1;
        //目录块初始化
        dirTable* a = (struct dirTable*) malloc(sizeof(dirTable));
        a->futher = a;
        a->self = a;
        a->dirUnitAmount = 0;
        a->blockSelf = num;
        num++;
        a->next_dirTable = NULL;
        a->free = '1';
        outFile.seekp(sizeof(sysInfo),ios::beg);
        outFile.write((char*)a, sizeof(dirTable));
        free(a);


        for (num = 2; num <= DIR_MAX ; ++num) {
            outFile.seekp(sizeof(dirTable)*num,ios::beg);
            a = (struct dirTable*) malloc(sizeof(dirTable));
            a->self = a;
            a->next_dirTable = NULL;
            a->free = '0';
            a->dirUnitAmount = 0;
            outFile.write((char*)a, sizeof(dirTable));
            free(a);
        }

        //FCB块初始化
        FCB *b;
        for (num = DIR_MAX+1; num <= FCB_MAX; ++num) {
            outFile.seekp(sizeof(FCB)*num,ios::beg);
            b = (struct FCB*)malloc(sizeof(FCB));
            b->fileSize = 0;
            b->free = 0;
            b->next_FCB = NULL;
            b->blockSelef = num;
            outFile.write((char*)b, sizeof(FCB));
            free(b);
        }

        //数据块初始化
        data* c;
        for (num = FCB_MAX+1; num <= DATA_MAX; ++num) {
            outFile.seekp(sizeof(data)*num,ios::beg);
            c = (struct data*)malloc(sizeof(data));
            c->blockSelef = num;
            c->free = '0';
            c->next_data = NULL;
            c->blockNext = -1;
            c->len = 0;
//            c->len = 0;
            outFile.write((char*)c, sizeof(data));
            free(c);
        }


    }
    outFile.close();
}

void mountSys(string sysName)
{
    regex reg("^[A-Z|a-z|_][\\w]*.minifs$");
    if (!regex_match(sysName,reg)){
        cout<<"order error, Type \"help\" for more information."<<endl;
        return;
    }
    fstream inFile(sysName,ios::in|ios::binary);

    if (!inFile){
        cout<<"The file open error!"<<endl;
        return;
    }else{
        curSysMountStatus = true;
        curSysName = sysName;
        sysInfo *sysInfo1 = new sysInfo;

        inFile.read((char*)sysInfo1, sizeof(sysInfo));
        cout<<"diskSpace     "<<sysInfo1->diskSpace<<"KB"<<endl;
        cout<<"availSpace    "<<sysInfo1->availSpace<<"KB"<<endl;
        cout<<"fileCount     "<<sysInfo1->fileCount<<endl;
        cout<<"contentCount  "<<sysInfo1->contentCount<<endl;
        cout<<"availDataBlock        "<<sysInfo1->availDataBlock<<endl;
        inFile.close();
        free(sysInfo1);
    }
}
void fmtSys()
{
//    cout<< sizeof(sysInfo)<<endl;
//    cout<< sizeof(dirUnit)<<endl;
//    cout<< sizeof(dirTable)<<endl;
//    cout<< sizeof(FCB)<<endl;
//    cout<< sizeof(data)<<endl;

//    cout<<curSysName<<endl;
    fstream sFile(curSysName,ios::out|ios::binary);
    if (!sFile){
        cout<<"The file fmt error!"<<endl;
        return;
    }

    if (curSysMountStatus){
        if (getLoadSysStatus())
            memoryClean();
        setLoadSysFalse();
    }
    //系统块初始化

    cout<<"waiting......"<<endl;

    sysInfo *sysInfo1 = (struct sysInfo*)malloc(sizeof(sysInfo));
//    sFile.read((char*)sysInfo1, sizeof(sysInfo1));
    sysInfo1->diskSpace = 1048576;
    sysInfo1->availSpace = (DATA_MAX - FCB_MAX)*4;
    sysInfo1->fileCount = 0;
    sysInfo1->contentCount = 1;
    sysInfo1->dirLen = 1;
    sysInfo1->fcbLen = 0;
    sysInfo1->dataLen = 0;
    sysInfo1->need_format = false;
    sysInfo1->availDataBlock = DATA_MAX - FCB_MAX;
    memset(sysInfo1->fill, '\000', sizeof(sysInfo1->fill));
    sFile.seekp(0,ios::beg);
    sFile.write((char*)sysInfo1, sizeof(sysInfo));

//    sysInfo *sysInfo2 = new sysInfo;
//    sFile
//    sFile.seekg(sizeof(sysInfo)*2,ios::beg);
//    sFile.read((char*)sysInfo2, sizeof(sysInfo2));

    //目录块初始化
    string s1 = "/root";
    char g[20];
    memset(g, '\000', sizeof(g));
    strcpy(g, s1.c_str());
    int num = 1;
    dirTable* a;
    a = (struct dirTable*)malloc(sizeof(dirTable));
    memset(a->name,'\000', sizeof(a->name));
    a->futher = a;
    a->self = a;
    a->dirUnitAmount = 0;
    a->blockSelf = num;
    strcpy(a->name,g);
    num++;
    a->next_dirTable = NULL;
    a->free = '1';
    for (int j = 0; j < DIRTABLE_MAX_SIZE; ++j) {
        a->dirs[j].type = '2';
        a->dirs[j].next_dir = NULL;
        a->dirs[j].next_FCB = NULL;
        memset(a->dirs[j].name, '\000', sizeof(a->dirs[j].name));
    }
    sFile.seekp(sizeof(sysInfo),ios::beg);
    sFile.write((char*)a, sizeof(dirTable));
    free(a);

    for (num = 2; num <= DIR_MAX; ++num) {
        a = (struct dirTable*)malloc(sizeof(dirTable));
        memset(a->name,'\000', sizeof(a->name));
        a->futher = NULL;
        a->self = a;
        a->dirUnitAmount = 0;
        a->blockSelf = num;
        a->free = '0';
        a->next_dirTable = NULL;
        for (int i = 0; i < DIRTABLE_MAX_SIZE; ++i) {
            memset(a->dirs[i].name,'\000', sizeof(a->dirs[i].name));
            a->dirs[i].type = '2';
            a->dirs[i].next_dir = NULL;
            a->dirs[i].next_FCB = NULL;
        }
        sFile.seekp(sizeof(dirTable)*num, ios::beg);
        sFile.write((char*)a, sizeof(dirTable));
        free(a);
    }

    //FCB块初始化
    FCB* b;
    for (num = DIR_MAX+1; num <= FCB_MAX; ++num) {
        b = (struct FCB*)malloc(sizeof(FCB));
        b->free = '0';
        b->next_data = NULL;
        b->blockSelef = num;
        b->next_FCB = NULL;
        b->fileSize = 0;
        memset(b->name,'\000', sizeof(b->name));
        memset(b->content,'\000', sizeof(b->content));
        sFile.seekp(sizeof(FCB)*num, ios::beg);
        sFile.write((char*)b, sizeof(FCB));
        free(b);
    }

    //数据块初始化
    data* c;
    for (num = FCB_MAX+1; num <= DATA_MAX; ++num) {
        c = (struct data*)malloc(sizeof(data));
        memset(c->content,'\000', sizeof(c->content));
//        c->len = 0;
        c->free = '0';
        c->blockSelef = num;
        c->next = NULL;
        c->next_data = NULL;
        c->blockNext = -1;
        c->len = 0;
        sFile.seekp(sizeof(data)*num, ios::beg);
        sFile.write((char*)c, sizeof(data));
        free(c);
    }
    sFile.close();
}

void closeSys()
{
    if (getLoadSysStatus()){
        completeStorage();
        memoryClean();
    }

    curSysMountStatus = false;
    if (curSysName.length() == 0){
        cout<<"Please mount the system first"<<endl;
    }
    curSysName = "";
}

bool get_mountStatus()
{
    return curSysMountStatus;
}

string get_sysName()
{
    return curSysName;
}

void show_sys()
{
    char path[80];
    getcwd(path,80);
    vector<string> files;
    regex reg("^[A_Z|a-z|_][\\w]*.minifs$");
    getFiles(path,files, reg);
    cout<<"these minifses already exist : ";
    for (int i = 0; i < files.size(); ++i) {
        cout<<files[i].c_str()<<"  ";
    }
    cout<<endl;
}

void getFiles( string path, vector<string>& files ,regex reg)
{
    //文件句柄
    long hFile = 0;
    //文件信息
    struct _finddata_t fileinfo;
    string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
        do {
            if (regex_match(fileinfo.name, reg))
                files.push_back(fileinfo.name);
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
}

void att()
{
    string sysName = get_sysName();
    if (sysName.length() == 0){
        cout<<"Please mount the system first"<<endl;
        return;
    }
    sysInfo *sysInfo1 = new sysInfo;
    fstream inFile(sysName,ios::in|ios::binary);
    inFile.read((char*)sysInfo1, sizeof(sysInfo));
    cout<<"diskSpace     "<<sysInfo1->diskSpace<<"KB"<<endl;
    cout<<"availSpace    "<<sysInfo1->availSpace<<"KB"<<endl;
    cout<<"fileCount     "<<sysInfo1->fileCount<<endl;
    cout<<"contentCount  "<<sysInfo1->contentCount<<endl;
    cout<<"availDataBlock        "<<sysInfo1->availDataBlock<<endl;
    inFile.close();
    free(sysInfo1);
}


void test()
{
    dirTable* a = (struct dirTable*)malloc(sizeof(dirTable));
    fstream sFile(curSysName,ios::in|ios::out|ios::binary);
    if (!sFile){
        cout<<"test error"<<endl;
        return;
    }
    sFile.seekg(sizeof(sysInfo),ios::beg);
    for (int i = 1; i <= DIR_MAX; ++i) {
        sFile.read((char*)a, sizeof(dirTable));
        if(a->free == '1')
            cout<<a->name<<endl;
        cout<<a->blockSelf<<endl;
    }
    FCB* b =(struct FCB*)malloc(sizeof(FCB));
    sFile.seekg(sizeof(sysInfo)*(DIR_MAX+1),ios::beg);
    for (int i = DIR_MAX+1; i <= FCB_MAX; ++i) {
        sFile.read((char*)b, sizeof(FCB));
        if (b->free == '1')
            cout<<b->name<<endl;
        cout<<b->blockSelef<<endl;
    }
    data* c =(struct data*)malloc(sizeof(data));
    sFile.seekg(sizeof(sysInfo)*(FCB_MAX+1),ios::beg);
    for (int i = FCB_MAX+1; i <= DATA_MAX; ++i) {
        sFile.read((char*)c, sizeof(data));
        if (c->free == '1')
            cout<<c->content<<endl;
        cout<<c->blockSelef<<endl;
    }
    sFile.close();
}
