#include "pch.h"
#include "console.h"

bool keepAlive = true;

int main()
{
    cout << "��ӭ����miniFSϵͳ (mini-FS version 2.0)" << endl;
    cout << "Type \"help\" for more information.\n" << endl;
    while (keepAlive)
    {
        miniFS_prompt();
        string order;
        order = getOrder();
        if (order == "exit")
            break;
        orderPerform(order);
    }
    return 1;
}