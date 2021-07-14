#include <asio.hpp>
#include <cstdio>
#include <iostream>
#include <array>
#include <optional>
#include <string>
#include <span>
#include <fmt/core.h>
#include <fmt/format.h>
#include <asio/experimental/as_tuple.hpp>
#include <asio/experimental/awaitable_operators.hpp>

using asio::awaitable;
using asio::buffer;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;
using asio::ip::tcp;
using namespace asio::experimental::awaitable_operators;
namespace this_coro = asio::this_coro;

awaitable<void> copy(tcp::socket &from, tcp::socket &to)
{
    uint8_t data[1024];
    try
    {
        for (;;)
        {
            size_t n = co_await from.async_read_some(buffer(data), use_awaitable);
            co_await asio::async_write(to, buffer(data, n), use_awaitable);
        }
    }
    catch (std::exception &e)
    {
        from.close();
        to.close();
    }
}

awaitable<void> socks5(tcp::socket local_socket)
{
    try
    {
        uint8_t data[1024];

        // 接收05 01 00
        std::size_t n = co_await local_socket.async_read_some(buffer(data), use_awaitable);
        if (n < 3 || (data[1] + 2) != n)
        {
            fmt::print("05 01 00 err, n={}\n", n);
            co_return;
        }
        fmt::print("fd={}, recved {} bytes: {}\n", local_socket.native_handle(), n, fmt::join(data, data + n, ","));

        // 回复05 00
        co_await asio::async_write(local_socket, buffer(std::array<uint8_t, 2>{0x05, 0x00}), use_awaitable);

        // 接收05 01 00 ...
        //    +----+-----+-------+------+----------+----------+
        //    |VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
        //    +----+-----+-------+------+----------+----------+
        //    | 1  |  1  | X'00' |  1   | Variable |    2     |
        //    +----+-----+-------+------+----------+----------+
        n = co_await local_socket.async_read_some(buffer(data), use_awaitable);
        fmt::print("fd={}, recved {} bytes: {}\n", local_socket.native_handle(), n, fmt::join(data, data+n, ","));
        if (n < 4)
        {
            fmt::print("recv ver cmd len error, n={}\n", n);
            co_return;
        }

        if (data[1] != 0x01)
        { // 只实现了CMD=1，即connect
            fmt::print("only supported cmd=1, requested cmd={} is not supported\n", data[1]);
            data[1] = 0x07; // Command not supported
            co_await asio::async_write(local_socket, buffer(data, n), use_awaitable);
            co_return;
        }

        std::optional<std::string> addr; // 解析出来的地址保存在这里
        uint16_t port = 0;
        switch (data[3])
        {          // ATYP
        case 0x01: // ipv4格式的地址: 05 01 00 01 ip[4] port[2]
        {
            if (n != 10)
            {
                data[1] = 0x1; // X'01' general SOCKS server failure
                fmt::print("ipv4 package length error, len should be 10, not {}\n", n);
                co_return;
            }

            addr = fmt::format("{}.{}.{}.{}", data[4], data[5], data[6], data[7]);
            port = uint16_t(data[8]) * 256 + data[9];
            fmt::print("recved dest addr: {}\n", *addr);
        }
        break;
        case 0x03: // 域名: 05 01 00 03 host_len host[host_len] port[2]
        {
            if (n < 5)
            {
                fmt::print("domain length err, should be greater than 5, now get {}\n", n);
                co_return;
            }

            const size_t domain_len = data[4];
            if (n != 5 + domain_len + 2)
            {
                fmt::print("domain full length err, should be {}, now get {}\n", 5 + domain_len + 2, n);
                co_return;
            }

            addr = std::string((char *)(&data[0]) + 5, domain_len);
            port = uint16_t(data[n - 2]) * 256 + data[n - 1];
            fmt::print("domain: {}, port: {}\n", *addr, port);
        }
        break;
        default: // 其它的type暂时不支持
            data[1] = 0x08; // X'08' Address type not supported
            co_await asio::async_write(local_socket, buffer(data, n), use_awaitable);
            co_return;
            break;
        }

        asio::ip::tcp::resolver resolver(local_socket.get_executor());
        asio::ip::tcp::resolver::results_type results = co_await resolver.async_resolve(addr.value(), std::to_string(port), use_awaitable);

        fmt::print("fd={}, resolved\n", local_socket.native_handle());

        // connect to dest server
        tcp::socket remote_socket(local_socket.get_executor());
        co_await asio::async_connect(remote_socket, results, use_awaitable);

        fmt::print("fd={}, connected to dest {}\n", local_socket.native_handle(), remote_socket.remote_endpoint().address().to_string());

        // 连接成功，回包给client
        uint8_t resp[] = {0x05, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        co_await asio::async_write(local_socket, buffer(resp), use_awaitable);
        //co_await local_socket.async_write_some(buffer(resp), use_awaitable);

        fmt::print("fd={}, write to client\n", local_socket.native_handle());

        // 握手完成，开始透传数据
        co_await (copy(local_socket, remote_socket) || copy(remote_socket, local_socket));
    }
    catch (asio::system_error& e)
    {
        fmt::print("socks5 Exception: {}, {}\n", e.what(), e.code().value());
    }
}

awaitable<void> listener()
{
    auto executor = co_await this_coro::executor;
    tcp::acceptor acceptor(executor, {tcp::v4(), 10001}); // 绑定端口
    for (;;)
    {
        tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
        fmt::print("new conn, fd={}\n", socket.native_handle());
        co_spawn(executor, socks5(std::move(socket)), detached);
    }
}

int main()
{
    try
    {
        asio::io_context io_context(1); // 单线程
        asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto)
                           { io_context.stop(); });
        co_spawn(io_context, listener(), detached);
        io_context.run();
    }
    catch (std::exception &e)
    {
        std::printf("Exception: %s\n", e.what());
    }
}