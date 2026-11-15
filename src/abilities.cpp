#include "dungeon_delve.h"

#include <cstdio>
#include <exception>
#include <new>
#include <utility>

namespace {

/* This intermediate class overrides describe(). Flail, Cleave, and Bite
   override it again, showing that the virtual function stays virtual at the
   next inheritance level. */
class AttackAbility : public Ability {
public:
    const char* describe() const override { return "Attack"; }
};

class FlailAbility final : public AttackAbility {
public:
    const char* describe() const override { return "Flail"; }
    int apply(Monster& self, Hero& hero, Wave&, int verbose) const override
    {
        const int amount = resolve_attack_on_hero(self, hero);
        if (verbose)
            std::printf("  %s flails .................. %d damage\n", self.name, amount);
        return amount;
    }
};

class CleaveAbility final : public AttackAbility {
public:
    const char* describe() const override { return "Cleave"; }
    int apply(Monster& self, Hero& hero, Wave&, int verbose) const override
    {
        int amount = resolve_attack_on_hero(self, hero);
        amount += amount / 2;
        apply_damage_to_hero(hero, amount / 2);
        if (verbose)
            std::printf("  %s cleaves ................. %d damage\n", self.name, amount);
        return amount;
    }
};

class BiteAbility final : public AttackAbility {
public:
    const char* describe() const override { return "Bite"; }
    int apply(Monster& self, Hero& hero, Wave&, int verbose) const override
    {
        int amount = resolve_attack_on_hero(self, hero);
        if (dd_rand_range(1, 100) > 70) {
            apply_damage_to_hero(hero, 2);
            amount += 2;
            if (verbose)
                std::printf("  %s bites deep ............. %d damage\n", self.name, amount);
        } else if (verbose) {
            std::printf("  %s bites ................... %d damage\n", self.name, amount);
        }
        return amount;
    }
};

class MendAbility final : public Ability {
public:
    const char* describe() const override { return "Mend"; }
    int apply(Monster& self, Hero&, Wave&, int verbose) const override
    {
        const int amount = roll_dice(2, 4);
        self.hp += amount;
        if (self.hp > self.max_hp) self.hp = self.max_hp;
        if (verbose)
            std::printf("  %s mends ................... +%d HP\n", self.name, amount);
        return amount;
    }
};

class HexAbility final : public Ability {
public:
    const char* describe() const override { return "Hex"; }
    int apply(Monster& self, Hero& hero, Wave&, int verbose) const override
    {
        const int amount = roll_dice(1, 4);
        hero.strength -= amount;
        if (hero.strength < 1) hero.strength = 1;
        if (verbose)
            std::printf("  %s hexes ................... -%d STR\n", self.name, amount);
        return amount;
    }
};

class SummonAbility final : public Ability {
public:
    const char* describe() const override { return "Summon"; }
    int apply(Monster& self, Hero& hero, Wave& wave, int verbose) const override
    {
        if (self.summon_budget > 0) {
            try {
                MonsterPtr whelp = make_monster("Whelp", "whelp", 6, 2, 0, 3,
                                                make_flail_ability());
                wave.push_back(std::move(whelp));
            } catch (const std::bad_alloc&) {
                std::throw_with_nested(AbilityFailure("could not allocate summoned Whelp"));
            }
            --self.summon_budget;
            if (verbose)
                std::printf("  %s summons a Whelp!\n", self.name);
            return 1;
        }

        const int amount = resolve_attack_on_hero(self, hero);
        if (verbose)
            std::printf("  %s is out of whelps ....... %d damage\n", self.name, amount);
        return amount;
    }
};

class GuardAbility final : public Ability {
public:
    const char* describe() const override { return "Guard"; }
    int apply(Monster& self, Hero&, Wave&, int verbose) const override
    {
        self.armor += 2;
        if (verbose)
            std::printf("  %s guards .................. +2 armor\n", self.name);
        return 2;
    }
};

class RallyAbility final : public Ability {
public:
    const char* describe() const override { return "Rally"; }
    int apply(Monster& self, Hero&, Wave& wave, int verbose) const override
    {
        for (const auto& member : wave) {
            if (!member->is_dead && raw(member) != &self)
                member->strength += 1;
        }
        if (verbose)
            std::printf("  %s rallies the pack ........ +1 STR each\n", self.name);
        return 1;
    }
};

class ShatterAbility final : public Ability {
public:
    const char* describe() const override { return "Shatter"; }
    int apply(Monster& self, Hero& hero, Wave&, int verbose) const override
    {
        const int damage = self.strength + dd_rand_range(1, 6);
        hero.hp -= damage;
        if (hero.hp < 0) hero.hp = 0;
        if (hero.armor > 0) --hero.armor;
        if (verbose)
            std::printf("  %s shatters ............... %d damage\n", self.name, damage);
        return damage;
    }
};

} // namespace

std::shared_ptr<const Ability> make_flail_ability()
{
    static const auto ability = std::make_shared<const FlailAbility>();
    return ability;
}

std::shared_ptr<const Ability> make_cleave_ability()
{
    static const auto ability = std::make_shared<const CleaveAbility>();
    return ability;
}

std::shared_ptr<const Ability> make_bite_ability()
{
    static const auto ability = std::make_shared<const BiteAbility>();
    return ability;
}

std::shared_ptr<const Ability> make_mend_ability()
{
    static const auto ability = std::make_shared<const MendAbility>();
    return ability;
}

std::shared_ptr<const Ability> make_hex_ability()
{
    static const auto ability = std::make_shared<const HexAbility>();
    return ability;
}

std::shared_ptr<const Ability> make_summon_ability()
{
    static const auto ability = std::make_shared<const SummonAbility>();
    return ability;
}

std::shared_ptr<const Ability> make_guard_ability()
{
    static const auto ability = std::make_shared<const GuardAbility>();
    return ability;
}

std::shared_ptr<const Ability> make_rally_ability()
{
    static const auto ability = std::make_shared<const RallyAbility>();
    return ability;
}

std::shared_ptr<const Ability> make_shatter_ability()
{
    static const auto ability = std::make_shared<const ShatterAbility>();
    return ability;
}
