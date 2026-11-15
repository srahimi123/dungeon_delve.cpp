# Build Sprint 04 files

This archive contains repository-root paths. Extract it into your repository root and allow the matching files to be replaced:

- `include/dungeon_delve.h`
- `src/dungeon_delve.cpp`
- `src/abilities.cpp` (new)
- `CMakeLists.txt`
- `POLYMORPHISM.md` (new)

`tests/regression.cpp` is included with the same three test assertions. Only the monster setup calls were updated to pass Ability objects instead of the old integer tags.

The runtime `Monster` no longer stores an ability tag. It stores a shared pointer to the abstract `Ability` base class. The regression fixtures were migrated to pass ability factories directly, so the live code has no ability tag constants or integer-based ability dispatch.

From the repository root, run:

```bash
cmake --preset course-debug
cmake --build --preset course-debug
ctest --preset course-debug --output-on-failure
./build/course-debug/dungeon_delve --quiet
```

Do not create the `build-04` tag until those commands pass in your own repository.
