#include "ThreadPool.h"
#include <stdexcept>

ThreadPool::ThreadPool(int size)
{
    for (int i = 0; i < size; ++i)
    {
        m_threads.emplace_back(std::thread([this](){
            while (true)
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(m_tasks_mtx);
                    m_cv.wait(lock, [this]()
                    {
                        return m_stop || !m_tasks.empty();
                    });

                    if (m_stop && m_tasks.empty()) return;
                    task = m_tasks.front();
                    m_tasks.pop();
                }
                task();
            }
        }));
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(m_tasks_mtx);
        m_stop = true;
    }

    m_cv.notify_all();  // 唤醒所有线程，让它们退出
    for (std::thread& th : m_threads)
    {
        if (th.joinable()) th.join();
    }
}

void ThreadPool::add(std::function<void()> func)
{
    {
        std::unique_lock<std::mutex> lock(m_tasks_mtx);
        if (m_stop)
        {
            throw std::runtime_error("ThreadPool already stop, can't add task any more");
        }
        m_tasks.emplace(func);
    }
    m_cv.notify_one();
}