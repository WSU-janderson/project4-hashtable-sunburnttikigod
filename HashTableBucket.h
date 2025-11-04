/*
// HashTableBucket.h
// Charlie Must
// CS3100 Data Structures and Algorithms
// Dr. James Anderson
// Fall 2025
// project4-HashTable
//
// Using C++ std::vector to store, probe, and otherwise manipulate user input
// data values and their associated keys in bucket objects.  This class, "HashTableBucket"
// is responsible for representing a storage node for one key and one value of the
// HashTable.  It holds a current stated which is used to check if it's full, empty,
// or never put to use.  The class holds functions that return boolean values to check state.
// The default constructor, initializes each bucket with a blank key value
// 'SENTINEL_KEY_42' and a data value of 0 - as suggested boy the project pdf and sets the
// initial state to ESS - empty since start.
// Actionable members include:
// - load - to load new key and value
// - getKey - returns the key
// - getValue - returns the value
// - getValueRef - returns a std::string& value
// - markRemoved - for node cleaning
*/
#ifndef PROJECT4_HASHTABLE_HASHTABLEBUCKET_H
#define PROJECT4_HASHTABLE_HASHTABLEBUCKET_H

#include <string>
#include <iostream>

namespace std {

    enum class BucketType {
        ESS,    // Empty Since Start
        NORMAL, // Occupied
        EAR     // Empty After Removal
    };

    class HashTableBucket {
    private:
        BucketType state;
        std::string key;
        size_t value;

    public:
        HashTableBucket();
        HashTableBucket(const std::string& key, const size_t& value);

        void load(const std::string& key, const size_t& value);
        void markRemoved();

        bool isEmpty() const;
        bool isEmptySinceStart() const;
        bool isEmptyAfterRemoval() const;
        bool isNormal() const;

        const std::string& getKey() const;
        size_t getValue() const;
        size_t& getValueRef();

        friend std::ostream& operator<<(std::ostream& os, const HashTableBucket& bucket);
    };

}

#endif // PROJECT4_HASHTABLE_HASHTABLEBUCKET_H