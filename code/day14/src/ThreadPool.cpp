#include "ThreadPool.h"
#include <stdexcept>

ThreadPool::ThreadPool(int size)
{
    for (int i = 0; i < size; ++i)
    {
        threads_.emplace_back(std::thread([this](){
            while (true)
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(tasks_mtx_);
                    cv_.wait(lock, [this]()
                    {
                        return stop_ || !tasks_.empty();
                    });

                    if (stop_ && tasks_.empty()) return;
                    task = tasks_.front();
                    tasks_.pop();
                }
                task();
            }
        }));
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(tasks_mtx_);
        stop_ = true;
    }

    cv_.notify_all();  // 唤醒所有线程，让它们退出
    for (std::thread& th : threads_)
    {
        if (th.joinable()) th.join();
    }
}

//void ThreadPool::add(std::function<void()> func)
//{
//    {
//        std::unique_lock<std::mutex> lock(m_tasks_mtx);
//        if (m_stop)
//        {
//            throw std::runtime_error("ThreadPool already stop, can't add task any more");
//        }
//        m_tasks.emplace(func);
//    }
//    m_cv.notify_one();
//}