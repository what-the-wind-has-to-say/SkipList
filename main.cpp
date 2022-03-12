#include "mySkipList.h"
using namespace std;
int main()
{
    mySkipList<int,string>list(12);
    for(int i = 0;i <= 10;i++)
    {
        list.insert_element(i,"value");
    }
    cout << "before delete :" << endl;
    list.print_key();
    // list.display_list();
    list.delete_element(4);
    list.delete_element(8);
    cout << "after delete :" << endl;
    list.print_key();
    // list.display_list();
    list.dump_flie();
    mySkipList<int,string>list_02(12);
    list_02.load_file();
    list_02.print_key();
    // list_02.display_list();
    return 0;
}