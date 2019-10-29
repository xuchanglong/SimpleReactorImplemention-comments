#ifndef intIME_HEAP
#define intIME_HEAP

#include <iostream>
#include <netinet/in.h>
#include <time.h>
using std::exception;

#define BUFFER_SIZE 64

class heap_timer;

/**
 * 用户数据，绑定 socket 和定时器。
*/
struct client_data
{
    sockaddr_in address;
    int sockfd;
    char buf[BUFFER_SIZE];
    heap_timer *timer;
};

/**
 * 定时器类。
*/
class heap_timer
{
public:
    heap_timer(int delay)
    {
        expire = time(NULL) + delay;
    }

public:
    /**
     * 定时器生效的绝对时间。
    */
    time_t expire;
    void (*cb_func)(client_data *);
    client_data *user_data;
};

/**
 * 基于时间堆的高性能定时器。
*/
class time_heap
{
public:
    time_heap(int cap) 
        : capacity(cap), cur_size(0)
    {
        array = new heap_timer *[capacity];
        if (!array)
        {
            throw std::exception();
        }
        for (int i = 0; i < capacity; ++i)
        {
            array[i] = NULL;
        }
    }
    time_heap(heap_timer **init_array, int size, int capacity) 
        : cur_size(size), capacity(capacity)
    {
        if (capacity < size)
        {
            throw std::exception();
        }
        array = new heap_timer *[capacity];
        if (!array)
        {
            throw std::exception();
        }
        for (int i = 0; i < capacity; ++i)
        {
            array[i] = NULL;
        }
        if (size != 0)
        {
            for (int i = 0; i < size; ++i)
            {
                array[i] = init_array[i];
            }
            for (int i = (cur_size - 1) / 2; i >= 0; --i)
            {
                percolate_down(i);
            }
        }
    }
    ~time_heap()
    {
        for (int i = 0; i < cur_size; ++i)
        {
            delete array[i];
        }
        delete[] array;
    }

public:
    void add_timer(heap_timer *timer) 
    {
        if (!timer)
        {
            return;
        }
        if (cur_size >= capacity)
        {
            resize();
        }
        int hole = cur_size++;
        int parent = 0;
        for (; hole > 0; hole = parent)
        {
            parent = (hole - 1) / 2;
            if (array[parent]->expire <= timer->expire)
            {
                break;
            }
            array[hole] = array[parent];
        }
        array[hole] = timer;
    }
    void del_timer(heap_timer *timer)
    {
        if (!timer)
        {
            return;
        }
        // lazy delelte
        timer->cb_func = NULL;
    }
    heap_timer *top() const
    {
        if (empty())
        {
            return NULL;
        }
        return array[0];
    }
    void pop_timer()
    {
        if (empty())
        {
            return;
        }
        if (array[0])
        {
            delete array[0];
            array[0] = array[--cur_size];
            percolate_down(0);
        }
    }
    void tick()
    {
        heap_timer *tmp = array[0];
        time_t cur = time(NULL);
        while (!empty())
        {
            if (!tmp)
            {
                break;
            }
            if (tmp->expire > cur)
            {
                break;
            }
            if (array[0]->cb_func)
            {
                array[0]->cb_func(array[0]->user_data);
            }
            pop_timer();
            tmp = array[0];
        }
    }
    bool empty() const { return cur_size == 0; }

private:
    void percolate_down(int hole)
    {
        heap_timer *temp = array[hole];
        int child = 0;
        for (; ((hole * 2 + 1) <= (cur_size - 1)); hole = child)
        {
            child = hole * 2 + 1;
            if ((child < (cur_size - 1)) && (array[child + 1]->expire < array[child]->expire))
            {
                ++child;
            }
            if (array[child]->expire < temp->expire)
            {
                array[hole] = array[child];
            }
            else
            {
                break;
            }
        }
        array[hole] = temp;
    }
    void resize() 
    {
        heap_timer **temp = new heap_timer *[2 * capacity];
        for (int i = 0; i < 2 * capacity; ++i)
        {
            temp[i] = NULL;
        }
        if (!temp)
        {
            throw std::exception();
        }
        capacity = 2 * capacity;
        for (int i = 0; i < cur_size; ++i)
        {
            temp[i] = array[i];
        }
        delete[] array;
        array = temp;
    }

private:
    /**
     * 堆数组。
    */
    heap_timer **array;

    /**
     * 堆数组容量。
    */
    int capacity;

    /**
     * 堆数组当前包含的元素的个数。
    */
    int cur_size;
};

#endif
