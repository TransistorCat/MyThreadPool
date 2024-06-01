
#include "src/TaskQueue/ITaskQueue.h"
#include "src/TaskQueue/SimpleQueue.h"
// #include "src/ThreadPool/IThreadPool.h"
#include "src/ThreadPool/IThreadPool.h"
#include "src/ThreadPool/MultplePool.h"
#include "src/ThreadPool/SimplePool.h"
#include "src/ThreadPool/ThreadLocalPool.h"
#include <iostream>
#include <memory>

// void printCurrentThreadId() {
//   // 获取当前线程的 ID
//   std::thread::id threadId = std::this_thread::get_id();
//   // 打印当前线程的 ID
//   // std::this_thread::sleep_for(std::chrono::seconds(3));
//   std::cout << "Current thread ID: " << threadId << std::endl;
// }
// void printTask() { printCurrentThreadId(); }
// int main(int, char **) {
//   std::unique_ptr<ITaskQueue> taskQueue = std::make_unique<SimpleQueue>();
//   auto threadPool = std::make_unique<MultplePool>(2, std::move(taskQueue));
//   threadPool->submit(printCurrentThreadId);
//   // std::this_thread::sleep_for(std::chrono::seconds(1));
//   threadPool->submit(printCurrentThreadId);
//   printf("Hello, from MyThreadPool!\n");
// }

void performanceTest(IThreadPool &pool, int numTasks) {
  std::vector<std::future<void>> results;
  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < numTasks; ++i) {
    results.emplace_back(pool.submit(

        [i] {
          // std::cout << "Start: " << i << std::endl;
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }));
  }

  for (auto &&result : results) {
    result.get();
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration = end - start;
  std::cout << "Completed " << numTasks << " tasks in " << duration.count()
            << " seconds.\n";
}

int main() {
  auto taskQueue1 = std::make_unique<SimpleQueue>();
  // auto taskQueue2 = std::make_unique<UnSafeQueue>();
  MultplePool multple_pool(1000, std::move(taskQueue1->clone()));
  SimplePool simple_pool(1000, std::move(taskQueue1->clone()));
  int numTasks = 500000;
  ThreadLocalPool threadlocal_pool(1000, std::move(taskQueue1->clone()));
  std::cout << "V1: ";
  performanceTest(simple_pool, numTasks);
  std::cout << "V2: ";
  performanceTest(multple_pool, numTasks);
  std::cout << "V3: ";
  performanceTest(threadlocal_pool, numTasks);

  return 0;
}