#ifndef MESSAGEPROCESSOR_HPP
#define MESSAGEPROCESSOR_HPP

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <nlohmann/json.hpp>

using namespace std; // Added as requested
using json = nlohmann::json;

class MessageProcessor {
public:
    // This list stores the names of everyone currently online [cite: 20]
    vector<string> active_usernames;

    // --- STEP 2: LOGIN LOGIC (Ibrahim's Requirement) ---
    // This checks if a username is already taken before letting them join [cite: 20, 60]
    string processLogin(string username) {
        // Check if the name is already in our vector
        for (const string& name : active_usernames) {
            if (name == username) {
                // Return the exact error required by the specs [cite: 63, 64]
                json errorResponse;
                errorResponse["type"] = "error";
                errorResponse["message"] = "Username already exists";
                return errorResponse.dump();
            }
        }

        // If name is unique, add it to the list and send success [cite: 92, 94]
        active_usernames.push_back(username);
        json successResponse;
        successResponse["type"] = "status";
        successResponse["message"] = "Connected successfully";
        return successResponse.dump();
    }

    // --- STEP 4: PRIVATE MESSAGE CHECK ---
    // Logic to see if a message should go to one person or everyone [cite: 41, 44]
    bool isPrivate(string raw_json) {
        try {
            auto data = json::parse(raw_json);
            return data["type"] == "private_message";
        } catch (...) {
            return false;
        }
    }

    // Logic to handle when someone closes the app [cite: 74]
    void handleLogout(string username) {
        active_usernames.erase(
            remove(active_usernames.begin(), active_usernames.end(), username),
            active_usernames.end()
            );
    }
};

#endif
