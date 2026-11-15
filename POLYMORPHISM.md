# Build Sprint 04 — Polymorphism Report

## Resolver comparison

| Old tag-switch resolver | New polymorphic resolver |
|---|---|
| **101 lines** | **13 lines** |
| Selects behavior with `switch (tag)` and eight cases plus a default. | Calls the virtual `apply()` function through `const Ability&`. |
| The resolver knows every concrete ability. | The resolver does not know any concrete ability type. |

### Old resolver (101 lines)

```cpp
int resolve_ability(int tag, Monster& self, Hero& hero,
                    Wave& wave, int verbose)
{
    int amount = 0;

    switch (tag) {

        case ABIL_NONE:
            amount = resolve_attack_on_hero(self, hero);
            if (verbose)
                std::printf("  %s flails .................. %d damage\n",
                            self.name, amount);
            break;

        case ABIL_CLEAVE:
            amount = resolve_attack_on_hero(self, hero);
            amount = amount + (amount / 2);
            apply_damage_to_hero(hero, amount / 2);
            if (verbose)
                std::printf("  %s cleaves ................. %d damage\n",
                            self.name, amount);
            break;

        case ABIL_BITE:
            amount = resolve_attack_on_hero(self, hero);
            if (dd_rand_range(1, 100) > 70) {
                apply_damage_to_hero(hero, 2);
                amount += 2;
                if (verbose)
                    std::printf("  %s bites deep ............. %d damage\n",
                                self.name, amount);
            } else {
                if (verbose)
                    std::printf("  %s bites ................... %d damage\n",
                                self.name, amount);
            }
            break;

        case ABIL_MEND:
            amount = roll_dice(2, 4);
            self.hp += amount;
            if (self.hp > self.max_hp) self.hp = self.max_hp;
            if (verbose)
                std::printf("  %s mends ................... +%d HP\n",
                            self.name, amount);
            break;

        case ABIL_HEX:
            amount = roll_dice(1, 4);
            hero.strength -= amount;
            if (hero.strength < 1) hero.strength = 1;
            if (verbose)
                std::printf("  %s hexes ................... -%d STR\n",
                            self.name, amount);
            break;

        case ABIL_SUMMON:
            if (self.summon_budget > 0) {
                self.summon_budget--;
                MonsterPtr pup = make_monster("Whelp", "whelp", 6, 2, 0, 3, ABIL_NONE);
                wave.push_back(std::move(pup));
                amount = 1;
                if (verbose)
                    std::printf("  %s summons a Whelp!\n", self.name);
            } else {
                amount = resolve_attack_on_hero(self, hero);
                if (verbose)
                    std::printf("  %s is out of whelps ....... %d damage\n",
                                self.name, amount);
            }
            break;

        case ABIL_GUARD:
            self.armor += 2;
            amount = 2;
            if (verbose)
                std::printf("  %s guards .................. +2 armor\n",
                            self.name);
            break;

        case ABIL_RALLY:
            for (size_t i = 0; i < wave.size(); i++) {
                if (!wave[i]->is_dead && raw(wave[i]) != &self)
                    wave[i]->strength += 1;
            }
            amount = 1;
            if (verbose)
                std::printf("  %s rallies the pack ........ +1 STR each\n",
                            self.name);
            break;

        default:
            amount = resolve_attack_on_hero(self, hero);
            if (verbose)
                std::printf("  %s does something ......... %d damage\n",
                            self.name, amount);
            break;
    }

    return amount;
}
```

### New resolver (13 lines)

```cpp
int resolve_ability(const Ability& ability, Monster& self, Hero& hero,
                    Wave& wave, int verbose)
{
    try {
        return ability.apply(self, hero, wave, verbose);
    } catch (const std::exception&) {
        std::throw_with_nested(AbilityResolutionError(
            std::string("ability '") + ability.describe() + "' failed for " + self.name));
    } catch (...) {
        std::throw_with_nested(AbilityResolutionError(
            std::string("ability '") + ability.describe() + "' failed for " + self.name));
    }
}
```

The monster-round dispatch statement is one line:

```cpp
total += resolve_ability_safely(*m->ability, *m, hero, wave, verbose);
```

That path has **0 type checks and 0 casts**. The ability is stored as a `std::shared_ptr<const Ability>` and the call to `apply()` is virtual.

## Inherited virtual function

`Ability::describe()` is pure virtual. `AttackAbility` overrides it once, and `FlailAbility`, `CleaveAbility`, and `BiteAbility` override it again. The second-level overrides remain virtual without writing the `virtual` keyword again.

## Concrete abilities

The engine now has Flail, Cleave, Bite, Mend, Hex, Summon, Guard, Rally, and Shatter. These perform different actions: normal and boosted attacks, a chance-based bite, healing, weakening the hero, summoning, armor increase, team strength increase, and armor-breaking raw damage.

## Exception handling and layered passing

A summon can fail while allocating a Whelp or growing the wave. `SummonAbility` catches `std::bad_alloc` and passes it upward as a nested `AbilityFailure`. `resolve_ability()` wraps ability exceptions in `AbilityResolutionError` while preserving the original exception. The encounter-round boundary catches that error, reports which monster and ability failed, and allows the round to continue. The monster factory also rejects a null ability with `std::invalid_argument`.

## Open/closed test

I added Shatter as an extension test. Adding it touched **3 existing files**:

1. `include/dungeon_delve.h` — declare the Shatter factory.
2. `src/abilities.cpp` — implement the new concrete ability and its factory.
3. `src/dungeon_delve.cpp` — add it to the bestiary.

`CMakeLists.txt` did not need another edit because `src/abilities.cpp` is already part of both engine targets. This is not a zero-file change, but it keeps the resolver and build configuration closed to modification. Separately, I migrated the existing regression test setup calls from integer tags to ability factories; the three test assertions were not changed.

## What got worse

The new design has more moving parts than the switch. Each monster now carries a shared pointer, ability behavior is spread across a separate implementation file, and resolving an ability adds an indirect virtual call. The exception wrappers also make the failure path longer and harder to trace than a direct return. The benefit is that the resolver no longer needs edits every time a new ability is added.

## Verification

- C++20 build: passed.
- `encounter_resolves`: passed.
- `loot_table_intact`: passed.
- `roster_survives_reallocation`: passed.
- Quiet campaign run: cleared 6 / 6 stages.
