#include "../ITaskQueue.h"
#include <condition_variable>
#include <mutex>
#include <queue>

class SimpleQueue : public ITaskQueue {
public:
  void push(const std::function<void()> &task);
  std::function<void()> pop();
  bool empty() const;
  size_t size() const;
  void stop();
  ~SimpleQueue();

private:
  std::queue<std::function<void()>> tasks;
  std::mutex mutex;
  std::condition_variable condition;
  bool is_stop;
};
