#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::as_tuple;

// ==============================
// Client structure
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
    try {
        while (true) {
            char data[1024];

            auto [ec, bytes] = co_await socket->async_read_some(
                boost::asio::buffer(data),
                as_tuple(use_awaitable)
            );

            if (ec) break;

            std::string msg(data, bytes);

            // ---- Extract helper ----
            auto extractValue = [&](const std::string& key) {
                size_t key_pos = msg.find("\"" + key + "\"");
                if (key_pos == std::string::npos) return std::string("");

                size_t colon = msg.find(":", key_pos);
                size_t start = msg.find("\"", colon + 1);
                size_t end = msg.find("\"", start + 1);

                return msg.substr(start + 1, end - start - 1);
            };

            std::string sender = extractValue("sender");
            std::string payload = extractValue("payload");
            std::string type = extractValue("type");

            std::cout << "\n--- JSON RECEIVED ---\n";
            std::cout << "Sender: " << sender << "\n";
            std::cout << "Message: " << payload << "\n";
            std::cout << "Type: " << type << "\n";
            std::cout << "---------------------\n";

            // ---- Register client if new ----
            bool exists = false;
            for (auto& c : connected_clients) {
                if (c.socket == socket) {
                    exists = true;
                    break;
                }
            }

            if (!exists) {
                connected_clients.push_back({sender, socket});
            }

            // ---- Broadcast message ----
            std::string response =
                "{"
                "\"type\":\"message\","
                "\"sender\":\"" + sender + "\","
                "\"payload\":\"" + payload + "\""
                "}";

            for (auto& c : connected_clients) {
                if (c.socket != socket) {
                    co_await boost::asio::async_write(
                        *c.socket,
                        boost::asio::buffer(response),
                        use_awaitable
                    );
                }
            }
        }
    }
    catch (...) {
        std::cout << "Client disconnected.\n";
    }

    // ---- REMOVE CLIENT ON DISCONNECT ----
    connected_clients.erase(
        std::remove_if(
            connected_clients.begin(),
            connected_clients.end(),
            [&](const Client& c) {
                return c.socket == socket;
            }
        ),
        connected_clients.end()
    );
}

// ==============================
// Listener
// ==============================
awaitable<void> listener() {
    auto io_ctx = co_await boost::asio::this_coro::executor;

    tcp::acceptor acceptor(io_ctx, { tcp::v4(), 54321 });

    std::cout << "Server running on 127.0.0.1:54321\n";

    while (true) {
        auto [ec, socket] = co_await acceptor.async_accept(as_tuple(use_awaitable));

        if (!ec) {
            auto shared_socket = std::make_shared<tcp::socket>(std::move(socket));

            co_spawn(io_ctx, handle_client(shared_socket), detached);
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
}
