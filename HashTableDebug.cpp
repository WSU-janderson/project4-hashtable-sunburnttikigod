/**
 * HashTableDebug.cpp
 *
 * Diagnostic test suite for HashTable
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include "HashTable.h"

using namespace std;

void testInsertAndGet(std::HashTable& ht) {
    cout << "\n[TEST] Insert and Get\n";
    assert(ht.insert("Alpha", 1));
    assert(ht.insert("Beta", 2));
    assert(ht.insert("Gamma", 3));
    assert(ht.get("Alpha").value() == 1);
    assert(ht.get("Beta").value() == 2);
    assert(ht.get("Gamma").value() == 3);
    cout << "PASS: Insert and Get\n";
}

void testRemoveAndContains(std::HashTable& ht) {
    cout << "\n[TEST] Remove and Contains\n";
    assert(ht.remove("Beta"));
    assert(!ht.contains("Beta"));
    assert(ht.contains("Alpha"));
    assert(ht.contains("Gamma"));
    cout << "PASS: Remove and Contains\n";
}

void testReinsertionAfterRemoval(std::HashTable& ht) {
    cout << "\n[TEST] Reinsertion After Removal\n";
    assert(ht.insert("Beta", 22)); // reuse tombstone
    assert(ht.get("Beta").value() == 22);
    cout << "PASS: Reinsertion After Removal\n";
}

void testOperatorAccess(std::HashTable& ht) {
    cout << "\n[TEST] Operator[] Access\n";
    ht["Delta"] = 4;
    assert(ht.get("Delta").value() == 4);
    ht["Delta"] = 44;
    assert(ht.get("Delta").value() == 44);
    cout << "PASS: Operator[] Access\n";
}

void testResizeBehavior() {
    cout << "\n[TEST] Resize Behavior\n";
    std::HashTable ht(4);
    for (int i = 0; i < 20; ++i) {
        ht.insert("Key" + to_string(i), i * 10);
    }
    assert(ht.size() == 20);
    assert(ht.capacity() >= 32);
    cout << "PASS: Resize Behavior\n";
}

void testKeysVector(std::HashTable& ht) {
    cout << "\n[TEST] Keys Vector\n";
    vector<string> keys = ht.keys();
    for (const auto& key : keys) {
        assert(ht.contains(key));
    }
    cout << "PASS: Keys Vector\n";
}

int main() {
    time_t t;
    srand(static_cast<unsigned int>(time(&t)));

    std::HashTable ht(8);

    testInsertAndGet(ht);
    testRemoveAndContains(ht);
    testReinsertionAfterRemoval(ht);
    testOperatorAccess(ht);
    testResizeBehavior();
    testKeysVector(ht);

    cout << "\nAll tests completed successfully.\n";
    return 0;
}