#ifndef SIMPLE_QUEUE_H
#define SIMPLE_QUEUE_H

#include "ITaskQueue.h"
#include <condition_variable>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>

class SimpleDeque : public ITaskQueue {
public:
  SimpleDeque();
  void push(const std::function<void()> &task) override;
  std::function<void()> pop() override;
  bool empty() override;
  size_t size() override;
  void stop() override;
  bool running() override;
  ~SimpleDeque() override;
  std::unique_ptr<ITaskQueue> clone() const override;

private:
  std::deque<std::function<void()>> tasks;
  std::mutex front_mutex;
  std::mutex back_mutex;
  std::condition_variable condition;
  bool is_stop;
};

#endif // SIMPLE_QUEUE_H
