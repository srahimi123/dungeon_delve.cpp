#include "dungeon_delve.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <string>

int main() {
    // std::vector roster, sorted three ways using the required comparator forms.
    PartyRoster roster;
    roster.add({"Cato", 61, 2});
    roster.add({"Ash", 100, 5});
    roster.add({"Bree", 84, 4});
    const auto alphabetical = roster.sorted_by(static_cast<PartyCompare>(by_name)); // function pointer
    const auto by_hp = roster.sorted_by(ByHP{});                                   // function object
    const auto by_level = roster.sorted_by([](const PartyMember& a, const PartyMember& b) { // lambda
        if (a.level != b.level) return a.level > b.level;
        return a.name < b.name;
    });
    assert(alphabetical.front().name == "Ash");
    assert(by_hp.front().name == "Ash" && by_hp.back().name == "Cato");
    assert(by_level.front().name == "Ash");

    // Registry<T> instantiated for two different types.
    Registry<int> item_ids;
    assert(item_ids.insert("potion", 7));
    assert(*item_ids.find("potion") == 7);
    Registry<std::string> names;
    assert(names.insert("hero", "Ash"));
    assert(*names.find("hero") == "Ash");

    // Pool<T> instantiated for two different types.
    Pool<int> damage_rolls;
    damage_rolls.add(12);
    assert(damage_rolls.at(0) == 12);
    Pool<std::string> loot_copy;
    assert(LegacyLootBridge::load([&]() {
        static const char* raw[12];
        for (int i = 0; i < loot_count(); ++i) raw[i] = loot_at(i);
        return raw;
    }(), loot_count(), loot_copy) == loot_count());
    assert(loot_copy.size() == static_cast<std::size_t>(loot_count()));
    const char* exported[12]{};
    assert(LegacyLootBridge::store(loot_copy, exported, 12) == loot_count());
    assert(std::strcmp(exported[0], loot_at(0)) == 0);
    assert(exported[0] == loot_copy.at(0).c_str());

    // std::set deduplicates repeated species encounters.
    SpeciesSeen seen;
    const char* encounters[] = {"kobold", "goblin", "bat", "adept", "priest", "warg",
                                "brood", "kobold", "goblin", "bat", "kobold"};
    int duplicates = 0;
    for (const char* species : encounters)
        if (!seen.record(species)) ++duplicates;
    assert(seen.size() == 7);
    assert(duplicates == 4);

    // Production bestiary is a map keyed by species: find is logarithmic.
    Bestiary bestiary = build_bestiary();
    Monster* kobold = find_in_bestiary(bestiary, "kobold");
    assert(kobold != nullptr);
    assert(std::strcmp(kobold->name, "Kobold") == 0);
    assert(count_species_in_bestiary(bestiary, "kobold") == 1);
    assert(find_in_bestiary(bestiary, "not-a-species") == nullptr);
    assert(loot_value_of("Rusted Dagger") == 4);
    assert(loot_value_of("not loot") == 0);

    std::cout << "Build 5 data-layer checks passed\n";
    std::cout << "bestiary.find(\"kobold\"): " << kobold->name << "  "
              << kobold->max_hp << " HP  3d6  lootable  [O(log n)]\n";
    std::cout << "roster.sorted_by(ByHP{}): ";
    for (const auto& member : by_hp) std::cout << member.name << ' ' << member.hp << " | ";
    std::cout << "\nspecies_seen.size(): " << seen.size() << " (11 encounters, "
              << duplicates << " duplicates absorbed)\n";
}
