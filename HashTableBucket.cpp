// HashTableBucket.cpp
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
//          load - to load new key and value
//          getKey - returns the key
//          getValue - returns the value
//          getValueRef - returns a std::string& value
//          markRemoved - for node cleaning

#include "HashTableBucket.h"
#include <iostream>

namespace std {

   // Default constructor: initializes bucket as ESS with sentinel key
   HashTableBucket::HashTableBucket()
       : state(BucketType::ESS), key("SENTINEL_KEY_42"), value(0) {}

   // Parameterized constructor: loads key-value pair and marks as NORMAL
   HashTableBucket::HashTableBucket(const std::string& key, const size_t& value)
       : state(BucketType::NORMAL), key(key), value(value) {}

   // Load a key-value pair into the bucket and mark as NORMAL
   void HashTableBucket::load(const std::string& key, const size_t& value) {
      this->key = key;
      this->value = value;
      this->state = BucketType::NORMAL;
   }

   // returns true if Bucket is not NORMAL (empty in any sense of the word)
   bool HashTableBucket::isEmpty() const {
      return state != BucketType::NORMAL;
   }

   // Returns true if Bucket state is Empty since start (ESS)
   bool HashTableBucket::isEmptySinceStart() const {
      return state == BucketType::ESS;
   }

   // Returns true if Bucket state is Empty after removal (EAR)
   bool HashTableBucket::isEmptyAfterRemoval() const {
      return state == BucketType::EAR;
   }

   // Returns true if Bucket state is NORMAL
   bool HashTableBucket::isNormal() const {
      return state == BucketType::NORMAL;
   }

   const std::string& HashTableBucket::getKey() const {
      return key;
   }

   size_t HashTableBucket::getValue() const {
      return value;
   }

   size_t& HashTableBucket::getValueRef() {
      return value;
   }

   // Mark bucket as EAR (Empty After Removal) and reset contents
   void HashTableBucket::markRemoved() {
      state = BucketType::EAR;
      key = "SENTINEL_KEY_42";
      value = 0;
   }

   // Overloaded stream operator for HashTableBucket
   // Prints NORMAL buckets as <key, value>, ESS as [ESS], EAR as [EAR]
   std::ostream& operator<<(std::ostream& os, const HashTableBucket& bucket) {
      if (bucket.isNormal()) {
         os << "<" << bucket.key << ", " << bucket.value << ">";
      } else if (bucket.isEmptySinceStart()) {
         os << "[ESS]";
      } else {
         os << "[EAR]";
      }
      return os;
   }

}