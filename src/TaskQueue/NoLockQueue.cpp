#include "NoLockQueue.h"
NoLockQueue::NoLockQueue()
    : head(new Node(std::function<void()>())), tail(head.load()),
      is_stop(false) {}

NoLockQueue::~NoLockQueue() {
  stop();
  while (Node *node = head.load()) {
    head.store(node->next);
    delete node;
  }
}

void NoLockQueue::push(const std::function<void()> &task) {
  Node *newNode = new Node(task);
  Node *oldTail = tail.exchange(newNode);
  oldTail->next.store(newNode);
  condition.notify_one();
}

std::function<void()> NoLockQueue::pop() {
  Node *oldHead = head.load();
  Node *next = oldHead->next.load();

  while (!next) {
    if (is_stop.load() && empty())
      return nullptr;
    next = oldHead->next.load();
  }

  head.store(next);
  std::function<void()> task = std::move(next->task);
  delete oldHead;
  return task;
}

bool NoLockQueue::empty() { return head.load() == tail.load(); }

size_t NoLockQueue::size() {
  size_t count = 0;
  Node *current = head.load();
  while (current != tail.load()) {
    count++;
    current = current->next.load();
  }
  return count;
}

void NoLockQueue::stop() {
  is_stop.store(true);
  condition.notify_all();
}

bool NoLockQueue::running() { return !is_stop.load(); }

std::unique_ptr<ITaskQueue> NoLockQueue::clone() const {
  return std::make_unique<NoLockQueue>();
}