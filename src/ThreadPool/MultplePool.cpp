#include "MultplePool.h"
#include <algorithm>
#include <cstddef>
#include <iostream>
MultplePool::MultplePool(size_t numThreads,
                         std::unique_ptr<ITaskQueue> taskQueue)
    : numThreads{numThreads} {
  auto worker = [this](size_t id) {
    while (true) {
      std::function<void()> task = this->taskQueues[id]->pop();
      // std::cout << "id: " << id << " run" << std::endl;
      if (task)
        task();
      if (taskQueues[id]->empty() && !taskQueues[id]->running() && !task) {
        break;
      }
    }
  };
  for (size_t i = 0; i < numThreads; ++i) {
    taskQueues.emplace_back(std::move(taskQueue->clone()));
  }
  for (size_t i = 0; i < numThreads; ++i) {
    workers.emplace_back(worker, i);
  }
}

auto MultplePool::submit(std::function<void()> task) -> std::future<void> {
  auto packagedTask = std::make_shared<std::packaged_task<void()>>(task);
  std::future<void> result = packagedTask->get_future();
  // int id = rand() % numThreads;
  // this->taskQueues[id]->push([packagedTask]() { (*packagedTask)(); });
  this->taskQueues[nextQueue]->push([packagedTask]() { (*packagedTask)(); });
  nextQueue = (nextQueue + 1) % numThreads; // 轮询效果更好
  return result;
}

MultplePool::~MultplePool() {
  // std::cout << "start destruct MultplePool" << std::endl;
  for (size_t i = 0; i < numThreads; ++i) {
    taskQueues[i]->stop();
  }

  for (std::thread &worker : workers) {
    worker.join();
  }
  // std::cout << "close MultplePool" << std::endl;
}