
#include "../TaskQueue/ITaskQueue.h"
#include "IThreadPool.h"
#include <functional>
#include <future>
#include <memory>

class ThreadLocalPool : public IThreadPool {
public:
  ~ThreadLocalPool();
  ThreadLocalPool(size_t numThreads, std::unique_ptr<ITaskQueue> taskQueue);
  // template <class F, class... Args>
  // auto submit(F &&f, Args &&...args)
  //     -> std::future<typename std::result_of<F(Args...)>::type>;
  auto submit(std::function<void()> task) -> std::future<void>;

private:
  std::vector<std::thread> workers;
  size_t numThreads;
  std::vector<std::shared_ptr<ITaskQueue>> localQueues;
  std::unique_ptr<ITaskQueue> globalQueue;
  std::function<void()> stealTask();
  std::mutex queueMutex;
  size_t nextQueue{0};
  std::condition_variable cv;
  thread_local static std::shared_ptr<ITaskQueue> localQueue;
};
