# Concept Lab 4 — Kill the Switch

**Name:**  ___________________    **Date:** ____________

---

## Step 0 — The baseline

Fill this in **before** you change anything. If this section is blank, nothing
below it is a measurement.

| | |
|:--|:--|
| `kill_the_switch_tests` final line, before I touched anything | |
| `kill_the_switch_variant_tests` final line, before | |
| Files in `impl/` before | |
| `wc -l` total for `impl/` before | |
| Code lines only (neither blank nor `//`) for `impl/` before | |

---

## Step 1 — Line counts, broken down by file

### Before

| File | Raw lines | Code lines only |
|:--|:--:|:--:|
| `impl/dispatch.cpp` | | |
| **total** | | |

### After

Add a row per file. Delete the spare rows you do not use.

| File | Raw lines | Code lines only |
|:--|:--:|:--:|
| | | |
| | | |
| | | |
| | | |
| | | |
| **total** | | |

**Did the total go up or down, and by how much?**

*My answer:*

If it went up, do not apologise for it. Say what you got in exchange for the
extra lines, in one sentence, and make it a specific thing rather than
"better design".

*My answer:*

---

## Step 2 — Behavior is unchanged. Prove it.

| | |
|:--|:--|
| `kill_the_switch_tests` final line, after the refactor | |
| `diff before.txt after.txt` on the driver output | *(write "empty", or paste what differed)* |
| Did you modify `tests.cpp`, `test_harness.h` or `kill_the_switch.h`? | *(the answer must be no)* |

**Any test that failed on the way, and what caused it.** This is the most
useful row on the page for next week, so write a real answer. "None" is
allowed only if it is true.

*My answer:*

---

## Step 3 — Zero type inspection in the dispatch path

**Where does an ability tag still appear in your code, and what is it doing
there?** List every place. A tag in a lookup is fine; a tag deciding what
happens next is not, and this is where you show which yours are.

*My answer:*

**Confirm, by searching rather than remembering:** no `switch` and no
comparison against an `ABIL_` constant decides behavior anywhere in `impl/`.
Paste what you searched and what it found.

```
(paste here)
```

---

## Step 4 — Add one ability

`ABIL_SHATTER`, specified in `kill_the_switch.h` and tested by
`variant_tests.cpp`.

| | |
|:--|:--|
| `kill_the_switch_variant_tests` final line | |
| `kill_the_switch_tests` still passing afterwards? | |
| Files I **added** | |
| Existing files I **edited** | |

Paste the `diff -rq /tmp/week04_before labs/week04_kill_the_switch` output you
counted from:

```
(paste here)
```

**If the number of existing files you edited is not zero, name each one and
say why.** An honest 2 with a reason scores better than a 0 you did not
measure.

*My answer:*

**For comparison: how many of the five sites in the old `dispatch.cpp` would
Shatter have needed?** Count them from the `SITE 1`–`SITE 5` markers in the
original file.

*My answer:*

---

## Step 5 — Name the thing that got worse. This is the graded artifact.

Something got worse. A submission claiming nothing got worse loses the
analysis marks — not because the claim is rude, but because it means the
comparison was not done.

It has to be a concrete cost in the new design: something that is now harder
to find, harder to read, slower, longer, more scattered, easier to get wrong,
or impossible to see in one place. Not "it took longer to write" and not "it
is less familiar."

*The thing that got worse:*

*Who pays that cost, and when:*

> Examples of the shape of a good answer, so you can see what is being asked
> — do not just pick one of these:
>
> - "Reading one ability's behavior used to be scrolling to a `case` label.
>   Now it is finding which file holds that class."
> - "The old file showed me all eight costs in fourteen lines. Now a cost is
>   a one-line method on each class and there is no single place to read the
>   balance of the game."

---

**And the honest counterweight: what got better, stated as a measurement.**
Point at a row from Step 1 or Step 4, not at an adjective.

*My answer:*

---

**Would you make this change to the engine's own `resolve_ability()` in Build
4?** Yes or no, and one reason drawn from something you measured in this lab.

*My answer:*

---

## What is being graded

| | |
|:--|:--|
| Step 0 filled in before anything changed | required |
| `kill_the_switch_tests` passes, unmodified, after the refactor | required |
| Driver output matches `before.txt` apart from Shatter | required |
| No `switch` or `ABIL_` comparison decides behavior in `impl/` | required |
| `kill_the_switch_variant_tests` passes | required |
| Existing files edited for Shatter, measured with `diff -rq` | required |
| Line counts are per file and came from a command | required |
| A genuine cost of the new design is named | **the graded artifact** — the line counts alone are not |

Submit this file, completed, as directed on the assignment page.
