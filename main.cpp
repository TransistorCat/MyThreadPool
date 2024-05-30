#include "src/ThreadPool/IThreadPool.h"
#include <iostream>

void printCurrentThreadId() {
  // 获取当前线程的 ID
  std::thread::id threadId = std::this_thread::get_id();
  // 打印当前线程的 ID
  std::cout << "Current thread ID: " << threadId << std::endl;
}
void printTask() { printCurrentThreadId(); }
int main(int, char **) {
  IThreadPool a;
  a.submit(printTask);
  printf("Hello, from MyThreadPool!\n");
}
