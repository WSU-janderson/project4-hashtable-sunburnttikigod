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