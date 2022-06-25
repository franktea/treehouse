#include <thread>
#include <iostream>
#include <sstream>
#include <vector>
#include <random>
#include <memory>
#include <string_view>
#include <mutex>
#include "asio.hpp"

using asio::ip::tcp;

class ThreadPool {
public:
    ThreadPool(size_t num) {
        for(size_t i = 0; i < num; ++i) {
            iocs_.emplace_back(std::make_unique<asio::io_context>());
            works_.emplace_back(asio::make_work_guard(*iocs_[i].get()));
            // 每个线程运行一个io_context
            threads_.emplace_back([this, i](){ iocs_[i]->run(); });
        }
    }
    tcp::socket CreateSocket() {
        // 随机取一个io_cotext，并用它来创建一个socket
        std::random_device rd;
        std::mt19937 gen(rd());
        auto index = std::uniform_int_distribution<>(0, threads_.size() - 1)(gen);
        tcp::socket socket(iocs_[index]->get_executor());
        return std::move(socket);
    }

    void Join() {
        for(auto&& t: threads_) {
            t.join();
        }
    }
private: // 线程、io_context、以及防止io_context退出的work guard
    std::vector<std::thread> threads_;
    std::vector<std::unique_ptr<asio::io_context>> iocs_;
    std::vector<asio::executor_work_guard<asio::io_context::executor_type>> works_;
};

std::mutex cout_mutex; // 防止cout输出混乱

// 收发数据，运行在socket的io_context::run()所在的线程中，这样的线程共3个
asio::awaitable<void> Echo(tcp::socket socket)
{
    try {
        char buff[1024];
        for(;;) {
            size_t n = co_await socket.async_read_some(asio::buffer(buff), asio::use_awaitable);
            { // 输出信息，包括thread id以及收到的内容
                std::lock_guard g(cout_mutex);
                std::cout<< "thread "<<std::this_thread::get_id()<<" received msg: "<<std::string_view(buff, n)<<"\n";
            }
            co_await asio::async_write(socket, asio::buffer(buff, n), asio::use_awaitable);
        }
    } catch (...) {
    }
}

int main() {
    ThreadPool pool(3); // 三个线程收发数据

    asio::io_context io; // 主线程接受连接
    tcp::acceptor acceptor(io, {tcp::v4(), 10001});
    asio::co_spawn(io, [&acceptor, &pool]()->asio::awaitable<void>{
        for(;;) {
            tcp::socket socket = pool.CreateSocket();
            co_await acceptor.async_accept(socket, asio::use_awaitable);
            asio::co_spawn(socket.get_executor(), Echo(std::move(socket)), asio::detached);
        }
    }, asio::detached);

    io.run();
    pool.Join();
}
