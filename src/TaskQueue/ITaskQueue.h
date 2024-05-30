#include <functional>

class ITaskQueue {
public:
  virtual ~ITaskQueue() = default;
  virtual void push(const std::function<void()> &task) = 0;
  virtual std::function<void()> pop() = 0;
  virtual bool empty() const = 0;
  virtual size_t size() const = 0;
};
