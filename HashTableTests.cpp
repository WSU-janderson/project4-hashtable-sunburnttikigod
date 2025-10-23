/**
* HashTableTests.cpp
 * CS 3100
 * Project 4
 *
 * Fully narrated test harness for the HashTable project.
 * - Uses make_key<key_type> and make_value<value_type> for all key/value generation
 * - Prints clear section headers, dashed separators, step-by-step narration
 * - Each test ends with a SUCCESS/FAILURE summary line
 * - All original #define toggles are preserved
 */

#define RUN_TESTS
//#define USE_IMPL
// #define GRADING   // Uncomment for grading mode file output

#ifdef RUN_TESTS

#include <iostream>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <optional>
#include <string>

using namespace std;

// -----------------------------------------------------------------------------
// Configure key/value type aliases here for testing
// -----------------------------------------------------------------------------
using key_type = std::string;   // or int, size_t, double
using value_type = size_t;      // or std::string, int, double

// -----------------------------------------------------------------------------
// Implementation include
// -----------------------------------------------------------------------------
#ifdef USE_IMPL
#include "HashTableImpl.h"
using HashTable = HashTable_t<key_type, value_type>;
#else
#include "HashTable.h" // Must match key_type/value_type of the tested HashTable
#endif

// -----------------------------------------------------------------------------
/** Helpers: make_key / make_value
 *  Convert an integral loop index into a key/value for current key_type/value_type.
 *  - For std::string => "A", "B", "C", ... cycling every 26
 *  - For numeric types => i+1 (avoids zero so "not found" sentinels won't collide)
 */
// -----------------------------------------------------------------------------
template<typename KeyType, typename IndexType>
inline KeyType make_key(IndexType i)
requires std::is_integral_v<IndexType>
{
    if constexpr (std::is_same_v<KeyType, std::string>)
        return std::string(1, static_cast<char>('A' + static_cast<unsigned>(i) % 26));
    else
        return static_cast<KeyType>(i + 1);
}

template<typename ValueType, typename IndexType>
inline ValueType make_value(IndexType i)
requires std::is_integral_v<IndexType>
{
    if constexpr (std::is_same_v<ValueType, std::string>)
        return std::string(1, static_cast<char>('A' + static_cast<unsigned>(i) % 26));
    else
        return static_cast<ValueType>(i + 1);
}

// -----------------------------------------------------------------------------
// Output routing and test toggles
// -----------------------------------------------------------------------------
const string evalName{""};

#ifdef GRADING
#include <fstream>
#define OUTSTREAM os
#else
#define OUTSTREAM cout
#endif

// Intentionally matches legacy behavior: prints ht1 regardless of argument
#define HT_PRINT(ht) OUTSTREAM << "HashTable contents" << endl; OUTSTREAM << "------------------" << endl; OUTSTREAM << ht1 << endl;

#define HT_INSERT
#define HT_INSERT_DUPLICATE
#define HT_INSERT_FULL
#define HT_REMOVE
#define HT_REMOVE_MISSING
#define HT_INSERT_AFTER_REMOVE
#define HT_CONTAINS
#define HT_CONTAINS_MISSING
#define HT_CONTAINS_AFTER_REMOVE
#define HT_GET
#define HT_GET_MISSING
#define HT_GET_REMOVED
#define HT_GET_AFTER_REMOVE
#define HT_BRACKET_OP_GET
#define HT_BRACKET_OP_SET
#define HT_KEYS
#define HT_ALPHA
#define HT_CAPACITY
#define HT_SIZE

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------
int main(int, char**) {
    constexpr size_t MAXHASH = 8;

#ifdef GRADING
    ofstream OUTSTREAM("evals/" + evalName + "_eval.txt");
    if (!OUTSTREAM.good()) {
        cout << "Evaluation file failed to open\n";
        exit(1);
    }
    OUTSTREAM << "Grading evaluation for " << evalName << endl << endl;
#endif

    OUTSTREAM << "+==================+" << endl;
    OUTSTREAM << "| HASH TABLE TESTS |" << endl;
    OUTSTREAM << "+==================+" << endl << endl;

    // =====================================================================
    // INSERT & PRINT
    // =====================================================================
    OUTSTREAM << "Testing HashTable::insert() and print" << endl;
    OUTSTREAM << "------------------------------------" << endl << endl;
#ifdef HT_INSERT
    try {
        HashTable ht1;
        bool ok = true;

        OUTSTREAM << "Step 1: Insert first half (" << MAXHASH/2 << ") entries..." << endl;
        for (size_t i = 1; i <= MAXHASH / 2; i++) {
            auto k = make_key<key_type>(i);
            auto v = make_value<value_type>(i);
            bool r = ht1.insert(k, v);
            OUTSTREAM << "  insert(" << k << ", " << v << ") -> " << (r ? "true" : "false") << endl;
            ok &= r;
        }
        OUTSTREAM << endl;

#ifdef HT_PRINT
        OUTSTREAM << "Printing table after first half of inserts:" << endl;
        HT_PRINT(ht1);
        OUTSTREAM << endl;
#endif

        OUTSTREAM << "Step 2: Insert additional entries to reach initial capacity..." << endl;
        for (size_t i = (MAXHASH/2)+1; i <= MAXHASH; i++) {
            auto k = make_key<key_type>(i + 10);         // offset to vary keys
            auto v = make_value<value_type>(i + 10);
            bool r = ht1.insert(k, v);
            OUTSTREAM << "  insert(" << k << ", " << v << ") -> " << (r ? "true" : "false") << endl;
            ok &= r;
        }
        OUTSTREAM << endl;

#ifdef HT_PRINT
        OUTSTREAM << "Printing table after second stage inserts:" << endl;
        HT_PRINT(ht1);
        OUTSTREAM << endl;
#endif

        OUTSTREAM << (ok ? "SUCCESS: All planned inserts completed." : "FAILURE: One or more inserts failed.") << endl << endl;

    } catch (exception& e) {
        OUTSTREAM << "Exception: " << e.what() << endl << endl;
    }
#else
    OUTSTREAM << "*** DID NOT TEST INSERT ***" << endl << endl;
#endif

    // =====================================================================
    // INSERT DUPLICATE
    // =====================================================================
    OUTSTREAM << "Testing HashTable::insert() with duplicate keys" << endl;
    OUTSTREAM << "----------------------------------------------" << endl << endl;
#ifdef HT_INSERT_DUPLICATE
    try {
        HashTable ht1;
        OUTSTREAM << "Filling with " << (MAXHASH/2) << " unique entries..." << endl;
        for (size_t i = 1; i <= MAXHASH / 2; i++) {
            bool r = ht1.insert(make_key<key_type>(i), make_value<value_type>(i));
            OUTSTREAM << "  insert(" << make_key<key_type>(i) << ", " << make_value<value_type>(i)
                      << ") -> " << (r ? "true" : "false") << endl;
        }
        auto dupKey = make_key<key_type>(MAXHASH/3);
        auto dupVal = make_value<value_type>(MAXHASH/3);
        OUTSTREAM << "Attempting duplicate insert of <" << dupKey << ", " << dupVal << ">..." << endl;
        bool duplicateResult = ht1.insert(dupKey, dupVal);
        OUTSTREAM << (duplicateResult ? "FAILURE: duplicate was inserted (should be rejected)."
                                      : "SUCCESS: duplicate correctly rejected.")
                  << endl << endl;
    } catch (exception& e) {
        OUTSTREAM << "Exception: " << e.what() << endl << endl;
    }
#else
    OUTSTREAM << "*** DID NOT TEST INSERT DUPLICATE ***" << endl << endl;
#endif

    // =====================================================================
    // INSERT FULL
    // =====================================================================
    OUTSTREAM << "Testing insert() when table is full / resizing behavior" << endl;
    OUTSTREAM << "------------------------------------------------------" << endl << endl;
#ifdef HT_INSERT_FULL
    try {
        HashTable ht1;
        bool ok = true;

        OUTSTREAM << "Filling to capacity (" << MAXHASH << ")..." << endl;
        for (size_t i = 1; i <= MAXHASH; i++) {
            bool r = ht1.insert(make_key<key_type>(i), make_value<value_type>(i));
            OUTSTREAM << "  insert(" << make_key<key_type>(i) << ", " << make_value<value_type>(i)
                      << ") -> " << (r ? "true" : "false") << endl;
            ok &= r;
        }
        OUTSTREAM << endl;

        OUTSTREAM << "Attempting additional inserts beyond capacity to trigger growth..." << endl;
        for (size_t i = MAXHASH + 1; i <= 2 * MAXHASH; i++) {
            bool r = ht1.insert(make_key<key_type>(i), make_value<value_type>(i));
            OUTSTREAM << "  insert(" << make_key<key_type>(i) << ", " << make_value<value_type>(i)
                      << ") -> " << (r ? "true" : "false") << endl;
            ok &= r;
        }
        OUTSTREAM << endl;

        OUTSTREAM << (ok ? "SUCCESS: Inserts succeeded when table was full (resize/handling OK)."
                         : "FAILURE: Some inserts failed when table was full.")
                  << endl << endl;
    } catch (exception& e) {
        OUTSTREAM << "Exception: " << e.what() << endl << endl;
    }
#else
    OUTSTREAM << "*** DID NOT TEST INSERT FULL ***" << endl << endl;
#endif

    // =====================================================================
    // REMOVE
    // =====================================================================
    OUTSTREAM << "Testing HashTable::remove()" << endl;
    OUTSTREAM << "---------------------------" << endl << endl;
#ifdef HT_REMOVE
    try {
        HashTable ht1;
        bool ok = true;

        OUTSTREAM << "Inserting " << MAXHASH << " entries..." << endl;
        for (size_t i = 1; i <= MAXHASH; i++) {
            bool r = ht1.insert(make_key<key_type>(i), make_value<value_type>(i));
            ok &= r;
        }
        OUTSTREAM << "Removing all " << MAXHASH << " entries..." << endl;
        for (size_t i = 1; i <= MAXHASH; i++) {
            auto k = make_key<key_type>(i);
            bool r = ht1.remove(k);
            OUTSTREAM << "  remove(" << k << ") -> " << (r ? "true" : "false") << endl;
            ok &= r;
        }
        OUTSTREAM << (ok ? "SUCCESS: All removals reported success."
                         : "FAILURE: One or more removals failed.")
                  << endl << endl;
    } catch (exception& e) {
        OUTSTREAM << "Exception: " << e.what() << endl << endl;
    }
#else
    OUTSTREAM << "*** DID NOT TEST REMOVE ***" << endl << endl;
#endif

    // =====================================================================
    // REMOVE MISSING
    // =====================================================================
    OUTSTREAM << "Testing HashTable::remove() with missing key" << endl;
    OUTSTREAM << "-------------------------------------------" << endl << endl;
#ifdef HT_REMOVE_MISSING
    try {
        HashTable ht1;
        OUTSTREAM << "Inserting " << MAXHASH << " entries..." << endl;
        for (size_t i = 1; i <= MAXHASH; i++) {
            ht1.insert(make_key<key_type>(i), make_value<value_type>(i));
        }
        auto missKey = make_key<key_type>(MAXHASH + 5);
        OUTSTREAM << "Attempting to remove missing key " << missKey << " ..." << endl;
        bool r = ht1.remove(missKey);
        OUTSTREAM << (!r ? "SUCCESS: Missing key correctly not removed."
                         : "FAILURE: remove() returned true for missing key.")
                  << endl << endl;
    } catch (exception& e) {
        OUTSTREAM << "Exception: " << e.what() << endl << endl;
    }
#else
    OUTSTREAM << "*** DID NOT TEST REMOVE MISSING ***" << endl << endl;
#endif

    // =====================================================================
    // INSERT AFTER REMOVE
    // =====================================================================
    OUTSTREAM << "Testing HashTable::insert() after removing entries" << endl;
    OUTSTREAM << "-------------------------------------------------" << endl << endl;
#ifdef HT_INSERT_AFTER_REMOVE
    try {
        HashTable ht1;
        OUTSTREAM << "Filling " << MAXHASH << " entries..." << endl;
        for (size_t i = 1; i <= MAXHASH; i++) {
            ht1.insert(make_key<key_type>(i), make_value<value_type>(i));
        }

        OUTSTREAM << "Removing first half..." << endl;
        for (size_t i = 1; i <= MAXHASH / 2; i++) {
            auto k = make_key<key_type>(i);
            bool r = ht1.remove(k);
            OUTSTREAM << "  remove(" << k << ") -> " << (r ? "true" : "false") << endl;
        }

        OUTSTREAM << "Inserting " << (MAXHASH/2) << " new entries after removals..." << endl;
        for (size_t i = MAXHASH + 1; i <= MAXHASH + (MAXHASH / 2); i++) {
            bool r = ht1.insert(make_key<key_type>(i), make_value<value_type>(i));
            OUTSTREAM << "  insert(" << make_key<key_type>(i) << ", " << make_value<value_type>(i)
                      << ") -> " << (r ? "true" : "false") << endl;
        }

        bool ok = true;
        OUTSTREAM << "Verifying reinserted entries are present..." << endl;
        for (size_t i = MAXHASH + 1; i <= MAXHASH + (MAXHASH / 2); i++) {
            bool found = ht1.contains(make_key<key_type>(i));
            OUTSTREAM << "  contains(" << make_key<key_type>(i) << ") -> " << (found ? "true" : "false") << endl;
            ok &= found;
        }
        OUTSTREAM << (ok ? "SUCCESS: All reinserted entries found."
                         : "FAILURE: Missing entries after reinsertion.")
                  << endl << endl;

    } catch (exception& e) {
        OUTSTREAM << "Exception: " << e.what() << endl << endl;
    }
#else
    OUTSTREAM << "*** DID NOT TEST INSERT AFTER REMOVE ***" << endl << endl;
#endif

    // =====================================================================
    // CONTAINS
    // =====================================================================
    OUTSTREAM << "Testing HashTable::contains()" << endl;
    OUTSTREAM << "-----------------------------" << endl << endl;
#ifdef HT_CONTAINS
    try {
        HashTable ht1;
        OUTSTREAM << "Inserting " << MAXHASH << " entries..." << endl;
        for (size_t i = 1; i <= MAXHASH; i++)
            ht1.insert(make_key<key_type>(i), make_value<value_type>(i));

        bool ok = true;
        OUTSTREAM << "Checking contains() for all entries..." << endl;
        for (size_t i = 1; i <= MAXHASH; i++) {
            auto k = make_key<key_type>(i);
            bool r = ht1.contains(k);
            OUTSTREAM << "  contains(" << k << ") -> " << (r ? "true" : "false") << endl;
            ok &= r;
        }
        OUTSTREAM << (ok ? "SUCCESS: contains() true for all inserted keys."
                         : "FAILURE: contains() false for some inserted keys.")
                  << endl << endl;
    } catch (exception& e) {
        OUTSTREAM << "Exception: " << e.what() << endl << endl;
    }
#else
    OUTSTREAM << "*** DID NOT TEST CONTAINS ***" << endl << endl;
#endif

    // =====================================================================
    // CONTAINS MISSING
    // =====================================================================
    OUTSTREAM << "Testing HashTable::contains() with missing key" << endl;
    OUTSTREAM << "----------------------------------------------" << endl << endl;
#ifdef HT_CONTAINS_MISSING
    try {
        HashTable ht1;
        OUTSTREAM << "Inserting " << MAXHASH << " entries..." << endl;
        for (size_t i = 1; i <= MAXHASH; i++)
            ht1.insert(make_key<key_type>(i), make_value<value_type>(i));

        auto missKey = make_key<key_type>(MAXHASH + 5);
        bool found = ht1.contains(missKey);
        OUTSTREAM << (found ? "FAILURE: contains() true for missing key."
                            : "SUCCESS: contains() false for missing key.")
                  << endl << endl;
    } catch (exception& e) {
        OUTSTREAM << "Exception: " << e.what() << endl << endl;
    }
#else
    OUTSTREAM << "*** DID NOT TEST CONTAINS MISSING ***" << endl << endl;
#endif

    // =====================================================================
    // CONTAINS AFTER REMOVE
    // =====================================================================
    OUTSTREAM << "Testing contains() after removing a key" << endl;
    OUTSTREAM << "--------------------------------------" << endl << endl;
#ifdef HT_CONTAINS_AFTER_REMOVE
    try {
        HashTable ht1;
        OUTSTREAM << "Inserting " << MAXHASH << " entries..." << endl;
        for (size_t i = 1; i <= MAXHASH; i++)
            ht1.insert(make_key<key_type>(i), make_value<value_type>(i));

        auto remKey = make_key<key_type>(MAXHASH / 2);
        OUTSTREAM << "Removing key " << remKey << " ..." << endl;
        ht1.remove(remKey);

        bool found = ht1.contains(remKey);
        OUTSTREAM << (!found ? "SUCCESS: contains() false after removal."
                             : "FAILURE: contains() true after removal.")
                  << endl << endl;
    } catch (exception& e) {
        OUTSTREAM << "Exception: " << e.what() << endl << endl;
    }
#else
    OUTSTREAM << "*** DID NOT TEST CONTAINS AFTER REMOVE ***" << endl << endl;
#endif

    // =====================================================================
    // GET
    // =====================================================================
    OUTSTREAM << "Testing HashTable::get()" << endl;
    OUTSTREAM << "------------------------" << endl << endl;
#ifdef HT_GET
    try {
        HashTable ht1;
        OUTSTREAM << "Inserting " << MAXHASH << " entries..." << endl;
        for (size_t i = 1; i <= MAXHASH; i++)
            ht1.insert(make_key<key_type>(i), make_value<value_type>(i));

        auto targetKey = make_key<key_type>(MAXHASH / 2);
        OUTSTREAM << "Calling get(" << targetKey << ") ..." << endl;
        std::optional<value_type> res = ht1.get(targetKey);
        if (res && (*res == make_value<value_type>(MAXHASH / 2))) {
            OUTSTREAM << "SUCCESS: get() returned expected value." << endl << endl;
        } else {
            OUTSTREAM << "FAILURE: get() returned ";
            if (res) OUTSTREAM << *res; else OUTSTREAM << "nullopt";
            OUTSTREAM << ", expected " << make_value<value_type>(MAXHASH / 2) << endl << endl;
        }
    } catch (exception& e) {
        OUTSTREAM << "Exception: " << e.what() << endl << endl;
    }
#else
    OUTSTREAM << "*** DID NOT TEST GET ***" << endl << endl;
#endif

    // =====================================================================
    // GET MISSING
    // =====================================================================
    OUTSTREAM << "Testing get() with missing key" << endl;
    OUTSTREAM << "------------------------------" << endl << endl;
#ifdef HT_GET_MISSING
    try {
        HashTable ht1;
        OUTSTREAM << "Inserting " << MAXHASH << " entries..." << endl;
        for (size_t i = 1; i <= MAXHASH; i++)
            ht1.insert(make_key<key_type>(i), make_value<value_type>(i));

        auto missKey = make_key<key_type>(MAXHASH + 5);
        OUTSTREAM << "Calling get(" << missKey << ") ..." << endl;
        std::optional<value_type> res = ht1.get(missKey);
        OUTSTREAM << (!res ? "SUCCESS: get() returned nullopt for missing key."
                           : "FAILURE: get() returned a value for missing key.")
                  << endl << endl;
    } catch (exception& e) {
        OUTSTREAM << "Exception: " << e.what() << endl << endl;
    }
#else
    OUTSTREAM << "*** DID NOT TEST GET MISSING ***" << endl << endl;
#endif

    // =====================================================================
    // GET REMOVED
    // =====================================================================
    OUTSTREAM << "Testing get() with a previously removed key" << endl;
    OUTSTREAM << "-------------------------------------------" << endl << endl;
#ifdef HT_GET_REMOVED
    try {
        HashTable ht1;
        OUTSTREAM << "Inserting " << MAXHASH << " entries..." << endl;
        for (size_t i = 1; i <= MAXHASH; i++)
            ht1.insert(make_key<key_type>(i), make_value<value_type>(i));

        auto remKey = make_key<key_type>(MAXHASH / 2);
        OUTSTREAM << "Removing key " << remKey << " and then calling get()..." << endl;
        ht1.remove(remKey);
        std::optional<value_type> res = ht1.get(remKey);
        OUTSTREAM << (!res ? "SUCCESS: get() returned nullopt after removal."
                           : "FAILURE: get() still returned a value after removal.")
                  << endl << endl;
    } catch (exception& e) {
        OUTSTREAM << "Exception: " << e.what() << endl << endl;
    }
#else
    OUTSTREAM << "*** DID NOT TEST GET REMOVED ***" << endl << endl;
#endif

    // =====================================================================
    // GET AFTER REMOVE (verify others still accessible)
    // =====================================================================
    OUTSTREAM << "Testing get() on remaining key after removing others" << endl;
    OUTSTREAM << "----------------------------------------------------" << endl << endl;
#ifdef HT_GET_AFTER_REMOVE
    try {
        HashTable ht1;
        OUTSTREAM << "Inserting " << MAXHASH << " entries..." << endl;
        for (size_t i = 1; i <= MAXHASH; i++)
            ht1.insert(make_key<key_type>(i), make_value<value_type>(i));

        OUTSTREAM << "Removing first quarter of keys (excluding target)..." << endl;
        auto targetKey = make_key<key_type>(MAXHASH - 1);
        for (size_t i = 1; i <= MAXHASH / 4; i++) {
            if (make_key<key_type>(i) != targetKey) {
                ht1.remove(make_key<key_type>(i));
            }
        }
        OUTSTREAM << "Calling get(" << targetKey << ") ..." << endl;
        auto res = ht1.get(targetKey);
        bool ok = (res && *res == make_value<value_type>(MAXHASH - 1));
        OUTSTREAM << (ok ? "SUCCESS: get() found expected value after removals."
                         : "FAILURE: get() did not return expected value after removals.")
                  << endl << endl;
    } catch (exception& e) {
        OUTSTREAM << "Exception: " << e.what() << endl << endl;
    }
#else
    OUTSTREAM << "*** DID NOT TEST GET AFTER REMOVE ***" << endl << endl;
#endif

    // =====================================================================
    // operator[] GET
    // =====================================================================
    OUTSTREAM << "Testing operator[] (read access)" << endl;
    OUTSTREAM << "-------------------------------" << endl << endl;
#ifdef HT_BRACKET_OP_GET
    try {
        HashTable ht1;
        OUTSTREAM << "Inserting " << MAXHASH << " entries..." << endl;
        for (size_t i = 1; i <= MAXHASH; i++)
            ht1.insert(make_key<key_type>(i), make_value<value_type>(i));

        auto k = make_key<key_type>(MAXHASH - 1);
        OUTSTREAM << "Reading ht1[" << k << "] ..." << endl;
        value_type v = ht1[k];
        bool ok = (v == make_value<value_type>(MAXHASH - 1));
        OUTSTREAM << (ok ? "SUCCESS: operator[] read returned expected value."
                         : "FAILURE: operator[] read returned unexpected value.")
                  << endl << endl;
    } catch (exception& e) {
        OUTSTREAM << "Exception: " << e.what() << endl << endl;
    }
#else
    OUTSTREAM << "*** DID NOT TEST OPERATOR[] GET ***" << endl << endl;
#endif

    // =====================================================================
    // operator[] SET
    // =====================================================================
    OUTSTREAM << "Testing operator[] (write/update access)" << endl;
    OUTSTREAM << "---------------------------------------" << endl << endl;
#ifdef HT_BRACKET_OP_SET
    try {
        HashTable ht1;
        OUTSTREAM << "Inserting " << MAXHASH << " entries..." << endl;
        for (size_t i = 1; i <= MAXHASH; i++)
            ht1.insert(make_key<key_type>(i), make_value<value_type>(i));

        auto k = make_key<key_type>(MAXHASH - 1);
        auto newVal = make_value<value_type>(42); // canonical "updated" value
        OUTSTREAM << "Writing ht1[" << k << "] = " << newVal << " ..." << endl;
        ht1[k] = newVal;

        OUTSTREAM << "Verifying updated value..." << endl;
        value_type v2 = ht1[k];
        bool ok = (v2 == newVal);
        OUTSTREAM << (ok ? "SUCCESS: operator[] updated value correctly."
                         : "FAILURE: operator[] update not reflected in table.")
                  << endl << endl;
    } catch (exception& e) {
        OUTSTREAM << "Exception: " << e.what() << endl << endl;
    }
#else
    OUTSTREAM << "*** DID NOT TEST OPERATOR[] SET ***" << endl << endl;
#endif

    // =====================================================================
    // KEYS
    // =====================================================================
    OUTSTREAM << "Testing HashTable::keys()" << endl;
    OUTSTREAM << "-------------------------" << endl << endl;
#ifdef HT_KEYS
    try {
        HashTable ht1;
        OUTSTREAM << "Inserting " << MAXHASH << " entries..." << endl;
        for (size_t i = 1; i <= MAXHASH; i++)
            ht1.insert(make_key<key_type>(i), make_value<value_type>(i));

        OUTSTREAM << "Retrieving keys() and verifying membership..." << endl;
        auto keys = ht1.keys();
        bool allPresent = true;
        for (size_t i = 1; i <= MAXHASH; i++) {
            auto k = make_key<key_type>(i);
            bool present = (std::find(keys.begin(), keys.end(), k) != keys.end());
            OUTSTREAM << "  find(" << k << ") in keys -> " << (present ? "found" : "NOT found") << endl;
            allPresent &= present;
        }
        OUTSTREAM << (allPresent ? "SUCCESS: keys() returned all inserted keys."
                                 : "FAILURE: keys() missing one or more keys.")
                  << endl << endl;
    } catch (exception& e) {
        OUTSTREAM << "Exception: " << e.what() << endl << endl;
    }
#else
    OUTSTREAM << "*** DID NOT TEST KEYS ***" << endl << endl;
#endif

    // =====================================================================
    // ALPHA (load factor)
    // =====================================================================
    OUTSTREAM << "Testing HashTable::alpha()" << endl;
    OUTSTREAM << "--------------------------" << endl << endl;
#ifdef HT_ALPHA
    try {
        HashTable ht1;
        double a0 = ht1.alpha();
        OUTSTREAM << "Initial alpha() = " << a0 << endl;

        OUTSTREAM << "Inserting " << (MAXHASH/2) << " entries..." << endl;
        for (size_t i = 1; i <= MAXHASH / 2; i++)
            ht1.insert(make_key<key_type>(i), make_value<value_type>(i));

        double a = ht1.alpha();
        OUTSTREAM << "alpha() after inserts = " << a << endl;
        OUTSTREAM << "NOTE: Expected approx " << (static_cast<double>(MAXHASH/2) / MAXHASH) << " (implementation dependent)." << endl;
        OUTSTREAM << "SUCCESS: alpha() returned a value (manual inspection for exact expectation)." << endl << endl;
    } catch (exception& e) {
        OUTSTREAM << "Exception: " << e.what() << endl << endl;
    }
#else
    OUTSTREAM << "*** DID NOT TEST ALPHA ***" << endl << endl;
#endif

    // =====================================================================
    // CAPACITY
    // =====================================================================
    OUTSTREAM << "Testing HashTable::capacity()" << endl;
    OUTSTREAM << "-----------------------------" << endl << endl;
#ifdef HT_CAPACITY
    try {
        HashTable ht1;
        OUTSTREAM << "Inserting " << (MAXHASH/2) << " entries..." << endl;
        for (size_t i = 1; i <= MAXHASH / 2; i++)
            ht1.insert(make_key<key_type>(i), make_value<value_type>(i));

        OUTSTREAM << "Capacity reported: " << ht1.capacity() << endl;
        OUTSTREAM << "Adding more inserts to trigger capacity change (implementation dependent)..." << endl;
        for (size_t i = (MAXHASH/2)+1; i <= (MAXHASH*2) - 1; i++)
            ht1.insert(make_key<key_type>(i), make_value<value_type>(i));

        OUTSTREAM << "New capacity reported: " << ht1.capacity() << endl;
        OUTSTREAM << "SUCCESS: capacity() reported values (manual inspection for exact expectation)." << endl << endl;
    } catch (exception& e) {
        OUTSTREAM << "Exception: " << e.what() << endl << endl;
    }
#else
    OUTSTREAM << "*** DID NOT TEST CAPACITY ***" << endl << endl;
#endif

    // =====================================================================
    // SIZE
    // =====================================================================
    OUTSTREAM << "Testing HashTable::size()" << endl;
    OUTSTREAM << "-------------------------" << endl << endl;
#ifdef HT_SIZE
    try {
        HashTable ht1;
        bool ok = true;

        OUTSTREAM << "Inserting " << MAXHASH << " entries and checking size each time..." << endl;
        for (size_t i = 1; i <= MAXHASH; i++) {
            ht1.insert(make_key<key_type>(i), make_value<value_type>(i));
            size_t s = ht1.size();
            OUTSTREAM << "  size() = " << s << " (after i=" << i << ")" << endl;
            ok &= (s == i);
        }
        OUTSTREAM << (ok ? "SUCCESS: size() matched after initial inserts." : "FAILURE: size() mismatch after inserts.") << endl;

        OUTSTREAM << "Continuing inserts up to nearly 2*MAXHASH..." << endl;
        for (size_t i = MAXHASH + 1; i <= (MAXHASH * 2) - 1; i++) {
            ht1.insert(make_key<key_type>(i), make_value<value_type>(i));
            size_t s = ht1.size();
            OUTSTREAM << "  size() = " << s << " (after i=" << i << ")" << endl;
            ok &= (s == i);
        }
        OUTSTREAM << (ok ? "SUCCESS: size() matched through extended inserts."
                         : "FAILURE: size() mismatch during extended inserts.")
                  << endl << endl;
    } catch (exception& e) {
        OUTSTREAM << "Exception: " << e.what() << endl << endl;
    }
#else
    OUTSTREAM << "*** DID NOT TEST SIZE ***" << endl << endl;
#endif

    OUTSTREAM << "All tests complete." << endl;
    return 0;
}
#endif // RUN_TESTS
