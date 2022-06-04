# Benchmarking two scheduling algorithms

A benchmark of two algorithms for scheduling n tasks, each having a release time, a deadline and a duration. All tasks have the same duration. The complexities of the two algorithms are O(n²) and O(nlogn).

Type `make` to compile and execute the generation of random tests as well as the benchmark. It also creates a plot and shows it.

## Tweaking the tests

The number of tests, max number of tasks to schedule, etc are all defined in the file `src/const.h`
