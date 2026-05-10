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

MessageProcessor processor;

struct Client {
    string username;
    shared_ptr<tcp::socket> socket;
};
vector<Client> connected_clients;

// Send a message to one specific user by name
awaitable<void> sendToUser(const string& targetUser, const string& msg) {
    for (auto& c : connected_clients) {
        if (c.username == targetUser) {
            co_await boost::asio::async_write(
                *c.socket, boost::asio::buffer(msg), use_awaitable);
            break;
        }
    }
}

// Broadcast updated user list to all connected clients
awaitable<void> broadcastUserList() {
    string userListJson = processor.buildUserListJson();
    for (auto& c : connected_clients) {
        co_await boost::asio::async_write(
            *c.socket, boost::asio::buffer(userListJson), use_awaitable);
    }
}

awaitable<void> handle_client(shared_ptr<tcp::socket> socket) {
    string current_user = "Unknown";
    try {
        while (true) {
            char data[4096];
            auto [ec, bytes] = co_await socket->async_read_some(
                boost::asio::buffer(data), as_tuple(use_awaitable));
            if (ec) break;

            string msg(data, bytes);
            json j;
            try { j = json::parse(msg); } catch (...) { continue; }

            string type   = j.value("type", "");
            string sender = j.value("sender", "");
            current_user  = sender;

            // ── LOGIN ────────────────────────────────────────────────────────
            if (type == "login") {
                string response = processor.processLogin(sender);
                if (response.find("successfully") != string::npos) {
                    connected_clients.push_back({sender, socket});
                    // BONUS: broadcast updated user list to everyone
                    co_await broadcastUserList();
                }
                co_await boost::asio::async_write(
                    *socket, boost::asio::buffer(response), use_awaitable);
            }
            // ── GLOBAL CHAT ──────────────────────────────────────────────────
            else if (type == "message" || type == "chat") {
                for (auto& c : connected_clients)
                    if (c.socket != socket)
                        co_await boost::asio::async_write(
                            *c.socket, boost::asio::buffer(msg), use_awaitable);
            }
            // ── BONUS: PRIVATE MESSAGE ───────────────────────────────────────
            else if (type == "private_message") {
                string recipient = processor.getPrivateRecipient(msg);
                if (!recipient.empty())
                    co_await sendToUser(recipient, msg);
            }
            // ── BONUS: JOIN GROUP ────────────────────────────────────────────
            else if (type == "join_group") {
                string group = j.value("group", "");
                if (!group.empty()) {
                    processor.joinGroup(group, sender);
                    json ack;
                    ack["type"]    = "status";
                    ack["message"] = "Joined group: " + group;
                    co_await boost::asio::async_write(
                        *socket, boost::asio::buffer(ack.dump()), use_awaitable);
                }
            }
            // ── BONUS: GROUP MESSAGE ─────────────────────────────────────────
            else if (type == "group_message") {
                string group = j.value("group", "");
                if (!group.empty()) {
                    for (const string& member : processor.getGroupRecipients(group, sender))
                        co_await sendToUser(member, msg);
                }
            }
        }
    }
    catch (exception& e) {
        cout << "User " << current_user << " disconnected." << endl;
    }

    processor.handleLogout(current_user);
    connected_clients.erase(
        remove_if(connected_clients.begin(), connected_clients.end(),
                  [&](const Client& c) { return c.socket == socket; }),
        connected_clients.end());

    // BONUS: broadcast updated user list after disconnect
    co_spawn(co_await boost::asio::this_coro::executor,
             broadcastUserList(), detached);
}

awaitable<void> listener() {
    auto io_ctx = co_await boost::asio::this_coro::executor;
    tcp::acceptor acceptor(io_ctx, {tcp::v4(), 54321});
    cout << "Server running on 127.0.0.1:54321" << endl;
    while (true) {
        auto [ec, socket] = co_await acceptor.async_accept(as_tuple(use_awaitable));
        if (!ec)
            co_spawn(io_ctx,
                     handle_client(make_shared<tcp::socket>(move(socket))), detached);
    }
}

int main() {
    boost::asio::io_context io_context;
    co_spawn(io_context, listener(), detached);
    io_context.run();
    return 0;
}
