#ifndef MESSAGEPROCESSOR_HPP
#define MESSAGEPROCESSOR_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

class MessageProcessor {
public:
    vector<string> active_usernames;

    // BONUS: {groupName -> set of member usernames}
    map<string, set<string>> groups;

    // ── LOGIN ─────────────────────────────────────────────────────────────────
    string processLogin(string username) {
        for (const string& name : active_usernames) {
            if (name == username) {
                json err; err["type"] = "error"; err["message"] = "Username already exists";
                return err.dump();
            }
        }
        active_usernames.push_back(username);
        json ok; ok["type"] = "status"; ok["message"] = "Connected successfully";
        return ok.dump();
    }

    // ── LOGOUT ────────────────────────────────────────────────────────────────
    void handleLogout(string username) {
        active_usernames.erase(
            remove(active_usernames.begin(), active_usernames.end(), username),
            active_usernames.end());
        for (auto& [g, members] : groups) members.erase(username);
    }

    // ── PRIVATE MESSAGE ───────────────────────────────────────────────────────
    bool isPrivate(string raw_json) {
        try { return json::parse(raw_json)["type"] == "private_message"; }
        catch (...) { return false; }
    }

    string getPrivateRecipient(const string& raw_json) {
        try { return json::parse(raw_json).value("recipient", ""); }
        catch (...) { return ""; }
    }

    // ── GROUP MANAGEMENT (BONUS) ──────────────────────────────────────────────
    void joinGroup(const string& groupName, const string& username) {
        groups[groupName].insert(username);
    }

    void leaveGroup(const string& groupName, const string& username) {
        if (groups.count(groupName)) groups[groupName].erase(username);
    }

    // Returns all group members except the sender
    vector<string> getGroupRecipients(const string& groupName, const string& sender) {
        vector<string> result;
        if (!groups.count(groupName)) return result;
        for (const string& m : groups[groupName])
            if (m != sender) result.push_back(m);
        return result;
    }

    bool isGroupMessage(const string& raw_json) {
        try { return json::parse(raw_json)["type"] == "group_message"; }
        catch (...) { return false; }
    }

    string getGroupName(const string& raw_json) {
        try { return json::parse(raw_json).value("group", ""); }
        catch (...) { return ""; }
    }

    // Build the current user list as JSON for broadcasting
    string buildUserListJson() {
        json j;
        j["type"] = "user_list";
        j["users"] = active_usernames;
        return j.dump();
    }
};

#endif
