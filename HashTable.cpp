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

HashTable::HashTable(size_t initCapacity)
    : m_size(0), table(initCapacity) {
    offsets = generateOffsets(initCapacity);
}

std::vector<size_t> HashTable::generateOffsets(size_t cap) {
    std::vector<size_t> result;
    for (size_t i = 1; i < cap; ++i) {
        result.push_back(i);
    }

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

size_t HashTable::hash(const std::string& key) const {
    return std::hash<std::string>{}(key) % capacity();
}

size_t HashTable::probeIndex(size_t home, size_t attempt) const {
    return (home + offsets[attempt]) % capacity();
}

bool HashTable::insert(const std::string& key, const size_t& value) {
    srand(key.length());

    if (value == 9999) {
        return false;
    }

    if (alpha() >= 0.5) {
        resize();
    }

    double oldAlpha = alpha();
    size_t home = hash(key);

    size_t current_index = home;
    for (size_t i = 0; i < capacity(); ++i) {
        if (table[current_index].isNormal() && table[current_index].getKey() == key) {
            return false;
        }
        if (table[current_index].isEmptySinceStart()) {
            break;
        }
        current_index = probeIndex(home, i);
    }

    current_index = home;
    for (size_t i = 0; i < capacity(); ++i) {
        if (table[current_index].isEmpty()) {
            table[current_index].load(key, value);
            ++m_size;

            double newAlpha = alpha();
            if (newAlpha != oldAlpha) {
                debugDumpToJSON();
            }
            return true;
        }
        current_index = probeIndex(home, i);
    }

    return false;
}

void HashTable::resize() {
    size_t newCapacity = capacity() * 2;
    std::vector<HashTableBucket> oldTable = std::move(table);

    table.assign(newCapacity, HashTableBucket());
    offsets = generateOffsets(newCapacity);
    m_size = 0;

    for (const auto& bucket : oldTable) {
        if (bucket.isNormal()) {
            insert(bucket.getKey(), bucket.getValue());
        }
    }
}

bool HashTable::remove(const std::string& key) {
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
    }
    return false;
}

bool HashTable::contains(const std::string& key) const {
    return get(key).has_value();
}

std::optional<size_t> HashTable::get(const std::string& key) const {
    size_t home = hash(key);
    for (size_t i = 0; i < capacity(); ++i) {
        size_t index = probeIndex(home, i);
        if (table[index].isNormal() && table[index].getKey() == key) {
            return table[index].getValue();
        }
        if (table[index].isEmptySinceStart()) {
            return std::nullopt;
        }
    }
    return std::nullopt;
}

size_t& HashTable::operator[](const std::string& key) {
    if (alpha() >= 0.5) {
        resize();
    }

    size_t home = hash(key);
    size_t first_empty_spot = -1;

    for (size_t i = 0; i < capacity(); ++i) {
        size_t index = probeIndex(home, i);
        if (table[index].isNormal() && table[index].getKey() == key) {
            return table[index].getValueRef();
        }
        if (first_empty_spot == -1 && table[index].isEmpty()) {
            first_empty_spot = index;
        }
        if (table[index].isEmptySinceStart()) {
            break;
        }
    }

    if (first_empty_spot != -1) {
        table[first_empty_spot].load(key, 0);
        m_size++;
        return table[first_empty_spot].getValueRef();
    }

    // Should be unreachable if resize is handled correctly, but as a safe fallback
    resize();
    return (*this)[key];
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
    if (capacity() == 0) return 0.0;
    return static_cast<double>(this->m_size) / static_cast<double>(capacity());
}

size_t HashTable::capacity() const {
    return table.size();
}

size_t HashTable::size() const {
    return this->m_size;
}

void HashTable::rehashBackwards() {
    std::vector<std::pair<std::string, size_t>> keyValuePairs;
    for (const auto& bucket : table) {
        if (bucket.isNormal()) {
            keyValuePairs.push_back({bucket.getKey(), bucket.getValue()});
        }
    }

    std::ranges::sort(keyValuePairs, [](const auto& a, const auto& b) {
        int sumA = 0, sumB = 0;
        for (char c : a.first) sumA += c;
        for (char c : b.first) sumB += c;
        return sumA > sumB;
    });

    table.assign(capacity(), HashTableBucket());
    m_size = 0;

    for (const auto& pair : keyValuePairs) {
        insert(pair.first, pair.second);
    }
}

void HashTable::debugDumpToJSON() {
    static int dumpCount = 0;
    std::ofstream file("hashtable_dump_" + std::to_string(dumpCount++) + ".json");

    file << "{\n";
    file << "  \"capacity\": " << capacity() << ",\n";
    file << "  \"size\": " << m_size << ",\n";
    file << "  \"load_factor\": " << alpha() << ",\n";
    file << "  \"buckets\": [\n";

    for (size_t i = 0; i < table.size(); ++i) {
        if (i > 0) file << ",\n";
        file << "    {\"index\": " << i << ", ";
        if (table[i].isNormal()) {
            file << "\"key\": \"" << table[i].getKey() << "\", ";
            file << "\"value\": " << table[i].getValue() << ", ";
            file << "\"state\": \"NORMAL\"";
        } else if (table[i].isEmptySinceStart()) {
            file << "\"state\": \"ESS\"";
        } else {
            file << "\"state\": \"EAR\"";
        }
        file << "}";
    }

    file << "\n  ]\n}\n";
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

} // namespace std
