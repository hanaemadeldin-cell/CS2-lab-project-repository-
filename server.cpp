#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "MessageProcessor.hpp"

using namespace std;
using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::as_tuple;

// Create the global business logic processor
MessageProcessor processor;

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

            string msg(data, bytes_read);

            // 1. Convert the string into a JSON object using your logic
            auto j = json::parse(msg);
            string type = j["type"];
            string sender = j["sender"];

            cout << "\n--- MESSAGE RECEIVED ---" << endl;
            cout << "From: " << sender << " | Type: " << type << endl;

            string response;

            // 2. RUN BUSINESS LOGIC (Hana/Ibrahim's Part)
            if (type == "login") {
                response = processor.processLogin(sender);
            }
            else if (type == "logout") {
                processor.handleLogout(sender);
                response = "{\"type\":\"status\",\"message\":\"Logged out\"}";
            }
            else {
                // Default response for other messages
                response = "{\"type\":\"status\",\"message\":\"Message received\"}";
            }

            // 3. Send the result back through the network
            co_await boost::asio::async_write(
                socket,
                boost::asio::buffer(response),
                use_awaitable
                );
        }
    }
    catch (exception& e) {
        cout << "Connection closed for a user." << endl;
    }
}

// ==============================
// Listen for connections
// ==============================
awaitable<void> listener() {
    auto io_ctx = co_await boost::asio::this_coro::executor;
    tcp::acceptor acceptor(io_ctx, { tcp::v4(), 54321 });

    cout << "Server is listening on port 54321..." << endl;

    while (true) {
        auto [ec, socket] = co_await acceptor.async_accept(as_tuple(use_awaitable));
        if (!ec) {
            co_spawn(io_ctx, handle_client(std::move(socket)), detached);
        }
    }
}

int main() {
    boost::asio::io_context io_context;
    co_spawn(io_context, listener(), detached);
    io_context.run();
    return 0;
}
