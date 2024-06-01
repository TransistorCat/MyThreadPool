#include "ITaskQueue.h"
#include <condition_variable>
#include <mutex>
#include <queue>

class SimpleQueue : public ITaskQueue {
public:
  SimpleQueue();
  void push(const std::function<void()> &task);
  std::function<void()> pop();
  bool empty();
  size_t size();
  void stop();
  bool running();
  ~SimpleQueue();
  std::unique_ptr<ITaskQueue> clone() const;

private:
  std::queue<std::function<void()>> tasks;
  std::mutex mutex;
  std::condition_variable condition;
  bool is_stop;
};
