#include <iostream>
#include <thread>
#include "asio.hpp"

using asio::awaitable;
using asio::use_awaitable;
using asio::bind_executor;
using asio::io_context;
using asio::make_strand;
using asio::post;
using asio::make_work_guard;
using asio::detached;

std::mutex mx;

inline void PrintInThead(auto const& msg) {
    std::lock_guard lk(mx);
    std::cout<<"thread "<<std::this_thread::get_id()<<" print: "<<msg<<std::endl;
}

awaitable<void> MainThread(io_context& io_work,
    io_context& io_main)
{
    auto exe_work = bind_executor(make_strand(io_work), use_awaitable);
    auto exe_main = bind_executor(make_strand(io_main), use_awaitable);

    PrintInThead("Running from main thread");
    co_await post(exe_work); // 切换到工作线程
    PrintInThead("Now I am in work thread");
    co_await post(exe_main); // 切换到主线程
    PrintInThead("Back to to main thread");
    co_await post(exe_work);
    PrintInThead("Now in work thread again");
    co_await post(exe_main);
    PrintInThead("Back to main thread again");
}

int main() {
    // 两个线程，io_work运行在工作线程，io_main运行在主线程
    io_context io_work, io_main;
    auto guard = make_work_guard(io_work);

    std::thread work_thread([&io_work]{
        PrintInThead("work thread start");
        io_work.run();
        PrintInThead("work thread end");
    });

    asio::co_spawn(io_main, MainThread(io_work, io_main), detached);

    PrintInThead("main thread start");
    io_main.run();
    PrintInThead("main thread done");

    guard.reset();
    work_thread.join();
}
