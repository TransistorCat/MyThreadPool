#include "../TaskQueue/ITaskQueue.h"
#include "IThreadPool.h"
#include <functional>
#include <future>
#include <vector>

class MultplePool : public IThreadPool {
public:
  ~MultplePool();
  MultplePool(size_t numThreads, std::unique_ptr<ITaskQueue> taskQueue);
  // template <class F, class... Args>
  // auto submit(F &&f, Args &&...args)
  //     -> std::future<typename std::result_of<F(Args...)>::type>;
  auto submit(std::function<void()> task) -> std::future<void>;

private:
  std::vector<std::thread> workers;
  std::vector<std::unique_ptr<ITaskQueue>> taskQueues;
  size_t numThreads;
  size_t nextQueue{0};
};
