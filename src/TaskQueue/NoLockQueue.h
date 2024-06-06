#include "ITaskQueue.h"
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <functional>
#include <memory>
#include <queue>
#include <thread>

// Node structure for the lock-free queue
struct Node {
  std::function<void()> task;
  std::atomic<Node *> next;

  Node(const std::function<void()> &t) : task(t), next(nullptr) {}
};

class NoLockQueue : public ITaskQueue {
public:
  NoLockQueue();
  ~NoLockQueue();

  void push(const std::function<void()> &task);
  std::function<void()> pop();
  bool empty();
  size_t size();
  void stop();
  bool running();
  std::unique_ptr<ITaskQueue> clone() const;

private:
  std::atomic<Node *> head;
  std::atomic<Node *> tail;
  std::atomic<bool> is_stop;
  std::condition_variable_any condition;
};