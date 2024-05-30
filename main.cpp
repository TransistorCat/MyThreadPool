
#include "src/TaskQueue/ITaskQueue.h"
#include "src/TaskQueue/SimpleQueue.h"
// #include "src/ThreadPool/IThreadPool.h"
#include "src/ThreadPool/SimplePool.h"
#include <iostream>
#include <memory>

void printCurrentThreadId() {
  // 获取当前线程的 ID
  std::thread::id threadId = std::this_thread::get_id();
  // 打印当前线程的 ID
  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::cout << "Current thread ID: " << threadId << std::endl;
}
void printTask() { printCurrentThreadId(); }
int main(int, char **) {
  std::unique_ptr<ITaskQueue> taskQueue = std::make_unique<SimpleQueue>();
  auto threadPool = std::make_unique<SimplePool>(2, std::move(taskQueue));
  auto task1 = threadPool->submit(printCurrentThreadId);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  auto task2 = threadPool->submit(printCurrentThreadId);
  printf("Hello, from MyThreadPool!\n");
}
