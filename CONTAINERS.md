# Build 5 — Container Choices

## Party roster: `std::vector`

The party roster is appended to and traversed for display and sorting. It does
not need frequent insertion in the middle or key-based lookup. `std::vector`
keeps the members contiguous, supports constant-time indexing, and works with
`std::sort`. `PartyRoster::sorted_by` returns a sorted copy so one view does not
reorder the original roster.

The roster demonstrates three comparator forms:

- Function pointer: `by_name`.
- Function object: `ByHP`.
- Lambda: descending level, with name as a tie-breaker.

## Bestiary: `std::map`

The engine asks for a monster prototype by its species key. This is a lookup
operation, not a sequential display operation, so `std::map<std::string,
MonsterPtr>` is appropriate: `find` is logarithmic, `O(log n)`. The production
`find_in_bestiary` now uses the map key directly instead of comparing every
species in a loop. The map is also used to build the loot-name/value lookup
index, while the original raw loot arrays remain intact for legacy callers.

## Encountered species: `std::set`

The engine needs the collection of distinct species encountered, not a count
of every encounter. `std::set<std::string>` maintains unique keys, so inserting
an already-seen species returns `false` and does not add a duplicate. The
acceptance test records 11 encounters and verifies 7 unique species, meaning
4 duplicate insertions were absorbed.

## `Registry<T>` and `Pool<T>`

`Registry<T>` wraps a map because its primary operation is finding a value by a
stable string key. The checks instantiate it as `Registry<int>` and
`Registry<std::string>`.

`Pool<T>` wraps a vector because its primary operations are append and indexed
access during iteration. The checks instantiate it as `Pool<int>` and
`Pool<std::string>`.

## `LegacyLootBridge`

The bridge loads the existing raw loot names into `Pool<std::string>` and can
write `const char*` views back out, pointing at the strings owned by the pool.
It does not alter, reorder, reallocate, or free the engine's original
`LOOT_TABLE` or `LOOT_VALUE` arrays. The caller must keep the pool alive and
must not mutate its strings while using exported pointers.

## Discussion 5 timings

The following are the actual results posted for the 100,000-element Discussion 5
harness. They were measured on a MacBook Air with an Apple M2 and 8 GB memory,
using Apple Clang 21.0.0, C++20, and `-O2` optimization.

| Container | Insert (ms) | Find (ms) | Iterate (ms) |
|---|---:|---:|---:|
| `std::vector` | 0.618 | 2627.10 | 0.108 |
| `std::list` | 2.100 | 9156.03 | 0.188 |
| `std::set` | 21.914 | 10.09 | 2.250 |
| `std::map` | 20.543 | 11.60 | 1.878 |

The result that surprised me most was the size of the vector-versus-set find
difference: vector took 2627.10 ms while set took 10.09 ms. This supports using
an indexed associative container when the engine needs repeated lookup by a
key, rather than scanning every element. The results did not change the main
design choices: the party roster is appended to, traversed, and sorted into
views, so vector remains suitable; the bestiary is searched by species key, so
map remains suitable; and encountered species must be unique, so set remains
suitable. The timings are from the supplied 100,000-element benchmark and are
evidence about those measured operations, not a guarantee for every workload.

## One choice to revisit at 1,000× roster size

I would revisit the party roster's repeated sorted-copy approach. For a much
larger roster, copying and sorting the full vector for every requested view
could become expensive. I would measure how often each view is requested and
consider maintaining a cached sorted view or an index if profiling showed
sorting had become a bottleneck. I would not change containers solely because
the roster is larger; the access pattern and measurements should decide.
