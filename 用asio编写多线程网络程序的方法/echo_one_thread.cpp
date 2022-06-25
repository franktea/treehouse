#include "asio.hpp"

using asio::ip::tcp;

int main() {
    asio::io_context io;
    tcp::acceptor acceptor(io, {tcp::v4(), 10001});
    asio::co_spawn(io, [&acceptor]()->asio::awaitable<void>{
        for(;;) {
            tcp::socket socket = co_await acceptor.async_accept(acceptor.get_executor(), asio::use_awaitable);
            // use socket ...
        }
    }, asio::detached);
}
