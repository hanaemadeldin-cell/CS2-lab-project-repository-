#include <iostream>
#include <cassert>
#include "MessageProcessor.hpp"

using namespace std;

void test_login_logic() {
    MessageProcessor mp;

    // Test 1: First login should succeed
    string res1 = mp.processLogin("Hana");
    assert(res1.find("Connected successfully") != string::npos);
    cout << "Test 1 Passed: Initial login works." << endl;

    // Test 2: Duplicate login should fail (Ibrahim's requirement)
    string res2 = mp.processLogin("Hana");
    assert(res2.find("Username already exists") != string::npos);
    cout << "Test 2 Passed: Duplicate login blocked." << endl;

    // Test 3: Logout should remove user
    mp.handleLogout("Hana");
    string res3 = mp.processLogin("Hana");
    assert(res3.find("Connected successfully") != string::npos);
    cout << "Test 3 Passed: Logout logic works." << endl;
}

int main() {
    cout << "--- RUNNING SERVER LOGIC TESTS ---" << endl;
    test_login_logic();
    cout << "ALL TESTS PASSED!" << endl;
    return 0;
}
