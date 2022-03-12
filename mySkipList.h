#ifndef __MYSKIPLIST__
#define __MYSKIPLIST__
#include<iostream>
#include<vector>
#include<string>
#include<pthread.h>
#include<mutex>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include<map>
#include<bits/stdc++.h>
using namespace std;

#define STORE_FILE "data/dumpfile.txt"//储存数据的路径
mutex mtx;//锁 为了读写安全

template<typename K,typename V>
class Node//定义跳表变量
{
public:
    Node();
    Node(K k, V v, int);//key value level
    // ~Node();
    K get_key() const;
    V get_value() const;
    void set_value(V value) const;

    vector<Node<K,V>*>forward;//存放下一个key的所有索引和其数据的指针
    // Node<K,V>** forward;//指向一个存放Node指针类型的数组的指针
    int node_level;//表示当前Node索引一共有多少层，数据存放在第0层
private:
    K key;
    V value;
};

template<typename K,typename V>
Node<K,V>::Node(K k, V v, int level):key(k),value(v),node_level(level)
{
    forward = vector<Node<K,V>*>(node_level + 1,NULL);//指向下一个key的指针数组初始为NULL
}

template<typename K,typename V>
K Node<K,V>::Node::get_key() const
{
    return key;
}

template<typename K,typename V>
V Node<K,V>::Node::get_value() const
{
    return value;
}

template<typename K,typename V>
void Node<K,V>::Node::set_value(V value) const
{
    this->value = value;
}

template<typename K,typename V>
class mySkipList
{
public:
    mySkipList(int);//构造函数设置跳表最大索引层数

    Node<K,V>* search_element(K);//查找功能
    void delete_element(K);//删除功能
    int insert_element(K, V);//插入键值，成功返回1，失败返回0
    Node<K,V>* create_Node(K, V, int);//创立一个新节点的封装函数，留给外界简单接口

    int get_random_level();//利用随机数返回一个新节点的层数(需要建立的索引数目)
    void print_key();//仅仅打印跳表的key便于观察结构，目前仅支持key为int类型
    void display_list();//打印跳表数据
    int size();//返回元素数目
    void dump_flie();//存储跳表
    void load_file();//加载跳表
    void clear();//清除跳表所有数据

private:
    Node<K,V>* head;//头指针

    int max_level;//跳表最大索引层数
    int skip_list_level;//跳表当前索引层数
    int element_count;//跳表存储数据数目
};

template<typename K,typename V>
mySkipList<K, V>::mySkipList(int _max_level):max_level(_max_level)
{
    srand(time(NULL));
    skip_list_level = 0;
    element_count = 0;
    K k;
    V v;
    head = new Node<K,V>(k, v, max_level);//初始化头结点
}

template<typename K,typename V>
Node<K,V>* mySkipList<K,V>:: search_element(K key)
{
    Node<K,V>* cur = head;
    //从顶层开始向下查找
    for(int i = skip_list_level;i >= 0;i--)
    {
        while(cur->forward[i] != NULL && cur->forward[i]->get_key() < key)
        {
            cur = cur->forward[i];//右侧不为空，且key小于目标key，向右查找
        }
    }
    cur = cur->forward[0];
    if(cur != NULL && cur->get_key()  != key)
    {
        cout << "Not Found key" << key << endl;
    }
    else
    {
        cout << "Found key " << key << endl;
    }
    return cur;
}

template<typename K,typename V>
void mySkipList<K,V>:: delete_element(K key)
{
    mtx.lock();
    Node<K,V>* cur = head;
    vector<Node<K,V>*>update(max_level + 1,NULL);//记录需要删除节点的前驱动节点
    //从顶层开始向下查找
    for(int i = skip_list_level;i >= 0;i--)
    {
        while(cur->forward[i] != NULL && cur->forward[i]->get_key()  < key)
        {
            cur = cur->forward[i];
        }
        update[i] = cur;
    }
    cur = cur->forward[0];
    if(cur != NULL && cur->get_key()  != key)
    {
        cout << "Not Found key " << key << endl;
        mtx.unlock();//解锁
        return;
    }
    //找到了对应的key，则从底向上开始删除
    for(int i = 0;i < update.size();i++)
    {
        if(update[i] == NULL || update[i]->forward[i] != cur)break;//如果后一个节点不再是需要删除的节点，跳出循环
        update[i]->forward[i] = cur->forward[i];//删除节点
    }
    //删除完成之后判断是否存在只剩下头结点的层数
    while(skip_list_level > 0 && head->forward[skip_list_level] == NULL)
    {
        skip_list_level--;//当前索引层数减1
    }
    cout << "Successfully deleted key "<< key << endl;
    element_count--;//计数减1
    delete cur;
    mtx.unlock();//解锁
}

template<typename K,typename V>
int mySkipList<K,V>:: insert_element(K key,V value)
{
    mtx.lock();//加锁
    Node<K,V>* cur = head;
    vector<Node<K,V>*>update(max_level + 1,NULL);//记录前驱节点
    //仍然从上往下查找，为了获得节点的前驱信息
    for(int i = skip_list_level;i >= 0;i--)
    {
        while(cur->forward[i] != NULL && cur->forward[i]->get_key()  < key)
        {
            cur = cur->forward[i];
        }
        update[i] = cur;
    }
    cur = cur->forward[0];
    if(cur != NULL && cur->get_key() == key)//待插入节点已经存在
    {
        cout << "key " << key << " already exists" << endl;
        mtx.unlock();//解锁
        return 0;//插入失败
    }
    //通过随机数判断需要建立的索引是多少层
    mtx.unlock();
    int random_num = get_random_level();
    Node<K,V>* new_Node = create_Node(key, value, random_num);//新建立节点
    mtx.lock();
    for(int i = 0;i < random_num;i++)
    {
        
        if(i + 1 > skip_list_level)//建立索引层数加数据层大于当前层数
        {
            update[i] = head;//新建立的索引的前驱节点均为头节点
            skip_list_level = i + 1;
        }
        new_Node->forward[i] = update[i]->forward[i];//新节点的下一个key赋值为前驱节点的下一个key
        update[i]->forward[i] = new_Node;//前驱节点的下一个key为新节点
    }
    cout << "Successfully inserted key: " << key << " value: " << value << endl;
    element_count++;
    mtx.unlock();//解锁
    return 1;//表示插入成功
}

template<typename K,typename V>
int mySkipList<K,V>::get_random_level()
{
    int level = 0;
    while(rand() % 2)
    {
        level++;
    }
    return level > max_level ? max_level + 1 : level + 1; //由于最底层数据必须插入，至少改动为1
}

template<typename K,typename V>
void mySkipList<K,V>::print_key()//打印整个key结构，目前仅仅支持key为整数的情况
{
    vector<vector<int> >vec(skip_list_level,vector<int>(element_count,-1));
    Node<K,V>* cur = head;
    //由于key值有可能遭到删除而不连续，需要建立一个key到index的映射
    map<K,int>mymap;
    cur = head->forward[0];//先指向第一个数据
    int index = 0;
    for(int i = 0;i < skip_list_level;i++)
    {
        cur = head->forward[i];
        index = 0;
        while(cur != NULL)
        {
            if(i == 0)//建立映射
            {
                mymap[cur->get_key()] = index;
                vec[i][index] = cur->get_key();
            }
            else
            {
                vec[i][mymap[cur->get_key()]] = cur->get_key();
            }
            index++;
            cur = cur->forward[i];
        }
    }
    //开始按行打印key
    for(int i = 0;i < skip_list_level;i++)
    {
        cout << "level " << i << ": ";
        for(int j = 0;j < element_count;j++)
        {
            string str;
            if(vec[i][j] == -1)//没有索引
            {
                cout << "     ";//5个空格保持对齐
            }
            else//在对齐的该处有索引需要打印key
            {
                cout << vec[i][j];
                str += "->";
                if(vec[i][j] < 10)
                {
                    str += "  ";
                }
                else if(vec[i][j] < 100)
                {
                    str += " ";
                }
//可以继续对齐，本例子只支持小于100的key
                {

                }
                cout << str;
            }
        }
        cout << endl;
    }
}

template<typename K,typename V>
void mySkipList<K,V>::display_list()
{
    Node<K,V>* cur = head;
    for(int i = 0;i < skip_list_level;i++)
    {
        cur = head->forward[i];
        cout << "level " << i << ":";
        while(cur != NULL)
        {
            cout << cur->get_key() << ":" << cur->get_value() << " ";
            cur = cur->forward[i];
        }
        cout << endl;
    }
}

template<typename K,typename V>
int mySkipList<K,V>::size()
{
    return element_count;
}

template<typename K,typename V>
Node<K,V>* mySkipList<K,V>::create_Node(const K k, const V v, const int level)
{
    Node<K,V>* new_node = new Node<K,V>(k, v, level);
    return new_node;
}

template<typename K,typename V>
void mySkipList<K,V>::dump_flie()//保存跳表数据
{
    cout << "dumpfile-------------------------" << endl;
    string path = STORE_FILE;
    ofstream out(path);
    Node<K,V>* cur = head->forward[0];
    while(cur != NULL)
    {
        out << cur->get_key() << " " << cur->get_value() << endl;//一行保存一个数据，以空格隔开，方便重新加载
        cur = cur->forward[0];
    }
    cout << "dumpfile-------------------------Successful" << endl;
    out.flush();
    out.close();
}

template<typename K,typename V>
void mySkipList<K,V>::load_file()//加载数据，重建跳表
{

    cout << "loading----------------------------------" << endl;
    string path = STORE_FILE;
    ifstream in(path);
    int key;
    string value;
    while((in >> key) && (in >> value))
    {
        this->insert_element(key,value);
    }
    cout << "loading------------------------------Successful" << endl;
    in.close();
}

template<typename K,typename V>
void mySkipList<K,V>::clear()
{
    Node<K,V>* cur = head;
    while(cur->forward[0] != NULL)
    {
        Node<K,V>*next = cur->forward[0];
        cur->forward.clear();
        cur = next;
    }
    skip_list_level = 0;
    element_count = 0;
    K k;
    V v;
    head = new Node<K,V>(k, v, max_level);
}
#endif