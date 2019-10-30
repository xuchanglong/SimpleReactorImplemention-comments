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
        expire = time(nullptr) + delay;
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
    time_heap(size_t cap)
        : capacity(cap), cur_size(0)
    {
        array = new heap_timer *[capacity];
        if (array == nullptr)
        {
            throw std::exception();
        }
        for (size_t i = 0; i < capacity; ++i)
        {
            array[i] = nullptr;
        }
    }
    time_heap(heap_timer **init_array, size_t capacity, size_t size)
        : capacity(capacity), cur_size(size)
    {
        if (init_array == nullptr)
        {
            return;
        }

        if (capacity < size)
        {
            throw std::exception();
        }
        array = new heap_timer *[capacity];
        if (array == nullptr)
        {
            throw std::exception();
        }
        for (size_t i = 0; i < capacity; ++i)
        {
            array[i] = nullptr;
        }
        if (size != 0)
        {
            for (size_t i = 0; i < size; ++i)
            {
                array[i] = init_array[i];
            }
            for (size_t i = (cur_size - 1) / 2; i >= 0; --i)
            {
                percolate_down(i);
            }
        }
    }
    ~time_heap()
    {
        for (size_t i = 0; i < cur_size; ++i)
        {
            delete array[i];
            array[i] = nullptr;
        }
        delete[] array;
        array = nullptr;
    }

public:
    /**
     * @function    向最小堆中加入定时器，按照定时器生效的绝对时间对最小堆重构，
     *              使其满足最小堆的性质。
     * @paras   timer   被加入到最小堆的定时器。
     * @ret 0   操作成功。
     *      -1  形参为空。
    */
    int add_timer(heap_timer *timer)
    {
        if (timer == nullptr)
        {
            return -1;
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
        return 0;
    }

    /**
     * @function    从最小堆中删除定时器。
     * @paras   待删除的定时器。
     * @ret 0   操作成功。
     *      -1  形参为空。
    */
    int del_timer(heap_timer *timer)
    {
        if (timer == nullptr)
        {
            return -1;
        }
        // lazy delelte
        timer->cb_func = nullptr;
        return 0;
    }
    heap_timer *top() const
    {
        if (empty())
        {
            return nullptr;
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

    /**
     * 从时间堆中寻找到时间的节点。
    */
    void tick()
    {
        heap_timer *tmp = array[0];
        time_t cur = time(nullptr);
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
    /**
     * 对堆节点进行下滤，确保第 hole 节点满足最小堆性质。
    */
    void percolate_down(size_t hole)
    {
        heap_timer *temp = array[hole];
        size_t child = 0;
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

    /**
     * 空间不足时，将空间扩大为原来的 2 倍。
    */
    void resize()
    {
        heap_timer **temp = new heap_timer *[2 * capacity];
        for (size_t i = 0; i < 2 * capacity; ++i)
        {
            temp[i] = nullptr;
        }
        if (!temp)
        {
            throw std::exception();
        }
        capacity = 2 * capacity;
        for (size_t i = 0; i < cur_size; ++i)
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
    size_t capacity;

    /**
     * 堆数组当前包含的元素的个数。
    */
    size_t cur_size;
};

#endif
