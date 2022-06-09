# Benchmarking two scheduling algorithms

A benchmark of two algorithms for scheduling n tasks, each having a release time and a deadline. All tasks have the same duration. The complexities of the two algorithms are O(n²) and O(nlogn).

Type `make` to compile and execute the generation of random tests as well as the benchmark. It also creates a plot and shows it.

## The code

When typing `make` for the first time :

- `test_gen.c` is compiled and executed. It fills the `sched_tests` folder with `nmax` test files, each having `Ntests` groups of `n` tasks to schedule, for n in range [1, n]. `nmax` and `Ntests` are defined in `const.h`.

- `benchmark.c` is compiled. The other C files are data structures and algorithms to benchmark, they are compiled as well. It is then executed and measures the average time taken by the different algorithms to schedule `n` tasks for each value of `n`. The results are stored in `results/times.out`.

- the Python script `save_graph.py` is launched and draws a plot from `results/times.out`.

- the resulting graph is shown using Firefox, because I assume most people have Firefox installed on their Linux computer.

## Generating random tests

When generating `n` random tasks to schedule, `test_gen.c` first chooses a release time `r` between 0 and `D * fact * n / 2`, where `D` is the duration of the tasks and `fact` a constant factor, both of which are defined in `const.h`.

Then, it chooses a deadline between `r + D` and `D * fact * n`.

## Tweaking the tests

The number of tests, max number of tasks to schedule, etc are all defined in the file `src/const.h`

## Miscellaneous

Compiling with GCC using the flag `-O3` instead of `-g` gives roughly an executable that is three times as fast.

Type `make format` to format the code using the rules of the `clang-format` file in the `src/` folder.

Type `make count` to count the number of lines of code per file and in total.
