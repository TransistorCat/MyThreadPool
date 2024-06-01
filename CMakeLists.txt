cmake_minimum_required(VERSION 3.0.0)
project(MyThreadPool VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

add_executable(MyThreadPool main.cpp
src/TaskQueue/SimpleQueue.cpp
src/ThreadPool/SimplePool.cpp
src/ThreadPool/MultplePool.cpp
src/ThreadPool/ThreadLocalPool.cpp
)
target_link_libraries(MyThreadPool PRIVATE pthread)
include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
