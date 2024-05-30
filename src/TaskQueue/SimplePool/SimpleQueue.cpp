#include "SimpleQueue.h"

#include <algorithm>
#include <functional>
#include <mutex>

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
  condition.wait(lock, [this] { return !tasks.empty() || !is_stop; });
  auto task = std::move(tasks.front());
  tasks.pop();
  return task;
}

bool SimpleQueue::empty() const {
  std::scoped_lock lock(mutex);
  return tasks.empty();
}

size_t SimpleQueue::size() const {
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
  {
    std::unique_lock<std::mutex> lock(mutex);
    is_stop = true;
  }
  condition.notify_all();
}
