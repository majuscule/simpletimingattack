# Simple Timing Attack

This program implements a simple [timing attack](http://en.wikipedia.org/wiki/Timing_attack). It measures the execution time of a target program over a number of iterations, rotating through a character set and appending the fastest (or slowest) character to the result.

## Requirements:

    librt (with clock_gettime implemented)

Note: OSX [does not currently implement](http://stackoverflow.com/questions/5167269/clock-gettime-alternative-in-mac-os-x) `clock_gettime`

## Compilation:

    gcc -lrt ./timingattack -o timingattack

## Usage:

    Usage: timingattack [options...] <invocation>
    Options:
      --l, --length          specify password length (20 is default)
      --i, --iterations      specify iterations per character (100 is default)
      --c, --charset         specify possible password characters (alphanum is default)
      --p, --prefer-slow     choose the slowest iteration (fastest is default)
      --q, --quiet           only output final result
      --v, --verbose         print timing results after iterations
