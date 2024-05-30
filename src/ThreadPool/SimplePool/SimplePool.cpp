#include "SimplePool.h"
#include <cstddef>
#include <memory>

template <class F, class... Args>
auto SimplePool::submit(F &&f, Args &&...args)
    -> std::future<typename std::result_of<F(Args...)>::type> {
  using returnType = typename std::result_of<F(Args...)>::type;

  auto task = std::make_shared<std::packaged_task<returnType()>>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));
  std::future<returnType> result = task->get_future();

  taskQueue->push([task]() { (*task)(); });

  return result;
}

SimplePool::SimplePool(size_t numThreads, std::unique_ptr<ITaskQueue> taskQueue)
    : taskQueue{std::move(taskQueue)} {
  for (size_t i = numThreads; i < numThreads; ++i) {
    workers.emplace_back([this]() {
      while (true) {
        if (stop)
          break;
        std::function<void()> task = this->taskQueue->pop();
        if (task)
          task();
      }
    });
  }
}

SimplePool::~SimplePool() {
  std::unique_lock<std::mutex> lock();
  stop = true;
  for (std::thread &worker : workers) {
    worker.join();
  }
}