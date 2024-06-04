#include "SimplePool.h"
#include <cstddef>
#include <memory>

// template <class F, class... Args>
// auto SimplePool::submit(F &&f, Args &&...args)
//     -> std::future<typename std::result_of<F(Args...)>::type> {
auto SimplePool::submit(std::function<void()> task) -> std::future<void> {
  auto packagedTask = std::make_shared<std::packaged_task<void()>>(task);
  std::future<void> result = packagedTask->get_future();
  this->taskQueue->push([packagedTask]() { (*packagedTask)(); });
  return result;
}

SimplePool::SimplePool(size_t numThreads, std::unique_ptr<ITaskQueue> taskQueue)
    : taskQueue{std::move(taskQueue)} {
  for (size_t i = 0; i < numThreads; ++i) {
    workers.emplace_back([this]() {
      while (true) {
        std::function<void()> task = this->taskQueue->pop();
        if (task)
          task();
        if (this->taskQueue->empty() && !this->taskQueue->running() && !task)
          break;
      }
    });
  }
}

SimplePool::~SimplePool() {
  taskQueue->stop();
  for (std::thread &worker : workers) {
    worker.join();
  }
}