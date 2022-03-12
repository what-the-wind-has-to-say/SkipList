#include "../mySkipList.h"
#include<time.h>
#include<cstdlib>
#include<chrono>
#include<pthread.h>
using namespace std::chrono;
//精确计时
class Timer
{
public:
    Timer() : m_begin(high_resolution_clock::now()) {}
    void reset() { m_begin = high_resolution_clock::now(); }
    //默认输出毫秒
    int64_t elapsed() const
    {
        return duration_cast<chrono::milliseconds>(high_resolution_clock::now() - m_begin).count();
    }
    //微秒
    int64_t elapsed_micro() const
    {
        return duration_cast<chrono::microseconds>(high_resolution_clock::now() - m_begin).count();
    } 
    //纳秒
    int64_t elapsed_nano() const
    {
        return duration_cast<chrono::nanoseconds>(high_resolution_clock::now() - m_begin).count();
    }
    //秒
    int64_t elapsed_seconds() const
    {
        return duration_cast<chrono::seconds>(high_resolution_clock::now() - m_begin).count();
    }
    //分
    int64_t elapsed_minutes() const
    {
        return duration_cast<chrono::minutes>(high_resolution_clock::now() - m_begin).count();
    }
    //时
    int64_t elapsed_hours() const
    {
        return duration_cast<chrono::hours>(high_resolution_clock::now() - m_begin).count();
    }
private:
    time_point<high_resolution_clock> m_begin;
};
#include<thread>
using namespace std;
void test_single_thread(int n)//单线程连续插入并查找n个数据，返回所需时间(ms)
{
    Timer t;
    mySkipList<int,string>list(24);
    for(int i = 0;i < n;i++)
    {
        list.insert_element(i,"value");
    }
    int64_t insert_time = t.elapsed();
    
    t.reset();
    for(int i = 0;i < n;i++)
    {
        list.search_element(i);
    }
    int64_t search_time = t.elapsed();
    cout << "insert " << n << " elements: " << insert_time << "ms" << endl;
    cout << "search " << n << " elements: " << search_time << "ms" << endl;
}

mySkipList<int,string>test_list(24);
void insert_element(int start, int end)
{
    for(int i = start;i < end;i++)
    {
        test_list.insert_element(i,"value");
    }
}

void search_element(int start, int end)
{
    for(int i = start;i < end;i++)
    {
        test_list.search_element(i);
    }
}

void test_thread(int n, int thread_num)//启动thread_num个线程插入连续的n个数据，返回所需时间(ms)
{
    Timer t;
    vector<thread>threads;
    int average = n/thread_num;
    int l = 0;
    int r = 0;
    for(int i = 0;i < thread_num;i++)
    {
        l = r;
        r += average;
        if(i == thread_num - 1)
        {
            threads.push_back(thread(insert_element,l,n));
        }
        else
        {
            threads.push_back(thread(insert_element,l,r));
        }
    }
    for(auto& t : threads)
    {
        t.join();
    }
    int64_t insert_time = t.elapsed();
    t.reset();
    threads.clear();
    l = r = 0;
    for(int i = 0;i < thread_num;i++)
    {
        l = r;
        r += average;
        if(i == thread_num - 1)
        {
            threads.push_back(thread(search_element,l,n));
        }
        else
        {
            threads.push_back(thread(search_element,l,r));
        }
    }
    for(auto& t : threads)
    {
        t.join();
    }
    int64_t search_time = t.elapsed();
    cout << "insert " << n << " elements: " << insert_time << "ms" << endl;
    cout << "search " << n << " elements: " << search_time << "ms" << endl;
}

int main()
{
    // test_single_thread(100000);
    test_list.clear();
    test_thread(100000,10);//由于insert操作加锁，所以速度提升不明显
    return 0;
}

