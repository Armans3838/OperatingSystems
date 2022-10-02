# CPSC380_22_ASSIGNMENT3_SIDDIQUI
Programming Assignment 3\
Arman Siddiqui - 2390583

# Program:
This assignment was a very interseting one in that we asked to created a multithreaded sorting program. For my sorting algorithm of choice, I chose merge sort as it was easiest to implement. This program can be tasked with sorting with as many threads as the user wants however is preset to 2 threads. The program will sort the array and then print the sorted array to the console. The program will also print the time it took to sort the array.\

Something cool about this assignment is exploring how employing more threads allowed for faster sorting which we could see in the console as a printed completion time. This was a very interesting assignment and I enjoyed it a lot.

# Sources/Help:
Online sources such as Geeks for Geeks and StackOverflow were used to help understand and explore how to implement merge sort in addition to how to implement pthread.\

The most useful source was the pthreads tutorial on Geeks for Geeks as it not only illustrated how to approach the assignment but also explained what the underlying code was doing.

# Notes:
To use this program, simply go into thrsort.cpp and change the array size defined as MAX and the array you want to use. You may also follow the comments to create a random array if you choose. Then, compile the program using "g++ -o thrsort thrsort.cpp" and run with "./thrsort". The program will then sort the array and print the sorted array to the console along with the time it took to sort the array. 