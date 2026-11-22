# Concept Lab 5 — Generic and Non-Generic, Side by Side

**Name:** Shahzad Rahimi    **Date:** November 20, 2026

---

## Step 1 — Get both sides working.

**Test result:** 0 check(s) failed. All 51 checks passed.

**The two types I instantiated `Satchel<T>` for:**

| | Type | Where it is used |
|:--|:--|:--|
| 1 | `std::string` | Holds the loot names loaded from `LOOT_TABLE`. |
| 2 | `int` | Holds the values from `LOOT_VALUE` using `Satchel<int, 12>`. |

---

## Step 2 — What the template version made easier.

| | What it made easier | The concrete evidence |
|:--|:--|:--|
| 1 | Keeping the entries and their count together | `Satchel::remove_at(int)` only needs the index. The array and count are private members, so I do not have to pass them separately. |
| 2 | Checking whether a removal is valid | `remove_at()` checks the index against `count_`. I do not have to check whether a separate count pointer is null. |
| 3 | Reusing the same operation with different types | I used the same `Satchel` implementation for `std::string` and `int`. I did not need to write a second remove function for integers. |

---

## Step 3 — What the raw version made easier.

| | What it made easier | The concrete evidence |
|:--|:--|:--|
| 1 | Direct access by index | The existing engine can keep using `LOOT_TABLE[i]` in `print_loot_table()` without calling a container method or changing its code. |
| 2 | Editing the list of loot names | The table is a simple list of string pointers, so someone can add or change a name in the table without needing to understand a class template or instantiate `Satchel<std::string>`. |
| 3 | Keeping the table small and simple | On this 64-bit machine, the twelve table entries are twelve pointers (96 bytes for the pointer array). A `Satchel<std::string>` has sixteen `std::string` slots plus a count, so it uses more fixed container storage. The raw table is a good fit for this small, fixed list that the engine already indexes directly. |

---

## Step 4 — The bridge.

**Where in your `load()` is the text copied, and how many times? Name the line.**

*My answer:*

The text is copied into the satchel's own string at `legacy_loot.cpp`, line 111: `*slot = slots[i];`. I get the destination from `out.add_slot()` on line 109, so I do not create a temporary string or call `add()`. The text is copied once into the satchel slot.

**`store()` hands out pointers into the satchel's own strings. Name one way a caller could get a dangling pointer out of that, and the rule that prevents it.**

*My answer:*

A caller could keep one of these pointers after the satchel is destroyed, or after the string is changed in a way that reallocates its character buffer. The caller must only use the pointers while the satchel and its strings stay alive and unchanged. The caller also has to respect that the satchel owns the strings; the caller must not delete the returned pointers.

**The bridge is not allowed to touch `LOOT_TABLE`. If that constraint were lifted — if you could rewrite the loot table into whatever shape you liked — name one thing you would still not change, and why.**

*My answer:*

I would still keep the loot names in one clear, easy-to-edit source list. The content team needs to be able to find and update the names without having to understand the rest of the game code. I would also avoid changing the existing direct-indexing interface until every part of the engine that uses it had been updated.

---

## Step 5 — The closing question.

**Build 5 adds `LegacyLootBridge` to the engine, against the same unmodifiable loot table. Which of the two implementations in this lab is the one you would actually put in Build 5, and what does the other one keep doing after you ship it?**

*My answer:*

I would use the template version, `Satchel`, for the container and the operations in the newer code because it keeps the data and count together and can work with different types. The raw array version still stays because the existing engine and content team use `LOOT_TABLE` directly. `LegacyLootBridge` connects the two: it loads the raw names into a `Satchel` when the newer code needs them and can provide pointers back to the satchel's strings when needed. Keeping both sides means the newer code can use the template without forcing changes to the old table and the code that depends on it.
