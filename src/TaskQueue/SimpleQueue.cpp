#include "SimpleQueue.h"

#include <algorithm>
#include <functional>
#include <mutex>

SimpleQueue::SimpleQueue() {}
void SimpleQueue::push(const std::function<void()> &task) {
  {
    std::scoped_lock lock(mutex);
    tasks.push(task);
  }
  condition.notify_one();
  return;
}

std::function<void()> SimpleQueue::pop() {
  std::unique_lock lock(mutex);
  condition.wait(lock, [this] { return !tasks.empty() || is_stop; });
  if (tasks.empty()) // 意味着当前stop且没任务了
    return nullptr;
  auto task = std::move(tasks.front());
  tasks.pop();
  return task;
}

bool SimpleQueue::empty() {
  std::scoped_lock lock(mutex);
  return tasks.empty();
}

size_t SimpleQueue::size() {
  std::scoped_lock lock(mutex);
  return tasks.size();
}

SimpleQueue::~SimpleQueue() {
  // 加锁以确保线程安全
  std::unique_lock<std::mutex> lock(mutex);

  // 清空任务队列中的所有任务
  while (!tasks.empty()) {
    tasks.pop();
  }
}

void SimpleQueue::stop() {
  std::unique_lock<std::mutex> lock(mutex);
  is_stop = true;
  condition.notify_all();
}

std::unique_ptr<ITaskQueue> SimpleQueue::clone() const {
  return std::make_unique<SimpleQueue>();
}