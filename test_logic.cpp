#include <iostream>
#include <cassert>
#include "MessageProcessor.hpp"

using namespace std;

// ── Original tests ────────────────────────────────────────────────────────────

void test_login_logic() {
    MessageProcessor mp;

    string res1 = mp.processLogin("Hana");
    assert(res1.find("Connected successfully") != string::npos);
    cout << "Test 1 Passed: Initial login works." << endl;

    string res2 = mp.processLogin("Hana");
    assert(res2.find("Username already exists") != string::npos);
    cout << "Test 2 Passed: Duplicate login blocked." << endl;

    mp.handleLogout("Hana");
    string res3 = mp.processLogin("Hana");
    assert(res3.find("Connected successfully") != string::npos);
    cout << "Test 3 Passed: Logout logic works." << endl;
}

// ── BONUS: Private message tests ──────────────────────────────────────────────

void test_private_messages() {
    MessageProcessor mp;

    string pmJson = R"({"type":"private_message","sender":"Alice","recipient":"Bob","payload":"Hey"})";
    assert(mp.isPrivate(pmJson));
    cout << "Test 4 Passed: isPrivate detects private_message type." << endl;

    string chatJson = R"({"type":"chat","sender":"Alice","payload":"Hello all"})";
    assert(!mp.isPrivate(chatJson));
    cout << "Test 5 Passed: isPrivate returns false for chat type." << endl;

    assert(mp.getPrivateRecipient(pmJson) == "Bob");
    cout << "Test 6 Passed: getPrivateRecipient extracts correct user." << endl;
}

// ── BONUS: Group tests ────────────────────────────────────────────────────────

void test_group_management() {
    MessageProcessor mp;

    mp.joinGroup("devs", "Alice");
    mp.joinGroup("devs", "Bob");
    mp.joinGroup("devs", "Carol");

    // Alice sends — Bob and Carol should receive
    auto recipients = mp.getGroupRecipients("devs", "Alice");
    assert(recipients.size() == 2);
    cout << "Test 7 Passed: Group recipients exclude the sender." << endl;

    mp.leaveGroup("devs", "Bob");
    auto afterLeave = mp.getGroupRecipients("devs", "Alice");
    assert(afterLeave.size() == 1);
    assert(afterLeave[0] == "Carol");
    cout << "Test 8 Passed: leaveGroup removes member correctly." << endl;

    string groupJson = R"({"type":"group_message","sender":"Alice","group":"devs","payload":"Hi!"})";
    assert(mp.isGroupMessage(groupJson));
    cout << "Test 9 Passed: isGroupMessage detects group_message type." << endl;

    assert(mp.getGroupName(groupJson) == "devs");
    cout << "Test 10 Passed: getGroupName extracts group name." << endl;
}

// ── BONUS: Logout cleans up group membership ──────────────────────────────────

void test_logout_cleans_groups() {
    MessageProcessor mp;
    mp.processLogin("Alice");
    mp.joinGroup("devs", "Alice");
    mp.handleLogout("Alice");

    auto members = mp.getGroupRecipients("devs", "");
    assert(members.empty());
    cout << "Test 11 Passed: Logout removes user from all groups." << endl;
}

// ── BONUS: User list JSON ─────────────────────────────────────────────────────

void test_user_list_json() {
    MessageProcessor mp;
    mp.processLogin("Alice");
    mp.processLogin("Bob");

    string userListJson = mp.buildUserListJson();
    auto parsed = json::parse(userListJson);
    assert(parsed["type"] == "user_list");
    assert(parsed["users"].size() == 2);
    cout << "Test 12 Passed: buildUserListJson produces correct JSON." << endl;
}

int main() {
    cout << "--- RUNNING ALL TESTS ---" << endl;
    test_login_logic();
    test_private_messages();
    test_group_management();
    test_logout_cleans_groups();
    test_user_list_json();
    cout << "\nALL 12 TESTS PASSED!" << endl;
    return 0;
}
