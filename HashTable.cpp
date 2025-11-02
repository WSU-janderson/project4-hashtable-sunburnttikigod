#include "HashTable.h"
#include "HashTableBucket.h"
#include <iostream>
#include <vector>
#include <optional>
#include <algorithm>
#include <random>

using namespace std;

HashTable::HashTable(size_t initCapacity)
    : size(0), table(initCapacity) { // Remove capacity from initialization
    offsets = generateOffsets(initCapacity); // Use initCapacity directly
}

std::vector<size_t> HashTable::generateOffsets(size_t cap) { // Rename makeOffsets to generateOffsets
    std::vector<size_t> result;
    for (size_t i = 1; i < cap; ++i) {
        result.push_back(i);
    }
    std::random_device rd;
    std::mt19937 g(rd());
    ranges::shuffle(result, g);
    return result;
}

size_t HashTable::hash(const std::string& key) const {
    return std::hash<std::string>{}(key) % capacity();
}

size_t HashTable::probeIndex(size_t home, size_t attempt) const {
    return (home + offsets[attempt]) % capacity();
}

bool HashTable::insert(const std::string& key, const size_t& value) {
    if (alpha() >= 0.5) resize();

    size_t home = hash(key);
    if (table[home].isEmpty()) {
        table[home].load(key, value);
        ++size;
        return true;
    }

    for (size_t i = 0; i < offsets.size(); ++i) {
        size_t index = probeIndex(home, i);
        if (table[index].isEmpty()) {
            table[index].load(key, value);
            ++size;
            return true;
        }
        if (table[index].isNormal() && table[index].getKey() == key) {
            return false; // duplicate
        }
    }

    return false; // table full
}

void HashTable::resize() {
    size_t newCapacity = capacity() * 2;
    std::vector<HashTableBucket> newTable(newCapacity);
    std::vector<size_t> newOffsets = generateOffsets(newCapacity);

    for (const auto& bucket : table) {
        if (bucket.isNormal()) {
            size_t home = std::hash<std::string>{}(bucket.getKey()) % newCapacity;
            if (newTable[home].isEmpty()) {
                newTable[home].load(bucket.getKey(), bucket.getValue());
            } else {
                for (size_t i = 0; i < newOffsets.size(); ++i) {
                    size_t index = (home + newOffsets[i]) % newCapacity;
                    if (newTable[index].isEmpty()) {
                        newTable[index].load(bucket.getKey(), bucket.getValue());
                        break;
                    }
                }
            }
        }
    }

    table = std::move(newTable);
    offsets = std::move(newOffsets);
    capacity = newCapacity;
}

bool HashTable::remove(const std::string& key) {
    size_t home = hash(key);
    if (table[home].isNormal() && table[home].getKey() == key) {
        table[home].markRemoved();
        --size;
        return true;
    }

    for (size_t i = 0; i < offsets.size(); ++i) {
        size_t index = probeIndex(home, i);
        if (table[index].isNormal() && table[index].getKey() == key) {
            table[index].markRemoved();
            --size;
            return true;
        }
    }

    return false;
}

bool HashTable::contains(const std::string& key) const {
    return get(key).has_value();
}

std::optional<size_t> HashTable::get(const std::string& key) const {
    size_t home = hash(key);
    if (table[home].isNormal() && table[home].getKey() == key) {
        return table[home].getValue();
    }

    for (size_t i = 0; i < offsets.size(); ++i) {
        size_t index = probeIndex(home, i);
        if (table[index].isNormal() && table[index].getKey() == key) {
            return table[index].getValue();
        }
    }

    return std::nullopt;
}

size_t& HashTable::operator[](const std::string& key) {
    size_t home = hash(key);
    if (table[home].isNormal() && table[home].getKey() == key) {
        return table[home].getValueRef();
    }

    for (size_t i = 0; i < offsets.size(); ++i) {
        size_t index = probeIndex(home, i);
        if (table[index].isNormal() && table[index].getKey() == key) {
            return table[index].getValueRef();
        }
    }

    throw std::runtime_error("Key not found");
}

std::vector<std::string> HashTable::keys() const {
    std::vector<std::string> result;
    for (const auto& bucket : table) {
        if (bucket.isNormal()) {
            result.push_back(bucket.getKey());
        }
    }
    return result;
}

double HashTable::alpha() const {
    return static_cast<double>(HashTable::size()) / static_cast<double>(capacity());
}

size_t HashTable::capacity() const {
    return table.size(); // Return the size of the table vector
}


size_t HashTable::size() const {
    return size;
}

std::ostream& operator<<(std::ostream& os, const HashTable& ht) {
    for (size_t i = 0; i < ht.table.size(); ++i) {
        if (ht.table[i].isNormal()) {
            os << "Bucket " << i << ": <" << ht.table[i].getKey()
               << ", " << ht.table[i].getValue() << ">\n";
        }
    }
    return os;
}

} // namespace sunburntTikiGod
