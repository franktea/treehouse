#include <iostream>
#include <thread>
#include <vector>
#include <sstream>
#include <chrono>
#include "asio.hpp"

int main() {
    asio::io_context io; 

    auto f = []{
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100us);
        std::stringstream ss;
        ss << "running in thread "<<std::this_thread::get_id()<<"\n";
        std::cout<<ss.str(); // 将需要cout的内容先组织好，然后一次输出，多线程时就不会乱
    };

    for(int i = 0; i < 10; ++i) {
        // 需要在io_context中执行的内容，每次执行会分配到任意一个运行io_context::run()的线程中去执行
        asio::post(io, f);
    }

    std::vector<std::thread> threads;
    for(int i = 0; i < 4; ++i) {
        // 在每个线程里面分别调用io_context::run()
        threads.emplace_back([&io]{ io.run(); });
    }

    for(auto&& t: threads) {
        t.join();
    }
}
