//Written by: Jared Wilson
//COP 3402 - Assignment 1 VM
//Due: 09/26/2021

This program will accept files containing up to 500 characters (spaces not included)
and file names up to 150 characters.

To compile: enter "gcc vm.c" into the terminal

To run: enter "./a.out" into the terminal

The user will be prompted for a file name.
Enter the name of the file wiht the file type appended. ex: "in.txt"
If the file name is not found in the directory,
the user will be prompted to re-enter a file or enter "-1" to exit.

The program will continue and may prompt the user for an integer.
If this occurs, enter a single integer (ex: "3"). 
The program will finish printing and quit when it has reached the end of the 
input file or when the quit ISA is triggered.