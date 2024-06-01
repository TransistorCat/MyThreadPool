#include "ThreadLocalPool.h"
#include <cstddef>
#include <memory>
#include <thread>

// template <class F, class... Args>
// auto ThreadLocalPool::submit(F &&f, Args &&...args)
//     -> std::future<typename std::result_of<F(Args...)>::type> {
auto ThreadLocalPool::submit(std::function<void()> task) -> std::future<void> {
  auto packagedTask = std::make_shared<std::packaged_task<void()>>(task);
  std::future<void> result = packagedTask->get_future();
  int id = rand() % numThreads;
  this->localQueues[id]->push([packagedTask]() { (*packagedTask)(); });
  return result;
}
thread_local std::shared_ptr<ITaskQueue> ThreadLocalPool::localQueue = nullptr;
ThreadLocalPool::ThreadLocalPool(size_t numThreads,
                                 std::unique_ptr<ITaskQueue> taskQueue)
    : numThreads(numThreads), globalQueue{std::move(taskQueue)} {
  localQueues.resize(numThreads);

  for (size_t i = 0; i < numThreads; ++i) {
    localQueues[i] = globalQueue->clone();
  }
  for (size_t i = 0; i < numThreads; ++i) {

    workers.emplace_back([this, i]() {
      localQueue = localQueues[i];
      while (true) {
        std::function<void()> task = nullptr;
        if (!localQueue->empty()) {
          task = localQueue->pop();
        }

        if (localQueue->empty() && !localQueue->running() && !task) {
          break;
        }
        if (!task)
          task = this->stealTask();
        if (task) {
          task();
        }
      }
    });
  }
}

std::function<void()> ThreadLocalPool ::stealTask() {

  for (auto &queue : localQueues) {
    if (!queue->empty()) {
      return queue->pop();
    }
  }
  return nullptr;
}
ThreadLocalPool::~ThreadLocalPool() {
  for (size_t i = 0; i < numThreads; ++i) {
    localQueues[i]->stop();
  }
  for (std::thread &worker : workers) {
    worker.join();
  }
}