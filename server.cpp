#include <iostream>
#include <string>
#include <vector>
#include <memory>
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
// Client structure (For Broadcasting)
// ==============================
struct Client {
    std::string username;
    std::shared_ptr<tcp::socket> socket;
};

std::vector<Client> connected_clients;

// ==============================
// Handle one client
// ==============================
awaitable<void> handle_client(std::shared_ptr<tcp::socket> socket) {
    string current_user = "Unknown";
    try {
        while (true) {
            char data[1024];

            auto [ec, bytes] = co_await socket->async_read_some(
                boost::asio::buffer(data),
                as_tuple(use_awaitable)
                );

            if (ec) break;

            string msg(data, bytes);
            auto j = json::parse(msg);
            string type = j["type"];
            string sender = j["sender"];
            current_user = sender;

            string response;

            // --- 1. LOGIN LOGIC ---
            if (type == "login") {
                response = processor.processLogin(sender);

                // If login successful, add to our broadcast list
                if (response.find("successfully") != string::npos) {
                    connected_clients.push_back({sender, socket});
                }
            }
            // --- 2. CHAT LOGIC ---
            else if (type == "chat") {
                response = msg; // Just relay the message
                for (auto& c : connected_clients) {
                    if (c.socket != socket) {
                        co_await boost::asio::async_write(*c.socket, boost::asio::buffer(response), use_awaitable);
                    }
                }
                continue; // Skip writing back to sender here
            }

            // Send response back to the sender
            co_await boost::asio::async_write(*socket, boost::asio::buffer(response), use_awaitable);
        }
    }
    catch (exception& e) {
        cout << "User " << current_user << " disconnected." << endl;
    }

    // --- CLEANUP ON DISCONNECT ---
    processor.handleLogout(current_user);
    connected_clients.erase(
        std::remove_if(connected_clients.begin(), connected_clients.end(),
                       [&](const Client& c) { return c.socket == socket; }),
        connected_clients.end()
        );
}

// ==============================
// Listener
// ==============================
awaitable<void> listener() {
    auto io_ctx = co_await boost::asio::this_coro::executor;
    tcp::acceptor acceptor(io_ctx, { tcp::v4(), 54321 });

    cout << "Server running on 127.0.0.1:54321" << endl;

    while (true) {
        auto [ec, socket] = co_await acceptor.async_accept(as_tuple(use_awaitable));
        if (!ec) {
            auto shared_socket = std::make_shared<tcp::socket>(std::move(socket));
            co_spawn(io_ctx, handle_client(shared_socket), detached);
        }
    }
}

int main() {
    boost::asio::io_context io_context;
    co_spawn(io_context, listener(), detached);
    io_context.run();
    return 0;
}
