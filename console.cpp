//
// Created by caojunyi on 2018/9/10.
//

#include "pch.h"
#include "console.h"
#include "file.h"
#include "IO.h"
#include <regex>
#include <map>
#include <functional>
using namespace std;

map<string, function<void()>> order1_map = {
        {"show",show_sys},{"fmt",fmtSys},{"close",closeSys},{"dr",dr},{"test", test},{"help", helpSys},{"att",att},{"tree", tree},{"look",look},{"opt",opt}
};
map<string, function<void(string)>> order2_map = {
        {"create",iniSys},{"mount",mountSys},{"cd",cd},{"mkdir",mkdir_dir},{"new",new_file},{"dl",dl},{"map", map_file},{"read",read_file},{"tp",tp}
};
map<string, function<void(string, string, string)>> order3_map = { {"cp",cpShunt},{"mv",mvShunt} };


//分割函数
vector<string> split(const string &s, const string &seperator) {
    vector<string> result;
    typedef string::size_type string_size;
    string_size i = 0;

    while (i != s.size()) {
        //找到字符串中首个不等于分隔符的字母；
        int flag = 0;
        while (i != s.size() && flag == 0) {
            flag = 1;
            for (string_size x = 0; x < seperator.size(); ++x)
                if (s[i] == seperator[x]) {
                    ++i;
                    flag = 0;
                    break;
                }
        }

        //找到又一个分隔符，将两个分隔符之间的字符串取出；
        flag = 0;
        string_size j = i;
        while (j != s.size() && flag == 0) {
            for (string_size x = 0; x < seperator.size(); ++x)
                if (s[j] == seperator[x]) {
                    flag = 1;
                    break;
                }
            if (flag == 0)
                ++j;
        }
        if (i != j) {
            result.push_back(s.substr(i, j - i));
            i = j;
        }
    }
    return result;
}

void miniFS_prompt()
{
    bool allive_status = false;
    allive_status = get_mountStatus();
    if (allive_status)
    {
        string fsName;
        string fsDir;
        fsName = get_sysName();
        fsDir = get_path();
        cout << "miniFS (" + fsName + ": " + fsDir + ")" + ">>> ";
    }
    else
    {
        cout<<"miniFS (no mount) >>> ";
    }
}


string getOrder()
{
    string order;
    getline(cin, order);
    return order;
}

void orderPerform(string order)
{
    vector<string> v = split(order, " "); //可按多个字符来分隔;
    regex reg_order0("^show|fmt|dr|close|test|help|att|tree|look|opt$");
    regex reg_order1("^create|mount|cd|mkdir|new|dl|map|read|tp$");
    regex reg_order2("^mv|cp$");
    if (regex_match(v[0], reg_order0))
    {
        if (v.size() != 1) {
            DEBUG_order
        }
        else {
            order1_map[v[0]]();
        }
    }
    else if (regex_match(v[0], reg_order1))
    {
        if (v.size() != 2) {
            DEBUG_order
        }
        else if (v.size() == 2){
            order2_map[v[0]](v[1]);
        }
    }
    else if (regex_match(v[0], reg_order2))
    {
        if (v.size() != 4) {
            DEBUG_order
        }
        else if (v.size() == 4) {
            order3_map[v[0]](v[1], v[2], v[3]);
        }
    }
    else {
        cout << "no such order, please retry, or input \"help\" to check orders." << endl;
    }
}

void helpSys()
{
    cout<<endl;
    cout<<"命令             说明                                           示例"<<endl;
    cout<<"create      创建一个miniFS系统                            create <test.minifs>"<<endl;
    cout<<"mount       挂载一个miniFS系统                            mount <test.minifs>"<<endl;
    cout<<"fmt         格式化一个miniFS系统                          fmt"<<endl;
    cout<<"show        展示已经存在的miniFS系统                      show"<<endl;
    cout<<"att         显示miniFS系统属性                            att"<<endl;
    cout<<"close       关闭挂载的miniFS系统                          close"<<endl;
    cout<<"dr          显示当前目录下的目录和文件                     dr"<<endl;
    cout<<"mkdir       新建一个目录                                  mkdir <test>"<<endl;
    cout<<"cd          更改当前目录                                  cd <..>  or  cd <./>  or  cd </test>"<<endl;
    cout<<"new         新建一个文本文件                              new <test.txt>"<<endl;
    cout<<"read        显示一个文本文件内容                          read <test.txt>"<<endl;
    cout<<"tp          通配显示文件或者目录                          tp" <<endl;
    cout<<"dl          删除一个目录或文件                            dl <test>  or  dl <test.txt>"<<endl;
    cout<<"look        查看windows下的文本文件                       look                    "<<endl;
    cout<<"cp          将指定路径的文件或目录复制到指定路径           cp -t <s1> <s2> 空间内部复制"<<endl;
    cout<<"                                                          cp -i <s1> <s2> windows下复制到空间"<<endl;
    cout<<"                                                          cp -o <s1> <s2> 空间复制到windows下"<<endl;
    cout<<"mv          将指定路径的文件或目录移动到指定路径           mv -t <s1> <s2> 空间内部移动"<<endl;
    cout<<"                                                          mv -o <s1> <s2> 空间移动到windows下"<<endl;
    cout<<"map         显示指定文件在空间所占用的全部块的块号         map <test.txt>"<<endl;
    cout<<"tree        以树型方式显示文件目录和文件                   tree"<<endl;
    cout<<"opt         优化整个空间，将每个文件尽可能连续存放(使用连续的块号存储文件内容) "<<endl;
}