
#include "src/TaskQueue/ITaskQueue.h"
#include "src/TaskQueue/SimpleQueue.h"
// #include "src/ThreadPool/IThreadPool.h"
#include "src/ThreadPool/IThreadPool.h"
#include "src/ThreadPool/MultplePool.h"
#include "src/ThreadPool/SimplePool.h"
#include "src/ThreadPool/ThreadLocalPool.h"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <unistd.h>

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
          std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
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

void test_SimplePool(int numThreads, int numTasks) {
  auto taskQueue1 = std::make_unique<SimpleQueue>();
  SimplePool simple_pool(numThreads, std::move(taskQueue1->clone()));
  std::cout << "SimplePool: ";
  performanceTest(simple_pool, numTasks);
}

void test_MultplePool(int numThreads, int numTasks) {
  auto taskQueue1 = std::make_unique<SimpleQueue>();
  MultplePool multple_pool(numThreads, std::move(taskQueue1->clone()));
  std::cout << "MultplePool: ";
  performanceTest(multple_pool, numTasks);
}

void test_ThreadLocalPool(int numThreads, int numTasks) {
  auto taskQueue1 = std::make_unique<SimpleQueue>();
  ThreadLocalPool threadlocal_pool(1000, std::move(taskQueue1->clone()));
  std::cout << "ThreadLocalPool: ";
  performanceTest(threadlocal_pool, numTasks);
}

int main() {
  int numThreads = 10000;
  int numTasks = 500000;
  // test_SimplePool(numThreads, numTasks);
  // sleep(5);
  test_MultplePool(numThreads, numTasks);
  test_ThreadLocalPool(numThreads, numTasks);

  return 0;
}