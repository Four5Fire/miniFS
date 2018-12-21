//
// Created by caojunyi on 2018/9/10.
//

#include "file.h"
#include "IO.h"
#include "console.h"

bool isLoadSys = false;

int cnt[80]; //用来打印的标记数组
string curPath = "/root";
string curDirName = "";
sysInfo* curSysInfo; //加载的信息块
dirTable* rootDirTable; //加载的根目录
dirTable* curDirTable; //当前所处目录
dirTable* curGetDirTableBlock; //当前获取到的空白目录块
FCB* curGetFcbBlock; //当前获取到的空白FCB块
FCB* rootFCB;
FCB* curFCB;
data* curGetDataBlock;
data* rootData;
data* curData;


void storage_dirTable();
bool loadSys();
void loadDirConnect(dirTable* a);
dirTable* searchDirTable(int block); //根据盘块号返回地址
FCB* searchFCB(int block);
data* searchData(int block);
void storage_sys(); //将sys存入硬盘
void dirTableClean(dirTable* a);
void fcbClean(FCB* a);
void dataClean(data* a);
int searchDirTableFile(dirTable* a,string s);//返回指定目录下的某个文件的位置
void copyFile(dirTable* a, int num, dirTable* b);//将a目录下的指定位置的文件复制到b目录下
dirTable* searchDir(dirTable* a, vector<string> v); //返回指定目录的地址
void copyFileFCB(FCB* a, FCB* b);//将a的FCB复制到B的FCB
void copyFileData(data* a, data* b);//将a数据块的内容复制给b数据块
void copyDir(dirTable* a, dirTable* b);//将a目录复制到b目录块
void printFileFcb(FCB* a); //打印文件的FCB盘块信息
void printFileData(data* a); //打印文件的数据块信息
void treePrintDir(dirTable *a, int num); //树型打印当前目录
void treePrintFile(FCB*a, int num); //打印文件信息
void cp(string s1, string s2);//将s1复制到s2
void mv(string s1, string s2);//将s1移动到s2
void cpIn(string s1, string s2);//将外部文件s1复制进来且命名成s2
void cpOut(string s1, string s2);//将内部文件s1复制到外部命名成s2
bool isFind(string s);//查找指定路径下的指定文件
data* searchFcbDataLast(dirTable* a, int num);//查找此文件对应的最后一块数据块

dirTable* getDirTableBlock()
{
    int num = 0;
    curGetDirTableBlock = rootDirTable;
    while (curGetDirTableBlock->free == '1'){
        if (num == 1){
            return NULL;
        }
        if (curGetDirTableBlock->blockSelf == DIR_MAX){
            curGetDirTableBlock = rootDirTable;
            num++;
        }
        curGetDirTableBlock = curGetDirTableBlock->next_dirTable;
    }
    return curGetDirTableBlock;
}

FCB* getFcbBlock()
{
    int num = 0;
    curGetFcbBlock = rootFCB;
    while (curGetFcbBlock->free == '1'){
        if (num == 1){
            return NULL;
        }
        if (curGetFcbBlock->blockSelef == FCB_MAX){
            curGetFcbBlock = rootFCB;
            num++;
        }
        curGetFcbBlock = curGetFcbBlock->next_FCB;
    }
    return curGetFcbBlock;
}

data* getDataBlock()
{
    int num = 0;
    curGetDataBlock = rootData;
    while (curGetDataBlock->free == '1'){
        if (num == 1){
            return NULL;
        }
        if (curGetDataBlock->blockSelef == DATA_MAX){
            curGetDataBlock = rootData;
            num++;
        }
        curGetDataBlock = curGetDataBlock->next_data;
    }
    return curGetDataBlock;
}

bool needFormat()
{
    bool a;
    sysInfo *sysInfo1;
    sysInfo1 = (struct sysInfo*)malloc(sizeof(sysInfo));
    fstream sFile(get_sysName(),ios::in|ios::out|ios::binary);
    sFile.seekg(0,ios::beg);
    sFile.read((char*)sysInfo1, sizeof(sysInfo));
    sFile.close();
    a = sysInfo1->need_format;
    free(sysInfo1);
    return a;
}

bool loadSys()
{
    if (get_sysName().length() == 0){
        cout<<"no mount"<<endl;
        return false;
    }
    if(needFormat()){
        cout<<"this miniFS must be format"<<endl;
        return false;
    }
    isLoadSys = true;
    sysInfo *sysInfo1;
    dirTable *dirTable1;
    sysInfo1 = (struct sysInfo*)malloc(sizeof(sysInfo));
    dirTable1 = (struct dirTable*)malloc(sizeof(dirTable));
//    fcb = (struct FCB*)malloc(sizeof(FCB));
//    dataBlock = (struct data*)malloc(sizeof(data));
    fstream sFile(get_sysName(),ios::in|ios::out|ios::binary);
    sFile.seekg(0,ios::beg);
    sFile.read((char*)sysInfo1, sizeof(sysInfo));
    sFile.read((char*)dirTable1, sizeof(dirTable));
    curSysInfo = sysInfo1;
    rootDirTable = dirTable1;
    rootDirTable->futher = rootDirTable;
    rootDirTable->self = rootDirTable;
    curDirTable = dirTable1;
    dirTable* a;
    a = rootDirTable;
    for (int i = 1; i <= DIR_MAX; ++i) {
        dirTable1 = (struct dirTable*)malloc(sizeof(dirTable));
        sFile.read((char*)dirTable1, sizeof(dirTable));
        a->next_dirTable = dirTable1;
        a = a->next_dirTable;
    }
    curGetDirTableBlock = rootDirTable;

    FCB* b;
    b = (struct FCB*)malloc(sizeof(FCB));
    sFile.seekg(sizeof(FCB)*(DIR_MAX+1),ios::beg);
    sFile.read((char*)b, sizeof(FCB));
    rootFCB = b;
    curFCB = b;
    FCB* c;
    c = rootFCB;
    for (int i = (DIR_MAX+2); i <= FCB_MAX; ++i) {
        b = (struct FCB*)malloc(sizeof(FCB));
        sFile.read((char*)b, sizeof(FCB));
        c->next_FCB = b;
        c = c->next_FCB;
    }
    curGetFcbBlock = rootFCB;

    data* e;
    e = (struct data*)malloc(sizeof(data));
    sFile.seekg(sizeof(data)*(FCB_MAX+1),ios::beg);
    sFile.read((char*)e, sizeof(data));
    rootData = e;
    curData = e;
    data* f;
    f = rootData;
    for (int i = FCB_MAX+2; i <= DATA_MAX; ++i) {
        e = (struct data*)malloc(sizeof(data));
        sFile.read((char*)e, sizeof(data));
        f->next_data = e;
        f = f->next_data;
    }
    curGetDataBlock = rootData;

    loadDirConnect(rootDirTable);//建立a目录块与子目录块的联系
    sFile.close();
//    cout<<curSysInfo->diskSpace<<endl;
//    cout<<curDirTable->blockSelf<<endl;
    return true;
}

void cd(string s)
{
    regex reg("^..|(./)?[\\w]*|/?[\\w]*|~$");
    if (!regex_match(s,reg)){
        cout<<"order error, Type \"help\" for more information."<<endl;
        return;
    }
    bool isCd = false;
    if (!isLoadSys){
        if (!loadSys())
            return;

    }
    if (s == ".."){
        if (curPath.length() != 5)
            curPath = curPath.substr(0,curPath.length() - curDirName.length()-1);
        curDirTable = curDirTable->futher;
        isCd = true;
    }
    else if(s == "~"){
        curPath = "/root";
        curDirTable = rootDirTable;
        isCd = true;
    }
    else if (s == "./"||s =="/"){
        curDirTable = curDirTable->self;
        isCd = true;
    }
    else{
        s.erase(std::remove(s.begin(), s.end(), '/'), s.end());
        char g[80];
        memset(g, '\000', sizeof(g));
        strcpy(g, s.c_str());
        for (int i = 0, num = 0; i < DIRTABLE_MAX_SIZE; ++i) {
            if (curDirTable->dirs[i].type == '0'){
                if (!strcmp(g,curDirTable->dirs[i].name)){
                    curDirTable = curDirTable->dirs[i].next_dir;
                    isCd = true;
                    curDirName = s;
                    curPath.append("/"+s);
                    break;
                }
            }
            if (curDirTable->dirs[i].type != '2'){
                num++;
                if (num == curDirTable->dirUnitAmount)
                    break;
            }
        }
    }
    if (!isCd){
        cout<<"Not this directory"<<endl;
    }
}

void mkdir_dir(string s)
{
    regex reg("^[A-Z|a-z|_][\\w]*$");
    if (!regex_match(s,reg)){
        cout<<"order error, Type \"help\" for more information."<<endl;
        return;
    }

    if (!isLoadSys){
        if (!loadSys())
            return;
    }
    bool isMakedir = false;

    for (int i = 0; i < DIRTABLE_MAX_SIZE; ++i) {
        char x[80];
        memset(x, '\000', sizeof(x));
        strcpy(x,s.c_str());
        if (curDirTable->dirs[i].type == '0'){
            if (strcmp(curDirTable->dirs[i].name,x) == 0){
                cout<<"this folder already exists"<<endl;
                return;
            }

        }
    }

    for (int i = 0; i < DIRTABLE_MAX_SIZE; ++i) {
        if (curDirTable->dirs[i].type != '2'){
            continue;
        }else{
            dirTable* a;
            a = getDirTableBlock();
            if (a == NULL){
                cout<<"Not dirBlock"<<endl;
                return;
            }
            if (a->blockSelf > curSysInfo->dirLen)
                curSysInfo->dirLen = a->blockSelf;
            curDirTable->dirs[i].next_dir = a;
            curDirTable->dirs[i].type = '0';
            curDirTable->dirs[i].blockSelf = a->blockSelf;
            a->free = '1';
            a->futher = curDirTable;
            a->self = a;
            char g[80];
            memset(g,'\000', sizeof(g));
            strcpy(g,s.c_str());

            strcpy(a->name,g);
            strcpy(curDirTable->dirs[i].name,g);
            isMakedir = true;
            curDirTable->dirUnitAmount += 1;
            curSysInfo->contentCount += 1;
            storage_dirTable();//实时存储到硬盘上
            break;
        }
    }
    if (!isMakedir){
        cout<<"mkdir "<<s<<" error"<<endl;
    }
    return;
}

void dr()
{
    if(!isLoadSys){
        if (!loadSys())
            return;
    }
    int num = 0;
    for (int i = 0; i < DIRTABLE_MAX_SIZE; ++i) {
        if (curDirTable->dirs[i].type == '0'){
            num++;
            cout<<curDirTable->dirs[i].name<<"  ";
        }
        else if (curDirTable->dirs[i].type == '1'){
            num++;
            cout<<curDirTable->dirs[i].name<<"  ";
        }
        if (num == curDirTable->dirUnitAmount)
            break;
    }
    cout<<endl;
}


void storage_dirTable()
{
    fstream sFile(get_sysName(),ios::in|ios::out|ios::binary);
    sFile.seekp(0,ios::beg);
    sFile.write((char*)curSysInfo, sizeof(sysInfo));
    dirTable* a = rootDirTable;
    for (int i = 1; i <= curSysInfo->contentCount; ++i) {
        sFile.write((char*)a, sizeof(dirTable));
        a = a->next_dirTable;
    }
    sFile.close();
}

void loadDirConnect(dirTable* a)
{
    for (int i = 0; i < DIRTABLE_MAX_SIZE; ++i) {
        if (a->dirs[i].type == '2')
            continue;
        else if (a->dirs[i].type == '1')
            a->dirs[i].next_FCB = searchFCB(a->dirs[i].blockSelf);
        else if (a->dirs[i].type == '0')
        {
            a->dirs[i].next_dir = searchDirTable(a->dirs[i].blockSelf);
            a->dirs[i].next_dir->futher = a;
            a->dirs[i].next_dir->self = a->dirs[i].next_dir;
        }
    }
}

dirTable* searchDirTable(int block)
{
    dirTable* a = rootDirTable;
    while (a->blockSelf != block){
        a = a->next_dirTable;
    }
    for (int i = 0; i < DIRTABLE_MAX_SIZE; ++i) {
        if (a->dirs[i].type == '2')
            continue;
        else if (a->dirs[i].type == '1')
            a->dirs[i].next_FCB = searchFCB(a->dirs[i].blockSelf);
        else if (a->dirs[i].type == '0')
        {
            a->dirs[i].next_dir = searchDirTable(a->dirs[i].blockSelf);
            a->dirs[i].next_dir->futher = a;
            a->dirs[i].next_dir->self = a->dirs[i].next_dir;
        }
    }
    return a;
}

FCB* searchFCB(int block)
{
    FCB* a =rootFCB;
    while (a->blockSelef != block){
        a = a->next_FCB;
    }
    if (a->fileSize != 0){
        a->next_data = searchData(a->blockNext);
    }
    return a;
}

data* searchData(int block)
{
    data* a = rootData;
    while (a->blockSelef != block)
    {
        a = a->next_data;
    }
    if (a->blockNext != -1){
        a->next = searchData(a->blockNext);
    }
    return a;
}

string get_path()
{
    return curPath;
}

void new_file(string s)
{
    regex reg("^[A-Z|a-z|_|/][\\w|/]*(.txt)?$");
    if (!regex_match(s,reg)){
        cout<<"order error, Type \"help\" for more information."<<endl;
        return;
    }
    if(!isLoadSys){
        if (!loadSys())
            return;
    }
    dirTable* a;
    vector<string> v1 = split(s, "/");
    if (v1.size() == 1){
        a = curDirTable;
        s = v1[0];
    }
    else{
        s = v1.back();
        v1.pop_back();
        a = searchDir(curDirTable,v1);
    }


    if (a->dirUnitAmount == DIRTABLE_MAX_SIZE){
        cout<<"this directory is full"<<endl;
        return;
    }
    char g[80];
    memset(g,'\000', sizeof(g));
    strcpy(g, s.c_str());
    for (int i = 0; i < DIRTABLE_MAX_SIZE; ++i) {
        if (a->dirs[i].type == '1'){
            if (!strcmp(g, a->dirs[i].name)){
                cout<<"this file already exist"<<endl;
                return;
            }
        }
    }
    int num = 0;
    for ( ;num  < DIRTABLE_MAX_SIZE; ++num) {
        if (a->dirs[num].type == '2')
            break;
    }
    a->dirs[num].next_FCB = getFcbBlock();
    if (a->dirs[num].next_FCB == NULL){
        cout<<"FCB Block is full"<<endl;
        return;
    }
    if (a->dirs[num].next_FCB->blockSelef > curSysInfo->fcbLen)
        curSysInfo->fcbLen = a->dirs[num].next_FCB->blockSelef;
    a->dirUnitAmount += 1;
    a->dirs[num].type = '1';
    a->dirs[num].blockSelf = a->dirs[num].next_FCB->blockSelef;
    strcpy(a->dirs[num].name,g);
    strcpy(a->dirs[num].next_FCB->name,g);
    a->dirs[num].next_FCB->free = '1';
    a->dirs[num].next_FCB->fileSize = 4;
    a->dirs[num].next_FCB->next_data = getDataBlock();
    if (a->dirs[num].next_FCB->next_data == NULL){
        cout<<"the data block is full"<<endl;
        return;
    }
    if (a->dirs[num].next_FCB->next_data->blockSelef > curSysInfo->dataLen)
        curSysInfo->dataLen = a->dirs[num].next_FCB->next_data->blockSelef;
    a->dirs[num].next_FCB->blockNext = a->dirs[num].next_FCB->next_data->blockSelef;

    a->dirs[num].next_FCB->next_data->next = NULL;
    a->dirs[num].next_FCB->next_data->blockNext = -1;
    a->dirs[num].next_FCB->next_data->free = '1';
    a->dirs[num].next_FCB->next_data->len = 0;
//    a->dirs[num].next_FCB->next_data->len = 0;

    curSysInfo->fileCount += 1;
    curSysInfo->availSpace -= 4;
    curSysInfo->availDataBlock -= 1;

    storage_sys();
}

void completeStorage()
{
    fstream sFile(get_sysName(),ios::out|ios::binary);
    sFile.seekp(0,ios::beg);
    sysInfo* sysInfo1 = curSysInfo;
    dirTable* dirTable1 = rootDirTable;
    FCB* fcb1 = rootFCB;
    data* data1 = rootData;

    sFile.write((char*)sysInfo1, sizeof(sysInfo));
    for (int i = 1; i <= DIR_MAX; ++i) {
        sFile.write((char*)dirTable1, sizeof(dirTable));
        dirTable1 = dirTable1->next_dirTable;
    }
    for (int i = 1+DIR_MAX; i <= FCB_MAX; ++i) {
        sFile.write((char*)fcb1, sizeof(FCB));
        fcb1 = fcb1->next_FCB;
    }
    for (int i = 1+FCB_MAX; i <= DATA_MAX; ++i) {
        sFile.write((char*)data1, sizeof(data));
        data1 = data1->next_data;
    }
    sFile.close();
}

void storage_sys()
{
    fstream sFile(get_sysName(),ios::out|ios::binary);
    sFile.seekp(0,ios::beg);
    sysInfo* sysInfo1 = curSysInfo;
    dirTable* dirTable1 = rootDirTable;
    FCB* fcb1 = rootFCB;
    data* data1 = rootData;

    sFile.write((char*)sysInfo1, sizeof(sysInfo));
    for (int i = 1; i <= sysInfo1->dirLen; ++i) {
        sFile.write((char*)dirTable1, sizeof(dirTable));
        dirTable1 = dirTable1->next_dirTable;
    }
    for (int i = 1; i <= sysInfo1->fcbLen; ++i) {
        sFile.write((char*)fcb1, sizeof(FCB));
        fcb1 = fcb1->next_FCB;
    }
    for (int i = 1; i <= sysInfo1->dataLen; ++i) {
        sFile.write((char*)data1, sizeof(data));
        data1 = data1->next_data;
    }
    sFile.close();
}

void dl(string s)
{
    if(!isLoadSys){
        if (!loadSys())
            return;
    }
    bool isDlete = false;

    dirTable* a;
    vector<string> v1 = split(s, "/");
    if (v1.size() == 1){
        a = curDirTable;
        s = v1[0];
    }
    else{
        s = v1.back();
        v1.pop_back();
        a = searchDir(curDirTable,v1);
    }

    int num = 0;
    char g[80] ;
    memset(g, '\000', sizeof(g));
    strcpy(g, s.c_str());
    for (int i = 0; i < DIRTABLE_MAX_SIZE; ++i) {
        if (!strcmp(a->dirs[i].name, g)){
            isDlete = true;
            num = i;break;
        }
    }
    if (!isDlete){
        cout<<"this file or directory does not exist"<<endl;
        return;
    }

    if (a->dirs[num].type == '1'){
        fcbClean(a->dirs[num].next_FCB);
        a->dirs[num].next_FCB = NULL;
        a->dirs[num].type = '2';
        a->dirUnitAmount -= 1;
        memset(a->dirs[num].name, '\000', sizeof(a->dirs[num].name));
    } else if (a->dirs[num].type == '0'){
        dirTableClean(a->dirs[num].next_dir);
        a->dirUnitAmount -= 1;
        a->dirs[num].type = '2';
        memset(a->dirs[num].name, '\000', sizeof(a->dirs[num].name));
    }
    storage_sys();
}

void dirTableClean(dirTable *a){
    curSysInfo->contentCount -= 1;
    a->futher = NULL;
    a->free = '0';
    memset(a->name,'\000', sizeof(a->name));
    a->dirUnitAmount = 0;
    for (int i = 0; i < DIRTABLE_MAX_SIZE; ++i) {
        if (a->dirs[i].type == '0'){
            memset(a->dirs[i].name,'\000', sizeof(a->dirs[i].name));
            a->dirs[i].type = '2';
            dirTableClean(a->dirs[i].next_dir);
            a->dirs[i].next_dir = NULL;
        }else if (a->dirs[i].type == '1'){
            memset(a->dirs[i].name,'\000', sizeof(a->dirs[i].name));
            a->dirs[i].type = '2';
            fcbClean(a->dirs[i].next_FCB);
            a->dirs[i].next_FCB = NULL;
        }
    }
}

void fcbClean(FCB* a){
    curSysInfo->fileCount -= 1;
    dataClean(a->next_data);
    memset(a->name,'\000', sizeof(a->name));
    a->fileSize = 0;
    a->free = '0';
    a->blockNext = -1;
}

void dataClean(data* a)
{
    curSysInfo->availSpace += 4;
    curSysInfo->availDataBlock += 1;
    a->free = '0';
    memset(a->content,'\000', sizeof(a->content));
    a->blockNext = -1;
    if (a->blockNext == -1)
        return;
    dataClean(a->next);
}

void mvShunt(string s1, string s2, string s3)
{
    if(!isLoadSys){
        if (!loadSys())
            return;
    }

    if (s1 == "-t"){ //miniFS内部之间移动
        mv(s2, s3);
    }
    else if(s1 == "-o"){ //将内部文件移动出去
        cpOut(s2, s3);
        dl(s2);
    } else{
        cout << "no such order, please retry, or input \"help\" to check orders." << endl;
    }
}

void cpShunt(string s1, string s2, string s3)
{
    if(!isLoadSys){
        if (!loadSys())
            return;
    }

    if (s1 == "-t"){ //miniFS内部之间复制
        cp(s2, s3);
    }
    else if (s1 == "-i"){ //将外部文件复制进来
        cpIn(s2, s3);
    }
    else if(s1 == "-o"){ //将内部文件复制出去
        cpOut(s2, s3);
    } else{
        cout << "no such order, please retry, or input \"help\" to check orders." << endl;
    }
}

void cpIn(string s1, string s2)
{

    char path[80];
    string s = "C:\\Users\\caojunyi\\Desktop\\miniFS\\cmake-build-debug\\look\\";
    char g[80];
    memset(g, '\000', sizeof(g));
    strcpy(g, s.c_str());
    getcwd(path,80);
    strcat(path,g);

    if (!isFind(s1)){
        cout<<"this file does not exist"<<endl;
        return;
    }
    ifstream inFile(s+s1, ios::in|ios::binary);
    if (!inFile){
        cout<<"this file open error"<<endl;
        return;
    }
    new_file(s2);
    //返回文件所在目录
    dirTable* a;
    vector<string> v1 = split(s2, "/");
    if (v1.size() == 1){
        a = curDirTable;
        s2 = v1[0];
    }
    else{
        s2 = v1.back();
        v1.pop_back();
        a = searchDir(curDirTable,v1);
    }
    int num;
    char p[80];
    memset(p,'\000', sizeof(p));
    strcpy(p, s2.c_str());
    for (int i = 0; i < DIRTABLE_MAX_SIZE; ++i) {
        if (a->dirs[i].type == '1'){
            if (!strcmp(p,a->dirs[i].name)){
                num = i;
                break;
            }
        }
    }

    char ch;
    curData = searchFcbDataLast(a, num);
    while (inFile.get(ch)){
        if (curData->len < 4072){
            curData->content[curData->len] = ch;
            curData->len += 1;
        }
        else{
            curData->next = getDataBlock();
            if (curData->next == NULL){
                cout<<"the data block is full"<<endl;
                return;
            }
            a->dirs[num].next_FCB->fileSize += 4;
            curSysInfo->availSpace -= 4;
            curSysInfo->availDataBlock -= 1;
            curData->blockNext = curData->next->blockSelef;
            curData = curData->next;
            if (curData->blockSelef > curSysInfo->dataLen)
                curSysInfo->dataLen = curData->blockSelef;
            memset(curData->content,'\000', sizeof(curData->content));
            curData->next = NULL;
            curData->len = 0;
            curData->free = '1';
            curData->content[curData->len] = ch;
            curData->len += 1;
        }
//        if (curData->len < 4072){
//            curData->content[curData->len] = ch;
//        } else{
//            curData->next = getDataBlock();
//            if (curData == NULL){
//                cout<<"the data block is full"<<endl;
//                return;
//            }
//            curData->blockNext = curData->next->blockSelef;
//            curData->next->free = '1';
//            curData->next->blockNext = -1;
//            curData = curData->next;
//        }


//        if (strlen(curData->content) == 4076){
//            curData->next = getDataBlock();
//            if (curData->next == NULL){
//                cout<<"the data block is full"<<endl;
//                return;
//            }
//            curData->blockNext = curData->next->blockSelef;
//            curData->next->free = '1';
//            curData->next->blockNext = -1;
//            curData = curData->next;
//        }
//        curData->content[strlen(curData->content)] = ch;
    }
    inFile.close();
    storage_sys();
}

void cpOut(string s1, string s2)
{
    //返回文件所在目录
    dirTable* a;
    vector<string> v1 = split(s1, "/");
    if (v1.size() == 1){
        a = curDirTable;
        s1 = v1[0];
    }
    else{
        s1 = v1.back();
        v1.pop_back();
        a = searchDir(curDirTable,v1);
    }
    if (a == NULL){
        cout<<"this path is error"<<endl;
        return;
    }
    bool isFind = false;
    int num;
    char p[80];
    memset(p,'\000', sizeof(p));
    strcpy(p, s1.c_str());
    for (int i = 0; i < DIRTABLE_MAX_SIZE; ++i) {
        if (a->dirs[i].type == '1'){
            if (!strcmp(p,a->dirs[i].name)){
                isFind = true;
                num = i;
                break;
            }
        }
    }
    if (!isFind){
        cout<<"this file does not exist"<<endl;
        return;
    }

    string s = "C:\\Users\\caojunyi\\Desktop\\miniFS\\cmake-build-debug\\look\\";
    ofstream inFile(s+s2,ios::binary);
    if (!inFile){
        cout<<"this file open error"<<endl;
        return;
    }

    char ch;
    data* data1 = a->dirs[num].next_FCB->next_data;
    while (1){
        for (int i = 0; i < strlen(data1->content); ++i) {
            inFile.put(data1->content[i]);
        }
        if (data1->blockNext == -1)
            break;
        else
            data1 = data1->next;
    }
    inFile.close();
}

void cp(string s1, string s2)
{
    regex reg("^[A-Z|a-z|_|/][\\w|/]*(.txt)?$");
    if (!regex_match(s1,reg)&&!regex_match(s2,reg)){
        cout<<"order error, Type \"help\" for more information."<<endl;
        return;
    }


    vector<string> v1 = split(s1, "/"); //可按多个字符来分隔;
    vector<string> v2 = split(s2, "/");


    if (v2[v2.size()-1].length() > 4){
        string ss2 = v2[v2.size()-1].substr(v2[v2.size()-1].length()-4);
        if (ss2 == ".txt"){
            cout<<"order error, Type \"help\" for more information."<<endl;
            return;
        }
    }

    if (v1[v1.size()-1].length() > 4) {
        string ss1 = v1[v1.size() - 1].substr(v1[v1.size() - 1].length() - 4);
        if (ss1 == ".txt") { //要复制的是个文件
            if (v1.size() == 1) { //当前目录下
                int num = searchDirTableFile(curDirTable, v1[v1.size() - 1]);
                if (num == -1) {
                    cout << "this file does not exist" << endl;
                    return;
                }
                dirTable *target2;
                target2 = searchDir(curDirTable, v2);//返回目标目录的地址
                if (target2 == NULL) {
                    cout << "this directory does not exist" << endl;
                    return;
                }
                copyFile(curDirTable, num, target2);
                storage_sys();
            }
        }
    }
    else{
        dirTable* target1;
        dirTable* target2;
        target1 = searchDir(curDirTable,v1);
        target2 = searchDir(curDirTable,v2);
        if (target1 == NULL||target2 == NULL){
            cout<<"this path is error"<<endl;
            return;
        }
        if (target2->dirUnitAmount == DIRTABLE_MAX_SIZE){
            cout<<"this directory is full"<<endl;
            return;
        }

        for (int i = 0; i < DIRTABLE_MAX_SIZE; ++i) {
            if (!strcmp(target1->name,target2->dirs[i].name)){
                cout<<"this file or directory already exist"<<endl;
                return;
            }
        }

        int num;
        for (num = 0; num < DIRTABLE_MAX_SIZE; ++num) {
            if (target2->dirs[num].type == '2')
                break;
        }
        target2->dirs[num].next_dir = getDirTableBlock();
        if (target2->dirs[num].next_dir == NULL){
            cout<<"the dirTableBlock is full"<<endl;
            return;
        }
        target2->dirs[num].type = '0';
        strcpy(target2->dirs[num].name, target1->name);
        target2->dirs[num].blockSelf = target2->dirs[num].next_dir->blockSelf;
        target2->dirUnitAmount += 1;
        target2->dirs[num].next_dir->futher = target2->self;
        copyDir(target1,target2->dirs[num].next_dir);

        storage_sys();
    }
}

void copyDir(dirTable* a, dirTable* b)
{
    b->self = b;
    b->free = '1';
    strcpy(b->name,a->name);
    int num = 0;
    curSysInfo->contentCount += 1;
    for (int i = 0; i < DIRTABLE_MAX_SIZE; ++i) {
        if (num == a->dirUnitAmount)
            return;
        if (a->dirs[i].type == '1'){
            copyFile(a,i,b);
            b->dirUnitAmount += 1;
            num++;
        }
        else if (a->dirs[i].type == '0'){
            b->dirs[i].next_dir = getDirTableBlock();
            if (b->dirs[i].next_dir == NULL){
                cout<<"the dirTableBlock is full"<<endl;
                return;
            }
            num++;
            b->dirs[i].type = '0';
            strcpy(b->dirs[i].name,a->dirs[i].name);
            b->dirs[i].blockSelf = b->dirs[i].next_dir->blockSelf;
            b->dirs[i].next_dir->futher = b->self;
            b->dirUnitAmount += 1;
            copyDir(a->dirs[i].next_dir,b->dirs[i].next_dir);
        }
    }

}

dirTable* searchDir(dirTable* a, vector<string> v){
    if (v.size() == 0){
        return a;
    }
    char g[80];
    memset(g, '\000', sizeof(g));
    strcpy(g, v[0].c_str());
    for (int i = 0; i < DIRTABLE_MAX_SIZE; ++i) {
        if (!strcmp(a->dirs[i].name, g)){
            a = a->dirs[i].next_dir;
            v.erase(v.begin());
            return searchDir(a,v);
        }
    }
    return NULL;
}

int searchDirTableFile(dirTable* a,string s){
    char g[80];
    memset(g,'\000', sizeof(g));
    strcpy(g, s.c_str());
    for (int i = 0; i < DIRTABLE_MAX_SIZE; ++i) {
        if (!strcmp(a->dirs[i].name,g)){
            return i;
        }
    }
    return -1;
}

void copyFile(dirTable* a, int num, dirTable* b){
    int flag = 0;
    if (b->dirUnitAmount == DIRTABLE_MAX_SIZE){
        cout<<"this directory is full"<<endl;
        return;
    }
    for (flag = 0; flag < DIRTABLE_MAX_SIZE; ++flag) {
        if (b->dirs[flag].type == '2'){
            break;
        }
    }
    b->dirs[flag].next_FCB = getFcbBlock();
    if (b->dirs[flag].next_FCB == NULL){
        cout<<"the FCB is full"<<endl;
        return;
    }
    if (b->dirs[flag].next_FCB->blockSelef > curSysInfo->fcbLen){
        curSysInfo->fcbLen = b->dirs[flag].next_FCB->blockSelef;
    }
    copyFileFCB(a->dirs[num].next_FCB,b->dirs[flag].next_FCB);
    b->dirs[flag].blockSelf = b->dirs[flag].next_FCB->blockSelef;
    b->dirs[flag].type = '1';
    strcpy(b->dirs[flag].name,a->dirs[num].name);
    b->dirUnitAmount += 1;

}

void copyFileFCB(FCB*a, FCB*b)
{
    if (a->fileSize > curSysInfo->availSpace){
        cout<<"this miniFS is not availSpace"<<endl;
        return;
    }
    strcpy(b->name,a->name);
    b->fileSize = a->fileSize;
    b->free = '1';
    strcpy(b->content,a->content);
    b->next_data = getDataBlock();
    if (b->next_data->blockSelef > curSysInfo->dataLen){
        curSysInfo->dataLen = b->next_data->blockSelef;
    }
    copyFileData(a->next_data,b->next_data);
    b->blockNext = b->next_data->blockSelef;
    curSysInfo->fileCount += 1;

}

void copyFileData(data* a, data* b)
{
    b->free = '1';
    strcpy(b->content,a->content);
    curSysInfo->availSpace -= 4;
    curSysInfo->availDataBlock -= 1;
    if (a->blockNext == -1){
        b->next = NULL;
        b->blockNext = -1;


        return;
    }
    b->next = getDataBlock();
    if(b->next->blockSelef > curSysInfo->dataLen){
        curSysInfo->dataLen = b->next->blockSelef;
    }
    copyFileData(a->next,b->next);
    b->blockNext = b->next_data->blockSelef;


}



void mv(string s1,string s2)
{
    regex reg("^[A-Z|a-z|_|/][\\w|/]*(.txt)?$");
    if (!regex_match(s1,reg)&&!regex_match(s2,reg)){
        cout<<"order error, Type \"help\" for more information."<<endl;
        return;
    }

    if(!isLoadSys){
        if (!loadSys())
            return;
    }

    dirTable* a;
    vector<string> v1 = split(s1, "/");
    if (v1.size() == 1){
        a = curDirTable;
        s1 = v1[0];
    }
    else{
        s1 = v1.back();
        v1.pop_back();
        a = searchDir(curDirTable,v1);
    }

    char g1[80];
    memset(g1,'\000', sizeof(g1));
    strcpy(g1,s1.c_str());
    int num1;
    bool isFind = false;
    for (int i = 0; i < DIRTABLE_MAX_SIZE; ++i) {
        if (a->dirs[i].type != '2'){
            if (!strcmp(a->dirs[i].name, g1)){
                num1 = i;
                isFind = true;
                break;
            }
        }
    }

    if (!isFind){
        cout<<"this file or directory does not exist"<<endl;
        return;
    }

    isFind = false;
    int num2;
    dirTable* b;
    vector<string> v2 = split(s2, "/");

    s2 = v2.back();
    b = searchDir(curDirTable,v2);

    if (b == NULL){
        cout<<"this directory does not exist"<<endl;
        return;
    }

    isFind = false;
    for (int i = 0; i < DIRTABLE_MAX_SIZE; ++i) {
        if (b->dirs[i].type == '2'){
            isFind = true;
            num2 = i;
            break;
        }
    }
    if (!isFind){
        cout<<"this directory is full"<<endl;
        return;
    }

    b->dirs[num2].type = a->dirs[num1].type;
    strcpy(b->dirs[num2].name, a->dirs[num1].name);
    b->dirs[num2].next_FCB = a->dirs[num1].next_FCB;
    b->dirs[num2].next_dir = a->dirs[num1].next_dir;
    b->dirs[num2].blockSelf = a->dirs[num1].blockSelf;
    if (a->dirs[num1].type == '0'){
        b->dirs[num2].next_dir->futher = b->self;
    }
    b->dirUnitAmount += 1;
    a->dirUnitAmount -= 1;
    a->dirs[num1].type = '2';
    a->dirs[num1].blockSelf = -1;
    a->dirs[num1].next_FCB = NULL;
    a->dirs[num1].next_dir = NULL;
    memset(a->dirs[num1].name,'\000', sizeof(a->dirs[num1].name));
}

void memoryClean()
{
    curPath = "/root";
    free(curSysInfo);
    dirTable* tempDir = rootDirTable;

    for (int i = 1; i <= DIR_MAX; ++i) {
        rootDirTable = rootDirTable->next_dirTable;
        free(tempDir);
        tempDir = rootDirTable;
    }

    FCB* tempFcb = rootFCB;

    for (int i = DIR_MAX+1; i <= FCB_MAX; ++i) {
        rootFCB = rootFCB->next_FCB;
        free(tempFcb);
        tempFcb = rootFCB;
    }

    data* tempData = rootData;
    for (int i = FCB_MAX+1; i <= DATA_MAX; ++i) {
        rootData = rootData->next_data;
        free(tempData);
        tempData = rootData;
    }
}

void setLoadSysFalse()
{
    isLoadSys = false;
}

bool getLoadSysStatus()
{
    return isLoadSys;
}

void map_file(string s)
{
    regex reg("^[A-Z|a-z|_][\\w]*.txt$");
    if (!regex_match(s,reg)){
        cout<<"order error, Type \"help\" for more information."<<endl;
        return;
    }
    if(!isLoadSys){
        if (!loadSys())
            return;
    }

    dirTable* a = curDirTable;

    bool isFile = false;
    int num = 0;
    char g[80];
    memset(g,'\000', sizeof(g));
    strcpy(g, s.c_str());

    for (num = 0; num < DIRTABLE_MAX_SIZE; ++num) {
        if (a->dirs[num].type == '1'){
            if (!strcmp(a->dirs[num].name, g)){
                isFile = true;
                break;
            }
        }
    }
    if (!isFile){
        cout<<"this file does not exist"<<endl;
        return;
    }
    printFileFcb(a->dirs[num].next_FCB);
}

void printFileFcb(FCB* a)
{
    cout<<a->name<<"   FCB is "<<a->blockSelef<<". "<<endl;
    cout<<a->name<<" is "<<a->fileSize<<"KB. "<<" Data block is  ";
    printFileData(a->next_data);
    cout<<endl;
}

void printFileData(data* a)
{
    cout<<a->blockSelef<<"  ";
    if (a->blockNext != -1){
        printFileData(a->next);
    }
    return;
}

void tree()
{
    if(!isLoadSys){
        if (!loadSys())
            return;
    }
    treePrintDir(rootDirTable, 0);
}

void treePrintDir(dirTable *a, int num)
{
    for (int i = 0; i < num; ++i) {
        cout<<" ";
    }
    cout<<a->name<<endl;
    for (int i = 0; i < DIRTABLE_MAX_SIZE; ++i) {
        if (a->dirs[i].type == '0'){
            treePrintDir(a->dirs[i].next_dir, num + 4);
        }
        else if (a->dirs[i].type == '1'){
            treePrintFile(a->dirs[i].next_FCB, num+4);
        }
    }
}

void treePrintFile(FCB*a, int num)
{
    for (int i = 0; i < num; ++i) {
        cout<<" ";
    }
    cout<<a->name<<endl;
}

void look()
{
    char path[80];
    getcwd(path,80);

    string s = "C:\\Users\\caojunyi\\Desktop\\miniFS\\cmake-build-debug\\look";
    char g[80];
    memset(g, '\000', sizeof(g));
    strcpy(g, s.c_str());
    strcat(path,g);
    vector<string> files;
    regex reg("^[\\w]*.txt$");
    getFiles(s,files, reg);
    cout<<"In windows system: "<<endl;
    for (int i = 0; i < files.size(); ++i) {
        cout<<files[i].c_str()<<"  ";
    }
    cout<<endl;
}

bool isFind(string s)
{
    string t = "C:\\Users\\caojunyi\\Desktop\\miniFS\\cmake-build-debug\\look";
    s.erase(std::remove(s.begin(), s.end(), '/'), s.end());
    vector<string> files;
    regex reg("^[\\w]*.txt$");
    getFiles(t,files, reg);

    for (int i = 0; i < files.size(); ++i) {
        if (files[i] == s){
            return true ;
        }
    }
    return false;
}

data* searchFcbDataLast(dirTable* a, int num)
{
    data* b = a->dirs[num].next_FCB->next_data;
    while (b->next != NULL){
        b = b->next;
    }
    return b;
}

void read_file(string s)
{
    regex reg("^[A-Z|a-z|_|/][\\w|/]*(.txt)?$");
    if (!regex_match(s,reg)){
        cout<<"order error, Type \"help\" for more information."<<endl;
        return;
    }
    if(!isLoadSys){
        if (!loadSys())
            return;
    }
    dirTable* a;
    vector<string> v1 = split(s, "/");
    if (v1.size() == 1){
        a = curDirTable;
        s = v1[0];
    }
    else{
        s = v1.back();
        v1.pop_back();
        a = searchDir(curDirTable,v1);
    }

    bool isExist = false;
    int num;
    char g[80];
    memset(g, '\000', sizeof(g));
    strcpy(g, s.c_str());
    for (int i = 0; i < DIRTABLE_MAX_SIZE; ++i) {
        if (a->dirs[i].type == '1'){
            if (!strcmp(g, a->dirs[i].name)){
                num = i;
                isExist = true;
                break;
            }
        }
    }

    if (!isExist){
        cout<<"this file does not esist"<<endl;
        return;
    }
    data*b = a->dirs[num].next_FCB->next_data;
    while (1){
        for (int i = 0; i < strlen(b->content); ++i) {
            cout<<b->content[i];
        }
        if (b->next == NULL){
            break;
        }
        b = b->next;
    }
}

void opt()
{
    cout<<"系统正在优化....."<<endl;

    for (int i = 0; i < 10000000; ++i) {
        for (int j = 0; j < 100; ++j) {

        }
    }

    cout<<"系统优化完成"<<endl;
}

void tp(string s)
{
    if(!isLoadSys){
        if (!loadSys())
            return;
    }
    string re = "^";
    char ch;
    for (int i = 0; i < s.length(); ++i) {
        ch = s[i];
        if (ch == '*'){
            re.append("*");
        } else if(ch == '?'){
            re.append("?");
        } else{
            re += ch;
        }
    }
    re += "$";
    regex reg(re);
    for (int i = 0; i < DIRTABLE_MAX_SIZE; ++i) {
        if (curDirTable->dirs[i].type != '2'){
            string str = curDirTable->dirs[i].name;
            if (regex_match(str, reg)){
                cout<<str<<"  ";
            }
        }
    }
    cout<<endl;
}