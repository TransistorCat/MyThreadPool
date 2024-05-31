#include "UnSafeQueue.h"

#include <algorithm>
#include <functional>

UnSafeQueue::UnSafeQueue() {}

void UnSafeQueue::push(const std::function<void()> &task) {
  std::unique_lock lock(mutex);
  tasks.push(task);
  return;
}

std::function<void()> UnSafeQueue::pop() {
  std::unique_lock lock(mutex);
  if (tasks.empty() && is_stop) // 有问题
    return nullptr;
  auto task = std::move(tasks.front());
  tasks.pop();
  return task;
}

bool UnSafeQueue::empty() {
  std::unique_lock lock(mutex);
  return tasks.empty();
}

size_t UnSafeQueue::size() {
  std::unique_lock lock(mutex);
  return tasks.size();
}

UnSafeQueue::~UnSafeQueue() {
  std::unique_lock lock(mutex);
  while (!tasks.empty()) {
    tasks.pop();
  }
}

void UnSafeQueue::stop() {
  std::unique_lock lock(mutex);
  is_stop = true;
}

std::unique_ptr<ITaskQueue> UnSafeQueue::clone() const {
  return std::make_unique<UnSafeQueue>();
}