# outputCompare
Simple file comparator for stos

This app compares expected test result from stos to your program's result. If no differences are found, it prints `OK!`, otherwise it prints the first line number where the 2 files are different.

Syntax:

`ministos.exe [file1] [file2]`, example `ministos.exe tests\12.out myOutputs\12.out`

# Generating output from your project

You can pass tests files to your project's binary and redirect output to file using this syntax:

`[project_filename].exe < [input_test] > [write_output_here]`

Example: `nmksolver.exe < tests\17.in > outputs\17.out`
