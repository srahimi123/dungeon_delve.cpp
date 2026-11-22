#ifndef DD_DATA_LAYER_H
#define DD_DATA_LAYER_H

#include <algorithm>
#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

// A party member is stored by value: roster iteration and sorting are common,
// and the records are small. Sorting returns a view copy, not a mutation.
struct PartyMember {
    std::string name;
    int hp{};
    int level{};
};

using PartyCompare = bool (*)(const PartyMember&, const PartyMember&);

inline bool by_name(const PartyMember& a, const PartyMember& b) {
    return a.name < b.name;
}

struct ByHP {
    bool operator()(const PartyMember& a, const PartyMember& b) const {
        if (a.hp != b.hp) return a.hp > b.hp;
        return a.name < b.name;
    }
};

class PartyRoster {
public:
    void add(PartyMember member) { members_.push_back(std::move(member)); }
    std::size_t size() const noexcept { return members_.size(); }
    const std::vector<PartyMember>& members() const noexcept { return members_; }

    template <typename Compare>
    std::vector<PartyMember> sorted_by(Compare compare) const {
        auto result = members_;
        std::sort(result.begin(), result.end(), compare);
        return result;
    }
private:
    std::vector<PartyMember> members_;
};

// Registry is for named lookup; map gives logarithmic key lookup.
template <typename T>
class Registry {
public:
    bool insert(std::string key, T value) {
        return entries_.emplace(std::move(key), std::move(value)).second;
    }
    T* find(const std::string& key) {
        auto it = entries_.find(key);
        return it == entries_.end() ? nullptr : &it->second;
    }
    const T* find(const std::string& key) const {
        auto it = entries_.find(key);
        return it == entries_.end() ? nullptr : &it->second;
    }
    std::size_t size() const noexcept { return entries_.size(); }
    auto begin() noexcept { return entries_.begin(); }
    auto end() noexcept { return entries_.end(); }
    auto begin() const noexcept { return entries_.begin(); }
    auto end() const noexcept { return entries_.end(); }
private:
    std::map<std::string, T> entries_;
};

// Pool is for dense iteration and append; vector gives contiguous storage.
template <typename T>
class Pool {
public:
    void add(T value) { items_.push_back(std::move(value)); }
    std::size_t size() const noexcept { return items_.size(); }
    T& at(std::size_t index) { return items_.at(index); }
    const T& at(std::size_t index) const { return items_.at(index); }
    auto begin() noexcept { return items_.begin(); }
    auto end() noexcept { return items_.end(); }
    auto begin() const noexcept { return items_.begin(); }
    auto end() const noexcept { return items_.end(); }
private:
    std::vector<T> items_;
};

class SpeciesSeen {
public:
    bool record(const std::string& species) { return species_.insert(species).second; }
    std::size_t size() const noexcept { return species_.size(); }
    bool contains(const std::string& species) const { return species_.find(species) != species_.end(); }
private:
    std::set<std::string> species_;
};

// Adapter for the intentionally unchanged raw C-array loot table.
struct LegacyLootBridge {
    static int load(const char* const* raw, int count, Pool<std::string>& out) {
        if (count < 0) return -1;
        for (int i = 0; i < count; ++i) {
            if (raw[i] == nullptr) return -1;
        }
        for (int i = 0; i < count; ++i) out.add(raw[i]);
        return count;
    }
    static int store(const Pool<std::string>& source, const char** dest, int capacity) {
        if (capacity < 0 || static_cast<std::size_t>(capacity) < source.size() ||
            (source.size() != 0 && dest == nullptr)) return -1;
        for (std::size_t i = 0; i < source.size(); ++i) dest[i] = source.at(i).c_str();
        return static_cast<int>(source.size());
    }
};

#endif
