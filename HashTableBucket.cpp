#include "HashTableBucket.h"
#include <iostream>

namespace std {

   HashTableBucket::HashTableBucket()
       : state(BucketType::ESS), key("SENTINEL_KEY_42"), value(0) {}

   HashTableBucket::HashTableBucket(const std::string& key, const size_t& value)
       : state(BucketType::NORMAL), key(key), value(value) {}

   void HashTableBucket::load(const std::string& key, const size_t& value) {
      this->key = key;
      this->value = value;
      this->state = BucketType::NORMAL;
   }

   bool HashTableBucket::isEmpty() const {
      return state != BucketType::NORMAL;
   }

   bool HashTableBucket::isEmptySinceStart() const {
      return state == BucketType::ESS;
   }

   bool HashTableBucket::isEmptyAfterRemoval() const {
      return state == BucketType::EAR;
   }

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

   void HashTableBucket::markRemoved() {
      state = BucketType::EAR;
      key = "SENTINEL_KEY_42";
      value = 0;
   }

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