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


//�ָ��
vector<string> split(const string &s, const string &seperator) {
    vector<string> result;
    typedef string::size_type string_size;
    string_size i = 0;

    while (i != s.size()) {
        //�ҵ��ַ������׸������ڷָ�������ĸ��
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

        //�ҵ���һ���ָ������������ָ���֮����ַ���ȡ����
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
    vector<string> v = split(order, " "); //�ɰ�����ַ����ָ�;
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
    cout<<"����             ˵��                                           ʾ��"<<endl;
    cout<<"create      ����һ��miniFSϵͳ                            create <test.minifs>"<<endl;
    cout<<"mount       ����һ��miniFSϵͳ                            mount <test.minifs>"<<endl;
    cout<<"fmt         ��ʽ��һ��miniFSϵͳ                          fmt"<<endl;
    cout<<"show        չʾ�Ѿ����ڵ�miniFSϵͳ                      show"<<endl;
    cout<<"att         ��ʾminiFSϵͳ����                            att"<<endl;
    cout<<"close       �رչ��ص�miniFSϵͳ                          close"<<endl;
    cout<<"dr          ��ʾ��ǰĿ¼�µ�Ŀ¼���ļ�                     dr"<<endl;
    cout<<"mkdir       �½�һ��Ŀ¼                                  mkdir <test>"<<endl;
    cout<<"cd          ���ĵ�ǰĿ¼                                  cd <..>  or  cd <./>  or  cd </test>"<<endl;
    cout<<"new         �½�һ���ı��ļ�                              new <test.txt>"<<endl;
    cout<<"read        ��ʾһ���ı��ļ�����                          read <test.txt>"<<endl;
    cout<<"tp          ͨ����ʾ�ļ�����Ŀ¼                          tp" <<endl;
    cout<<"dl          ɾ��һ��Ŀ¼���ļ�                            dl <test>  or  dl <test.txt>"<<endl;
    cout<<"look        �鿴windows�µ��ı��ļ�                       look                    "<<endl;
    cout<<"cp          ��ָ��·�����ļ���Ŀ¼���Ƶ�ָ��·��           cp -t <s1> <s2> �ռ��ڲ�����"<<endl;
    cout<<"                                                          cp -i <s1> <s2> windows�¸��Ƶ��ռ�"<<endl;
    cout<<"                                                          cp -o <s1> <s2> �ռ临�Ƶ�windows��"<<endl;
    cout<<"mv          ��ָ��·�����ļ���Ŀ¼�ƶ���ָ��·��           mv -t <s1> <s2> �ռ��ڲ��ƶ�"<<endl;
    cout<<"                                                          mv -o <s1> <s2> �ռ��ƶ���windows��"<<endl;
    cout<<"map         ��ʾָ���ļ��ڿռ���ռ�õ�ȫ����Ŀ��         map <test.txt>"<<endl;
    cout<<"tree        �����ͷ�ʽ��ʾ�ļ�Ŀ¼���ļ�                   tree"<<endl;
    cout<<"opt         �Ż������ռ䣬��ÿ���ļ��������������(ʹ�������Ŀ�Ŵ洢�ļ�����) "<<endl;
}