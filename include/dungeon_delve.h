/* ===========================================================================
   dungeon_delve.h -- Build 3 entity hierarchy.
   =========================================================================== */

#ifndef DUNGEON_DELVE_H
#define DUNGEON_DELVE_H

#include <memory>
#include <ostream>
#include <string>
#include <vector>

/*
   Describable is the shared top of the hierarchy. The name stays here so
   existing engine code can continue to use hero.name and monster.name.
*/
struct Describable {
    char name[32]{};
    virtual ~Describable() = default;
    virtual void describe(std::ostream& out) const = 0;
};

/*
   The diamond is intentional:

       Entity -------+
       Combatant ----+----> Describable
       Lootable -----+

   Each branch uses virtual inheritance, so a Monster that inherits Entity,
   Combatant, and Lootable contains exactly one shared Describable subobject.
   This avoids duplicate name/description state and makes conversion to
   Describable unambiguous through the different interface paths.
*/
struct Entity : public virtual Describable {
    virtual ~Entity() = default;
};

struct Combatant : public virtual Describable {
    int hp{};
    int max_hp{};
    int strength{};
    int armor{};
    virtual ~Combatant() = default;
};

struct Lootable : public virtual Describable {
    virtual const char* loot_kind() const { return "lootable"; }
    virtual ~Lootable() = default;
};

struct Hero : public Entity, public Combatant {
    int gold{};
    int xp{};
    int level{};
    int potions{};
    void describe(std::ostream& out) const override;
};

struct Monster : public Entity, public Combatant, public Lootable {
    char species[24]{};
    int gold_drop{};
    int ability_tag{};
    int is_dead{};
    int summon_budget{};
    void describe(std::ostream& out) const override;
};

struct Merchant : public Entity, public Lootable {
    int gold{};
    int markup_percent{};
    int stock_tags[8]{};
    int stock_count{};
    void describe(std::ostream& out) const override;
};

/* A non-owning polymorphic roster. Entity& accepts Hero, Monster, Merchant. */
void describe_as(const Describable& item, std::ostream& out);
int inspect_combatant(const Combatant& item);
const char* inspect_lootable(const Lootable& item);

class Roster {
public:
    void add(Entity& entity) { entities_.push_back(&entity); }
    void describe_all(std::ostream& out) const;
private:
    std::vector<Entity*> entities_;
};

using MonsterPtr  = std::unique_ptr<Monster>;
using HeroPtr     = std::unique_ptr<Hero>;
using MerchantPtr = std::unique_ptr<Merchant>;
using Wave        = std::vector<MonsterPtr>;
using Bestiary    = std::vector<MonsterPtr>;

inline Monster* raw(Monster* p)                        { return p; }
inline Monster* raw(const std::unique_ptr<Monster>& p) { return p.get(); }
inline Monster* raw(const std::shared_ptr<Monster>& p) { return p.get(); }

#define ABIL_NONE    0
#define ABIL_CLEAVE  1
#define ABIL_BITE    2
#define ABIL_MEND    3
#define ABIL_HEX     4
#define ABIL_SUMMON  5
#define ABIL_GUARD   6
#define ABIL_RALLY   7

void dd_seed(unsigned s);
int  dd_rand();
int  dd_rand_range(int lo, int hi);
int  roll_dice(int count, int sides);

const char* loot_at(int i);
int         loot_count();
const char* roll_loot();
int         loot_value_of(const char* loot_name);

HeroPtr     make_hero(const char* name, int hp, int strength, int armor);
MonsterPtr  make_monster(const char* name, const char* species, int hp,
                         int strength, int armor, int gold_drop, int ability_tag);
MerchantPtr make_merchant(const char* name, int gold, int markup);

int compute_raw_damage(int strength, int dice_count, int dice_sides);
int apply_armor(int raw_damage, int armor);
int apply_damage_to_monster(Monster& m, int incoming);
int apply_damage_to_hero(Hero& h, int incoming);
int resolve_attack_on_monster(Hero& attacker, Monster& target);
int resolve_attack_on_hero(Monster& attacker, Hero& target);

const char* ability_name(int tag);
int resolve_ability(int tag, Monster& self, Hero& hero, Wave& wave, int verbose);

Bestiary   build_bestiary();
Monster*   find_in_bestiary(Bestiary& bestiary, const char* species);
int        count_species_in_bestiary(Bestiary& bestiary, const char* species);
MonsterPtr spawn_from_bestiary(Bestiary& bestiary, const char* species);

int  wave_is_cleared(Wave& wave);
int  living_in_wave(Wave& wave);
int  run_monster_round(Wave& wave, Hero& hero, int verbose,
                       std::vector<std::string>* acted_names);
int  run_hero_round(Wave& wave, Hero& hero, int verbose);
int  run_encounter(Hero& hero, Wave& wave, int verbose);
Wave build_wave(Bestiary& bestiary, const char* a, const char* b, const char* c);

int  xp_for_next_level(int level);
void check_level_up(Hero& h, int verbose);
int  merchant_price(Merchant& mer, int base_value);
void visit_merchant(Merchant& mer, Hero& hero, int verbose);
void quaff_potion(Hero& h, int verbose);

int     save_hero(Hero& h, const char* path);
HeroPtr load_hero(const char* path);

void print_banner();
void print_hero(Hero& h);
void print_wave(Wave& wave);
void print_bestiary(Bestiary& bestiary);
void print_loot_table();
void print_roster_demo();

int run_campaign(int verbose);

#endif /* DUNGEON_DELVE_H */
