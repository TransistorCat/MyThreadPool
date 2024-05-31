#ifndef ITASKQUEUE_H
#define ITASKQUEUE_H
#include <functional>
#include <memory>

class ITaskQueue {
public:
  virtual ~ITaskQueue() = default;
  virtual void push(const std::function<void()> &task) = 0;
  virtual std::function<void()> pop() = 0;
  virtual bool empty() = 0;
  virtual size_t size() = 0;
  virtual void stop() = 0;
  virtual std::unique_ptr<ITaskQueue> clone() const = 0;
};
#endif