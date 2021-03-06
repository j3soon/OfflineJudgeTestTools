# OJTT Sample
## Prerequisites
- Windows (Linux, OS X aren't tested yet).
- `g++` in environment variable `PATH`.
- `ojtt` in environment variable `PATH`.
- `code.cmd` in environment variable `PATH`. (if using `--diff-level 2`)
- `clang++` in environment variable `PATH`. (if using `--input-randomizer`)
## Problem
Add 2 numbers.
### Input
Input pairs of 2 numbers (-10000 <= x <= 10000) until end-of-file.
### Output
Output the addition of the 2 numbers, with newline in the end.
### Sample Input
```
1 2

```
### Sample Output
```
3

```
## Commands (Testing)
### AC (All Accepted)
```shell
ojtt adder.cpp
ojtt -i input.txt read_adder.cpp
ojtt -o output.txt write_adder.cpp
ojtt -i input.txt -o output.txt read_write_adder.cpp
```
### CE (Compile Error)
```shell
ojtt compile_error.cpp
```
### RE (Runtime Error)
```shell
ojtt pointer_misused.cpp
```
### TLE (Time Limit Exceeded)
```shell
ojtt inf_loop.cpp
```
### WA (Wrong Answer)
```shell
ojtt goodbye_cruel_world.cpp
ojtt -i input.txt read_subtractor.cpp
ojtt -o output.txt write_subtractor.cpp
ojtt -i input.txt -o output.txt read_write_subtractor.cpp
ojtt -L 2 goodbye_cruel_world.cpp
```
## Commands (Cross-file testing)
```shell
ojtt adder.cpp -d adder_bug.cpp -r randomizer.cpp
ojtt adder.cpp -d adder_bug.cpp -r randomizer.cpp -L 2
```