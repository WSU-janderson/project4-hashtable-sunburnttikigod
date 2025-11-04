#include "HashTable.h"
#include "HashTableBucket.h"
#include <vector>
#include <optional>
#include <algorithm>
#include <random>
#include <fstream>
#include <cstdlib>

using namespace std;

namespace std {
    // Constructor: initializes hash table with given capacity
    // Sets size to 0 and generates randomized probe offsets
    HashTable::HashTable(size_t initCapacity)
        : m_size(0), table(initCapacity) {
        offsets = generateOffsets(initCapacity);
    }

    // Generate a randomized sequence of probe offsets for open addressing
    std::vector<size_t> HashTable::generateOffsets(size_t cap) {
        std::vector<size_t> result;

        // Fill offset vector with values [1, cap-1]
        for (size_t i = 1; i < cap; ++i) {
            result.push_back(i);
        }

        // Shuffle offsets using a C-style random generator
        if (!result.empty()) {
            struct CStyleRandGenerator {
                using result_type = unsigned int;
                static constexpr result_type min() { return 0; }
                static constexpr result_type max() { return RAND_MAX; }
                result_type operator()() { return static_cast<unsigned int>(::rand()); }
            };

            CStyleRandGenerator g;
            ranges::shuffle(result, g);
        }

        return result;
    }

    // Hash function: maps key to index in table using std::hash
    size_t HashTable::hash(const std::string &key) const {
        return std::hash<std::string>{}(key) % capacity();
    }

    // Compute probe index using home index and offset sequence
    size_t HashTable::probeIndex(size_t home, size_t attempt) const {
        return (home + offsets[attempt]) % capacity();
    }

    // Insert a key-value pair into the hash table
    bool HashTable::insert(const std::string &key, const size_t &value) {
        // Seed randomness based on key length (used in offset shuffling)
        srand(key.length());

        // Reject sentinel value (9999) as invalid
        if (value == 9999) {
            return false;
        }

        // Resize table if load factor exceeds threshold
        if (alpha() >= 0.5) {
            resize();
        }

        double oldAlpha = alpha();
        size_t home = hash(key);
        size_t first_ear_index = capacity(); // sentinel for tombstone reuse

        // First pass: check for duplicate key or early termination
        for (size_t i = 0; i < capacity(); ++i) {
            size_t index = probeIndex(home, i);

            // Duplicate key found — reject insert
            if (table[index].isNormal() && table[index].getKey() == key) {
                return false;
            }

            // Hit an ESS — key not present, stop probing
            if (table[index].isEmptySinceStart()) {
                break;
            }

            // Continue probing through EAR (tombstones)
        }


        // Find slot to insert
        for (size_t i = 0; i < capacity(); ++i) {
            size_t index = probeIndex(home, i);
            if (table[index].isEmptyAfterRemoval() && first_ear_index == capacity()) {
                first_ear_index = index;
            }
            if (table[index].isEmptySinceStart()) {
                table[index].load(key, value);
                ++m_size;
                if (alpha() != oldAlpha) debugDumpToJSON();
                return true;
            }
        }

        // Reuse tombstone if no ESS found
        if (first_ear_index != capacity()) {
            table[first_ear_index].load(key, value);
            ++m_size;
            if (alpha() != oldAlpha) debugDumpToJSON();
            return true;
        }

        return false;
    }

    // Resize the hash table when load factor exceeds threshold.
    // Doubles capacity, rehashes all NORMAL buckets, and regenerates probe offsets.
    void HashTable::resize() {
        size_t newCapacity = capacity() * 2;
        std::vector<HashTableBucket> oldTable = std::move(table);

        table.assign(newCapacity, HashTableBucket());
        offsets = generateOffsets(newCapacity);
        m_size = 0;

        for (const auto &bucket: oldTable) {
            if (bucket.isNormal()) {
                insert(bucket.getKey(), bucket.getValue());
            }
        }
    }

    // Remove a key from the table by marking its bucket as EAR (Empty After Removal).
    // Returns true if key was found and removed, false otherwise.

    bool HashTable::remove(const std::string &key) {
        size_t home = hash(key);
        for (size_t i = 0; i < capacity(); ++i) {
            size_t index = probeIndex(home, i);
            if (table[index].isNormal() && table[index].getKey() == key) {
                table[index].markRemoved();
                --m_size;
                return true;
            }
            if (table[index].isEmptySinceStart()) {
                return false;
            }
            // continue probing through EAR
        }
        return false;
    }

    // Check if a key exists in the table.

    bool HashTable::contains(const std::string &key) const {
        return get(key).has_value();
    }

    // Retrieve the value associated with a key, if present.
    // Returns std::optional<size_t> to indicate presence or absence.
    std::optional<size_t> HashTable::get(const std::string &key) const {
        // Compute the home index using the hash function
        size_t home = hash(key);

        // Begin linear probing using precomputed offset sequence
        for (size_t i = 0; i < capacity(); ++i) {
            size_t index = probeIndex(home, i);

            // Case 1: Found a NORMAL bucket with matching key — return its value
            if (table[index].isNormal() && table[index].getKey() == key) {
                return table[index].getValue();
            }

            // Case 2: Hit an ESS (Empty Since Start) — key was never inserted, terminate search
            if (table[index].isEmptySinceStart()) {
                return std::nullopt;
            }

            // Case 3: Hit an EAR (Empty After Removal) — continue probing
        }

        // Exhausted all probe attempts without finding the key — return nullopt
        return std::nullopt;
    }


    // Retrieve the value associated with a key, if present.
    // Returns std::optional<size_t> to indicate presence or absence.
    size_t &HashTable::operator[](const std::string &key) {
        //Initial check of alpha to ensure table size before access
        if (alpha() >= 0.5) {
            resize();
        }

        //declaring limiters for traversal
        size_t home = hash(key);
        size_t first_empty_spot = capacity(); // sentinel
        /*
        //probing the this table for the requested index, incrementing through the loop and returning the value
        //if the key is found, or the first empty spot if the key is not found and the found empty spot is ESS
        */
        for (size_t i = 0; i < capacity(); ++i) {
            size_t index = probeIndex(home, i);
            if (table[index].isNormal() && table[index].getKey() == key) {
                return table[index].getValueRef();
            }
            if (table[index].isEmpty() && first_empty_spot == capacity()) {
                first_empty_spot = index;
            }
            if (table[index].isEmptySinceStart()) {
                break;
            }
            // continue probing through EAR
        }
        /*
        // Access or insert a key-value pair using bracket notation.
        // If key is missing, inserts with default value 0 and returns reference.
        */
        if (first_empty_spot != capacity()) {
            table[first_empty_spot].load(key, 0);
            ++m_size;
            return table[first_empty_spot].getValueRef();
        }

        resize();
        return (*this)[key];
    }

    // Return a vector of all keys currently stored in NORMAL buckets.

    std::vector<std::string> HashTable::keys() const {
        std::vector<std::string> result;
        for (const auto &bucket: table) {
            if (bucket.isNormal()) {
                result.push_back(bucket.getKey());
            }
        }
        return result;
    }

    // Find and return current occupancy ratio (alpha = size / capacity).

    double HashTable::alpha() const {
        if (capacity() == 0) return 0.0;
        return static_cast<double>(this->m_size) / static_cast<double>(capacity());
    }

    // Return tables current total possible capacity.

    size_t HashTable::capacity() const {
        return table.size();
    }

    // return tables current total occupancies (normals)

    size_t HashTable::size() const {
        return this->m_size;
    }

    // Rehash all occupants in reverse ASCII-sum order of keys
    // deterministic reordering tool

    void HashTable::rehashBackwards() {
        // Collect all NORMAL buckets into a temporary vector of key-value pairs
        std::vector<std::pair<std::string, size_t> > keyValuePairs;
        for (const auto &bucket: table) {
            if (bucket.isNormal()) {
                keyValuePairs.push_back({bucket.getKey(), bucket.getValue()});
            }
        }

        // Sort the key-value pairs in descending order based on ASCII sum of the key string
        std::ranges::sort(keyValuePairs, [](const auto &a, const auto &b) {
            int sumA = 0, sumB = 0;
            for (char c: a.first) sumA += c;
            for (char c: b.first) sumB += c;
            return sumA > sumB;
        });

        // Clear the current table and reset size counter
        table.assign(capacity(), HashTableBucket());
        m_size = 0;

        // Reinsert sorted key-value pairs into the table using standard insert logic
        for (const auto &pair: keyValuePairs) {
            insert(pair.first, pair.second);
        }
    }

    // Dump current table state to JSON for debugging and forensic inspection.

    void HashTable::debugDumpToJSON() {
        // Static counter to uniquely name each dump file (e.g., hashtable_dump_0.json, hashtable_dump_1.json, ...)
        static int dumpCount = 0;

        // Open output file stream with incremented filename
        std::ofstream file("hashtable_dump_" + std::to_string(dumpCount++) + ".json");

        file << "{\n";

        // Write global metadata: capacity, current size, and load factor
        file << "  \"capacity\": " << capacity() << ",\n";
        file << "  \"size\": " << m_size << ",\n";
        file << "  \"load_factor\": " << alpha() << ",\n";

        // Begin array of bucket entries
        file << "  \"buckets\": [\n";

        // Iterate through each bucket in the table
        for (size_t i = 0; i < table.size(); ++i) {
            // Add comma separator for all but the first entry
            if (i > 0) file << ",\n";
            // Begin JSON object for this bucket
            file << "    {\"index\": " << i << ", ";

            // If the bucket is normal write its state and contents
            if (table[i].isNormal()) {
                file << "\"key\": \"" << table[i].getKey() << "\", ";
                file << "\"value\": " << table[i].getValue() << ", ";
                file << "\"state\": \"NORMAL\"";
            }

            // If bucket has never been used
            else if (table[i].isEmptySinceStart()) {
                file << "\"state\": \"ESS\"";
            }

            // If bucket was used and then removed (tombstone)
            else {
                file << "\"state\": \"EAR\"";
            }

            // Close bucket object
            file << "}";
        }

        // Close buckets array and JSON object
        file << "\n  ]\n}\n";
    }

    // Overloaded stream operator for HashTable
    // Prints only NORMAL buckets with their index and key-value pair
    std::ostream &operator<<(std::ostream &os, const HashTable &ht) {
        for (size_t i = 0; i < ht.table.size(); ++i) {
            if (ht.table[i].isNormal()) {
                os << "Bucket " << i << ": <" << ht.table[i].getKey()
                        << ", " << ht.table[i].getValue() << ">\n";
            }
        }
        return os;
    }
} //namespace std
