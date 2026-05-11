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

awaitable<void> sendToUser(const string& targetUser, const string& msg) {
    for (auto& c : connected_clients) {
        if (c.username == targetUser) {
            co_await boost::asio::async_write(
                *c.socket, boost::asio::buffer(msg + "\n"), use_awaitable);
            break;
        }
    }
}

awaitable<void> broadcastUserList() {
    string userListJson = processor.buildUserListJson() + "\n";
    for (auto& c : connected_clients)
        co_await boost::asio::async_write(
            *c.socket, boost::asio::buffer(userListJson), use_awaitable);
}

awaitable<void> handle_client(shared_ptr<tcp::socket> socket) {
    string current_user = "Unknown";
    try {
        boost::asio::streambuf buf;
        while (true) {
            auto [ec, n] = co_await boost::asio::async_read_until(
                *socket, buf, '\n', as_tuple(use_awaitable));
            if (ec) break;

            string line;
            istream is(&buf);
            getline(is, line);
            if (line.empty()) continue;

            json j;
            try { j = json::parse(line); } catch (...) { continue; }

            string type    = j.value("type", "");
            string sender  = j.value("sender", "");
            string payload = j.value("payload", "");
            current_user   = sender.empty() ? current_user : sender;

            cout << "[" << type << "] from " << sender << endl;

            if (type == "login") {
                string response = processor.processLogin(sender);
                if (response.find("successfully") != string::npos)
                    connected_clients.push_back({sender, socket});
                co_await boost::asio::async_write(
                    *socket, boost::asio::buffer(response + "\n"), use_awaitable);
                co_await broadcastUserList();
            }
            else if (type == "chat") {
                cout << "  Message: " << payload << endl;
                for (auto& c : connected_clients)
                    if (c.socket != socket)
                        co_await boost::asio::async_write(
                            *c.socket, boost::asio::buffer(line + "\n"), use_awaitable);
            }
            else if (type == "private_message") {
                string recipient = j.value("recipient", "");
                cout << "  PM to " << recipient << ": " << payload << endl;
                if (!recipient.empty())
                    co_await sendToUser(recipient, line);
            }
            else if (type == "join_group") {
                string group = j.value("group", "");
                processor.joinGroup(group, sender);
                cout << "  Joined group: " << group << endl;
                json ack;
                ack["type"]    = "status";
                ack["message"] = "Joined group: " + group;
                co_await boost::asio::async_write(
                    *socket, boost::asio::buffer(ack.dump() + "\n"), use_awaitable);
            }
            else if (type == "group_message") {
                string group = j.value("group", "");
                cout << "  Group msg to #" << group << ": " << payload << endl;
                for (const string& member : processor.getGroupRecipients(group, sender))
                    co_await sendToUser(member, line);
            }
        }
    }
    catch (exception& e) {
        cout << "User " << current_user << " disconnected: " << e.what() << endl;
    }

    processor.handleLogout(current_user);
    connected_clients.erase(
        remove_if(connected_clients.begin(), connected_clients.end(),
                  [&](const Client& c) { return c.socket == socket; }),
        connected_clients.end());

    co_spawn(co_await boost::asio::this_coro::executor,
             broadcastUserList(), detached);
}

awaitable<void> listener() {
    auto ex = co_await boost::asio::this_coro::executor;
    tcp::acceptor acceptor(ex, {tcp::v4(), 54321});
    cout << "Server running on 127.0.0.1:54321" << endl;
    while (true) {
        auto [ec, socket] = co_await acceptor.async_accept(as_tuple(use_awaitable));
        if (!ec) {
            cout << "New connection!" << endl;
            co_spawn(ex, handle_client(
                make_shared<tcp::socket>(std::move(socket))), detached);
        }
    }
}

int main() {
    try {
        boost::asio::io_context io_context(1);
        co_spawn(io_context, listener(), detached);
        cout << "Starting server..." << endl;
        io_context.run();
    } catch (exception& e) {
        cerr << "Server error: " << e.what() << endl;
    }
    return 0;
}
