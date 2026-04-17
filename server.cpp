#include <iostream>
#include <string_view>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::as_tuple;

// Handle one client
awaitable<void> handle_client(tcp::socket socket) {
    char data[1024];

    auto [ec, bytes_read] = co_await socket.async_read_some(
        boost::asio::buffer(data),
        as_tuple(use_awaitable)
    );

    if (!ec) {
        std::cout << "Server received: "
                  << std::string_view(data, bytes_read) << "\n";
    } else {
        std::cout << "Read error: " << ec.message() << "\n";
        co_return;
    }

    std::string response = "Message received\n";

    co_await socket.async_write_some(
        boost::asio::buffer(response),
        use_awaitable
    );
}

// Listener coroutine
awaitable<void> listener() {
    auto executor = co_await boost::asio::this_coro::executor;

    tcp::acceptor acceptor(executor, { tcp::v4(), 54321 });

    std::cout << "Server is listening on 127.0.0.1: port 54321...\n";

    while (true) {
        auto [ec, socket] = co_await acceptor.async_accept(as_tuple(use_awaitable));

        if (!ec) {
            co_spawn(executor, handle_client(std::move(socket)), detached);
        } else {
            std::cout << "Accept error: " << ec.message() << "\n";
        }
    }
}

int main() {
    boost::asio::io_context io_context;

    co_spawn(io_context, listener(), detached);

    io_context.run();

    return 0;
}
