#include <functional>
#include <future>

class IThreadPool {
public:
  virtual ~IThreadPool() = default;
  // 虚函数声明
  virtual auto submit(std::function<void()>) -> std::future<void> = 0;
  //   template <class F, class... Args>
  //    auto submit(F &&f, Args &&...args)
  //       -> std::future<typename std::result_of<F(Args...)>::type> ;
};
