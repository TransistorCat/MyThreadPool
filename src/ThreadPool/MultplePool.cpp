#include "MultplePool.h"
#include <algorithm>
#include <cstddef>
MultplePool::MultplePool(size_t numThreads,
                         std::unique_ptr<ITaskQueue> taskQueue)
    : numThreads{numThreads} {
  auto worker = [this](size_t id) {
    while (true) {
      std::function<void()> task = this->taskQueues[id]->pop();
      if (task)
        task();
      else
        break;
    }
  };
  for (size_t i = 0; i < numThreads; ++i) {
    taskQueues.emplace_back(std::move(taskQueue->clone()));
    workers.emplace_back(worker, i);
  }
}

auto MultplePool::submit(std::function<void()> task) -> std::future<void> {
  auto packagedTask = std::make_shared<std::packaged_task<void()>>(task);
  std::future<void> result = packagedTask->get_future();
  int id = rand() % numThreads;
  this->taskQueues[id]->push([packagedTask]() { (*packagedTask)(); });
  return result;
}

MultplePool::~MultplePool() {
  for (size_t i = 0; i < numThreads; ++i) {
    taskQueues[i]->stop();
  }

  for (std::thread &worker : workers) {
    worker.join();
  }
}