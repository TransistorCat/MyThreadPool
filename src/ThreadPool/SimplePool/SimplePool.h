#include "../../TaskQueue/ITaskQueue.h"
#include "../IThreadPool.h"
#include <functional>
#include <future>

class SimplePool : public IThreadPool {
public:
  ~SimplePool();
  SimplePool(size_t numThreads, std::unique_ptr<ITaskQueue> taskQueue);
  template <class F, class... Args>
  auto submit(F &&f, Args &&...args)
      -> std::future<typename std::result_of<F(Args...)>::type>;

private:
  std::vector<std::thread> workers;
  std::unique_ptr<ITaskQueue> taskQueue;
  bool stop;
};
