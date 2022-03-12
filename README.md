项目描述：本项目是基于C++泛型编程实现的一个轻量级跳表。完成了插入，检索，删除，
整齐打印键值等功能。并且编写了测试脚本，分别测试了多线程和单线程的插入、查找的性能。跳表核心实现在mySkipList.h文件中，中文注释详细，容易理解。

项目运行方法：终端打开项目目录
输入make
输入./bin/main
测试脚本运行方法：./test.sh
测试脚本测试了插入10W个数据所需的时间，经过测试大致性能如下：
插入10W数据耗时：1350ms左右，查找10W条数据耗时450ms左右。

提供接口：
    search_element(K);//查找功能
    delete_element(K);//删除功能
    insert_element(K, V);//插入键值，成功返回1，失败返回0
    create_Node(K, V, int);//创立一个新节点的封装函数，留给外界简单接口

    get_random_level();//利用随机数返回一个新节点的层数(需要建立的索引数目)
    print_key();//仅仅打印跳表的key便于观察结构，目前仅支持key为int类型
    display_list();//打印跳表数据
    size();//返回元素数目
    dump_flie();//存储跳表
    load_file();//加载跳表
    clear();//清除跳表所有数据
本项目参考了卡哥的项目：https://github.com/youngyangyang04/Skiplist-CPP

