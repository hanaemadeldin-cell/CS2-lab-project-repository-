#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::as_tuple;

// ==============================
// Handle one client
// ==============================
awaitable<void> handle_client(tcp::socket socket) {
    try {
        while (true) {
            char data[1024];

            auto [ec, bytes_read] = co_await socket.async_read_some(
                boost::asio::buffer(data),
                as_tuple(use_awaitable)
            );

            if (ec) break;

std::string msg(data, bytes_read);

std::cout << "\n--- JSON RECEIVED ---\n";

// Extract value helper
auto extractValue = [&](const std::string& key) {
    size_t key_pos = msg.find("\"" + key + "\"");
    if (key_pos == std::string::npos) return std::string("");

    size_t colon = msg.find(":", key_pos);
    size_t start = msg.find("\"", colon + 1);
    size_t end = msg.find("\"", start + 1);

    return msg.substr(start + 1, end - start - 1);
};

// Extract clean values
std::string sender = extractValue("sender");
std::string payload = extractValue("payload");
std::string type = extractValue("type");

// Print nicely
std::cout << "Sender: " << sender << "\n";
std::cout << "Message: " << payload << "\n";
std::cout << "Type: " << type << "\n";

std::cout << "---------------------\n";
            std::string response = "Echo: " + msg;

            co_await boost::asio::async_write(
                socket,
                boost::asio::buffer(response),
                use_awaitable
            );
        }
    }
    catch (...) {
        std::cout << "Client disconnected.\n";
    }
}
// ==============================
// Listen for connections
// ==============================
awaitable<void> listener() {
    auto io_ctx = co_await boost::asio::this_coro::executor;

    tcp::acceptor acceptor(io_ctx, { tcp::v4(), 54321 });

    std::cout << "Server is listening on 127.0.0.1: port 54321...\n";

    while (true) {
        auto [ec, socket] = co_await acceptor.async_accept(as_tuple(use_awaitable));

        if (!ec) {
            co_spawn(io_ctx, handle_client(std::move(socket)), detached);
        }
        else {
            std::cout << "Accept error: " << ec.message() << "\n";
        }
    }
}

// ==============================
// Main
// ==============================
int main() {
    boost::asio::io_context io_context;

    co_spawn(io_context, listener(), detached);

    io_context.run();

    return 0;
}
