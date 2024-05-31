#include "ITaskQueue.h"
#include <condition_variable>
#include <mutex>
#include <queue>

class UnSafeQueue : public ITaskQueue {
public:
  UnSafeQueue();
  void push(const std::function<void()> &task) override;
  std::function<void()> pop() override;
  bool empty() override;
  size_t size() override;
  void stop() override;
  std::unique_ptr<ITaskQueue> clone() const override;
  ~UnSafeQueue();

private:
  std::queue<std::function<void()>> tasks;
  bool is_stop = false;
  std::mutex mutex;
};