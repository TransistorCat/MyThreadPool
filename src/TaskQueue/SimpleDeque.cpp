#include "SimpleDeque.h"

SimpleDeque::SimpleDeque() : is_stop(false) {}

void SimpleDeque::push(const std::function<void()> &task) {
  // 尝试获取后端锁
  std::unique_lock front_lock(front_mutex, std::defer_lock);
  std::unique_lock back_lock(back_mutex, std::defer_lock);

  if (back_lock.try_lock()) {
    tasks.push_back(task);
    condition.notify_one();
    return;
  }

  // 尝试获取前端锁
  if (front_lock.try_lock()) {
    tasks.push_front(task);
    condition.notify_one();
    return;
  }

  // 如果前后端锁都未获取到，等待直到有一个锁可用
  {
    std::unique_lock<std::mutex> lock(back_mutex);
    tasks.push_back(task);
  }
  condition.notify_one();
}

std::function<void()> SimpleDeque::pop() {
  std::function<void()> task;
  std::unique_lock front_lock(front_mutex, std::defer_lock);
  std::unique_lock back_lock(back_mutex, std::defer_lock);

  if (front_lock.try_lock()) {
    // 尝试从前端获取任务
    if (!tasks.empty()) {
      task = std::move(tasks.front());
      tasks.pop_front();
      return task;
    }
    front_lock.unlock();
  }

  if (back_lock.try_lock()) {
    // 如果前端没有获取到任务，尝试从后端获取
    if (!tasks.empty()) {
      task = std::move(tasks.back());
      tasks.pop_back();
      return task;
    }
    back_lock.unlock();
  }

  // 如果前后都没有获取到任务，等待条件变量
  std::unique_lock lock(front_mutex);
  condition.wait(lock, [this] { return !tasks.empty() || is_stop; });

  if (tasks.empty()) // 没有任务且停止
    return nullptr;

  task = std::move(tasks.front());
  tasks.pop_front();
  return task;
}

bool SimpleDeque::empty() {
  std::scoped_lock lock(front_mutex, back_mutex);
  return tasks.empty();
}

size_t SimpleDeque::size() {
  std::scoped_lock lock(front_mutex, back_mutex);
  return tasks.size();
}

SimpleDeque::~SimpleDeque() {
  stop();
  std::scoped_lock lock(front_mutex, back_mutex);
  while (!tasks.empty()) {
    tasks.pop_front();
  }
}

bool SimpleDeque::running() {
  std::scoped_lock lock(front_mutex, back_mutex);
  return !is_stop;
}

void SimpleDeque::stop() {
  {
    std::scoped_lock lock(front_mutex, back_mutex);
    is_stop = true;
  }
  condition.notify_all();
}

std::unique_ptr<ITaskQueue> SimpleDeque::clone() const {
  return std::make_unique<SimpleDeque>();
}
