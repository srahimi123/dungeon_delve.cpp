# Build 1 — Baseline and Triage

## 1. The three defects

### Defect 1 — Loot table out-of-bounds access

**How I found it:** I inspected the `roll_loot()` function at line 182 and checked how the random index was selected.

**What was actually wrong:** The random range used `LOOT_COUNT` as its maximum index. Array indexes start at zero, so the largest valid index is `LOOT_COUNT - 1`. The original code could access an element outside the loot table, causing undefined behavior.

**What I changed:** I changed the upper limit of the random range to `LOOT_COUNT - 1`.

### Defect 2 — Armor applied twice

**How I found it:** I traced the damage calculations in `resolve_attack_on_monster()` and `resolve_attack_on_hero()` at lines 235 and 243.

**What was actually wrong:** The attack functions applied armor before calling the damage functions, which also applied armor. This caused armor to be subtracted twice and made attacks deal less damage than intended.

**What I changed:** I changed the attack functions to pass raw damage to the damage functions so armor is applied only once.

### Defect 3 — Vector iterator invalidation

**How I found it:** I inspected `run_monster_round()` at line 429 and checked what happens when a monster summons another monster.

**What was actually wrong:** The monster round used an iterator-based loop while the ability system could add monsters to the same vector. Adding an element can reallocate the vector and invalidate its iterators, causing undefined behavior.

**What I changed:** I changed the loop to use an index and retrieve each monster from the vector. I left `resolve_ability()` unchanged as instructed.

## 2. Ownership table

The original code used raw pointers for heap-allocated objects. I converted the owning pointers to smart pointers so memory is released automatically.

| Allocation site | Who owned it before | Converted to | Why that choice |
|:--|:--|:--|:--|
| `make_hero()` | The returned owner represented the hero's lifetime. | `std::unique_ptr<Hero>` using `std::make_unique`. | The hero has one owner. |
| `make_monster()` | The returned owner represented the monster's lifetime. | `std::unique_ptr<Monster>` using `std::make_unique`. | Each monster has one owner. |
| `make_merchant()` | The returned owner represented the merchant's lifetime. | `std::unique_ptr<Merchant>` using `std::make_unique`. | The merchant has one owner. |
| `load_hero()` | The function allocated and manually deleted the hero on an error path. | `std::unique_ptr<Hero>` using `std::make_unique`. | Automatic cleanup also works when the function returns early. |
| `Wave` and `Bestiary` collections | Collections held raw monster pointers. | `std::vector<std::unique_ptr<Monster>>`. | The collections own their monsters and release them automatically. |
| Summoned monsters | A newly created monster was transferred into the active wave. | Move its `std::unique_ptr` into the wave. | Ownership transfers to the wave. |

**Non-owning pointers and references:** Raw `Monster*` values obtained through `raw()` or lookup functions are used to observe existing monsters. They do not own the objects and must not delete them. The owning `std::unique_ptr` or collection controls each object's lifetime. In the ownership audit, the comments on non-owning pointers also make this distinction clear.

## 3. The three worst things about this codebase

1. **Loot table bounds — `src/dungeon_delve.cpp`, line 182 (`roll_loot()`).** An invalid random index can access memory outside the loot table. I fixed the upper bound. I would also keep boundary-focused regression tests.

2. **Damage calculations — `src/dungeon_delve.cpp`, lines 235 and 243.** Armor was applied twice, making combat damage incorrect. I changed the attack functions so armor is applied only once. I would add tests for damage with and without armor.

3. **Monster vector invalidation — `src/dungeon_delve.cpp`, line 429 (`run_monster_round()`).** Summoning can grow the vector and invalidate iterators. I changed the loop to use indexes and left `resolve_ability()` unchanged as instructed. I would add a regression test that exercises summoning during a monster round.
