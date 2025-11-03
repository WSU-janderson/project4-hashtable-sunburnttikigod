#ifndef PROJECT4_HASHTABLE_HASHTABLE_H
#define PROJECT4_HASHTABLE_HASHTABLE_H
#include <optional>
#include <vector>

#include "HashTableBucket.h"

namespace std {
 class HashTable {
 private:
  size_t m_size = 0;
  std::vector<HashTableBucket> table;
  std::vector<size_t> offsets;

  size_t hash(const std::string& key) const;
  size_t probeIndex(size_t home, size_t attempt) const;
  void resize();

 public:
  HashTable(size_t initCapacity = 8);

  static std::vector<::size_t> generateOffsets(size_t cap);

  bool insert(const std::string& key, const size_t& value);
  bool remove(const std::string& key);
  bool contains(const std::string& key) const;
  optional<size_t> get(const std::string& key) const;
  size_t& operator[](const std::string& key);

  vector<std::string> keys() const;
  double alpha() const;
  size_t capacity() const;

  size_t size() const;

  void rehashBackwards();

  void debugDumpToJSON();

  friend std::ostream& operator<<(std::ostream& os, const HashTable& ht);
 };


}
#endif //PROJECT4_HASHTABLE_HASHTABLE_H