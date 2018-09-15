# Offline Judge Test Tools (OJTT)
Simple command line tool for testing code. Can be used as an offline judge (not online judge).

## Screenshots
![](/doc/imgs/screenshot.png)

## Usages
1. Test a single code file.

   Can be used for testing online competition (competitive programming) locally.

   OJTT's workflow is described below:
   - Compile the code to an executable.
   - Execute the executable with test inputs (can be input as file).
   - Retrieve the executable actual outputs (can be output as file).
   - Compare the actual outputs and the expected outputs.
   - Keep looping for every test cases.
   - Report the results.

2. Find out the difference between 2 code files.

   Can be used if your code cannot AC an online judge problem, while you cannot figure out which test cases your code will fail and you have an AC code file (maybe from the internet).

   OJTT's workflow is described below:
   - Compile the 2 codes and the input-randomizer.
   - Execute the input-randomizer executable and get an input.
   - Execute the 2 executables respectively with inputs (can be input as file) and compare their outputs (can be output as file).
   - Keep looping until different outputs are found.
   - Report the results.

## Prerequisites
- Windows Platform

## How to use
- Download executable [here](https://github.com/j3soon/OfflineJudgeTestTools/releases) and add it to environment variable `PATH`.
- Set up default configuration file at `%USERPROFILE%/ojtt/ojtt.cfg`
- Learn how to use basic commands [here](/samples).

## Samples

Some useful [samples](/samples).

## Configuration
1. The options in the command line has the highest priority.
2. The `ojtt.cfg` in the working directory has the second highest priority.
3. The `%USERPROFILE%/ojtt/ojtt-<file_extension>.cfg` in the working directory has the third highest priority.
4. The `%USERPROFILE%/ojtt/ojtt.cfg` in the working directory has the lowest priority.
5. Multiple config files can be used in the same time.

### Command line help
```
Usage:
1. ojtt [options] [file] [input1] [output1] [input2] [output2]...
2. ojtt [options] [file] -d [diff-file] -r [input-randomizer]:

Generic options:
  -? [ --help ]                         show help message
  -v [ --version ]                      show executable version
  --clean                               clean temp files
  --config arg                          path of config file
  -f [ --file ] arg                     file to test
  -i [ --file-input ] arg               input file path that will be read by
                                        code file
  -o [ --file-output ] arg              output file path that will be written
                                        by code file
  -t [ --input-output ] arg             key-value pairs of input test files and
                                        the matching expected output files
  -d [ --diff-file ] arg                the 2nd file to test (diff)
  -r [ --input-randomizer ] arg         file to generate random inputs

Configuration:
  --tmp-dir arg (=C:\Users\Johnson\AppData\Local\Temp\ojtt)
                                        directory for saving temp files
  --eol arg (=<EOL>)                    end-of-line symbol will show up
  --universal-eol arg (=1)              ignore end-of-line differences
  -C [ --compile ] arg                  command to compile the source file
  -E [ --execute ] arg                  command to execute the output file
  -O [ --output-file ] arg              output file
  -C [ --randomizer-compile ] arg       command to compile the randomizer file
                                        (if not set uses 'compile' instead)
  -D [ --diff ] arg                     diff UI's command
  -T [ --time-out ] arg (=15000)        executable max running time in
                                        milliseconds
  -L [ --diff-level ] arg (=1)          0 for don't show difference
                                        1 for show all directly
                                        2 for show by 'diff' command
  -P [ --pause ] arg (=0)               pause when exit
  ```

## Compile Dependencies
- https://www.boost.org/