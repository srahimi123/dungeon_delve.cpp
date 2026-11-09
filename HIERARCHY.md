# Build 3 — Entity Hierarchy

## Hierarchy diagram

```text
                         Describable
                    ^         ^         ^
             virtual |  virtual|  virtual|
                    /          |          \\
                Entity      Combatant    Lootable
               ^  ^  ^       ^   ^        ^   ^
              /   |   \\     /     \\      /     \\
           Hero Monster Merchant Hero  Monster Monster Merchant
```

The concrete classes declare these direct bases:

```text
Hero     : Entity, Combatant
Monster  : Entity, Combatant, Lootable
Merchant : Entity, Lootable
```

`Entity`, `Combatant`, and `Lootable` each virtually inherit `Describable`. This is my most debatable decision because the shared name and description interface could have lived only in `Entity`, but putting `Describable` above all three branches makes the diamond explicit and gives every capability the same description subobject. `Monster` inherits all three branches, so virtual inheritance is necessary to ensure its `Entity`, `Combatant`, and `Lootable` paths lead to one `Describable` subobject instead of duplicate name/description state. The roster stores non-owning `Entity*` values and calls the virtual `describe()` function.

## Substitutability — one sentence per inheritance edge

- `Describable` → `Entity`: `describe_as(const Describable&, std::ostream&)` accepts an `Entity` and calls its virtual description function.
- `Describable` → `Combatant`: `inspect_combatant(const Combatant&)` works with a `Hero` or `Monster` through the combatant interface.
- `Describable` → `Lootable`: `inspect_lootable(const Lootable&)` works with a `Monster` or `Merchant` through the lootable interface.
- `Entity` → `Hero`: `Roster::add(Entity&)` accepts a `Hero` and later calls its overridden `describe()`.
- `Entity` → `Monster`: `Roster::add(Entity&)` accepts a `Monster` and later calls its overridden `describe()`.
- `Entity` → `Merchant`: `Roster::add(Entity&)` accepts a `Merchant` and later calls its overridden `describe()`.
- `Combatant` → `Hero`: `inspect_combatant(const Combatant&)` accepts a `Hero` and reads its inherited HP field.
- `Combatant` → `Monster`: `inspect_combatant(const Combatant&)` accepts a `Monster` and reads its inherited HP field.
- `Lootable` → `Monster`: `inspect_lootable(const Lootable&)` accepts a `Monster` and returns its loot interface label.
- `Lootable` → `Merchant`: `inspect_lootable(const Lootable&)` accepts a `Merchant` and returns its loot interface label.

The existing combat engine continues to use the derived objects' inherited public fields, while the new roster demonstrates dispatch through the `Entity` base. No `dynamic_cast`, type tags, or polymorphic ability system are added.
