#pragma once

#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

class ThreadPool
{
public:
    ThreadPool(int size = std::thread::hardware_concurrency());
    ~ThreadPool();

    //void add(std::function<void()>);
    template<class F, class... Args>
    auto add(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>;
private:
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_tasks_mtx;
    std::condition_variable m_cv;
    bool m_stop;
};

// 不能放在cpp文件，原因是C++编译器不支持模板的分离编译
template<class F, class... Args>
auto ThreadPool::add(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(m_tasks_mtx);

        // don't allow enqueueing after stopping the pool
        if (m_stop) throw std::runtime_error("enqueue on stopped ThreadPool");

        m_tasks.emplace([task](){ (*task)(); });
    }
    m_cv.notify_one();
    return res;
}