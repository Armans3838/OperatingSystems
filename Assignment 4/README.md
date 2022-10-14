# CPSC380_Operating_Systems
Arman Siddiqui

# Sources/Help
For this assignment, I used the following resources:
- Lucus helped me work through the algorithms used for this assignment
- GitHub CoPilot helped me with the syntax for the algorithms and helpful code suggestions as I worked through the assignment
- GeeksforGeeks offered lots of insight into the algorithms used for this assignment through their scheduling algorithms page

# Goal:
The Goal of this assignment was to expand on how the 4 main algorithms for our CPU scheduler work. This includes, First Come First Serve, Priority, Round Robin, and Shortest Job First. The goal of this assignment was to implement these algorithms in a way that would allow for the user to input a file that would be read and then the scheduler would run the designated algorithm(s) on the inputted file.

In addition, the use of a Makefile was required to compile the code and run the program. This was a great expansion into such files and their use.

# How to Run:
Copy the set of files to a directory of your choice. Then, open a terminal and navigate to the directory. Once there, type the following command:

```
make (algorithm)
```

Where (algorithm) is the algorithm you want to run. This can be any of the following:

```
fcfs
priority
rr
sjf
```

Using the options above run the following to run the program:

```
./(algorithm) (input file)
```

Where (algorithm) is the algorithm you want to run and (input file) is the file you want to run the algorithm on. The input file must be in the same directory as the program.