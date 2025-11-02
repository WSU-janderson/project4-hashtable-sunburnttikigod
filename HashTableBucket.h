#ifndef HASHTABLEBUCKET_H
#define HASHTABLEBUCKET_H

#include <string>
#include <iostream>

namespace  std{

    enum class BucketType { ESS, NORMAL, EAR };

    class HashTableBucket {
    private:
        BucketType state;
        std::string key;
        size_t value;

    public:
        HashTableBucket(); // sets state to ESS
        HashTableBucket(const std::string& key, const size_t& value); // sets state to NORMAL

        void load(const std::string& key, const size_t& value); // sets state to NORMAL
        bool isEmpty() const;
        bool isEmptySinceStart() const;
        bool isEmptyAfterRemoval() const;
        bool isNormal() const;

        const std::string& getKey() const;
        size_t getValue() const;
        void markRemoved(); // sets state to EAR

        friend std::ostream& operator<<(std::ostream& os, const HashTableBucket& bucket);
    };
#endif // HASHTABLEBUCKET_H

